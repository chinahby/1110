#ifndef AUTH_H
#define AUTH_H
/*===========================================================================

               E X T E R N A L   A U T H E N T I C A T I O N
                      T A S K   D E F I N I T I O N S

DESCRIPTION
  This contains all the declarations for the Authentication Task.

Copyright (c) 1995-2005 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/auth.h_v   1.8   30 Aug 2002 13:34:04   azafer  $
$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/inc/auth.h#1 $ $DateTime: 2009/05/29 03:54:56 $ $Author: prabhuk $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/30/05   va      Added new function auth_send_update_a_key_cmd  which is 
                   remotable for calling from SVCPRG module.
11/30/05   an      Mainlined FEATURE_IS2000
05/03/05   fc      Mainlined FEATURE_IS2000.
03/19/04   sb      Removed auth_switch_to_private_lcm.
08/30/02   ts      Added support for Run Time R-UIM Enable
08/06/02   lh      Remove SVD ifdefs.
04/12/02   lh      Added support for subaddress in Orig_C.
03/12/02   lh      Added CMEA support for release A messages.
08/09/01   sb      Merged the following from common archive:
                   06/19/01   ts      Moved the VPM size constant to cave.c.  Took out the 
                                      redundant #ifdef FEATURE_AUTH.
                   06/13/01   ts      Changed the Generate Keys octets from 1-65 to 0-64.
05/25/01   ts      Removed RUIM server code from auth and added interface to 
                   new UIM server.
02/07/01   wli     Split RUIM_UPDATE_RECORD_ST to two states: 
                   RUIM_UPDATE_LINEAR_FIXED_RECORD_ST and
                   RUIM_UPDATE_CYCLIC_RECORD_ST.
1/27/01    wli     Added new RUIM commands, INVALIDATE and REHABILITATE.
12/18/00   ck      Changed the request offset in the SSPR Configuration Request
                   Interface to a word from byte.
11/28/00   ck      Added definitions to support IS-683A with the R-UIM.
11/14/00   ts      Added access to the MSISDN in the CDMA directory.
10/11/00   ts      Moved the RUIM status outside of the rpt union field into its 
                   own field.
07/16/00   ck      Added CDMA DF definition.
06/12/00   ck      Added definitions for checks related to PPS and access of
                   records from RUIM.
05/30/00   ck      Changed RUIM_UI_CHV_SIG to RUIM_CHV_SIG and also removed
                   RUIM_MAX_CHV_DIGITS as it was defined in ruim.h
05/26/00   ck      Added a comma that caused the compile to break
05/01/00   ck      Added support for RUIM software. Included the new interfaces
                   for RUIM software and featurised it under FEATURE_RUIM.
                   AUTH only interface is featurised under FEATURE_AUTH to give
                   the flexibility of defining only FEATURE_AUTH or 
                   FEATURE_RUIM.
04/06/00   va      Merged the following from MSM3100_CP.03.00.17
           ry      Added OTAPA support
05/06/99   kmp     Changed T_ACP to FEATURE_ACP
03/18/99   ck      Merged the support for A-Key exchange and Re-Authentication
09/04/98   pms     Neared the variables to save ROM space
07/28/98   dna     Retain message encryption on CDMA->AMPS handoff by
                   separating auth_reset from auth_resync.
11/17/95   dna     Report back when keys are generated.
08/03/95   dna     Added support for message encryption
06/07/95   dna     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "queue.h"
#include "qw.h"
#include "cmd.h"
#include "cai.h"
#include "caii.h"
#include "ulpn.h"
#include "nv.h"
#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */
#ifdef FEATURE_UIM_RUIM
#include "uim.h"
#endif /* FEATURE_UIM_RUIM */

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#ifdef FEATURE_AUTH

#define AUTH_A_KEY_DIGITS         26
  /* Number of digits in the A-key */
  
#ifdef FEATURE_UIM_RUIM

#define RUIM_RUN_CAVE_DIGITS      3
#define RUIM_GLOBAL_RAND_CHAL     0x00
#define RUIM_UNIQUE_RAND_CHAL     0x01

  /* Process control bits */     
#define RUIM_DEFLT_PROC_CNTRL     0x00
#define RUIM_SAVE_REGS_ON         0x10
#define RUIM_COMMIT_NEW_SSD       0x04

#ifdef  FEATURE_UIM_RUIM_VPM_OFFSET_ONE
#error code not present
#else
#define RUIM_FIRST_OCTET_OF_VPM   0
#define RUIM_LAST_OCTET_OF_VPM    64    /* 520 bits of VPM */   
#endif /* FEATURE_UIM_RUIM_VPM_OFFSET_ONE */

#define RUIM_CC_MASK              0x3F
#define RUIM_CC_SHIFT             8
#endif /* FEATURE_UIM_RUIM */

/*--------------------------------------------------------------------------
                             Signal Masks
--------------------------------------------------------------------------*/
#define  AUTH_RPT_TIMER_SIG      0x0001
  /* This signal is set when the auth_rpt_timer expires.
     This timer is used to enable AUTH to kick the watchdog on time */

#define  AUTH_CMD_Q_SIG          0x0002
  /* This signal is set when an item is put on the auth_cmd_q. */
#ifdef FEATURE_AUTH
#define  AUTH_SSD_TIMER_SIG      0x0004
  /* This signal is set when the SSD timer expires */
#endif /* FEATURE_AUTH */

#define  AUTH_NV_CMD_SIG         0x0008
  /* This signal is set when NV completes a command */
  
#ifdef FEATURE_UIM_RUIM
#define  RUIM_CMD_RSP_SIG        0x0010
  /* This signal is set when RUIM is done processing a command */
  
#define  AUTH_UIM_CMD_RSP_SIG    0x0020
  /* This signal is set when UIM is done processing a command */
  
#endif /* FEATURE_UIM_RUIM */


/*--------------------------------------------------------------------------
                         AUTHENTICATION REPORT

--------------------------------------------------------------------------*/
/* -------------------------------------------------------------------
** The Authentication Task optionally sends a report when it completes
** a command which produces a result which would be useful to the
** task which issued the command.
** ------------------------------------------------------------------- */
typedef enum
{
#ifdef FEATURE_AUTH
  AUTH_SIGNATURE_R,     /* Authentication Signature report */
  AUTH_A_KEY_R,         /* Whether A key was updated OK */
  AUTH_FINISH_SSD_R,    /* Whether SSD update completed OK */
  AUTH_GENERATE_KEY_R,  /* Indicates the key generation has completed */
  AUTH_RESYNC_R,         /* Verify AUTH has reset itself */
  AUTH_UPDATE_A_KEY_TEMP_R,/* Verfify that A_KEY_TEMP has been updated */
  AUTH_COMMIT_KEYS_R,
#ifdef FEATURE_OTASP_OTAPA
  AUTH_SIGNATURE_SPASM_R, /* Verify auth_otapa for SPASM has completed */
#endif /* FEATURE_OTASP_OTAPA */
#ifdef FEATURE_UIM_RUIM
  AUTH_BS_CHAL_R,       /* Return the RANDBS */
#endif /* FEATURE_UIM_RUIM */
#endif /* FEATURE_AUTH */
#ifdef FEATURE_UIM_RUIM
  AUTH_ACCESS_RUIM_R,   /* Access to the R-UIM                         */
  AUTH_RUIM_STATUS_R,   /* Status of the card */
#endif /* FEATURE_UIM_RUIM */
  AUTH_MAX_R
} auth_report_code_type;

#ifdef FEATURE_UIM_RUIM
#define RUIM_FAIL UIM_FAIL
#define RUIM_PASS UIM_PASS
typedef uim_rpt_status ruim_rpt_status;
#endif /* FEATURE_UIM_RUIM */

typedef struct
{
  cmd_hdr_type         rpt_hdr;
    /* Includes the queue links,  pointer to requesting task TCB,
       signal to set on completion,  and done queue. */
  auth_report_code_type rpt_type; /* What type of report this is */
#ifdef FEATURE_UIM_RUIM
  uim_rpt_status       rpt_status;      /* whether the access was OK */
#endif /* FEATURE_UIM_RUIM */
  union
  {
#ifdef FEATURE_AUTH
    dword              auth;           /* auth_signature (18 bits) */
    boolean            ssd_update_ok;  /* Whether SSD update completed OK */
    boolean            a_key_update_ok;/* Whether A key was updated OK */
    boolean            commit_keys_ok; /* Whether the keys committed were OK */
#ifdef FEATURE_UIM_RUIM
    dword              randbs;         /* RANDBS from BS challenge */
#endif /* FEATURE_UIM_RUIM */
#endif /* FEATURE_AUTH */
  } rpt;
}
auth_rpt_type;

/*--------------------------------------------------------------------------
                    AUTHENTICATION COMMANDS

--------------------------------------------------------------------------*/

/* Status values for a command */
typedef enum {
  AUTH_RECEIVED_S,                  /* Command received */
  AUTH_INVALID_S,                   /* Invalid command */
  AUTH_ERR_S,                       /* An error occured */
  AUTH_UNINITIALIZED_S,             /* Authentication is not initialized */
  AUTH_MAXIMUM_S                    /* always last, this counts statuses */
} auth_status_type;

/* Command types */
typedef enum {
#ifdef FEATURE_AUTH
  AUTH_INIT_F,                      /* Initialize on powerup or NAM change */
  AUTH_SSD_UPDATE_F,                /* Begin process to update SSD */
  AUTH_FINISH_SSD_UPDATE_F,         /* Finish process to update SSD */
  AUTH_SIGNATURE_F,                 /* Perform auth signature */
  AUTH_GENERATE_KEY_F,              /* Generate CMEA key and VPM */
  AUTH_UPDATE_A_KEY_F,              /* Update the A-key */
  AUTH_INC_COUNT_F,                 /* Increment count value */
  AUTH_START_TIMER_F,               /* Starts SSD Timer */
  AUTH_RESYNC_F,                    /* Re-synchronizes the command queue */
  AUTH_RESET_F,                     /* Reset Auth Task state */
  AUTH_UPDATE_A_KEY_TEMP_F,         /* Update the A-key */
  AUTH_COMMIT_KEYS_F,               /* Commit the key */
#ifdef FEATURE_OTASP_OTAPA
  AUTH_SIGNATURE_SPASM_F,           /* Perform auth signature for SPASM */
#endif /* FEATURE_OTASP_OTAPA */
#ifdef FEATURE_UIM_RUIM
  AUTH_BS_CHAL_F,                   /* Do a Base station Challenge */
#endif /* FEATURE_UIM_RUIM */
#endif /* FEATURE_AUTH */
#ifdef FEATURE_UIM_RUIM
  AUTH_ACCESS_RUIM_F,               /* Read or Write from RUIM */
  AUTH_RUIM_STATUS_F,               /* Status of the Card */
#endif /* FEATURE_UIM_RUIM */
  AUTH_MAX_F
} auth_cmd_name_type;

#ifdef FEATURE_UIM_RUIM 
#define RUIM_READ_F UIM_READ_F         /* Read item from RUIM */
#define RUIM_WRITE_F UIM_WRITE_F       /* Write item to RUIM */
#define RUIM_INC_F UIM_INC_F           /* Increment item in RUIM */
typedef uim_access_type ruim_access_type;

/* Mobile Directory Number */
#define  RUIM_MSISDN UIM_TELECOM_MSISDN
/* Call Count */
#define  RUIM_CC UIM_CDMA_CC              
/* IMSI_M */
#define  RUIM_IMSI_M UIM_CDMA_IMSI_M      
/* IMSI_T */
#define  RUIM_IMSI_T UIM_CDMA_IMSI_T      
/* TMSI */
#define  RUIM_TMSI UIM_CDMA_TMSI          
/* Analog Home SID */
#define  RUIM_ANALOG_HOME_SID UIM_CDMA_ANALOG_HOME_SID  
/* Analog Operational Parameters */
#define  RUIM_ANALOG_OP_PARAMS UIM_CDMA_ANALOG_OP_PARAMS
/* Analog Locn and Regn Indicators */
#define  RUIM_ANALOG_LOCN_AND_REGN_IND UIM_CDMA_ANALOG_LOCN_AND_REGN_IND
/* CDMA Home SID and NID */
#define  RUIM_CDMA_HOME_SID_NID UIM_CDMA_HOME_SID_NID 
/* CDMA Zone Based Regn Indicators */
#define  RUIM_CDMA_ZONE_BASED_REGN_IND UIM_CDMA_ZONE_BASED_REGN_IND
/* CDMA System/Network Regn Indicators */
#define  RUIM_CDMA_SYS_REGN_IND UIM_CDMA_SYS_REGN_IND
/* CDMA Distance Based Regn Indicators */
#define  RUIM_CDMA_DIST_BASED_REGN_IND UIM_CDMA_DIST_BASED_REGN_IND
/* Access Overload Class */
#define  RUIM_ACCOLC UIM_CDMA_ACCOLC
/* Call Termination Mode Preferences */
#define  RUIM_CALL_TERM_MODE_PREF UIM_CDMA_CALL_TERM_MODE_PREF
/* Suggested Slot Cycle Index */
#define  RUIM_SCI UIM_CDMA_SCI
/* Analog Channel Preferences */
#define  RUIM_ANALOG_CHAN_PREF UIM_CDMA_ANALOG_CHAN_PREF
/* Preferred Roaming List */
#define  RUIM_PRL UIM_CDMA_PRL
/* Removable UIM ID */
#define  RUIM_RUIM_ID UIM_CDMA_RUIM_ID
/* CDMA Service Table */
#define  RUIM_CDMA_SVC_TABLE UIM_CDMA_CDMA_SVC_TABLE
/* Service Programming Code */
#define  RUIM_SPC UIM_CDMA_SPC
/* OTAPA/SPC-Enable */
#define  RUIM_OTAPA_SPC_ENABLE UIM_CDMA_OTAPA_SPC_ENABLE
/* NAM-Lock */
#define  RUIM_NAM_LOCK UIM_CDMA_NAM_LOCK
/* OTASP-OTAPA Parameters */
#define  RUIM_OTASP_OTAPA_FEATURES UIM_CDMA_OTASP_OTAPA_FEATURES
/* Service Preferences */
#define  RUIM_SERVICE_PREF UIM_CDMA_SERVICE_PREF
/* ESN_ME */
#define  RUIM_ESN_ME UIM_CDMA_ESN_ME
/* R-UIM Phase */
#define  RUIM_RUIM_PHASE UIM_CDMA_RUIM_PHASE
/* Preferred Languages */
#define  RUIM_PREF_LANG UIM_CDMA_PREF_LANG
/* Not assigned yet */
#define  RUIM_UNASSIGNED_1 UIM_CDMA_UNASSIGNED_1
/* Short Messages */
#define  RUIM_SMS UIM_CDMA_SMS
/* Short Message Service Parameters */
#define  RUIM_SMS_PARAMS UIM_CDMA_SMS_PARAMS
/* SMS Status */
#define  RUIM_SMS_STATUS UIM_CDMA_SMS_STATUS
/* Sup Svcs Feature Code Table */
#define  RUIM_SUP_SVCS_FETURE_CODE_TABLE UIM_CDMA_SUP_SVCS_FETURE_CODE_TABLE
/* Not assigned yet */
#define  RUIM_UNASSIGNED_2 UIM_CDMA_UNASSIGNED_2
/* CDMA Home Service Provider Name */
#define  RUIM_CDMA_HOME_SVC_PVDR_NAME UIM_CDMA_HOME_SVC_PVDR_NAME
/* R-UIM ID usage indicator */
#define  RUIM_UIM_ID_USAGE_IND UIM_CDMA_UIM_ID_USAGE_IND
/* Not assigned yet */
#define  RUIM_UNASSIGNED_3 UIM_CDMA_UNASSIGNED_3
/* Mobile Directory Number */
#define  RUIM_MDN UIM_CDMA_MSISDN
/* Phonebook from Telecom directory */
#define  RUIM_ADN UIM_TELECOM_ADN
/* Administrative Data */
#define  RUIM_ADM_DATA UIM_CDMA_ADM_DATA

typedef uim_items_enum_type ruim_items_enum_type;

#define  RUIM_INVALID_S AUTH_INVALID_S   /* Recd the command in an invalid State */ 
#define  RUIM_UNINITIALIZED_S AUTH_UNINITIALIZED_S /* RUIM is not initialized yet */               
#define  RUIM_POWERED_DOWN_S AUTH_ERR_S  /* RUIM is powered down */                           
#define  RUIM_INITIALIZED_S AUTH_ERR_S   /* RUIM is initialized */                   
#define  RUIM_ERR_S AUTH_ERR_S           /* RUIM is faulty */                
#define  RUIM_MAXIMUM_S

typedef auth_status_type auth_ruim_status_type;

/* Indicate command handling options */
typedef enum {
  RUIM_OPTION_NONE       = 0,        /* No options used */
  RUIM_OPTION_ALWAYS_RPT = 1         /* Always report   */
} auth_ruim_option_type;

#endif /* FEATURE_UIM_RUIM */

/* The common Header for all the commands */
typedef struct {
  cmd_hdr_type          cmd_hdr;       /* command header */
  auth_cmd_name_type    command;       /* The command */
  auth_status_type      status;        /* Space for message status */
#if defined( FEATURE_UIM_RUIM )
  auth_ruim_option_type options;       /* Indicate command handling options */
  byte                  cmd_count;     /* Number of Attempts  */
#endif /* FEATURE_UIM_RUIM */
} auth_hdr_type;

#ifdef FEATURE_AUTH
/* Initialize on powerup or NAM change */
typedef struct {                  /* AUTH_INIT_F */
  auth_hdr_type   hdr;            /* Command header */
  byte            nam;            /* Which NAM info to initialize with */
} auth_init_type;

/* Begin process to update SSD */
typedef struct {                  /* AUTH_SSD_UPDATE_F */
  auth_hdr_type   hdr;            /* Command header */
  qword           randssd;        /* 56 bits from BS for SSD Generation */
#ifdef FEATURE_UIM_RUIM
  byte            process_control;/* process control bits */
  dword           esn;            /* ESN ( 7 bytes per standard ) */
#endif /* FEATURE_UIM_RUIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
  dword           randbs;         /* 32 random bits for Auth Signature */
  dword           auth_data;      /* 24 data bits for Auth Signature */
#endif /* FEATURE_UIM_RUIM */
} auth_ssd_update_type;

/* Finish process to update SSD */
typedef struct {                  /* AUTH_FINISH_SSD_UPDATE_F */
  auth_hdr_type   hdr;            /* Command header */
  dword           authbs;         /* Auth Signature from Base Station */
  void            (*rpt_function)( auth_rpt_type * );
                                  /* Pointer to Function to send report */
} auth_finish_ssd_update_type;

/* Perform auth signature */
typedef struct {                  /* AUTH_SIGNATURE_F */
  auth_hdr_type   hdr;            /* Command header */
#ifdef FEATURE_UIM_RUIM
  byte            rand_type;      /* Rand/Randu     */
  byte            dig_len;        /* digit length expressed in bits */
  byte            digits[RUIM_RUN_CAVE_DIGITS];      
                                  /* maximum of 6 BCD coded digits */
  byte            process_control;/* process control bits */
  dword           esn;            /* ESN ( 7 bytes per standard ) */
#endif /* FEATURE_UIM_RUIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
  dword           auth_data;      /* 24 data bits for Auth Signature */
  boolean         save_reg;       /* TRUE if registers are to be saved */
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */
  dword           rand_chal;      /* 32 bits from BS for Auth Signature */
  void            (*rpt_function)( auth_rpt_type * );
                                  /* Pointer to Function to send report */
} auth_signature_type;

/* Generate CMEA key and VPM */
typedef struct {                  /* AUTH_GENERATE_KEY_F */
  auth_hdr_type   hdr;            /* Command header */
#ifdef FEATURE_UIM_RUIM
  byte            vpm_first_octet;/* First octet of VPM to be output */
  byte            vpm_last_octet; /* Last  octet of VPM to be output */
#endif /* FEATURE_UIM_RUIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
  boolean         do_vpm;         /* TRUE if Voice Privacy Mask is needed */
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */
  void            (*rpt_function)( auth_rpt_type * );
                                  /* Pointer to Function to send report */
} auth_generate_key_type;

/* Update the A-key */
typedef struct {                  /* AUTH_UPDATE_A_KEY_F */
  auth_hdr_type   hdr;            /* Command header */
  byte            a_key[AUTH_A_KEY_DIGITS]; /* New A-key to be used */
  byte            nam;            /* NAM for which the A-key is intended */
  void            (*rpt_function)( auth_rpt_type * );
                                  /* Pointer to Function to send report */
} auth_update_a_key_type;

/* Increment count value */
typedef struct {                  /* AUTH_START_TIMER_F */
  auth_hdr_type   hdr;            /* Command header */
} auth_inc_count_type;

/* Starts SSD Timer */
typedef struct {                  /* AUTH_START_TIMER_F */
  auth_hdr_type   hdr;            /* Command header */
} auth_start_timer_type;

typedef struct {                  /* AUTH_RESYNC_F */
  auth_hdr_type   hdr;            /* Command header */
  void            (*rpt_function)( auth_rpt_type * );
                                  /* Pointer to Function to send report */
} auth_resync_type;

typedef struct {                  /* AUTH_RESET_F */
  auth_hdr_type   hdr;            /* Command header */
} auth_reset_type;

typedef struct {                  /* AUTH_COMMIT_KEYS_F */
  auth_hdr_type   hdr;            /* Command header */
  byte            nam_index;       /* the nam to commit the a_key into */
  void            (*rpt_function)( auth_rpt_type * );
                                  /* Pointer to Function to send report */
} auth_commit_keys_type;

typedef struct {                  /* AUTH_UPDATE_A_KEY_TEMP_F */
  auth_hdr_type   hdr;            /* Command header */
  qword           a_key;/* a_key_temp */
  void            (*rpt_function)( auth_rpt_type * );
                                  /* Pointer to Function to send report */
} auth_update_a_key_temp_type;

typedef struct {                  /* AUTH_BS_CHAL_F */
  auth_hdr_type   hdr;            /* Command header */
  dword           randseed;          /* Random number generator seed */
  void            (*rpt_function)( auth_rpt_type * );
                                  /* Pointer to Function to send report */
} auth_bs_chal_type;
#endif /* FEATURE_AUTH */

#ifdef FEATURE_UIM_RUIM

typedef struct {
  auth_hdr_type   hdr;            /* Command header */
  ruim_items_enum_type item;      /* Item to be accessed */
  byte            nam;            /* NAM info */
  ruim_access_type access;        /* Type of access, Read or Write */ 
  word            num_bytes;      /* Number of bytes to be read or written */
  word            num_bytes_rsp;  /* Number of bytes returned */
  word            num_records_rsp;/* Number of records in the response */
  word            offset;         /* Offset into the EF */
  void            *data_ptr;      /* Pointer for data */
  void            (*rpt_function)( auth_rpt_type * );
                                  /* Pointer to Function to send report */
} auth_access_ruim_type;

typedef struct {
  auth_hdr_type   hdr;            /* Command header */
  void            (*rpt_function)( auth_rpt_type * );
                                  /* Pointer to Function to send report */
} auth_ruim_status_cmd_type;

typedef struct {
  auth_hdr_type   hdr;            /* Command header */
  byte            len_and_usage;  /* Length and usage of ESN */
  dword           esn;            /* ESN */
  void            (*rpt_function)( auth_rpt_type * );
                                  /* Pointer to Function to send report */
} auth_store_esn_type;

#endif /* FEATURE_UIM_RUIM */

typedef union {
  auth_hdr_type                hdr;     /* Generic header */
#ifdef FEATURE_AUTH
  auth_init_type               init;    /* Init auth variables */
  auth_ssd_update_type         ssd;     /* SSD Update*/
  auth_finish_ssd_update_type  f_ssd;   /* Finish SSD Update*/
  auth_signature_type          sig;     /* Authentication Signature */
  auth_generate_key_type       key;     /* Generate CMEA key and VPM */
  auth_update_a_key_type       a_key;   /* Update the A-key */
  auth_start_timer_type        start;   /* Start SSD timer */
  auth_inc_count_type          inc;     /* Increment COUNTsp */
  auth_resync_type             resync;  /* Re-syncronize command queue */
  auth_reset_type              reset;   /* Reset AUTH task state */
  auth_commit_keys_type        commit_keys;  /* commit the keys  */
  auth_update_a_key_temp_type  temp;    /* Update the a_key_temp */
  auth_bs_chal_type            chal;    /* Base station challenge type */
#endif /* FEATURE_AUTH */
#ifdef FEATURE_UIM_RUIM
  auth_access_ruim_type        access_ruim; /* Read from or Write to RUIM */
  auth_ruim_status_cmd_type    status;      /* Status of the Card */
#endif /* FEATURE_UIM_RUIM */
} auth_cmd_type;

/* Queue for other tasks to get auth command buffers from */
extern q_type auth_free_q;

#ifdef FEATURE_AUTH
/* Storage for A_KEY_TEMP */
extern qword a_key_temp;

/* to indicate if new A-Key has been generated */
extern boolean a_key_temp_valid;

/* to inidcate if the new ssd is valid */
extern boolean new_ssd_valid;

#endif /* FEATURE_AUTH */

#if defined (FEATURE_AUTH) && (!defined(FEATURE_UIM_RUIM) || \
    defined (FEATURE_UIM_RUN_TIME_ENABLE))
/*===========================================================================

FUNCTION AUTH_VALIDATE_A_KEY

DESCRIPTION
  This procedure tests if the given A key is valid.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the A key is valid
  FALSE if the A key is invalid

SIDE EFFECTS
  None

===========================================================================*/
extern boolean auth_validate_a_key
(
  byte a_key[AUTH_A_KEY_DIGITS]
    /* The A key to be validated */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION auth_validate_a_key */
/*~ PARAM a_key ARRAY AUTH_A_KEY_DIGITS */
#endif /* FEATURE_HTORPC_METACOMMENTS */

#endif /* FEATURE_AUTH && (!FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

#ifdef FEATURE_AUTH
/*===========================================================================

FUNCTION AUTH_PRIVATE_LCM_AVAILABLE

DESCRIPTION
  This procedure checks the private lcm to make sure it can be activated.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the new lcm is available, FALSE othewise.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean auth_private_lcm_available( void );

/*===========================================================================

FUNCTION AUTH_GET_COUNT

DESCRIPTION
  This procedure retrieves the current value of COUNTsp.

DEPENDENCIES
  None

RETURN VALUE
  Returns COUNTsp.

SIDE EFFECTS
  None

===========================================================================*/
extern byte auth_get_count( void );
#endif /* FEATURE_AUTH */

#ifdef FEATURE_UIM_RUIM
#if defined (FEATURE_AUTH) && defined (FEATURE_AUTH_DIGITS)
/*===========================================================================

FUNCTION AUTH_CALCULATE_DIGITS

DESCRIPTION 
  This function packs the dialed digits that need to be sent to the RUIM. If
  the number of digits is less than 6, the leading digits are zero padded.
  A max of 6 digits are packed in a 3 byte array.

DEPENDENCIES
  The digits passed in must be in a byte array and be encoded in BCD.

RETURN VALUE
  The computed digits

SIDE EFFECTS
  None

===========================================================================*/
void auth_calculate_digits
(
  byte digits[],
    /* Dialed digits or destination address digits */
  byte array_len,
    /* The number of digits in the above array */
  boolean digit_mode,
    /* TRUE if digits are ASCII, FALSE if digits are DTMF */
  byte output_digits[]
    /* output array */
);
#endif /* defined (FEATURE_AUTH) && defined (FEATURE_AUTH_DIGITS) */
#endif /* FEATURE_UIM_RUIM */

#ifdef FEATURE_AUTH
/*===========================================================================

FUNCTION AUTH_GET_PRIVATE_LCM

DESCRIPTION
  This procedure returns the private long code mask. 
 
DEPENDENCIES
  The function auth_check_private_lcm needs to be called first to ensure
  a valid long code mask is available.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void auth_get_private_lcm ( qword qplcm );

/*===========================================================================

FUNCTION AUTH_ENCRYPTION_POSSIBLE

DESCRIPTION
  This procedure determines if message encryption is possible.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if auth could successfully encrypt messages.
  FALSE if auth is not initialized properly to do encryption.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean auth_encryption_possible( void );

/*===========================================================================

FUNCTION AUTH_ENCRYPT_CDMA

DESCRIPTION
  This procedure encrypts CDMA reverse traffic channel messages.
  If any fields require encryption they are encrypted.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The num_digits, num_fields, record_len, etc. fields may not accurately
  represent the size of the field(s) if bits are added for encryption.

===========================================================================*/
extern void auth_encrypt_cdma
(
  word *len_ptr,
    /* Length of message in bits */
  byte *buf_ptr,
    /* Pointer to body of already packed traffic channel message */
  caii_tx_msg_type *int_ptr
    /* Pointer to internal message to be transmitted */
);

/*===========================================================================

FUNCTION AUTH_DECRYPT_CDMA

DESCRIPTION
  This procedure decrypts CDMA reverse traffic channel messages.
  If any fields require decryption they are decrypted.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if there are no problems encountered.
  FALSE if fields of the given message that should be decrypted are not.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean auth_decrypt_cdma
(
  word *len_ptr,
    /* Length of message in bits */
  byte *buf_ptr
    /* Pointer to received message to be translated */
);

#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

/*===========================================================================

FUNCTION AUTH_ENCRYPT_INFO_RECS

DESCRIPTION
  This procedure encrypts type specific fields in reverse traffic channel 
  message.  To accomplish this I first make a temporary
  copy of the packed message body.  Then I unpack each set of type
  specific fields from the copy, encrypt them, and copy them back
  into the original message to be transmitted.  If a record_len is
  only one byte, another byte is added.  Once the message to be
  transmitted is lengthened it is necessary to copy all remaining
  fields (still encrypting the type specific fields, of course).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The length of Traffic Channel message to transmit may be increased.

===========================================================================*/
void auth_encrypt_info_recs
(
  word *len_ptr,
    /* Length of message in bits */
  byte *buf_ptr,
    /* Pointer to body of packed external-format message */
  caii_tx_msg_type *int_ptr,
    /* Pointer to internal message to be transmitted */
  word rec_start_pos,
    /* Start position of the info record after num_rec field */
  byte num_recs
    /* Number of info records to be processed */
);

/*===========================================================================

FUNCTION AUTH_ENCRYPT_ORIG_C

DESCRIPTION
  This procedure encrypts CDMA reverse traffic channel origination
  continuation message.  I copy the packed digit data into a temporary
  buffer, encrypt it, and then put it back.

DEPENDENCIES
  The Origination Continuation Message must have at least one digit in it.

RETURN VALUE
  None

SIDE EFFECTS
  The length of Traffic Channel message to transmit may be increased.

===========================================================================*/
void auth_encrypt_orig_c
(
  word *len_ptr,
    /* Length of message in bits */
  byte *buf_ptr,
    /* Pointer to body of packed external-format message */
  caii_tx_msg_type *int_ptr
    /* Pointer to internal message to be transmitted */
);

#ifdef FEATURE_IS2000_REL_A
/*===========================================================================

FUNCTION AUTH_ENCRYPT_EFWIM

DESCRIPTION
  This procedure encrypts CDMA reverse traffic channel Extended Flash with
  Information message.  To accomplish this I first make a temporary
  copy of the packed message body.  Then I unpack each set of type
  specific fields from the copy, encrypt them, and copy them back
  into the original message to be transmitted.  If a record_len is
  only one byte, another byte is added.  Once the message to be
  transmitted is lengthened it is necessary to copy all remaining
  fields (still encrypting the type specific fields, of course).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The length of Traffic Channel message to transmit may be increased.

===========================================================================*/
void auth_encrypt_efwim
(
  word *len_ptr,
    /* Length of message in bits */
  byte *buf_ptr,
    /* Pointer to body of packed external-format message */
  caii_tx_msg_type *int_ptr
    /* Pointer to internal message to be transmitted */
);

/*===========================================================================

FUNCTION AUTH_ENCRYPT_EOM_DIGITS

DESCRIPTION
  This procedure encrypts CDMA reverse traffic channel enhanced origination
  message.  I copy the packed digit data into a temporary
  buffer, encrypt it, and then put it back.

DEPENDENCIES
  The Enhanced Origination Message must have at least one digit in it.

RETURN VALUE
  None

SIDE EFFECTS
  The length of Traffic Channel message to transmit may be increased.

===========================================================================*/
void auth_encrypt_eom_digits
(
  word *len_ptr,
    /* Length of message in bits */
  byte *buf_ptr,
    /* Pointer to body of packed external-format message */
  caii_tx_msg_type *int_ptr,
    /* Pointer to internal message to be transmitted */
  word digit_start_pos
    /* Start location of digits */
);

/*===========================================================================

FUNCTION AUTH_DECRYPT_EXTENDED_INFO

DESCRIPTION
  This procedure decrypts CDMA forward traffic channel Extended Flash with
  Information and Extended Alert with information messages.  To accomplish
  this I unpack each set of type specific fields, encrypt them,
  and copy them back into the original received packed message.
  If a record_len is only one byte, the extra byte added for
  encryption is removed and the overall length is decreased.
  Once the received packed message is shortened it is necessary
  to copy all remaining fields (still decrypting the type specific
  fields, of course).
  
DEPENDENCIES
  None

RETURN VALUE
  TURE if successful. Otherwise return FALSE.

SIDE EFFECTS
  The length of Traffic Channel message may be increased.

===========================================================================*/
boolean auth_decrypt_extended_info
(
  word *len_ptr,
    /* Length of message in bits */
  byte *buf_ptr,
    /* Pointer to body of packed external-format message */
  word start_pos,
    /* Start point of Information Record */
  byte num_recs
    /* Numer of records in this message */
);
#endif /* FEATURE_IS2000_REL_A */

#endif /* FEATURE_AUTH */

#if defined(FEATURE_UIM_RUIM) || defined (FEATURE_AUTH)
/*===========================================================================

FUNCTION AUTH_TASK

DESCRIPTION
  This procedure is the entrance procedure for the Authentication task.

DEPENDENCIES
  None

RETURN VALUE
  Does not return.

SIDE EFFECTS
  None

===========================================================================*/

extern void auth_task (
  dword dummy
    /* Parameter required for REX.  Tell lint to ignore it. */
    /*lint -esym(715,dummy) */
);

/*===========================================================================

FUNCTION AUTH_CMD

DESCRIPTION
  The auth_cmd pointed to by the parameter passed in is queued up for AUTH.

DEPENDENCIES
  None.
RETURN VALUE
  None.
SIDE EFFECTS
  None.
===========================================================================*/
extern void auth_cmd (
  auth_cmd_type *cmd_ptr                    /* the command to queue up */
);

/*===========================================================================

FUNCTION AUTH_SEND_UPDATE_A_KEY_CMD

DESCRIPTION
  Gets an available auth_cmd_type from the auth_free_q and constructs
  an update A_KEY cmd to AUTH task. This is used by the SVCPRG module.
  This function is remoted for use by the SVCPRG module in some targets.

DEPENDENCIES
  None.
  
RETURN VALUE
  True if auth cmd was successfully queued, False otherwise
  
SIDE EFFECTS
  Queue signal is set for the AUTH task
===========================================================================*/

extern boolean auth_send_update_a_key_cmd 
(
  byte * a_key,
  int num_a_key_digits,
  int nam
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION auth_send_update_a_key_cmd */
/*~ PARAM a_key VARRAY LENGTH num_a_key_digits */
#endif /* FEATURE_HTORPC_METACOMMENTS */

#endif /* FEATURE_UIM_RUIM || FEATURE_AUTH */

#endif /* FEATURE_AUTH */

#endif /* AUTH_H */

