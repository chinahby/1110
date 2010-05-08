#ifndef _MMP_RDK_H_
#define _MMP_RDK_H_

/************************************************************************************ 
MP4   或者MMP  平台定义
************************************************************************************/


#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "compile_macro.h"
#include "mmd_mm.h"
#include "mmd_common.h"
#include "davincicode.h"

//user depend head file
#include "mmd_user_depend.h"
//#include "mmd_OS_depend.h"
//#include "mmd_baseband_depend.h"
//#include "mmd_GUI_depend.h"
#include "DRIVERIC_Info.h"
#include "panel.h"
#include "mmd_irq_handler.h"

//rdk head file
#include "mmd_lif_driver.h"
#include "MMD_I2C_Driver.h"
#include "vim_common.h"
#include "mmd_lcdif_driver.h"

#include "v8_biu.h"
#include "..\Drivers\biu\v8_bypass.h" 
#include "mmd_audio.h"
#include "codec.h"		//add by hbp 2009-02-10

#include "mmd_sys_driver.h"
#include "mmd_file_driver.h"
#include "mmd_sd_driver.h"
#include "mmd_lcd_driver.h"
#include "mmd_ply_driver.h"
#include "mmd_usb_driver.h"
#include "mmd_cmmb_driver.h"
//#include "mmd_rv_driver.h"
#include "me_object.h"

// AASP test config define
#define AASP_LCD_ENABLE	0
#define AASP_SD_ENABLE		0
#define AASP_FILE_ENABLE	0
#define AASP_SYS_ENABLE	1 
#define AASP_MEM_ENABLE	0
#define AASP_VIDEO_ENABLE	0
#define AASP_USB_ENABLE	0
#define AASP_MEDIA_ENABLE		0
#ifdef CMMB_ENABLE	
#define AASP_CMMB_ENABLE		1
#endif
#define AASP_VC988_ENABLE		1
#define AASP_RV_ENABLE			0
#endif  /* _MMP_RDK_API_H_ */


