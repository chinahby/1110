/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  DISPLAY API Driver

  GENERAL DESCRIPTION
  This module contains functions and data blocks pertaining to the
  Display Device Driver API

  EXTERNALIZED FUNCTIONS

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

  Copyright (c) 2003 - 2004 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*============================================================================

  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/display/mdp1/lcd_qsc1100/main/latest/src/disp_api_default.c#4 $ $DateTime: 2008/12/11 05:53:50 $ $Author: rkashyap $

when      who     what, where, why
--------  -----   ----------------------------------------------------------
12/11/08  rk      Compiler Warning fixes.
09/29/08  dw      Allow continuous power up/down of LCD
03/13/08  dw      Enable display hardware acceleration for Pegs
03/07/08  dw      Pegs Epson S1D19120 bring-up development
01/07/08  dw      Initial version for QSC1100 compilation
11/07/07  dw      Remove compiler warning.
11/07/07  dw      Removal of physical dimension assignment and code clean up
10/31/07  cl      Changes for LCD to work on 6290 FFA 
10/15/07  dw      Enable UI updates through MDP for Koel FFA
02/21/07  tws     Board revision detection is not present on the MBP (DTV/MFlo)
                  FFA. Force the LCD driver to use H5B settings.
01/31/07  cl      Added support for display update under error fatal
08/11/06  cf      Disable disp update by MDP when LCD is off.
03/16/05  cjr     Disp capture argument separation
07/25/05  trg     Auto detect MDDI/EBI LCD panel
03/28/05  tb      Changes to not send update after disp_off.
02/07/05  tb      Changes to support different resolutions.
11/08/04  tb      Changes to support multple MDDI clients.
08/26/04  jas     Stub out the 2ary display functions for single-disp targets
                  Pass correct args to drv_ioctl in disp_clear_screen_area
08/09/04  tb      Modifications for MC4 secondary.
07/22/04  cjr	  Added MDDI MC4 Secondary LCD support
06/07/04  ap      Fix compile issue with #elif.
05/21/04  cjr     Initial code
============================================================================ */

/*============================================================================
INCLUDE FILES FOR MODULE
============================================================================*/

#include "disp.h"
#include "drvLib.h"
#include "string.h"

#ifdef FEATURE_HW_BOARD_REV
#include "bio.h"
#include "clk.h"
#include "hwi.h"
#endif 
#include "pm.h"

#ifdef FEATURE_TORCH_SUPPORT
#include "gpio_1100.h"
#endif

#if defined(FEATURE_DSP) || defined(FEATURE_BOOT_SPLASH_SCREEN)
#include "clk.h"
#endif

#ifndef CUST_EDITION
int lcd_mddi_cur_panel = MDDI_MC4_PRIM;
#endif
int fd = -1;
int fd2 = -1;

#ifdef FEATURE_MDP
int mdp_fd = -1;
#endif

static disp_info_type lcd_info_1;
static disp_update_type dup_arg;

#ifdef FEATURE_SECONDARY_DISPLAY
#error code not present
#endif

#ifdef FEATURE_HS_DISPLAY_CAPTURE
static disp_update_type dup_arg_capture;
#endif

static disp_cls_type cls_arg;
#if (defined(FEATURE_MP4_DECODER) || defined(FEATURE_CAMERA_NOFULLSCREEN)) //&& !defined(T_QSC1110)
static disp_update_type duplock_arg;
static disp_update_type dupyuv_arg;
static disp_lock_type lock_arg;
#endif
extern boolean mdp_disp_fatal_err_flag;
#ifndef CUST_EDITION
extern int epson_S1D19120_install(char *);
#else
extern int disp_drv_install(char * str);
#endif
#ifdef FEATURE_BOOT_SPLASH_SCREEN
extern void disp_epson_S1D19120_boot_chg_splash_screen (void);
#endif
#ifdef FEATURE_MDP
extern int mdp_install(char *);
extern void mdp_set_lcd_size(LCD_TYPE dest, LCD_TYPE_PHYS physDest, uint32 width, uint32 height);
extern dword mddi_client_lcd_disp_capture
(
 void * buf_ptr,
 dword copy_count,
 int16 starting_row,
 int16 starting_column,
 int16 ending_row,
 int16 ending_column
);

static void mdp_init(void);
#endif /* FEATURE_MDP */

void disp_init(void)
{
#ifdef FEATURE_DSP
	gpio_tlmm_config(AIT701_RESET);
	gpio_tlmm_config(AIT701_BYPASS);
	gpio_tlmm_config(GPIO_OUTPUT_10);
	(void)gpio_out(AIT701_BYPASS, GPIO_HIGH_VALUE);
	(void)gpio_out(AIT701_BYPASS, GPIO_HIGH_VALUE);
	(void)gpio_out(GPIO_OUTPUT_10, GPIO_HIGH_VALUE);
	clk_busy_wait(10*1000);
#endif

#ifndef CUST_EDITION
	if(epson_S1D19120_install(PRIMARY_LCD_NAME) == -1)
		return;
#else
    if(disp_drv_install(PRIMARY_LCD_NAME) == -1)
        return;
#ifdef FEATURE_MDP
    mdp_disp_fatal_err_flag = TRUE;
#endif
#endif

	fd = drv_open(PRIMARY_LCD_NAME);
	drv_init(fd);
	disp_powerup();    

#ifndef FEATURE_MDDI
	lcd_info_1 = disp_get_info();
#endif /* FEATURE_MDDI */

#ifdef FEATURE_BOOT_SPLASH_SCREEN
	disp_epson_S1D19120_boot_chg_splash_screen();
    clk_busy_wait(100*1000);
#endif

#ifdef CUST_EDITION
    disp_set_backlight(LCD_BACKLIGHT_LVL_2);
#ifdef FEATURE_MDP
    mdp_disp_fatal_err_flag = FALSE;
#endif
#endif

#ifdef FEATURE_MDP
	mdp_init();
#endif
}


disp_info_type disp_get_info(void)
{
  disp_info_type lcd_info;
  
  drv_ioctl(fd, IOCTL_DISP_GET_INFO, (void *)&lcd_info);
    
  return lcd_info;
}

void disp_powerup(void)
{
  drv_ioctl(fd, IOCTL_DISP_POWERUP, NULL);
}

void disp_powerdown(void)
{
  drv_shutdown(fd);
}

void disp_update
(
  /* The buffer pointer point to the first byte of the whole buffer.
  */
  void *buf_ptr,
  /* Source image width */
  int16 src_width,
  /* Source rectangle starting row */
  int16 src_starting_row,
  /* Source rectangle starting column */
  int16 src_starting_column,
  /* Number of rows to update */
  int16 num_of_rows,
  /* Number of columns to update */
  int16 num_of_columns,
  /* Device rectangle starting row */
  int16 dst_starting_row,
  /* Device rectangle starting column */
  int16 dst_starting_column
)
{
  dup_arg.buf_ptr = buf_ptr;
  dup_arg.src_width = src_width;
  dup_arg.src_starting_row = src_starting_row;
  dup_arg.src_starting_column = src_starting_column;
  dup_arg.num_of_rows = num_of_rows;
  dup_arg.num_of_columns = num_of_columns;
  dup_arg.dst_starting_row = dst_starting_row;
  dup_arg.dst_starting_column = dst_starting_column;
  dup_arg.dest = PRIMARY_LCD_TYPE;

#ifdef FEATURE_MDP
  if (mdp_disp_fatal_err_flag)
    drv_ioctl(fd, IOCTL_DISP_UPDATE_UNDER_ERR_FATAL, (void *)&dup_arg);
  else
    drv_ioctl(mdp_fd, IOCTL_DISP_UPDATE, (void *)&dup_arg);
#else
  drv_ioctl(fd, IOCTL_DISP_UPDATE, (void *)&dup_arg);
#endif

//#ifdef FEATURE_MDDI
//  if (lcd_info_1.lcd_type == MDDI_LCD_PRIM)
//    mddi_mdp_active_flag = FALSE;
//#endif // FEATURE_MDDI

  return;
}

#ifdef FEATURE_DSP

void disp_setwindows(unsigned short x,unsigned short y,unsigned short w,unsigned short h)
{
	dup_arg.dst_starting_row 	= x;
	dup_arg.dst_starting_row 	= y;
	dup_arg.num_of_rows 		= h;
	dup_arg.num_of_columns 		= w;

	drv_ioctl(fd, IOCTL_DISP_SET_WINDOWS, (void *)&dup_arg);
	return;
}

#endif

void disp_set_contrast(
  word contrast /* Contrast value to set */
)
{
  drv_ioctl(fd, IOCTL_DISP_SET_CONTRAST, (void *)&contrast);
}

void disp_set_backlight(byte level)
{
  drv_ioctl(fd, IOCTL_DISP_SET_BACKLIGHT, (void *)&level);
}


void disp_clear_whole_screen(void)
{
#ifdef FEATURE_MDP
  LCD_TYPE dest = PRIMARY_LCD_TYPE;

  if (!mdp_disp_fatal_err_flag)
    drv_ioctl(mdp_fd, IOCTL_DISP_CLEAR_WHOLE_SCREEN,(void *)dest);
#ifdef CUST_EDITION
  else
    drv_ioctl(fd, IOCTL_DISP_CLEAR_WHOLE_SCREEN, NULL);
#endif
#else
  drv_ioctl(fd, IOCTL_DISP_CLEAR_WHOLE_SCREEN, NULL);
#endif
}

void disp_clear_screen_area
(
  word start_row,
  word end_row,
  word start_column,
  word end_column
)
{
  cls_arg.start_row = start_row;
  cls_arg.end_row = end_row;
  cls_arg.start_column = start_column;
  cls_arg.end_column = end_column;
  cls_arg.dest = PRIMARY_LCD_TYPE;

#ifdef FEATURE_MDP
  if (!mdp_disp_fatal_err_flag)
    drv_ioctl(mdp_fd, IOCTL_DISP_CLEAR_SCREEN_AREA, (void *)&cls_arg);
#ifdef CUST_EDITION
  else
    drv_ioctl(fd, IOCTL_DISP_CLEAR_SCREEN_AREA, (void *)&cls_arg);
#endif
#else
  drv_ioctl(fd, IOCTL_DISP_CLEAR_SCREEN_AREA, (void *)&cls_arg);
#endif
}


void disp_off(void)
{
  drv_ioctl(fd, IOCTL_DISP_OFF, NULL);
#ifdef FEATURE_MDP
  drv_ioctl(mdp_fd, IOCTL_DISP_OFF, (void *)PRIMARY_LCD_TYPE);
#endif
}

void disp_on(void)
{
  drv_ioctl(fd, IOCTL_DISP_ON, NULL);
#ifdef FEATURE_MDP
  drv_ioctl(mdp_fd, IOCTL_DISP_ON, (void *)PRIMARY_LCD_TYPE);
#endif
}

dword disp_capture
(
 /* destination */
 void * buf_ptr,
 /* size of the destination buffer */
 dword copy_count,
 /* rectangle starting row */
 int16 starting_row,
 /* rectangle starting column */
 int16 starting_column,
 /* Number of rows to capture */
 int16 ending_row,
 /* Number of columns to capture */
 int16 ending_column
 )
{
#if defined(FEATURE_HS_DISPLAY_CAPTURE) && defined(FEATURE_MDP)

  dup_arg_capture.buf_ptr = buf_ptr;
  dup_arg_capture.src_width = (ending_column - starting_column) + 1;
  dup_arg_capture.src_starting_row = starting_row;
  dup_arg_capture.src_starting_column = starting_column;
  dup_arg_capture.num_of_rows = (ending_row - starting_row) + 1;
  dup_arg_capture.num_of_columns = (ending_column - starting_column) + 1;
  dup_arg_capture.dest = PRIMARY_LCD_TYPE;

  
  //send command to driver to capture screen shot
  if(drv_ioctl(mdp_fd, IOCTL_DISP_CAPTURE_SCREEN, (void *)&dup_arg_capture))
  {

	  //return status of operation - sucess by returning number of pixels written
	  return (dup_arg_capture.num_of_rows * dup_arg_capture.num_of_columns);
  }

#endif /* FEATURE_HS_DISPLAY_CAPTURE */

  return 0;
  
}

/************* 2nd display ********************/

disp_info_type disp_get_info2(void)
{
  disp_info_type lcd_info;

#ifdef FEATURE_SECONDARY_DISPLAY
#error code not present
#else
  memset(&lcd_info,0,sizeof (disp_info_type));
  lcd_info.lcd_type = LCD_NONE;
#endif /* FEATURE_SECONDARY_DISPLAY */

return lcd_info;
}

void disp_powerup2(void)
{
#ifdef FEATURE_SECONDARY_DISPLAY
#error code not present
#endif /* FEATURE_SECONDARY_DISPLAY */
}

void disp_powerdown2(void)
{
#ifdef FEATURE_SECONDARY_DISPLAY
#error code not present
#endif /* FEATURE_SECONDARY_DISPLAY */
}

void disp_update2
(
  /* The buffer pointer point to the first byte of the whole buffer.
  */
  void *buf_ptr,
  /* Source image width */
  int16 src_width,
  /* Source rectangle starting row */
  int16 src_starting_row,
  /* Source rectangle starting column */
  int16 src_starting_column,
  /* Number of rows to update */
  int16 num_of_rows,
  /* Number of columns to update */
  int16 num_of_columns,
  /* Device rectangle starting row */
  int16 dst_starting_row,
  /* Device rectangle starting column */
  int16 dst_starting_column
)
{
#ifdef FEATURE_SECONDARY_DISPLAY
#error code not present
#else // FEATURE_SECONDARY_DISPLAY
  // Empty stub
#endif // FEATURE_SECONDARY_DISPLAY
  return;
}


void disp_set_contrast2(
  word contrast /* Contrast value to set */
)
{
#ifdef FEATURE_SECONDARY_DISPLAY
#error code not present
#else
  // Empty stub
#endif
}

void disp_set_backlight2(byte level)
{
#ifdef FEATURE_SECONDARY_DISPLAY
#error code not present
#else
  // Empty stub
#endif
}


void disp_clear_whole_screen2(void)
{
#ifdef FEATURE_SECONDARY_DISPLAY
#error code not present
#else // FEATURE_SECONDARY_DISPLAY
  // Empty stub
#endif // FEATURE_SECONDARY_DISPLAY
}

void disp_clear_screen_area2
(
  word start_row,
  word end_row,
  word start_column,
  word end_column
)
{
#ifdef FEATURE_SECONDARY_DISPLAY
#error code not present
#else // FEATURE_SECONDARY_DISPLAY
  // Empty stub
#endif // FEATURE_SECONDARY_DISPLAY
}


void disp_off2(void)
{
#ifdef FEATURE_SECONDARY_DISPLAY
#error code not present
#else // FEATURE_SECONDARY_DISPLAY
  // Empty stub
#endif // FEATURE_SECONDARY_DISPLAY
}

void disp_on2(void)
{
#ifdef FEATURE_SECONDARY_DISPLAY
#error code not present
#else // FEATURE_SECONDARY_DISPLAY
  // Empty stub
#endif // FEATURE_SECONDARY_DISPLAY
}

dword disp_capture2
(
 /* destination */
 void * buf_ptr,
 /* size of the destination buffer */
 dword copy_count,
 /* rectangle starting row */
 int16 starting_row,
 /* rectangle starting column */
 int16 starting_column,
 /* Number of rows to capture */
 int16 ending_row,
 /* Number of columns to capture */
 int16 ending_column
 )
 {
  return 0;
 }

#ifdef FEATURE_TORCH_SUPPORT
#ifdef FEATURE_LCD_TOUCH_ENABLE
#define GPIO_LAMP_EN GPIO_OUTPUT_62

void disp_set_torch(void)
{
	gpio_tlmm_config(GPIO_LAMP_EN);
	gpio_out(GPIO_LAMP_EN,GPIO_HIGH_VALUE);
}
void disp_clear_torch(void)
{
	gpio_tlmm_config(GPIO_LAMP_EN);
	gpio_out(GPIO_LAMP_EN,GPIO_LOW_VALUE);
}

#else
//#if defined(FEATURE_PROJECT_W021) || defined(FEATURE_PROJECT_W022) || defined(FEATURE_PROJECT_W021_128x160)|| defined (FEATURE_PROJECT_W021_176X220) || defined (FEATURE_PROJECT_W021_240X320)|| defined (FEATURE_PROJECT_W021_220X176) || defined (FEATURE_PROJECT_W021_320X240)
//#if defined(FEATURE_PROJECT_W021) || defined(FEATURE_PROJECT_W022) || defined(FEATURE_PROJECT_W021_128x160)

#if defined (FEATURE_GPIO_LAMP_EN_OUTPUT_31)
#if defined(FEATURE_VERSION_K232_Y100)||defined(FEATURE_VERSION_K232_Y105A)||defined(FEATURE_VERSION_K232_Y100A)||defined(FEATURE_VERSION_K232_Y101)
#define GPIO_LAMP_EN GPIO_OUTPUT_33
#else
#define GPIO_LAMP_EN GPIO_OUTPUT_31
#endif
//#elif defined(FEATURE_PROJECT_W203) || defined(FEATURE_PROJECT_W204) 
#elif defined(FEATURE_GPIO_LAMP_EN_OUTPUT_32)
#define GPIO_LAMP_EN GPIO_OUTPUT_32
#elif defined(FEATURE_GPIO_LAMP_EN_OUTPUT_33)
#define GPIO_LAMP_EN GPIO_OUTPUT_33
#else
#define GPIO_LAMP_EN GPIO_GENERIC_DEFAULT
#endif

void disp_set_torch(void)
{
	gpio_tlmm_config(GPIO_LAMP_EN);
	gpio_out(GPIO_LAMP_EN,GPIO_HIGH_VALUE);
}
void disp_clear_torch(void)
{
	gpio_tlmm_config(GPIO_LAMP_EN);
	gpio_out(GPIO_LAMP_EN,GPIO_LOW_VALUE);
}
#endif
#endif

/************* MDP display ********************/
#ifdef FEATURE_MDP
static void mdp_init(void)
{
  if (mdp_install(MDP_DEV_NAME) != -1)
  {
    mdp_fd = drv_open(MDP_DEV_NAME);
    drv_init(mdp_fd);
  }

  mdp_set_lcd_size(PRIMARY_LCD_TYPE, lcd_info_1.lcd_type, 
                   lcd_info_1.disp_width, lcd_info_1.disp_height);
#ifndef CUST_EDITION
  drv_ioctl(mdp_fd, IOCTL_DISP_CLEAR_WHOLE_SCREEN, (void *)PRIMARY_LCD_TYPE);
#endif
#ifdef FEATURE_SECONDARY_DISPLAY
#error code not present
#endif /* FEATURE_SECONDARY_DISPLAY */
}
#endif /* FEATURE_MDP */

#if (defined(FEATURE_MP4_DECODER) || defined(FEATURE_CAMERA_NOFULLSCREEN)) //&& !defined(T_QSC1110)
void disp_update_lock
(
  /* The buffer pointer point to the first byte of the whole buffer.
  */
  void *buf_ptr,
  /* Number of rows to update */
  int16 num_of_rows,
  /* Number of columns to update */
  int16 num_of_columns
)
{
  duplock_arg.buf_ptr = buf_ptr;
  duplock_arg.num_of_rows = num_of_rows;
  duplock_arg.num_of_columns = num_of_columns;
  duplock_arg.dest = PRIMARY_LCD_TYPE;
  
  drv_ioctl(fd, IOCTL_DISP_UPDATE_LOCK, (void *)&duplock_arg);
}

void disp_update_yuv420
(
  /* The buffer pointer point to the first byte of the whole buffer.
  */
  void *buf_ptr,
  /* Number of rows to update */
  int16 num_of_rows,
  /* Number of columns to update */
  int16 num_of_columns
)
{
  dupyuv_arg.buf_ptr = buf_ptr;
  dupyuv_arg.num_of_rows = num_of_rows;
  dupyuv_arg.num_of_columns = num_of_columns;
  dupyuv_arg.dest = PRIMARY_LCD_TYPE;
  
  drv_ioctl(fd, IOCTL_DISP_UPDATE_LOCK_YUV, (void *)&dupyuv_arg);
}

void disp_lock_screen
(
  word start_row,
  word num_row,
  word start_column,
  word num_column
)
{
  lock_arg.start_row = start_row;
  lock_arg.num_row = num_row;
  lock_arg.start_column = start_column;
  lock_arg.num_column = num_column;
  
  drv_ioctl(fd, IOCTL_DISP_LOCK_SCR, (void *)&lock_arg);
}
#endif

