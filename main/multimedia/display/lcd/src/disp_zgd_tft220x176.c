/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   L C D    D I S P L A Y    D R I V E R

GENERAL DESCRIPTION
  This module is the driver for the Epson S1D19120 QVGA LCD Display panel.

EXTERNALIZED FUNCTIONS
  zgd_tft220x176_disp_init

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
#include "disp_zgd_tft220X176.h"
#include "gpio_1100.h"
#include "pm.h"
#include "pmapp.h"
#ifdef T_FFA
#error code not present
#endif

/*============================================================================

                 CONSTANT AND MACRO DEFINITIONS FOR MODULE

============================================================================*/

#define ZGD_TFT220X176_DISP_WIDTH             176
#define ZGD_TFT220X176_DISP_HEIGHT            220

#define ZGD_TFT220X176_DISP_MIN_CONTRAST      0
#define ZGD_TFT220X176_DISP_MAX_CONTRAST      255
#define ZGD_TFT220X176_DISP_DEFAULT_CONTRAST  128

#define ZGD_TFT220X176_DISP_MIN_BACKLIGHT     0 
#define ZGD_TFT220X176_DISP_MAX_BACKLIGHT     7
#define ZGD_TFT220X176_DISP_DEFAULT_BACKLIGHT 3

#define ZGD_TFT220X176_DISP_ENABLE            TRUE
#define ZGD_TFT220X176_DISP_DISABLE           FALSE

#define LCD_MAX_DISPLAYS 1
/*=====================================================================
      Constants and Macros 
======================================================================*/
/* Undefine backlight-related constant labels in case they are also 
 * defined in another display driver module */

#define LCD_VISIBLE_BACKLIGHT_LEVELS   \
        ( (ZGD_TFT220X176_DISP_MAX_BACKLIGHT) - (ZGD_TFT220X176_DISP_MIN_BACKLIGHT) )
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


static void zgd_tft220x176_disp_update
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


#define HEXING_LCD_WRITE_CMD(v)  out_byte(lcatDispCfg[0].cmd_addr,(v))
#define HEXING_LCD_WRITE_DATA(v)  \
do \
{ \
	out_byte(lcatDispCfg[0].param_addr,(uint8)((v>>8)&0xff)); \
	out_byte(lcatDispCfg[0].param_addr,(uint8)(v&0xff)); \
}while(0)

#define HEXING_LCD_READ_DATA in_byte(lcatDispCfg[0].param_addr)
#define HEXING_LCD_DELAY(v) clk_busy_wait(v*1000)


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
      ZGD_TFT220X176_DISP_HEIGHT, //320
	  ZGD_TFT220X176_DISP_WIDTH, //240
      8,             // Bits per pixel    
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
      PEGS_TM_EBI2_ID,
      EBI2_WIDTH_8,
      HWIO_ADDR(LCD_CFG0),      // HWIO_LCD_CFG0_PHYS 
      0x00680608,
      /*(// lcd_cfg0 value   = 0x04120105, 
        (   LCD_CFG0_LCD_CS_SETUP = 0x0 )|
        (   LCD_CFG0_LCD_RECOVERY = 0x4)|
        (   LCD_CFG0_LCD_HOLD_WR = 0x1 )|
        (   LCD_CFG0_LCD_HOLD_RD = 0x2 )|                                 
        (   LCD_CFG0_LCD_WAIT_WR = 0x01 )|
        (   LCD_CFG0_LCD_WAIT_RD = 0x05 )  
      ),*/
      HWIO_ADDR(LCD_CFG1),      // HWIO_LCD_CFG1_PHYS
      0x59000000,               //0x39000000,
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
} zgd_tft220x176_state_type;

/*============================================================================

                           VARIABLE DECLARATION

============================================================================*/

static rex_crit_sect_type        zgd_tft220x176_crit_sect = {0};
static disp_info_type            zgd_tft220x176_disp_info = {0};
static zgd_tft220x176_state_type zgd_tft220x176_state = {0};

/*===========================================================================

                        LOCAL FUNCTIONS PROTOTYPES

============================================================================*/

int zgd_tft220x176_install(char *str);

/*===========================================================================

                            FUNCTION PROTOTYPES

===========================================================================*/
/*===========================================================================

FUNCTION      zgd_tft220x176_HW_INIT

DESCRIPTION
  Initialize MSM for TMD display

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  MSM initialized for TMD LCD operation

===========================================================================*/
static void zgd_tft220x176_hw_init(void)
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

} /* zgd_tft220x176_hw_init() */


/*===========================================================================

FUNCTION      zgd_tft220x176_SET_SCREEN_AREA

DESCRIPTION
  Set up the LCD for a section of the screen to start displaying data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void zgd_tft220x176_set_screen_area(uint32 start_row, uint32 start_col,
                                           uint32 end_row, uint32 end_col)
{
	int nDisp  = LCD_PRIMARY; /*TODO - Should come as a parameter */

	if ( nDisp > PEGS_TM_MAX_DISPLAYS )
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
		HEXING_LCD_WRITE_CMD(0x36);
		HEXING_LCD_WRITE_DATA((uint16)end_row);
		HEXING_LCD_WRITE_CMD(0x37);
		HEXING_LCD_WRITE_DATA((uint16)start_row);

		/* Set LCD hardware to set start address */
		/* Transfer command to display hardware */
		HEXING_LCD_WRITE_CMD(0x38); 
		HEXING_LCD_WRITE_DATA((uint16)end_col);
		HEXING_LCD_WRITE_CMD(0x39);
		HEXING_LCD_WRITE_DATA((uint16)start_col);

		HEXING_LCD_WRITE_CMD(0x20);
		HEXING_LCD_WRITE_DATA((uint16)start_row);
		HEXING_LCD_WRITE_CMD(0x21);
		HEXING_LCD_WRITE_DATA((uint16)start_col);
	}
} /* zgd_tft220x176_set_screen_area() */

/*===========================================================================

FUNCTION      zgd_tft220x176_DISP_SET_BACKLIGHT

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
static void zgd_tft220x176_disp_set_backlight(byte level)
{
	static uint32 current_level =  ZGD_TFT220X176_DISP_MIN_BACKLIGHT;
  
	if(zgd_tft220x176_state.disp_initialized &&
		zgd_tft220x176_state.disp_powered_up  &&
		zgd_tft220x176_state.display_on)
	{
		if (level!= current_level)
		{
			rex_enter_crit_sect(&zgd_tft220x176_crit_sect);

			/* Bound given intensity */
			if (level > ZGD_TFT220X176_DISP_MAX_BACKLIGHT )
			{
				level = ZGD_TFT220X176_DISP_MAX_BACKLIGHT;
			}
			else if (ZGD_TFT220X176_DISP_MIN_BACKLIGHT - level > 0)
			{
				/* Turn backlight off */
				level = ZGD_TFT220X176_DISP_MIN_BACKLIGHT;
			}
            
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

    			pefRet = pm_set_led_intensity(PM_LCD_LED, (uint8)level);
    			if (PM_ERR_FLAG__SUCCESS != pefRet)
    			{
    				MSG_MED("PMIC LCD backlight set failed: pefRet=%d, nLevel=%d",
    				pefRet,
    				level,
    				0);
    			}
			}
			current_level = level;   
			rex_leave_crit_sect(&zgd_tft220x176_crit_sect);
		}
	}
} /* zgd_tft220x176_disp_set_backlight */

/*===========================================================================

FUNCTION      zgd_tft220x176_DISP_CLEAR_SCREEN_AREA

DESCRIPTION
  This function clears an area of the screen.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void zgd_tft220x176_disp_clear_screen_area(word start_row, word start_col,
                                                  word end_row, word end_col)
{
  	uint32 i = (end_row - start_row + 1) * (end_col - start_col + 1);
  	static uint32 whitebpp = 0xf100;

	if (zgd_tft220x176_state.disp_initialized &&
		zgd_tft220x176_state.disp_powered_up  &&
		zgd_tft220x176_state.display_on)
	{
		rex_enter_crit_sect(&zgd_tft220x176_crit_sect);

		zgd_tft220x176_set_screen_area(start_row, start_col, end_row, end_col);
		/* Transfer command to display hardware */
		HEXING_LCD_WRITE_CMD(ZGD_TFT220X176_RAM_WRITE_C);  
		while(i--)
		{
			HEXING_LCD_WRITE_DATA(whitebpp);
		}

		rex_leave_crit_sect(&zgd_tft220x176_crit_sect);
	}
} /* zgd_tft220x176_disp_clear_screen_area() */


/*===========================================================================

FUNCTION      zgd_tft220x176_DISP_CLEAR_WHOLE_SCREEN

DESCRIPTION
  This function clears the whole screen.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void zgd_tft220x176_disp_clear_whole_screen(void)
{
	zgd_tft220x176_disp_clear_screen_area(0, 
	                                    0,
	                                    zgd_tft220x176_disp_info.disp_height-1,
	                                    zgd_tft220x176_disp_info.disp_width-1);
} /* zgd_tft220x176_disp_clear_whole_screen() */


/*===========================================================================

FUNCTION      zgd_tft220x176_DISP_OFF

DESCRIPTION
  This function turns off the display.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void zgd_tft220x176_disp_off(void)
{
	rex_enter_crit_sect(&zgd_tft220x176_crit_sect);

	if(zgd_tft220x176_state.disp_powered_up && zgd_tft220x176_state.display_on)
	{
        HEXING_LCD_WRITE_CMD(0x07);
		HEXING_LCD_WRITE_DATA(0x0000);
		HEXING_LCD_DELAY(50);

		HEXING_LCD_WRITE_CMD(0x10);
		HEXING_LCD_WRITE_DATA(0x0A01); // // Enter Standby mode
        HEXING_LCD_DELAY(10);
		zgd_tft220x176_disp_set_backlight(ZGD_TFT220X176_DISP_MIN_BACKLIGHT);
		/* Display put to SLEEP state */
		zgd_tft220x176_state.display_on = FALSE;
	}

	rex_leave_crit_sect(&zgd_tft220x176_crit_sect);
} /* zgd_tft220x176_disp_off() */


/*===========================================================================

FUNCTION      zgd_tft220x176_DISP_ON

DESCRIPTION
  This function turns on the display.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void zgd_tft220x176_disp_on(void)
{
	rex_enter_crit_sect(&zgd_tft220x176_crit_sect);

	if(zgd_tft220x176_state.disp_powered_up && !zgd_tft220x176_state.display_on)
	{
        HEXING_LCD_WRITE_CMD(0x10);
		HEXING_LCD_WRITE_DATA(0xa000);
		HEXING_LCD_DELAY(50);

		HEXING_LCD_WRITE_CMD(0x07);
		HEXING_LCD_WRITE_DATA(0x1017); //Exit Sleep   
        HEXING_LCD_DELAY(10);
        
		/* Display put to ACTIVE state */
		zgd_tft220x176_state.display_on = TRUE;
	}

	rex_leave_crit_sect(&zgd_tft220x176_crit_sect);
} /* zgd_tft220x176_disp_on() */


/*===========================================================================

FUNCTION      zgd_tft220x176_DISP_POWERUP

DESCRIPTION
  This function powers up the device.

DEPENDENCIES
  None

RETURN VALUE
  Display buffer address

SIDE EFFECTS
  None

===========================================================================*/
static void zgd_tft220x176_disp_powerup(void)
{
	rex_enter_crit_sect(&zgd_tft220x176_crit_sect);
    if (!zgd_tft220x176_state.disp_powered_up && !zgd_tft220x176_state.display_on)
    {
    	HEXING_LCD_WRITE_CMD(0x28); HEXING_LCD_WRITE_DATA(0x00ce);
    	HEXING_LCD_DELAY(200); // Delay 150ms
    	HEXING_LCD_WRITE_CMD(0x01); HEXING_LCD_WRITE_DATA(0x031C); // set SS and NL bit
		HEXING_LCD_WRITE_CMD(0x02); HEXING_LCD_WRITE_DATA(0x0100); // set 1 line inversion
		HEXING_LCD_WRITE_CMD(0x03); HEXING_LCD_WRITE_DATA(0x1038); // set GRAM write direction and BGR=1.
		HEXING_LCD_WRITE_CMD(0x08); HEXING_LCD_WRITE_DATA(0x0808); // set BP and FP
		HEXING_LCD_WRITE_CMD(0x0C); HEXING_LCD_WRITE_DATA(0x0000); // RGB interface setting R0Ch=0x0110 for RGB 18Bit and R0Ch=0111 for RGB16Bit
		HEXING_LCD_WRITE_CMD(0x0F); HEXING_LCD_WRITE_DATA(0x0B01); // Set frame rate
		HEXING_LCD_WRITE_CMD(0x20); HEXING_LCD_WRITE_DATA(0x0000); // Set GRAM Address
		HEXING_LCD_WRITE_CMD(0x21); HEXING_LCD_WRITE_DATA(0x0000); // Set GRAM Address
		//-*************Power On sequence ****************-//
		HEXING_LCD_DELAY(50); // Delay 50ms
		HEXING_LCD_WRITE_CMD(0x10); HEXING_LCD_WRITE_DATA(0x0800); // Set SAP,DSTB,STB
		HEXING_LCD_WRITE_CMD(0x11); HEXING_LCD_WRITE_DATA(0x1038); // Set APON,PON,AON,VCI1EN,VC
		HEXING_LCD_DELAY(50); // Delay 50ms
		HEXING_LCD_WRITE_CMD(0x12); HEXING_LCD_WRITE_DATA(0x1121); // Internal reference voltage= Vci;
		HEXING_LCD_WRITE_CMD(0x13); HEXING_LCD_WRITE_DATA(0x0066); // Set GVDD
		HEXING_LCD_WRITE_CMD(0x14); HEXING_LCD_WRITE_DATA(0x333A); // Set VCOMH/VCOML voltage   0x3336
		//------------------------ Set GRAM area --------------------------------//
		HEXING_LCD_WRITE_CMD(0x30); HEXING_LCD_WRITE_DATA(0x0000);
		HEXING_LCD_WRITE_CMD(0x31); HEXING_LCD_WRITE_DATA(0x00DB);
		HEXING_LCD_WRITE_CMD(0x32); HEXING_LCD_WRITE_DATA(0x0000);
		HEXING_LCD_WRITE_CMD(0x33); HEXING_LCD_WRITE_DATA(0x0000);
		HEXING_LCD_WRITE_CMD(0x34); HEXING_LCD_WRITE_DATA(0x00DB);
		HEXING_LCD_WRITE_CMD(0x35); HEXING_LCD_WRITE_DATA(0x0000);
		HEXING_LCD_WRITE_CMD(0x36); HEXING_LCD_WRITE_DATA(0x00AF);
		HEXING_LCD_WRITE_CMD(0x37); HEXING_LCD_WRITE_DATA(0x0000);
		HEXING_LCD_WRITE_CMD(0x38); HEXING_LCD_WRITE_DATA(0x00DB);
		HEXING_LCD_WRITE_CMD(0x39); HEXING_LCD_WRITE_DATA(0x0000);
		// ----------- Adjust the Gamma Curve ----------//
		HEXING_LCD_WRITE_CMD(0x50); HEXING_LCD_WRITE_DATA(0x0000);
		HEXING_LCD_WRITE_CMD(0x51); HEXING_LCD_WRITE_DATA(0x0102);
		HEXING_LCD_WRITE_CMD(0x52); HEXING_LCD_WRITE_DATA(0x0901);
		HEXING_LCD_WRITE_CMD(0x53); HEXING_LCD_WRITE_DATA(0x0700);
		HEXING_LCD_WRITE_CMD(0x54); HEXING_LCD_WRITE_DATA(0x0109);
		HEXING_LCD_WRITE_CMD(0x55); HEXING_LCD_WRITE_DATA(0x0201);
		HEXING_LCD_WRITE_CMD(0x56); HEXING_LCD_WRITE_DATA(0x0000);
		HEXING_LCD_WRITE_CMD(0x57); HEXING_LCD_WRITE_DATA(0x0007);
		HEXING_LCD_WRITE_CMD(0x58); HEXING_LCD_WRITE_DATA(0x1F00);
		HEXING_LCD_WRITE_CMD(0x59); HEXING_LCD_WRITE_DATA(0x001F);
		HEXING_LCD_DELAY(50); // Delay 50ms
		HEXING_LCD_WRITE_CMD(0x07); HEXING_LCD_WRITE_DATA(0x1017);
        zgd_tft220x176_state.disp_powered_up = TRUE;
   }
   
   rex_leave_crit_sect(&zgd_tft220x176_crit_sect);
}

/*===========================================================================

FUNCTION      zgd_tft220x176_DISP_POWERDOWN

DESCRIPTION
  This function powers down the device.

DEPENDENCIES
  None

RETURN VALUE
  Return True if success, False otherwise

SIDE EFFECTS
  None

===========================================================================*/
static int zgd_tft220x176_disp_powerdown(void)
{
	rex_enter_crit_sect(&zgd_tft220x176_crit_sect);

	if (zgd_tft220x176_state.disp_powered_up && !zgd_tft220x176_state.display_on) 
	{
		/* 
		* Power-down the controller 
		*/
		zgd_tft220x176_state.disp_powered_up = FALSE;
	}

	rex_leave_crit_sect(&zgd_tft220x176_crit_sect);

	return FALSE;
} /* zgd_tft220x176_disp_powerdown() */


/*===========================================================================

FUNCTION      zgd_tft220x176_DISP_INIT

DESCRIPTION
  This function initialize the Pegs Toshiba display panel

DEPENDENCIES
  Assumes that this function will only be called once at boot up

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
int zgd_tft220x176_disp_init(void)
{
	if (zgd_tft220x176_state.disp_initialized) 
	{
		/* Do not re-initialize the display */
		return 1;
	}

	zgd_tft220x176_disp_info.disp_width         = ZGD_TFT220X176_DISP_HEIGHT;
	zgd_tft220x176_disp_info.disp_height        = ZGD_TFT220X176_DISP_WIDTH;
	zgd_tft220x176_disp_info.bpp                = DISP_16BPP;
	zgd_tft220x176_disp_info.palette_support    = FALSE;
	zgd_tft220x176_disp_info.contrast_support   = FALSE;
	zgd_tft220x176_disp_info.contrast_min       = ZGD_TFT220X176_DISP_MIN_CONTRAST;
	zgd_tft220x176_disp_info.contrast_max       = ZGD_TFT220X176_DISP_MAX_CONTRAST;
	zgd_tft220x176_disp_info.contrast_default   = ZGD_TFT220X176_DISP_DEFAULT_CONTRAST;
	zgd_tft220x176_disp_info.backlight_support  = TRUE;
	zgd_tft220x176_disp_info.backlight_min      = ZGD_TFT220X176_DISP_MIN_BACKLIGHT;
	zgd_tft220x176_disp_info.backlight_max      = ZGD_TFT220X176_DISP_MAX_BACKLIGHT;
	zgd_tft220x176_disp_info.backlight_default  = ZGD_TFT220X176_DISP_DEFAULT_BACKLIGHT;
  	zgd_tft220x176_disp_info.lcd_type           = ZGD_TFT220X176;
  	zgd_tft220x176_disp_info.phys_width         = ZGD_TFT220X176_DISP_WIDTH;    
  	zgd_tft220x176_disp_info.phys_height        = ZGD_TFT220X176_DISP_HEIGHT; 

	rex_init_crit_sect(&zgd_tft220x176_crit_sect);

	zgd_tft220x176_hw_init();

	zgd_tft220x176_disp_powerup();

	zgd_tft220x176_disp_on();

	zgd_tft220x176_state.disp_initialized = TRUE;

	zgd_tft220x176_disp_clear_whole_screen();
    
	HEXING_LCD_DELAY(100);
	zgd_tft220x176_disp_set_backlight(ZGD_TFT220X176_DISP_DEFAULT_BACKLIGHT);

	return 1;
} /* zgd_tft220x176_disp_init() */


/*===========================================================================

FUNCTION      zgd_tft220x176_DISP_GET_INFO

DESCRIPTION
  This function returns display information

DEPENDENCIES
  None

RETURN VALUE
  Display information

SIDE EFFECTS
  None

===========================================================================*/
static disp_info_type zgd_tft220x176_disp_get_info(void)
{
	return zgd_tft220x176_disp_info;
} /* zgd_tft220x176_disp_get_info() */


/*===========================================================================

FUNCTION      zgd_tft220x176_DISP_COPY

DESCRIPTION
  Copy image data from a buffer to the LCD display

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  LCD is updated

===========================================================================*/
static void zgd_tft220x176_disp_copy(void *src_ptr, dword copy_count)
{  
	int i;
	uint16 *pdata = src_ptr;
    uint16 data;
//	uint8 data1,data2;
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
        for ( i = 0; i < copy_count; i++ )
        {
            data = (uint16)(*pdata);
			HEXING_LCD_WRITE_DATA(data);
            pdata++;
        }        
    }
    else 
    {
      	MSG_ERROR(" Illegal Src Ptr assigned",0,0,0);
    }
    
} /* zgd_tft220x176_disp_copy() */


/*===========================================================================

FUNCTION      zgd_tft220x176_DISP_UPDATE

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
static void zgd_tft220x176_disp_update
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
		!zgd_tft220x176_state.disp_initialized ||
		!zgd_tft220x176_state.disp_powered_up  ||
		!zgd_tft220x176_state.display_on)
	{
		return;
	}

	if ((dst_starting_row + num_of_rows) > zgd_tft220x176_disp_info.disp_height)
	{
		num_of_rows = zgd_tft220x176_disp_info.disp_height - dst_starting_row;
	}
	
	if ((dst_starting_column + num_of_columns) > zgd_tft220x176_disp_info.disp_width)
	{
		num_of_columns = zgd_tft220x176_disp_info.disp_width - dst_starting_column;
	}

	/* Ensure buffer aligned and parameters valid */
	if((src_starting_row    < zgd_tft220x176_disp_info.disp_height) &&
		(src_starting_column < zgd_tft220x176_disp_info.disp_width)  &&
		(dst_starting_row    < zgd_tft220x176_disp_info.disp_height) &&
		(dst_starting_column < zgd_tft220x176_disp_info.disp_width)  &&
		(!(((uint32)buf_ptr) & 0x3)))
	{
		rex_enter_crit_sect(&zgd_tft220x176_crit_sect);    

		zgd_tft220x176_set_screen_area(dst_starting_row, dst_starting_column, 
	                       				dst_starting_row + num_of_rows - 1,
	                       				dst_starting_column + num_of_columns -1);

		src_ptr = (uint16*)buf_ptr;
		src_ptr += src_starting_row * src_width + src_starting_column;

		/* Transfer command to display hardware */
		HEXING_LCD_WRITE_CMD(ZGD_TFT220X176_RAM_WRITE_C); 

		if((src_starting_column == 0) && (num_of_columns == src_width)) 
		{
			uint32 copy_count;

			/* The whole row is updated, copy the whole update area */
			copy_count = num_of_rows * num_of_columns;

			zgd_tft220x176_disp_copy(src_ptr, copy_count);
		} 
		else 
		{
			/* Partial row is updated, need to copy one row at a time */
			int16 row;
			uint32 copy_count;

			copy_count = num_of_columns;
			for (row = 0; row < num_of_rows; row++)
			{
				zgd_tft220x176_disp_copy(src_ptr, copy_count);
				src_ptr += src_width;
			}
		}
		
		rex_leave_crit_sect(&zgd_tft220x176_crit_sect);
	}  
} /* zgd_tft220x176_disp_update() */


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
static void zgd_tft220x176_disp_set_contrast(word contrast)
{
	return;
} /* zgd_tft220x176_disp_set_contrast() */


/*===========================================================================

FUNCTION      zgd_tft220x176_DISP_IOCTL

DESCRIPTION
  This function installs LCD device driver

DEPENDENCIES
  None

RETURN VALUE
  >0 if success, <0 if failure

SIDE EFFECTS
  None

===========================================================================*/
static int zgd_tft220x176_disp_ioctl ( int cmd, void *arg )
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
			zgd_tft220x176_disp_update(disp_update_cmd->buf_ptr,
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
		*(disp_info_type *)arg = zgd_tft220x176_disp_get_info();
		break;

	case IOCTL_DISP_OFF:
		zgd_tft220x176_disp_off();
		break;

	case IOCTL_DISP_ON:
		zgd_tft220x176_disp_on();
		break;

	case IOCTL_DISP_POWERUP:
		zgd_tft220x176_disp_powerup();
		break;

	case IOCTL_DISP_SET_CONTRAST:
		zgd_tft220x176_disp_set_contrast(*(word *)arg);
		break;

	case IOCTL_DISP_SET_BACKLIGHT:
		zgd_tft220x176_disp_set_backlight(*(byte *)arg);
		break;

	case IOCTL_DISP_CLEAR_WHOLE_SCREEN:
		zgd_tft220x176_disp_clear_whole_screen();
		break;

	case IOCTL_DISP_CLEAR_SCREEN_AREA:
		zgd_tft220x176_disp_clear_screen_area(((disp_cls_type *)arg)->start_row,
		                                      ((disp_cls_type *)arg)->start_column,
		                                      ((disp_cls_type *)arg)->end_row,                                          
		                                      ((disp_cls_type *)arg)->end_column);
		break;
	default: 
		return -1;
	}
	
	return 1;
} /* zgd_tft220x176_disp_ioctl() */


/*===========================================================================

FUNCTION      ZGD_TFT220X176_INSTALL

DESCRIPTION
  This function installs LCD device driver

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

========================================================================*/
int zgd_tft220x176_install(char * str)
{
	return drv_install(str,
	                 NULL,
	                 NULL,
	                 zgd_tft220x176_disp_ioctl,
	                 zgd_tft220x176_disp_init,
	                 zgd_tft220x176_disp_powerdown);
} /* zgd_tft220x176_install() */

