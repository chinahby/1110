#ifndef TRAMPI_H
#define TRAMPI_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               T R A M P O L I N E    S E R V I C E S

GENERAL DESCRIPTION
  This file contains the internal definitions for the Trampoline Services 
  that are provided on MSMs that have a Prioritized Interrupt Controller 
  (PIC).

  Specifically this file contains the types used for creating the tables 
  used to configure the and maintain the interrupts.

EXTERNALIZED FUNCTIONS


INTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  Function tramp_init() should be called before any of the Tramp Services
  can be used.

Copyright (c) 2000, 2001, 2004 - 2008 by QUALCOMM Incorporated.
All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/tramp/trampi.h#4 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/20/07   vph     Ported from QSC6055.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/* Customer Feature definitions */
#include "customer.h"

/*  This file also #includes TRAMPI_MSM_H at the bottom of the file   */


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/* Some basic mask definitions */
#define CLEAR_ALL_MASK    0xFFFFFFFF
#define DISABLE_ALL_MASK  0x00000000
#define ALL_ZEROS_MASK    0x00000000
#define TRAMP_PRIO_MASK         0x07
#define TRAMP_MASK_ALL    0xFFFFFFFF

/* Most interrupt registers need their status bits cleared before calling
 * the ISR. A few need it cleared after.  In particular, some HW devices
 * keep the line high until they've been cleared at the source.
 */
typedef enum
{
  TRAMP_CLR_BEFORE,
  TRAMP_CLR_AFTER
} tramp_clr_whn_type;

/* The PIC hardware supports 8 different priority levels. The higher the 
 * priority number the higher the priority. This enum captures this
 * with TRAMP_PRIO_LVL_0 being the lowest priority and TRAMP_PRIO_LVL_7
 * being the highest priority.
 */
typedef enum
{
  TRAMP_PRIO_LVL_0 = 0,
  TRAMP_PRIO_LVL_1 = 1,
  TRAMP_PRIO_LVL_2 = 2,
  TRAMP_PRIO_LVL_3 = 3,
  TRAMP_PRIO_LVL_4 = 4,
  TRAMP_PRIO_LVL_5 = 5,
  TRAMP_PRIO_LVL_6 = 6,
  TRAMP_PRIO_LVL_7 = 7,
  TRAMP_PRIO_LVL_MAX = 8,
  TRAMP_PRIO_LVL_INVALID = 0xFF
} tramp_prio_lvl_type;

/* The PIC hardware can support the setting of edge or level
 * triggering on some interrupts. 
 */
typedef enum
{
  TRAMP_LEVEL_DETECT = 0,
  TRAMP_EDGE_DETECT
} tramp_detect_type;

/* The PIC hardware can support the setting of polarity
 * triggering on some interrupts.
 */
typedef enum
{
  TRAMP_NEG_POLARITY = 0,
  TRAMP_POS_POLARITY
} tramp_polarity_type;

/* Indicator that the sub interrupt uses enable, clear, and status
 * registers that are either within the PIC HW block (TRAMP_PIC) or 
 * within the MODEM HW block (TRAMP_MODEM).
 */
typedef enum
{
  TRAMP_PIC = 0,
  TRAMP_MODEM
} tramp_reg_usage_type;


/* The following typedef defines the tramp_isr_func_tbl entry structures */
typedef struct
{
  /* ISR settings */
  tramp_invoke_isr_ptr_type  isr_ptr;      /* Dynamically set pointer to ISR */
  tramp_prio_lvl_type        priority;     /* Static priority for interrupt */
  tramp_detect_type          edge_or_level;/* Detect int on EDGE or LEVEL */
  tramp_clr_whn_type         clr_when;     /* Clear int BEFORE or AFTER ISR */
  
  #ifdef FEATURE_2ND_LVL_INTR
  /* Sub interrupt info */
  uint32               sub_tbl_idx;    /* Base index into sub ISR table for 
                                        * consolidated interrupts. 
                                        */
  uint32               sub_cnt;        /* Num of ints consolidated into this
                                        * one
                                        */
  uint32               sub_en_mask;    /* Bit-map of sub-interrupts enabled 
                                        * bits.
                                        */
  uint32               sub_en_idx;     /* Index to the sub-interrupt
                                        * enable table.
                                        */
#endif /* FEATURE_2ND_LVL_INTR */
 
  /* Register addresses */
  volatile uint32     *status_reg;    /* Address of STATUS reg */
  volatile uint32     *enable_reg;    /* Address of ENABLE reg */
  volatile uint32     *clear_reg;     /* Address of CLEAR reg */
  volatile uint32     *detect_reg;    /* Address of the TYPE or DETECT reg */
  volatile uint32     *set_reg;       /* Address of the SET/SOFTINT reg */
  volatile uint32     *select_reg;    /* Address of the SELECT reg */
  
  /* Profiling info */
  uint32              cnt;            /* Total count of #times int has 
                                       * occurred
                                       */

#ifdef TIMETEST
  byte                id;             /* Assumed to be last field in 
                                       * structure
                                       */
#endif /* #ifdef TIMETEST */ 
} tramp_isr_tab_type; 


/* The following typedef defines the structure to be used for the entries
 * in the tramp_sub_isr_func_tbl. 
 *
 * This table contains the information needed to process the interrupts
 * which have been consolidated into one PIC interrupt. 
 *
 *   For example: the tramp_isr_func_tbl will have an entry for the 
 *   MDSP_INT, which is used for the MDSP_INT0, MDSP_INT1 and MDSP_INT2 
 *   interrupts. The tramp_sub_isr_tbl will contain sequential entries 
 *   for the MDSP_INT0, MDSP_INT1 and MDSP_INT2 interrupts.
 */
typedef struct 
{
  /* ISR settings */
  tramp_invoke_isr_ptr_type  isr_ptr;      /* Dynamically set pointer to ISR */
  tramp_clr_whn_type         clr_when;     /* Clear int BEFORE or AFTER ISR  */
  tramp_detect_type          edge_or_level;/* Detect int on EDGE or LEVEL    */
  tramp_polarity_type        polarity;     /* Polarity int, NEG or POS       */
  uint32                     clr_bit;      /* Interrupt clear bit location   */
  uint32                     status_bit;   /* Interrupt status bit location  */
  uint32                     en_bit;       /* Interrupt enable bit location  */
  uint32                     detect_bit;   /* Interrupt detect bit location  */
  uint32                     pol_bit;      /* Int polarity bit location      */

  /* Consolidated int info */
  uint32                     pic_int_num;  /* Interrupt num for consolidated 
                                            * int seen by the PIC HW
                                            */
  
  /* Register addresses of original int */
  volatile uint32            *clear_reg;   /* Address of CLEAR register      */
  volatile uint32            *status_reg;  /* Address of STATUS register     */
  volatile uint32            *detect_reg;  /* Addr of the TYPE or DETECT reg */
  volatile uint32            *pol_reg;     /* Address of the POLARITY reg    */

  /* Index for table access for sub interrupt enabling */
  uint32                     enable_reg;   /* Index for enabling */

  /* Profiling info */
  uint32                     cnt;          /* Total count of #times int has
                                            * occurred
                                            */
  /* Register Type indicator */
  tramp_reg_usage_type       reg_used;     /* Indicator if this interrupt
                                            * uses pic or modem registers
                                            * for enabling, clearing, and
                                            * status.
                                            */

#ifdef TIMETEST
  byte                       id;           /* Assumed to be last field in 
                                            * structure
                                            */
#endif /* #ifdef TIMETEST */ 
} tramp_sub_isr_tab_type; 

/* Tramp interrupt information struct */
typedef struct
{
  tramp_isr_type                int_num;
  tramp_invoke_isr_ptr_type     isr_ptr;
} tramp_int_info_type;

/* Table of pointers to the ISRs. Use index retrieved from PIC to access */
extern tramp_isr_tab_type tramp_isr_tbl[];

/* Table of pointers to the sub ISRs. Use index from associated 
 * tramp_isr_func_tbl entry to access 
 */
extern tramp_sub_isr_tab_type tramp_sub_isr_tbl[];

/* TRAMP_EN access info for sub interrupt enabling */
typedef struct
{
  uint32  out_addr;
  uint32* out_shdw;
  uint32  out_rmsk;
  uint32  shdw_shft;
  uint32* shrd_shdw;
  uint32  shrd_shft;
  uint32* comb_shdw;
} tramp_access_type;

#define TRAMP_EN(io, mask, val) HWIO_OUTMI(TRAMP_EN, io, mask, (val))
#define TRAMP_EN_CHECK(io, mask, result) HWIO_TRAMP_EN_CHECK(io, mask, result)

/* Pick up the chip-specific definitions */
#include "trampi_msm.h"



/*===========================================================================

                     FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/


/*===========================================================================

FUNCTION     TRAMP_INIT_PROC_SPEC

DESCRIPTION
  Processor-specific intialization.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tramp_init_proc_spec( void );



/*===========================================================================

FUNCTION     TRAMP_RESTORE_PROC_SPEC

DESCRIPTION
  This function does the setup so that tramp_handle_sw_workaround_isr
  is called in the right circumstances during power restore.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tramp_restore_proc_spec( void );

#endif  /* TRAMPI_H */

