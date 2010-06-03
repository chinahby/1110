/*=============================================================================

                      D e v i c e  B o o t  L o a d e r

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/romboot/sbl/dbl/config/clk/dbl_clk_settings.c#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
02/05/08   ps     Initial revision.
=============================================================================*/


/*=============================================================================
                            INCLUDE FILES FOR MODULE
=============================================================================*/
#include "boot_comdef.h"
#include "dbl_clki.h"
#include "dbl_error_handler.h"
#include "boot_msm.h"
#include "dbl_parser.h"
#include "customer.h"

/*=============================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.
=============================================================================*/

/*-----------------------------------------------------------------------
  ENUM determining which settings low and high speed combination is to 
  be picked from the constant structure below.
-----------------------------------------------------------------------*/
typedef enum
{
  DBL_CLK_PLL_384_ARM_48_EBI1_48_EBI2_24_PBL,
  DBL_CLK_TCXO_ARM_20_EBI1_20_EBI2_20_PBL,
  DBL_CLK_PLL_384_ARM_128_EBI1_64_EBI2_32,  /* PLL = 384Mhz and 1x mode */
  DBL_CLK_PLL_192_ARM_96_EBI1_48_EBI2_24,   /* PLL = 192Mhz and 1x mode */
  DBL_CLK_PLL_192_ARM_96_EBI1_96_EBI2_24,   /* PLL = 384Mhz and 2x mode */
  DBL_PLL_LOW_HIGH_COMBO_ENUM_SIZE = 0x7FFFFFFF
} dbl_clk_speed_enum_type;


/*-----------------------------------------------------------------------
             Structures defining clock configurations.
-----------------------------------------------------------------------*/
const boot_clk_speed_type dbl_clk_pll_384_arm_48_ebi1_48_ebi2_24_pbl =
{ BOOT_CLK_SOURCE_PLL_384 , 48 /* ARM9    */, 48 /* AHB  */,
   48 /* EBI1    */, 24 /* EBI2 */  };

const boot_clk_speed_type dbl_clk_tcxo_arm_20_ebi1_20_ebi2_20_pbl =
{ BOOT_CLK_SOURCE_TCXO , 20 /* ARM9    */, 20 /* AHB  */,
   20 /* EBI1    */, 20 /* EBI2 */  };

const boot_clk_speed_type dbl_clk_pll_384_arm_128_ebi1_64_ebi2_32 =
{ BOOT_CLK_SOURCE_PLL_384 , 128 /* ARM9    */, 64 /* AHB  */,
   64 /* EBI1    */, 32 /* EBI2 */  };

const boot_clk_speed_type dbl_clk_pll_192_arm_96_ebi1_48_ebi2_24 =
{ BOOT_CLK_SOURCE_PLL_192 , 96 /* ARM9    */, 48 /* AHB  */,
   48 /* EBI1    */, 24 /* EBI2 */  };

const boot_clk_speed_type dbl_clk_pll_192_arm_96_ebi1_96_ebi2_24 =
{ BOOT_CLK_SOURCE_PLL_192 , 96 /* ARM9    */, 48 /* AHB  */,
   96 /* EBI1    */, 24 /* EBI2 */  };

#if defined (FEATURE_PLL_384) && defined (FEATURE_CLKREGIM_1X_MODE)
/* 1xmm Mode TCXO => PLL @ 384Mhz */
const static dbl_parser_cfg_data_item_type dbl_clk_tcxo_arm_20_ebi1_20_ebi2_20_pbl_TO_dbl_clk_pll_384_arm_128_ebi1_64_ebi2_32[] =
{
  /* Reset the PLL, put it in Bypass, and disable the Output */
  {HWIO_OPERATION,   HWIO_ADDR(PLL_MODE_REG), ( 0 << HWIO_SHFT( PLL_MODE_REG, RESET_N  ) |
                                                0 << HWIO_SHFT( PLL_MODE_REG, BYPASSNL ) |
                                                0 << HWIO_SHFT( PLL_MODE_REG, OUTCTRL  ) ) },
  /* Setup PLL to 384MHz. */
  {HWIO_OPERATION,   HWIO_ADDR(PLL_L_VAL_REG),     0x14        }, 
  {HWIO_OPERATION,   HWIO_ADDR(PLL_M_VAL_REG),     0x0         }, 
  {HWIO_OPERATION,   HWIO_ADDR(PLL_N_VAL_REG),     0x1         }, 
  {HWIO_OPERATION,   HWIO_ADDR(PLL_TEST_CTL_REG),  0x00000000  },
  {HWIO_OPERATION,   HWIO_ADDR(PLL_CONFIG_REG),    0x00201182  }, 

  /* Bring the PLL out of Bypass */
  {HWIO_OPERATION,   HWIO_ADDR(PLL_MODE_REG),  HWIO_FMSK( PLL_MODE_REG, BYPASSNL ) }, 

  /* Wait >= 5us */
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  20 * 5  },

  /* Bring the PLL out of Reset */
  {HWIO_OPERATION,   HWIO_ADDR(PLL_MODE_REG),  ( HWIO_FMSK( PLL_MODE_REG, RESET_N  ) |
                                                 HWIO_FMSK( PLL_MODE_REG, BYPASSNL ) ) },

  /* Wait >= 50us for PLLs to lock. */
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  20 * 50  }, 

  /* Enable PLL Output */
  {HWIO_OPERATION,   HWIO_ADDR(PLL_MODE_REG), ( HWIO_FMSK( PLL_MODE_REG, RESET_N  ) |
                                                HWIO_FMSK( PLL_MODE_REG, BYPASSNL ) |
                                                HWIO_FMSK( PLL_MODE_REG, OUTCTRL  ) ) }, 

  /* Configure the ARM_DIV_MOD1 divider to div_8 = 48Mhz */
  {HWIO_OPERATION,   HWIO_ADDR(ARM_MOD_REG), ( HWIO_ARM_MOD_REG_ARM_DIV_MOD1_DIV_8_FVAL <<  HWIO_SHFT( ARM_MOD_REG, ARM_DIV_MOD1 ) ) }, 

  /* Configure SRC1 to select the PLL 
   * Switch to ARM_SRC1
   * ARM_RATE_SEL << div_1
   * AHB_RATE_SEL << div_2
   * EBI1_RATE_SEL << div_2
   * EBI2_RATE_SEL << div_4
   */
  {HWIO_OPERATION,   HWIO_ADDR(ARM_CFG_REG),  ( HWIO_ARM_CFG_REG_SRC_SEL1_PLL_SRC_FVAL     << HWIO_SHFT( ARM_CFG_REG, SRC_SEL1        )  |
                                                HWIO_ARM_CFG_REG_ARM_SRC_SEL_ARM_SRC1_FVAL << HWIO_SHFT( ARM_CFG_REG, ARM_SRC_SEL     )  |
                                                HWIO_ARM_CFG_REG_ARM_RATE_SEL_DIV_1_FVAL   <<  HWIO_SHFT( ARM_CFG_REG, ARM_RATE_SEL   )  |      
                                                HWIO_ARM_CFG_REG_AHB_RATE_SEL_DIV_2_FVAL   <<  HWIO_SHFT( ARM_CFG_REG, AHB_RATE_SEL   )  |
                                                HWIO_ARM_CFG_REG_EBI1_RATE_SEL_DIV_2_FVAL  <<   HWIO_SHFT( ARM_CFG_REG, EBI1_RATE_SEL )  |
                                                HWIO_ARM_CFG_REG_EBI2_RATE_SEL_DIV_4_FVAL  <<   HWIO_SHFT( ARM_CFG_REG, EBI2_RATE_SEL )  )  }, 

  /* Configure the ARM_DIV_MOD1 divider for div_4 = 96Mhz */
  {HWIO_OPERATION,   HWIO_ADDR(ARM_MOD_REG), (HWIO_ARM_MOD_REG_ARM_DIV_MOD1_DIV_4_FVAL <<  HWIO_SHFT( ARM_MOD_REG, ARM_DIV_MOD1 ) ) }, 

  /* Configure the ARM_DIV_MOD1 divider for div_3 = 128Mhz */
  {HWIO_OPERATION,   HWIO_ADDR(ARM_MOD_REG), (HWIO_ARM_MOD_REG_ARM_DIV_MOD1_DIV_3_FVAL <<  HWIO_SHFT( ARM_MOD_REG, ARM_DIV_MOD1 ) ) },  


  /*-----------------------------------------------------------------------
                            End of Configuration
  -----------------------------------------------------------------------*/   
  {END_OF_CFG_DATA,  0x00000000,  0x00000000  }
};


/* 1xmm Mode PLL @ 384Mhz  => PLL @ 384Mhz */
const static dbl_parser_cfg_data_item_type dbl_clk_pll_384_arm_48_ebi1_48_ebi2_24_pbl_TO_dbl_clk_pll_384_arm_128_ebi1_64_ebi2_32[] =
{
  /* Configure the ARM_DIV_MOD1 divider to div_8 = 48Mhz */
  {HWIO_OPERATION,   HWIO_ADDR(ARM_MOD_REG), ( HWIO_ARM_MOD_REG_ARM_DIV_MOD1_DIV_8_FVAL <<  HWIO_SHFT( ARM_MOD_REG, ARM_DIV_MOD1 ) ) }, 

  /* Configure SRC1 to select the PLL 
   * Switch to ARM_SRC1
   * ARM_RATE_SEL << div_1
   * AHB_RATE_SEL << div_2
   * EBI1_RATE_SEL << div_2
   * EBI2_RATE_SEL << div_4
   */
  {HWIO_OPERATION,   HWIO_ADDR(ARM_CFG_REG),  ( HWIO_ARM_CFG_REG_SRC_SEL1_PLL_SRC_FVAL     << HWIO_SHFT( ARM_CFG_REG, SRC_SEL1        )  |
                                                HWIO_ARM_CFG_REG_ARM_SRC_SEL_ARM_SRC1_FVAL << HWIO_SHFT( ARM_CFG_REG, ARM_SRC_SEL     )  |
                                                HWIO_ARM_CFG_REG_ARM_RATE_SEL_DIV_1_FVAL   <<  HWIO_SHFT( ARM_CFG_REG, ARM_RATE_SEL   )  |      
                                                HWIO_ARM_CFG_REG_AHB_RATE_SEL_DIV_2_FVAL   <<  HWIO_SHFT( ARM_CFG_REG, AHB_RATE_SEL   )  |
                                                HWIO_ARM_CFG_REG_EBI1_RATE_SEL_DIV_2_FVAL  <<   HWIO_SHFT( ARM_CFG_REG, EBI1_RATE_SEL )  |
                                                HWIO_ARM_CFG_REG_EBI2_RATE_SEL_DIV_4_FVAL  <<   HWIO_SHFT( ARM_CFG_REG, EBI2_RATE_SEL )  )  }, 

  /* Configure the ARM_DIV_MOD1 divider for div_4 = 96Mhz */
  {HWIO_OPERATION,   HWIO_ADDR(ARM_MOD_REG), (HWIO_ARM_MOD_REG_ARM_DIV_MOD1_DIV_4_FVAL <<  HWIO_SHFT( ARM_MOD_REG, ARM_DIV_MOD1 ) ) }, 

  /* Configure the ARM_DIV_MOD1 divider for div_3 = 128Mhz */
  {HWIO_OPERATION,   HWIO_ADDR(ARM_MOD_REG), (HWIO_ARM_MOD_REG_ARM_DIV_MOD1_DIV_3_FVAL <<  HWIO_SHFT( ARM_MOD_REG, ARM_DIV_MOD1 ) ) },  


  /*-----------------------------------------------------------------------
                            End of Configuration
  -----------------------------------------------------------------------*/   
  {END_OF_CFG_DATA,  0x00000000,  0x00000000  }
};

#endif /* #if defined (FEATURE_PLL_384) && defined (FEATURE_CLKREGIM_1X_MODE) */

#if defined (FEATURE_PLL_192) && defined (FEATURE_CLKREGIM_1X_MODE)
/* 1x Mode.  TCXO => PLL @ 192Mhz */
const static dbl_parser_cfg_data_item_type dbl_clk_tcxo_arm_20_ebi1_20_ebi2_20_pbl_TO_dbl_clk_pll_192_arm_96_ebi1_48_ebi2_24[] =
{
  /* Reset the PLL, put it in Bypass, and disable the Output */
  {HWIO_OPERATION,   HWIO_ADDR(PLL_MODE_REG), ( 0 << HWIO_SHFT( PLL_MODE_REG, RESET_N  ) |
                                                0 << HWIO_SHFT( PLL_MODE_REG, BYPASSNL ) |
                                                0 << HWIO_SHFT( PLL_MODE_REG, OUTCTRL  ) ) },
  /* Setup PLL to 192MHz. */
  {HWIO_OPERATION,   HWIO_ADDR(PLL_L_VAL_REG),     0xA         }, 
  {HWIO_OPERATION,   HWIO_ADDR(PLL_M_VAL_REG),     0x0         }, 
  {HWIO_OPERATION,   HWIO_ADDR(PLL_N_VAL_REG),     0x1         }, 
  {HWIO_OPERATION,   HWIO_ADDR(PLL_TEST_CTL_REG),  0x00000000  },
  {HWIO_OPERATION,   HWIO_ADDR(PLL_CONFIG_REG),    0x00200182  }, 

  /* Bring the PLL out of Bypass */
  {HWIO_OPERATION,   HWIO_ADDR(PLL_MODE_REG),  HWIO_FMSK( PLL_MODE_REG, BYPASSNL ) }, 

  /* Wait >= 5us */
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  20 * 5  }, 

  /* Bring the PLL out of Reset */
  {HWIO_OPERATION,   HWIO_ADDR(PLL_MODE_REG),  ( HWIO_FMSK( PLL_MODE_REG, RESET_N  ) |
                                                 HWIO_FMSK( PLL_MODE_REG, BYPASSNL ) ) },

  /* Wait >= 50us for PLLs to lock. */
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  20 * 50  }, 

  /* Enable PLL Output */
  {HWIO_OPERATION,   HWIO_ADDR(PLL_MODE_REG), ( HWIO_FMSK( PLL_MODE_REG, RESET_N  ) |
                                                HWIO_FMSK( PLL_MODE_REG, BYPASSNL ) |
                                                HWIO_FMSK( PLL_MODE_REG, OUTCTRL  ) ) }, 

  /* Configure the ARM_DIV_MOD1 divider to div_4 = 48Mhz */
  {HWIO_OPERATION,   HWIO_ADDR(ARM_MOD_REG), ( HWIO_ARM_MOD_REG_ARM_DIV_MOD1_DIV_4_FVAL <<  HWIO_SHFT( ARM_MOD_REG, ARM_DIV_MOD1 ) ) }, 

  /* Configure SRC1 to select the PLL 
   * Switch to ARM_SRC1
   * ARM_RATE_SEL << div_1
   * AHB_RATE_SEL << div_2
   * EBI1_RATE_SEL << div_2
   * EBI2_RATE_SEL << div_4
   */
  {HWIO_OPERATION,   HWIO_ADDR(ARM_CFG_REG),  ( HWIO_ARM_CFG_REG_SRC_SEL1_PLL_SRC_FVAL     << HWIO_SHFT( ARM_CFG_REG, SRC_SEL1        )  |
                                                HWIO_ARM_CFG_REG_ARM_SRC_SEL_ARM_SRC1_FVAL << HWIO_SHFT( ARM_CFG_REG, ARM_SRC_SEL     )  |
                                                HWIO_ARM_CFG_REG_ARM_RATE_SEL_DIV_1_FVAL   <<  HWIO_SHFT( ARM_CFG_REG, ARM_RATE_SEL   )  |      
                                                HWIO_ARM_CFG_REG_AHB_RATE_SEL_DIV_2_FVAL   <<  HWIO_SHFT( ARM_CFG_REG, AHB_RATE_SEL   )  |
                                                HWIO_ARM_CFG_REG_EBI1_RATE_SEL_DIV_2_FVAL  <<   HWIO_SHFT( ARM_CFG_REG, EBI1_RATE_SEL )  |
                                                HWIO_ARM_CFG_REG_EBI2_RATE_SEL_DIV_4_FVAL  <<   HWIO_SHFT( ARM_CFG_REG, EBI2_RATE_SEL )  )  }, 

  /* Configure the ARM_DIV_MOD1 divider for div_2 = 96Mhz */
  {HWIO_OPERATION,   HWIO_ADDR(ARM_MOD_REG), (HWIO_ARM_MOD_REG_ARM_DIV_MOD1_DIV_2_FVAL <<  HWIO_SHFT( ARM_MOD_REG, ARM_DIV_MOD1 ) ) }, 

  /*-----------------------------------------------------------------------
                            End of Configuration
  -----------------------------------------------------------------------*/   
  {END_OF_CFG_DATA,  0x00000000,  0x00000000  }
};

/* 1x Mode. PLL @ 384Mhz => PLL @ 192Mhz */
const static dbl_parser_cfg_data_item_type dbl_clk_pll_384_arm_48_ebi1_48_ebi2_24_pbl_TO_dbl_clk_pll_192_arm_96_ebi1_48_ebi2_24[] =
{
  /* Configure the ARM_DIV_MOD1 divider to div_8 = 48Mhz */
  {HWIO_OPERATION,   HWIO_ADDR(ARM_MOD_REG), ( HWIO_ARM_MOD_REG_ARM_DIV_MOD1_DIV_4_FVAL <<  HWIO_SHFT( ARM_MOD_REG, ARM_DIV_MOD1 ) ) }, 

  /* Switch to XO on ARM_SRC0 */
  {HWIO_OPERATION,   HWIO_ADDR(ARM_CFG_REG),  ( HWIO_ARM_CFG_REG_ARM_SRC_SEL_ARM_SRC0_FVAL << HWIO_SHFT( ARM_CFG_REG, ARM_SRC_SEL ) ) },


  /* Reset the PLL, put it in Bypass, and disable the Output */
  {HWIO_OPERATION,   HWIO_ADDR(PLL_MODE_REG), ( 0 << HWIO_SHFT( PLL_MODE_REG, RESET_N  ) |
                                                0 << HWIO_SHFT( PLL_MODE_REG, BYPASSNL ) |
                                                0 << HWIO_SHFT( PLL_MODE_REG, OUTCTRL  ) ) },

  /* Setup PLL to 192MHz. */
  {HWIO_OPERATION,   HWIO_ADDR(PLL_L_VAL_REG),     0xA         }, 
  {HWIO_OPERATION,   HWIO_ADDR(PLL_M_VAL_REG),     0x0         }, 
  {HWIO_OPERATION,   HWIO_ADDR(PLL_N_VAL_REG),     0x1         }, 
  {HWIO_OPERATION,   HWIO_ADDR(PLL_TEST_CTL_REG),  0x00000000  },
  {HWIO_OPERATION,   HWIO_ADDR(PLL_CONFIG_REG),    0x00200182  }, 

  /* Bring the PLL out of Bypass */
  {HWIO_OPERATION,   HWIO_ADDR(PLL_MODE_REG),  HWIO_FMSK( PLL_MODE_REG, BYPASSNL ) }, 

  /* Wait >= 5us */
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  20 * 5  }, 

  /* Bring the PLL out of Reset */
  {HWIO_OPERATION,   HWIO_ADDR(PLL_MODE_REG),  ( HWIO_FMSK( PLL_MODE_REG, RESET_N  ) |
                                                 HWIO_FMSK( PLL_MODE_REG, BYPASSNL ) ) },

  /* Wait >= 50us for PLLs to lock. */
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  20 * 50  }, 

  /* Enable PLL Output */
  {HWIO_OPERATION,   HWIO_ADDR(PLL_MODE_REG), ( HWIO_FMSK( PLL_MODE_REG, RESET_N  ) |
                                                HWIO_FMSK( PLL_MODE_REG, BYPASSNL ) |
                                                HWIO_FMSK( PLL_MODE_REG, OUTCTRL  ) ) }, 

  /* Configure the ARM_DIV_MOD1 divider to div_4 = 48Mhz */
  {HWIO_OPERATION,   HWIO_ADDR(ARM_MOD_REG), ( HWIO_ARM_MOD_REG_ARM_DIV_MOD1_DIV_4_FVAL <<  HWIO_SHFT( ARM_MOD_REG, ARM_DIV_MOD1 ) ) }, 

  /* Configure SRC1 to select the PLL 
   * Switch to ARM_SRC1
   * ARM_RATE_SEL << div_1
   * AHB_RATE_SEL << div_2
   * EBI1_RATE_SEL << div_2
   * EBI2_RATE_SEL << div_4
   */
  {HWIO_OPERATION,   HWIO_ADDR(ARM_CFG_REG),  ( HWIO_ARM_CFG_REG_SRC_SEL1_PLL_SRC_FVAL     << HWIO_SHFT( ARM_CFG_REG, SRC_SEL1        )  |
                                                HWIO_ARM_CFG_REG_ARM_SRC_SEL_ARM_SRC1_FVAL << HWIO_SHFT( ARM_CFG_REG, ARM_SRC_SEL     )  |
                                                HWIO_ARM_CFG_REG_ARM_RATE_SEL_DIV_1_FVAL   <<  HWIO_SHFT( ARM_CFG_REG, ARM_RATE_SEL   )  |      
                                                HWIO_ARM_CFG_REG_AHB_RATE_SEL_DIV_2_FVAL   <<  HWIO_SHFT( ARM_CFG_REG, AHB_RATE_SEL   )  |
                                                HWIO_ARM_CFG_REG_EBI1_RATE_SEL_DIV_2_FVAL  <<   HWIO_SHFT( ARM_CFG_REG, EBI1_RATE_SEL )  |
                                                HWIO_ARM_CFG_REG_EBI2_RATE_SEL_DIV_4_FVAL  <<   HWIO_SHFT( ARM_CFG_REG, EBI2_RATE_SEL )  )  }, 

  /* Configure the ARM_DIV_MOD1 divider for div_2 = 96Mhz */
  {HWIO_OPERATION,   HWIO_ADDR(ARM_MOD_REG), (HWIO_ARM_MOD_REG_ARM_DIV_MOD1_DIV_2_FVAL <<  HWIO_SHFT( ARM_MOD_REG, ARM_DIV_MOD1 ) ) }, 

  /*-----------------------------------------------------------------------
                            End of Configuration
  -----------------------------------------------------------------------*/   
  {END_OF_CFG_DATA,  0x00000000,  0x00000000  }
};

#endif /* #if defined (FEATURE_PLL_192) && defined (FEATURE_CLKREGIM_1X_MODE) */

#if defined (FEATURE_PLL_192) && defined (FEATURE_CLKREGIM_2X_MODE)
/* 2x Mode TCXO => PLL @ 192Mhz */
const static dbl_parser_cfg_data_item_type dbl_clk_tcxo_arm_20_ebi1_20_ebi2_20_pbl_TO_dbl_clk_pll_192_arm_96_ebi1_96_ebi2_24[] =
{
  /* Reset the PLL, put it in Bypass, and disable the Output */
  {HWIO_OPERATION,   HWIO_ADDR(PLL_MODE_REG), ( 0 << HWIO_SHFT( PLL_MODE_REG, RESET_N  ) |
                                                0 << HWIO_SHFT( PLL_MODE_REG, BYPASSNL ) |
                                                0 << HWIO_SHFT( PLL_MODE_REG, OUTCTRL  ) ) },
  /* Setup PLL to 192MHz. */
  {HWIO_OPERATION,   HWIO_ADDR(PLL_L_VAL_REG),     0xA         }, 
  {HWIO_OPERATION,   HWIO_ADDR(PLL_M_VAL_REG),     0x0         }, 
  {HWIO_OPERATION,   HWIO_ADDR(PLL_N_VAL_REG),     0x1         }, 
  {HWIO_OPERATION,   HWIO_ADDR(PLL_TEST_CTL_REG),  0x0         },
  {HWIO_OPERATION,   HWIO_ADDR(PLL_CONFIG_REG),    0x00200182  }, 

  /* Bring the PLL out of Bypass */
  {HWIO_OPERATION,   HWIO_ADDR(PLL_MODE_REG),  HWIO_FMSK( PLL_MODE_REG, BYPASSNL ) }, 

  /* Wait >= 5us */
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  20 * 5  }, 

  /* Bring the PLL out of Reset */
  {HWIO_OPERATION,   HWIO_ADDR(PLL_MODE_REG),  ( HWIO_FMSK( PLL_MODE_REG, RESET_N  ) |
                                                 HWIO_FMSK( PLL_MODE_REG, BYPASSNL ) ) },

  /* Wait >= 50us for PLLs to lock. */
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  20 * 50  }, 

  /* Enable PLL Output */
  {HWIO_OPERATION,   HWIO_ADDR(PLL_MODE_REG), ( HWIO_FMSK( PLL_MODE_REG, RESET_N  ) |
                                                HWIO_FMSK( PLL_MODE_REG, BYPASSNL ) |
                                                HWIO_FMSK( PLL_MODE_REG, OUTCTRL  ) ) }, 

  /* Configure the ARM_DIV_MOD1 divider to div_4 = 48Mhz */
  {HWIO_OPERATION,   HWIO_ADDR(ARM_MOD_REG), ( HWIO_ARM_MOD_REG_ARM_DIV_MOD1_DIV_4_FVAL <<  HWIO_SHFT( ARM_MOD_REG, ARM_DIV_MOD1 ) ) }, 

  /* Configure SRC1 to select the PLL 
   * Switch to ARM_SRC1
   * ARM_RATE_SEL << div_2
   * AHB_RATE_SEL << div_4
   * EBI1_RATE_SEL << div_2
   * EBI2_RATE_SEL << div_8
   */
  {HWIO_OPERATION,   HWIO_ADDR(ARM_CFG_REG),  ( HWIO_ARM_CFG_REG_SRC_SEL1_PLL_SRC_FVAL     << HWIO_SHFT( ARM_CFG_REG, SRC_SEL1        )  |
                                                HWIO_ARM_CFG_REG_ARM_SRC_SEL_ARM_SRC1_FVAL << HWIO_SHFT( ARM_CFG_REG, ARM_SRC_SEL     )  |
                                                HWIO_ARM_CFG_REG_ARM_RATE_SEL_DIV_2_FVAL   <<  HWIO_SHFT( ARM_CFG_REG, ARM_RATE_SEL   )  |      
                                                HWIO_ARM_CFG_REG_AHB_RATE_SEL_DIV_4_FVAL   <<  HWIO_SHFT( ARM_CFG_REG, AHB_RATE_SEL   )  |
                                                HWIO_ARM_CFG_REG_EBI1_RATE_SEL_DIV_2_FVAL  <<   HWIO_SHFT( ARM_CFG_REG, EBI1_RATE_SEL )  |
                                                HWIO_ARM_CFG_REG_EBI2_RATE_SEL_DIV_8_FVAL  <<   HWIO_SHFT( ARM_CFG_REG, EBI2_RATE_SEL )  )  }, 

  /* Configure the ARM_DIV_MOD1 divider for div_3 = 64Mhz */
  {HWIO_OPERATION,   HWIO_ADDR(ARM_MOD_REG), (HWIO_ARM_MOD_REG_ARM_DIV_MOD1_DIV_3_FVAL <<  HWIO_SHFT( ARM_MOD_REG, ARM_DIV_MOD1 ) ) }, 

  /* Configure the ARM_DIV_MOD1 divider for div_2 = 96Mhz */
  {HWIO_OPERATION,   HWIO_ADDR(ARM_MOD_REG), (HWIO_ARM_MOD_REG_ARM_DIV_MOD1_DIV_2_FVAL <<  HWIO_SHFT( ARM_MOD_REG, ARM_DIV_MOD1 ) ) }, 

  /* Configure the ARM_DIV_MOD1 divider for div_1 = 192Mhz */
  {HWIO_OPERATION,   HWIO_ADDR(ARM_MOD_REG), (HWIO_ARM_MOD_REG_ARM_DIV_MOD1_DIV_1_FVAL <<  HWIO_SHFT( ARM_MOD_REG, ARM_DIV_MOD1 ) ) }, 

  /*-----------------------------------------------------------------------
                            End of Configuration
  -----------------------------------------------------------------------*/   
  {END_OF_CFG_DATA,  0x00000000,  0x00000000  }
};

/* 2x Mode. PLL @ 384Mhz => PLL @ 192Mhz */
const static dbl_parser_cfg_data_item_type dbl_clk_pll_384_arm_48_ebi1_48_ebi2_24_pbl_TO_dbl_clk_pll_192_arm_96_ebi1_96_ebi2_24[] =
{
  /* Configure the ARM_DIV_MOD1 divider to div_8 = 48Mhz */
  {HWIO_OPERATION,   HWIO_ADDR(ARM_MOD_REG), ( HWIO_ARM_MOD_REG_ARM_DIV_MOD1_DIV_4_FVAL <<  HWIO_SHFT( ARM_MOD_REG, ARM_DIV_MOD1 ) ) }, 

  /* Switch to XO on ARM_SRC0 */
  {HWIO_OPERATION,   HWIO_ADDR(ARM_CFG_REG),  ( HWIO_ARM_CFG_REG_ARM_SRC_SEL_ARM_SRC0_FVAL << HWIO_SHFT( ARM_CFG_REG, ARM_SRC_SEL ) ) },

  /* Reset the PLL, put it in Bypass, and disable the Output */
  {HWIO_OPERATION,   HWIO_ADDR(PLL_MODE_REG), ( 0 << HWIO_SHFT( PLL_MODE_REG, RESET_N  ) |
                                                0 << HWIO_SHFT( PLL_MODE_REG, BYPASSNL ) |
                                                0 << HWIO_SHFT( PLL_MODE_REG, OUTCTRL  ) ) },
  /* Setup PLL to 192MHz. */
  {HWIO_OPERATION,   HWIO_ADDR(PLL_L_VAL_REG),     0xA         }, 
  {HWIO_OPERATION,   HWIO_ADDR(PLL_M_VAL_REG),     0x0         }, 
  {HWIO_OPERATION,   HWIO_ADDR(PLL_N_VAL_REG),     0x1         }, 
  {HWIO_OPERATION,   HWIO_ADDR(PLL_TEST_CTL_REG),  0x00000000  },
  {HWIO_OPERATION,   HWIO_ADDR(PLL_CONFIG_REG),    0x00200182  }, 

  /* Bring the PLL out of Bypass */
  {HWIO_OPERATION,   HWIO_ADDR(PLL_MODE_REG),  HWIO_FMSK( PLL_MODE_REG, BYPASSNL ) }, 

  /* Wait >= 5us */
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  20 * 5  }, 

  /* Bring the PLL out of Reset */
  {HWIO_OPERATION,   HWIO_ADDR(PLL_MODE_REG),  ( HWIO_FMSK( PLL_MODE_REG, RESET_N  ) |
                                                 HWIO_FMSK( PLL_MODE_REG, BYPASSNL ) ) },

  /* Wait >= 50us for PLLs to lock. */
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  20 * 50  }, 

  /* Enable PLL Output */
  {HWIO_OPERATION,   HWIO_ADDR(PLL_MODE_REG), ( HWIO_FMSK( PLL_MODE_REG, RESET_N  ) |
                                                HWIO_FMSK( PLL_MODE_REG, BYPASSNL ) |
                                                HWIO_FMSK( PLL_MODE_REG, OUTCTRL  ) ) }, 

  /* Configure the ARM_DIV_MOD1 divider to div_4 = 48Mhz */
  {HWIO_OPERATION,   HWIO_ADDR(ARM_MOD_REG), ( HWIO_ARM_MOD_REG_ARM_DIV_MOD1_DIV_4_FVAL <<  HWIO_SHFT( ARM_MOD_REG, ARM_DIV_MOD1 ) ) }, 

  /* Configure SRC1 to select the PLL 
   * Switch to ARM_SRC1
   * ARM_RATE_SEL << div_2
   * AHB_RATE_SEL << div_4
   * EBI1_RATE_SEL << div_2
   * EBI2_RATE_SEL << div_8
   */
  {HWIO_OPERATION,   HWIO_ADDR(ARM_CFG_REG),  ( HWIO_ARM_CFG_REG_SRC_SEL1_PLL_SRC_FVAL     << HWIO_SHFT( ARM_CFG_REG, SRC_SEL1        )  |
                                                HWIO_ARM_CFG_REG_ARM_SRC_SEL_ARM_SRC1_FVAL << HWIO_SHFT( ARM_CFG_REG, ARM_SRC_SEL     )  |
                                                HWIO_ARM_CFG_REG_ARM_RATE_SEL_DIV_2_FVAL   <<  HWIO_SHFT( ARM_CFG_REG, ARM_RATE_SEL   )  |      
                                                HWIO_ARM_CFG_REG_AHB_RATE_SEL_DIV_4_FVAL   <<  HWIO_SHFT( ARM_CFG_REG, AHB_RATE_SEL   )  |
                                                HWIO_ARM_CFG_REG_EBI1_RATE_SEL_DIV_2_FVAL  <<   HWIO_SHFT( ARM_CFG_REG, EBI1_RATE_SEL )  |
                                                HWIO_ARM_CFG_REG_EBI2_RATE_SEL_DIV_8_FVAL  <<   HWIO_SHFT( ARM_CFG_REG, EBI2_RATE_SEL )  )  }, 

  /* Configure the ARM_DIV_MOD1 divider for div_3 = 64Mhz */
  {HWIO_OPERATION,   HWIO_ADDR(ARM_MOD_REG), (HWIO_ARM_MOD_REG_ARM_DIV_MOD1_DIV_3_FVAL <<  HWIO_SHFT( ARM_MOD_REG, ARM_DIV_MOD1 ) ) }, 

  /* Configure the ARM_DIV_MOD1 divider for div_2 = 96Mhz */
  {HWIO_OPERATION,   HWIO_ADDR(ARM_MOD_REG), (HWIO_ARM_MOD_REG_ARM_DIV_MOD1_DIV_2_FVAL <<  HWIO_SHFT( ARM_MOD_REG, ARM_DIV_MOD1 ) ) }, 

  /* Configure the ARM_DIV_MOD1 divider for div_1 = 192Mhz */
  {HWIO_OPERATION,   HWIO_ADDR(ARM_MOD_REG), (HWIO_ARM_MOD_REG_ARM_DIV_MOD1_DIV_1_FVAL <<  HWIO_SHFT( ARM_MOD_REG, ARM_DIV_MOD1 ) ) }, 

  /*-----------------------------------------------------------------------
                            End of Configuration
  -----------------------------------------------------------------------*/   
  {END_OF_CFG_DATA,  0x00000000,  0x00000000  }
};

#endif /* #if defined (FEATURE_PLL_192) && defined (FEATURE_CLKREGIM_2X_MODE) */

typedef struct
{
  dbl_clk_speed_enum_type   dbl_clk_speed;
  dbl_clk_cfg_data_type     clk_data[BOOT_CLK_SOURCE_PBL_MAX];
}dbl_clk_cfg_data_tbl_type;


const static dbl_clk_cfg_data_tbl_type dbl_clk_cfg_data_table=
{
#if defined (FEATURE_PLL_384) && defined (FEATURE_CLKREGIM_1X_MODE)
    DBL_CLK_PLL_384_ARM_128_EBI1_64_EBI2_32,
    {
      {
        /*-----------------------------------------------------------------
          PBL Source TCXO
        -----------------------------------------------------------------*/
        &dbl_clk_tcxo_arm_20_ebi1_20_ebi2_20_pbl,
        &dbl_clk_pll_384_arm_128_ebi1_64_ebi2_32,
        dbl_clk_tcxo_arm_20_ebi1_20_ebi2_20_pbl_TO_dbl_clk_pll_384_arm_128_ebi1_64_ebi2_32
      },
      {
        /*-----------------------------------------------------------------
          PBL Source PLL at 384 Mhz
        -----------------------------------------------------------------*/
        &dbl_clk_pll_384_arm_48_ebi1_48_ebi2_24_pbl,
        &dbl_clk_pll_384_arm_128_ebi1_64_ebi2_32,
        dbl_clk_pll_384_arm_48_ebi1_48_ebi2_24_pbl_TO_dbl_clk_pll_384_arm_128_ebi1_64_ebi2_32
      }
    }
#elif defined (FEATURE_PLL_192) && defined (FEATURE_CLKREGIM_1X_MODE)
    DBL_CLK_PLL_192_ARM_96_EBI1_48_EBI2_24,
    {
      {
        /*-----------------------------------------------------------------
          PBL Source TCXO
        -----------------------------------------------------------------*/
        &dbl_clk_tcxo_arm_20_ebi1_20_ebi2_20_pbl,
        &dbl_clk_pll_192_arm_96_ebi1_48_ebi2_24,
        dbl_clk_tcxo_arm_20_ebi1_20_ebi2_20_pbl_TO_dbl_clk_pll_192_arm_96_ebi1_48_ebi2_24
      },
      {
        /*-----------------------------------------------------------------
          PBL Source PLL at 384 Mhz
        -----------------------------------------------------------------*/
        &dbl_clk_pll_384_arm_48_ebi1_48_ebi2_24_pbl,
        &dbl_clk_pll_192_arm_96_ebi1_48_ebi2_24,
        dbl_clk_pll_384_arm_48_ebi1_48_ebi2_24_pbl_TO_dbl_clk_pll_192_arm_96_ebi1_48_ebi2_24
      }
    }
#elif defined (FEATURE_PLL_192) && defined (FEATURE_CLKREGIM_2X_MODE)
    DBL_CLK_PLL_192_ARM_96_EBI1_96_EBI2_24,
    {
      {
        /*-----------------------------------------------------------------
          PBL Source TCXO
        -----------------------------------------------------------------*/
        &dbl_clk_tcxo_arm_20_ebi1_20_ebi2_20_pbl,
        &dbl_clk_pll_192_arm_96_ebi1_96_ebi2_24,
        dbl_clk_tcxo_arm_20_ebi1_20_ebi2_20_pbl_TO_dbl_clk_pll_192_arm_96_ebi1_96_ebi2_24
      },
      {
        /*-----------------------------------------------------------------
          PBL Source PLL at 384 Mhz
        -----------------------------------------------------------------*/
        &dbl_clk_pll_384_arm_48_ebi1_48_ebi2_24_pbl,
        &dbl_clk_pll_192_arm_96_ebi1_96_ebi2_24,
        dbl_clk_pll_384_arm_48_ebi1_48_ebi2_24_pbl_TO_dbl_clk_pll_192_arm_96_ebi1_96_ebi2_24
      }
    }
#else 

  #error "Clock Plan feature not defined." 
    
#endif
  };


/*=============================================================================   
                              FUNCTION DEFINITIONS
=============================================================================*/

/*===========================================================================

FUNCTION  DBL_CLK_GET_PTR_TO_SETTINGS

DESCRIPTION
 This function will configure the CLK interface.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
const dbl_clk_cfg_data_type* dbl_clk_get_ptr_to_settings( const boot_clk_speed_type *shared_clk_speed )
{
  if( shared_clk_speed->boot_clk_source == BOOT_CLK_SOURCE_TCXO )
  {
    return (&dbl_clk_cfg_data_table.clk_data[BOOT_CLK_SOURCE_TCXO]);
  }
  else if(shared_clk_speed->boot_clk_source == BOOT_CLK_SOURCE_PLL_384)
  {
    return (&dbl_clk_cfg_data_table.clk_data[BOOT_CLK_SOURCE_PLL_384]);
  }
  else 
  {
    return NULL;
  }
} /* dbl_clk_get_ptr_to_low_speed_settings */


