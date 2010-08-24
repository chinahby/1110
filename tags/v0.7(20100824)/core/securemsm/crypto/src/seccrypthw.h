#ifndef SECCRYPTHW_H
#define SECCRYPTHW_H

/*===========================================================================

DESCRIPTION

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


 Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
  $Header: //source/qcom/qct/core/securemsm/crypto/main/latest/src/seccrypthw.h#2 $ 
  $DateTime: 2007/11/14 00:17:01 $ $Author: davidf $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/12/07    df      created
===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_SEC
#ifdef FEATURE_SEC_HW_CRYPTO

//#include MSMHWIOREGP_H

#include "dmov.h"
#define SECCRYPTHW_DEBUG

/*===========================================================================

                         CONSTANTS AND DEFINITIONS

===========================================================================*/
/* CE_STATUS bit definitions */
#define CE_STATUS_DBG_EN                0x00040000
#define CE_STATUS_DBG_SEL               0x00030000
#define CE_STATUS_C2_FIRST_BLK          0x00008000
#define CE_STATUS_CLK_EN                0x00004000
#define CE_STATUS_AUTO_SHUTDOWN_ENABLE  0x00002000
#define CE_STATUS_CLR_DATA_OUT_VLD_INT  0x00001000
#define CE_STATUS_CLR_DATA_IN_RDY_INT   0x00000800
#define CE_STATUS_CLR_HASH_DONE_INT     0x00000400
#define CE_STATUS_CLR_ERR_INT           0x00000200
#define CE_STATUS_MASK_DATA_OUT_VLD_INT 0x00000100
#define CE_STATUS_MASK_DATA_IN_RDY_INT  0x00000080
#define CE_STATUS_MASK_HASH_DONE_INT    0x00000040
#define CE_STATUS_MASK_ERR_INT          0x00000020
#define CE_STATUS_DATA_OUT_VALID        0x00000010
#define CE_STATUS_DATA_IN_RDY           0x00000008
#define CE_STATUS_HASH_DONE             0x00000004
#define CE_STATUS_ERR                   0x00000002
#define CE_STATUS_RST                   0x00000001

/* CE_PROC_SEL bit definitons */
#define CE_PROC_SEL_HASH_ONLY       0x0
#define CE_PROC_SEL_ENC_ONLY        0x1
#define CE_PROC_SEL_HASH_AFTER_ENC  0x2
#define CE_PROC_SEL_HASH_BEFORE_ENC 0x3

/* CE_CNTXT bit definitions */
#define CE_CNTXT_MASK_CNT       0x003F8000
#define CE_CNTXT_SHIFT_VAL      0x00007C00
#define CE_CNTXT_COUNTER_MODE   0x00000200
#define CE_CNTXT_ADD_PAD        0x00000100
#define CE_CNTXT_HASH_ALG       0x000000C0
#define CE_CNTXT_ENC            0x00000020
#define CE_CNTXT_MODE           0x00000010
#define CE_CNTXT_ENC_ALG        0x0000000F

#define CE_CNTXT_HASH_ALG_SHA256    0x80
#define CE_CNTXT_HASH_ALG_SHA1      0x40
#define CE_CNTXT_MODE_ECB           0x00
#define CE_CNTXT_MODE_CBC           0x10
#define CE_CNTXT_ENC_ALG_PASS       0x00
#define CE_CNTXT_ENC_ALG_DES        0x01
#define CE_CNTXT_ENC_ALG_3DES       0x02
#define CE_CNTXT_ENC_ALG_AES128     0x04
#define CE_CNTXT_ENC_ALG_C2         0x08

/* Task context and wait function for HW CE related signals */
#define SECCRYPTHW_TCB_PTR         &sec_tcb
#define SECCRYPTHW_WAIT            sec_wait

/* HW specific signals */
#define SECCRYPTHW_INLINE_CMD_SIG  SEC_DSP_INLINE_CMD_SIG

/* Time to wait before disabling the HW CE [ms] */
#define SECCRYPTHW_DISABLE_WAIT_TIME 20

/* 8 byte alignment, necessary for data mover */
#define SECCRYPTHW_ALIGN(addr) \
   (((uint32)addr & 7 ) ? ((uint32)addr & 0xfffffff8) + 8 : (uint32)addr)

/* iov table sizes... */
#define SECCRYPTHW_MAX_DMOV_SG_SIZE DMOV_MAX_NUM_DSCR
#define SECCRYPTHW_MAX_DMOV_BUFFER_SIZE (16384 + 128)

/* for some reason dmov sg list size must be less than 80 items
 *  found this out via trial and error
 *  using max of 76 (i.e 75 + 1 for pre-pending leftover for hash)
 */
#define SECCRYPTHW_INPUT_DMOV_TABLE_SIZE 76
#define SECCRYPTHW_MAX_SG_CMDS 6  // 456 items (for ssl 16384/36 items)

/* sanity check on iovec table size, similar to max_dmov_sg_size (above) */
#define SECCRYPTHW_MAX_IOVEC_SIZE 512

#ifdef T_MSM7500
#error code not present
#else

#ifdef FEATURE_MEMORY_VIRTUAL
#error Virtual Memory!!!
#endif

#define SECCRYPTHW_CRCI_DBG_ADDR    HWIO_PHYS(MDM_CRCI_DBG)
#define HWIO_CE_HASH_IVn_ADDRI_PHYS HWIO_CE_HASH_IVn_ADDRI
#define HWIO_CE_CNTXT_PHYS          HWIO_CE_CNTXT_ADDR
#define HWIO_CE_HASH_LEN_PHYS       HWIO_CE_HASH_LEN_ADDR
#define HWIO_CE_HASH_BIT_CNT0_PHYS  HWIO_CE_HASH_BIT_CNT0_ADDR
#define HWIO_CE_DATA_IN_PHYS        HWIO_CE_DATA_IN_ADDR
#define HWIO_CE_DATA_OUT_PHYS       HWIO_CE_DATA_OUT_ADDR
#define HWIO_CE_AES_RNDKEYn_PHYS    HWIO_CE_AES_RNDKEYn_ADDRI
#endif


/* additional dmov definitions */
#define SECCRYPTHW_DMOV_SG_LAST_BIT DMOV_DSCR_WORD1_LD_BMSK
typedef struct
{
   uint32 addr;
   uint32 ld_len;
}seccrypthw_sg_desc_type;

extern seccrypthw_sg_desc_type seccrypthw_dmov_in_src_sg_list[SECCRYPTHW_INPUT_DMOV_TABLE_SIZE + 1];
extern seccrypthw_sg_desc_type seccrypthw_dmov_in_dst_sg_list[1];
extern seccrypthw_sg_desc_type seccrypthw_dmov_out_src_sg_list[1];
extern seccrypthw_sg_desc_type seccrypthw_dmov_out_dst_sg_list[1];
extern uint8 seccrypthw_dmov_buf[SECCRYPTHW_MAX_DMOV_BUFFER_SIZE];

/*===========================================================================

                           TYPE DECLARATIONS

===========================================================================*/

/* General command structure definition */
typedef struct
{
   /* quick check to see if we are in the middle of an op 
    *  1 = running/continue
    *  0 = idle
    */
   uint32   context_state;

   /* dummy pad bytes to add to the data buffer to make it 16 byte mult */
   uint32   pad[4];
} seccrypthw_ce_cmd_type;

typedef struct
{
   dmov_xfer_desc_s_type ixfer;   // dmov input transfer structure
   dmov_xfer_desc_s_type oxfer;   // dmov output transfer structure

   /* the following commands MUST be 8 byte aligned 
    * see secenchw.h, sechshhw.h
    */
   void *icmd_list_ptr;
   void *ocmd_list_ptr;

   /* this is the index into the currect iovec */
   uint32 iov_index;
   uint16 start_offset;
   uint16 curr_msg_len;
}seccrypthw_dmov_cmd_type;

struct seccrypthw_common_data_s
{
   seccrypthw_ce_cmd_type   ce;
   seccrypthw_dmov_cmd_type dmov;

   struct
   {
      uint32 encrypt;
      uint32 decrypt;
   } context_state;
};

/* ce applet types */
typedef enum
{
   SECCRYPTHW_NO_APPLET = 0,
   SECCRYPTHW_CIPHER_APPLET,
   SECCRYPTHW_HASH_APPLET
} seccrypthw_applet_enum_type;

/* ce hw states */
typedef enum
{
   SECCRYPTHW_DISABLED_STATE = 0,
   SECCRYPTHW_BUSY_STATE,
   SECCRYPTHW_IDLE_STATE
} seccrypthw_state_enum_type;

/* crypto engine state vars */
typedef struct
{
   boolean initialized;
   seccrypthw_state_enum_type state;

   /* Current and previous image / applet */
   seccrypthw_applet_enum_type  applet;
   seccrypthw_applet_enum_type  previous_applet;

   /* current instance */
   seccrypt_instance_type *instance_ptr;
   uint32                 user_mask;

   /* DSP timers for execution and pending user timeouts */
   rex_timer_type         exec_timer;
   rex_timer_type         user_timer;
   uint32                 user_timeout;
}seccrypthw_type;

/*===========================================================================

                           DATA DECLARATIONS

===========================================================================*/
/* Global declarations */
extern seccrypthw_type  seccrypthw;

/* Queue of commands for the HW platform */
extern const seccrypt_cmd_q_type   seccrypthw_cmd_q;

/*===========================================================================

                           EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

                           FUNCTION DECLARATIONS

===========================================================================*/
/*==========================================================================
 FUNCTION SECCRYPTHW_PROCESS_CMD_SIG

 DESCRIPTION 
   This function processes the signal of the CE HW crypto command queue.

 DEPENDENCIES 
   None

 RETURN VALUE 
   None

 SIDE EFFECTS
   None
===========================================================================*/
void seccrypthw_process_cmd_sig( void );

/*==========================================================================
 FUNCTION SECCRYPTHW_PROCESS_EXEC_TIMER_SIG

 DESCRIPTION 
   This function handles the execution timeout to abort a CE HW operation.

 DEPENDENCIES 
   None

 RETURN VALUE 
   None

 SIDE EFFECTS
   None
===========================================================================*/
void seccrypthw_process_exec_timer_sig( void );

/*==========================================================================
 FUNCTION SECCRYPTHW_PROCESS_USER_TIMER_SIG

 DESCRIPTION 
   This function handles the user timeout to disable CE clk regime

 DEPENDENCIES 
   None

 RETURN VALUE 
   None

 SIDE EFFECTS
   None
===========================================================================*/
void seccrypthw_process_user_timer_sig( void );

/*==========================================================================
 FUNCTION SECCRYPTHW_PROCESS_OP_COMPLETE_SIG

 DESCRIPTION 
   This function handles the hw op complete, to resume operation

 DEPENDENCIES 
   None

 RETURN VALUE 
   None

 SIDE EFFECTS
   None
===========================================================================*/
void seccrypthw_process_op_complete_sig( void );

/*===========================================================================

FUNCTION SECCRYPHW_POWERUP_INIT()

DESCRIPTION
  One-time initialization of DSP platform-specific data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void seccrypthw_powerup_init( void );

/*===========================================================================

FUNCTION SECCRYPTHW_PLATFORM_INIT()

DESCRIPTION
  Re-initialization of HW CE platform-specific data after switching
  platforms.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void seccrypthw_platform_init(  seccrypt_instance_type*  instance_ptr );

/*===========================================================================

FUNCTION SECCRYPTHW_PLATFORM_RELEASE()

DESCRIPTION
  Cleanup and release of HW CE platform-specific data after switching
  platforms.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void seccrypthw_platform_release(  seccrypt_instance_type*  instance_ptr );

/*===========================================================================

FUNCTION SECCRYPTHW_RELEASE_APPLET()

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  Status of state transition.

SIDE EFFECTS
  None.
===========================================================================*/
secerrno_enum_type seccrypthw_release_applet
(
seccrypt_instance_type*             instance_ptr,
seccrypts_instance_state_enum_type  old_state,
seccrypts_instance_state_enum_type  new_state
);

/*==========================================================================
 FUNCTION SECCRYPTHW_SCHEDULE_OP

 DESCRIPTION 
   This function schedules an operation for the CE HW interface task.

 DEPENDENCIES 
   None

 RETURN VALUE 
   Security services error code.

 SIDE EFFECTS
   None
===========================================================================*/
secerrno_enum_type seccrypthw_schedule_op
(
seccrypt_cmd_enum_type   cmd,
seccrypt_instance_type*  instance_ptr
);

/*===========================================================================

FUNCTION SECCRYPTHW_ISSUE_APPLET_CMD()

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  Status of state transition.

SIDE EFFECTS
  None.
===========================================================================*/
secerrno_enum_type seccrypthw_issue_applet_cmd
(
seccrypt_instance_type*  instance_ptr
);

/*===========================================================================

FUNCTION SECCRYPTHW_DMOV_SINGLE()

DESCRIPTION
  Utility function to setup single address data mover command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void seccrypthw_dmov_single( dmov_cmd_single_s_type *cmd,
                             uint32 src,
                             uint32 dst,
                             uint32 len,
                             boolean swap );

#else /* FEATURE_SEC_HW_CRYPTO */

/* Dummy definition for the non-existent HW implementation */
struct seccrypthw_common_data_s
{
   void* dummy;
};

#endif /* FEATURE_SEC_HW_CRYPTO */

#endif /* FEATURE_SEC */

#endif  /* SECCRYPTHW_H */

