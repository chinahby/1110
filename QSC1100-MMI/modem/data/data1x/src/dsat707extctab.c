/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

               I S - 7 0 7   C O M M A N D   T A B L E S

GENERAL DESCRIPTION
  This module contains the command tables and data definitions required
  to define the extended AT modem commands for the IS-707 mode.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2004 - 2008 by QUALCOMM, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsat707extctab.c_v   1.5   08 Jan 2003 12:27:34   sramacha  $
  $Header: //source/qcom/qct/modem/data/common/1xatcop/main/lite/src/dsat707extctab.c#2 $ $DateTime: 2008/06/25 14:10:04 $ $Author: mganjam $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/24/08   mga     Merged changes to support CTA timer NV item
10/15/02   ak      Updated FEATURE_JCDMA_DS to FEATURE_JCDMA_DS_1X
10/15/02   ak      CMUX list no longer changeable under JCDMA, so make const.
10/13/02   ak      Clean up AT+CRM featurization for JCDMA.
04/03/01   rsl     Initial release.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_DATA
#ifdef FEATURE_DATA_IS707
#include "dsati.h"
#include "dsat707ext.h"
#include "dsat707extctab.h"
#include "dsatparm.h"
#include "msg.h"

/*=========================================================================*/
/* Data declarations for extended commands */
/*=========================================================================*/

dsat_num_item_type dsat707_cdr_val;
dsat_num_item_type dsat707_cds_val[4];
dsat_num_item_type dsat707_cfc_val;
dsat_string_item_type dsat707_cfg_val[249];

#ifdef FEATURE_DS_IS707A_CMUX
/* Look-up table for the numeric value of the fwd mux option. This table  */
/* must contain the same number of items as the "cmux_fwd_valstr" table,  */
/* and the value and order of the items must match exactly.               */
const word dsat707_cmux_fwd_table[15] =
{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 15, 0 };

/* Look-up table for the numeric value of the rev mux option. This table  */
/* must contain the same number of items as the "cmux_rev_valstr" table,  */
/* and the value and order of the items must match exactly.               */
const word dsat707_cmux_rev_table[2] =
{ 1, 2 };

LOCAL const dsat_string_item_type dsat707_cmux_fwd_valstr [][8] =
{ "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "F", 0 };

LOCAL const dsat_string_item_type dsat707_cmux_fwd_tststr [] =
{ "(1, 2, 3, 4, 5, 6, 7, 8, 9, A, B, C, D, F, 0)" };

LOCAL const dsat_string_item_type dsat707_cmux_rev_valstr [][8] =
{ "1", "2" };

LOCAL const dsat_string_item_type dsat707_cmux_rev_tststr [] =
{ "(1, 2)" };

/* The default for at+cmux is set to C,2 here (C,2 corresponds to fwd &    */
/* rev valstr indexes of 11 & 1 respectively.                              */
LOCAL const def_list_type dsat707_cmux_list[] =
{
  { 11, dsat707_cmux_fwd_valstr, dsat707_cmux_fwd_tststr },
  { 1, dsat707_cmux_rev_valstr, dsat707_cmux_rev_tststr }
};

/* The values stored in the "cmux_val" array are the indices to the fwd &  */
/* rev mux options in the "valstr" tables (which contain strings). Use the */
/* "cmux_fwd_table" & "cmux_rev_table" to lookup the actual numeric value. */
dsat_num_item_type dsat707_cmux_val[2];

#else  /* FEATURE_DS_IS707A_CMUX */

LOCAL const dflm_type dsat707_cmux_dflm [] =
{
  { 2  , 1  , 2    },
} ;
dsat_num_item_type dsat707_cmux_val;

#endif  /* FEATURE_DS_IS707A_CMUX */

dsat_num_item_type dsat707_cqd_val;
dsat_num_item_type dsat707_crc_val;
dsat_num_item_type dsat707_crm_val;
dsat_num_item_type dsat707_cta_val;
dsat_num_item_type dsat707_cxt_val;
dsat_num_item_type dsat707_eb_val[3];
dsat_num_item_type dsat707_efcs_val;
dsat_num_item_type dsat707_er_val;
dsat_num_item_type dsat707_es_val[3];
dsat_num_item_type dsat707_esr_val;
dsat_num_item_type dsat707_etbm_val[3];
dsat_num_item_type dsat707_ilrr_val;
dsat_string_item_type dsat707_ma_val[249];
dsat_string_item_type dsat707_ms_val[249];
dsat_num_item_type dsat707_mr_val;
dsat_num_item_type dsat707_mv18r_val;
dsat_num_item_type dsat707_mv18s_val[3];

/*=========================================================================*/
/* Defaults & Limits for extended commands */
/*=========================================================================*/

LOCAL const dflm_type dsat707_cdr_dflm [] =
{
  { 0  , 0  , 1    },
} ;

LOCAL const dflm_type dsat707_cds_dflm [] =
{
  { 0  , 0  , 0    },
  { 1  , 1  , 1    },
  { 2048, 512, 65535 },
  { 6  , 6  , 250  },
} ;

LOCAL const dflm_type dsat707_cfc_dflm [] =
{
  { 0  , 0  , 0    },
} ;

LOCAL const dflm_type dsat707_cqd_dflm [] =
{
  { 10 , 0  , 255  },
} ;

LOCAL const dflm_type dsat707_crc_dflm [] =
{
  { 0  , 0  , 1    },
} ;

LOCAL const dflm_type dsat707_crm_dflm [] =
{
#ifdef FEATURE_DS_NET_MODEL
  #ifdef FEATURE_JCDMA_1X
  { 2  , 2  , 2    },
  #else
  { 0  , 0  , 2    },
  #endif /* FEATURE_JCDMA_1X */
#else
  { 0  , 0  , 1    },
#endif /* FEATURE_DS_NET_MODEL */
} ;

dflm_type dsat707_cta_dflm [] =
{
  { DS707_CTA_DEFAULT_VALUE , DS707_CTA_LOWER_LIMIT , DS707_CTA_UPPER_LIMIT },
} ;

LOCAL const dflm_type dsat707_cxt_dflm [] =
{
  { 0  , 0  , 1    },
} ;

LOCAL const dflm_type dsat707_eb_dflm [] =
{
  { 1  , 0  , 3    },
  { 0  , 0  , 1    },
  { 30 , 0  , 254  },
} ;

LOCAL const dflm_type dsat707_efcs_dflm [] =
{
  { 1  , 0  , 2    },
} ;

LOCAL const dflm_type dsat707_er_dflm [] =
{
  { 0  , 0  , 1    },
} ;

LOCAL const dflm_type dsat707_es_dflm [] =
{
  { 3  , 0  , 4    },
  { 0  , 0  , 4    },
  { 2  , 0  , 6    },
} ;


LOCAL const dflm_type dsat707_esr_dflm [] =
{
  { 1  , 0  , 2    },
} ;


LOCAL const dflm_type dsat707_etbm_dflm [] =
{
  { 1  , 0  , 2    },
  { 1  , 0  , 2    },
  { 20 , 0  , 30   },
} ;

LOCAL const dflm_type dsat707_ilrr_dflm [] =
{
  { 0  , 0  , 0    },
} ;

LOCAL const dflm_type dsat707_mr_dflm [] =
{
  { 0  , 0  , 1    },
} ;

LOCAL const dflm_type dsat707_mv18r_dflm [] =
{
  { 0  , 0  , 1    },
} ;

LOCAL const dflm_type dsat707_mv18s_dflm [] =
{
  { 0  , 0  , 7    },
  { 0  , 0  , 4    },
  { 0  , 0  , 1    },
} ;
/*=========================================================================*/
/* General IS-707 extended format AT parameter command table */
/*=========================================================================*/

const dsati_cmd_type dsat707_ext_para_table [] =
{

/*-------------------------------------------------------------------------*/
  { "+CAD",
      EXTENDED | READ_ONLY,
      SPECIAL_CAD,
      0,
      NULL,
      NULL,
      dsat707_exec_cad_cmd },
/*-------------------------------------------------------------------------*/
  { "+CBC",
      EXTENDED | READ_ONLY,
      SPECIAL_CBC,
      0,
      NULL,
      NULL,
      dsat707_exec_cbc_cmd },
/*-------------------------------------------------------------------------*/
  { "+CBIP",
      EXTENDED | READ_ONLY,
      SPECIAL_CBIP,
      0,
      NULL,
      NULL,
      dsat707_exec_cbip_cmd },
/*-------------------------------------------------------------------------*/
  { "+CDR",
      EXTENDED | CONFIG | LOCAL_TEST,
      SPECIAL_NONE,
      1,
      &dsat707_cdr_val,
      &dsat707_cdr_dflm[0],
      dsat707_exec_cdr_cmd },
/*-------------------------------------------------------------------------*/
  { "+CDS",
      EXTENDED | CONFIG | LOCAL_TEST,
      SPECIAL_NONE,
      4,
      &dsat707_cds_val[0],
      &dsat707_cds_dflm[0],
      dsat707_exec_cds_cmd },
/*-------------------------------------------------------------------------*/
  { "+CFC",
      EXTENDED | CONFIG | LOCAL_TEST,
      SPECIAL_NONE,
      1,
      &dsat707_cfc_val,
      &dsat707_cfc_dflm[0],
      dsat707_exec_cfc_cmd },
/*-------------------------------------------------------------------------*/
  { "+CFG",
      EXTENDED | CONFIG | STRING,
      SPECIAL_NONE,
      248,
      &dsat707_cfg_val[0],
      NULL,
      dsat707_exec_cfg_cmd },
/*-------------------------------------------------------------------------*/
  { "+CMIP",
      EXTENDED | READ_ONLY,
      SPECIAL_CMIP,
      0,
      NULL,
      NULL,
      dsat707_exec_cmip_cmd },
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_DS_IS707A_CMUX
  { "+CMUX",
      EXTENDED | CONFIG | LOCAL_TEST | LIST,
      SPECIAL_CMUX,
      2,
      &dsat707_cmux_val[0],
      &dsat707_cmux_list[0],
      dsat707_exec_cmux_cmd },
#else
  { "+CMUX",
      EXTENDED | CONFIG | LOCAL_TEST,
      SPECIAL_CMUX,
      1,
      &dsat707_cmux_val,
      &dsat707_cmux_dflm[0],
      dsat707_exec_cmux_cmd },
#endif  /* FEATURE_DS_IS707A_CMUX */
/*-------------------------------------------------------------------------*/
  { "+CQD",
      EXTENDED | CONFIG | DOWN_LOAD,
      SPECIAL_NONE,
      1,
      &dsat707_cqd_val,
      &dsat707_cqd_dflm[0],
      dsat707_exec_cqd_cmd },
/*-------------------------------------------------------------------------*/
  { "+CRC",
      EXTENDED | CONFIG | DOWN_LOAD,
      SPECIAL_NONE,
      1,
      &dsat707_crc_val,
      &dsat707_crc_dflm[0],
      dsat707_exec_crc_cmd },
/*-------------------------------------------------------------------------*/
  { "+CRM",
      EXTENDED | CONFIG | LOCAL_TEST,
      SPECIAL_NONE,
      1,
      &dsat707_crm_val,
      &dsat707_crm_dflm[0],
      dsat707_exec_crm_cmd },
/*-------------------------------------------------------------------------*/
  { "+CSQ",
      EXTENDED | READ_ONLY,
      SPECIAL_CSQ,
      0,
      NULL,
      NULL,
      dsat707_exec_csq_cmd },
/*-------------------------------------------------------------------------*/
  { "+CSS",
      EXTENDED | READ_ONLY,
      SPECIAL_CSS,
      0,
      NULL,
      NULL,
      dsat707_exec_css_cmd },
/*-------------------------------------------------------------------------*/
  { "+CTA",
      EXTENDED | CONFIG | LOCAL_TEST,
      SPECIAL_NONE,
      1,
      &dsat707_cta_val,
      &dsat707_cta_dflm[0],
      dsat707_exec_cta_cmd },
/*-------------------------------------------------------------------------*/
  { "+CXT",
      EXTENDED | CONFIG | LOCAL_TEST,
      SPECIAL_NONE,
      1,
      &dsat707_cxt_val,
      &dsat707_cxt_dflm[0],
      dsat707_exec_cxt_cmd },
/*-------------------------------------------------------------------------*/
  { "+EB",
      EXTENDED | CONFIG | DOWN_LOAD,
      SPECIAL_NONE,
      3,
      &dsat707_eb_val[0],
      &dsat707_eb_dflm[0],
      dsat707_exec_eb_cmd },
/*-------------------------------------------------------------------------*/
  { "+EFCS",
      EXTENDED | CONFIG | DOWN_LOAD,
      SPECIAL_NONE,
      1,
      &dsat707_efcs_val,
      &dsat707_efcs_dflm[0],
      dsat707_exec_efcs_cmd },
/*-------------------------------------------------------------------------*/
  { "+ER",
      EXTENDED | CONFIG | DOWN_LOAD,
      SPECIAL_NONE,
      1,
      &dsat707_er_val,
      &dsat707_er_dflm[0],
      dsat707_exec_er_cmd },
/*-------------------------------------------------------------------------*/
  { "+ES",
      EXTENDED | CONFIG | DOWN_LOAD,
      SPECIAL_NONE,
      3,
      &dsat707_es_val[0],
      &dsat707_es_dflm[0],
      dsat707_exec_es_cmd },
/*-------------------------------------------------------------------------*/
  { "+ESR",
      EXTENDED | CONFIG | DOWN_LOAD,
      SPECIAL_NONE,
      1,
      &dsat707_esr_val,
      &dsat707_esr_dflm[0],
      dsat707_exec_esr_cmd },
/*-------------------------------------------------------------------------*/
  { "+ETBM",
      EXTENDED | CONFIG | DOWN_LOAD,
      SPECIAL_NONE,
      3,
      &dsat707_etbm_val[0],
      &dsat707_etbm_dflm[0],
      dsat707_exec_etbm_cmd },
/*-------------------------------------------------------------------------*/
  { "+GOI",
      EXTENDED | READ_ONLY,
      SPECIAL_GOI,
      0,
      NULL,
      NULL,
      dsat707_exec_goi_cmd },
/*-------------------------------------------------------------------------*/
  { "+ILRR",
      EXTENDED | CONFIG | LOCAL_TEST,
      SPECIAL_NONE,
      1,
      &dsat707_ilrr_val,
      &dsat707_ilrr_dflm[0],
      dsat707_exec_ilrr_cmd },
/*-------------------------------------------------------------------------*/
  { "+MA",
      EXTENDED | CONFIG | STRING | NO_QUOTE | DOWN_LOAD,
      SPECIAL_NONE,
      248,
      &dsat707_ma_val[0],
      NULL,
      dsat707_exec_ma_cmd },
/*-------------------------------------------------------------------------*/
  { "+MR",
      EXTENDED | CONFIG | DOWN_LOAD,
      SPECIAL_NONE,
      1,
      &dsat707_mr_val,
      &dsat707_mr_dflm[0],
      dsat707_exec_mr_cmd },
/*-------------------------------------------------------------------------*/
  { "+MS",
      EXTENDED | CONFIG | STRING | NO_QUOTE | DOWN_LOAD,
      SPECIAL_NONE,
      248,
      &dsat707_ms_val[0],
      NULL,
      dsat707_exec_ms_cmd },
/*-------------------------------------------------------------------------*/
  { "+MV18R",
      EXTENDED | CONFIG | DOWN_LOAD,
      SPECIAL_NONE,
      1,
      &dsat707_mv18r_val,
      &dsat707_mv18r_dflm[0],
      dsat707_exec_mv18r_cmd },
/*-------------------------------------------------------------------------*/
  { "+MV18S",
      EXTENDED | CONFIG | DOWN_LOAD,
      SPECIAL_NONE,
      3,
      &dsat707_mv18s_val[0],
      &dsat707_mv18s_dflm[0],
      dsat707_exec_mv18s_cmd }
/*-------------------------------------------------------------------------*/
};

/*=========================================================================*/
/* General IS-707 extended format AT action command table */
/*=========================================================================*/
const dsati_cmd_type dsat707_ext_action_table [] =
{
  { "+CDV",
	EXTENDED,
	SPECIAL_NONE,
	0,
	NULL,
	NULL,
	dsat707_exec_cdv_cmd },
  /*-------------------------------------------------------------------------*/
  { "+CHV",
      EXTENDED,
      SPECIAL_CHV,
      0,
      NULL,
      NULL,
      dsat707_exec_chv_cmd },
/*-------------------------------------------------------------------------*/
  { "+CHV0",
      EXTENDED,
      SPECIAL_CHV,
      0,
      NULL,
      NULL,
      dsat707_exec_chv0_cmd },
/*-------------------------------------------------------------------------*/
};

/* Size of command table */
const unsigned int dsat707_ext_para_table_size = ARR_SIZE( dsat707_ext_para_table );
const unsigned int dsat707_ext_action_table_size = ARR_SIZE( dsat707_ext_action_table );

#endif /* FEATURE_DATA_IS707 */
#endif /* FEATURE_DATA */
