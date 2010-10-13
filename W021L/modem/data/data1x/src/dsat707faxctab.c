/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

               I S - 7 0 7   C O M M A N D   T A B L E S

GENERAL DESCRIPTION
  This module contains the command tables and data definitions required
  to define the FAX related AT modem commands for the IS-707 mode.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

   Copyright (c) 1995-2009 by QUALCOMM, Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsat707faxctab.c_v   1.5   23 Oct 2002 10:10:04   sramacha  $
  $Header: //source/qcom/qct/modem/data/common/1xatcop/main/lite/src/dsat707faxctab.c#2 $ $DateTime: 2009/01/27 05:42:02 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/23/09   sn      Featurized code under FEATURE_ASYNC_DATA_NOOP.
10/15/02   ak      Updated FEATURE_JCDMA_DS to FEATURE_JCDMA_DS_1X
4/3/01     rsl     Initial release.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#ifndef FEATURE_ASYNC_DATA_NOOP
#ifdef FEATURE_DATA_IS707
#include "dsati.h"
#include "dsat707fax.h"
#include "dsat707faxctab.h"
#include "dsatparm.h"
#include "msg.h"


/*  Flag to indicate that an AT+FDT command has been   */
/*  and that a land to mobile FAX image is about to start */
boolean dsat707_fdt_seen = FALSE;

/*=========================================================================*/
/* Data declarations for fax commands */
/*=========================================================================*/
dsat_num_item_type dsat707_faa_val;
dsat_num_item_type dsat707_fap_val[3];
dsat_num_item_type dsat707_fbo_val;
dsat_num_item_type dsat707_fbu_val;
dsat_num_item_type dsat707_fcc_val[8];
dsat_num_item_type dsat707_fcq_val[2];
dsat_num_item_type dsat707_fcr_val;
dsat_num_item_type dsat707_fct_val;
dsat_num_item_type dsat707_fea_val;
dsat_num_item_type dsat707_ffc_val[4];
dsat_num_item_type dsat707_fhs_val;
dsat_num_item_type dsat707_fie_val;
dsat_num_item_type dsat707_fip_val;
dsat_num_item_type dsat707_fis_val[8];
dsat_string_item_type dsat707_fli_val[21];
dsat_num_item_type dsat707_flp_val;
dsat_num_item_type dsat707_fms_val;
dsat_num_item_type dsat707_fnr_val[4];
dsat_string_item_type dsat707_fns_val[181];
dsat_string_item_type dsat707_fpa_val[21];
dsat_string_item_type dsat707_fpi_val[21];
dsat_num_item_type dsat707_fpp_val;
dsat_num_item_type dsat707_fpr_val;
dsat_num_item_type dsat707_fps_val;
dsat_string_item_type dsat707_fpw_val[21];
dsat_num_item_type dsat707_frq_val[2];
dsat_num_item_type dsat707_fry_val;
dsat_string_item_type dsat707_fsa_val[21];
dsat_num_item_type dsat707_fsp_val;

const unsigned int dsat707_sizeof_fns_val = ARR_SIZE (dsat707_fns_val);

/*=========================================================================*/
/* Defaults & Limits for FAX commands */
/*=========================================================================*/

LOCAL const dflm_type dsat707_faa_dflm [] =
{
  { 0  , 0  , 1    },
} ;

LOCAL const dflm_type dsat707_fap_dflm [] =
{
  { 0  , 0  , 1    },
  { 0  , 0  , 1    },
  { 0  , 0  , 1    },
} ;

LOCAL const dflm_type dsat707_fbo_dflm [] =
{
  { 0  , 0  , 3    },
} ;

LOCAL const dflm_type dsat707_fbu_dflm [] =
{
  { 0  , 0  , 1    },
} ;

LOCAL const dflm_type dsat707_fcc_dflm [] =
{
#ifdef FEATURE_JCDMA_1X
  { 0  , 0  , 1    },  /* VR: R8*3.85l/mm, R8*7.7l/mm */
  { 1  , 0  , 5    },  /* BR: 2400,4800,7200,9600,12000,14400bps */
  { 0  , 0  , 2    },  /* WD: 1728,2048,2432d/mm */
  { 0  , 0  , 2    },  /* LN: A4(297mm), B4(364mm), Unlimited */
  { 0  , 0  , 1    },  /* DF: MH, MR */
  { 0  , 0  , 0    },  /* EC: disable */
  { 0  , 0  , 0    },  /* BF: disable */
  { 0  , 0  , 7    },  /* ST: 0,5,10,20,40ms */
#else
  { 0  , 0  , 1    },
  { 1  , 0  , 5    },
  { 0  , 0  , 4    },
  { 0  , 0  , 2    },
  { 0  , 0  , 3    },
  { 0  , 0  , 1    },
  { 0  , 0  , 1    },
  { 0  , 0  , 7    },
#endif /* FEATURE_JCDMA_1X */
} ;

LOCAL const dflm_type dsat707_fclass_dflm [] = 
{
  { 0  , 0  , 2    },
} ;

LOCAL const dflm_type dsat707_fcq_dflm [] =
{
  { 1  , 0  , 2    },
  { 0  , 0  , 2    },
} ;

LOCAL const dflm_type dsat707_fcr_dflm [] =
{
  { 0,   0  , 1    },
} ;


LOCAL const dflm_type dsat707_fct_dflm [] =
{
  { 0x1E, 0x0, 0xFF  },
} ;

LOCAL const dflm_type dsat707_fea_dflm [] =
{
  { 0  , 0  , 1    },
} ;

LOCAL const dflm_type dsat707_ffc_dflm [] =
{
  { 0  , 0  , 3    },
  { 0  , 0  , 2    },
  { 0  , 0  , 3    },
  { 0  , 0  , 2    },
} ;

LOCAL const dflm_type dsat707_fhs_dflm [] =
{
  { 0  , 00 , 0xFF  },
} ;

LOCAL const dflm_type dsat707_fie_dflm [] =
{
  { 0  , 0  , 1    },
} ;

LOCAL const dflm_type dsat707_fip_dflm [] =
{
  { 0  , 0  , 0    },
} ;

LOCAL const dflm_type dsat707_fis_dflm [] =
{
  { 0  , 0  , 1    },
  { 1  , 0  , 5    },
  { 0  , 0  , 4    },
  { 0  , 0  , 2    },
  { 0  , 0  , 3    },
  { 0  , 0  , 1    },
  { 0  , 0  , 1    },
  { 0  , 0  , 7    },
} ;

LOCAL const dflm_type dsat707_flo_dflm [] =
{
  { 1  , 0  , 2    },
} ;


LOCAL const dflm_type dsat707_flp_dflm [] =
{
  { 0  , 0  , 1    },
} ;

LOCAL const dflm_type dsat707_fms_dflm [] =
{
  { 0  , 0  , 5    },
} ;

LOCAL const dflm_type dsat707_fnr_dflm [] =
{
  { 0  , 0  , 1    },
  { 0  , 0  , 1    },
  { 0  , 0  , 1    },
  { 0  , 0  , 1    },
} ;

LOCAL const dflm_type dsat707_fpp_dflm [] =
{
  { 0  , 0  , 0    },
} ;

LOCAL const dflm_type dsat707_fpr_dflm [] =
{
  { 8  , 8  , 8  },
} ;

LOCAL const dflm_type dsat707_fps_dflm [] =
{
  { 1  , 0  , 5    },
} ;

LOCAL const dflm_type dsat707_frq_dflm [] =
{
  { 0,   0x0, 0x64  },
  { 0,   0x0, 0xFF  },
} ;

LOCAL const dflm_type dsat707_fry_dflm [] =
{
  { 0x0, 0x0, 0xFF  },
} ;

LOCAL const dflm_type dsat707_fsp_dflm [] =
{
  { 0  , 0  , 1    },
} ;

/*=========================================================================*/
/* IS-707 Fax related AT command table */
/*=========================================================================*/
const dsati_cmd_type dsat707_fax_table [] =
{
  { "+FAA",     
    EXTENDED | CONFIG | HEX | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE,   
    1, 
    &dsat707_faa_val, 
    &dsat707_faa_dflm[0],
    dsat707_exec_faa_cmd },
/*-------------------------------------------------------------------------*/
  { "+FAP",
    EXTENDED | CONFIG | HEX | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE,
    3,
    &dsat707_fap_val[0],
    &dsat707_fap_dflm[0],
    dsat707_exec_fap_cmd },
/*-------------------------------------------------------------------------*/
  { "+FBO",
    EXTENDED | CONFIG | HEX | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE,
    1,
    &dsat707_fbo_val,  
    &dsat707_fbo_dflm[0],
    dsat707_exec_fbo_cmd },
/*-------------------------------------------------------------------------*/
  { "+FBS", 
    EXTENDED | READ_ONLY | FCLASS2P0,
    SPECIAL_FBS,
    0, 
    NULL,  
    NULL,
    dsat707_exec_fbs_cmd },
/*-------------------------------------------------------------------------*/
  { "+FBU",
    EXTENDED | CONFIG | HEX | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE,
    1,
    &dsat707_fbu_val,
    &dsat707_fbu_dflm[0],
    dsat707_exec_fbu_cmd },
/*-------------------------------------------------------------------------*/
  { "+FCLASS",
    EXTENDED | CONFIG | DOWN_LOAD,
    SPECIAL_FCLASS,
    1,
    &dsat_fclass_val, 
    &dsat707_fclass_dflm[0], 
    dsat707_exec_fclass_cmd},
/*-------------------------------------------------------------------------*/
  { "+FCQ",
    EXTENDED | CONFIG | HEX | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE,
    2, 
    &dsat707_fcq_val[0],
    &dsat707_fcq_dflm[0],
    dsat707_exec_fcq_cmd },
/*-------------------------------------------------------------------------*/
  { "+FCC",
    EXTENDED | CONFIG | HEX | ALWAYS | FCLASS2P0 | DOWN_LOAD | LOCAL_TEST,
    SPECIAL_FCC,
    8,   
    &dsat707_fcc_val[0],
    &dsat707_fcc_dflm[0],
    dsat707_exec_fcc_cmd},
/*-------------------------------------------------------------------------*/
  { "+FCR",
    EXTENDED | CONFIG | HEX | ALWAYS | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE,
    1,
    &dsat707_fcr_val,
    &dsat707_fcr_dflm[0],
    dsat707_exec_fcr_cmd },
/*-------------------------------------------------------------------------*/
  { "+FCT", 
    EXTENDED | CONFIG | HEX | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE,
    1, 
    &dsat707_fct_val,
    &dsat707_fct_dflm[0],
    dsat707_exec_fct_cmd },
/*-------------------------------------------------------------------------*/
  { "+FDR",  
    EXTENDED | FCLASS2P0,
    SPECIAL_FDR,
    0,    
    NULL,
    NULL,
    dsat707_exec_fdr_cmd },
/*-------------------------------------------------------------------------*/
  { "+FDT",  
    EXTENDED | FCLASS2P0,
    SPECIAL_FDT,
    0,    
    NULL,
    NULL,
    dsat707_exec_fdt_cmd },
/*-------------------------------------------------------------------------*/
  { "+FEA",
    EXTENDED | CONFIG | HEX | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE,
    1,
    &dsat707_fea_val, 
    &dsat707_fea_dflm[0],
    dsat707_exec_fea_cmd },
/*-------------------------------------------------------------------------*/
  { "+FFC", 
    EXTENDED | CONFIG | HEX | ALWAYS | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE, 
    4, 
    &dsat707_ffc_val[0],
    &dsat707_ffc_dflm[0],
    dsat707_exec_ffc_cmd },
/*-------------------------------------------------------------------------*/
  { "+FHS", 
    EXTENDED | CONFIG | HEX | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_FHS,
    1,   
    &dsat707_fhs_val,
    &dsat707_fhs_dflm[0],
    dsat707_exec_fhs_cmd },
/*-------------------------------------------------------------------------*/
  { "+FIE",
    EXTENDED | CONFIG | HEX | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE,
    1,
    &dsat707_fie_val,  
    &dsat707_fie_dflm[0],
    dsat707_exec_fie_cmd },
/*-------------------------------------------------------------------------*/
  { "+FIP",
    EXTENDED | CONFIG | HEX | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE, 
    1,
    &dsat707_fip_val, 
    &dsat707_fip_dflm[0],
    dsat707_exec_fip_cmd },
/*-------------------------------------------------------------------------*/
  { "+FIS",
    EXTENDED | CONFIG | HEX | ALWAYS | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE,
    8,
    &dsat707_fis_val[0],
    &dsat707_fis_dflm[0],
    dsat707_exec_fis_cmd},
/*-------------------------------------------------------------------------*/
  { "+FKS",     
    EXTENDED | FCLASS2P0,
    SPECIAL_NONE, 
    0,
    NULL,  
    NULL,
    dsat707_exec_fks_cmd },
/*-------------------------------------------------------------------------*/
  { "+FLI",  
    EXTENDED | CONFIG | STRING | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE,
    20,
    &dsat707_fli_val[0],
    NULL,
    dsat707_exec_fli_cmd },
/*-------------------------------------------------------------------------*/
  { "+FLO", 
    EXTENDED | CONFIG | HEX | FCLASS2P0 | LOCAL_TEST,
    SPECIAL_NONE,
    1, 
    &dsat_flo_val,
    &dsat707_flo_dflm[0],
    dsat707_exec_flo_cmd },
/*-------------------------------------------------------------------------*/
  { "+FLP",
    EXTENDED | CONFIG | HEX | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE,
    1, 
    &dsat707_flp_val,
    &dsat707_flp_dflm[0],
    dsat707_exec_flp_cmd },
/*-------------------------------------------------------------------------*/
  { "+FMS",
    EXTENDED | CONFIG | HEX | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE,
    1,
    &dsat707_fms_val,   
    &dsat707_fms_dflm[0],
    dsat707_exec_fms_cmd },
/*-------------------------------------------------------------------------*/
  { "+FNR",
    EXTENDED | CONFIG | HEX | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE,
    4,
    &dsat707_fnr_val[0],
    &dsat707_fnr_dflm[0],
    dsat707_exec_fnr_cmd },
/*-------------------------------------------------------------------------*/
  { "+FNS",
    EXTENDED | CONFIG | STRING | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_FNS,
    180,  
    &dsat707_fns_val[0],
    NULL,
    dsat707_exec_fns_cmd },
/*-------------------------------------------------------------------------*/
  { "+FPA",
    EXTENDED | CONFIG | STRING | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE,
    20,
    &dsat707_fpa_val[0],
    NULL,
    dsat707_exec_fpa_cmd },
/*-------------------------------------------------------------------------*/
  { "+FPI", 
    EXTENDED | CONFIG | STRING | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE, 
    20,
    &dsat707_fpi_val[0],
    NULL,
    dsat707_exec_fpi_cmd },
/*-------------------------------------------------------------------------*/
  { "+FPP",
    EXTENDED | CONFIG | HEX | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE,  
    1,
    &dsat707_fpp_val,  
    &dsat707_fpp_dflm[0],
    dsat707_exec_fpp_cmd },
/*-------------------------------------------------------------------------*/
  { "+FPR",  
    EXTENDED | CONFIG | HEX | FCLASS2P0 | LOCAL_TEST,
    SPECIAL_NONE,  
    1,
    &dsat707_fpr_val,
    &dsat707_fpr_dflm[0],
    dsat707_exec_fpr_cmd },
/*-------------------------------------------------------------------------*/
  { "+FPS", 
    EXTENDED | CONFIG | HEX | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE, 
    1,
    &dsat707_fps_val,
    &dsat707_fps_dflm[0],
    dsat707_exec_fps_cmd },
/*-------------------------------------------------------------------------*/
  { "+FPW",
    EXTENDED | CONFIG | STRING | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE,  
    20,
    &dsat707_fpw_val[0],
    NULL,
    dsat707_exec_fpw_cmd },
/*-------------------------------------------------------------------------*/
  { "+FRQ",
    EXTENDED | CONFIG | HEX | ALWAYS | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE, 
    2,
    &dsat707_frq_val[0],
    &dsat707_frq_dflm[0],
    dsat707_exec_frq_cmd },
/*-------------------------------------------------------------------------*/
  { "+FRY",
    EXTENDED | CONFIG | HEX | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE,
    1, 
    &dsat707_fry_val, 
    &dsat707_fry_dflm[0],
    dsat707_exec_fry_cmd },
/*-------------------------------------------------------------------------*/
  { "+FSA",
    EXTENDED | CONFIG | STRING | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE, 
    20, 
    &dsat707_fsa_val[0],
    NULL,
    dsat707_exec_fsa_cmd },
/*-------------------------------------------------------------------------*/
  { "+FSP", 
    EXTENDED | CONFIG | HEX | FCLASS2P0 | DOWN_LOAD,
    SPECIAL_NONE,
    1,
    &dsat707_fsp_val,
    &dsat707_fsp_dflm[0],
    dsat707_exec_fsp_cmd },
/*-------------------------------------------------------------------------*/
};

/* Size of FAX command table */
const unsigned int dsat707_fax_table_size = ARR_SIZE( dsat707_fax_table );

#endif /* FEATURE_DATA_IS707 */
#endif /* FEATURE_ASYNC_DATA_NOOP */

