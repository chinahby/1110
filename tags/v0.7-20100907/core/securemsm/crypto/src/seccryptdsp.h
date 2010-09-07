#ifndef SECCRYPTDSP_H
#define SECCRYPTDSP_H
/*===========================================================================

                      S E C U R I T Y    S E R V I C E S
      A R M - Q D S P 2    C R Y P T O G R A P H I C    I N T E R F A C E
                   I N T E R N A L    H E A D E R    F I L E

DESCRIPTION
  This is the internal header file for the ARM-QDSP2 Cryptographic
  Interface Module. It defines the internal and low-level data types
  and structures for key exchange, message digest, and encrypt/decrypt
  for various algorithms. This is meant to be used internally by the
  crypto layer module.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  seccrypt_init() has to be called before using any of the other routines.

 Copyright (c) 2000-2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
  $PVCSPath: L:/src/asw/COMMON/vcs/seccryptdsp.h_v   1.2   22 Oct 2003 11:58:44   omichael  $
  $Header: //source/qcom/qct/core/securemsm/crypto/main/latest/src/seccryptdsp.h#3 $ 
  $DateTime: 2007/10/04 09:03:46 $ $Author: jmiera $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/18/07   avm     reduced SECCRYPTDSP_DISABLE_WAIT_TIME to 100 ms
06/07/06   df      increased the dme table size for 6800/7500
08/31/05   df      Changes to support hipri/devkey
09/21/04   df      numerous changes to support dsp-arm failover
05/05/04   rwh     Added #error for image-based SVD combination.
                   Added enum values for module-based crypto DSP capability.
02/28/04   rwh     Added SVD support; added QDSP Services module support.
                   Changed _send_cmd() boolean param to enum for readability.
02/19/04   rwh     Modified SEC_DSM_DS_SMALL_ITEM_SIZ for 6250.
01/06/03   jay     Changed signature of issue_cmd
12/02/03   jay     Added externs for DME tables.
11/11/03   jay     Added states to track the DSP state
12/11/00   mvd     Created this file, copied contents from "crypif.c"


===========================================================================*/


#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */

#ifdef FEATURE_SEC
#ifdef FEATURE_SEC_DSP_CRYPTO

#include "rex.h"
#include "dsm.h"
#include "clk.h"
#include "qdsp.h"

#include "seccryptdspdefs.h"
#include "seccrypt.h"
#include "secerrno.h"

/* for 7500 which only supports pkx in dsp */
#if defined FEATURE_MULTIPROCESSOR && defined IMAGE_MODEM_PROC
#error code not present
#endif


/*===========================================================================

                         CONSTANTS AND DEFINITIONS

===========================================================================*/

#if defined( FEATURE_QDSP_USES_MODULES)

/* Map the module-based function i/f to the image-based one. */
#define qdsp_allow_reset(x)   qdsp_disable_event_rsp( x)

#else  /* FEATURE_QDSP_USES_MODULES */

#endif /* FEATURE_QDSP_USES_MODULES */


/* Task context and wait function for DSP related signals */
#define SECCRYPTDSP_TCB_PTR         &sec_tcb
#define SECCRYPTDSP_WAIT            rex_wait

/* DSP specific signals */
#define SECCRYPTDSP_OP_COMPLETE_SIG SEC_DSP_OP_COMPLETE_SIG
#define SECCRYPTDSP_INLINE_CMD_SIG  SEC_DSP_INLINE_CMD_SIG

/* We need to guess the size of the small DSM items at compile time, 
   because this value is used to estimate storage allocations.  
   There is no problem if the items are larger than this "guestimate".

    For 6800/HDR later builds (4.2, 5.x) it has been our experience
    that the "used" size is way smaller than the item size;  So, now
    we will assume the smallest possible size, even though it might
    actually be bigger.

   The following info is derived from dsm.c:

   The smallest dsm size is 36 bytes (FEATURE_DSM_MINIMIZE)
                            48 bytes (FEATURE_WCDMA)
*/

/* we have to make some adjustment for wcdma -
 * the actual payload is 40 bytes, and in some cases less
 * it averages out to be a bit over 38, so if we use 36 
 *  as above, it should be fine
 */

#define SEC_DSM_DS_SMALL_ITEM_SIZ  36

/* Right now the DME table size is determined by the MAX SSL record len
   and the size of the small DSM item payload i.e
   SSL_V3_MAX_RECORD_LENGTH/ DSM_DS_SMALL_ITEM_SIZ  plus some extra.
   Since these values can vary  we can only estimate here.
   16384 /80 + 20
    
   Also we need more for input DME table than output since we do 
   cipher-in-place.
   */
#define SECCRYPTDSP_INPUT_DME_TABLE_SIZE \
   (((16384 + 20) / SEC_DSM_DS_SMALL_ITEM_SIZ) + 20)
   

#define SECCRYPTDSP_OUTPUT_DME_TABLE_SIZE  10

/* Time to wait before disabling the DSP [ms] */
#ifdef FEATURE_SEC_DSP_SHORT_TIMEOUT
#define SECCRYPTDSP_DISABLE_WAIT_TIME  100
#else
  #define SECCRYPTDSP_DISABLE_WAIT_TIME  1100
#endif

/* temporary feature */
#define SECCRYPTDSP_REQ_TIMEOUT 200
#define SECCRYPTDSP_RETRY_TIMEOUT 1000
#define SECCRYPTDSP_RESET_PENDING_TIMEOUT 20  /* was 350 */
#define SECCRYPTDSP_SYNC_EXEC_TIMEOUT 20

/* RTOS API defines */
#ifdef SECCRYPTDSP_RTOS_API
#define SECCRYPTDSP_RTOS_MSG_ERROR_LEN 1
#define SECCRYPTDSP_RTOS_MSG_CFG_DONE_LEN 0
#define SECCRYPTDSP_RTOS_MSG_MODMATH_DONE_LEN 1
#define SECCRYPTDSP_RTOS_MODMATH_CMD_CFG_LEN 7
#define SECCRYPTDSP_RTOS_MODMATH_CFG_CMD_BUF_SIZE (SECCRYPTDSP_RTOS_MODMATH_CMD_CFG_LEN * sizeof(uint16))
#define SECCRYPTDSP_RTOS_MODMATH_CMD_DO_LEN 4
#define SECCRYPTDSP_RTOS_MODMATH_DO_CMD_BUF_SIZE (SECCRYPTDSP_RTOS_MODMATH_CMD_DO_LEN * sizeof(uint16))
#define SECCRYPTDSP_RTOS_MODMATH_PARTITION_ID 0
#endif


/*===========================================================================

                           TYPE DECLARATIONS

===========================================================================*/

typedef enum
{
  SECCRYPTDSP_STANDALONE_IMAGE_MAP = 0,
  SECCRYPTDSP_SVD_IMAGE_MAP        = 1,
  SECCRYPTDSP_NUM_IMAGE_MAPS
} seccryptdsp_image_map_enum_type;

typedef enum
{
  SECCRYPTDSP_NO_IMAGE       = 0,
  SECCRYPTDSP_NO_MODULE      = 0,

#if defined( FEATURE_QDSP_USES_MODULES)

  #ifdef SECCRYPTDSP_RTOS_API
  SECCRYPTDSP_CRYPTO_MODULE_MODMATH = QDSP_IMAGE_MODMATH,
  #else
  SECCRYPTDSP_CRYPTO_MODULE_SVD = QDSP_MODULE_CRYPTO, /*qdsp_module_type */
  SECCRYPTDSP_CRYPTO_MODULE_STANDALONE = QDSP_MODULE_CRYPTO_STANDALONE,
  #endif

#else  /* FEATURE_QDSP_USES_MODULES */

  #ifdef FEATURE_SEC_SVD_DSP_IMAGE
  
    #error Must use module-based QDSP Services with SVD crypto capability.
    SECCRYPTDSP_CRYPTO_IMAGE   =   QDSP_IMAGE_VOC,
  
  #else  /* FEATURE_SEC_SVD_DSP_IMAGE */
  
    SECCRYPTDSP_CRYPTO_IMAGE = QDSP_IMAGE_CRYPT, /*qdsp_image_type enum value*/

  #endif /* FEATURE_SEC_SVD_DSP_IMAGE */

#endif /* FEATURE_QDSP_USES_MODULES */

} seccryptdsp_image_enum_type;

typedef enum
{
  SECCRYPTDSP_NO_APPLET = 0,
  SECCRYPTDSP_MODMATH_APPLET,
  SECCRYPTDSP_CIPHER_APPLET,
  SECCRYPTDSP_HASH_APPLET
} seccryptdsp_applet_enum_type;

typedef enum
{
  SECCRYPTDSP_DISABLED_STATE = 0,
  SECCRYPTDSP_REQUESTED_STATE,
  SECCRYPTDSP_UNAVAIL_STATE,
  SECCRYPTDSP_ENABLED_STATE,
  SECCRYPTDSP_BUSY_STATE,
  SECCRYPTDSP_IDLE_STATE
} seccryptdsp_state_enum_type;

typedef enum
{
  SECCRYPTDSP_FLUSH_CACHE_CMD = 0,
  SECCRYPTDSP_DELETE_CACHE_CMD = 1,
  SECCRYPTDSP_MAX_CMD
} seccryptdsp_cmd_enum_type;

typedef enum
{
  SECCRYPTDSP_EVENT_NONE = 0,
  SECCRYPTDSP_EVENT_READY,
  SECCRYPTDSP_EVENT_DISABLE
} seccryptdsp_event_enum_type;

typedef seccryptdsp_dme_table_entry_type*  seccryptdsp_dme_table_type;

/* Instance's common data for DSP implementations */
#ifdef SECCRYPTDSP_RTOS_API
#define SECCRYPTDSP_MAX_INPUT_CMD_SIZE ( SECCRYPT_PKX_MAX_BASE_SIZE + \
                                         SECCRYPT_PKX_MAX_EXP_SIZE + \
                                         SECCRYPT_PKX_MAX_MOD_SIZE )

struct seccryptdsp_common_data_s
{
   uint8 input_cmd_buf[SECCRYPTDSP_MAX_INPUT_CMD_SIZE];
   uint8 output_cmd_res_len;
};

#else /* SECCRYPTDSP_RTOS_API */

struct seccryptdsp_common_data_s
{
  /* DSP DME I/O tables */
  struct dsp_dme_info_s
  {
    seccryptdsp_dme_table_type  input_table;
    int                         input_table_size;

    seccryptdsp_dme_table_type  output_table;
    int                         output_table_size;
  } dme;

  /* DSP command data */
  seccryptdsp_cmd_type  cmd;
  struct
  {
    uint32              encrypt;
    uint32              decrypt;
  } context_state;
};

#endif /* SECCRYPTDSP_RTOS_API */

/* Information about the aDSP */
typedef struct
{
  /* State of the DSP */
  seccryptdsp_state_enum_type   state;

  /* current qdsp app (QDSP_APP_MODMATH = high QDSP_APP_CRYPT = low prioty) */
  qdsp_app_type                 qdsp_app;

  /* Current and previous image / applet */
  seccryptdsp_image_enum_type   image;
  seccryptdsp_image_enum_type   previous_image;
  seccryptdsp_applet_enum_type  applet;
  seccryptdsp_applet_enum_type  previous_applet;

  /* Reset request by another DSP application */
  boolean                       reset_pending;

  /* Instance currently using the DSP */
  seccrypt_instance_type*       instance_ptr;
  uint32                        user_mask;
  
  /* DSP timers for execution and pending user timeouts */
  rex_timer_type        exec_timer;
  rex_timer_type        user_timer;
  rex_timer_type        req_timer;
  rex_timer_type        gp_timer;
  uint32                user_timeout;

  /* DSP global command */
  seccryptdsp_cmd_type          global_cmd;

  /* Force reload when enabling DSP image. */
  boolean                      dsp_reload_needed;
} seccryptdsp_type;

#define SECCRYPTDSP_MAX_EVENTS 8

typedef struct
{
   int head;
   int tail;
   rex_crit_sect_type crit_sect;
   seccryptdsp_event_enum_type q[SECCRYPTDSP_MAX_EVENTS];
}seccryptdsp_event_q_type;

/*===========================================================================

                           DATA DECLARATIONS

===========================================================================*/

/* Queue of commands for the DSP platform */
extern const seccrypt_cmd_q_type   seccryptdsp_cmd_q;

/* Information about the aDSP */
extern seccryptdsp_type  seccryptdsp;

/* DME tables are read by the DSP for payload address and size */
extern seccryptdsp_dme_table_entry_type  seccryptdsp_dme_input_table
                                          [SECCRYPTDSP_INPUT_DME_TABLE_SIZE];

extern seccryptdsp_dme_table_entry_type  seccryptdsp_dme_output_table
                                         [SECCRYPTDSP_OUTPUT_DME_TABLE_SIZE];


/*===========================================================================

                      EXTERNAL MACRO DEFINITIONS

===========================================================================*/


/*===========================================================================

                    EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/
secerrno_enum_type seccryptdsp_invalidate_data_cache (void);



/*===========================================================================

FUNCTION SECCRYPTDSP_POWERUP_INIT()

DESCRIPTION
  One-time initialization of DSP platform-specific data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void seccryptdsp_powerup_init( void );


/*===========================================================================

FUNCTION SECCRYPTDSP_PLATFORM_INIT()

DESCRIPTION
  Re-initialization of DSP platform-specific data after switching
  platforms.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void seccryptdsp_platform_init
(
  seccrypt_instance_type*      instance_ptr
);

/*===========================================================================

FUNCTION SECCRYPTDSP_PLATFORM_RELEASE()

DESCRIPTION
  Cleanup and release of DSP platform-specific data after switching
  platforms.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void seccryptdsp_platform_release
(
  seccrypt_instance_type*      instance_ptr
);

/*===========================================================================

FUNCTION SECCRYPTDSP_RELEASE_DSP()

DESCRIPTION
  Release of the DSP for this instance.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void seccryptdsp_release_dsp
(
  seccrypt_instance_type*      instance_ptr
);

extern secerrno_enum_type seccryptdsp_req_dsp
(
  seccrypt_instance_type*             instance_ptr,
  seccrypts_instance_state_enum_type  old_state,
  seccrypts_instance_state_enum_type  new_state
);

extern secerrno_enum_type seccryptdsp_enable_applet
(
  seccrypt_instance_type*             instance_ptr,
  seccrypts_instance_state_enum_type  old_state,
  seccrypts_instance_state_enum_type  new_state
);

extern secerrno_enum_type seccryptdsp_check_applet_status
(
  seccrypt_instance_type*             instance_ptr,
  seccrypts_instance_state_enum_type  old_state,
  seccrypts_instance_state_enum_type  new_state
);

extern secerrno_enum_type seccryptdsp_release_applet
(
  seccrypt_instance_type*             instance_ptr,
  seccrypts_instance_state_enum_type  old_state,
  seccrypts_instance_state_enum_type  new_state
);

extern secerrno_enum_type seccryptdsp_issue_applet_cmd
(
  seccrypt_instance_type*             instance_ptr
);

extern secerrno_enum_type seccryptdsp_issue_dsp_cmd
(
  seccrypt_instance_type*    instance_ptr,
  seccryptdsp_cmd_enum_type  cmd
);

/*===========================================================================

FUNCTION SECCRYPTDSP_SEND_CMD()

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  Status of state transition.

SIDE EFFECTS
  None.
===========================================================================*/
secerrno_enum_type seccryptdsp_send_cmd
(
  seccryptdsp_cmd_type*      cmd_ptr,
  //boolean                    inline_cmd
  secapi_exec_mode_enum_type exec_mode
);

/*==========================================================================
 FUNCTION SECCRYPTDSP_PROCESS_CMD_SIG

 DESCRIPTION 
   This function processes the signal of the DSP crypto command queue.

 DEPENDENCIES 
   None

 RETURN VALUE 
   None

 SIDE EFFECTS
   None
===========================================================================*/
extern void seccryptdsp_process_cmd_sig( void );

/*==========================================================================
 FUNCTION SECCRYPTDSP_PROCESS_CMD

 DESCRIPTION 
   This function processes commands for execution in the DSP interface 
   task.

 DEPENDENCIES 
   None

 RETURN VALUE 
   Security services error code.
  
 SIDE EFFECTS
   None
===========================================================================*/
extern secerrno_enum_type seccryptdsp_process_cmd
(
  seccrypt_cmd_type*  cmd_ptr
);

/*==========================================================================
 FUNCTION SECCRYPTDSP_SCHEDULE_OP

 DESCRIPTION 
   This function schedules an operation for the DSP interface task.

 DEPENDENCIES 
   None

 RETURN VALUE 
   Security services error code.

 SIDE EFFECTS
   None
===========================================================================*/
secerrno_enum_type seccryptdsp_schedule_op
(
  seccrypt_cmd_enum_type   cmd,
  seccrypt_instance_type*  instance_ptr
);

/*==========================================================================
 FUNCTION SECCRYPTDSP_PROCESS_EXEC_TIMER_SIG

 DESCRIPTION 
   This function handles the execution timeout to abort a DSP operation.

 DEPENDENCIES 
   None

 RETURN VALUE 
   None

 SIDE EFFECTS
   None
===========================================================================*/
void seccryptdsp_process_exec_timer_sig( void );

/*==========================================================================
 FUNCTION SECCRYPTDSP_PROCESS_GP_TIMER_SIG

 DESCRIPTION 
   This function handles the dsp gp timeout to switch platform scheduler

 DEPENDENCIES 
   None

 RETURN VALUE 
   None

 SIDE EFFECTS
   None
===========================================================================*/
void seccryptdsp_process_gp_timer_sig( void );

/*==========================================================================
 FUNCTION SECCRYPTDSP_PROCESS_USER_TIMER_SIG

 DESCRIPTION 
   This function handles the user timeout to disable the DSP crypto image.

 DEPENDENCIES 
   None

 RETURN VALUE 
   None

 SIDE EFFECTS
   None
===========================================================================*/
void seccryptdsp_process_user_timer_sig( void );

/*==========================================================================
 FUNCTION SECCRYPTDSP_PROCESS_REQ_TIMER_SIG

 DESCRIPTION 
   This function handles the dsp req timeout for failover mechanism.

 DEPENDENCIES 
   None

 RETURN VALUE 
   None

 SIDE EFFECTS
   None
===========================================================================*/
void seccryptdsp_process_req_timer_sig( void );

/*==========================================================================
 FUNCTION SECCRYPTDSP_PROCESS_EVENT_SIG

 DESCRIPTION 
   This function handles dsp events.

 DEPENDENCIES 
   None

 RETURN VALUE 
   None

 SIDE EFFECTS
   None
===========================================================================*/
void seccryptdsp_process_event_sig( void );

/*==========================================================================
 FUNCTION SECCRYPTDSP_PROCESS_OP_COMPLETE_SIG

 DESCRIPTION 
   This function handles dsp events.

 DEPENDENCIES 
   None

 RETURN VALUE 
   None

 SIDE EFFECTS
   None
===========================================================================*/
void seccryptdsp_process_op_complete_sig( void );

#else /* FEATURE_SEC_DSP_CRYPTO */

/* Dummy definition for the non-existent DSP implementation */
struct seccryptdsp_common_data_s
{
  void* dummy;
};

#endif /* FEATURE_SEC_DSP_CRYPTO */

#endif /* FEATURE_SEC */

#endif /* SECCRYPTDSP_H */

