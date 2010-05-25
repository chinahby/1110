/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

               C O M M O N   C O M M A N D   T A B L E S

GENERAL DESCRIPTION
  This module contains the command tables and data definitions required
  to define the AT modem commands that are common across all modes of
  operation (ETSI, CDMA2000, IS-95, etc.).

EXTERNALIZED FUNCTIONS
  dsatctab_data_init
    This function initializes the AT command defaults, limits, and lists
    that depend on a run-time determination of the hardware.

    
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

   Copyright (c) 2000 - 2009 QUALCOMM Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsatctab.c_v   1.10   11 Nov 2002 14:02:12   sbandaru  $
  $Header: //source/qcom/qct/modem/data/common/commonatcop/main/lite/src/dsatctab.c#2 $ $DateTime: 2009/01/22 06:46:48 $ $Author: bhaviks $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/02/09   bs      Added support for 1X AT Phonebook commands.
12/25/03   rsl     Changes relating to Unrec. cmds not working.
05/28/03   sb      Fix for WCDMA CS Calls
02/20/02   sb      changed the range of S10 register from 1-255
02/19/03   ar      Fix DSPE build error.
02/11/03   ar      Adjust +DS string paramater range
01/27/03   ar      Added 230400 baud to common +IPR and $QCTER value lists.
01/16/03   ar      Migrated context cmds to persistent PDP profile support
01/07/03   rsl     Removed dsat_qcpma_val, no longer used.
11/11/02   sb      NV intialization in dsatctab_data_init()
11/07/02   ar      Added FEATURE_GSM_GPRS to FEATURE_DATA_WCDMA_PS wrappers
10/15/02   sb      Changed the power on default of &C in JCDMA mode
04/17/02   rc      Removed FEATURE wrap FEATURE_DS_SOCKETS around dns 
                   variables.
04/11/02   ar      Add support for $QCPDPP command
09/12/01   ar      Added commands to extended AT command table:
                     +GMI,+GMM,+GMR,+GSN,+GCAP,I
08/27/01   sjd     Initial release of new VU structure to MSM archives.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA

#include "target.h"
#include "dsati.h"
#include "hw.h"
#include "msg.h"
#include "dsatparm.h"
#include "dsatact.h"
#include "dsatctab.h"
#include "dsatme.h"
#include "dsatvend.h"
#include "dstaski.h"
#include "nv.h"

#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)
#error code not present
#endif /* FEATURE_DATA_WCDMA_PS  || FEATURE_GSM_GPRS */


/*===========================================================================

                    REGIONAL DEFINITIONS AND DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  NV interface data 
---------------------------------------------------------------------------*/
nv_item_type         ds_nv_item;                  /* actual data item     */



/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
                           Defaults and limits
---------------------------------------------------------------------------*/
dsat_num_item_type dsat_cmee_val;

#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)
/* +CPBS Command */
dsat_num_item_type dsat_cpbs_val;

/* the sequence of the storage type is important, it has to be the same as
   that of dsatetsi_pb_pbm_table 
   DEFAULT_CPB_STORAGE is SM, we use macro here to synchronize it with 
   me_cpbs_default */
const dsat_string_item_type dsat_cpbs_storage_valstr [][8] =
{ DEFAULT_CPB_STORAGE, "DC", "FD", "LD","MC",DEFAULT_NV_CPB_STORAGE, "RC", "EN","ON",""};

LOCAL const dsat_string_item_type dsat_cpbs_storage_tststr [] =
{ "(\"SM\",\"DC\",\"FD\",\"LD\",\"MC\",\"ME\",\"RC\",\"EN\",\"ON\")" };

const def_list_type dsat_cpbs_list =
{ 0, dsat_cpbs_storage_valstr, dsat_cpbs_storage_tststr };
#endif /* defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) */

LOCAL const dflm_type dsat_amp_c_dflm [] =
{
#ifdef FEATURE_JCDMA_1X
  { 1  , 0  , 1    },
#else
  { 2  , 0  , 2    },
#endif
} ;

LOCAL const dflm_type dsat_amp_d_dflm [] =
{
  { 2  , 0  , 2    },
} ;

LOCAL const dflm_type dsat_amp_f_dflm [] =
{
  { 0  , 0  , 0    },
} ;

LOCAL const dflm_type dsat_icf_dflm [] =
{
  { 3  , 3  , 3    },
  { 3  , 0  , 3    },
} ;

LOCAL const dflm_type dsat_ifc_dflm [] =
{
  { 2  , 0  , 3    },
  { 2  , 0  , 2    },
} ;

/*  The bit rates in the "valstr" table must match exactly the  */
/*  value and order of the "sio_baudrate_type" enum in sio.h    */
/*  Unused values must have space strings as placeholders       */
/*  The "valstr" table must be terminated with a null string    */

/*  Bit rates for MSM 2.3 and later                             */
#ifdef FEATURE_JCDMA_1X
LOCAL const dsat_string_item_type dsat_ipr_valstr [][8] =
  { "0", " ", " ", " ", " ", " ", " ", " ", " ", " ",
    " ", " ", " ", " ", " ", " ", " ", "115200", 
#ifdef FEATURE_UART_TCXO_CLK_FREQ
    "230400",
#endif
#ifdef FEATURE_UART_RATE1
    "460800", 
#endif /* FEATURE_UART_RATE1 */
    "" };

#ifdef FEATURE_UART_RATE1
LOCAL const dsat_string_item_type dsat_ipr_tststr [] =
  { "(),"
    "(0,115200,230400,460800)" };
#else
LOCAL const dsat_string_item_type dsat_ipr_tststr [] =
  { "(),"
    "(0,115200,230400)" };
#endif /* FEATURE_UART_RATE1 */

#else /* FEATURE_JCDMA_1X */
/* Applies to ETSI & IS707 modes */
LOCAL const dsat_string_item_type dsat_ipr_valstr [][8] =
  { " ", " ", " ", " ", " ", " ", "300", "600", "1200","2400",
    "4800", "9600", " ", "19200", "38400", "57600", " ", "115200",
#ifdef FEATURE_UART_TCXO_CLK_FREQ
    "230400",
#endif /* FEATURE_UART_TCXO_CLK_FREQ */
    "" };

LOCAL const dsat_string_item_type dsat_ipr_tststr [] =
  { "(),"
#ifdef FEATURE_UART_TCXO_CLK_FREQ
    "(300,600,1200,2400,4800,9600,19200,38400,57600,115200,230400)"
#else
    "(300,600,1200,2400,4800,9600,19200,38400,57600,115200)"
#endif /* FEATURE_UART_TCXO_CLK_FREQ */
  };
#endif /* FEATURE_JCDMA_1X */

/* Initialized at powerup by dsatctab_data_init().This initialization to 
MSM 2.2 is "just in case". */
LOCAL def_list_type dsat_ipr_list =
{
  17,  /*  115200 */
  dsat_ipr_valstr,  
  dsat_ipr_tststr
} ;

LOCAL const dsat_string_item_type dsat_qcdmr_valstr [][8] =
  { " ", " ", " ", " ", " ", " ", " ", " ", " ",
    " ", " ", " ", " ", " ", "38400", "57600", " ", "115200", " ", "" };

LOCAL const dsat_string_item_type dsat_qcdmr_tststr [] =
  { "(38400,57600,115200)" };

/* $QCTER baud rates should match those for +IPR */
#ifdef FEATURE_JCDMA_1X
LOCAL const dsat_string_item_type dsat_qcter_valstr [][8] =
  { "0", " ", " ", " ", " ", " ", " ", " ", " ", " ",
    " ", " ", " ", " ", " ", " ", " ", "115200", 
#ifdef FEATURE_UART_TCXO_CLK_FREQ
    "230400",
#endif
#ifdef FEATURE_UART_RATE1
    "460800", 
#endif /* FEATURE_UART_RATE1 */
    "" };

#ifdef FEATURE_UART_RATE1
LOCAL const dsat_string_item_type dsat_qcter_tststr [] =
  { "(),"
    "(0,115200,230400,460800)" };
#else
LOCAL const dsat_string_item_type dsat_qcter_tststr [] =
  { "(),"
    "(0,115200,230400)" };
#endif /* FEATURE_UART_RATE1 */

#else /* FEATURE_JCDMA_1X */
/* Applies to ETSI & IS707 modes */
LOCAL const dsat_string_item_type dsat_qcter_valstr [][8] =
  { " ", " ", " ", " ", " ", " ", "300", "600", "1200","2400",
    "4800", "9600", " ", "19200", "38400", "57600", " ", "115200",
#ifdef FEATURE_UART_TCXO_CLK_FREQ
    "230400",
#endif /* FEATURE_UART_TCXO_CLK_FREQ */
    "" };

LOCAL const dsat_string_item_type dsat_qcter_tststr [] =
  { "(),"
#ifdef FEATURE_UART_TCXO_CLK_FREQ
    "(300,600,1200,2400,4800,9600,19200,38400,57600,115200,230400)"
#else
    "(300,600,1200,2400,4800,9600,19200,38400,57600,115200)"
#endif /* FEATURE_UART_TCXO_CLK_FREQ */
  };
#endif /* FEATURE_JCDMA_1X */

LOCAL const def_list_type dsat_qcdmr_list =
{
  17,  /*  115200 */ 
  dsat_qcdmr_valstr,
  dsat_qcdmr_tststr
} ;

LOCAL def_list_type dsat_qcter_list =
{
  17,  /*  115200 */
  dsat_qcter_valstr,    
  dsat_qcter_tststr
} ;

LOCAL const dflm_type dsat_e_dflm [] =
{
  { 1  , 0  , 1    },
} ;

LOCAL const dflm_type dsat_l_dflm [] =
{
  { 0  , 0  , 3    },
} ;

LOCAL const dflm_type dsat_m_dflm [] =
{
  { 0  , 0  , 2    },
} ;

LOCAL const dflm_type dsat_q_dflm [] =
{
  { 0  , 0  , 1    },
} ;

LOCAL const dflm_type dsat_s0_dflm [] =
{
  { 0  , 0  , 255  },
} ;

/* V.25 defines a range of 1 - 154            */
/* IS707 has a range of 1 - 255, IS-707 Wins! */
LOCAL const dflm_type dsat_s10_dflm [] =
{
  { 14 , 1  , 255  },
} ;

LOCAL const dflm_type dsat_s11_dflm [] =
{
  { 95 , 50 , 255  },
} ;

LOCAL const dflm_type dsat_s3_dflm [] =
{
  { 13 , 0  , 127  },
} ;

LOCAL const dflm_type dsat_s4_dflm [] =
{
  { 10 , 0  , 127  },
} ;

LOCAL const dflm_type dsat_s5_dflm [] =
{
  { 8  , 0  , 127  },
} ;

LOCAL const dflm_type dsat_s6_dflm [] =
{
  { 2  , 2  , 10   },
} ;

LOCAL const dflm_type dsat_s7_dflm [] =
{
  { 50 , 1  , 255  },
} ;

LOCAL const dflm_type dsat_s8_dflm [] =
{
  { 2  , 0  , 255  },
} ;

LOCAL const dflm_type dsat_s9_dflm [] =
{
  { 6  , 0  , 255  },
} ;

LOCAL const dflm_type dsat_v_dflm [] =
{
  { 1  , 0  , 1    },
} ;

LOCAL const dflm_type dsat_x_dflm [] =
{
  { 4  , 0  , 4    },
} ;

LOCAL const dflm_type dsat_z_dflm [] =
{
  { 0  , 0  , 0    },
} ;


#ifdef FEATURE_DS_SOCKETS_TEST_UDP
#error code not present
#endif /* FEATURE_DS_SOCKETS_TEST_UDP */

/*---------------------------------------------------------------------------
  The arrays dsat_qcdnspri_val and dsat_qcdnssec_val are used to 
  store the incoming strings. Since the incoming string should contain a 
  " to start and end the string and "/0", the length 18 was chosen. 
  Example, "129.046.050.007"
---------------------------------------------------------------------------*/
#define MAX_DNS_SIZE 18
dsat_string_item_type dsat_qcdnspri_val[MAX_DNS_SIZE];
dsat_string_item_type dsat_qcdnssec_val[MAX_DNS_SIZE];

/*---------------------------------------------------------------------------
  The array dsat_qcdns_val stores the HEX value obtained after 
  concatenating the incoming string.
  dsat_qcdns_val[0] is for Primary DNS IP address
  dsat_qcdns_val[1] is for Secondary DNS IP address
  Since we store Primary and Secondary DNS IP addresses at two separate
  locations we need an array. The "resolver" function, accesses the IP
  addresses through an index to this array.
---------------------------------------------------------------------------*/
uint32 dsat_qcdns_val[2];


#if defined(FEATURE_GSM) || defined(FEATURE_DATA_IS707)  /* Exclude WCDMA */

/* +DR Command */
LOCAL const dflm_type dsat_dr_dflm [] =
{
  { 0  ,  0  , 1    }
};

/* +DS Command */
LOCAL const dflm_type dsat_ds_dflm [] =
{
  { 3  ,  0  , 3    },  /* Direction       */
  { 0  ,  0  , 0    },  /* Negotiation     */
  { 2048, 512, 2048 },  /* Max Dictionary  */
  { 6  ,  6  , 6   }    /* Max String      */
} ;

#endif /* FEATURE_GSM  || FEATURE_DATA_IS707*/ 


#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)
#error code not present
#endif /* FEATURE_DATA_WCDMA_PS || FEATURE_GSM_GPRS */


/*  Numeric Config items  */

dsat_num_item_type dsat_qcdmr_val;
dsat_num_item_type dsat_qcter_val;

dsat_num_item_type dsat_amp_c_val;
dsat_num_item_type dsat_amp_d_val;
dsat_num_item_type dsat_amp_f_val;

dsat_num_item_type dsat_crc_val;

dsat_num_item_type dsat_fclass_val;

dsat_num_item_type dsat_flo_val;

dsat_num_item_type dsat_icf_val[2];

dsat_num_item_type dsat_ifc_val[2];

dsat_num_item_type dsat_ipr_val;

dsat_num_item_type dsat_e_val;

dsat_num_item_type dsat_l_val;
dsat_num_item_type dsat_m_val;
dsat_num_item_type dsat_o_val;

dsat_num_item_type dsat_q_val;

dsat_num_item_type dsat_s0_val;

dsat_num_item_type dsat_s10_val;
dsat_num_item_type dsat_s11_val;

dsat_num_item_type dsat_s3_val;
dsat_num_item_type dsat_s4_val;
dsat_num_item_type dsat_s5_val;

dsat_num_item_type dsat_s6_val;
dsat_num_item_type dsat_s7_val;
dsat_num_item_type dsat_s8_val;
dsat_num_item_type dsat_s9_val;

dsat_num_item_type dsat_v_val;

dsat_num_item_type dsat_x_val;
dsat_num_item_type dsat_z_val;

#if defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_IS707) || \
    defined(FEATURE_DATA_WCDMA_CS)
dsat_num_item_type dsat_dr_val;
dsat_num_item_type dsat_ds_val[4];
#endif


#ifdef FEATURE_DSAT_TEST_32ARG 

/*---------------------------------------------------------------------------
  The following def and val's are for test purpose only.
  AT+TST32, AT+TTT32, and AT$TVT32 are used to test 32 bit argument support
  Since we don't currently have a AT command with 32 bit argument,
  we have to make up commands to test the functionality.
---------------------------------------------------------------------------*/

LOCAL dsat_num_item_type dsat_tst32_val[2];
LOCAL const dflm_type dsat_tst32_dflm[] = 
{
  { 0x075BCD15,  /* 123456789 default*/
    0x00000fff,  /* 4095  */
    ~0         },/* 0xffffffff, 4294967295 */

  { 0x075BCD15,  /* 123456789 */
    0x0000ffff,  /* 65535  */
    ~0         } /* 4294967295 */
};

#define DSAT_TUT32_FIRST_STR_LEN 21

LOCAL dsat_string_item_type 
dsat_tut32_first_str_val[DSAT_TUT32_FIRST_STR_LEN + 1];

LOCAL dsat_mixed_param_val_type dsat_tut32_val[2] = {
  dsat_tut32_first_str_val, 0
};

LOCAL uint16 dsat_tut32_first_str_len = DSAT_TUT32_FIRST_STR_LEN;

LOCAL mixed_def_s_type dsat_tut32_first_def =
{ CONFIG | STRING, (mixed_def_u_type *) & dsat_tut32_first_str_len};

LOCAL dflm_type dsat_tut32_second_dflm =
{ 0x075BCD15,   /* 123456789  */
  0x0000ffff,   /* 65535      */
  ~0         }; /* 4294967295 */

LOCAL mixed_def_s_type dsat_tut32_second_def =
{ CONFIG | LOCAL_TEST, (mixed_def_u_type *) &dsat_tut32_second_dflm };

LOCAL mixed_def_s_type * dsat_tut32_mixed_dfl[] =
{
  &dsat_tut32_first_def, &dsat_tut32_second_def
};

#endif /* FEATURE_DSAT_TEST_32ARG */

/*--------------------------------------------------------------------------
                          Common Command Tables 
--------------------------------------------------------------------------*/

/* Basic common AT command table. */
const dsati_cmd_type dsat_basic_table [] =
{

  { "&C",       CONFIG | SIMPLE_DEC,
    SPECIAL_NONE,    1,      &dsat_amp_c_val,   &dsat_amp_c_dflm[0],
    dsatparm_exec_param_cmd,            NULL },

  { "&D",       CONFIG | SIMPLE_DEC,
    SPECIAL_NONE,    1,      &dsat_amp_d_val,   &dsat_amp_d_dflm[0],
    dsatparm_exec_param_cmd,            NULL },

  { "&F",       CONFIG | SIMPLE_DEC,
    SPECIAL_AMP_F,   1,      &dsat_amp_f_val,   &dsat_amp_f_dflm[0],
    dsatact_exec_ampf_cmd,              NULL },

  { "&V",       ATTRIB_NONE,
    SPECIAL_AMP_V,   0,      NULL,                  NULL,
    dsatparm_exec_ampv_cmd,             NULL },

  { "E",        CONFIG | SIMPLE_DEC | DOWN_LOAD,
    SPECIAL_NONE,    1,      &dsat_e_val,       &dsat_e_dflm[0],
    dsatparm_exec_param_cmd,            NULL },

  { "I",        READ_ONLY,
    SPECIAL_NONE,    0,      NULL,                  NULL,
    dsatact_exec_ati_cmd,                 NULL },

  { "L",        CONFIG | SIMPLE_DEC,
    SPECIAL_NONE,    1,      &dsat_l_val,       &dsat_l_dflm[0],
    dsatparm_exec_param_cmd,            NULL },

  { "M",        CONFIG | SIMPLE_DEC,
    SPECIAL_NONE,    1,      &dsat_m_val,       &dsat_m_dflm[0],
    dsatparm_exec_param_cmd,            NULL },

  { "Q",        CONFIG | SIMPLE_DEC | DOWN_LOAD,
    SPECIAL_NONE,    1,      &dsat_q_val,       &dsat_q_dflm[0],
    dsatparm_exec_param_cmd,            NULL },

  { "V",        CONFIG | SIMPLE_DEC | DOWN_LOAD,
    SPECIAL_NONE,    1,      &dsat_v_val,       &dsat_v_dflm[0],
    dsatparm_exec_param_cmd,            NULL },

  { "X",        CONFIG | SIMPLE_DEC | DOWN_LOAD,
    SPECIAL_NONE,    1,      &dsat_x_val,       &dsat_x_dflm[0],
    dsatparm_exec_param_cmd,            NULL },

  { "Z",        CONFIG | SIMPLE_DEC,
    SPECIAL_Z,       1,      &dsat_z_val,       &dsat_z_dflm[0],
    dsatact_exec_atz_cmd,               NULL },

  { "T",       ATTRIB_NONE,
    SPECIAL_NONE,   0,      NULL,                  NULL,
    dsatparm_exec_dtype_cmd,            NULL },

  { "P",       ATTRIB_NONE,
    SPECIAL_NONE,   0,      NULL,                  NULL,
    dsatparm_exec_dtype_cmd,            NULL }
} ;

/* Basic common action command table. */
const dsati_cmd_type dsat_basic_action_table [] =
{
  
  { "D",       RESTRICTED,
    SPECIAL_NONE,    0,      NULL,                  NULL,
    dsatact_exec_atd_cmd,               dsatact_call_abort_handler },

  { "A",       ATTRIB_NONE,
    SPECIAL_NONE,    0,      NULL,                  NULL,
    dsatact_exec_ata_cmd,               dsatact_call_abort_handler },

  { "H",       ATTRIB_NONE,
    SPECIAL_NONE,    0,      NULL,                  NULL,
    dsatact_exec_ath_cmd,               NULL },

  { "O",       ATTRIB_NONE,
    SPECIAL_NONE,    0,      NULL,                  NULL,
    dsatact_exec_ato_cmd,               NULL }
};

/* Common S-register command table. */
const dsati_cmd_type dsat_sreg_table [] =
{

  { "S0",       CONFIG,
    SPECIAL_NONE,    1,      &dsat_s0_val,      &dsat_s0_dflm[0],
    dsatparm_exec_param_cmd,            NULL },

  { "S3",       CONFIG | DOWN_LOAD,
    SPECIAL_NONE,    1,      &dsat_s3_val,      &dsat_s3_dflm[0],
    dsatparm_exec_param_cmd,            NULL },

  { "S4",       CONFIG | DOWN_LOAD,
    SPECIAL_NONE,    1,      &dsat_s4_val,      &dsat_s4_dflm[0],
    dsatparm_exec_param_cmd,            NULL },

  { "S5",       CONFIG | DOWN_LOAD,
    SPECIAL_NONE,    1,      &dsat_s5_val,      &dsat_s5_dflm[0],
    dsatparm_exec_param_cmd,            NULL },

  { "S6",       CONFIG | DOWN_LOAD,
    SPECIAL_NONE,    1,      &dsat_s6_val,      &dsat_s6_dflm[0],
    dsatparm_exec_param_cmd,            NULL },

  { "S7",       CONFIG | DOWN_LOAD,
    SPECIAL_NONE,    1,      &dsat_s7_val,      &dsat_s7_dflm[0],
    dsatparm_exec_param_cmd,            NULL },

  { "S8",       CONFIG | DOWN_LOAD,
    SPECIAL_NONE,    1,      &dsat_s8_val,      &dsat_s8_dflm[0],
    dsatparm_exec_param_cmd,            NULL },

  { "S9",       CONFIG | DOWN_LOAD,
    SPECIAL_NONE,    1,      &dsat_s9_val,      &dsat_s9_dflm[0],
    dsatparm_exec_param_cmd,            NULL },

  { "S10",      CONFIG | DOWN_LOAD,
    SPECIAL_NONE,    1,      &dsat_s10_val,     &dsat_s10_dflm[0],
    dsatparm_exec_param_cmd,            NULL },

  { "S11",       CONFIG | DOWN_LOAD,
    SPECIAL_NONE,    1,      &dsat_s11_val,      &dsat_s11_dflm[0],
    dsatparm_exec_param_cmd,            NULL }
} ;

/* Extended common AT command table */
const dsati_cmd_type dsat_ext_table [] =
{

  { "+ICF",     EXTENDED | CONFIG | LOCAL_TEST,
    SPECIAL_NONE,    2,      &dsat_icf_val[0],  &dsat_icf_dflm[0],
    dsatparm_exec_param_cmd,            NULL },

  { "+IFC",     EXTENDED | CONFIG | LOCAL_TEST,
    SPECIAL_NONE,    2,      &dsat_ifc_val[0],  &dsat_ifc_dflm[0],
    dsatparm_exec_param_cmd,            NULL },

  { "+IPR",     EXTENDED | CONFIG | LIST | LOCAL_TEST,
    SPECIAL_NONE,     1,      &dsat_ipr_val,     &dsat_ipr_list,
    dsatparm_exec_ipr_cmd,              NULL },

  { "+GMI",     READ_ONLY,
    SPECIAL_NONE,   0,      NULL,                  NULL,
    dsatact_exec_gmi_cmd,               NULL },

  { "+GMM",     READ_ONLY,
    SPECIAL_NONE,   0,      NULL,                  NULL,
    dsatact_exec_gmm_cmd,               NULL },

  { "+GMR",     READ_ONLY,
    SPECIAL_NONE,   0,      NULL,                  NULL,
    dsatact_exec_gmr_cmd,               NULL },

  { "+GSN",     READ_ONLY,
    SPECIAL_NONE,   0,      NULL,                  NULL,
    dsatact_exec_gsn_cmd,               NULL },

  { "+GCAP",    READ_ONLY,
    SPECIAL_NONE,   0,      NULL,                  NULL,
    dsatact_exec_gcap_cmd,              NULL },

#if defined(FEATURE_GSM) || defined(FEATURE_DATA_IS707)   /* Exclude WCDMA */
  { "+DR",      EXTENDED | CONFIG | LOCAL_TEST,
    SPECIAL_NONE,   1,      &dsat_dr_val,     &dsat_dr_dflm[0],
    dsatparm_exec_param_cmd,            NULL },

  { "+DS",      EXTENDED | CONFIG | LOCAL_TEST,
    SPECIAL_NONE,   4,      &dsat_ds_val[0],  &dsat_ds_dflm[0],
    dsatparm_exec_param_cmd,            NULL },
#endif /* FEATURE_GSM || FEATURE_DATA_IS707*/

#ifdef FEATURE_DSAT_TEST_32ARG 
  { "+TST32",   EXTENDED | CONFIG | LOCAL_TEST,
    SPECIAL_NONE,   2,      &dsat_tst32_val[0], &dsat_tst32_dflm[0],
    dsatparm_exec_param_cmd,            NULL },
#endif /* FEATURE_DSAT_TEST_32ARG */

} ;

/* Vendor specific common AT command table. */
const dsati_cmd_type dsat_vendor_table [] =
{

  { "$QCCLR",   EXTENDED | READ_ONLY,
    SPECIAL_NONE,   0,      NULL,                  NULL,
    dsatvend_exec_qcclr_cmd,            NULL },

  { "$QCDMG",   EXTENDED,
    SPECIAL_NONE,   0,      NULL,                  NULL,
    dsatvend_exec_qcdmg_cmd,            NULL },

  { "$QCDMR",   EXTENDED | CONFIG | LIST | LOCAL_TEST,
    SPECIAL_NONE,   1,      &dsat_qcdmr_val,       &dsat_qcdmr_list,
    dsatvend_exec_qcdmr_cmd,            NULL },

#ifdef FEATURE_DS_SOCKETS_TEST_UDP
#error code not present
#endif  /* FEATURE_DS_SOCKETS_TEST_UDP */

  { "$QCDNSP",  EXTENDED | CONFIG | STRING | NO_QUOTE,
    SPECIAL_NONE,  MAX_DNS_SIZE,      &dsat_qcdnspri_val[0],    NULL,
    dsatvend_exec_qcdnsp_cmd,           NULL },

  { "$QCDNSS",  EXTENDED | CONFIG | STRING | NO_QUOTE,
    SPECIAL_NONE,  MAX_DNS_SIZE,      &dsat_qcdnspri_val[0],    NULL,
    dsatvend_exec_qcdnss_cmd,           NULL },

  { "$QCTER",  EXTENDED | CONFIG | LIST | LOCAL_TEST,
    SPECIAL_NONE,    1,      &dsat_qcter_val,     &dsat_qcter_list,
    dsatvend_exec_qcter_cmd,            NULL },

#if defined(FEATURE_DATA_WCDMA_PS)|| defined(FEATURE_GSM_GPRS)
#error code not present
#endif /* FEATURE_DATA_WCDMA_PS || FEATURE_GSM_GPRS */

#ifdef FEATURE_DSAT_TEST_32ARG 
  { "$TUT32",  EXTENDED | CONFIG | MIXED_PARAM | LIST | LOCAL_TEST,
    SPECIAL_NONE,  2,      &dsat_tut32_val[0],    &dsat_tut32_mixed_dfl[0],
    dsatparm_exec_param_cmd,           NULL },

#endif /* FEATURE_DSAT_TEST_32ARG */

#ifdef FEATURE_DSAT_BREW_SUPPORT
  { "$BREW",   EXTENDED,
    SPECIAL_NONE,   0,      NULL,                  NULL,
    dsatvend_exec_brew_cmd,            NULL }
#endif /* FEATURE_DSAT_BREW_SUPPORT */
} ;

/* Command table sizes. */
const unsigned int dsat_basic_table_size = ARR_SIZE( dsat_basic_table );
const unsigned int dsat_basic_action_table_size =
                        ARR_SIZE( dsat_basic_action_table );
const unsigned int dsat_sreg_table_size = ARR_SIZE( dsat_sreg_table );
const unsigned int dsat_ext_table_size = ARR_SIZE( dsat_ext_table );
const unsigned int dsat_vendor_table_size = ARR_SIZE( dsat_vendor_table );


/*===========================================================================

FUNCTION DSATCTAB_DATA_INIT

DESCRIPTION
  This function initializes the AT command defaults, limits, and lists
  that depend on a run-time determination of the hardware.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void dsatctab_data_init
(
  void
)
{
  /*---------------------------------------------------------------------------
  status from NV call. This variable will be used to obtain the status of the 
  read/write operations to the NV.
  ---------------------------------------------------------------------------*/
  nv_stat_enum_type    nv_status;          

  /*-----------------------------------------------------------------------
  Read the stored Baud rate from the NV. If NV is not written, then use 
  the default values.
  -----------------------------------------------------------------------*/
  nv_status = dsi_get_nv_item(NV_DS_DEFAULT_BAUDRATE_I, &ds_nv_item);
  
  /*-----------------------------------------------------------------------
  Check if the NV has valid data inside and load the data from NV to 
  local item.
  
  If NV is ACTIVE, confirm that the value inside NV is the valid value 
  by calling sio_is_baud_valid()       
  -----------------------------------------------------------------------*/
  if( nv_status == NV_DONE_S)
  {
    if( sio_is_baud_valid((sio_bitrate_type)ds_nv_item.ds_default_baudrate) 
      == TRUE)
    {
      dsat_qcter_list.default_v = ds_nv_item.ds_default_baudrate;
      dsat_ipr_list.default_v   = ds_nv_item.ds_default_baudrate;
    }
  }
  else
  {
    /*-----------------------------------------------------------------------
    If NV was never written then copy the default values into the ipr_val. 
    -----------------------------------------------------------------------*/
    MSG_HIGH("Baud rate not written to NV", 0, 0, 0);
    
    dsat_qcter_list.default_v  = SIO_BITRATE_115200;
    dsat_ipr_list.default_v    = SIO_BITRATE_115200;  
    
    /*-----------------------------------------------------------------------
      Initialize NV from statically-initialized default.
    -----------------------------------------------------------------------*/
    if( nv_status == NV_NOTACTIVE_S )
    {
      ds_nv_item.ds_default_baudrate = (uint16)dsat_qcter_list.default_v;
      (void) dsatutil_put_nv_item(NV_DS_DEFAULT_BAUDRATE_I, &ds_nv_item);
    }
  }

}  /* dsatctab_data_init( ) */


#endif /* FEATURE_DATA */
