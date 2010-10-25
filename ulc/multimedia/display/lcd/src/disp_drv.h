#ifndef __DISP_DRV_H__
#define __DISP_DRV_H__

#include "target.h"
#include "disp.h"         /* Bit map LCD display driver interface */
#include "drvLib.h"
#include "clk.h"
#include "clkrgm_msm.h"
#include "gpio_1100.h"
#include "pm.h"
#include "pmapp.h"

/*============================================================================

                 CONSTANT AND MACRO DEFINITIONS FOR MODULE

============================================================================*/
#define DISP_DRV_MIN_CONTRAST      0
#define DISP_DRV_MAX_CONTRAST      255
#define DISP_DRV_DEFAULT_CONTRAST  128

#define DISP_DRV_MIN_BACKLIGHT     0 
#define DISP_DRV_MAX_BACKLIGHT     7
#define DISP_DRV_DEFAULT_BACKLIGHT 3

#define LCD_MAX_DISPLAYS 1
#define LCD_PRIMARY 0

#define LCD_CMD_WH    EBI2_LCD_BASE           // command register for primary LCD port
#define LCD_DATA_WH   EBI2_LCD_BASE+0x100000  // data register for primary LCD port

#define LCD_WRITE_CMD(v)    out_byte(LCD_CMD_WH,(v))
#define LCD_WRITE_DATA(v)   out_byte(LCD_DATA_WH,(v))
#define LCD_READ_DATA()     in_byte(LCD_DATA_WH)
#define LCD_WRITE_DATA16(v) out_byte(LCD_DATA_WH,(uint8)(v>>8)); \
                            out_byte(LCD_DATA_WH,(uint8)(v))
#define LCD_READ_DATA16()   (uint16)((in_byte(LCD_DATA_WH)<<8)|in_byte(LCD_DATA_WH))
#define LCD_DELAY(v)        clk_busy_wait(v*1000)

/*============================================================================

                        STRUCTURE TYPES FOR MODULE

============================================================================*/
typedef struct{
    void   (*disp_ic_init)(void);
    void   (*disp_ic_setwindow)(uint32 start_row, uint32 start_col, uint32 end_row, uint32 end_col);
    void   (*disp_ic_bitblt)(void *src_ptr, dword copy_count);
    void   (*disp_ic_set)(uint32 src, dword copy_count);
    void   (*disp_ic_sleep)(boolean bin);
}disp_drv_ic_type;

typedef boolean (*disp_ic_install_type)(disp_drv_ic_type *pdispic);

typedef struct 
{
	boolean     disp_initialized;
	boolean     display_on;
	boolean     disp_powered_up;
} disp_drv_state_type;
#endif
