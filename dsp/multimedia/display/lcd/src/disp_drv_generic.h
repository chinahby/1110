/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   L C D    D I S P L A Y    D R I V E R

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS
  disp_drv_init

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*============================================================================

                           VARIABLE DECLARATION

============================================================================*/

static rex_crit_sect_type       disp_drv_crit_sect = {0};
static disp_info_type           disp_drv_info = {0};
static disp_drv_state_type      disp_drv_state = {0};
static disp_drv_ic_type         disp_drv_ic = {0};

/*===========================================================================

                            FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

FUNCTION      disp_drv_hw_init

DESCRIPTION
  Initialize MSM for TMD display

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  MSM initialized for TMD LCD operation

===========================================================================*/
static void disp_drv_hw_init(void)
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
    out_dword(HWIO_ADDR(LCD_CFG0), 0x00480408);
      /*(// lcd_cfg0 value   = 0x04120105, 
        (   LCD_CFG0_LCD_CS_SETUP = 0x0 )|
        (   LCD_CFG0_LCD_RECOVERY = 0x4)|
        (   LCD_CFG0_LCD_HOLD_WR = 0x1 )|
        (   LCD_CFG0_LCD_HOLD_RD = 0x2 )|                                 
        (   LCD_CFG0_LCD_WAIT_WR = 0x01 )|
        (   LCD_CFG0_LCD_WAIT_RD = 0x05 )  
      ),*/
    out_dword(HWIO_ADDR(LCD_CFG1), 0x59000000);
      /*(                         // lcd_cfg1 value   = 0x39000000,
        (   0x1<<LCD_CFG1__LCD_D_ON_UB_LB___S       )|
        (   0x1<<LCD_CFG1__LCD_18_OR_24_BIT_ENA___S )|
        (   0x1<<LCD_CFG1__LCD_RS_ENA___S           )|
        (   0x1<<LCD_CFG1__LCD_RS_MODE___S          )
      ), */           
} /* disp_drv_hw_init() */

/*===========================================================================

FUNCTION      disp_drv_SET_BACKLIGHT

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
static void disp_drv_set_backlight(byte level)
{
    static uint32 current_level =  DISP_DRV_MIN_BACKLIGHT;
  
    if(disp_drv_state.disp_initialized &&
        disp_drv_state.disp_powered_up  &&
        disp_drv_state.display_on)
    {
        if (level!= current_level)
        {
            rex_enter_crit_sect(&disp_drv_crit_sect);

            /* Bound given intensity */
            if (level > DISP_DRV_MAX_BACKLIGHT )
            {
                level = DISP_DRV_MAX_BACKLIGHT;
            }
            else if (DISP_DRV_MIN_BACKLIGHT - level > 0)
            {
                /* Turn backlight off */
                level = DISP_DRV_MIN_BACKLIGHT;
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
            rex_leave_crit_sect(&disp_drv_crit_sect);
        }
    }
} /* disp_drv_set_backlight */

/*===========================================================================

FUNCTION      disp_drv_CLEAR_SCREEN_AREA

DESCRIPTION
  This function clears an area of the screen.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void disp_drv_clear_screen_area(word start_row, word start_col,
                                                  word end_row, word end_col)
{
    uint32 count = (end_row - start_row + 1) * (end_col - start_col + 1);
    static uint32 whitebpp = 0x00;
    
    if (disp_drv_state.disp_initialized &&
        disp_drv_state.disp_powered_up  &&
        disp_drv_state.display_on)
    {
        rex_enter_crit_sect(&disp_drv_crit_sect);
        disp_drv_ic.disp_ic_setwindow(start_row, start_col, end_row, end_col);
        disp_drv_ic.disp_ic_set(whitebpp, count);
        rex_leave_crit_sect(&disp_drv_crit_sect);
    }
} /* disp_drv_clear_screen_area() */


/*===========================================================================

FUNCTION      disp_drv_CLEAR_WHOLE_SCREEN

DESCRIPTION
  This function clears the whole screen.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void disp_drv_clear_whole_screen(void)
{
    disp_drv_clear_screen_area( 0, 
                                0,
                                disp_drv_info.disp_height-1,
                                disp_drv_info.disp_width-1);
} /* disp_drv_clear_whole_screen() */


/*===========================================================================

FUNCTION      disp_drv_OFF

DESCRIPTION
  This function turns off the display.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void disp_drv_off(void)
{
    rex_enter_crit_sect(&disp_drv_crit_sect);

    if(disp_drv_state.disp_powered_up && disp_drv_state.display_on)
    {
        disp_drv_set_backlight(DISP_DRV_MIN_BACKLIGHT);
        disp_drv_ic.disp_ic_sleep(TRUE);
        /* Display put to SLEEP state */
        disp_drv_state.display_on = FALSE;
    }

    rex_leave_crit_sect(&disp_drv_crit_sect);
} /* disp_drv_off() */


/*===========================================================================

FUNCTION      disp_drv_ON

DESCRIPTION
  This function turns on the display.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void disp_drv_on(void)
{
    rex_enter_crit_sect(&disp_drv_crit_sect);

    if(disp_drv_state.disp_powered_up && !disp_drv_state.display_on)
    {
        disp_drv_ic.disp_ic_sleep(FALSE);
        /* Display put to ACTIVE state */
        disp_drv_state.display_on = TRUE;
    }
    
    rex_leave_crit_sect(&disp_drv_crit_sect);
} /* disp_drv_on() */


/*===========================================================================

FUNCTION      disp_drv_POWERUP

DESCRIPTION
  This function powers up the device.

DEPENDENCIES
  None

RETURN VALUE
  Display buffer address

SIDE EFFECTS
  None

===========================================================================*/
#ifdef CUST_EDITION
static void disp_drv_powerup(void)
{
    rex_enter_crit_sect(&disp_drv_crit_sect);
    if (!disp_drv_state.disp_powered_up && !disp_drv_state.display_on)
    {
        disp_drv_ic.disp_ic_init();
        disp_drv_state.disp_powered_up = TRUE;
    }
   
    rex_leave_crit_sect(&disp_drv_crit_sect);
}
#endif

/*===========================================================================

FUNCTION      disp_drv_POWERDOWN

DESCRIPTION
  This function powers down the device.

DEPENDENCIES
  None

RETURN VALUE
  Return True if success, False otherwise

SIDE EFFECTS
  None

===========================================================================*/
static int disp_drv_powerdown(void)
{
    rex_enter_crit_sect(&disp_drv_crit_sect);

    if (disp_drv_state.disp_powered_up && !disp_drv_state.display_on) 
    {
        /* 
        * Power-down the controller 
        */
        disp_drv_state.disp_powered_up = FALSE;
    }

    rex_leave_crit_sect(&disp_drv_crit_sect);

    return 0;
} /* disp_drv_powerdown() */


/*===========================================================================

FUNCTION      disp_drv_init

DESCRIPTION
  This function initialize the Pegs Toshiba display panel

DEPENDENCIES
  Assumes that this function will only be called once at boot up

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
int disp_drv_init(void)
{
    int i;
    
    if (disp_drv_state.disp_initialized) 
    {
        /* Do not re-initialize the display */
        return 1;
    }

    disp_drv_info.disp_width         = DISP_DRV_WIDTH;
    disp_drv_info.disp_height        = DISP_DRV_HEIGHT;
    disp_drv_info.bpp                = DISP_16BPP;
    disp_drv_info.palette_support    = FALSE;
    disp_drv_info.contrast_support   = FALSE;
    disp_drv_info.contrast_min       = DISP_DRV_MIN_CONTRAST;
    disp_drv_info.contrast_max       = DISP_DRV_MAX_CONTRAST;
    disp_drv_info.contrast_default   = DISP_DRV_DEFAULT_CONTRAST;
    disp_drv_info.backlight_support  = TRUE;
    disp_drv_info.backlight_min      = DISP_DRV_MIN_BACKLIGHT;
    disp_drv_info.backlight_max      = DISP_DRV_MAX_BACKLIGHT;
    disp_drv_info.backlight_default  = DISP_DRV_DEFAULT_BACKLIGHT;
    disp_drv_info.lcd_type           = ZGD_TFT128X160;
    disp_drv_info.phys_width         = DISP_DRV_WIDTH;    
    disp_drv_info.phys_height        = DISP_DRV_HEIGHT; 

    rex_init_crit_sect(&disp_drv_crit_sect);

    disp_drv_hw_init();
    
    i = 0;
    do
    {
        if(disp_ic_install[i] == NULL)
        {
            return 0;
        }
        
        if(disp_ic_install[i](&disp_drv_ic))
        {
            break;
        }
    }while(1);
    
    disp_drv_powerup();
    
    disp_drv_on();

    disp_drv_state.disp_initialized = TRUE;
    
    disp_drv_clear_whole_screen();
    return 1;
} /* disp_drv_init() */


/*===========================================================================

FUNCTION      disp_drv_get_info

DESCRIPTION
  This function returns display information

DEPENDENCIES
  None

RETURN VALUE
  Display information

SIDE EFFECTS
  None

===========================================================================*/
static disp_info_type disp_drv_get_info(void)
{
    return disp_drv_info;
} /* disp_drv_get_info() */

/*===========================================================================

FUNCTION      disp_drv_update

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
static void disp_drv_update
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
        !disp_drv_state.disp_initialized ||
        !disp_drv_state.disp_powered_up  ||
        !disp_drv_state.display_on)
    {
        return;
    }

    if ((dst_starting_row + num_of_rows) > disp_drv_info.disp_height)
    {
        num_of_rows = disp_drv_info.disp_height - dst_starting_row;
    }
    
    if ((dst_starting_column + num_of_columns) > disp_drv_info.disp_width)
    {
        num_of_columns = disp_drv_info.disp_width - dst_starting_column;
    }

    /* Ensure buffer aligned and parameters valid */
    if((src_starting_row    < disp_drv_info.disp_height) &&
        (src_starting_column < disp_drv_info.disp_width)  &&
        (dst_starting_row    < disp_drv_info.disp_height) &&
        (dst_starting_column < disp_drv_info.disp_width)  &&
        (!(((uint32)buf_ptr) & 0x3)))
    {
        rex_enter_crit_sect(&disp_drv_crit_sect);    
        
        disp_drv_ic.disp_ic_setwindow(dst_starting_row, dst_starting_column, 
                                      dst_starting_row + num_of_rows - 1,
                                      dst_starting_column + num_of_columns -1);
                                      
        src_ptr = (uint16*)buf_ptr;
        src_ptr += src_starting_row * src_width + src_starting_column;

        if((src_starting_column == 0) && (num_of_columns == src_width)) 
        {
            uint32 copy_count;

            /* The whole row is updated, copy the whole update area */
            copy_count = num_of_rows * num_of_columns;
            
            disp_drv_ic.disp_ic_bitblt(src_ptr, copy_count);
        } 
        else 
        {
            /* Partial row is updated, need to copy one row at a time */
            int16 row;
            uint32 copy_count;

            copy_count = num_of_columns;
            for (row = 0; row < num_of_rows; row++)
            {
                disp_drv_ic.disp_ic_bitblt(src_ptr, copy_count);
                src_ptr += src_width;
            }
        }
        
        rex_leave_crit_sect(&disp_drv_crit_sect);
    }  
} /* disp_drv_update() */


/*===========================================================================

FUNCTION      disp_drv_set_contrast

DESCRIPTION
  This function sets the contrast of the display device.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void disp_drv_set_contrast(word contrast)
{
    return;
} /* disp_drv_set_contrast() */


/*===========================================================================

FUNCTION      disp_drv_ioctl

DESCRIPTION
  This function installs LCD device driver

DEPENDENCIES
  None

RETURN VALUE
  >0 if success, <0 if failure

SIDE EFFECTS
  None

===========================================================================*/
static int disp_drv_ioctl ( int cmd, void *arg )
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
            disp_drv_update(disp_update_cmd->buf_ptr,
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
        *(disp_info_type *)arg = disp_drv_get_info();
        break;

    case IOCTL_DISP_OFF:
        disp_drv_off();
        break;

    case IOCTL_DISP_ON:
        disp_drv_on();
        break;

    case IOCTL_DISP_POWERUP:
        disp_drv_powerup();
        break;

    case IOCTL_DISP_SET_CONTRAST:
        disp_drv_set_contrast(*(word *)arg);
        break;

    case IOCTL_DISP_SET_BACKLIGHT:
        disp_drv_set_backlight(*(byte *)arg);
        break;

    case IOCTL_DISP_CLEAR_WHOLE_SCREEN:
        disp_drv_clear_whole_screen();
        break;

    case IOCTL_DISP_CLEAR_SCREEN_AREA:
        disp_drv_clear_screen_area(((disp_cls_type *)arg)->start_row,
                                   ((disp_cls_type *)arg)->start_column,
                                   ((disp_cls_type *)arg)->end_row,                                          
                                   ((disp_cls_type *)arg)->end_column);
        break;
    default: 
        return -1;
    }
    
    return 1;
} /* disp_drv_ioctl() */


/*===========================================================================

FUNCTION      disp_drv_install

DESCRIPTION
  This function installs LCD device driver

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

========================================================================*/
int disp_drv_install(char * str)
{
    return drv_install(str,
                       NULL,
                       NULL,
                       disp_drv_ioctl,
                       disp_drv_init,
                       disp_drv_powerdown);
} /* disp_drv_install() */

