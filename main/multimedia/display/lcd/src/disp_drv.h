#ifndef __DISP_DRV_H__
#define __DISP_DRV_H__
#include "customer.h"
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


#define LCD_WRITD_CMDW(v)     out_word(LCD_CMD_WH,(v))
#define LCD_WRITE_DATAW(v)    out_word(LCD_DATA_WH,(v))
#define LCD_READ_DATAW()      in_word(LCD_DATA_WH)



/*============================================================================

                        STRUCTURE TYPES FOR MODULE

============================================================================*/
typedef struct{
    boolean disp_w_h_swap;
    void   (*disp_ic_init)(void);
    void   (*disp_ic_setwindow)(uint32 start_row, uint32 start_col, uint32 end_row, uint32 end_col);
    void   (*disp_ic_bitblt)(const void *src_ptr, dword copy_count);
    void   (*disp_ic_set)(uint32 src, dword copy_count);
    void   (*disp_ic_sleep)(boolean bin);
    void   (*disp_ic_rot)(uint16 degree);
#ifdef FEATURE_MDP
    uint8  (*disp_ic_mdp_getformat)(void);
    uint16 (*disp_ic_mdp_getscr)(uint32 **ppscr);
    void   (*disp_ic_mdp_scrupdate)(uint32 *scr, uint32 start_row, uint32 start_col, uint32 end_row, uint32 end_col);
#endif
#if (defined(FEATURE_MP4_DECODER) || defined(FEATURE_CAMERA_NOFULLSCREEN)) && !defined(T_QSC1110)
    void   (*disp_ic_yuv420)(const byte *src_ptr, word src_w, word src_h, word dst_w, word dst_h);
#endif
}disp_drv_ic_type;

typedef boolean (*disp_ic_install_type)(disp_drv_ic_type *pdispic);

typedef struct 
{
    boolean     disp_initialized;
    boolean     display_on;
    boolean     disp_powered_up;
#if (defined(FEATURE_MP4_DECODER) || defined(FEATURE_CAMERA_NOFULLSCREEN)) && !defined(T_QSC1110)
    word        lock_row_start;
    word        lock_row_num;
    word        lock_col_start;
    word        lock_col_num;
#endif
} disp_drv_state_type;

#ifdef FEATURE_MDP
#define DISP_IC_INIT_TBL(p) \
    p->disp_w_h_swap        = FALSE; \
    p->disp_ic_init         = disp_ic_init; \
    p->disp_ic_setwindow    = disp_ic_setwindow; \
    p->disp_ic_bitblt       = disp_ic_bitblt; \
    p->disp_ic_set          = disp_ic_set; \
    p->disp_ic_sleep        = disp_ic_sleep; \
    p->disp_ic_rot          = disp_ic_rot; \
    p->disp_ic_mdp_getformat= disp_ic_mdp_getformat; \
    p->disp_ic_mdp_getscr   = disp_ic_mdp_getscr; \
    p->disp_ic_mdp_scrupdate= disp_ic_mdp_scrupdate
#elif (defined(FEATURE_MP4_DECODER) || defined(FEATURE_CAMERA_NOFULLSCREEN)) && !defined(T_QSC1110)
#define DISP_IC_INIT_TBL(p) \
    p->disp_w_h_swap        = FALSE; \
    p->disp_ic_init         = disp_ic_init; \
    p->disp_ic_setwindow    = disp_ic_setwindow; \
    p->disp_ic_bitblt       = disp_ic_bitblt; \
    p->disp_ic_set          = disp_ic_set; \
    p->disp_ic_sleep        = disp_ic_sleep; \
    p->disp_ic_rot          = disp_ic_rot; \
    p->disp_ic_yuv420       = disp_ic_yuv420
#else
#define DISP_IC_INIT_TBL(p) \
    p->disp_w_h_swap        = FALSE; \
    p->disp_ic_init         = disp_ic_init; \
    p->disp_ic_setwindow    = disp_ic_setwindow; \
    p->disp_ic_bitblt       = disp_ic_bitblt; \
    p->disp_ic_set          = disp_ic_set; \
    p->disp_ic_sleep        = disp_ic_sleep; \
    p->disp_ic_rot          = disp_ic_rot
#endif
#endif
