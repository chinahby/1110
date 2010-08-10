#ifndef BOOT_TRAP_H
#define BOOT_TRAP_H
/*==========================================================================

                        Boot Trap Handlers Header File
                 
DESCRIPTION
  Routines to intercept trap handlers from the processor
  
  Details ARM instructions so they can be interpreted by the data abort
  handler.  These formats are detailed in the ARM documentation and in
  The ARM ARM.

Copyright (c) 2004, 2005 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$PVCSPath:
$Header: //depot/asic/msm6275/drivers/boot/boot_trap.h#1
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/01/08   ejh     Featurize for FEATURE_PMIC_WDOG
05/06/05   tbg     Pulled in changes from 6250.
12/01/04   tkuo    Ported from MSM6250.

===========================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/



/*===========================================================================
 
                           DATA DECLARATIONS

===========================================================================*/

/* -----------------------------------------------------------------------
** Offsets in ARM_TRAP_FRAME. These are used in assembly files.
** ----------------------------------------------------------------------- */

#define ARM_TRAP_FRAME_SIZE             0x44
#define ARM_TRAP_FRAME_SPSR_OFFSET      0x0
#define ARM_TRAP_FRAME_A_OFFSET         0x4
#define ARM_TRAP_FRAME_V_OFFSET         0x14
#define ARM_TRAP_FRAME_FP_OFFSET        0x30
#define ARM_TRAP_FRAME_IP_OFFSET        0x34
#define ARM_TRAP_FRAME_SP_OFFSET        0x38
#define ARM_TRAP_FRAME_LR_OFFSET        0x3c
#define ARM_TRAP_FRAME_PC_OFFSET        0x40


#define THUMB_INST_MASK_7BIT_OPCODE     0xfE00
#define THUMB_INST_MASK_5BIT_OPCODE     0xf800


#if !defined(_ARM_ASM_)

#include "comdef.h"

#if defined(FEATURE_PMIC_WDOG) && defined(FEATURE_SHARED_SEGMENT)

#include "boot_shared_seg.h"

/* boot trap structure to be stored in shared segment */
typedef struct
{
  BOOT_SHARED_SEG_HEADER
  uint16 pon_status;
}boot_trap_shared_type;

#endif

typedef enum
{
	LITTLE_ENDIAN =0,
	BIG_ENDIAN
}ARM_endiannesstype;
typedef void (*PFUNCTION)(void);
typedef union { word w; void *addr; PFUNCTION func; } ArmRegister;

typedef struct {
  ArmRegister Spsr;
  ArmRegister a[4];
  ArmRegister v[7];
  struct _StackFrame *fp;             /*  (r11) Saved Frame pointer */
  ArmRegister ip;                     /*  (r12) Saved ip            */
  ArmRegister sp;                     /*  (r13) Saved stack pointer */
  ArmRegister lr;                     /*  (r14) Saved Link Register */
  ArmRegister pc_not_used;            /*  (r15) Saved PC            */
} ARM_EXCEPTION_FRAME_EX, *PARM_EXCEPTION_FRAME_EX;

typedef union {
  ARM_EXCEPTION_FRAME_EX f;
  struct {
    dword Spsr;
    dword r[16];
  } l;
} ARM_TRAP_FRAME, *PARM_TRAP_FRAME;

typedef union {
/* load word immediate offset */
#define THUMB_INST_MASK_LD_W_I_OFFSET 0xf800
#define THUMB_INST_VAL_LD_W_I_OFFSET  0x6800
  struct {
    uint32 rd:3;
    uint32 rn:3;
    uint32 offset:5;
    uint32 opcode:5;
  } ld_w_i_offset;
  
/* load word  register offset */
#define THUMB_INST_MASK_LD_W_R_OFFSET 0xfe00
#define THUMB_INST_VAL_LD_W_R_OFFSET  0x5800
  struct {
    uint32 rd:3;
    uint32 rn:3;
    uint32 rm:3;
    uint32 opcode:7;
  } ld_w_r_offset;

/* load word  pc_relative */
#define THUMB_INST_MASK_LD_W_PC_RELATIVE 0xf800
#define THUMB_INST_VAL_LD_W_PC_RELATIVE  0x4800
  struct {
    uint32 offset:8;
    uint32 rd:3;
    uint32 opcode:5;
  } ld_w_pc_relative;

/* load word  sp_relative */
#define THUMB_INST_MASK_LD_W_SP_RELATIVE 0xf800
#define THUMB_INST_VAL_LD_W_SP_RELATIVE  0x9800
  struct {
    uint32 offset:8;
    uint32 rd:3;
    uint32 opcode:5;
  } ld_w_sp_relative;

/* load unsigned half word immediate offset  */
#define THUMB_INST_MASK_LD_UHW_I_OFFSET 0xf800
#define THUMB_INST_VAL_LD_UHW_I_OFFSET  0x8800
  struct {
    uint32 rn:3;
    uint32 rd:3;
    uint32 offset:5;
    uint32 opcode:5;
  } ld_uhw_i_offset;

/* load unsigned half word register offset  */
#define THUMB_INST_MASK_LD_UHW_R_OFFSET 0xfe00
#define THUMB_INST_VAL_LD_UHW_R_OFFSET  0x5a00
  struct {
	  uint32 rd:3;
	  uint32 rn:3;
	  uint32 rm:3;
	  uint32 opcode:7;
  } ld_uhw_r_offset;

/* load signed half word register offset  */
#define THUMB_INST_MASK_LD_SHW_R_OFFSET 0xfe00
#define THUMB_INST_VAL_LD_SHW_R_OFFSET  0x5e00
  struct {
	  uint32 rd:3;
	  uint32 rn:3;
	  uint32 rm:3;
	  uint32 opcode:7;
  } ld_shw_r_offset;

/* Load multiple increment after  */
#define THUMB_INST_MASK_LDM_INC_AFTER 0xf800
#define THUMB_INST_VAL_LDM_INC_AFTER  0xc800
  struct {
	  uint32 register_list:8;
	  uint32 rn:3;
	  uint32 opcode:5;
  } ldm_inc_after;
/* Store word immediate offset */
#define THUMB_INST_MASK_STR_W_I_OFFSET 0xf800
#define THUMB_INST_VAL_STR_W_I_OFFSET  0x6000
  struct {
    uint32 rd:3;
    uint32 rn:3;
    uint32 offset:5;
    uint32 opcode:5;
  } str_w_i_offset;
  
/* Store word  register offset */
#define THUMB_INST_MASK_STR_W_R_OFFSET 0xfe00
#define THUMB_INST_VAL_STR_W_R_OFFSET  0x5000
  struct {
    uint32 rd:3;
    uint32 rn:3;
    uint32 rm:3;
    uint32 opcode:7;
  } str_w_r_offset;

/* Store word  sp_relative */
#define THUMB_INST_MASK_STR_W_SP_RELATIVE 0xf800
#define THUMB_INST_VAL_STR_W_SP_RELATIVE  0x9000
  struct {
    uint32 offset:8;
    uint32 rd:3;
    uint32 opcode:5;
  } str_w_sp_relative;

/* Store half word immediate offset  */
#define THUMB_INST_MASK_STR_HW_I_OFFSET 0xf800
#define THUMB_INST_VAL_STR_HW_I_OFFSET  0x8000
  struct {
    uint32 rd:3;
    uint32 rn:3;
    uint32 offset:5;
    uint32 opcode:5;
  } str_hw_i_offset;

/* Store half word register offset  */
#define THUMB_INST_MASK_STR_HW_R_OFFSET 0xfe00
#define THUMB_INST_VAL_STR_HW_R_OFFSET  0x5200
  struct {
	  uint32 rd:3;
	  uint32 rn:3;
	  uint32 rm:3;
	  uint32 opcode:7;
  } str_hw_r_offset;


/* Store multiple increment after  */
#define THUMB_INST_MASK_STM_INC_AFTER 0xf800
#define THUMB_INST_VAL_STM_INC_AFTER  0xc000
  struct {
	  uint32 register_list:8;
	  uint32 rn:3;
	  uint32 opcode:5;
  } stm_inc_after;

/* Pop */
#define THUMB_INST_MASK_POP_STACK 0xfe00
#define THUMB_INST_VAL_POP_STACK  0xbc00
  struct {
    uint32 register_list:8;
    uint32 r:1;
    uint32 opcode:7;
  } pop_stack;

/* Push */
#define THUMB_INST_MASK_PUSH_STACK 0xfe00
#define THUMB_INST_VAL_PUSH_STACK  0xb400
  struct {
    uint32 register_list:8;
    uint32 r:1;
    uint32 opcode:7;
  } push_stack;
  uint32 l;
} ThumbInst;

typedef union {

  struct {
    uint32 cond:4;
    uint32 opcode:3;
    uint32 operation:4;
    uint32 s:1;
    uint32 rn:4;
    uint32 rd:4;
    uint32 rotate:4;
    uint32 immediate:8;
  } dp_immed;

  struct {
    uint32 cond:4;
    uint32 opcode:3;
    uint32 operation:4;
    uint32 s:1;
    uint32 rn:4;
    uint32 rd:4;
    uint32 shift_immed:5;
    uint32 shift:2;
    uint32 opcode2:1;
    uint32 rm:4;
  } dp_immed_shift;

  struct {
    uint32 cond:4;
    uint32 opcode:3;
    uint32 operation:4;
    uint32 s:1;
    uint32 rn:4;
    uint32 rd:4;
    uint32 rs:4;
    uint32 opcode2:1;
    uint32 shift:2;
    uint32 rm:4;
  } dp_reg_shift;

  struct {
    uint32 cond:4;
    uint32 opcode:6;
    uint32 a:1;
    uint32 s:1;
    uint32 rd:4;
    uint32 rn:4;
    uint32 rs:4;
    uint32 opcode2:4;
    uint32 rm:4;
  } multiply;

  struct {
    uint32 cond:4;
    uint32 opcode:5;
    uint32 u:1;
    uint32 a:1;
    uint32 s:1;
    uint32 rdhi:4;
    uint32 rdlo:4;
    uint32 rs:4;
    uint32 opcode2:4;
    uint32 rm:4;
  } multiply_long;

  struct {
    uint32 cond:4;
    uint32 opcode:5;
    uint32 r:1;
    uint32 opcode2:2;
    uint32 mask:4;
    uint32 sbo:4;
    uint32 rotate:4;
    uint32 immediate:8;
  } msr_immediate;

  struct {
    uint32 cond:4;
    uint32 opcode:5;
    uint32 r:1;
    uint32 opcode2:2;
    uint32 mask:4;
    uint32 sbo:4;
    uint32 sbz:7;
    uint32 opcode3:1;
    uint32 rm:4;
  } msr;

  struct {
    uint32 cond:4;
    uint32 opcode:5;
    uint32 r:1;
    uint32 opcode2:2;
    uint32 mask:4;
    uint32 sbo:4;
    uint32 sbz:12;
  } mrs;

  struct {
    uint32 cond:4;
    uint32 opcode:7;
    uint32 sbo:4;
    uint32 sbo2:4;
    uint32 sbo3:4;
    uint32 rn:4;
    uint32 opcode2:4;
    uint32 rm:4;
  } bx;

  struct {
    uint32 cond:4;
    uint32 opcode:3;
    uint32 p:1;
    uint32 u:1;
    uint32 b:1;
    uint32 w:1;
    uint32 l:1;
    uint32 rn:4;
    uint32 rd:4;
    uint32 immediate:12;
  } ld_st_immediate_offset;

  struct {
    uint32 cond:4;
    uint32 opcode:3;
    uint32 p:1;
    uint32 u:1;
    uint32 b:1;
    uint32 w:1;
    uint32 l:1;
    uint32 rn:4;
    uint32 rd:4;
    uint32 shifted_immediate:6;
    uint32 shift:3;
    uint32 opcode2:1;
    uint32 rm:4;
  } ld_st_register_offset;

  struct {
    uint32 cond:4;
    uint32 opcode:3;
    uint32 p:1;
    uint32 u:1;
    uint32 opcode2:1;
    uint32 w:1;
    uint32 l:1;
    uint32 rn:4;
    uint32 rd:4;
    uint32 hi_offset:4;
    uint32 opcode3:1;
    uint32 s:1;
    uint32 h:1;
    uint32 opcode4:1;
    uint32 lo_offset:4;
  } ld_st_half_byte;

  struct {
    uint32 cond:4;
    uint32 opcode:3;
    uint32 p:1;
    uint32 u:1;
    uint32 opcode2:1;
    uint32 w:1;
    uint32 l:1;
    uint32 rn:4;
    uint32 rd:4;
    uint32 sbz:4;
    uint32 opcode3:1;
    uint32 s:1;
    uint32 h:1;
    uint32 opcode4:1;
    uint32 rm:4;
  } ld_st_half_byte_2;

  struct {
    uint32 cond:4;
    uint32 opcode:5;
    uint32 b:1;
    uint32 opcode2:2;
    uint32 rn:4;
    uint32 rd:4;
    uint32 sbz:4;
    uint32 opcode3:4;
    uint32 rm:4;
  } swap;

  struct {
    uint32 cond:4;
    uint32 opcode:3;
    uint32 p:1;
    uint32 u:1;
    uint32 s:1;
    uint32 w:1;
    uint32 l:1;
    uint32 rn:4;
    uint32 rlist:16;
  } ld_st_multiple;

  struct {
    uint32 cond:4;
    uint32 opcode:4;
    uint32 op:4;
    uint32 crn:4;
    uint32 crd:4;
    uint32 cp_num:4;
    uint32 op2:3;
    uint32 opcode2:1;
    uint32 crm:4;
  } coprocessor_data;

  struct {
    uint32 cond:4;
    uint32 opcode:4;
    uint32 op:3;
    uint32 l:1;
    uint32 crn:4;
    uint32 rd:4;
    uint32 cp_num:4;
    uint32 op2:3;
    uint32 opcode2:1;
    uint32 crm:4;
  } coprocessor_register;

  struct {
    uint32 cond:4;
    uint32 opcode:3;
    uint32 p:1;
    uint32 u:1;
    uint32 n:1;
    uint32 w:1;
    uint32 l:1;

    uint32 rn:4;
    uint32 crd:4;
    uint32 cp_num:4;
    uint32 offset:8;
  } coprocessor_ld_st;

  struct {
    uint32 cond:4;
    uint32 opcode:3;
    uint32 l:1;
    uint32 offset:24;
  } b_bl;

  struct {
    uint32 cond:4;
    uint32 opcode:4;
    uint32 swi_num:24;
  } swi;

  struct {
    uint32 cond:4;
    uint32 opcode:3;
    uint32 op:20;
    uint32 opcode2:1;
    uint32 op2:4;
  } undef;

  dword l;

} ArmInstr;


/* -----------------------------------------------------------------------
** ARM Instruction Values
** ----------------------------------------------------------------------- */
#define ARM_INST_VAL_LD_ST_IMMEDIATE_OFFSET 0x04000000
#define ARM_INST_VAL_LD_ST_REGISTER_OFFSET  0x05000000
#define ARM_INST_VAL_LD_ST_HALF_BYTE        0x00400090
#define ARM_INST_VAL_LD_ST_HALF_BYTE_2      0x00000090
#define ARM_INST_VAL_SWAP                   0x01000090
#define ARM_INST_VAL_LD_ST_MULTIPLE         0x08000000

/* -----------------------------------------------------------------------
** ARM Instruction Masks
** ----------------------------------------------------------------------- */
#define ARM_INST_MASK_LD_ST_IMMEDIATE_OFFSET 0x0e000000
#define ARM_INST_MASK_LD_ST_REGISTER_OFFSET  0x0e000010
#define ARM_INST_MASK_LD_ST_HALF_BYTE        0x0e400050
#define ARM_INST_MASK_LD_ST_HALF_BYTE_2      0x0e400050
#define ARM_INST_MASK_SWAP                   0x0fb000f0
#define ARM_INST_MASK_LD_ST_MULTIPLE         0x0e000000

/* -----------------------------------------------------------------------
** MISC defines
** ----------------------------------------------------------------------- */
#ifdef CUST_EDITION
#define EXCEPTION_DISPLAY_HOLD_SEC 2
#else
#define EXCEPTION_DISPLAY_HOLD_SEC 4
#endif
extern void dload_entry( void );
void boot_record_err_fatal_info(
  word       line,
  const char *file_ptr
  );
void boot_generic_error_handler( void );

#ifdef FEATURE_ERR_EXTENDED_STORE
void boot_process_err_reset_check( void );
#endif

#ifdef FEATURE_ENABLE_FLOATING_POINT_ERRORS
/*===========================================================================

FUNCTION boot_enable_fp_errors

DESCRIPTION
  Configures FP library flags so that FP errors will cause the appropriate
  system signal to be raised.
  
DEPENDENCIES
  RT libraries need to have been initialized prior to calling this function.
  This occurs when the main() function is called.

RETURN VALUE
  None
  
SIDE EFFECTS
  FP environment status flag is altered.

===========================================================================*/
void boot_enable_fp_errors( void );
#endif

boolean boot_exception_handler_is_running ( void );

void boot_keypad_init ( void );

/* If updating enum, also update label array (in boot_trap.c) */
typedef enum 
{
  CAUSE_RESET = 0,
	CAUSE_UNDEF_INSTR,
  CAUSE_SOFTWARE_INT,
	CAUSE_PREFETCH_ABORT,
	CAUSE_DATA_ABORT,
  CAUSE_RESERVED_INT,
  CAUSE_DIV_BY_ZERO,
  CAUSE_UNKNOWN_SIG,
  CAUSE_SYSTEM_EXIT,
  CAUSE_ERROR_FATAL,
  CAUSE_SOFT_RESET,
  CAUSE_PVFN_SIGNAL,
  CAUSE_FPE_INVALID_OP,
  CAUSE_FPE_DIV_BY_ZERO,
  CAUSE_FPE_OVERFLOW,
  CAUSE_FPE_UNDERFLOW,
  CAUSE_FPE_INEXACT,
  CAUSE_UNKNOWN,
	SIZEOF_CAUSE_TYPE
} exception_cause_type;

#endif /* _ARM_ASM_ */

/* These must mimic the exception_cause_type values */
#define ARM_CAUSE_RESET          0
#define ARM_CAUSE_UNDEF_INSTR    1
#define ARM_CAUSE_SOFTWARE_INT   2
#define ARM_CAUSE_PREFETCH_ABORT 3
#define	ARM_CAUSE_DATA_ABORT     4
#define ARM_CAUSE_RESERVED_INT   5

#define ARM_CAUSE_DIV_BY_ZERO    6
#define ARM_CAUSE_UNKNOWN_SIG    7
#define ARM_CAUSE_SYSTEM_EXIT    8
#define ARM_CAUSE_ERROR_FATAL    9
#define ARM_CAUSE_SOFT_RESET     10
#define ARM_CAUSE_PVFN_SIGNAL    11

#define ARM_CAUSE_FPE_INVALID_OP  12
#define ARM_CAUSE_FPE_DIV_BY_ZERO 13
#define ARM_CAUSE_FPE_OVERFLOW    14
#define ARM_CAUSE_FPE_UNDERFLOW   15
#define ARM_CAUSE_FPE_INEXACT     16

#define ARM_CAUSE_UNKNOWN         17

#endif /* END BOOT_TRAP_H */
