/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

               I S - 7 0 7   C O M M A N D   T A B L E S

GENERAL DESCRIPTION
  This module contains the command tables and data definitions required
  to define the SMS related AT modem commands for the IS-707 mode.

  The AT commands here are not specified in 707 spec.  They are vendor
  specific commands for automating sms testing.
  

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

   Copyright (c) 2009 by QUALCOMM Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //source/qcom/qct/modem/data/common/1xatcop/main/lite/src/dsat707smsctab.c#1 $ $DateTime: 2009/01/23 10:36:19 $ $Author: nchagant $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/21/09   nc      Added support for SMS Commands. 

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_DATA
#ifdef FEATURE_CDMA_SMS
#include "dsati.h"
#include "dsat707sms.h"
#include "dsat707smsctab.h"
#include "dsatparm.h"
#include "msg.h"


/*=========================================================================*/
/* Defaults & Limits for SMS commands */
/*=========================================================================*/

/* Teleservice Id */
LOCAL const dsat_string_item_type       qcsmp_tid_valstr [][8] =
{ "4095", "4096", "4097", "4098", "4099", "4100", "4101", "4102", ""};

LOCAL const dsat_string_item_type       qcsmp_tid_teststr[] =
{ "(\"4095\",\"4096\",\"4097\",\"4098\",\"4099\",\"4100\",\"4101\",\"4102\")" };

/* Teleservice Id uses 4099 (VMN 95) as default */
LOCAL const def_list_type               qcsmp_tid_list =
{ 3, qcsmp_tid_valstr, qcsmp_tid_teststr };

LOCAL const mixed_def_s_type            qcsmp_tid_def_val = 
{
  CONFIG | LIST | LOCAL_TEST,
  (mixed_def_u_type *) &qcsmp_tid_list
};

/* Valid Period Format, 0 Absolute, 1 Relative 
 * See EIA 637 Table 4.5.5 for Absolute Time Format
 * See EIA 637 Table 4.5.6-1 for Relative Time Format
 */
LOCAL const dflm_type                   qcsmp_vpf_dflm =
{ 0, 0, 1 };

LOCAL const mixed_def_s_type            qcsmp_vpf_def_val =
{
  CONFIG | LOCAL_TEST,
  (mixed_def_u_type *) &qcsmp_vpf_dflm
};

#define QCSMP_VP_STR_MAX_LEN 22
LOCAL const word                        qcsmp_vp_len =
                                          QCSMP_VP_STR_MAX_LEN;
LOCAL const mixed_def_s_type            qcsmp_vp_def_val =
{
  CONFIG | STRING,
  (mixed_def_u_type *) &qcsmp_vp_len
};

/* Deferred Delivery Time Format, 0 Absolute, 1 Relative */
LOCAL const dflm_type                   qcsmp_ddtf_dflm =
{ 0, 0, 1 };

LOCAL const mixed_def_s_type            qcsmp_ddtf_def_val =
{
  CONFIG | LOCAL_TEST,
  (mixed_def_u_type *) &qcsmp_ddtf_dflm
};

#define QCSMP_DDT_STR_MAX_LEN 22
LOCAL const uint16                      qcsmp_ddt_len = QCSMP_DDT_STR_MAX_LEN;
LOCAL const mixed_def_s_type            qcsmp_ddt_def_val =
{
  CONFIG | STRING,
  (mixed_def_u_type *) &qcsmp_ddt_len
};

LOCAL const mixed_def_s_type * qcsmp_list[] =
{
  &qcsmp_tid_def_val, &qcsmp_vpf_def_val, &qcsmp_vp_def_val, 
  &qcsmp_ddtf_def_val, &qcsmp_ddt_def_val
};

LOCAL const dsat_string_item_type qcpms_mem1_valstr[][8] =
/* Don't include SM device option if RUIM is not present */
#ifdef FEATURE_RUIM
{ "ME", "MT", "SM", "" };
#else
{ "ME", "MT", "" };
#endif /* FEATURE_RUIM */


LOCAL const dsat_string_item_type qcpms_mem1_tststr[] =
/* Don't include SM device option if RUIM is not present */
#ifdef FEATURE_RUIM
{ "(\"ME\",\"MT\",\"SM\")" };
#else
{ "(\"ME\",\"MT\")" };
#endif /* FEATURE_RUIM */


LOCAL const dsat_string_item_type qcpms_mem2_valstr[][8] =
/* Don't include SM device option if RUIM is not present */
#ifdef FEATURE_RUIM
{ "ME", "MT", "SM", "" };
#else
{ "ME", "MT", "" };
#endif /* FEATURE_RUIM */

LOCAL const dsat_string_item_type qcpms_mem2_tststr[] =
/* Don't include SM device option if RUIM is not present */
#ifdef FEATURE_RUIM
{ "(\"ME\",\"MT\",\"SM\")" };
#else
{ "(\"ME\",\"MT\")" };
#endif /* FEATURE_RUIM */


LOCAL const dsat_string_item_type qcpms_mem3_valstr[][8] =
/* Don't include SM device option if RUIM is not present */
#ifdef FEATURE_RUIM
{ "ME", "MT", "SM", "" };
#else
{ "ME", "MT", "" };
#endif /* FEATURE_RUIM */

LOCAL const dsat_string_item_type qcpms_mem3_tststr[] =
/* Don't include SM device option if RUIM is not present */
#ifdef FEATURE_RUIM
{ "(\"ME\",\"MT\",\"SM\")" };
#else
{ "(\"ME\",\"MT\")" };
#endif /* FEATURE_RUIM */

const def_list_type dsat707sms_qcpms_list[] =
{
  { 0, qcpms_mem1_valstr, qcpms_mem1_tststr},
  { 0, qcpms_mem2_valstr, qcpms_mem2_tststr},
  { 0, qcpms_mem3_valstr, qcpms_mem3_tststr}
};

/* $QCNMI Command */
LOCAL const dsat_string_item_type dsat_qcnmi_mode_valstr [][8] =
{
  "0", "1", "2", ""
};

LOCAL const dsat_string_item_type dsat_qcnmi_mode_tststr [] =
{ 
  "(0,1,2)" 
};

/* NOTE: Enable this only when WMS releases all the needed fixes  */
LOCAL const dsat_string_item_type dsat_qcnmi_mt_valstr [][8] =
{ "0", "1", "2", "3", "" };

LOCAL const dsat_string_item_type dsat_qcnmi_mt_tststr [] =
{ "(0,1,2,3)" };

LOCAL const dsat_string_item_type dsat_qcnmi_bfr_valstr [][8] =
{ "0", "1", ""};

LOCAL const dsat_string_item_type dsat_qcnmi_bfr_tststr [] =
{ "(0,1)" };

const def_list_type dsat707_qcnmi_list[] =
{
  { 0, dsat_qcnmi_mode_valstr, dsat_qcnmi_mode_tststr },
  { 0, dsat_qcnmi_mt_valstr, dsat_qcnmi_mt_tststr },
  { 0, dsat_qcnmi_bfr_valstr, dsat_qcnmi_bfr_tststr }
} ;

const dflm_type dsat707sms_qcmgf_dflm [] =
{
  { 1  , 1  , 1  },
};

/*=========================================================================*/
/* Data declarations for sms commands */
/*=========================================================================*/

LOCAL dsat_string_item_type     qcsmp_vp_val[QCSMP_VP_STR_MAX_LEN+1];
LOCAL dsat_string_item_type     qcsmp_tdd_val[QCSMP_DDT_STR_MAX_LEN+1];

dsat_mixed_param_val_type dsat707sms_qcsmp_val[] =
{ 0, 0, qcsmp_vp_val, 0, qcsmp_tdd_val };

dsat_num_item_type  dsat707sms_qcpms_val[3];
dsat_num_item_type  dsat707sms_qcnmi_val[3];

dsat_num_item_type dsat707sms_qcmgf_val;

/*=========================================================================*/
/* IS-707 Sms related AT command table */
/*=========================================================================*/
const dsati_cmd_type dsat707_sms_table [] =
{
  { "$QCNMI",
    EXTENDED | CONFIG | LOCAL_TEST | LIST | LOCAL_TEST,
    SPECIAL_NONE,
    3,
    &dsat707sms_qcnmi_val[0], 
    &dsat707_qcnmi_list[0],
    dsat707sms_exec_qcnmi_cmd,
    NULL
  },

  { "$QCSMP",
    EXTENDED | CONFIG | MIXED_PARAM | LIST | LOCAL_TEST,
    SPECIAL_NONE,
    5,
    &dsat707sms_qcsmp_val[0], 
    &qcsmp_list[0],
    dsat707sms_exec_qcsmp_cmd,
    NULL
  },

  { "$QCPMS",
    EXTENDED | CONFIG | LIST | YES_QUOTE | LOCAL_TEST,
    SPECIAL_NONE,
    3,
    &dsat707sms_qcpms_val, 
    &dsat707sms_qcpms_list,
    dsat707sms_exec_qcpms_cmd,
    NULL
  },

  { "$QCMGR",
    EXTENDED,
    SPECIAL_NONE,
    1,
    NULL,
    NULL,
    dsat707sms_exec_qcmgr_cmd,
    NULL
  },

  { "$QCMGS",
    EXTENDED,
    SPECIAL_NONE,
    2,
    NULL,
    NULL,
    dsat707sms_exec_qcmgs_cmd,
    NULL
  },

  { "$QCMSS",
     EXTENDED,
     SPECIAL_NONE,
     3,
     NULL,
     NULL,
     dsat707sms_exec_qcmss_cmd,
     NULL 
  },

  { "$QCMGD",
    EXTENDED,
    SPECIAL_NONE,
    2,
    NULL,
    NULL,
    dsat707sms_exec_qcmgd_cmd,
    NULL
  },

  { "$QCMGL",
    EXTENDED,
    SPECIAL_NONE,
    1,
    NULL,
    NULL,
    dsat707sms_exec_qcmgl_cmd,
    NULL
  },

  { "$QCMGF",
    EXTENDED | CONFIG | LOCAL_TEST,
    SPECIAL_NONE,
    1,
    &dsat707sms_qcmgf_val,
    &dsat707sms_qcmgf_dflm[0],
    dsatparm_exec_param_cmd,
    NULL 
  },

  { "$QCMGW",
    EXTENDED ,
    SPECIAL_NONE,
    3,
    NULL,
    NULL,
    dsat707sms_exec_qcmgw_cmd,
    NULL 
  }

};

/* Size of SMS command table */
const unsigned int dsat707_sms_table_size = ARR_SIZE( dsat707_sms_table );


#endif /* FEATURE_DS_IS707 */
#endif /* FEATURE_DATA */
