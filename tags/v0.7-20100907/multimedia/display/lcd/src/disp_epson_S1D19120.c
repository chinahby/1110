/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   L C D    D I S P L A Y    D R I V E R

GENERAL DESCRIPTION
  This module is the driver for the Epson S1D19120 QVGA LCD Display panel.

EXTERNALIZED FUNCTIONS
  epson_S1D19120_disp_init

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/display/mdp1/lcd_qsc1100/main/latest/src/disp_epson_S1D19120.c#5 $ $DateTime: 2008/12/08 07:02:59 $ $Author: rkashyap $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/08/08  rk       Changed EBI-2 wait states to avoid overwriting boot config.
08/07/08  rk       Added new EBI-2 wait states.
07/10/08  rk       LCD driver clean up
03/13/08  dw       Add support for dual 9-bit and single 32-bit pixel data 
                   transfers for debug purposes. 
                   Adjust LCD bus configurations for bring-up verifications.
03/07/08  dw       Initial version

============================================================================ */

/*===========================================================================

                         INCLUDE FILES FOR MODULE

===========================================================================*/

#include "target.h"
#include "disp.h"         /* Bit map LCD display driver interface */
#include "drvLib.h"
#include "clk.h"
#include "clkrgm_msm.h"
#include "disp_epson_S1D19120.h"
#include "gpio_1100.h"

#ifdef T_FFA
#error code not present
#endif

/*============================================================================

                 CONSTANT AND MACRO DEFINITIONS FOR MODULE

============================================================================*/
#define EPSON_S1D19120_DISP_WIDTH             240
#define EPSON_S1D19120_DISP_HEIGHT            320

#define EPSON_S1D19120_DISP_MIN_CONTRAST      0
#define EPSON_S1D19120_DISP_MAX_CONTRAST      255
#define EPSON_S1D19120_DISP_DEFAULT_CONTRAST  128

#define EPSON_S1D19120_DISP_MIN_BACKLIGHT     0 
#define EPSON_S1D19120_DISP_MAX_BACKLIGHT     7
#define EPSON_S1D19120_DISP_DEFAULT_BACKLIGHT 3

#define EPSON_S1D19120_DISP_ENABLE            TRUE
#define EPSON_S1D19120_DISP_DISABLE           FALSE

#define LCD_MAX_DISPLAYS 1
/*=====================================================================
      Constants and Macros 
======================================================================*/
#define LCD_CONTROLLER_IS_EPSON_S1D19120(nDisp)            \
           (LCD_QVGA_WIDTH == lptDispInfo[nDisp].width)  // TRUE: Epson S1D19120

#define CONVERT_16_TO_18_BIT(v) ((((v)&0x1F)<<1)|(((v)&0x7E0)<<1)|(((v)&0xF800)<<2))
#define PAD_18_TO_32_BIT(v)     ((((v)&0x3FE00)>>9)|(((v)&0x1FF)<<16))
static boolean test_32_bit_mode = TRUE;

/* Undefine backlight-related constant labels in case they are also 
 * defined in another display driver module */

#define LCD_VISIBLE_BACKLIGHT_LEVELS   \
        ( (EPSON_S1D19120_DISP_MAX_BACKLIGHT) - (EPSON_S1D19120_DISP_MIN_BACKLIGHT) )

// GYOCKEY: The following was borrowed from SC2x AMSS for the Verde display card

// Minimum PDM val for most dim display (without flicker) is different
// between SURF and FFA. The minimum observed for the FFA is 0x8D60.
// The minimum for SURF (with two LCD panels on Verde display card)
// is 0xA340 (any lower and the secondary panel's backlight flickers).
// If the FFA minimum is used on SURF, both LCD panels' backlights are
// completely dark at the GUI's lowest brightness setting.
//
// Since SC2x uses a single executable for both FFA and SURF, use the
// SURF minimum to be sure minimum backlight will be visible on both
// platforms.
#define LCD_PDM_MIN_BACKLIGHT           0x8000   // Backlight off
#define LCD_PDM_MIN_VISIBLE_BACKLIGHT   0xA340   // PDM val for most dim
                                                  // (lower value induces
                                                  // flicker)
#define LCD_PDM_MAX_VISIBLE_BACKLIGHT   0xFFFF   // PDM val for brightest
#define LCD_PDM_SPAN_VISIBLE_BACKLIGHT      \
        ( (LCD_PDM_MAX_VISIBLE_BACKLIGHT) - \
          (LCD_PDM_MIN_VISIBLE_BACKLIGHT) )
#define LCD_PDM_BACKLIGHT_STEP                \
        ( (LCD_PDM_SPAN_VISIBLE_BACKLIGHT) /  \
          (LCD_VISIBLE_BACKLIGHT_LEVELS) )

#define PEGS_EPSON_MFR_CODE      0
#define PEGS_EPSON_PRODUCT_CODE  0
#define PEGS_EPSON_ID_REGISTER                (LCD_CMD_WH) 

typedef struct
{
   pixel_format_type pixel_format; // RGB565, RGB666, etc.
   int col_offset, row_offset;  // Row&col offsets between HW controller & glass
   boolean is_parallel_iface;      // TRUE: EBI2 parallel bus, FALSE: MDDI

   // The following are only valid when interface is configured for parallel bus
   uint32 disp_id;            // Display ID expected from hardware
   lcd_ebi2_width_type bus_width;  // Bus width & transfer protocol
   uint32 cs_cfg0_addr;       // Chip Select Config 0 reg addr (e.g. LCD_CFG0)
   uint32 cs_cfg0;            // Chip Select config 0 value
   uint32 cs_cfg1_addr;       // Chip Select Config 1 reg addr (e.g. LCD_CFG1)
   uint32 cs_cfg1;            // Chip Select config 1 value
   uint32 cmd_addr;           // Port addr to send LCD commands 
   uint32 param_addr;         // Port addr to send LCD command parameters 
   uint32 data_addr;          // Port addr to send pixels
} lcd_config_attribs_type;

typedef struct
{
  uint32 width;
  uint32 height;
  uint32 bpp;
  uint32 mfr_code;
  uint32 product_code;
  char*  name;
} lcd_properties_type;


static void epson_S1D19120_disp_update
(
  const void *buf_ptr,        /* Buffer pointer */
  uint32 src_width,           /* Source image width */
  uint32 src_starting_row,    /* Source rectangle starting row */
  uint32 src_starting_column, /* Source rectangle starting column */
  uint32 num_of_rows,         /* Number of rows to update */
  uint32 num_of_columns,      /* Number of columns to update */
  uint32 dst_starting_row,    /* Device rectangle starting row */
  uint32 dst_starting_column  /* Device rectangle starting column */
);


/*=====================================================================
      Variables
======================================================================*/
// Capabilities and identification of LCD units mounted on Pegs
// display cards configured for a parallel (EBI2) interface to the MPU. 
// This info is for public as well as private use.
static lcd_properties_type lptDispInfo[LCD_MAX_DISPLAYS] =
{
   /* 
    * Display 0 (Primary - QVGA)
    */
   {
      EPSON_S1D19120_DISP_WIDTH, //240
      EPSON_S1D19120_DISP_HEIGHT, //320
      18,             // Bits per pixel    
      PEGS_EPSON_MFR_CODE, //0
      PEGS_EPSON_PRODUCT_CODE, //0
      "Pegs-EBI2-0",
   }
};

// Interface configuration and parallel-bus port attributes for
// Pegs displays. This info is for private use.
// 
// NOTE: By default, a 9-bit parallel interface is assumed. If during display
//       probing the 18-bit interface Pegs card is detected, the bus width
//       fields of this structure must be updated to EBI2_WIDTH_18.
// 
static lcd_config_attribs_type lcatDispCfg[LCD_MAX_DISPLAYS] =
{
    /* 
    * Display 0
    */   
   {
      pixel_format_rgb666_unpacked32,
      0,              // Column offset to viewable window
      0,              // Row offset to viewable window
      TRUE,           // Interface is parallel via EBI2 bus

      // Parallel bus interface attributes
      PEGS_EPSON_EBI2_ID,
      EBI2_WIDTH_2_x_9,
      HWIO_ADDR(LCD_CFG0),      // HWIO_LCD_CFG0_PHYS 
      0x04120105,
      /*(// lcd_cfg0 value   = 0x04120105, 
        (   LCD_CFG0_LCD_CS_SETUP = 0x0 )|
        (   LCD_CFG0_LCD_RECOVERY = 0x4)|
        (   LCD_CFG0_LCD_HOLD_WR = 0x1 )|
        (   LCD_CFG0_LCD_HOLD_RD = 0x2 )|                                 
        (   LCD_CFG0_LCD_WAIT_WR = 0x01 )|
        (   LCD_CFG0_LCD_WAIT_RD = 0x05 )  
      ),*/
      HWIO_ADDR(LCD_CFG1),      // HWIO_LCD_CFG1_PHYS
      0xC9000000,               //0x39000000,
      /*(                         // lcd_cfg1 value   = 0x39000000,
        (   0x1<<LCD_CFG1__LCD_D_ON_UB_LB___S       )|
        (   0x1<<LCD_CFG1__LCD_18_OR_24_BIT_ENA___S )|
        (   0x1<<LCD_CFG1__LCD_RS_ENA___S           )|
        (   0x1<<LCD_CFG1__LCD_RS_MODE___S          )
      ), */           
      LCD_CMD_WH,     // Port addr to send LCD commands
      LCD_DATA_WH,    // Port addr to send LCD command parameters
      LCD_DATA_WH,    // Port addr to send pixels
   }
};

/*============================================================================

                        STRUCTURE TYPES FOR MODULE

============================================================================*/

typedef struct 
{
  boolean disp_initialized;
  boolean display_on;
  boolean disp_powered_up;
  uint16  disp_area_start_row;
  uint16  disp_area_end_row;
} epson_S1D19120_state_type;

/*============================================================================

                           VARIABLE DECLARATION

============================================================================*/

static rex_crit_sect_type        epson_S1D19120_crit_sect = {0};
static disp_info_type            epson_S1D19120_disp_info = {0};
static epson_S1D19120_state_type epson_S1D19120_state = {0};

/*===========================================================================

                        LOCAL FUNCTIONS PROTOTYPES

============================================================================*/

int epson_S1D19120_install(char *str);

/*===========================================================================

                            FUNCTION PROTOTYPES

===========================================================================*/
/*===========================================================================

FUNCTION      epson_S1D19120_reset

DESCRIPTION
  Reset LCD controller

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/

static void epson_S1D19120_reset(void)
{
     /* Reset controller to ensure clean state */
     /* Transfer command to display hardware*/
     out_word(lcatDispCfg[0].cmd_addr, DISP_EPSON_CMD9(EPSON_S1D19120_SOFT_RESET_C));  
     clk_busy_wait(100);
	 
  } /*epson_S1D19120_reset */

/*===========================================================================

FUNCTION      epson_S1D19120_HW_INIT

DESCRIPTION
  Initialize MSM for TMD display

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  MSM initialized for TMD LCD operation

===========================================================================*/
static void epson_S1D19120_hw_init(void)
{
  uint32 regval = 0;

  /* Configure pull-ups for LCD-related pins */
  /* HWIO_EBI2_PAD_PULL_0_PHYS */
  regval = (0x0 << HWIO_EBI2_PAD_PULL_0_EBI2_LCD_CS1_N_SHFT )  | 
           (0x0 << HWIO_EBI2_PAD_PULL_0_EBI2_LCD_CS0_N_SHFT )  | 
           (0x0 << HWIO_EBI2_PAD_PULL_0_EBI2_OE_N_SHFT)        | 
           (0x0 << HWIO_EBI2_PAD_PULL_0_EBI2_WE_N_SHFT)        | 
           (0x0 << HWIO_EBI2_PAD_PULL_0_EBI2_LCDEN_SHFT)       | 
           (0x0 << HWIO_EBI2_PAD_PULL_0_EBI2_LCDRS_ADV_N_SHFT) |
           (0x0 << HWIO_EBI2_PAD_PULL_0_EBI2_A_D_BUS_SHFT);
            
  out_dword(HWIO_EBI2_PAD_PULL_0_ADDR, regval);

  /* HWIO_EBI2_PAD_PULL_1_PHYS */
  regval = (0x0 << HWIO_EBI2_PAD_PULL_1_EBI2_LCD_CS1_N_SHFT)   | 
           (0x0 << HWIO_EBI2_PAD_PULL_1_EBI2_LCD_CS0_N_SHFT)   | 
           (0x0 << HWIO_EBI2_PAD_PULL_1_EBI2_OE_N_SHFT)        | 
           (0x0 << HWIO_EBI2_PAD_PULL_1_EBI2_WE_N_SHFT)        | 
           (0x0 << HWIO_EBI2_PAD_PULL_1_EBI2_LCDEN_SHFT)       | 
           (0x0 << HWIO_EBI2_PAD_PULL_1_EBI2_LCDRS_ADV_N_SHFT) | 
           (0x1 << HWIO_EBI2_PAD_PULL_1_EBI2_A_D_BUS_SHFT);

  out_dword(HWIO_EBI2_PAD_PULL_1_ADDR, regval);

  /* Configure MSM/LCD(primary) interface registers (in MSM) */
  out_dword(lcatDispCfg[LCD_PRIMARY].cs_cfg0_addr, 
            lcatDispCfg[LCD_PRIMARY].cs_cfg0);
  out_dword(lcatDispCfg[LCD_PRIMARY].cs_cfg1_addr, 
            lcatDispCfg[LCD_PRIMARY].cs_cfg1);

#ifdef T_FFA
#error code not present
#endif /* T_FFA */

} /* epson_S1D19120_hw_init() */


/*===========================================================================

FUNCTION      epson_S1D19120_SET_SCREEN_AREA

DESCRIPTION
  Set up the LCD for a section of the screen to start displaying data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void epson_S1D19120_set_screen_area(uint32 start_row, uint32 start_col,
                                           uint32 end_row, uint32 end_col)
{
   int nDisp  = LCD_PRIMARY; /*TODO - Should come as a parameter */
  
   if ( nDisp > PEGS_EPSON_MAX_DISPLAYS )
   {
      nDisp = LCD_PRIMARY;
   }
   /* Compensate for any offset */
   start_col += lcatDispCfg[nDisp].col_offset;
   start_row += lcatDispCfg[nDisp].row_offset;
   end_col   += lcatDispCfg[nDisp].col_offset;
   end_row   += lcatDispCfg[nDisp].row_offset;

   /* 
    * Should put clipping in here
    */

   /* Range check compensated rectangle vertices */
   if ( (start_col < lptDispInfo[nDisp].width) && 
        (start_row < lptDispInfo[nDisp].height) && 
        (end_col   < lptDispInfo[nDisp].width)   && 
        (end_row   < lptDispInfo[nDisp].height) )
   {
      /* Set LCD hardware to new drawing rectangle */
        /* Set LCD hardware to set start address */
      	 /* Transfer command to display hardware*/
       out_word(lcatDispCfg[0].cmd_addr, DISP_EPSON_CMD9(EPSON_S1D19120_SET_START_ADDRESS_C));  
       out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9((uint8)start_col));
       out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9((uint8)(start_row>>8)));
       out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9((uint8)start_row));
	
      /* Set LCD hardware to set start address */
      /* Transfer command to display hardware */
       out_word(lcatDispCfg[0].cmd_addr, DISP_EPSON_CMD9( EPSON_S1D19120_SET_END_ADDRESS_C));  
       out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9((uint8) end_col));
       out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9((uint8)(end_row>>8)));
       out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9((uint8)end_row));
	
   }
} /* epson_S1D19120_set_screen_area() */

/*===========================================================================

FUNCTION      epson_S1D19120_set_backlight_pdm_util

DESCRIPTION
  Utility function to set LCD backlight. Invoked by epson_S1D19120_set_backlight function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void epson_S1D19120_set_backlight_pdm_util(uint16 nLevel)
{
   static uint16 wPdmVal = LCD_PDM_MAX_VISIBLE_BACKLIGHT;

   /* Handle min and max explicitly to be sure backlight is all the way off
    * or on. If level is intermediate, calculate PDM value */
   /* nLevel is guaranteed by the caller to be bound by the valid range */
  if (nLevel == EPSON_S1D19120_DISP_MIN_BACKLIGHT)
   {
      wPdmVal = LCD_PDM_MIN_BACKLIGHT;
   }
   else if (nLevel == EPSON_S1D19120_DISP_MAX_BACKLIGHT)
   {
      wPdmVal = LCD_PDM_MAX_VISIBLE_BACKLIGHT;
   }
   else
   {
      /* For levels (min+1)..(max-1), calculate PDM value */
      wPdmVal = (uint16)(LCD_PDM_MIN_VISIBLE_BACKLIGHT +
      ((nLevel - 1) * LCD_PDM_BACKLIGHT_STEP));                    
   }

/* Make sure TCXO4_CLK is running since TCXO_PDM_CTL has to be clocked by
      TCXO4_CLK. If TCXO4_CLK is disabled, then enable it */
  if(!clk_regime_msm_is_on(CLK_RGM_GEN_M ))
   {
    /*Switch on the clock if it is already not switched on*/
    clk_regime_msm_enable( CLK_RGM_GEN_M );
   }

   /* Set new PDM0 value */
   HWIO_PDM0_CTL_OUT(
      (wPdmVal & HWIO_PDM0_CTL_PDM0_DAT_BMSK) << HWIO_PDM0_CTL_PDM0_DAT_SHFT);

   if (nLevel > EPSON_S1D19120_DISP_MIN_BACKLIGHT)
   {
      /* Set PDM signal to normal polarity (sense of PDM output not inverted)*/
      HWIO_TCXO_PDM_CTL_OUTM(HWIO_TCXO_PDM_CTL_PDM0_EN_BMSK,
         (0 << HWIO_TCXO_PDM_CTL_PDM0_POLARITY_SHFT));

      /* Drive backlight by enabling PDM output*/
      HWIO_TCXO_PDM_CTL_OUTM(HWIO_TCXO_PDM_CTL_PDM0_EN_BMSK,
         (1 << HWIO_TCXO_PDM_CTL_PDM0_EN_SHFT));
   }
   else
   {
      /* Shutoff backlight completely by disabling PDM output */
      HWIO_TCXO_PDM_CTL_OUTM(HWIO_TCXO_PDM_CTL_PDM0_EN_BMSK,
      //   (HWIO_TCXO_PDM_CTL_PDM0_EN_ENABLES_THE_PDM0_PIN_FVAL <<
          (0<<  HWIO_TCXO_PDM_CTL_PDM0_EN_SHFT));   
   }
}


/*===========================================================================

FUNCTION      epson_S1D19120_DISP_SET_BACKLIGHT

DESCRIPTION
  This function sets the backlight level.
  If level is 0, backlight is turned off.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void epson_S1D19120_disp_set_backlight(byte level)
{
   static uint32 current_level =  EPSON_S1D19120_DISP_MIN_BACKLIGHT;
  
  if(epson_S1D19120_state.disp_initialized &&
     epson_S1D19120_state.disp_powered_up  &&
     epson_S1D19120_state.display_on)
  {
    
	  if (level!= current_level)
	  {
	    rex_enter_crit_sect(&epson_S1D19120_crit_sect);

        /* Bound given intensity */
        if (level > EPSON_S1D19120_DISP_MAX_BACKLIGHT )
		{
         level = EPSON_S1D19120_DISP_MAX_BACKLIGHT;
		}
        else if (EPSON_S1D19120_DISP_MIN_BACKLIGHT - level > 0)
		{
         /* Turn backlight off */
         level = EPSON_S1D19120_DISP_MIN_BACKLIGHT;
		}

  #ifdef FEATURE_PMIC_LCDKBD_LED_DRIVER
		{
         pm_err_flag_type pefRet = (pm_err_flag_type)~PM_ERR_FLAG__SUCCESS;  // Assume failure

         pefRet = pm_set_led_intensity(PM_KBD_LED, (uint8)level);
         if (PM_ERR_FLAG__SUCCESS != pefRet)
		 {
          MSG_FATAL("PMIC KBD backlight set failed: pefRet=%d, nLevel=%d",
                pefRet,
                level,
                0);
		 }

  #ifdef FEATURE_PDM_BACKLIGHT
        /* SC2X/ULC use GPIO PDM to drive the LCD backlight, not the PMIC core */
        epson_S1D19120_set_backlight_pdm_util(level);
  #else  /* FEATURE_PDM_BACKLIGHT*/
        pefRet = pm_set_led_intensity(PM_LCD_LED, (uint8)level);   
        if (PM_ERR_FLAG__SUCCESS != pefRet)
		{
         MSG_MED("PMIC LCD backlight set failed: pefRet=%d, nLevel=%d",
                pefRet,
                nLevel,
                0);
		}
  #endif /* FEATURE_PDM_BACKLIGHT */
		}
  #else  /* FEATURE_PMIC_LCDKBD_LED_DRIVER */
    #ifdef FEATURE_PDM_BACKLIGHT
       epson_S1D19120_set_backlight_pdm_util(level);
    #endif /* FEATURE_PDM_BACKLIGHT */
  #endif /* FEATURE_PMIC_LCDKBD_LED_DRIVER */

       current_level = level;   
       rex_leave_crit_sect(&epson_S1D19120_crit_sect);
      }
    }
} /* epson_S1D19120_disp_set_backlight */

/*===========================================================================

FUNCTION      epson_S1D19120_DISP_CLEAR_SCREEN_AREA

DESCRIPTION
  This function clears an area of the screen.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void epson_S1D19120_disp_clear_screen_area(word start_row, word start_col,
                                                  word end_row, word end_col)
{
  uint32 i = (end_row - start_row + 1) * (end_col - start_col + 1);
  static uint32 white_16bpp = 0xFFFF;
  uint32 white_18bpp = CONVERT_16_TO_18_BIT(white_16bpp);
  uint32 white_32bpp = PAD_18_TO_32_BIT(white_18bpp);

  if (epson_S1D19120_state.disp_initialized &&
      epson_S1D19120_state.disp_powered_up  &&
      epson_S1D19120_state.display_on)
  {
    rex_enter_crit_sect(&epson_S1D19120_crit_sect);

    epson_S1D19120_set_screen_area(start_row, start_col, end_row, end_col);
    /* Transfer command to display hardware */
    out_word(lcatDispCfg[0].cmd_addr, DISP_EPSON_CMD9(EPSON_S1D19120_RAM_WRITE_C));  
    while(i--)
    {
      if(!test_32_bit_mode)
      {
        out_word(lcatDispCfg[0].data_addr, (uint16)(white_18bpp>>9));
        out_word(lcatDispCfg[0].data_addr, (uint16)(white_18bpp));
	    }
	    else
      {
        out_dword(lcatDispCfg[0].data_addr, (uint32)(white_32bpp));
	    }
    }
    
    rex_leave_crit_sect(&epson_S1D19120_crit_sect);
  }
} /* epson_S1D19120_disp_clear_screen_area() */


/*===========================================================================

FUNCTION      epson_S1D19120_DISP_CLEAR_WHOLE_SCREEN

DESCRIPTION
  This function clears the whole screen.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void epson_S1D19120_disp_clear_whole_screen(void)
{
  epson_S1D19120_disp_clear_screen_area(0, 
                                        0,
                                        epson_S1D19120_disp_info.disp_height-1,
                                        epson_S1D19120_disp_info.disp_width-1);
} /* epson_S1D19120_disp_clear_whole_screen() */


/*===========================================================================

FUNCTION      epson_S1D19120_DISP_OFF

DESCRIPTION
  This function turns off the display.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void epson_S1D19120_disp_off(void)
{
  rex_enter_crit_sect(&epson_S1D19120_crit_sect);

  if(epson_S1D19120_state.disp_powered_up && epson_S1D19120_state.display_on)
  {
    /* Add code here */
  /* Transfer command to display hardware */
    out_word(lcatDispCfg[0].cmd_addr, DISP_EPSON_CMD9(EPSON_S1D19120_DISPLAY_OFF_C));  
    clk_busy_wait(100);
    epson_S1D19120_disp_set_backlight(EPSON_S1D19120_DISP_MIN_BACKLIGHT);
    /* Display put to SLEEP state */
    epson_S1D19120_state.display_on = FALSE;
  }

  rex_leave_crit_sect(&epson_S1D19120_crit_sect);
} /* epson_S1D19120_disp_off() */


/*===========================================================================

FUNCTION      epson_S1D19120_DISP_ON

DESCRIPTION
  This function turns on the display.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void epson_S1D19120_disp_on(void)
{
  rex_enter_crit_sect(&epson_S1D19120_crit_sect);

  if(epson_S1D19120_state.disp_powered_up && !epson_S1D19120_state.display_on)
  {  
    /* Transfer command to display hardware */
    out_word(lcatDispCfg[0].cmd_addr, DISP_EPSON_CMD9(EPSON_S1D19120_DISPLAY_ON_C));  
    clk_busy_wait(100);

    /* Display put to ACTIVE state */
    epson_S1D19120_state.display_on = TRUE;
  }

  rex_leave_crit_sect(&epson_S1D19120_crit_sect);
} /* epson_S1D19120_disp_on() */


/*===========================================================================

FUNCTION      epson_S1D19120_DISP_POWERUP

DESCRIPTION
  This function powers up the device.

DEPENDENCIES
  None

RETURN VALUE
  Display buffer address

SIDE EFFECTS
  None

===========================================================================*/
static void epson_S1D19120_disp_powerup(void)
{
  rex_enter_crit_sect(&epson_S1D19120_crit_sect);

  if (!epson_S1D19120_state.disp_powered_up && !epson_S1D19120_state.display_on)
  {
    /* 
     * Power-up controller 
     */
  	/* Transfer command to display hardware */
   out_word(lcatDispCfg[0].cmd_addr, DISP_EPSON_CMD9(EPSON_S1D19120_VDD_ON_C));  
   clk_busy_wait(100);

   /* 
    * Reset controller to a known state before initializing 
    */
     /* Transfer command to display hardware */
   /*out_word(lcatDispCfg[0].cmd_addr, DISP_EPSON_CMD9(EPSON_S1D19120_SOFT_RESET_C));  
   clk_busy_wait(100);*/
   epson_S1D19120_reset();

   /* 
    * Configure display attributes 
    */
   /* Set Display */
    /* Transfer command to display hardware */
   out_word(lcatDispCfg[0].cmd_addr, DISP_EPSON_CMD9(EPSON_S1D19120_SET_DISPLAY_C));  
   out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x31)); // Num clocks for 1H [row] period
   out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x00));// LCD Type
   out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9((uint8)((lptDispInfo[0].height - 1)>>8)));// Number of lines/rows MSB   
   out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9((uint8)lptDispInfo[0].height - 1)); // Number of lines/rows LSB
   out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x03)); // Back Porch line
   out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x00));// Front Porch line MSB
   out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x08)); // Front Porch line LSB

   /* Set Display Timing */
   /* Transfer command to display hardware */
   out_word(lcatDispCfg[0].cmd_addr, DISP_EPSON_CMD9(EPSON_S1D19120_SET_DISPLAY_TIMING_C));  
   out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x01));// Source output ON time
   out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x2E)); // Source output OFF time
   out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x0A));// Gate signal Select time
   out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x2C));// Gate signal De-Select Time
   out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x23));// Drive Mode (Boost/No Power Saving/Switch mode)
   out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x2F));// Equalizing start timing
   out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x00));// Gamma-circuit power save setting, Equalize setting

   /* Set Data */
   /* Transfer command to display hardware */
   out_word(lcatDispCfg[0].cmd_addr, DISP_EPSON_CMD9(EPSON_S1D19120_SET_DATA_C));   // Built in Osc
   out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x00));// Built in Osc

   /* Set Display Start Line */
     /* Transfer command to display hardware */
   out_word(lcatDispCfg[0].cmd_addr, DISP_EPSON_CMD9(EPSON_S1D19120_SET_DISPLAY_START_LINE_C));  
   out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x00));

   /* Set Display Data Interface */
   /* Transfer command to display hardware */
   out_word(lcatDispCfg[0].cmd_addr, DISP_EPSON_CMD9(EPSON_S1D19120_SET_DISPLAY_DATA_INTERFACE_C));  
   out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x00)); // Ctrlr/Panel iface is always 18 bits wide
   out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x04));// DOTCLK divide ratio

   /* Set Display Color Mode */
   /* Transfer command to display hardware*/
   out_word(lcatDispCfg[0].cmd_addr, DISP_EPSON_CMD9(EPSON_S1D19120_SET_DISPLAY_COLOR_MODE_C));  
   out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x00)); // 262K color mode

   /* Set Gate Line Scan Mode */
   /* Transfer command to display hardware */
   out_word(lcatDispCfg[0].cmd_addr, DISP_EPSON_CMD9(EPSON_S1D19120_GATE_LINE_SCAN_MODE_C ));  
   out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x00));// Normal direction scan

   /* Set AC Operation Drive */
    /* Transfer command to display hardware */
  out_word(lcatDispCfg[0].cmd_addr, DISP_EPSON_CMD9(EPSON_S1D19120_SET_AC_OPERATION_DRIVE_C));  
  out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x00));// Default 0: 1-line reverse driving

   /* Set Electronic Control */
   /* Transfer command to display hardware*/
  out_word(lcatDispCfg[0].cmd_addr, DISP_EPSON_CMD9(EPSON_S1D19120_SET_ELECTRONIC_CONTROL_C));
   out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x10));  // VDDHS  4.0V
   out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x80));  // VCOMH  5.2V
   out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x11));  // VCA    2.35V
   
  out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x1B));// VONREG 5.1V
  out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x02));// VOFREG 2.6V
  out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x0D));   // VDDRH  3.7V
  out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x00));// VDDRL  0.0V

   /* Set Positive Polarity Gamma Correction Chars */
     /* Transfer command to display hardware*/
   out_word(lcatDispCfg[0].cmd_addr, DISP_EPSON_CMD9(EPSON_S1D19120_SET_POS_GAMMA_CORRECTN_CHARS_C));  
   out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x37));
   out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0xFF));
   out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x7F));
   out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x15));
   out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x37));

   /* Set Negative Polarity Gamma Correction Chars */
   /* Transfer command to display hardware */
   out_word(lcatDispCfg[0].cmd_addr, DISP_EPSON_CMD9(EPSON_S1D19120_SET_NEG_GAMMA_CORRECTN_CHARS_C));  
   out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x37));
   out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0xFF));
   out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x7F));
   out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x15));
   out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x37));

   /* Set Power Control */
   /* Transfer command to display hardware */
   out_word(lcatDispCfg[0].cmd_addr, DISP_EPSON_CMD9(EPSON_S1D19120_SET_POWER_CONTROL_C));  
   out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x01)); // Wait time
   out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x24));  // Wait time
   out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x0F));  // DCDC-converter settings
   out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0xFE));  // DCDC-converter switching
   out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x33));  // DCDC-converter freqency
   out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x31));  // reference voltage setttings
   out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0xFF));   // regulator settings
   out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x03));   // regulator strength setting 1.
   out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x00)); // regulator strength setting 2.
   out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x77)); // pre-buffer strength
   out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x33));   // Gamma amplifier settings
   out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x11)); // Gamma amplifier strength
   out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x44));  // Bias circuit strength
   out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x00));  // White writting method in off sequence

   /* Set Partial Power Control */
   /* Transfer command to display hardware */
  out_word(lcatDispCfg[0].cmd_addr, DISP_EPSON_CMD9(EPSON_S1D19120_SET_PARTIAL_POWER_CONTROL_C));  
  out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x33));  // DCDC-converter settings
  out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0xFF));   // regulator settings
  out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x03)); // regulator strength setting 1
  out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x00));  // regulator strength setting 2
  out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x44));   // Bias circuit strength
  out_word(lcatDispCfg[0].param_addr, DISP_EPSON_PARAM9(0x00));  // White writting method in off sequence


   /* 
    * Wake up controller
    */
    /* Transfer command to display hardware */
  out_word(lcatDispCfg[0].cmd_addr, DISP_EPSON_CMD9(EPSON_S1D19120_SLEEP_OUT_C));  
   
   clk_busy_wait(100);

    /* Display put to SLEEP state */
    epson_S1D19120_state.disp_powered_up = TRUE;
  }

   rex_leave_crit_sect(&epson_S1D19120_crit_sect);
} /* epson_S1D19120_disp_powerup() */


/*===========================================================================

FUNCTION      epson_S1D19120_DISP_POWERDOWN

DESCRIPTION
  This function powers down the device.

DEPENDENCIES
  None

RETURN VALUE
  Return True if success, False otherwise

SIDE EFFECTS
  None

===========================================================================*/
static int epson_S1D19120_disp_powerdown(void)
{
  rex_enter_crit_sect(&epson_S1D19120_crit_sect);

  if (epson_S1D19120_state.disp_powered_up && !epson_S1D19120_state.display_on) 
  {
    /* 
     * Power-down the controller 
     */
    /* Transfer command to display hardware */
    /* out_word(lcatDispCfg[0].cmd_addr, DISP_EPSON_CMD9(EPSON_S1D19120_SOFT_RESET_C));  
     clk_busy_wait(100);*/
     epson_S1D19120_reset();
    /* Reset controller to ensure clean state */
   /* Transfer command to display hardware*/
     out_word(lcatDispCfg[0].cmd_addr, DISP_EPSON_CMD9(EPSON_S1D19120_VDD_OFF_C));  
     clk_busy_wait(100);
     epson_S1D19120_state.disp_powered_up = FALSE;
  }

  rex_leave_crit_sect(&epson_S1D19120_crit_sect);

  return FALSE;
} /* epson_S1D19120_disp_powerdown() */


/*===========================================================================

FUNCTION      epson_S1D19120_DISP_INIT

DESCRIPTION
  This function initialize the Pegs Toshiba display panel

DEPENDENCIES
  Assumes that this function will only be called once at boot up

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
int epson_S1D19120_disp_init(void)
{
  if (epson_S1D19120_state.disp_initialized) 
  {
    /* Do not re-initialize the display */
    return 1;
  }

  epson_S1D19120_disp_info.disp_width         = EPSON_S1D19120_DISP_WIDTH;
  epson_S1D19120_disp_info.disp_height        = EPSON_S1D19120_DISP_HEIGHT;
  epson_S1D19120_disp_info.bpp                = DISP_18BPP;
  epson_S1D19120_disp_info.palette_support    = FALSE;
  epson_S1D19120_disp_info.contrast_support   = FALSE;
  epson_S1D19120_disp_info.contrast_min       = EPSON_S1D19120_DISP_MIN_CONTRAST;
  epson_S1D19120_disp_info.contrast_max       = EPSON_S1D19120_DISP_MAX_CONTRAST;
  epson_S1D19120_disp_info.contrast_default   = EPSON_S1D19120_DISP_DEFAULT_CONTRAST;
  epson_S1D19120_disp_info.backlight_support  = TRUE;
  epson_S1D19120_disp_info.backlight_min      = EPSON_S1D19120_DISP_MIN_BACKLIGHT;
  epson_S1D19120_disp_info.backlight_max      = EPSON_S1D19120_DISP_MAX_BACKLIGHT;
  epson_S1D19120_disp_info.backlight_default  = EPSON_S1D19120_DISP_DEFAULT_BACKLIGHT;
  epson_S1D19120_disp_info.lcd_type           = EPSON_QVGA;
  epson_S1D19120_disp_info.phys_width         = EPSON_QVGA_LCD_DIMENSION_WIDTH;    
  epson_S1D19120_disp_info.phys_height        = EPSON_QVGA_LCD_DIMENSION_HEIGHT; 

  rex_init_crit_sect(&epson_S1D19120_crit_sect);

  epson_S1D19120_hw_init();

  epson_S1D19120_disp_powerup();

  epson_S1D19120_disp_on();

  epson_S1D19120_state.disp_initialized = TRUE;

  epson_S1D19120_disp_clear_whole_screen();

  epson_S1D19120_disp_set_backlight(EPSON_S1D19120_DISP_MAX_BACKLIGHT);

  return 1;
} /* epson_S1D19120_disp_init() */


/*===========================================================================

FUNCTION      epson_S1D19120_DISP_GET_INFO

DESCRIPTION
  This function returns display information

DEPENDENCIES
  None

RETURN VALUE
  Display information

SIDE EFFECTS
  None

===========================================================================*/
static disp_info_type epson_S1D19120_disp_get_info(void)
{
  return epson_S1D19120_disp_info;
} /* epson_S1D19120_disp_get_info() */


/*===========================================================================

FUNCTION      epson_S1D19120_DISP_COPY

DESCRIPTION
  Copy image data from a buffer to the LCD display

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  LCD is updated

===========================================================================*/
static void epson_S1D19120_disp_copy(void *src_ptr, dword copy_count)
{
  
  int nDisp = 0;
  int i;
  uint32 data;
  uint16 *pdata = src_ptr;

  /* Bus is wired from LCD D9-D17 (9 bits) to MPU bus, need two transfer 
  * to complete one pixel(RGB666).
  *
  * first write    second write
  * D17--------D9  D17--------D9
  * R[5:0]+G[5:3]  G[2:0]+B[5:3]
  *
  * each loop will write one pixel
  */

  if(pdata != NULL)
  {
   if(!test_32_bit_mode)
   {
    for ( i = 0; i < copy_count; i++ )
    {
      data = CONVERT_16_TO_18_BIT(*pdata);
      pdata++;

      out_word(lcatDispCfg[nDisp].data_addr, (uint16)(data>>9));
      out_word(lcatDispCfg[nDisp].data_addr, (uint16)(data));
    }
   }
   else
   {
    /* UXMC on ULC bus-sizes 32-bit data into 16-bit transfers
     * and this scheme reduces one chip select hold time between each
     * 16-bit transfer */

    for ( i = 0; i < copy_count; i++ )
    {
      data = CONVERT_16_TO_18_BIT(*pdata);
      data = PAD_18_TO_32_BIT(data);
      pdata++;

      out_dword(lcatDispCfg[nDisp].data_addr, (uint32)(data));
    }
   }
  }
  else 
  	{
  	  	MSG_ERROR(" Illegal Src Ptr assigned",0,0,0);
  	}
} /* epson_S1D19120_disp_copy() */


/*===========================================================================

FUNCTION      epson_S1D19120_DISP_UPDATE

DESCRIPTION
  Update LCD display with image from a buffer. The position of the image
  sent to the display is also supplied in parameters.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  LCD is updated

===========================================================================*/
static void epson_S1D19120_disp_update
(
  const void *buf_ptr,        /* Buffer pointer */
  uint32 src_width,           /* Source image width */
  uint32 src_starting_row,    /* Source rectangle starting row */
  uint32 src_starting_column, /* Source rectangle starting column */
  uint32 num_of_rows,         /* Number of rows to update */
  uint32 num_of_columns,      /* Number of columns to update */
  uint32 dst_starting_row,    /* Device rectangle starting row */
  uint32 dst_starting_column  /* Device rectangle starting column */
)
{
  uint16* src_ptr = NULL;

  if (!buf_ptr                          || 
      !epson_S1D19120_state.disp_initialized ||
      !epson_S1D19120_state.disp_powered_up  ||
      !epson_S1D19120_state.display_on)
  {
    return;
  }

  if ((dst_starting_row + num_of_rows) > epson_S1D19120_disp_info.disp_height) {
    num_of_rows = epson_S1D19120_disp_info.disp_height - dst_starting_row;
  }
  if ((dst_starting_column + num_of_columns) > epson_S1D19120_disp_info.disp_width) {
    num_of_columns = epson_S1D19120_disp_info.disp_width - dst_starting_column;
  }

  /* Ensure buffer aligned and parameters valid */
  if((src_starting_row    < epson_S1D19120_disp_info.disp_height) &&
     (src_starting_column < epson_S1D19120_disp_info.disp_width)  &&
     (dst_starting_row    < epson_S1D19120_disp_info.disp_height) &&
     (dst_starting_column < epson_S1D19120_disp_info.disp_width)  &&
     (!(((uint32)buf_ptr) & 0x3)))
  {
    rex_enter_crit_sect(&epson_S1D19120_crit_sect);    

    epson_S1D19120_set_screen_area(dst_starting_row, dst_starting_column, 
                                   dst_starting_row + num_of_rows - 1,
                                   dst_starting_column + num_of_columns -1);

    src_ptr = (uint16*)buf_ptr;
    src_ptr += src_starting_row * src_width + src_starting_column;

    /* Transfer command to display hardware */
    out_word(lcatDispCfg[0].cmd_addr, DISP_EPSON_CMD9(EPSON_S1D19120_RAM_WRITE_C));  

    if((src_starting_column == 0) && (num_of_columns == src_width)) 
    {
      uint32 copy_count;

      /* The whole row is updated, copy the whole update area */
      copy_count = num_of_rows * num_of_columns;

      epson_S1D19120_disp_copy(src_ptr, copy_count);
    } 
    else 
    {
      /* Partial row is updated, need to copy one row at a time */
      int16 row;
      uint32 copy_count;
  
      copy_count = num_of_columns;
      for (row = 0; row < num_of_rows; row++)
      {
        epson_S1D19120_disp_copy(src_ptr, copy_count);
        src_ptr += src_width;
      }
    }
	rex_leave_crit_sect(&epson_S1D19120_crit_sect);
  }

  
} /* epson_S1D19120_disp_update() */


/*===========================================================================

FUNCTION      DISP_SET_CONTRAST

DESCRIPTION
  This function sets the contrast of the display device.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void epson_S1D19120_disp_set_contrast(word contrast)

{
  return;
} /* epson_S1D19120_disp_set_contrast() */


/*===========================================================================

FUNCTION      epson_S1D19120_DISP_IOCTL

DESCRIPTION
  This function installs LCD device driver

DEPENDENCIES
  None

RETURN VALUE
  >0 if success, <0 if failure

SIDE EFFECTS
  None

===========================================================================*/
static int epson_S1D19120_disp_ioctl ( int cmd, void *arg )
{
  disp_update_type *disp_update_cmd;

  switch (cmd) {
  case IOCTL_DISP_UPDATE:
  case IOCTL_DISP_UPDATE_UNDER_ERR_FATAL:
    disp_update_cmd = (disp_update_type*)arg;
    /* bitwise OR all int16 together. If the result is
     * less than 0, then at least one of them is negative */
    if((disp_update_cmd->src_width |
        disp_update_cmd->src_starting_row |
        disp_update_cmd->src_starting_column |
        disp_update_cmd->num_of_rows |
        disp_update_cmd->num_of_columns |
        disp_update_cmd->dst_starting_row |
        disp_update_cmd->dst_starting_column ) >= 0)
    {
      epson_S1D19120_disp_update(disp_update_cmd->buf_ptr,
                                (uint32)disp_update_cmd->src_width,
                                (uint32)disp_update_cmd->src_starting_row,
                                (uint32)disp_update_cmd->src_starting_column,
                                (uint32)disp_update_cmd->num_of_rows,
                                (uint32)disp_update_cmd->num_of_columns,
                                (uint32)disp_update_cmd->dst_starting_row,
                                (uint32)disp_update_cmd->dst_starting_column);
    }
    break;

  case IOCTL_DISP_GET_INFO:
    *(disp_info_type *)arg = epson_S1D19120_disp_get_info();
    break;

  case IOCTL_DISP_OFF:
    epson_S1D19120_disp_off();
    break;

  case IOCTL_DISP_ON:
    epson_S1D19120_disp_on();
    break;

  case IOCTL_DISP_POWERUP:
    epson_S1D19120_disp_powerup();
    break;

  case IOCTL_DISP_SET_CONTRAST:
    epson_S1D19120_disp_set_contrast(*(word *)arg);
    break;

  case IOCTL_DISP_SET_BACKLIGHT:
    epson_S1D19120_disp_set_backlight(*(byte *)arg);
    break;

  case IOCTL_DISP_CLEAR_WHOLE_SCREEN:
    epson_S1D19120_disp_clear_whole_screen();
    break;

  case IOCTL_DISP_CLEAR_SCREEN_AREA:
    epson_S1D19120_disp_clear_screen_area(((disp_cls_type *)arg)->start_row,
                                          ((disp_cls_type *)arg)->start_column,
                                          ((disp_cls_type *)arg)->end_row,                                          
                                          ((disp_cls_type *)arg)->end_column);
    break;
  default: 
    return -1;
  }
  return 1;
} /* epson_S1D19120_disp_ioctl() */


/*===========================================================================

FUNCTION      EPSON_S1D19120_INSTALL

DESCRIPTION
  This function installs LCD device driver

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

========================================================================*/
int epson_S1D19120_install(char * str)
{
  return drv_install(str,
                     NULL,
                     NULL,
                     epson_S1D19120_disp_ioctl,
                     epson_S1D19120_disp_init,
                     epson_S1D19120_disp_powerdown);
} /* epson_S1D19120_install() */
