/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              Boot Trap Handlers

GENERAL DESCRIPTION
  Handles Boot Trap handlers which intercept exception events other than
  interrupts from the processor.  Specifically, handles data aborts, which
  are non-maskable, and must be present in the boot block.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  No requirements

Copyright (c) 2004-2007 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/qsc1100/drivers/boot/boot_trap.c#1 $ $DateTime: 2009/04/10 12:38:25 $ $Author: ehong $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/18/07   gdp     Changes to support "*"/"#" while in phone is crashed state
04/23/07   amw     Remove banned APIs.
09/14/05   tbg     Added support for FEATURE_LOG_RESET_REGISTERS
08/01/05   wd      L4 Merge.
01/24/05   ck/rp   Killed the PA before calling the function
                   boot_data_abort_handler when the feature
12/06/04   ts      Removed handling for spurious data aborts.
06/24/04   bgc     Added handling for MSM6550 rev A spurious data aborts.
04/15/04   AP      Ported for MSM6550 from MSM6500.

===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "rex.h"
#include "AEEstd.h"
#include "armasm.h"
#include "arm.h"
#include "signal.h"
#include "boot_trap.h"
#include "lcd.h"
#include "err.h"
#include "enc.h"
#include "fenv.h"
#include "hs.h"

#ifdef FEATURE_LOOPING_EXCEPTION_HANDLER
#include <stdio.h>
#include "keypad.h"
#include "clk.h"
#include "boothw.h"
#include "hw.h"
#include "rf.h"
#endif /* FEATURE_LOOPING_EXCEPTION_HANDLER */

#include "rex.h"
#include "rexlog.h"
#ifdef CUST_EDITION
#include "miheader.h"
#endif
#ifdef FEATURE_ERR_EXTENDED_STORE
    extern int err_reset_check( void );
#endif
#ifdef FEATURE_LOOPING_EXCEPTION_HANDLER
    extern void boot_set_abort_mode( void );
#endif
#ifdef FEATURE_SUPPORT_MISALIGNED_MEMORY_ACCESS
extern void boot_restore_misaligned_exception_frame(PARM_TRAP_FRAME frame);
extern boolean boot_is_data_abort_misaligned_access(void);
extern ARM_endiannesstype boot_get_endianness(void);
uint32 saved_abort_stack_pointer;
#endif /* FEATURE_SUPPORT_MISALIGNED_MEMORY_ACCESS*/


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

#ifdef FEATURE_SUPPORT_MISALIGNED_MEMORY_ACCESS

static void memop(
               PARM_TRAP_FRAME frame,
               uint32 op, dword adr,
               uint32 reg, uint32 size
             );
#endif /* FEATURE_SUPPORT_MISALIGNED_MEMORY_ACCESS*/

#ifdef FEATURE_LOOPING_EXCEPTION_HANDLER
extern void *  memset(void * s, int c, size_t n);

#define BOOT_RESET_KEYVAL    (HS_STAR_K)
#define BOOT_DLOAD_KEYVAL    (HS_POUND_K)

#endif /*FEATURE_LOOPING_EXCEPTION_HANDLER*/

static uint32 boot_error_count = 0;

ARM_TRAP_FRAME boot_trap_frame;
static char boot_err_fatal_file[HS_SCRN_WIDE];
static uint32 boot_err_fatal_line = 0;
exception_cause_type boot_generic_cause = CAUSE_UNKNOWN;
exception_cause_type boot_error_cause = CAUSE_UNKNOWN;

/* External declarations */
extern void Error_Fatal(char *error_msg);
#if !defined(FEATURE_L4)
extern rex_tcb_type *rex_curr_task;
#endif
extern LOG_COREDUMP_C_type rex_core;
/* -------------------------------
** Public Code Prototypes.
** ------------------------------- */

#ifdef FEATURE_BOOT_DATA_ABORT_DEBUG
void boot_data_abort_debug(
               PARM_TRAP_FRAME frame
             );
#endif /*FEATURE_BOOT_DATA_ABORT_DEBUG*/

PARM_TRAP_FRAME boot_data_abort_handler_thumb(
               PARM_TRAP_FRAME frame
             );

#ifdef FEATURE_LOOPING_EXCEPTION_HANDLER
void boot_looping_exception_handler(
    PARM_TRAP_FRAME frame,
    exception_cause_type cause
    );

static char disp_stack_format[]  =  "%s0x%8x [%2d] 0x%08x";
static char disp_reg_format[]  =  "%s0x%8x  reg[%2d]   = 0x%08x";
static char disp_misc_format[] =  "%s0x%8x  %-11.11s=0x%08x";
static char disp_err_fatal_format[] =  "%12.12s%12.12s%5d";
                      /* 123456789AB+123456789AB+123456789AB+1234567890B+ */
static char disp_dload_format[] = "%sHold # key   to enter    dload mode";
#endif /* FEATURE_LOOPING_EXCEPTION_HANDLER */

#ifdef FEATURE_ERR_EXTENDED_STORE
static char disp_reset_format[] = "%sHold * key   to reset &  log abort ";
#endif /* FEATURE_ERR_EXTENDED_STORE */


#if defined (FEATURE_LOOPING_EXCEPTION_HANDLER) || defined (FEATURE_ERR_EXTENDED_STORE)
char *exception_cause_label[SIZEOF_CAUSE_TYPE] = {
/* 123456789AB+ */
  "   Reset    ",
  "Undef Instr ",
  "Software INT",
  "Preftch Abrt",
  " Data Abort ",
  "Reserved INT",
  "Div by Zero ",
  "Unknown Sig ",
  "System Exit ",
  "Error Fatal ",
  " Soft Reset ",
  "PVFN Signal ",
  "FP: Invalid ",
  "FP: Div by 0",
  "FP: Overflow",
  "FP: Undrflow",
  "FP: Inexact ",
  "Unknown Err "
};
/*===========================================================================

FUNCTION boot_rexcore_to_armframe

DESCRIPTION
  Convert data in rex_core to an ARM_TRAP_FRAME.  Uses cause parameter to
  copy correct r13, r14, and SPSR values into ARM_TRAP_FRAME.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  'frame' populated with rex_core values.

===========================================================================*/
void boot_rexcore_to_armframe(
  PARM_TRAP_FRAME frame,
  exception_cause_type cause
  )
{
  uint32 i;

  /* Reference unused parameter cause to keep lint happy. */
  (void)cause;

  for( i = REX_SVC_R0; i <= REX_SVC_R12; i++)
  {
    frame->l.r[i-REX_SVC_R0] = rex_core.regs[i];
  }

  /*
  ** PC in rex_core may not tell us much, but we copy it anyway.
  ** Chances are the caller has already adjusted it for the error
  ** in question
  */
  frame->l.r[15] = rex_core.regs[REX_SVC_PC];

  /*
  ** Get CPSR value from rex_core.cpsr
  */
  frame->l.Spsr = rex_core.cpsr;

  /*
  ** Now use cpsr value to copy correct r13 and r14 values to frame
  ** (assumes rex_core.cpsr is the mode of the CPU at the time of the
  ** error).
  */
  switch(rex_core.cpsr & PSR_Mode_Mask)
  {
  case PSR_Supervisor:
    frame->l.r[13] = rex_core.regs[REX_SVC_SP];
    frame->l.r[14] = rex_core.regs[REX_SVC_LR];
    break;

  case PSR_Abort:
    frame->l.r[13] = rex_core.regs[REX_ABT_SP];
    frame->l.r[14] = rex_core.regs[REX_ABT_LR];
    break;

  case PSR_User:
  case PSR_System:
    frame->l.r[13] = rex_core.regs[REX_SYS_SP];
    frame->l.r[14] = rex_core.regs[REX_SYS_LR];
    break;

  case PSR_Irq:
    frame->l.r[13] = rex_core.regs[REX_IRQ_SP];
    frame->l.r[14] = rex_core.regs[REX_IRQ_LR];
    break;

  case PSR_Undef:
    frame->l.r[13] = rex_core.regs[REX_UDF_SP];
    frame->l.r[14] = rex_core.regs[REX_UDF_LR];
    break;

  case PSR_Fiq:
    for(i=REX_FIQ_R8; i<=REX_FIQ_LR; i++)
    {
      frame->l.r[(i-REX_FIQ_R8)+8] = rex_core.regs[i];
    }
    break;

  default:
    break;
  }
}
#endif


#ifdef FEATURE_SUPPORT_MISALIGNED_MEMORY_ACCESS

/*===========================================================================

FUNCTION mem_op

DESCRIPTION
  Function to manually perform misaligned memory accesses.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void  memop(
            PARM_TRAP_FRAME frame,
            uint32 op, dword adr,
            uint32 reg, uint32 size
          )

     /*************************************
        Execute aborted memory operations.
        args: frame = execution context
        op    = 1 for load, 0 for store
        adr   = effective address of memory operation.
        reg   = target register for operation
        size  = size in bytes (1, 2, or 4) of operation.
        ************************************************/
{
  ARM_endiannesstype endianness;

  if(((adr > IO_BASE)||(adr == IO_BASE)) && ((frame->l.Spsr & PSR_Mode_Mask) == PSR_User))
  {
              /* 123456789AB+123456789AB+123456789AB+123456789AB+ */
    Error_Fatal(" USER Mode     Error:   Illegal Mem    Access   ");
  }

  /* Find the endianness of the processor.*/
   endianness = boot_get_endianness();
  switch (endianness)
  {
  case LITTLE_ENDIAN:
      switch(size)
      {
        /* word */
      case 4:
        if(op) {  /* load */

          frame->l.r[reg]  = (dword) (*((byte *) (adr + 0)) << 0);
          frame->l.r[reg] |= (dword) (*((byte *) (adr + 1)) << 8);
          frame->l.r[reg] |= (dword) (*((byte *) (adr + 2)) << 16);
          frame->l.r[reg] |= (dword) (*((byte *) (adr + 3)) << 24);

        } else { /* store */

          *((byte *) (adr + 0)) = (frame->l.r[reg] & 0x000000ff) >> 0;
          *((byte *) (adr + 1)) = (frame->l.r[reg] & 0x0000ff00) >> 8;
          *((byte *) (adr + 2)) = (frame->l.r[reg] & 0x00ff0000) >> 16;
          *((byte *) (adr + 3)) = (frame->l.r[reg] & 0xff000000) >> 24;
        }
        break;

      case 2:
        /* half-word */
        if(op) {  /* load */

          frame->l.r[reg]  = (dword) (*((byte *) (adr + 0)) << 0);
          frame->l.r[reg] |= (dword) (*((byte *) (adr + 1)) << 8);

        } else {  /* store */

          *((byte *) (adr + 0)) = (byte) ((frame->l.r[reg] & 0x00ff) >> 0);
          *((byte *) (adr + 1)) = (byte) ((frame->l.r[reg] & 0xff00) >> 8);

        }
        break;

      case 1:
        /* byte */
        /* This really shouldn't happen for a byte. */

        if(op) frame->l.r[reg] = *((byte *) adr);
        else *((byte *) adr) = frame->l.r[reg];

        break;

      default:
                  /* 123456789AB+123456789AB+123456789AB+123456789AB+ */
        Error_Fatal("  Invalid     Parameter   In Memop  ");
        break;
      }
      break;
  case BIG_ENDIAN:
      switch(size)
      {
        /* word */
      case 4:
        if(op) {  /* load */

          frame->l.r[reg]  = (dword) (*((byte *) (adr + 3)) << 0);
          frame->l.r[reg] |= (dword) (*((byte *) (adr + 2)) << 8);
          frame->l.r[reg] |= (dword) (*((byte *) (adr + 1)) << 16);
          frame->l.r[reg] |= (dword) (*((byte *) (adr + 0)) << 24);

        } else { /* store */

          *((byte *) (adr + 3)) = (frame->l.r[reg] & 0x000000ff) >> 0;
          *((byte *) (adr + 2)) = (frame->l.r[reg] & 0x0000ff00) >> 8;
          *((byte *) (adr + 1)) = (frame->l.r[reg] & 0x00ff0000) >> 16;
          *((byte *) (adr + 0)) = (frame->l.r[reg] & 0xff000000) >> 24;
        }
        break;

      case 2:
        /* half-word */
        if(op) {  /* load */

          frame->l.r[reg]  = (dword) (*((byte *) (adr + 1)) << 0);
          frame->l.r[reg] |= (dword) (*((byte *) (adr + 0)) << 8);

        } else {  /* store */

          *((byte *) (adr + 1)) = (byte) ((frame->l.r[reg] & 0x00ff) >> 0);
          *((byte *) (adr + 0)) = (byte) ((frame->l.r[reg] & 0xff00) >> 8);

        }
        break;

      case 1:
        /* byte */
        /* This really shouldn't happen for a byte. */

        if(op) frame->l.r[reg] = *((byte *) adr);
        else *((byte *) adr) = frame->l.r[reg];

        break;

      default:
                  /* 123456789AB+123456789AB+123456789AB+123456789AB+ */
        Error_Fatal("  Invalid     Parameter   In Memop  ");
        break;
      }
      break;

  }
}


/*===========================================================================

FUNCTION boot_data_abort_handler_thumb

DESCRIPTION
  Handler called for a data abort from the processor.  Specifically, this
  function handles misaligned memory accesses which occur in the 16-bit
  Thumb mode of the processor.  It decodes the memory instruction that
  caused the abort and then manually executes it.

DEPENDENCIES
  None

RETURN VALUE
  The trap frame

SIDE EFFECTS
  None

===========================================================================*/
PARM_TRAP_FRAME
boot_data_abort_handler_thumb(PARM_TRAP_FRAME frame)
{
  ThumbInst instr;
  dword sreg, dreg, offset, size, op, adr, rlist, x, opcode1_thumb,opcode2_thumb;

  /* Get the offending instruction. */
  adr = frame->l.r[15];
  adr &=0xfffffffe;
  instr.l = *((word *) adr);
  opcode1_thumb = instr.l & THUMB_INST_MASK_7BIT_OPCODE;

  switch (opcode1_thumb)
  {
  case THUMB_INST_VAL_LD_W_R_OFFSET:

      sreg   = instr.ld_w_r_offset.rn;
      dreg   = instr.ld_w_r_offset.rd;
      offset = frame->l.r[instr.ld_w_r_offset.rm];
      size   = 4;
      op     = 1;
      adr    = frame->l.r[sreg] + offset;
      /* Do the load or store. */
      memop(frame, op, adr, dreg, size);
      break;

  case THUMB_INST_VAL_LD_UHW_R_OFFSET:

      sreg   = instr.ld_uhw_r_offset.rn;
      dreg   = instr.ld_uhw_r_offset.rd;
      offset = frame->l.r[instr.ld_uhw_r_offset.rm];
      size   = 2;
      op     = 1;
      adr    = frame->l.r[sreg] + offset;

      /* Do the load or store. */
      memop(frame, op, adr, dreg, size);
      break;

  case THUMB_INST_VAL_LD_SHW_R_OFFSET:

      sreg   = instr.ld_shw_r_offset.rn;
      dreg   = instr.ld_shw_r_offset.rd;
      offset = frame->l.r[instr.ld_shw_r_offset.rm];
      size   = 2;
      op     = 1;
      adr    = frame->l.r[sreg] + offset;

      /* Do the load or store. */
      memop(frame, op, adr, dreg, size);

      if(frame->l.r[dreg] & 0x00008000)
        frame->l.r[dreg] |= 0xffff0000;
      break;

  case THUMB_INST_VAL_STR_HW_R_OFFSET:

      sreg   = instr.str_hw_r_offset.rn;
      dreg   = instr.str_hw_r_offset.rd;
      offset = frame->l.r[instr.str_hw_r_offset.rm];
      size   = 2;
      op     = 0;
      adr    = frame->l.r[sreg] + offset;

      /* Do the load or store. */
      memop(frame, op, adr, dreg, size);
      break;

  case THUMB_INST_VAL_POP_STACK:
      sreg   = 13;
      rlist  = instr.pop_stack.register_list;
      size   = 4;
      op     = 1;
      adr    = frame->l.r[sreg];

      /* Do the load or store. */
      for(x=0; x<8; x++)
      {
        if((1 << x) & rlist)
        {
           memop(frame, op, adr, x, size);
           adr+=4;
        }
      }
      if (instr.pop_stack.r == 1)
      {
          memop(frame, op, adr, 15, size);
          adr+=4;
      }
      frame->l.r[sreg] = adr;
      break;

  case THUMB_INST_VAL_PUSH_STACK:

      sreg   = 13;
      rlist  = instr.push_stack.register_list;
      size   = 4;
      op     = 0;
      adr    = frame->l.r[sreg];

      for(x=0; x<8; x++)
      {
        /* sreg was already updated so back it up. */
        if((1 << x) & rlist)
        {
            adr -=4;
        }
      }
      if (instr.pop_stack.r == 1)
      {
          adr -=4;
      }
      frame->l.r[sreg] = adr;
      /* Do the load or store. */
      for(x=0; x<8; x++)
      {
        if((1 << x) & rlist)
        {
            memop(frame, op, adr, x, size);
            adr+=4;
        }
      }
      if (instr.pop_stack.r == 1)
      {
          memop(frame, op, adr, 14, size);
          adr+=4;
      }
      break;

  default:
      opcode2_thumb = instr.l & THUMB_INST_MASK_5BIT_OPCODE;
      switch (opcode2_thumb)
      {
      case THUMB_INST_VAL_LD_W_I_OFFSET:
          sreg   = instr.ld_w_i_offset.rn;
          dreg   = instr.ld_w_i_offset.rd;
          offset = (instr.ld_w_i_offset.offset) <<2;
          size   = 4;
          op     = 1;
          adr    = frame->l.r[sreg] + offset;
          /* Do the load or store. */
          memop(frame, op, adr, dreg, size);
          break;

      case THUMB_INST_VAL_LD_W_PC_RELATIVE:
          sreg   = 15;
          dreg   = instr.ld_w_pc_relative.rd;
          offset = (instr.ld_w_pc_relative.offset) <<2;
          size   = 4;
          op     = 1;
          adr    = (frame->l.r[sreg] & 0xfffffffc) <<1 + offset;
          /* Do the load or store. */
          memop(frame, op, adr, dreg, size);
          break;

      case THUMB_INST_VAL_LD_W_SP_RELATIVE:
          sreg   = 13;
          dreg   = instr.ld_w_sp_relative.rd;
          offset = (instr.ld_w_sp_relative.offset) <<2;
          size   = 4;
          op     = 1;
          adr    = frame->l.r[sreg] + offset;
          /* Do the load or store. */
          memop(frame, op, adr, dreg, size);
          break;

      case THUMB_INST_VAL_LD_UHW_I_OFFSET:
          sreg   = instr.ld_uhw_i_offset.rn;
          dreg   = instr.ld_uhw_i_offset.rd;
          offset = (instr.ld_uhw_i_offset.offset) << 1;
          size   = 2;
          op     = 1;
          adr    = frame->l.r[sreg] + offset;
          /* Do the load or store. */
          memop(frame, op, adr, dreg, size);
          break;

      case THUMB_INST_VAL_LDM_INC_AFTER:

          sreg   = instr.ldm_inc_after.rn;
          rlist  = instr.ldm_inc_after.register_list;
          size   = 4;
          op     = 1;
          adr    = frame->l.r[sreg];
          /* Do the load or store. */
          for(x=0; x<8; x++)
          {
            if((1 << x) & rlist)
            {
                memop(frame, op, adr, x, size);
                adr +=4;
            }
          }
          frame->l.r[sreg] = adr;
          break;

      case THUMB_INST_VAL_STR_W_I_OFFSET:
          sreg   = instr.str_w_i_offset.rn;
          dreg   = instr.str_w_i_offset.rd;
          offset = (instr.str_w_i_offset.offset) <<2;
          size   = 4;
          op     = 0;
          adr    = frame->l.r[sreg] + offset;
          /* Do the load or store. */
          memop(frame, op, adr, dreg, size);
          break;
      case THUMB_INST_VAL_STR_W_SP_RELATIVE:
          sreg   = 13;
          dreg   = instr.str_w_sp_relative.rd;
          offset = (instr.str_w_sp_relative.offset) <<2;
          size   = 4;
          op     = 0;
          adr    = frame->l.r[sreg] + offset;
          /* Do the load or store. */
          memop(frame, op, adr, dreg, size);
          break;
      case THUMB_INST_VAL_STR_HW_I_OFFSET:
          sreg   = instr.str_hw_i_offset.rn;
          dreg   = instr.str_hw_i_offset.rd;
          offset = (instr.str_hw_i_offset.offset) << 1;
          size   = 2;
          op     = 0;
          adr    = frame->l.r[sreg] + offset;
          /* Do the load or store. */
          memop(frame, op, adr, dreg, size);
          break;
      case THUMB_INST_VAL_STM_INC_AFTER:

          sreg   = instr.stm_inc_after.rn;
          rlist  = instr.stm_inc_after.register_list;
          size   = 4;
          op     = 0;
          adr    = frame->l.r[sreg];
          /* Do the load or store. */
          for(x=0; x<8; x++)
          {
            if((1 << x) & rlist)
            {
                memop(frame, op, adr, x, size);
                adr +=4;
            }
          }
          break;
      default:
          Error_Fatal("   Cannot   Recover From Data Abort ");
          break;
      }
      break;
  }
  /* Move pc */
  frame->l.r[15] += 2;
  return frame;
}
#endif/* FEATURE_SUPPORT_MISALIGNED_MEMORY_ACCESS */

/*===========================================================================

FUNCTION boot_exception_pc_adjustment

DESCRIPTION
  Returns the amount needed to adjust the LR/PC value in the case of an
  exception.  Uses cpsr parameter to determine if ARM or THUMB mode was
  active (when appropriate).

DEPENDENCIES
  None

RETURN VALUE
  (int) Value needed to offset PC

SIDE EFFECTS
  None

===========================================================================*/

int boot_exception_pc_adjustment(
  exception_cause_type cause,
  uint32 cpsr)
{
  int offset = 0;
  /* Assumes rex_core is valid */
  switch(cause)
  {
  case CAUSE_UNDEF_INSTR:
    /* For undefined instructions, PC depends on mode */
    /* For thumb, PC = LR - 2; For arm, PC = LR - 4;  */
    if(cpsr & PSR_Thumb)
    {
      offset = 2;
    }
    else
    {
      offset = 4;
    }
    break;

  case CAUSE_PREFETCH_ABORT:
    offset = 4;
    break;

  case CAUSE_DATA_ABORT:
    offset = 8;
    break;

  default:
    break;
  }
  return offset;
}

/*===========================================================================

FUNCTION boot_validate_cause

DESCRIPTION
  If cause is valid, returns the same value.  If cause is out of range, returns
  the value 'CAUSE_UNKNOWN'.  Used to validate a cause value before using it
  as an array index.

DEPENDENCIES
  None

RETURN VALUE
  exception_cause_type

SIDE EFFECTS
  None

===========================================================================*/

exception_cause_type
boot_validate_cause(
  exception_cause_type cause
  )
{
  if(cause >= SIZEOF_CAUSE_TYPE)
  {
    return CAUSE_UNKNOWN;
  }
  else
  {
    return cause;
  }
}


/*===========================================================================

FUNCTION boot_default_exception_handler

DESCRIPTION
  Handler to take care of all exceptions.  Assumes that the initial exception
  handlers (located in boot_exceptions_handlers.s) has called rex_jettison_core,
  and that the 'cause' value corresponding to the exception and the LR value
  at the time of the exception are passed in as parameters.  Will populate
  an ARM_TRAP_FRAME with the values from rex_core, and (depending on which
  features are enabled) display a message to the screen.

DEPENDENCIES
  None

RETURN VALUE
  Does not return

SIDE EFFECTS
  None

===========================================================================*/

void boot_default_exception_handler(
  exception_cause_type cause,
  uint32 orig_r14
  )
{
    
#ifdef USES_RELEASE_VERSION
  hw_reset ();
#else
#ifndef FEATURE_L4

#if !defined( FEATURE_LOOPING_EXCEPTION_HANDLER) && !defined(FEATURE_L4)
  static char text[49];
#endif
  PARM_TRAP_FRAME frame = &boot_trap_frame;

  cause = boot_validate_cause(cause);
  /*----------------
  Adjustments to rex_core values needed for Data Aborts, Undefined Instructions,
  and Prefetch Aborts.  We need to adjust the cpsr value, along with the PC and
  LR values.
  -----------------*/
  switch(cause)
  {
  case CAUSE_DATA_ABORT:
    rex_core.cpsr = rex_core.regs[REX_ABT_SPSR];
    rex_core.regs[REX_SVC_PC] = ( orig_r14 -
    boot_exception_pc_adjustment(cause,rex_core.regs[REX_ABT_SPSR]) );
    rex_core.regs[REX_ABT_LR] = orig_r14;

#ifdef FEATURE_SUPPORT_MISALIGNED_MEMORY_ACCESS
    if (boot_is_data_abort_misaligned_access())
    {
        boot_rexcore_to_armframe(frame,cause);
        frame = boot_data_abort_handler_thumb(frame);
        boot_restore_misaligned_exception_frame(frame);
    }
#endif /* FEATURE_SUPPORT_MISALIGNED_MEMORY_ACCESS */
    break;
  case CAUSE_PREFETCH_ABORT:
    rex_core.cpsr = rex_core.regs[REX_ABT_SPSR];
    rex_core.regs[REX_SVC_PC] = ( orig_r14 -
      boot_exception_pc_adjustment(cause,rex_core.regs[REX_ABT_SPSR]) );
    rex_core.regs[REX_ABT_LR] = orig_r14;
    break;

  case CAUSE_UNDEF_INSTR:
    rex_core.cpsr = rex_core.regs[REX_UDF_SPSR];
    rex_core.regs[REX_SVC_PC] = ( orig_r14 -
      boot_exception_pc_adjustment(cause,rex_core.regs[REX_UDF_SPSR]) );
    rex_core.regs[REX_UDF_LR] = orig_r14;
    break;

  default:
    /* For all other cases, no modifications to rex_core are needed. */
    break;
  }
#ifdef FEATURE_ERR_EXTENDED_STORE
  err_store_info(exception_cause_label[cause],0);
#endif

  boot_rexcore_to_armframe(frame,cause);

#ifdef FEATURE_LOOPING_EXCEPTION_HANDLER
  /* No return from this function call */
  boot_looping_exception_handler(frame,cause);
#elif !defined(FEATURE_L4)
  (void)std_strlprintf(text, sizeof(text), "%-12.12stcb:%08xr14:%08xr15:%08x",
          exception_cause_label[cause], (int)rex_curr_task,
          (int)frame->l.r[14], (int)frame->l.r[15]);

  Error_Fatal(text);
#endif /* FEATURE_LOOPING_EXCEPTION_HANDLER */
#else
#error code not present
#endif /* FEATURE_L4 */
#endif //#ifdef USES_RELEASE_VERSION
}


/*===========================================================================

FUNCTION boot_default_signal_handler

DESCRIPTION
  Handler to take care of all system signals.  Assumes that interrupts are
  disabled (done by earlier handlers for the specific signals being used),
  and that the cause value corresponds to the signal which occurred.  Will call
  rex_jettison_core, populate an ARM_TRAP_FRAME with the values from rex_core,
  and (depending on which features are enabled) display a message to the screen.

DEPENDENCIES
  None

RETURN VALUE
  Does not return

SIDE EFFECTS
  None

===========================================================================*/


void boot_default_signal_handler(exception_cause_type cause)
{
#if defined(FEATURE_LOOPING_EXCEPTION_HANDLER)
  PARM_TRAP_FRAME frame = &boot_trap_frame;
#elif  !defined(FEATURE_L4)
  static char text[49];
#endif

  cause = boot_validate_cause(cause);

  rex_jettison_core();
#ifdef FEATURE_ERR_EXTENDED_STORE
  err_store_info(exception_cause_label[cause],0);
#endif

#ifdef FEATURE_LOOPING_EXCEPTION_HANDLER
  boot_rexcore_to_armframe(frame,cause);
  /* No return from this function call */
  boot_looping_exception_handler(frame,cause);
#elif !defined(FEATURE_L4)
  (void)std_strlprintf(text, sizeof(text), "%-12.12s           tcb:%08x",
          exception_cause_label[cause],(int)rex_curr_task);

  Error_Fatal(text);
#endif
}


/*===========================================================================

FUNCTION boot_record_err_fatal_info

DESCRIPTION
  When an ERR_FATAL occurs, this function should be called to save the
  file name and line number of the ERR_FATAL.  This is so the information
  can later be displayed to the screen by the appropriate handler.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sets boot_generic_cause to CAUSE_ERROR_FATAL.

===========================================================================*/

void boot_record_err_fatal_info(
  word       line,
  const char *file_ptr
  )
{
  uint32 i;

  /* Copy the filename to the LCD message buffer, max HS_SCRN_WIDE */
  for (i = 0; i < HS_SCRN_WIDE && file_ptr[i] != '\0'; i++)
    boot_err_fatal_file[i] = file_ptr[i];

  /* Pad the line with spaces */
  for ( ; i < HS_SCRN_WIDE; i++)
    boot_err_fatal_file[i] = ' ';

  boot_err_fatal_line = line;

  /* Set the generic cause so we know this is an ERR_FATAL when the */
  /* generic handler is later called.  This will allow us to print  */
  /* the file and line number in the looping exception handler.     */
  boot_generic_cause = CAUSE_ERROR_FATAL;
}

/*===========================================================================

FUNCTION boot_generic_error_handler

DESCRIPTION
  Called from diag_panic_end after an error has occurred, which is typically
  after an ERR_FATAL.  If not an ERR_FATAL (as indicated by boot_generic_cause).
  rex_jettison_core() is called and an ARM_TRAP_FRAME is populated (for ERR_FATALs,
  rex_jettison_core should have already been called, and an ARM_TRAP_FRAME is not
  needed).


DEPENDENCIES
  boot_record-err_fatal_info must be called prior to this function in the
  case of an ERR_FATAL.

RETURN VALUE
  Does not return

SIDE EFFECTS
  None

===========================================================================*/

void boot_generic_error_handler()
{
#if !defined( FEATURE_LOOPING_EXCEPTION_HANDLER) && !defined(FEATURE_L4)
  static char text[49];
#endif

  /* Make sure interrupts are disabled */
  _disable();

  boot_generic_cause = boot_validate_cause(boot_generic_cause);

  /* For ERR_FATALs, this has already been done. */
  if(boot_generic_cause != CAUSE_ERROR_FATAL)
  {
    rex_jettison_core();
#ifdef FEATURE_ERR_EXTENDED_STORE
    err_store_info(exception_cause_label[boot_generic_cause],0);
#endif
  }

#ifdef FEATURE_LOOPING_EXCEPTION_HANDLER
  boot_looping_exception_handler(NULL,boot_generic_cause);
#elif !defined(FEATURE_L4)

  if(boot_generic_cause == CAUSE_ERROR_FATAL)
  {
    (void)std_strlprintf(text, sizeof(text), disp_err_fatal_format,
            exception_cause_label[boot_generic_cause],
            boot_err_fatal_file,boot_err_fatal_line);
  }
  else
  {
    (void)std_strlprintf(text, sizeof(text), "%-12.12s           tcb:%08x",
            exception_cause_label[boot_generic_cause], (int)rex_curr_task);
  }

  Error_Fatal(text);
#endif
}


/*===========================================================================

FUNCTION  __default_signal_handler_c

DESCRIPTION
  Determines type of signal and calls boot_default_signal_handler.

DEPENDENCIES
  None

RETURN VALUE
  Does not return

SIDE EFFECTS
  None

===========================================================================*/
void __default_signal_handler_c(
  int signal,
  int argument
)
{
  exception_cause_type cause = CAUSE_UNKNOWN;

  if (SIGFPE == signal)
  {
    /* Compare argument with mask, in case more than one exception occurred
     * at the same time. */
    if(argument & DIVBYZERO)
    {
      cause = CAUSE_DIV_BY_ZERO;
    }
    else if (argument & FE_EX_DIVBYZERO)
    {
      cause = CAUSE_FPE_DIV_BY_ZERO;
    }
    else if (argument & FE_EX_INVALID)
    {
      cause = CAUSE_FPE_INVALID_OP;
    }
    else if (argument & FE_EX_OVERFLOW)
    {
      cause = CAUSE_FPE_OVERFLOW;
    }
    else if (argument & FE_EX_INEXACT)
    {
      cause = CAUSE_FPE_INEXACT;
    }
    else if (argument & FE_EX_UNDERFLOW)
    {
      cause = CAUSE_FPE_UNDERFLOW;
    }
  }
  else if (SIGPVFN == signal)
  {
    cause = CAUSE_PVFN_SIGNAL;
  }

  boot_default_signal_handler(cause);
}


/*===========================================================================

FUNCTION boot_keypress_check

DESCRIPTION
  Polls the keypad checking for keypresses which can be used to reset or enter
  download mode.  Because some platforms do not support multiple keypresses,
  this remembers the last value that was polled, and compares it to the current
  value (time between polling is roughly 1 sec).  If identical, it assumes that
 key has been held down and the appropriate action is taken.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  May reset phone or enter download mode,
  depending on key(s) being held down.

===========================================================================*/

#ifdef FEATURE_LOOPING_EXCEPTION_HANDLER
void boot_keypress_check()
{
   hs_key_type keyval = HS_NONE_K;
   static hs_key_type keyval_prev = HS_NONE_K;
    
   if(keypad_is_key_pressed(BOOT_RESET_KEYVAL))
   {
      keyval = BOOT_RESET_KEYVAL;
   }
   else if(keypad_is_key_pressed(BOOT_DLOAD_KEYVAL))
   {
      keyval = BOOT_DLOAD_KEYVAL;
   }

   if ((HS_NONE_K != keyval) && (keyval == keyval_prev))
   {
      /* We have apparently been holding down the same key for some time,
       * so take associated recovery action */
      if (BOOT_RESET_KEYVAL == keyval)
      {
                  /* 123456789AB+123456789AB+123456789AB+123456789AB+ */
        lcd_message("             Resetting.                         ");
        hw_reset();
      }
      else if(BOOT_DLOAD_KEYVAL == keyval)
      {
                  /* 123456789AB+123456789AB+123456789AB+123456789AB+ */
        lcd_message("  Entering    download     mode.                ");
        dload_entry();
      }
   }

   /* Retain current keyval to determine if the same key has been held
    * through at least two keypad poll operations */
   keyval_prev = keyval;
}


/*===========================================================================

FUNCTION boot_looping_delay

DESCRIPTION
  Used to control the amount of delay needed when display messages and other
  information to the LCD.  Polls the keypad during the delay using
  boot_keypress_check.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Keypad is polled once every second.

===========================================================================*/

void boot_looping_delay(uint16 delay_sec)
{
  static uint32 j,i;

  for(i=0; i < (delay_sec); i++)
  {
    /* scan keypad every one second */
    boot_keypress_check();

    /* Need to kick the watchdog every 213ms so we kick it every 100ms
     * to be safe*/
    for (j=0; j < 10; j++ )
    {
      BOOTHW_KICK_WATCHDOG();
      clk_busy_wait(100000);
    }
  }
}



/*===========================================================================

FUNCTION boot_looping_exception_handler

DESCRIPTION
  Handler called for all exceptions and errors.  Will display type of error
  to the LCD, as well as register values and other information (depending on
  the type of error that occurred)

DEPENDENCIES
  Valid stack pointer.

RETURN VALUE
  Does not return

SIDE EFFECTS
  Code will remain in a loop displaying each register (and other information
    for EXCEPTION_DISPLAY_HOLD_SEC seconds.  Display loop will be exited if the
    correct combination of keys are pressed on the keypad.

===========================================================================*/
void boot_looping_exception_handler(
    PARM_TRAP_FRAME frame,
    exception_cause_type cause
    )
{
  /*48 characters fit in the lcd display plus a null*/
  static char text[49];
  static uint32 boot_handler_orig_cpsr = 0;
  static volatile uint32 i;

  /* Increment error count.  Can use this count to catch nested/repeating
   * error conditions and take another course of action, i.e. enter
   * download mode.  Currently we just record the number of errors for
   * debug purposes.
   */
  boot_error_count++;

  /*
    Make sure that the cause value is not out-of-range before using it to
    access the exception_cause_label array.
  */
  boot_error_cause = boot_validate_cause(cause);

  /* make sure the PA is turned off */
 //  enc_kill_pa();

  /*
    At this point, interrupts should be disabled and this code should be
    able to run uninterrupted.  However, we will explicitly disable
    interrupts here to be safe and ensure that they are disabled.
    */
  _disable();
  keypad_init_matrix();

  /* Change modes here if needed */
  boot_handler_orig_cpsr = rex_get_cpsr();

  if(((boot_handler_orig_cpsr & PSR_Mode_Mask) != PSR_Abort) &&
     ((boot_handler_orig_cpsr & PSR_Mode_Mask) != PSR_Undef))
  {
    /* We use our own function here rather than rex_set_cpsr() because
     * we may want to do more than switch modes, i.e. we may choose
     * to also reset the stack pointer.
     */
      boot_set_abort_mode();
  }

  /* Loop forever displaying the different registers with a two second pause */
  while(TRUE)
  {

    if(boot_error_cause == CAUSE_ERROR_FATAL)
    {
      /* ERR_FATAL is a special case, and requires it's own formatting */
      /* If it is an ERR_FATAL, we will output the file/line first.    */
      memset (text, ' ', (sizeof(text)-1));
      (void)std_strlprintf(text, sizeof(text), disp_err_fatal_format,
              exception_cause_label[boot_error_cause], boot_err_fatal_file,
              boot_err_fatal_line);
      lcd_message(text);
      boot_looping_delay(EXCEPTION_DISPLAY_HOLD_SEC);
    }

#ifdef FEATURE_ERR_EXTENDED_STORE
    /*Output reset message */
    memset (text, ' ', (sizeof(text)-1));
    (void)std_strlprintf(text, sizeof(text), disp_reset_format,
            exception_cause_label[boot_error_cause]);
    lcd_message(text);
    boot_looping_delay(EXCEPTION_DISPLAY_HOLD_SEC);
#endif /* FEATURE_ERR_EXTENDED_STORE */

    /*Output dload message */
    memset (text, ' ', (sizeof(text)-1));
    (void)std_strlprintf(text, sizeof(text), disp_dload_format,
            exception_cause_label[boot_error_cause]);
    lcd_message(text);
    boot_looping_delay(EXCEPTION_DISPLAY_HOLD_SEC);

    /* Check frame pointer, just to make sure we don't cause another abort. */
    /* Also, some errors may choose not to pass in a valid frame structure. */
    if(frame != NULL)
    {
      /*Output rex_curr_task also */
      memset (text, ' ', (sizeof(text)-1));
#ifdef FEATURE_L4
#error code not present
#else
      (void)std_strlprintf(text, sizeof(text), disp_misc_format,
              exception_cause_label[boot_error_cause],
              frame->l.r[15],"tcb",rex_curr_task);
#endif
      lcd_message(text);
      boot_looping_delay(EXCEPTION_DISPLAY_HOLD_SEC);

      /* display registers r0-r15
       * r[15] contains the instruction of the address that caused
       * the data abort*/
      for( i = 0; i < 16; i++ )
      {
        memset (text, ' ', (sizeof(text)-1));
        (void)std_strlprintf(text, sizeof(text), disp_reg_format,
                exception_cause_label[boot_error_cause],
                frame->l.r[15],i,frame->l.r[i]);
        lcd_message(text);
        boot_looping_delay(EXCEPTION_DISPLAY_HOLD_SEC);
      }
      /*Output the CPSR also */
      /*CPSR = SPSR in data aborts*/
      memset (text, ' ', (sizeof(text)-1));
      (void)std_strlprintf(text, sizeof(text), disp_misc_format,
              exception_cause_label[boot_error_cause],
              frame->l.r[15],"CPSR", frame->l.Spsr);
      lcd_message(text);
      boot_looping_delay(EXCEPTION_DISPLAY_HOLD_SEC);
#ifdef CUST_EDITION
      if(rex_curr_task)
      {
        volatile uint32 *pStackEnd = (uint32 *)rex_curr_task->stack_limit+rex_curr_task->stack_size;
        volatile uint32 *pStack = (uint32 *)frame->l.r[13];
        
        // LOOP Stack
        for( i = 0; pStack < pStackEnd; pStack++)
        {
          if(((*pStack)&0x1) == 0 
            ||(*pStack) > FLASH_NOR_EFS2_START_BYTE_OFFSET 
            ||(*pStack) < MI_NOR_AMSS_BOOT_ELF_HEADER_SRC_ADDR)
          {
            continue;
          }
          
          memset (text, ' ', (sizeof(text)-1));
          (void)std_strlprintf(text, sizeof(text), disp_stack_format,
                  exception_cause_label[boot_error_cause],
                  frame->l.r[15],i,(*pStack)-1);
          lcd_message(text);
          boot_looping_delay(EXCEPTION_DISPLAY_HOLD_SEC);
          i++;
        }
      }
#endif
    }
  }
}

#endif /*FEATURE_LOOPING_EXCEPTION_HANDLER*/

boolean boot_exception_handler_is_running()
{
  return (boolean)boot_error_count;
}

#ifdef FEATURE_ERR_EXTENDED_STORE

#ifdef FEATURE_DEBUG_TRAMP_EXECUTION
#error code not present
#endif

void boot_process_err_reset_check(void)
{
#ifdef FEATURE_LOG_RESET_REGISTERS
  /*
   * Due to how register values are preserved upon reset, we need to
   * rearrange some of the values saved by rex_jettison_core.
   */
  rex_core.regs[REX_SVC_PC] = 0;
  rex_core.regs[REX_SVC_LR] = rex_core.regs[REX_FIQ_R12];
  rex_core.regs[REX_SVC_SP] = rex_core.regs[REX_FIQ_R11];
#else
  uint32 i;
  /* Register contents not valid on a reset -- zero out structure to avoid confusion */
  for(i=0; i<SIZEOF_LOG_COREDUMP_REGISTERS;i++)
  {
    rex_core.regs[i] = 0;
  }
#endif

#if 0
  err_add_to_mem_table("RESET_STATUS",
    (void *)HWIO_ADDR(RESET_STATUS),4);
#endif

#ifdef FEATURE_DEBUG_TRAMP_EXECUTION
#error code not present
#endif

  err_reset_check();
#if 0
  err_delete_from_mem_table((void *)HWIO_ADDR(RESET_STATUS));
#endif

#ifdef FEATURE_DEBUG_TRAMP_EXECUTION
#error code not present
#endif
}

#endif



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
#ifdef FEATURE_ENABLE_FLOATING_POINT_ERRORS
void boot_enable_fp_errors( void )
{
  fenv_t env;

  /*
   * Currently set to enable the following FP errors:
   *    FP DIVIDE BY ZERO
   *    FP INVALID OPERATION
   *    FP OVERFLOW
   */

  fegetenv(&env);
  env.statusword |= (FE_IEEE_MASK_DIVBYZERO);
  fesetenv(&env);

}
#endif /* FEATURE_ENABLE_FLOATING_POINT_ERRORS */

