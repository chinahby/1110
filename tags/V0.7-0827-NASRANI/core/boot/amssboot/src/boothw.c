/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

          B O O T    H A R D W A R E    I N I T I A L I Z A T I O N

GENERAL DESCRIPTION
  This module initializes MSM peripheral control hardware.

EXTERNALIZED FUNCTIONS
  boot_hw_get_bootmode
  boot_hw_calculate_wait_states
  boot_hw_use_calculated_wait_states
  boot_hw_ctrl_init
  boot_hw_configure_wait_states

Copyright (c) 2004-2007 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/boot/boothw.c#7 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/30/07   ps      Adding support for SECBOOT 2.0
04/24/07   amw     Fix compiler warning and some general cleanup.
03/29/07   gmy     Port remaining LCD_CFG1 defs to SC2x
02/02/07    cr     Integrate CLKREGIM_DEVMAN.
12/20/06   gmy     SC2x now drops PS_HOLD enabling power-off to complete.
10/18/05   rp      Modified boot_hw_get_bootmode(0 to add secured boot mode.
04/19/05   tkuo    Rename it to boothw.c from boothw_6275.c
12/28/04   tkuo    remove A23 for Nonsecured boot NOR.
12/08/04   chs     Moved HW init from boot_hw_ctrl_init to oemsbl_hw_init.
10/19/04   tkuo    Ported from MSM6550.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "target.h"
#include "comdef.h"
#include "assert.h"
#include "msm.h"
#include "timetest.h"
#include "boothw.h"
#include "bio.h"
//#include "bsp.h"
#include BOOTHWI_MSM_H
#include "hw.h"
#include "hwi.h"

/* This custoemsbl.h has be the end of "include" to ensure it can re-define
** the Macro(s). So, OEMSBL can share the code in the file.
*/
#ifdef BUILD_BOOT_CHAIN
  #include BOOT_CUSTSBL_H
#endif

#ifndef T_BRINGUP
#define T_BRINGUP
#endif

extern uint32 clk_regime_get_max_hclk_freq_khz( void );
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/* --------------------------------------------------------------------------
   External symbol declarations.
-------------------------------------------------------------------------- */

/* Memory region for boot functions that cannot be executed from external
   memory. */



/* --------------------------------------------------------------------------
   Local variable definitions.
-------------------------------------------------------------------------- */

/* EBI1 memory bus configuration. */

const boothw_ebi1_bus_type  boothw_ebi1 = EBI1_MEMORY_BUS_CFG;

/* EBI2 memory bus configuration. */

const boothw_ebi2_bus_type  boothw_ebi2 = EBI2_MEMORY_BUS_CFG;
/*===========================================================================

  FUNCTION boothw_roundup_memory_cycles()

  DESCRIPTION
    Determine number of cycles required for the specified access time at the
    specified bus frequency, rounded up to the nearest integer.

  PARAMETERS
    Memory access time in nanoseconds.
    Memory bus frequency in kilohertz.

  DEPENDENCIES
    None.

  RETURN VALUE
    Memory access cycles.

  SIDE EFFECTS
    None.

===========================================================================*/
LOCAL uint32 boothw_roundup_memory_cycles
(
  uint8                   time_in_nanoseconds,
  uint32                  frequency_in_khz
)
{
  uint32                  access_cycles = 0;
  uint32                  cyclesx10e6;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Multiply cycle time in nanoseconds by bus frequency in kilohertz to get
     bus cycle count with sufficient precision to determine number of cycles
     required for an access. */

  cyclesx10e6 = time_in_nanoseconds * frequency_in_khz;

  /* Count the number of memory cycles required at the given frequency. */

  while ( cyclesx10e6 >= 1000000 )
  {
    access_cycles++;
    cyclesx10e6 -= 1000000;
  }

  /* Add another memory cycle for the remainder, if required. */

  if ( cyclesx10e6 != 0 )
  {
    access_cycles++;
  }

  return access_cycles;
}

/*===========================================================================

  FUNCTION boothw_calculate_memory_cycles()

  DESCRIPTION
    Determine number of cycles required for the specified access time at the
    specified bus frequency.

  PARAMETERS
    Memory access time in nanoseconds.
    Memory bus frequency in kilohertz.

  DEPENDENCIES
    None.

  RETURN VALUE
    Memory access cycles.

  SIDE EFFECTS
    None.

===========================================================================*/
LOCAL uint32 boothw_calculate_memory_cycles
(
  uint8                   time_in_ns,
  uint32                  freq_in_khz
)
{
  uint32                  access_cycles;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  access_cycles = boothw_roundup_memory_cycles( time_in_ns, freq_in_khz );

  return access_cycles;
}

/*===========================================================================

  FUNCTION boothw_calculate_memory_wait_cycles()

  DESCRIPTION
    Determine number of wait cycles required for the specified access time at
    the specified bus frequency.

  PARAMETERS
    Memory access time in nanoseconds.
    Memory bus frequency in kilohertz.

  DEPENDENCIES
    None.

  RETURN VALUE
    Wait cycles.

  SIDE EFFECTS
    None.

===========================================================================*/
LOCAL uint32 boothw_calculate_memory_wait_cycles
(
  uint8                   time_in_ns,
  uint32                  freq_in_khz
)
{
  uint32                  access_cycles;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Determine the number of access cycles required. */

  access_cycles = boothw_roundup_memory_cycles( time_in_ns, freq_in_khz );

  /* Determine wait states from access cycles. */

  if (access_cycles != 0)
  {
    access_cycles--;
  }

  return access_cycles;
}

#if defined(BOOT_MODE_NOR) && !defined(BUILD_BOOT_CHAIN)
/*===========================================================================

  FUNCTION boothw_calculate_memory_delta_cycles()

  DESCRIPTION
    Determine the delta cycles required for the given memory configuration.
    This parameter is required for burst/page type of memories where the
    access times are different for the first access and subsequent accesses.

  PARAMETERS
    First access time of the memory device
    Subsequent Access time of the memory device
    Bus frequency in kilohertz

  DEPENDENCIES
    None.

  RETURN VALUE
    The difference between the cycles required to access the memory the
    first time and subsequent times.

  SIDE EFFECTS
    None.

===========================================================================*/
LOCAL uint32 boothw_calculate_memory_delta_cycles
(
  uint8                   first_time_in_ns,
  uint8                   second_time_in_ns,
  uint32                  freq_in_khz
)
{
  uint32                  first_access_cycles;
  uint32                  second_access_cycles;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  first_access_cycles =  boothw_roundup_memory_cycles( first_time_in_ns,
                                                       freq_in_khz );

  second_access_cycles = boothw_roundup_memory_cycles( second_time_in_ns,
                                                       freq_in_khz );

  return ( first_access_cycles - second_access_cycles );
}
#endif /* defined(BOOT_MODE_NOR) && !defined(BUILD_BOOT_CHAIN) */
/*===========================================================================

  FUNCTION boothw_calculate_lcd_wait_cycles()

  DESCRIPTION
    Determine the number of cycles required to access the LCD.

  PARAMETERS
    Access time in nanoseconds
    Bus frequency in kilohertz

  DEPENDENCIES
    None.

  RETURN VALUE
    LCD access wait cycles.

  SIDE EFFECTS
    None.

===========================================================================*/
LOCAL uint32 boothw_calculate_lcd_wait_cycles
(
  uint8                   time_in_ns,
  uint32                  freq_in_khz
)
{
  uint32                  access_cycles;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Determine number of access cycles corresponding to input parameters. */

  access_cycles = boothw_roundup_memory_cycles( time_in_ns, freq_in_khz );

  /* Determine number of actual cycles required based on device type. */

  if ( boothw_ebi2.lcd.interface_type == EBI_LCD_INTERFACE_TYPE_INTEL )
  {
    if ( access_cycles >= 2)
    {
      access_cycles -= 1;
    }
    else
    {
      access_cycles = 1;
    }
  }
  else if( boothw_ebi2.lcd.interface_type == EBI_LCD_INTERFACE_TYPE_MOTOROLA )
  {
    if ( access_cycles >= 3)
    {
      access_cycles -= 1;
    }
    else
    {
      access_cycles = 2;
    }
  }
  else
  {
    ERR("LCD interface type not supported!",0,0,0);
  }

  return  access_cycles;
}

/*===========================================================================

  FUNCTION boothw_calculate_lcd_hold_read_cycles()

  DESCRIPTION
    Determine cycles required to access the LCD.

  PARAMETERS
    Access time in nanoseconds
    Bus frequency in kilohertz

  DEPENDENCIES
    None.

  RETURN VALUE
    LCD read hold cycles.

  SIDE EFFECTS
    None.

===========================================================================*/
LOCAL uint32 boothw_calculate_lcd_hold_read_cycles
(
  uint8                   time_in_ns,
  uint32                  freq_in_khz
)
{
  uint32                  access_cycles;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Determine number of access cycles corresponding to input parameters. */

  access_cycles = boothw_roundup_memory_cycles( time_in_ns, freq_in_khz );

  /* Determine number of actual cycles required based on device type. */

  if ( boothw_ebi2.lcd.interface_type == EBI_LCD_INTERFACE_TYPE_INTEL )
  {
    if ( access_cycles >= 1)
    {
      access_cycles -= 1;
    }
  }
  else if( boothw_ebi2.lcd.interface_type == EBI_LCD_INTERFACE_TYPE_MOTOROLA )
  {
      access_cycles = 0;
  }
  else
  {
    ERR("LCD controller not supported!",0,0,0);
  }

  return access_cycles;
}

/*===========================================================================

  FUNCTION boothw_calculate_lcd_hold_write_cycles()

  DESCRIPTION
    Determine cycles required to access the LCD.

  PARAMETERS
    Access time in nanoseconds
    Bus frequency in kilohertz

  DEPENDENCIES
    None.

  RETURN VALUE
    LCD access cycles.

  SIDE EFFECTS
    None.

===========================================================================*/
LOCAL uint32 boothw_calculate_lcd_hold_write_cycles
(
  uint8                   time_in_ns,
  uint32                  freq_in_khz
)
{
  uint32                  access_cycles;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Determine number of access cycles corresponding to input parameters. */

  access_cycles = boothw_roundup_memory_cycles( time_in_ns, freq_in_khz );

  /* Determine number of actual cycles required based on device type. */

  if ( boothw_ebi2.lcd.interface_type == EBI_LCD_INTERFACE_TYPE_INTEL )
  {
    if ( access_cycles >= 2)
    {
       access_cycles -= 1;
    }
    else
    {
      access_cycles = 1;
    }
  }
  else if( boothw_ebi2.lcd.interface_type == EBI_LCD_INTERFACE_TYPE_MOTOROLA )
  {
    access_cycles = 0;
  }
  else
  {
    ERR("LCD controller not supported!",0,0,0);
  }

  return access_cycles;
}

/*===========================================================================

  FUNCTION boothw_calculate_lcd_setup_cycles()

  DESCRIPTION
    Determine cycles required to access the LCD.

  PARAMETERS
    Access time in nanoseconds
    Bus frequency in kilohertz

  DEPENDENCIES
    None.

  RETURN VALUE
    LCD access cycles.

  SIDE EFFECTS
    None.

===========================================================================*/
LOCAL uint32 boothw_calculate_lcd_setup_cycles
(
   uint8                   time_in_ns,
   uint32                  freq_in_khz
)
{
   uint32                  access_cycles;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

   /* Determine number of access cycles corresponding to input parameters. */

   access_cycles = boothw_roundup_memory_cycles( time_in_ns, freq_in_khz );

   /* Determine number of actual cycles required based on device type. */

   if ( boothw_ebi2.lcd.interface_type == EBI_LCD_INTERFACE_TYPE_INTEL )
   {
     access_cycles = 0;
   }
   else if( boothw_ebi2.lcd.interface_type == EBI_LCD_INTERFACE_TYPE_MOTOROLA )
   {
     if ( access_cycles >= 2)
     {
        access_cycles -= 1;
     }
     else
     {
       access_cycles = 1;
     }
   }
   else
   {
     ERR("LCD controller not supported!",0,0,0);
   }

   return  access_cycles;
}

#if 0
/*===========================================================================

  FUNCTION boothw_compute_ebi1_cs_cfg0_val()

  DESCRIPTION
    Determine the value for the EBI1_CSn_CFG0 register.

  PARAMETERS
    Pointer to chip select configuration structure.
    Memory bus frequency in kilohertz.

  DEPENDENCIES
    None.

  RETURN VALUE
    Value for the EBI1_CSn_CFG0 register.

  SIDE EFFECTS
    None.

===========================================================================*/
LOCAL uint32 boothw_compute_ebi1_cs_cfg0_val
(
  const boothw_ebi1_memory_type*  cs_cfg,
  uint32                          clock_frequency
)
{
  uint32                          reg_val = 0;
  uint32                          fld_val = 0;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Memory component page size. */
  reg_val |= cs_cfg->page_size
             << HWIO_SHFT(EBI1_CSn_CFG0,PAGE_SIZE);

  /* Consecutive access PRECHARGE cycles. */
  reg_val |= boothw_calculate_memory_cycles( cs_cfg->cs_precharge_ns,
                                             clock_frequency )
             << HWIO_SHFT(EBI1_CSn_CFG0,PRECHARGE_CYC);

  /* Chip select de-assert RECOVERY cycles. */
  reg_val |= boothw_calculate_memory_cycles( cs_cfg->cs_recovery_ns,
                                             clock_frequency )
             << HWIO_SHFT(EBI1_CSn_CFG0,RECOVERY);

  /* Write access HOLD cycles. Must have a minimum of 1 cycle. */
  fld_val = boothw_calculate_memory_cycles( cs_cfg->write_hold_ns,
                                            clock_frequency );
  if ( fld_val == 0 ) fld_val++;

  reg_val |= fld_val << HWIO_SHFT(EBI1_CSn_CFG0,HOLD_WAIT);

  /* Write access cycles. Burst write and page write accesses are not
     supported, so program the delta field to the number of accesses required
     minus one since one cycle will be added automatically, and program the
     WAIT field to zero.  Write access DELTA cycles must at least 1. */

  fld_val = boothw_calculate_memory_cycles( cs_cfg->first_wr_ns,
                                            clock_frequency );
  if ( fld_val == 0 ) fld_val++;
  if ( fld_val >  1 ) fld_val--;

  if ( cs_cfg->access_type == EBI_ACCESS_TYPE_BURST )
  {
    /* DELTA cycles. */

    fld_val = EBI1_BURST_DELTA_CYCLES_VAL;
  }

  reg_val |= fld_val << HWIO_SHFT(EBI1_CSn_CFG0,DELTA_WR);

  /* Read access cycles. Read access WAIT cycles must be 0 for burst mode
     and read access DELTA cycles must be 3 for burst mode. Read access
     DELTA cycles must be at least 1. */

  if ( cs_cfg->access_type == EBI_ACCESS_TYPE_PAGE )
  {
    /* DELTA cycles. */

    fld_val = boothw_calculate_memory_delta_cycles( cs_cfg->first_rd_ns,
                                                    cs_cfg->second_rd_ns,
                                                    clock_frequency );
    if ( fld_val == 0 ) fld_val++;

    reg_val |= fld_val << HWIO_SHFT(EBI1_CSn_CFG0,DELTA_RD);

    /* WAIT cycles. */

    fld_val = boothw_calculate_memory_wait_cycles( cs_cfg->second_rd_ns,
                                                   clock_frequency );

    reg_val |= fld_val << HWIO_SHFT(EBI1_CSn_CFG0,WAIT_RD);
  }

  else if ( cs_cfg->access_type == EBI_ACCESS_TYPE_BURST )
  {
    /* DELTA cycles. */

    fld_val = EBI1_BURST_DELTA_CYCLES_VAL;

    reg_val |= fld_val << HWIO_SHFT(EBI1_CSn_CFG0,DELTA_RD);
  }

  else /* EBI_ACCESS_TYPE_ASYNC */
  {
    /* DELTA cycles. */

    fld_val = boothw_calculate_memory_cycles( cs_cfg->first_rd_ns,
                                              clock_frequency );
    if ( fld_val == 0 ) fld_val++;
    if ( fld_val >  1 ) fld_val--;

    reg_val |= fld_val  << HWIO_SHFT(EBI1_CSn_CFG0,DELTA_RD);
  }

  return reg_val;
}

/*===========================================================================

  FUNCTION boothw_compute_ebi1_cs_cfg1_val()

  DESCRIPTION
    Determine the value for the EBI1_CSn_CFG1 register.

  PARAMETERS
    Pointer to chip select configuration structure.
    Memory bus frequency in kilohertz.

  DEPENDENCIES
    None.

  RETURN VALUE
    Value for the EBI1_CSn_CFG1 register.

  SIDE EFFECTS
    None.

===========================================================================*/
LOCAL uint32 boothw_compute_ebi1_cs_cfg1_val
(
  const boothw_ebi1_memory_type*  cs_cfg,
  uint32                          clock_frequency
)
{
  uint32                          reg_val = 0;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Synchronous AVD_N signal RECOVERY cycles. */
  reg_val |= boothw_calculate_memory_cycles( cs_cfg->avd_recovery_ns,
                                             clock_frequency )
             << HWIO_SHFT(EBI1_CSn_CFG1,AVD_RECOVERY);

  /* Memory write protection. */
  reg_val |= cs_cfg->write_prot_ena
             << HWIO_SHFT(EBI1_CSn_CFG1,WRITE_PROTECT);

  /* Memory wrap mode. */
  reg_val |= cs_cfg->wrap8_mode_ena
             << HWIO_SHFT(EBI1_CSn_CFG1,WRAP8_MODE);

  /* Synchronous AVD_N signal state during access. */
  reg_val |= cs_cfg->avd_state
             << HWIO_SHFT(EBI1_CSn_CFG1,AVD_STATE);

  /* Burst read enable. */
  if ( cs_cfg->access_type == EBI_ACCESS_TYPE_BURST )
  {
    reg_val |= EBI_BURST_READ_ENABLED
             << HWIO_SHFT(EBI1_CSn_CFG1,BURST_RD_ENA);
  }

  /* Burst write enable. */
  if ( cs_cfg->access_type == EBI_ACCESS_TYPE_BURST &&
       cs_cfg->device_type == EBI_MEM_TYPE_PSRAM)
  {
    reg_val |= EBI_BURST_WRITE_ENABLED
             << HWIO_SHFT(EBI1_CSn_CFG1,BURST_WR_ENA);
  }

  /* Enable x32 Interface and Addr Hold Enable (Valid for 32 only). */
  if (cs_cfg->data_width == EBI_DATA_WIDTH_32_BITS)
  {
    reg_val |= EBI_ENA_X32_INTERFACE_ENABLE
             << HWIO_SHFT(EBI1_CSn_CFG1,ENA_X32_INTERFACE);

    reg_val |= EBI_ADDR_HOLD_ENA_ENABLE
             << HWIO_SHFT(EBI1_CSn_CFG1,ADDR_HOLD_ENA);

    reg_val |= EBI_AVD_OE_RECOVERY_VAL
             << HWIO_SHFT(EBI1_CSn_CFG1,AVD_OE_RECOVERY);

    reg_val |= EBI_AVD_PULSE_ENA_ENABLE
             << HWIO_SHFT(EBI1_CSn_CFG1,AVD_PULSE_ENA);
  }

  /* Page read enable. */
  if ( cs_cfg->access_type == EBI_ACCESS_TYPE_PAGE )
  {
    reg_val |= EBI_PAGE_READ_ENABLED
             << HWIO_SHFT(EBI1_CSn_CFG1,PAGE_RD_ENA);
  }

  return reg_val;
}

#endif /*BOOT_MODE_NOR */
/*===========================================================================

  FUNCTION boothw_compute_ebi2_cs_cfg0_val()

  DESCRIPTION
    Determine the value for the EBI2_CSn_CFG0 register.

  PARAMETERS
    Pointer to chip select configuration structure.
    Memory bus frequency in kilohertz.

  DEPENDENCIES
    None.

  RETURN VALUE
    Value for the EBI2_CSn_CFG0 register.

  SIDE EFFECTS
    None.

===========================================================================*/
LOCAL uint32 boothw_compute_ebi2_cs_cfg0_val
(
  const boothw_ebi2_memory_type*  cs_cfg,
  uint32                          clock_frequency
)
{
  uint32                         reg_val = 0;
  uint32                         fld_val = 0;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Chip select SETUP cycles. */
  reg_val |= boothw_calculate_memory_cycles( cs_cfg->cs_setup_ns,
                                             clock_frequency )
             << HWIO_SHFT(GPn_CFG0,CS_SETUP);

  /* Chip select RECOVERY cycles. */
  reg_val |= boothw_calculate_memory_cycles( cs_cfg->cs_recovery_ns,
                                             clock_frequency )
             << HWIO_SHFT(GPn_CFG0,RECOVERY);

  /* Write access HOLD cycles. Minimum value of 1. */
  fld_val = boothw_calculate_memory_cycles( cs_cfg->write_hold_ns,
                                            clock_frequency );
  if ( fld_val == 0 ) fld_val++;

  reg_val |= fld_val <<  HWIO_SHFT(GPn_CFG0,HOLD_WR);

  /* Read access HOLD cycles. */
  reg_val |= boothw_calculate_memory_cycles( cs_cfg->read_hold_ns,
                                             clock_frequency )
             << HWIO_SHFT(GPn_CFG0,HOLD_RD);

#if 0 /* TBD */
  /* Write access DELTA cycles. */
  reg_val |= boothw_calculate_memory_delta_cycles( cs_cfg->first_wr_ns,
                                                   cs_cfg->second_wr_ns,
                                                   clock_frequency )
             << HWIO_SHFT(GPn_CFG0,DELTA_WR);

  /* Read access DELTA cycles. */
  reg_val |= boothw_calculate_memory_delta_cycles( cs_cfg->first_rd_ns,
                                                   cs_cfg->second_rd_ns,
                                                   clock_frequency )
             << HWIO_SHFT(GPn_CFG0,DELTA_RD);
#endif

  /* Write access WAIT cycles. Minimum value of 1. */
  fld_val = boothw_calculate_memory_wait_cycles( cs_cfg->second_wr_ns,
                                                 clock_frequency );
  if ( fld_val == 0 ) fld_val++;

  reg_val |= fld_val << HWIO_SHFT(GPn_CFG0,WAIT_WR);

  /* Read access WAIT cycles. Minimum value of 1. */
  fld_val = boothw_calculate_memory_wait_cycles( cs_cfg->second_rd_ns,
                                                 clock_frequency );
  if ( fld_val == 0 ) fld_val++;

  reg_val |= fld_val << HWIO_SHFT(GPn_CFG0,WAIT_RD);

  return reg_val;
}
/*===========================================================================

  FUNCTION boothw_compute_lcd_cfg0_val()

  DESCRIPTION
    Determine the value for the LCD_CFG0 register.

  PARAMETERS
    Pointer to chip select configuration structure.
    Memory bus frequency in kilohertz.

  DEPENDENCIES
    None.

  RETURN VALUE
    Value for the LCD_CFG0 register.

  SIDE EFFECTS
    None.

===========================================================================*/
LOCAL uint32 boothw_compute_lcd_cfg0_val
(
  const boothw_ebi2_lcd_type*  cs_cfg,
  uint32                       clock_frequency
)
{
  uint32                       reg_val = 0;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Chip select SETUP cycles. */
  reg_val |= boothw_calculate_memory_cycles( cs_cfg->cs_setup_ns,
                                             clock_frequency )
            << HWIO_SHFT(LCD_CFG0,LCD_CS_SETUP);

  /* Chip select RECOVERY cycles. */
  reg_val |= boothw_calculate_memory_cycles( cs_cfg->cs_recovery_ns,
                                             clock_frequency )
             << HWIO_SHFT(LCD_CFG0,LCD_RECOVERY);

  /* Write access HOLD cycles. */
  reg_val |= boothw_calculate_lcd_hold_write_cycles( cs_cfg->write_hold_ns,
                                                     clock_frequency )
             << HWIO_SHFT(LCD_CFG0,LCD_HOLD_WR);

  /* Read access HOLD cycles. */
  reg_val |= boothw_calculate_lcd_hold_read_cycles( cs_cfg->read_hold_ns,
                                                    clock_frequency )
             << HWIO_SHFT(LCD_CFG0,LCD_HOLD_RD);

  /* Write access WAIT cycles. */
  reg_val |= boothw_calculate_lcd_wait_cycles( cs_cfg->first_wr_ns,
                                               clock_frequency )
             << HWIO_SHFT(LCD_CFG0,LCD_WAIT_WR);

  /* Read access WAIT cycles. */
  reg_val |= boothw_calculate_lcd_wait_cycles( cs_cfg->first_rd_ns,
                                               clock_frequency )
             << HWIO_SHFT(LCD_CFG0,LCD_WAIT_RD);

  return reg_val;
}

/*===========================================================================

  FUNCTION boothw_compute_lcd_cfg1_val()

  DESCRIPTION
    Determine the value for the LCD_CFG1 register.

  PARAMETERS
    Pointer to chip select configuration structure.
    Memory bus frequency in kilohertz.

  DEPENDENCIES
    None.

  RETURN VALUE
    Value for the LCD_CFG1 register.

  SIDE EFFECTS
    None.

===========================================================================*/
LOCAL uint32 boothw_compute_lcd_cfg1_val
(
  const boothw_ebi2_lcd_type*  cs_cfg,
  uint32                       clock_frequency
)
{
  uint32                       reg_val = 0;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#ifdef T_QSC1100
  /* LCD mode = LCD. */
  reg_val |= 1 << HWIO_SHFT(LCD_CFG1,LCD_MODE);
  /* LCD Muxes address on data lines */
  reg_val |= HWIO_LCD_CFG1_LCD_MUX_ADDR_DATA_ENA_BMSK;
#else
  /* LCD data width type. */
  reg_val |= (uint32)cs_cfg->data_width
             << HWIO_SHFT(LCD_CFG1,LCD_18_OR_24_BIT_ENA);
#endif

#ifdef T_QSC60X5

#ifndef T_QSC1100
  /* SC2x augments the 16-bit EBI2 bus with two muxed
   * address lines to support an 18-bit parallel LCD interface */
  reg_val |= (uint32)cs_cfg->data_width
             << HWIO_SHFT(LCD_CFG1,LCD_D_ON_UB_LB);
#endif

  /* SC2x uses address line 20 (A20) to drive the LCD's "register select"
   * pin. When this pin is asserted (high), the LCD interprets data on
   * the parallel bus as pixel data or a command parameter. When this
   * pin is deasserted (low), data on the bus is interpreted as a command. */
  reg_val |= (1 << HWIO_LCD_CFG1_LCD_RS_ENA_SHFT) |
     (HWIO_LCD_CFG1_LCD_RS_MODE_BIT_20_OF_AHB_ADDRESS_DURING_THE_ACCESS_FVAL <<
      HWIO_LCD_CFG1_LCD_RS_MODE_SHFT);
#endif

  /* LCD interface type. */
  reg_val |= cs_cfg->interface_type
             << HWIO_SHFT(LCD_CFG1,LCD_INTERFACE_TYPE);

#ifndef T_QSC60X5
#error code not present
#endif

  /* Byte mode enable. */
  reg_val |= cs_cfg->byte_mode_ena
             << HWIO_SHFT(LCD_CFG1,LCD_BYTE_DEVICE_ENA);

  /* LCD_E write access SETUP cycles. */
  reg_val |= boothw_calculate_lcd_setup_cycles( cs_cfg->lcde_wr_setup_ns,
                                                clock_frequency )
             << HWIO_SHFT(LCD_CFG1,LCD_E_SETUP_WRITE);

  /* LCD_E read access SETUP cycles. */
  reg_val |= boothw_calculate_lcd_setup_cycles( cs_cfg->lcde_rd_setup_ns,
                                                clock_frequency )
             << HWIO_SHFT(LCD_CFG1,LCD_E_SETUP_READ);

  /* LCD_E write access HIGH cycles. */
  reg_val |= boothw_calculate_lcd_setup_cycles( cs_cfg->lcde_wr_high_ns,
                                                clock_frequency )
             << HWIO_SHFT(LCD_CFG1,LCD_E_HIGH_WRITE);

  /* LCD_E read access HIGH cycles. */
  reg_val |= boothw_calculate_lcd_setup_cycles( cs_cfg->lcde_rd_high_ns,
                                                clock_frequency )
             << HWIO_SHFT(LCD_CFG1,LCD_E_HIGH_READ);

  return reg_val;
}


/*===========================================================================

  FUNCTION BOOT_HW_COMPUTE_EBI_REGS

  DESCRIPTION
    Compute the values of the EBI registers that include fields which are
    dependent on the memory bus clock frequency.

  PARAMETERS
    Pointer to EBI register structure.
    Memory bus frequency in kilohertz.

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    Caller EBI register value structure is updated.

===========================================================================*/
void boot_hw_compute_ebi_regs
(
  boothw_ebi_regs_type*  regs,
  uint32                 bus_freq_in_khz
)
{
  /* ------------------------------------------------------------------------
     EBI1 chip select configuration registers.
  ------------------------------------------------------------------------ */

#if 0

  regs->ebi1_cs0_cfg0 = boothw_compute_ebi1_cs_cfg0_val( &(boothw_ebi1.cs0),
                                                         bus_freq_in_khz );

  regs->ebi1_cs0_cfg1 = boothw_compute_ebi1_cs_cfg1_val( &(boothw_ebi1.cs0),
                                                         bus_freq_in_khz );

  regs->ebi1_cs1_cfg0 = boothw_compute_ebi1_cs_cfg0_val( &(boothw_ebi1.cs1),
                                                         bus_freq_in_khz );

  regs->ebi1_cs1_cfg1 = boothw_compute_ebi1_cs_cfg1_val( &(boothw_ebi1.cs1),
                                                         bus_freq_in_khz );

  regs->ebi1_cs2_cfg0 = boothw_compute_ebi1_cs_cfg0_val( &(boothw_ebi1.cs2),
                                                         bus_freq_in_khz );

  regs->ebi1_cs2_cfg1 = boothw_compute_ebi1_cs_cfg1_val( &(boothw_ebi1.cs2),
                                                         bus_freq_in_khz );

  regs->ebi1_cs3_cfg0 = boothw_compute_ebi1_cs_cfg0_val( &(boothw_ebi1.cs3),
                                                         bus_freq_in_khz );

  regs->ebi1_cs3_cfg1 = boothw_compute_ebi1_cs_cfg1_val( &(boothw_ebi1.cs3),
                                                         bus_freq_in_khz );

#endif /* BOOT_MODE_NOR */

  /* ------------------------------------------------------------------------
     EBI2 chip select configuration registers.
  ------------------------------------------------------------------------ */

  regs->ebi2_cs0_cfg0 = boothw_compute_ebi2_cs_cfg0_val( &(boothw_ebi2.cs0),
                                                         bus_freq_in_khz );

  regs->ebi2_cs1_cfg0 = boothw_compute_ebi2_cs_cfg0_val( &(boothw_ebi2.cs1),
                                                         bus_freq_in_khz );

  regs->ebi2_cs2_cfg0 = boothw_compute_ebi2_cs_cfg0_val( &(boothw_ebi2.cs2),
                                                         bus_freq_in_khz );

  regs->ebi2_cs3_cfg0 = boothw_compute_ebi2_cs_cfg0_val( &(boothw_ebi2.cs3),
                                                         bus_freq_in_khz );

  regs->ebi2_lcd_cfg0 = boothw_compute_lcd_cfg0_val( &(boothw_ebi2.lcd),
                                                     bus_freq_in_khz );

  regs->ebi2_lcd_cfg1 = boothw_compute_lcd_cfg1_val( &(boothw_ebi2.lcd),
                                                     bus_freq_in_khz );
}

/*===========================================================================

  FUNCTION BOOT_HW_PROGRAM_EBI_REGS

  DESCRIPTION
    Program EBI registers with values provided by caller.

  PARAMETERS
    Pointer to structure containing EBI register values.

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    EBI register programming is updated.

===========================================================================*/
void boot_hw_program_ebi_regs
(
  const boothw_ebi_regs_type*  regs
)
{
  /* ------------------------------------------------------------------------
     EBI1 chip select configuration registers.
  ------------------------------------------------------------------------ */

#if 0

  HWIO_OUTI( EBI1_CSn_CFG0, 0, regs->ebi1_cs0_cfg0 );
  HWIO_OUTI( EBI1_CSn_CFG1, 0, regs->ebi1_cs0_cfg1 );
  HWIO_OUTI( EBI1_CSn_CFG0, 1, regs->ebi1_cs1_cfg0 );
  HWIO_OUTI( EBI1_CSn_CFG1, 1, regs->ebi1_cs1_cfg1 );
  HWIO_OUTI( EBI1_CSn_CFG0, 2, regs->ebi1_cs2_cfg0 );
  HWIO_OUTI( EBI1_CSn_CFG1, 2, regs->ebi1_cs2_cfg1 );
  HWIO_OUTI( EBI1_CSn_CFG0, 3, regs->ebi1_cs3_cfg0 );
  HWIO_OUTI( EBI1_CSn_CFG1, 3, regs->ebi1_cs3_cfg1 );

#endif /* BOOT_MODE_NOR */

  /* ------------------------------------------------------------------------
     EBI2 chip select configuration registers.
  ------------------------------------------------------------------------ */
#ifndef T_BRINGUP
  /* Commented out because these settings conflict with psram when set in
     burst mode.

     Probable reasons
     - (1) cmm script configures burst mode for psram and this code tries to
           set it in async mode
     - (2) The delay cycles set here do not meet the requirements

     TODO - check if we get the correct settings populated in
            regs->ebi2_csn_cfg0 and then reenable this code
  */
  HWIO_OUTI( GPn_CFG0, 0, regs->ebi2_cs0_cfg0 );
  HWIO_OUTI( GPn_CFG0, 1, regs->ebi2_cs1_cfg0 );
  HWIO_OUTI( GPn_CFG0, 2, regs->ebi2_cs2_cfg0 );
  HWIO_OUTI( GPn_CFG0, 3, regs->ebi2_cs3_cfg0 );
#endif

  HWIO_OUT( LCD_CFG0, regs->ebi2_lcd_cfg0 );
  HWIO_OUT( LCD_CFG1, regs->ebi2_lcd_cfg1 );
}

/*===========================================================================

  FUNCTION boot_hw_powerdown()

  DESCRIPTION
    This function disables the PS_HOLD line to the PMIC allowing the
    phone to power off.

  PARAMETERS
    None.

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.

===========================================================================*/
void boot_hw_powerdown()
{
   /* Turn off PS_HOLD, this line feeds into the voltage regulator.
   ** The power will turn off in ~50ms when the capacitor drains.
   */
   hw_set_ps_hold(HW_PS_HOLD_DISABLE);

  for (;;)
  {
    /* ----------------------------------------------------------------------
       Do nothing until the phone powers off. Make certain the watchdog
       does not expire and reset us before the power off.
    ---------------------------------------------------------------------- */
    BOOTHW_KICK_WATCHDOG();
  }
}


/*===========================================================================

  FUNCTION boot_hw_get_bootmode()

  DESCRIPTION
   Returns the bootmode
  PARAMETERS
    None.

  DEPENDENCIES
    None.

  RETURN VALUE
    Bootmode.

  SIDE EFFECTS

===========================================================================*/
boothw_boot_mode_enum_type boot_hw_get_bootmode(void )
{

#ifdef BOOT_MODE_NOR
 return BOOTMODE_NOR_TRUSTED;
#else
 return BOOTMODE_NAND_TRUSTED;
#endif

 } /* boot_hw_get_bootmode() */

#ifndef BUILD_BOOT_CHAIN
/*===========================================================================

  FUNCTION BOOT_HW_GET_MAX_RAM_BANDWIDTH()

  DESCRIPTION
   This function calculates the bandwidth in bytes per second for the
   read write memory.Note that the bandwidth calculated is the maximum
   bandwidth calculated based on the time taken for a cache line fill and a
   100% bus utilization.
   The real bandwidth will be much lower than the value calculated here and
   will depend on many hardware conditions other than just cache fill time.

  PARAMETERS
    None.

  DEPENDENCIES
    None.

  RETURN VALUE
    The bandwidth in bytes per second for the read write memory.

  SIDE EFFECTS

===========================================================================*/
uint32 boot_hw_get_max_ram_bandwidth(void )
{
  #define CACHE_LINE_SIZE                32

  uint32 cache_line_fill_cycles;
  uint32 bw;                            /* bandwidth in bytes per second   */
  uint32 max_hclk_khz;                  /* max hclk in khz                 */

  max_hclk_khz = clk_regime_get_max_hclk_freq_khz();

  /* calculate the number of cycles required for a cache line fill. Each
  ** cache line fill is 32 bytes.
  */

  #ifdef T_SDRAM_IS_16_BIT
    /* For 16 bit SDRAM, the average cache line fill cycles is 26
       based on a controller delay of 10 cycles and 16 data cycles */
    cache_line_fill_cycles = 26 ;

  #elif  defined(T_SDRAM_IS_32_BIT)
    /* For 32 bit SDRAM, the average cache line fill cycles is 18
       based on a controller delay of 10 cycles and 8 data cycles */
    cache_line_fill_cycles = 18 ;

  #elif  defined(T_PSRAM_IS_BURST_TYPE) && !defined(T_PSRAM_32)
    /* For 16 bit Burst PSRAM, the average cache line fill cycles is 26
       based on a controller delay of 10 cycles and 16 data cycles */
    cache_line_fill_cycles = 26;

  #elif  defined(T_PSRAM_IS_BURST_TYPE) && defined(T_PSRAM_32)
    /* For 32 bit Burst PSRAM, the average cache line fill cycles is 18
       based on a controller delay of 10 cycles and 8 data cycles */
    cache_line_fill_cycles = 18;

  #else

  #endif /* T_SDRAM */

  /* Calculate the bandwidth based on the cache_line_fill_cycles and HCLK
   *
   *      WHAT WE'RE DOING HERE:
   *      ======================
   *      Define clfc (cache line fill cycles) as the number of
   *      HCLK cycles needed to fill the 32 byte Cache line.
   *      Then RAM performance is 32 bytes per clfc hclk_cycles, or:
   *
   *      32      bytes
   *      --------------------
   *      clfc    hclk_cycles
   *
   *      There are "hclk_freq_in_hz" hclk_cycles per second, so,
   *      then bandwidth is:
   *
   *      32      bytes          hclk_freq_in_hz hclk_cycles
   *      -------------------- * -----------------------------
   *      clfc    hclk_cycles    1 second
   *
   *      hclk_cycles "units" cancel out and we have:
   *
   *      BW = 32 * hclk_freq_in_hz       bytes
   *           ---------------------------------
   *           clfc                       seconds
   *
   *      or:
   *
   *        bw =  CACHE_LINE_SIZE * hclk_khz * 1000 / clfc (bytes per sec)
   *
   *      To avoid future risks of overflow on intermediate results, we
   *      re-write it as:
   *
   *        bw = ( CACHE_LINE_SIZE * hclk_khz / clfc ) * 1000;
   */

  bw = ( CACHE_LINE_SIZE * max_hclk_khz / cache_line_fill_cycles ) * 1000;

  return bw;
}
#endif
