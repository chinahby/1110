/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

               I S - 7 0 7   C O M M A N D   T A B L E S

GENERAL DESCRIPTION
  This module contains the command tables and data definitions required
  to define the vendor specific AT modem commands for the IS-707 mode.

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

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsat707vendctab.c_v   1.7   08 Jan 2003 12:27:58   sramacha  $
  $Header: //source/qcom/qct/modem/data/common/1xatcop/main/lite/src/dsat707vendctab.c#6 $ $DateTime: 2009/01/22 06:46:48 $ $Author: bhaviks $
  
when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/02/09    bs     Added support for 1X AT Phonebook commands.
08/29/08    bs     Cleaned up featurization for $qcmdr.
02/07/08    sn     Removed FEATURE_SPECIAL_MDR.
11/21/02    rsl    Fixed merge issues with at$qcchv.
11/14/02    atp    Fixed minor compilation errors.
10/15/02    ak     Added support for AT$QCCHS, which is for control/hold
10/15/02    ak     Updated FEATURE_JCDMA_DS to FEATURE_JCDMA_DS_1X
09/24/02    atp    Added support for QOS. (1x Release-A feature).
04/03/01    rsl    Initial release.

===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "dsati.h"
#include "ds707.h"
#include "dsat707vend.h"
#include "dsat707vendctab.h"
#include "dsatctab.h"
#include "dsatme.h"
#include "dsatparm.h"
#include "dstaski.h"
#include "err.h"
#include "msg.h"
#include "nv.h"

/*=========================================================================*/
/* Data declarations for Vendor Specific commands */
/*=========================================================================*/

#ifdef FEATURE_DS_SOCKETS_TEST_UDP
#error code not present
#endif /* FEATURE_DS_SOCKETS_TEST_UDP */

#ifdef FEATURE_DS_SOCKETS
/*---------------------------------------------------------------------------
  The arrays dsat707_qcdnspri_val and dsat707_qcdnssec_val are used to
  store the incoming strings. Since the incoming string should contain a
  " to start and end the string and "/0", the length 18 was chosen.
  Example, "129.046.050.007"
---------------------------------------------------------------------------*/
dsat_string_item_type dsat707_qcdnspri_val[18];
dsat_string_item_type dsat707_qcdnssec_val[18];

/*---------------------------------------------------------------------------
  The array dsat707_qcdns_val stores the HEX value obtained after
  concatenating the incoming string.
  dsat707_qcdns_val[0] is for Primary DNS IP address
  dsat707_qcdns_val[1] is for Secondary DNS IP address
  Since we store Primary and Secondary DNS IP address at two separate
  locations we need an array. The "resolver" function, accesses the IP
  addresses through an index to this array.
---------------------------------------------------------------------------*/
uint32 dsat707_qcdns_val[2];

#endif /* FEATURE_DS_SOCKETS */

dsat_num_item_type dsat707_qcmdr_val;

#ifdef FEATURE_IS2000_R_SCH
/* TURN SCRM on/off */
dsat_num_item_type dsat707_qcscrm_val;
/* TURN R-SCH throttle on/off */
dsat_num_item_type dsat707_qctrtl_val;
#endif /* FEATURE_IS2000_R_SCH */

dsat_string_item_type dsat707_qcotc_val[241];

/*  Numeric Config items  */

#ifdef FEATURE_IS2000_CHS
/* Enable/disable Mobile-originated Control/Hold */
dsat_num_item_type dsat707_qcchs_val;
#endif /* FEATURE_IS2000_CHS */

#ifdef  FEATURE_DS_QNC
/* Quick Net Connect on/off */
dsat_num_item_type dsat707_qcqnc_val;
#endif

/* Which s.o. set */
dsat_num_item_type dsat707_qcso_val;

dsat_num_item_type dsat707_qcvad_val;

#ifdef FEATURE_IS2000_REL_A
dsat_num_item_type dsat707_qcqospri_val;
#endif /* FEATURE_IS2000_REL_A */


/*=========================================================================*/
/* Defaults & Limits for HDR commands */
/*=========================================================================*/

dflm_type dsat707_qcmdr_dflm [] =
{
  { DS_MDR_MODE_DEFAULT, 0, (DS_MDR_MODE_MAX - 1) },
};

#ifdef FEATURE_IS2000_R_SCH
dflm_type dsat707_qcscrm_dflm [] =
{
  { DS_SCRM_DEFAULT, 0, 1 },
};

dflm_type dsat707_qctrtl_dflm [] =
{
  { DS_TRTL_DEFAULT, 0, 1 },
};
#endif /* FEATURE_IS2000_R_SCH */

#ifdef FEATURE_IS2000_CHS
dflm_type dsat707_qcchs_dflm [] =
{
  { DS_CHS_DEFAULT, 0  , 255    },
} ;
#endif /* FEATURE_IS2000_CHS */

#ifdef FEATURE_DS_QNC

dflm_type dsat707_qcqnc_dflm [] =
{
  { DS_QNC_DEFAULT  , 0  , 1    },
} ;

#endif /* FEATURE_DS_QNC */

#ifdef FEATURE_DS_CHOOSE_SO

dflm_type dsat707_qcso_dflm [] =
{
  { DS_SO_SET_DEFAULT  , 0 , DS_SO_SET_COUNT-1  },
} ;

#endif /* FEATURE_DS_CHOOSE_SO */

#ifdef FEATURE_IS2000_REL_A
dflm_type dsat707_qcqospri_dflm [] =
{
  { DS707_QOS_DESIRED_PRI_DEFAULT, 0, DS707_QOS_DESIRED_PRI_MAX  },
};
#endif /* FEATURE_IS2000_REL_A */

LOCAL const dflm_type dsat707_qcvad_dflm [] =
{
  { 0  , 0  , 4    },
} ;

/*=========================================================================*/
/* IS-707 Vendor Specific AT parameter command table */
/*=========================================================================*/
const dsati_cmd_type dsat707_vend_para_table [] =
{
/*-------------------------------------------------------------------------*/
 { "$QCPREV",
    EXTENDED  | READ_ONLY,
    SPECIAL_QCPREV,
    0,
    NULL,
    NULL,
    dsat707_exec_qcprev_cmd}
#ifdef FEATURE_DSAT_CDMA_PBM
/*-------------------------------------------------------------------------*/
   ,{ "$QCCPBS",
    EXTENDED | CONFIG | LIST | YES_QUOTE | LOCAL_TEST,
    SPECIAL_NONE,
    1,
    &dsat_cpbs_val,
    &dsat_cpbs_list,
    dsatme_exec_cpbs_cmd }
#endif /* FEATURE_DSAT_CDMA_PBM */
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_DS_AT_TEST_ONLY

 ,{ "$QCMDR",
    EXTENDED | CONFIG | LOCAL_TEST,
    SPECIAL_QCMDR,
    1,
    &dsat707_qcmdr_val,
    &dsat707_qcmdr_dflm[0],
    dsat707_exec_qcmdr_cmd }

#endif /* FEATURE_DS_AT_TEST_ONLY */
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_IS2000_R_SCH
#ifdef FEATURE_DS_AT_TEST_ONLY

 ,{ "$QCSCRM",
    EXTENDED | CONFIG | LOCAL_TEST,
    SPECIAL_QCSCRM,
    1,
    &dsat707_qcscrm_val,
    &dsat707_qcscrm_dflm[0],
    dsat707_exec_qcscrm_cmd }
/*-------------------------------------------------------------------------*/
 ,{ "$QCTRTL",
    EXTENDED | CONFIG | LOCAL_TEST,
    SPECIAL_QCTRTL,
    1,
    &dsat707_qctrtl_val,
    &dsat707_qctrtl_dflm[0],
    dsat707_exec_qctrtl_cmd }
#endif /* FEATURE_DS_AT_TEST_ONLY */
#endif /* FEATURE_IS2000_R_SCH */
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_DS_QNC

 ,{ "$QCQNC",
    EXTENDED | CONFIG | LOCAL_TEST,
    SPECIAL_QCQNC,
    1,
    &dsat707_qcqnc_val,
    &dsat707_qcqnc_dflm[0] ,
    dsat707_exec_qcqnc_cmd}

#endif /* FEATURE_DS_QNC */
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_DS_CHOOSE_SO
#ifdef FEATURE_DS_AT_TEST_ONLY
 ,{ "$QCSO",
    EXTENDED | CONFIG | LOCAL_TEST,
    SPECIAL_QCSO,
    1,
    &dsat707_qcso_val,
    &dsat707_qcso_dflm[0],
    dsat707_exec_qcso_cmd }
#endif /* FEATURE_DS_AT_TEST_ONLY */
#endif /* FEATURE_DS_CHOOSE_SO */
/*-------------------------------------------------------------------------*/
 ,{ "$QCVAD",
    EXTENDED | CONFIG | LOCAL_TEST,
    SPECIAL_QCVAD,
    1,
    &dsat707_qcvad_val,
    &dsat707_qcvad_dflm[0] ,
    dsat707_exec_qcvad_cmd}
/*-------------------------------------------------------------------------*/
 ,{ "$QCCAV",
    EXTENDED,
    SPECIAL_QCCAV,
    0,
    NULL,
    NULL,
    dsat707_exec_qccav_cmd }
/*-------------------------------------------------------------------------*/
 ,{ "$QCCHV",
    EXTENDED,
    SPECIAL_QCCHV,
    0,
    NULL,
    NULL,
    dsat707_exec_qcchv_cmd }
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_IS2000_REL_A
 ,{ "$QCQOSPRI",
    EXTENDED | CONFIG | LOCAL_TEST,
    SPECIAL_QCQOSPRI,
    1,
    &dsat707_qcqospri_val,
    &dsat707_qcqospri_dflm[0],
    dsat707_exec_qcqospri_cmd }
#endif /* FEATURE_IS2000_REL_A */
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_IS2000_CHS

 ,{ "$QCCHS",
    EXTENDED | CONFIG | LOCAL_TEST,
    SPECIAL_QCCHS,
    1,
    &dsat707_qcchs_val,
    &dsat707_qcchs_dflm[0] ,
    dsat707_exec_qcchs_cmd }

#endif /* FEATURE_IS2000_CHS */
/*-------------------------------------------------------------------------*/
};

/*=========================================================================*/
/* IS-707 Vendor Specific AT parameter command table */
/*=========================================================================*/
const dsati_cmd_type dsat707_vend_action_table [] =
{
#ifdef FEATURE_DSAT_CDMA_PBM
  { "$QCCPBR",
    ATTRIB_NONE,
    SPECIAL_NONE,
    2,
    NULL,
    NULL,
    dsatme_exec_cpbr_cmd },
/*-------------------------------------------------------------------------*/
  { "$QCCPBF",
    ATTRIB_NONE,
    SPECIAL_NONE,
    1,
    NULL,
    NULL,
    dsatme_exec_cpbf_cmd },
/*-------------------------------------------------------------------------*/
  { "$QCCPBW",
    ATTRIB_NONE,
    SPECIAL_NONE,
    5,
    NULL,
    NULL,
    dsatme_exec_cpbw_cmd },
#endif /* FEATURE_DSAT_CDMA_PBM */
/*-------------------------------------------------------------------------*/
  { "$QCOTC",
    EXTENDED | CONFIG | STRING | LOCAL_TEST,
    SPECIAL_QCOTC,
    240,
    &dsat707_qcotc_val[0],
    NULL,
    dsat707_exec_qcotc_cmd }
/*-------------------------------------------------------------------------*/
 ,{ "$QCVAD",
    EXTENDED | CONFIG | LOCAL_TEST,
    SPECIAL_QCVAD,
    1,
    &dsat707_qcvad_val,
    &dsat707_qcvad_dflm[0] ,
    dsat707_exec_qcvad_cmd}
/*-------------------------------------------------------------------------*/
 ,{ "$QCCAV",
    EXTENDED,
    SPECIAL_QCCAV,
    0,
    NULL,
    NULL,
    dsat707_exec_qccav_cmd }
/*-------------------------------------------------------------------------*/
 ,{ "$QCCHV",
    EXTENDED,
    SPECIAL_QCCHV,
    0,
    NULL,
    NULL,
    dsat707_exec_qcchv_cmd }
};

  /* Size of vendor specific command table */
const unsigned int dsat707_vend_para_table_size = ARR_SIZE( dsat707_vend_para_table );
const unsigned int dsat707_vend_action_table_size = ARR_SIZE( dsat707_vend_action_table );


/*===========================================================================
                          EXTERNAL FUNCTIONS
===========================================================================*/

#endif /* FEATURE_DATA_IS707 */
