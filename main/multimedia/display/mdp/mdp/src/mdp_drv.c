/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  MDP Device Driver

  GENERAL DESCRIPTION
  This module contains functions and data blocks pertaining to the
  Mobile Display Processor (MDP) Driver

  EXTERNALIZED FUNCTIONS

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

  Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*============================================================================

  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/display/mdp1/mdp/main/latest/src/mdp_drv.c#20 $ $DateTime: 2009/04/28 02:06:06 $ $Author: gauravs $

when      who     what, where, why
--------  -----   ----------------------------------------------------------
04/28/09  gs     Adding changes to avoid rex_wait when MDPOP_VSYNC_OFF is enabled 
04/22/09  gs     Adding support for MDPOP_VSYNC_OFF and changes to avoid waiting in mdp_start_script when MDP_SYS_VSYNC_OFF is enabled.
04/17/09  gs      MDP H/W Vsync support
03/17/09  gs      Adding support for 24BPP output support
11/07/08  gs      Adding WQVGA Display support
10/01/08  rk      Fixed bug when MDP writes odd number of RGB565 pixels to memory.
09/19/08  gs      compiler warning & Lint fixes
09/09/08  rk      Added fix for ULC Cache write back
08/04/08  rk      Added GPIO header files for LCU target
07/11/08  rk      Added Featurization for GPIO header files on ULC.
06/20/08  rk      Added GPIO support for MSM6246
06/11/08  rk      Added support for S/W Vsync when MDP update time is less than LCD read time.
06/05/08  dw      Prevent multiple initializations of sleep/clock handles
03/18/08  dw      Reset script counter for Epson QVGA 
03/13/08  dw      Add support for Epson QVGA panels
                  Add references to support MDP 1.4 hardware
12/07/07  dw      Fixed bug that caused black pixels during clear screen updates
11/07/07  dw      Add support for dynamic memory allocation of display buffers
10/06/07  dw      Temporary workaround to remove TMD20 offset dependency
09/21/07  dw      Fix compile warnings when FEATURE_HS_DISPLAY_CAPTURE disabled
06/22/07  dw      Remove erroneous WAIT_JOB_DONE's from SC2X QCIFP scripts
06/20/07  trg     Default the debug bus state check to off, somebody else
                  appears to be using the debug bus.
05/24/07  cl      MDP reset enhancement for 6k targets
05/16/07  dw      Fixed mdp_copyLayer565() to work with multiple overlay ROIs.
                  Fixed bug to ensure MDPOP_VSYNC_OFF is reflected in script.
                  Ensured SST_TERM_DIV_CNT & MDP_MAX_SST_RD_PTR include porches.
05/14/07  dw      Fixed compile error when FEATURE_MDDI is not defined.
04/30/07  dw      Set hclk restriction to full speed when negating sleep.
04/11/07  dw      Fixed mdp_copyLayer565() to work with different panel sizes
04/06/07  dw      Add support for Epson Optrex QcifP EBI2 display panel.
03/30/07  trg     Add interface for qvga to wqvga scaling for 6k.  Use 
                  FEATURE_MDP_WSCALE to enable.
03/29/07  dw      Dynamic hclk support without use of critical sections 
                  Added support to request minimum bus to prevent TCXO shutdown
03/12/07  dw      Finetune and bug fix for dynamic hclk support
03/09/07  dw      Use new sleep APIs during MDP HW operation
03/08/07  dw      Added support for dynamic hclk
03/08/07  dw      Disable MDDI client initiated wakeup for selected targets.
02/14/07  trg     Fixed MDDI/130nm 4pixel horizontal sectioning bug.
                  Added error checking for invalid MDDI/130 update configs.
02/09/07  trg     Rewrote mdp_copyLayer to fix screen capture + scale issue.
02/01/07  trg     Added code to prevent infinite loop from occuring and
                  an sanity exit for infinite loop.
01/11/07  cjr     Fixed mdp_task_wait_sig to use MDP_COMPLETE_SIG
01/05/07  trg     Added 6245 target to disable SFD.
12/22/06  cl      Featurized lcd_mddi_cur_panel with FEATURE_MDDI
10/04/06  trg     Fixed layer 1 issue which may drop layer 2 updates.
10/02/06  cjr     Fixed 16n+1 workaround happens at VDB (MDP HW Buffer)
09/08/06  cjr     Replaced mdp_wait_script_done() with rex_sleep() in mdp_start_script()
08/03/06  trg     Enable SFD for supporting HW only.
08/03/06  trg     Move vsync script/power script to mdp_start_script
07/17/06  dw      Remove FEATURE_DISP_TASK
07/14/06  cjr     MDP HW update timestamp in debug mode
07/11/06  trg     Add power cleanup to script, change OKTS calls per target.
06/30/06  cjr     MDP ISR clean up at mdp_reset()
05/16/06  trg     Add MDP QXDM messages
05/10/06  cjr     Added support for display API queue
05/04/06  cjr     Added MDDI window setting workaround
04/26/06  dw      Explicitly set and unset dithering operation
04/06/06  dw      Enable dithering operation
03/29/06  trg     Disable sfd for ALL MDP/MDDI targets, CR 90868
03/13/06  cjr     Added Layer1 support for the secondary display
03/13/06  cjr     Fixed Layer1 buffer cache issue
02/24/06  trg     Add XRGB_8888 support for MDP 1
02/01/05  cjr     Added autodog() during rex_wait()
01/09/05  cjr     Added workaround for MDDI CRC error and 16*n+1 image width
10/21/05  cjr     Added MDP1.1 support
10/13/05  cjr     Added MDPOP_QUEUE_EXPAND operation under MDP IOCTL
08/23/05  trg     Added mddi vsync
08/22/05  cjr     Fixed LCD-auto detect mddi setting 
08/19/05  cjr     Added L4 changes in mdp_start_script()
08/05/05  cjr     display task integration for high fps
07/25/05  trg     Auto detect MDDI/EBI LCD panel
06/30/05  cjr     Added clearing signal after script done
06/20/05  gr      Added support for building with gcc
06/20/05  gm      Added a fix for new MDDI display--girishm
06/17/05  cjr     Fixed LAYER_1 memcpy issue
04/27/05  cjr     Added dual clock support
04/12/05  cjr     Fixed video fringe condition and enhanced VSYNC algorithm
03/31/05  cjr     L4 feature support 
03/29/05  tb      Added calls to mddi_host_mdp_active().
03/11/05  cjr     CCS reprogramming at the reset
03/11/05  cjr     Added to support any size of image for layer 1
03/11/05  cjr     LCD edge one pixel line bug fixed
03/08/05  cjr     Adjusted MDP system bitmap with BREW offset
02/25/05  cjr     Added Gamma correction table 
02/23/05  cjr     Zero alpha value bug fix
02/10/05  cjr     Completed source file re-architect
01/27/05  cjr     Toshiba LCD rev2 support
01/20/05  cjr     Fixed CCS script bug
12/30/04  cjr     Added the third layer support
12/17/04  cjr     Fixed LCD size cropping, the maximum LCD width and height
12/15/04  cjr     Added BREW Application/System context switch support
                  Removed CCS settings from the video script
                  Disable Simultaneous fill and dump for MDDI
12/07/04  cjr     Added mdp_memcpy_dblimg()
11/29/04  cjr     Aligned all script address to 16 bytes boundary
11/04/04  cjr     Inserted more NOPs after MDP_SET_DEFAULT instruction
                  No script pointer update if MDP is in the freeze state
                  Changed timer back to 100 msec
10/26/04  cjr     Changed MDDI minimum packet size from 2 to 4
                  Changed timer expiration from 100 to 500 msec
10/15/04  cjr     Added support for MDP memory write back
10/13/04  cjr     MDDI strip break-down algorithm fix
10/07/04  cjr     redesigned mddi stripe break-down algorithm for maximum performance
10/04/04  cjr     new registeration rule only activated by calling mdp_set_sys_bmp()
09/23/04  cjr     LCD location adjustment bug fix
09/07/04  cjr     Fixed MDDI line number calculation
08/31/04  cjr     Added VSYNC support
08/27/04  cjr     Added BREW Display update ovimg region and rectangular sorting method
08/16/04  cjr     Added mdp_in_processing and mdp_total_vdopkts vars
08/10/04  cjr     Added additional MDP overlay functions
07/19/04  cjr     Added more MDP overlay functions
06/30/04  cjr     Added MDP overlay functions
05/28/04  cjr     Initial code
============================================================================ */

/*============================================================================
INCLUDE FILES FOR MODULE
============================================================================*/
  #include <math.h>
  #include "customer.h"

  #ifdef FEATURE_MDP

  #include "msmhwioreg.h"
  #include "msmhwio.h"
  #include "task.h"
  #include "sleep.h"
  #include "tramp.h"
  #include "drvLib.h"
  #include "msg.h"
  #include "disp.h"

  #include "mdp_drv.h"
  #include "mdp_api_proc.h"
  #include "mdp_layer_proc.h"
  #include "mdp_reg_defs.h"
  #include "mdp_hw.h"
  #include "mdp_buffers.h"

  #include "err.h"
  #include "rex.h"
  #include "armasm.h"
  #include CLKRGM_H 

  #include "gpio_int.h"

  #if (defined (T_QSC1110))
   #include "gpio_1100.h"
  #elif ((defined(T_QSC60X5) && !defined(FEATURE_TMD_20_QVGA)))
   #include "gpio_60x5.h"
  #elif (defined (T_QSC6270) || defined (T_QSC6240))
   #include "tlmm_gpio.h"

  #elif defined(FEATURE_TMD_22_QVGA)
   #include "gpio_6246.h"
  #endif

  #include "disp_task.h"

  #ifdef FEATURE_MDDI
#error code not present
  #endif

  #ifdef FEATURE_DISPLAY_MDP_HARD_FAILURE
#error code not present
  #endif /* FEATURE_DISPLAY_MDP_HARD_FAILURE */

  #if defined (T_MSM6280) || defined (T_MSM6260)
#error code not present
  #endif

  /* FEATURE_CLKREGIM_DEVMAN for 6280, 6260. 6275 support not needed */
  /* FEATURE_CLKRGM_AHB_CLIENT for 6275 and 6800             */
  #if defined (FEATURE_CLKREGIM_DEVMAN) || defined (FEATURE_CLKRGM_AHB_CLIENT)
    #include "clkrgm_api.h"
  #endif

  #ifdef FEATURE_CLKRGM_AHB_CLIENT
  #define MDP_VOTE_FOR_MIN_BUS() {clk_regime_register_ahb_constant(CLK_AHB_MDP);}
  #define MDP_RELEASE_MIN_BUS() {clk_regime_deregister_ahb_constant(CLK_AHB_MDP);}
  #else
  #define MDP_VOTE_FOR_MIN_BUS()
  #define MDP_RELEASE_MIN_BUS()
  #endif

  #define MDP_WB_CLR  0xFFFFFFFF

  #define MDP_MDDI130_MIN_PX_SZ   5
  #define MDP_MDDI130_MIN_CWIN_X  2
  #define MDP_MDDI130_MIN_CWIN_Y  2

  #define MDP_SET_CCS_WAIT_CNT    0xFFFF

/* QVGA resolution */
  #define QVGA_WIDTH 240
  #define QVGA_HEIGHT 320

/*******************/
/* Local Functions */
  static boolean mdp_init_system_frames(void);
  static uint32 mdp_read_debug_bus(uint32 dbgCode);
  static void mdp_script_replace(uint32 *ptr_script,
                               uint32 script_size,
                               uint32 script_length,
                               uint32 old_command,
                               uint32 new_command);

  static void mdp_hclk_active(void);
  static void mdp_hclk_not_active(void);
#ifdef MDP_SW_VSYNC
  static uint32  mdp_get_vsync_wait_time(MDP_CWIN cwin);
#endif
  static void mdp_config_vsync(uint32 hclk);

  #ifdef FEATURE_MDDI
#error code not present
  #endif /* FEATURE_MDDI */

  #ifdef FEATURE_CLKREGIM_DEVMAN
  static void mdp_reconfig_vsync(uint32 hclk);
  static void mdp_clkrgm_cb(clkrgm_device_type device,
                          clkrgm_device_speed_change_type position,
                          uint32 old_speed_khz, uint32 new_speed_khz);
  static clkrgm_devman_client_type mdp_client;
  #endif /* FEATURE_CLKREGIM_DEVMAN */

/* rex task associated data */
  rex_crit_sect_type   mdp_crit_sect;
  rex_timer_type mdp_rex_timer;
  static rex_tcb_type * mdp_tcb_ptr;

  uint32 mdp_drv_initialized = FALSE;
  uint32 *mdp_sys_bmp = NULL;

  uint32 mdp_sum_job_cnt = 0;
  uint32 mdp_sum_isr = 0;
  uint32 mdp_sum_isr_cip = 0;
  uint32 mdp_sum_tcb_sigclr_isr = 0;
  volatile uint32 mdp_in_processing = FALSE;
  uint32 mdp_log_enable =  FALSE;
  uint32 mdp_scr_started = FALSE;
  #define MDP_MAX_PIXEL_WIDTH 1024
  #define MDP_MAX_PIXEL_HEIGHT 1024
  extern boolean mdp_disp_fatal_err_flag;
  extern boolean mdp_debug_calcFps;

  #ifdef FEATURE_NEW_SLEEP_API
  sleep_okts_handle mdp_sleep_handle = {0};
  #endif /* FEATURE_NEW_SLEEP_API */

  int mdp_exp_time = 100;

  /*********************VSYNC related local variables****************************/
  #define MDP_DEFAULT_LCD_PORCH_LINE 16
  #define NUM_USEC_IN_MSEC 1000000
  uint32 mdp_vsync_enabled = FALSE; /*enables the s/w vsync algo (MDP WT< LCD RT)*/
  static timetick_type mdp_last_vsync_timetick=0;
  uint32 lcd_ref_usec_time = 0;
  uint32 sub_width = 0;
  uint32 sub_height = 0;
  boolean  mdp_update_memMode = FALSE;
  uint32 last_update_width = 0;
  uint32 last_update_height=0;
  uint32 lcd_ref_hz = 0;
  int32 lcd_y =0;
  int mdp_width_boundary[MDP_MAX_PIXEL_WIDTH];
  int mdp_update_time[MDP_MAX_PIXEL_HEIGHT];

  int32 back_porch_line = MDP_DEFAULT_LCD_PORCH_LINE; 
  int32 prim_lcd_lines = 0 ;
  int32 porch_line = 0;
  int mdp_vsync_resync_time=0;
  static timetick_type mdp_hw_start_timestamp_ms=0;
  static timetick_type mdp_hw_finish_timestamp_ms=0;
  static timetick_type  mdp_dma2_update_time_in_usec=0;
  static timetick_type  mdp_dma2_start_time_in_usec=0;
  static int mdp_debug_vsync = 1;
  uint32 mdp_last_vsync_timetick_index = 0;
  uint32 wait_time = 0;
  
  #ifdef FEATURE_MDP_WSCALE
  // When enter Wscale no stretch, the HW requires a reset the first time only.
  static boolean resetMdpForWscaleNoStretch = TRUE;
  #endif

#ifdef MDP_HW_VSYNC 
static void mdp_modify_vsync_script(MDP_CWIN cwin); 
#endif

/***************/
/* DEBUG FLAGS */
/***************/

  static boolean mdp_debug_misrCrc = FALSE;    /* QXDM out CRC for each MDP update? */
  static boolean mdp_debug_compState = FALSE;  /* QXDM out non-zero "invalid" states? */

  dbg_flags mdp_debug = {  /* QXDM output default settings*/
  FALSE,    /* =0 do nothing, else ignore flags, turn all on */

  /* THE FLAGS BELOW APPLY ONLY IF ABOVE FLAG IS SET TO ZERO */

  TRUE,   /* Enable below flags, superceded by above */
  TRUE,   /* Fatal error */
  TRUE,   /* Error msg */
  FALSE,  /* High */
  FALSE,  /* Medium */
  FALSE,  /* Low */
  FALSE,  /* Debug */
  FALSE   /* Profile */
  };

LCD_TYPE_PHYS lcdPhysDest[MAX_LCD_NUM];

/* --- Display Buffer Pointers --- */
static mdp_frame_type* white_frame;         /* White buffer                 */
static mdp_frame_type* mdpTempImgBuf;       /* Temp buffer for L1, capture  */
static mdp_frame_type* mdp_slBuf_1;         /* Pri sub layer bitmap, 16-bit */
static mdp_frame_type* mdp_slBuf_2;         /* Sec sub layer bitmap, 16-bit */
static mdp_frame_type* mdp_L1buf_1;         /* Layer-1 for PRI panel        */
static mdp_frame_type* mdp_L1buf_2;         /* Layer-1 for SEC panel        */
#ifdef FEATURE_HS_DISPLAY_CAPTURE
static mdp_frame_type* mdpScreenCaptureBuf; /* Buffer for screen capture    */
#endif
/* --- End of Display Buffers --- */

MDPIMG_LIST *mdpSubNode, *mdpSubNode2;

boolean mdpRuntimeDispMode = FALSE;

uint32 GAMMA_R[GAMMA_TABLE_SIZE + 1];    // RAM locations + RETURN instruction
uint32 GAMMA_G[GAMMA_TABLE_SIZE + 1];    // RAM locations + RETURN instruction
uint32 GAMMA_B[GAMMA_TABLE_SIZE + 1];    // RAM locations + RETURN instruction

uint32 mdp_gamma_enable = 0;
boolean mdp_dither_on = TRUE;

uint32 mdp_task_wait_sig = MDP_COMPLETE_SIG;
// mdp user callback pointer to be called and cleared by ISR
disp_cb_type mdp_user_cb;

extern void mddi_window_adjust(uint16 x1,
                               uint16 x2,
                               uint16 y1,
                               uint16 y2);

uint16 mddi_assign_pkt_height(uint16 pkt_width, uint16 pkt_height,
                            uint16 bpp);
                        
  #ifdef FEATURE_HS_DISPLAY_CAPTURE
//========================================================================================
// MDP Screen Capture - For Diagnosis only
//========================================================================================
//screen capture flag - set to true to store LCD content
boolean mdpCaptureMode = FALSE;
//one shot flag to ensure that the first LCD screen capture has been obtained
boolean mdpCaptureScreenBufferValid = FALSE;
  #endif /*FEATURE_HS_DISPLAY_CAPTURE*/

//========================================================================================
// MDP raw graphic display API prototype
//========================================================================================

// #define MAX_MEM_LIMIT (2*1024*1024 - 4)  // 2MB memory
  #define MAX_MEM_LIMIT (0xffffffff)
  
int32 mdpLcdSize[MAX_LCD_NUM][2];
uint32 mdpWhiteBgOvId[MAX_LCD_NUM];

uint16 mdp_scr_give[MDP_MAX_SCRNUM];
uint16 mdp_scr_lock[MDP_MAX_SCRNUM];

boolean mdp_mddi_enable = FALSE;
boolean mdp_mddi_bypass_16lines = TRUE;

#ifdef FEATURE_MDDI
#error code not present
#endif // FEATURE_MDDI

#ifndef FEATURE_MDDI
static uint32 mdp_ebi_rows_per_refresh = 0;   /*vsync algo parameters for EBI-2 LCDs*/
static uint32 mdp_ebi_rows_per_second = 0;
#endif /****#ifndef FEATURE_MDDI****/

/* LCD display information */
disp_info_type _primary_lcd; 
disp_info_type _secondary_lcd; 
disp_info_type _biggest_lcd;

uint32 mdp_prim_lcdWidth =0 ;
uint32 mdp_prim_lcdHeight = 0;

int mdp_sfd_enable;
int mdp_min_pixel_size = 2;

static boolean mdp_toggle_output_format_enable = FALSE;

#ifdef FEATURE_TMD_20_QVGA
// EBI2 QVGA panel offset - temporary workaround to remove TMD20 dependency
extern uint32 tmd20qvga_panel_offset;
#else
uint32 tmd20qvga_panel_offset = 0;
#endif /* FEATURE_TMD_20_QVGA */


static MDP_CCS_TYPE * mdp_curr_ccs;
static MDP_CCS_TYPE mdp_default_ccs = 
{
  0x254, //0
  0x000, //1
  0x331, //2
  0x254, //3
  0xF37, //4
  0xE60, //5
  0x254, //6
  0x409, //7
  0x000, //8
  0xF21, //9
  0x088, //10
  0xEEB, //11
};

#ifdef CUST_EDITION
extern uint8 disp_drv_mdp_getformat(void);
extern uint16 disp_drv_mdp_getscr(uint32 **ppscr);
extern void disp_drv_mdp_scrupdate(uint32 *scr, uint32 start_row, uint32 start_col, uint32 end_row, uint32 end_col);
#else

// LCD address must be 4-bytes aligned 
  #if (DISP_CMD_PORT1%4) || (DISP_DATA_PORT1%4)
    #error : MDP requires 4-bytes address alignment
  #endif

// All mdp scripts and return addresses must be aligned to 16bytes boundary
// Epson EBI2 LCD Script
  #ifdef __GNUC__
uint32 MDP_DISP_EPSON_QCIF_SCR[MDP_EPSON_QCIF_SCR_SIZE][MDP_EPSON_QCIF_SCR_LEN]
__attribute__ ((aligned (16))) =
  #else
__align(16) uint32 MDP_DISP_EPSON_QCIF_SCR[MDP_EPSON_QCIF_SCR_SIZE][MDP_EPSON_QCIF_SCR_LEN] =
  #endif
{
  NOP,                //0
  NOP,                //1
  SET_LCD_CNTL_ADDR,  //2 set LCD command port address
  DISP_CMD_PORT1,     //3

  SET_LCD_DATA_ADDR,  //4 set LCD data port address
  DISP_DATA_PORT1,    //5

  SEND_LCD_CNTL(DISP_CMD_SD_PSET),  //6

  NULL,               //7 ROW START
  NULL,               //8 ROW START

  NULL,               //9 ROW END
  NULL,               //10 ROW END

  NOP,                //11
  SEND_LCD_CNTL(DISP_CMD_SD_CSET), //12

  NULL,              //13 COL START
  NULL,              //14 COL START

  NULL,              //15 COL END
  NULL,              //16 COL END

  NOP,               //17
  SEND_LCD_CNTL(DISP_CMD_RAMWR),  //18

  RETURN             //19
};

// All mdp scripts and return addresses must be aligned to 16bytes boundary
// Epson QVGA EBI2 LCD Script
  #ifdef __GNUC__
uint32 MDP_DISP_EPSON_QVGA_SCR[MDP_EPSON_QVGA_SCR_SIZE][MDP_EPSON_QVGA_SCR_LEN]
__attribute__ ((aligned (16))) =
  #else
__align(16) uint32 MDP_DISP_EPSON_QVGA_SCR[MDP_EPSON_QVGA_SCR_SIZE][MDP_EPSON_QVGA_SCR_LEN] =
  #endif
{
  NOP,                //0
  NOP,                //1
  SET_LCD_CNTL_ADDR,  //2 set LCD command port address
  DISP_CMD_PORT3,     //3

  SET_LCD_DATA_ADDR,  //4 set LCD data port address
  DISP_DATA_PORT3,    //5

  SEND_LCD_CNTL(DISP_CMD_SD_CSET << 1),  //6
  NULL,               //7 COL START
  NULL,               //8 ROW START >> 8
  NULL,               //9 ROW START

  NOP,                //10

  SEND_LCD_CNTL(DISP_CMD_SD_PSET << 1), //11
  NULL,              //12 COL END
  NULL,              //13 ROW END >> 8 
  NULL,              //14 ROW END

  NOP,               //15

  SEND_LCD_CNTL(DISP_CMD_RAMWR << 1),  //16

  NOP,               //17

  NULL,              //18

  RETURN             //19
};

// LCD address must be 4-bytes aligned 
  #if (DISP_CMD_PORT3%4) || (DISP_DATA_PORT3%4)
    #error : MDP requires 4-bytes address alignment
  #endif

// Epson Optrex EBI2 QCIFP Pri LCD Script
  #ifdef __GNUC__
uint32 MDP_DISP_EP_OP_QCIFP_PRI_SCR[MDP_EP_OP_QCIFP_PRI_SCR_SIZE][MDP_EP_OP_QCIFP_PRI_SCR_LEN]
__attribute__ ((aligned (16))) =
  #else
__align(16) uint32 MDP_DISP_EP_OP_QCIFP_PRI_SCR[MDP_EP_OP_QCIFP_PRI_SCR_SIZE][MDP_EP_OP_QCIFP_PRI_SCR_LEN] =
  #endif
{
  NOP,                                   //0
  NOP,                                   //1
  SET_LCD_CNTL_ADDR,                     //2 set LCD command port address
  DISP_CMD_PORT3,                        //3

  SET_LCD_DATA_ADDR,                     //4 set LCD data port address
  DISP_DATA_PORT3,                       //5

  SEND_LCD_CNTL(DISP_CMD_SD_CSET << 10), //6 set start address

  NULL,                                  //7 COL START
  NULL,                                  //8 ROW START

  SEND_LCD_CNTL(DISP_CMD_SD_PSET << 10), //9 set end address

  NULL,                                  //10 COL END
  NULL,                                  //11 ROW END

  SEND_LCD_CNTL(DISP_CMD_RAMWR << 10),   //12 start ram write

  RETURN                                 //13 return to mdp script
};


// Epson Optrex EBI2 QCIFP Sec LCD Script
  #ifdef __GNUC__
uint32 MDP_DISP_EP_OP_QCIFP_SEC_SCR[MDP_EP_OP_QCIFP_SEC_SCR_SIZE][MDP_EP_OP_QCIFP_SEC_SCR_LEN]
__attribute__ ((aligned (16))) =
  #else
__align(16) uint32 MDP_DISP_EP_OP_QCIFP_SEC_SCR[MDP_EP_OP_QCIFP_SEC_SCR_SIZE][MDP_EP_OP_QCIFP_SEC_SCR_LEN] =
  #endif
{
  NOP,                                    // 0
  NOP,                                    // 1
  SET_LCD_CNTL_ADDR,                      // 2 set LCD command port address
  DISP_CMD_PORT3,                         // 3

  SET_LCD_DATA_ADDR,                      // 4 set LCD data port address
  DISP_DATA_PORT3,                        // 5 

  SEND_LCD_CNTL(DISP_CMD_SD_CSET << 10),  // 6 set start address

  NULL,                                   // 7 COL START
  NULL,                                   // 8 ROW START

  SEND_LCD_CNTL(DISP_CMD_SD_PSET << 10),  // 9 set end address

  NULL,                                   // 10 COL END
  NULL,                                   // 11 ROW END

  SEND_LCD_CNTL(DISP_CMD_RAMWR << 10),    // 12 start ram write

  RETURN                                  // 13 return to MDP script
};


// LCD address must be 4-bytes aligned 
  #if (DISP_CMD_PORT2%4) || (DISP_DATA_PORT2%4)
    #error : MDP requires 4-bytes address alignment
  #endif

// TMD EBI2 LCD Script
  #ifdef __GNUC__
uint32 MDP_DISP_TMD_QVGA_SCR[MDP_TMD_QVGA_SCR_SIZE][MDP_TMD_QVGA_SCR_LEN]
__attribute__ ((aligned (16))) =
  #else
__align(16) uint32 MDP_DISP_TMD_QVGA_SCR[MDP_TMD_QVGA_SCR_SIZE][MDP_TMD_QVGA_SCR_LEN] =
  #endif
{
  NOP,               //0
  NOP,               //1
  SET_LCD_CNTL_ADDR, //2 set LCD command port address
  DISP_CMD_PORT2,    //3

  SET_LCD_DATA_ADDR, //4 set LCD data port address
  DISP_DATA_PORT2,   //5

  SEND_LCD_CNTL(DISP_QVGA_18BPP(DISP_QVGA_HORZ_RAM_ADDR_POS_1_ADDR)),//6
  NULL,             //7
  SEND_LCD_CNTL(DISP_QVGA_18BPP(DISP_QVGA_HORZ_RAM_ADDR_POS_2_ADDR)),//8
  NULL,             //9
  SEND_LCD_CNTL(DISP_QVGA_18BPP(DISP_QVGA_VERT_RAM_ADDR_POS_1_ADDR)),//10
  NULL,             //11
  SEND_LCD_CNTL(DISP_QVGA_18BPP(DISP_QVGA_VERT_RAM_ADDR_POS_2_ADDR)),//12
  NULL,             //13
  SEND_LCD_CNTL(DISP_QVGA_18BPP(DISP_QVGA_RAM_ADDR_SET_1_ADDR)),//14
  NULL,             //15
  SEND_LCD_CNTL(DISP_QVGA_18BPP(DISP_QVGA_RAM_ADDR_SET_2_ADDR)),//16
  NULL,             //17
  SEND_LCD_CNTL(DISP_QVGA_18BPP(DISP_QVGA_CMD_RAMWR)),//18

  RETURN            //19
};
#endif // CUST_EDITION

// MDDI LCD Script
  #ifdef __GNUC__
uint32 MDP_DISP_MDDI_SCR[MDP_MDDI_SCR_SIZE][MDP_MDDI_SCR_LEN]
__attribute__ ((aligned(16))) =
  #else
__align(16) uint32 MDP_DISP_MDDI_SCR[MDP_MDDI_SCR_SIZE][MDP_MDDI_SCR_LEN] =
  #endif
{
  NOP,              //0
  NOP,              //1
  NOP,              //2 SET_MDDI_X_LEFT
  NOP,              //3 SET_MDDI_X_RIGHT
  NOP,              //4 SET_MDDI_Y_TOP
  NOP,              //5 SET_MDDI_Y_BOTTOM
  NOP,              //6 SET_MDDI_X_START
  NOP,              //7 SET_MDDI_Y_START
  NOP,              //8 SET_MDDI_PIXEL_COUNT

// MDP1.1 support
  NOP,              //9 SET_MDDI_CLIENT_ID
  NOP,              //10 SET_MDDI_VID_FMT_DESC
  NOP,              //11 SET_MDDI_PIXEL_DATA_ATTR

  RETURN              
};

// VSYNC Script
  #ifdef __GNUC__
uint32 MDP_DISP_VSYNC_SCR[]
__attribute__ ((aligned(16))) =
  #else
__align(16) uint32 MDP_DISP_VSYNC_SCR[] =
  #endif
{
  NOP,            //0
  NOP,            //1
  NOP,            //2 WAIT_LINE_GTE or WAIT_LINE_LT
  NOP,            //3 WAIT_LINE_GTE or WAIT_LINE_LT  
  JUMP,           //4
  NOP,            //5
};

// Power ON/OFF Script - Link to front of script
//   Minimal power settings for each update (not used)
//     GFX: 0x3efffc0f
//     VDO: 0x3efffdef
//     GOG: 0x3efffd0f
//     GOV: 0x3effffff
  #ifdef __GNUC__
uint32 MDP_DISP_POWERON_SCR[]
__attribute__ ((aligned(16))) =
  #else
__align(16) uint32 MDP_DISP_POWERON_SCR[] =
  #endif
{
  NOP,            //0
  NOP,            //1 
  0x3effffff,     //2 Turn on all MDP sub-blocks
  NOP,            //3
  JUMP,           //4
  NULL,           //5 
};

// CCS Script
  #ifdef __GNUC__
uint32 MDP_DISP_CCS_SCR[]
__attribute__ ((aligned (16))) =
  #else
__align(16) uint32 MDP_DISP_CCS_SCR[] =
  #endif
{
  NOP, //0
  NOP, //1
  NOP, //2
  NOP, //3
  NOP, //4
  NOP, //5
  NOP, //6
  NOP, //7
  NOP, //8
  NOP, //9
  NOP, //10
  NOP, //11
  SET_CCS_CCTF(0xC, 0x001), //12
  NOP, //13
  NOP, //14
  HALT //15
};

// GAMMA Script
  #ifdef __GNUC__
uint32 MDP_DISP_GAMMA_SCR[]
__attribute__ ((aligned (16))) =
  #else
__align(16) uint32 MDP_DISP_GAMMA_SCR[] =
  #endif
{
  SUBROUTINE,  //0
  NOP,         //1
  SUBROUTINE,  //2
  NOP,         //3
  SUBROUTINE,  //4
  NOP,         //5
  NOP,         //6
  NOP,         //7
  HALT         //8
};


// MDP Graphic(RGB) Script
  #ifdef __GNUC__
uint32 MDP_DISP_GFX_SCR[MDP_GFX_SCR_SIZE][MDP_GFX_SCR_LEN]
__attribute__ ((aligned (16))) =
  #else
__align(16) uint32 MDP_DISP_GFX_SCR[MDP_GFX_SCR_SIZE][MDP_GFX_SCR_LEN] =
  #endif
{
  NOP,            //0
  NOP,            //1

  SET_GFX_ADDR,   //2
  NULL,           //3 image address
  NULL,           //4 SET_GFX_LINE
  NULL,           //5 16 pixels x 2 bytes
  NULL,           //6 SET_GFX_STRIPE

  NULL,           //7 0:to LCD   8:16bpp
  NULL,           //8 operation - gfx_565 over nothing; gfx and video orient:flip/rotate
  SET_MBC_ACTION(0),      //9 disable mbc operations

  NULL,           //10 Output WQVGA scaled?
  NULL,           //11 : X Crop
  NULL,           //12 : Y Crop
  NULL,           //13 : VDB SIZE

  NULL,           //14 LCD Write Script SubRoutine or memAddr set
  NULL,           //15 address

  SET_MEMORY_MAX,       //16 : Set MAX Memory Boundary
  NULL,           //17 : Memory address

  NOP,            //18 SEND_MDDI_START
  SEND_JOB_BEGIN,       //19
  NOP,            //20
  NOP,            //21
  WAIT_JOB_DONE,        //22

  SET_DEFAULT,    //23 *work-around simultaneous fill&dump
  NOP,            //24
  NOP,            //25
  NOP,            //26
  NOP,            //27
  NOP,            //28
  NOP,            //29
  NOP,            //30
  NOP,            //31
  NOP,            //32
  NOP,            //33

  NOP,            //34 Jump or NOP
  NOP,            //35 Jump Addr or Nop
  NOP,            //36
  0x3e000000,     //37 Turn all clocks off, except the default cgc
  SET_INTR,       //38
  HALT            //39 the script should not run after halt
};

// MDP Graphic over Graphic (RGB/RGB) Script
  #ifdef __GNUC__
uint32 MDP_DISP_GOG_SCR[MDP_GOG_SCR_SIZE][MDP_GOG_SCR_LEN]
__attribute__ ((aligned (16))) =
  #else
__align(16) uint32 MDP_DISP_GOG_SCR[MDP_GOG_SCR_SIZE][MDP_GOG_SCR_LEN] =
  #endif
{
  SET_BKGND_ADDR, //0
  NULL,           //1 bkg image address
  NULL,           //2 SET_BKGND_LINE
  NULL,           //3 16 pixels x 2 bytes
  NULL,           //4 SET_BKGND_STRIPE

  SET_FRGND_ADDR, //5
  NULL,           //6 image address
  NULL,           //7 SET_GFX_LINE
  NULL,           //8 16 pixels x 2 bytes
  NULL,           //9 SET_GFX_STRIPE

  NULL,           //10 0:to LCD   8:16bpp
  NULL,           //11 operation - gfx_565 over gfx_565; gfx and video orient:flip/rotate

  NULL,           //12 SET_MBC_ALPHA
  NULL,           //13 SET_MBC_ACTION

  NULL,           //14 LCD Write Script SubRoutine or memAddr set
  NULL,           //15 address

  NULL,           //16 SET_MBC_TV

  NULL,           //17 Output WQVGA scaled?
  NULL,           //18 : X Crop
  NULL,           //19 : Y Crop
  NULL,           //20 : VDB SIZE
  SET_MEMORY_MAX,       //21 : Set MAX Memory Boundary
  NULL,           //22 : Memory address

  NOP,            //23 SEND_MDDI_START
  SEND_JOB_BEGIN, //24

  NOP,            //25
  NOP,            //26
  NOP,            //27
  WAIT_JOB_DONE,  //28
  SET_DEFAULT,    //29 *work-around simultaneous fill&dump
  NOP,            //30
  NOP,            //31
  NOP,            //32
  NOP,            //33
  NOP,            //34
  NOP,            //35
  NOP,            //36
  NOP,            //37
  NOP,            //38
  NOP,            //39

  NOP,            //40 Jump or NOP
  NOP,            //41 Jump Addr or Nop
  NOP,            //42
  0x3e000000,     //43 Turn all clocks off, except the default cgc
  SET_INTR,       //44
  HALT            //45 the script should not run after halt
};

// MDP Video(YCbCr420) Script
  #ifdef __GNUC__
uint32 MDP_DISP_VDO_SCR[MDP_VDO_SCR_SIZE][MDP_VDO_SCR_LEN]
__attribute__ ((aligned (16))) =
  #else
__align(16) uint32 MDP_DISP_VDO_SCR[MDP_VDO_SCR_SIZE][MDP_VDO_SCR_LEN] =
  #endif
{
/* Y value */
  SET_LUMA_ADDR,  //0
  NULL,           //1 luma start address

  NULL,           //2 Line Jump
  NULL,           //3 Block Jump
  NULL,           //4 Strip Jump

/* CbCr value */
  SET_CHROMA_ADDR,//5
  NULL,           //6 chroma start address
  NULL,           //7 Line Jump
  NULL,           //8 Block Jump
  NULL,           //9 Strip Jump

  NULL, //10
  NULL, //11
  NULL, //12
  NULL, //13
  NULL, //14
  NULL, //15
  NULL, //16
  NULL, //17
  NULL, //18
  NULL, //19
  NULL, //20
  NULL, //21
  NULL, //22

  NULL,           //23 LCD_FORMAT
  NULL,           //24 SET_FFR_MODE

  SET_MBC_ACTION(0),      //25 disable MBC operations

  NULL,            //26 Add WQVGA scaling?
  NOP,            //27
  NOP,            //28
  NOP,            //29

  NULL,           //30 LCD Write Script SubRoutine or memAddr set
  NULL,           //31 address

  NULL,           //32 : X Crop
  NULL,           //33 : Y Crop
  NULL,           //34 : VDB SIZE
  SET_MEMORY_MAX, //35 : Set MAX Memory Boundary
  NULL,           //36 : Memory address

  NOP,            //37 SEND_MDDI_START
  SEND_JOB_BEGIN, //38
  WAIT_JOB_DONE,  //39
  SET_DEFAULT,    //40 *work-around simultaneous fill&dump
  NOP,            //41
  NOP,            //42
  NOP,            //43
  NOP,            //44
  NOP,            //45
  NOP,            //46
  NOP,            //47

  NOP,            //48 Jump or NOP
  NOP,            //49 Jump Addr or Nop
  0x3e000000,     //50 Turn all clocks off, except the default cgc
  SET_INTR,       //51
  HALT            //52
};

// MDP Graphic(RGB) over Video(YCbCr420) Script
  #ifdef __GNUC__
uint32 MDP_DISP_GOV_SCR[MDP_GOV_SCR_SIZE][MDP_GOV_SCR_LEN]
__attribute__ ((aligned (16))) =
  #else
__align(16) uint32 MDP_DISP_GOV_SCR[MDP_GOV_SCR_SIZE][MDP_GOV_SCR_LEN] =
  #endif
{
/* Y value */
  SET_LUMA_ADDR,  //0
  NULL,           //1 luma start address

  NULL,           //2 Line Jump
  NULL,           //3 Block Jump
  NULL,           //4 Strip Jump

/* CbCr value */
  SET_CHROMA_ADDR,      //5
  NULL,           //6 chroma start address
  NULL,           //7 Line Jump
  NULL,           //8 Block Jump
  NULL,           //9 Strip Jump

/* Graphics - Foreground */
  SET_FRGND_ADDR, //10 Fg Image Set
  NULL,           //11 Fg Image start addr
  NULL,           //12 Line Jump
  NULL,           //13 Block Jump
  NULL,           //14 Strip Jump

  NULL, //15
  NULL, //16
  NULL, //17
  NULL, //18
  NULL, //19
  NULL, //20
  NULL, //21
  NULL, //22
  NULL, //23
  NULL, //24
  NULL, //25
  NULL, //26
  NULL, //27

  NULL,           //28 LCD_FORMAT
  NULL,           //29 SET_FFR_MODE

  NULL,           //30 LCD Write Script SubRoutine or memAddr set
  NULL,           //31 address

  NULL,           //32 SET_MBC_ALPHA
  NULL,           //33 SET_MBC_ACTION
  NULL,           //34 SET MBC TP

  NULL,            //35 Turn On WQVGA scaling?

  NULL,           //36 : X Crop
  NULL,           //37 : Y Crop
  NULL,           //38 : VDB SIZE
  SET_MEMORY_MAX, //39 : Set MAX Memory Boundary
  NULL,           //40 : Memory address

  NOP,            //41 SEND_MDDI_START
  SEND_JOB_BEGIN, //42
  WAIT_JOB_DONE,  //43
  SET_DEFAULT,    //44 *work-around simultaneous fill&dump
  NOP,            //45
  NOP,            //46
  NOP,            //47
  NOP,            //48
  NOP,            //49
  NOP,            //50
  NOP,            //51
  NOP,            //52
  NOP,            //53
  NOP,            //54

  NOP,            //55 Jump or NOP
  NOP,            //56 Jump Addr or Nop
  0x3e000000,     //57 Turn all clocks off, except the default cgc
  SET_INTR,       //58
  HALT            //59
};

extern MDPIMG_LIST mdpImgList[];
extern MDPIMG_LIST * mdpImgTailPtr_1[];
extern MDPIMG_LIST * mdpImgTailPtr_2[];

extern boolean mdp_dest_power_on[];

uint32 mdp_total_vdopkts;
uint32 mdp_rev;

/*===========================================================================
    LOCAL FUNCTIONS
============================================================================*/
/*===========================================================================

FUNCTION mdp_set_gamma()

DESCRIPTION
  This function is to enable and configure MDP gamma table.

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  Display color (RGB) will be get corrected through MDP gamma table.
===========================================================================*/

void mdp_set_gamma(float gammaR, float gammaG, float gammaB)
{
  uint16 count;
  float count_temp;
  uint16 gamma_tempR, gamma_tempG, gamma_tempB;

  rex_enter_crit_sect(&mdp_crit_sect);

  mdp_gamma_enable = 1;
  GAMMA_R[GAMMA_TABLE_SIZE] = RETURN;  // Return Instruction for the subroutine
  GAMMA_G[GAMMA_TABLE_SIZE] = RETURN;  // Return Instruction for the subroutine
  GAMMA_B[GAMMA_TABLE_SIZE] = RETURN;  // Return Instruction for the subroutine

  for (count = 0; count < GAMMA_TABLE_SIZE; count++)
  {
    count_temp = (float) count/64;
    count_temp = (pow(count_temp, gammaR)) * 64;  // Normalizing 0 to 3F in (0,1)
    gamma_tempR = (uint16) floor(count_temp);
    if (gamma_tempR > 0x3F)
      gamma_tempR = 0x3F;  // Saturation

    count_temp = (float) count/64;
    count_temp = (pow(count_temp, gammaG)) * 64;  // Normalizing 0 to 3F in (0,1)
    gamma_tempG = (uint16) floor(count_temp);
    if (gamma_tempG > 0x3F)
      gamma_tempG = 0x3F;  // Saturation

    count_temp = (float) count/64;
    count_temp = (pow(count_temp, gammaB)) * 64;  // Normalizing 0 to 3F in (0,1)
    gamma_tempB = (uint16) floor(count_temp);
    if (gamma_tempB > 0x3F)
      gamma_tempB = 0x3F;  // Saturation

    GAMMA_R[count] = SET_MBC_GAMMA(4/*R,G,B*/, count/*address*/, gamma_tempR/*value*/);
    GAMMA_G[count] = SET_MBC_GAMMA(2/*R,G,B*/, count/*address*/, gamma_tempG/*value*/);
    GAMMA_B[count] = SET_MBC_GAMMA(1/*R,G,B*/, count/*address*/, gamma_tempB/*value*/);
  } 

  //initializing YCbCr Color Coefficient Matrix    
  outpdw(MDP_SCRIPT_ADDR, MDP_DISP_GAMMA_SCR);
  outpdw(MDP_COMMAND, BEGIN_SCRIPT);      
  while (inpdw(MDP_SCRIPT_STATUS)&0x1);  

  rex_leave_crit_sect(&mdp_crit_sect);  
}

/*===========================================================================

FUNCTION mdp_clear_ovimg_sub()

DESCRIPTION
  This function is to clear the ovimg on the screen.

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  If other layer doesn't contain an overlapped image, then it clears the region
  with white color

===========================================================================*/
void mdp_clear_ovimg_sub( MDPIMG_LIST *node)
{
  MDPIMG clsImg;

  clsImg.dest     = node->orig_img.dest;
  clsImg.imgType  = MDP_RGB565;
  clsImg.bmy_addr = (uint32 *)(white_frame->buffer);
  clsImg.width    = white_frame->pxWidth;

  clsImg.cwin.x1 = 0;
  clsImg.cwin.x2 = node->img.cwin.x2-node->img.cwin.x1;
  clsImg.cwin.x2 = (clsImg.cwin.x2 >= clsImg.width)?(clsImg.width - 1):clsImg.cwin.x2;
  clsImg.cwin.y1 = 0;
  clsImg.cwin.y2 = node->img.cwin.y2-node->img.cwin.y1;

  clsImg.lcd_x = node->orig_img.lcd_x;
  clsImg.lcd_y = node->orig_img.lcd_y;
  clsImg.mdpOp = MDPOP_TRANSP | MDPOP_DISABLE_ALPHAB;
  clsImg.tpVal = MDP_WB_CLR;
  clsImg.layer = node->orig_img.layer;

  #ifdef FEATURE_MDP_WSCALE
  if(node->orig_img.mdpOp & MDPOP_WSCALE_LINEAR_STRETCH)
    clsImg.mdpOp |= MDPOP_WSCALE_LINEAR_STRETCH;
  else if(node->orig_img.mdpOp & MDPOP_WSCALE_EDGE_STRETCH)
    clsImg.mdpOp |= MDPOP_WSCALE_EDGE_STRETCH;
  else if(node->orig_img.mdpOp & MDPOP_WSCALE_NO_STRETCH)
    clsImg.mdpOp |= MDPOP_WSCALE_NO_STRETCH;
  #endif /* FEATURE_MDP_WSCALE */
  
  mdp_disp_update(&clsImg);
}


/*===========================================================================

FUNCTION mdp_touch_img()

DESCRIPTION
  This function is to translate user mdpImg into driver mdpImg

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdp_touch_img(MDPIMG_LIST *node)
{
  uint32 tpVal;

  // it's safe to assume node pointer is valid
  if (node->img.bmy_addr == NULL)
  {
    node->img.imgType = MDP_RGB565;
  }

  // from logical to real x-y coordinate conversion
  mdp_get_real_xy(node);

  if (node->img.imgType ==  MDP_RGB565)
  {
    tpVal = node->img.tpVal;
    // transparent color conversion into 24 bpp
    node->img.tpVal = MDP_CONVTP(tpVal);
  }
}

/*===========================================================================

FUNCTION mdp_verify_dispArea()

DESCRIPTION
  This function is to verify whether the specified mdpImg display region
  intersects with LCD region

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  Returns MDP_SUCCESSFUL if valid, otherwise returns MDP_FAIL

SIDE EFFECTS
  None

===========================================================================*/
uint32 mdp_verify_dispArea(MDPIMG *mdpImg)
{
  return mdpPeekOverXYInt(mdpImg->cwin.x1,
                          mdpImg->cwin.x2,
                          mdpImg->cwin.y1,
                          mdpImg->cwin.y2,
                          0,
                          mdpLcdSize[mdpImg->dest][COL_IDX],
                          0,
                          mdpLcdSize[mdpImg->dest][ROW_IDX]);
}


/*===========================================================================

FUNCTION mdp_sub_layer_update()

DESCRIPTION
  This function is to copy layer 1 image into the sublayer buffer.

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  Returns the registration ID if successful, otherwise returns NULL

SIDE EFFECTS
  None

===========================================================================*/
void mdp_sub_layer_update(MDPIMG *mdpImg, MDPIMG_LIST *node)
{
  MDPIMG_LIST *mdpSubNodePtr, **mdpImgTailPtr, *mdpListPtrTemp;
  
  uint32 tpVal;
  uint16 *buf, *L1buf;
  uint32 bufSzBytes, L1bufSzBytes, bufClearVal;
  

  rex_enter_crit_sect(&mdp_crit_sect);
  if (mdpImg->dest == PRIMARY_LCD_TYPE)
  {
    mdpSubNodePtr = mdpSubNode;
    buf           = (uint16*)(mdp_slBuf_1->buffer);
    L1buf         = (uint16*)(mdp_L1buf_1->buffer);
    mdpImgTailPtr =  mdpImgTailPtr_1;

    // Set-up values for mdp_clearLayer565 call below.
    bufSzBytes   = mdp_slBuf_1->byteSize;
    L1bufSzBytes = mdp_L1buf_1->byteSize;
    bufClearVal  = mdpSubNode->orig_img.tpVal;
  }
  else // Default to secondary if not for primary LCD...
  {
    mdpSubNodePtr = mdpSubNode2;
    buf           = (uint16*)(mdp_slBuf_2->buffer);
    L1buf         = (uint16*)(mdp_L1buf_2->buffer);
    mdpImgTailPtr =  mdpImgTailPtr_2;

    // Set-up values for mdp_clearLayer565 call below.
    bufSzBytes   = mdp_slBuf_2->byteSize;
    L1bufSzBytes = mdp_L1buf_2->byteSize;
    bufClearVal  = mdpSubNode2->orig_img.tpVal;
  }

  tpVal = (mdpImg->mdpOp&MDPOP_TRANSP) ? mdpImg->tpVal:MDP_WB_CLR;
  mdpSubNodePtr->orig_img.tpVal = tpVal;
  mdpSubNodePtr->img.tpVal = MDP_CONVTP(tpVal);

  if (mdpImg->mdpOp&MDPOP_ALPHAB)
  {
    mdpSubNodePtr->img.alpha = mdpImg->alpha;
    mdpSubNodePtr->orig_img.alpha = mdpImg->alpha;
    mdpSubNodePtr->orig_img.mdpOp |= MDPOP_ALPHAB;
    mdpSubNodePtr->mbcOp |= MDPOP_ALPHAB;
  }
  else
  {
    mdpSubNodePtr->orig_img.mdpOp &= ~MDPOP_ALPHAB;
    mdpSubNodePtr->mbcOp &= ~MDPOP_ALPHAB;
  }

  // Clear (to transparency color) the buffers associated
  //   with Layer 1 updates.  Buffers are assumed to be RGB565.
  mdp_clearLayer565(buf,
                    bufSzBytes,
                    bufClearVal);

  mdp_clearLayer565(L1buf,
                    L1bufSzBytes,
                    bufClearVal);

  mdpListPtrTemp = mdpImgTailPtr[MDP_LAYER_2];
  mdpImgTailPtr[MDP_LAYER_2] = NULL;
  mdp_disp_update_sub(mdpImg, TRUE, L1buf, L1bufSzBytes, NULL);
  mdpImgTailPtr[MDP_LAYER_2] = mdpListPtrTemp;


  node->img = mdpSubNodePtr->img;
  node->orig_img = mdpSubNodePtr->orig_img;
  node->mbcOp = mdpSubNodePtr->mbcOp;

  node->img.bmy_addr = node->orig_img.bmy_addr = (uint32 *)L1buf;
  node->img.layer = node->orig_img.layer = MDP_LAYER_0;
  mdp_disp_update_sub(&node->orig_img, TRUE, buf, bufSzBytes, NULL);
  node->img.layer = node->orig_img.layer = MDP_LAYER_1;
  rex_leave_crit_sect(&mdp_crit_sect);  
}

/*===========================================================================

FUNCTION mdp_start_script()

DESCRIPTION
  This function first clears MDP task signal and prevent sleep task from
  halting bus clock.  And then, it send the first script pointer to MDP HW.

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mdp_start_script(uint32 *scrPtr,
                      LCD_TYPE destDev,
                      boolean addVsyncScript,
                      MDP_CWIN vsyncCwin)
{
  // while MDP is still in processing...
  while (((inpdw(MDP_SCRIPT_STATUS) & 0x1)) || (mdp_in_processing))
    rex_sleep(1);  //task sleep for 1 msec 

  MDP_MSG_DEBUG("MDP Start Script Event",0,0,0);

  // Add power up script, before vsync to reduce power waste
  MDP_DISP_POWERON_SCR[5] = (uint32)scrPtr;
  scrPtr = &MDP_DISP_POWERON_SCR[0];

// Calculate vsync wait time if enabled and asked for...
// Add vsync script if enable and asked for...
  if ((addVsyncScript == TRUE) &&    /* Do vsync for this update? */
      (mdp_vsync_enabled == TRUE) && /* Global flag */
    (destDev == PRIMARY_LCD_TYPE)) /* MDP only has one vsync, use for PRIM */
  {
  
    // Modify the vsync script for this update
    #ifdef MDP_HW_VSYNC
    mdp_modify_vsync_script(vsyncCwin);
     // Link the script in...
    MDP_DISP_VSYNC_SCR[5] = (uint32)scrPtr;
    scrPtr = &MDP_DISP_VSYNC_SCR[0];
    #elif defined(MDP_SW_VSYNC) 	
    wait_time = mdp_get_vsync_wait_time(vsyncCwin);
    #endif
	
  }

#ifdef FEATURE_MDDI
#error code not present
#endif /* FEATURE_MDDI */

  /* Start the MDP debug bus to output the MISR CRC? */
  if (mdp_debug_misrCrc)
  {
    if (mdp_is_dest_mddi(destDev))
    {
     // write 0x300 to MDP_TEST_MODE -> Set MDDI MISR CRC mode
     outpw(MDP_DBGBUS_REG_CFG, MDP_DBGBUS_MDDI_MISR);
    }
    else
    {
     // write 0x182 to MDP_TEST_MODE -> Set EBI2 MISR CRC mode
     outpw(MDP_DBGBUS_REG_CFG, MDP_DBGBUS_EBI2_MISR);
    }
  
     // write 0 to MDP_TEST_MISR_RESET
     outpw(MDP_DBGBUS_TEST_MISR_RESET, 0);
    
     // write 0 to MDP_TEST_EXPORT_MISR
     outpw(MDP_DBGBUS_TEST_EXPORT_MISR, 0);
  }
#ifdef MDP_SW_VSYNC

  if ((addVsyncScript == TRUE) &&    /* Do vsync for this update? */
      (mdp_vsync_enabled == TRUE)) /* Global flag */	
  {
     int dog_autokick;

     // watchdog auto tick during rex_sleep()
     dog_autokick = rex_autodog_enable(rex_self()->dog_report_val);
   
     /* add 1 millisecond to wait time to get the ceiling*/ 
     wait_time =(wait_time/1000) + 1;
	 if(wait_time > 17)
	 {
	    /*Wait time should not be greater than 1/60ms for 60Hz or higher freq.LCD*/
		wait_time = 17;
		MDP_MSG_HIGH("Too Long Vsync Wait Time - Please Check Carefully",0,0,0);
	 }
	 /*Need to sleep before firing the next update, inorder to avoid tearing*/
     rex_sleep(wait_time); 

	 if (!dog_autokick)
	 {
      rex_autodog_disable();
	 }

     if(mdp_debug_vsync)
	 {
        MDP_MSG_HIGH("wait_time time is %u", wait_time, 0, 0);
	 }

  }
 #endif
     mdp_tcb_ptr = rex_self();
     rex_def_timer(&mdp_rex_timer, mdp_tcb_ptr, mdp_task_wait_sig);
     /* expires in 100 msec */
     rex_set_timer(&mdp_rex_timer, mdp_exp_time);

     // if software vsync is disabled, kick the hardware right away
     // tearing prevention logic is skipped
     mdp_hclk_active();
      
     mdp_sum_job_cnt++;//keeps a count of how many times the timer expires.
     mdp_in_processing = TRUE;
     mdp_scr_started = TRUE;
     TASKLOCK();
     mdp_hw_start_timestamp_ms = timetick_get_ms();
     //reset the hardware update time and start time
     mdp_dma2_update_time_in_usec = 0;
     mdp_dma2_start_time_in_usec = timetick_get();
      // set the script starting address
     outpdw(MDP_SCRIPT_ADDR, scrPtr);
     // start fetching and operating the script
     outpdw(MDP_COMMAND, BEGIN_SCRIPT);
     TASKFREE();
}

/*===========================================================================

FUNCTION mdp_wait_script_done()

DESCRIPTION
  This function is to block calling task until the previously started script
  gets completed by MDP HW or rex timer gets expired

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdp_wait_script_done()
{
  uint32 regVal;
  static int totStateErrors = 0;
  int resetFlag = 0;

  if (!mdp_scr_started)
    return;

  mdp_scr_started = FALSE;
  //MDP_MSG_ERROR("mdp_wait_script_done start ****",0,0,0);
  if (!mdp_disp_fatal_err_flag)
  {
    int dog_autokick;

    // watchdog auto tick during rex_wait()
    dog_autokick = rex_autodog_enable(rex_self()->dog_report_val);
    rex_wait(mdp_task_wait_sig);

    if (!dog_autokick)
    {
      rex_autodog_disable();
    }

    rex_clr_timer(&mdp_rex_timer);
  
    /* Get the CRC for last update */
    if ((mdp_debug_misrCrc) && (mdp_rev != MDP_REV_01_00))
    {
      //  Read the CRC value
      regVal = inpdw(MDP_DBGBUS_TEST_MISR_REG);
  
      // Zero the debug bus, don't drive while not using
      outpw(MDP_DBGBUS_REG_CFG, 0);
  
      //  Output to QXDM
      MDP_MSG_MED("MDP MISR CRC = 0x%08x", regVal, 0, 0);
    }
    
    /* Check the MDP end state */
    if ((mdp_debug_compState) && (mdp_rev != MDP_REV_01_00))
    {
      // Get the state
      regVal = mdp_read_debug_bus(MDP_DBGBUS_SEL_MISR) & MDP_BIT_STATE_MASK;
  
      // Output to QXDM
      if (regVal != 0)
      {
        totStateErrors++;
        MDP_MSG_ERROR("MDP Completion Error = %d, MDP State = 0x%x ", totStateErrors, regVal, 0);
        resetFlag = 1;
      }
    }

	//MDP_MSG_ERROR("mdp_wait_script_done mid ==%d ****",inpdw(MDP_SCRIPT_STATUS)&0x1,0,0);

    // if MDP is still in processing after time-out...
    if (inpdw(MDP_SCRIPT_STATUS)&0x1)
    {
      MDP_MSG_ERROR("ERROR!!! MDP Freeze() from mdp_wait_script_done() ****",0,0,0);
      resetFlag = 1;
    }
	//MDP_MSG_ERROR("mdp_wait_script_done end ==%d ****",resetFlag,0,0);

   if(mdp_debug_vsync || mdp_debug_calcFps)
    {
      MDP_MSG_HIGH("MDP HW Update Time = %d msec",
      mdp_hw_finish_timestamp_ms-mdp_hw_start_timestamp_ms,0,0);
    }    
  }
  else
  {
    MDP_MSG_HIGH("WARNING - MDP operating in polling mode",0,0,0);

    // while MDP is still in processing...
    while ((inpdw(MDP_SCRIPT_STATUS) & 0x1));
    mdp_hclk_not_active();
  }

  if (resetFlag != 0)
  {
    mdp_reset();
  }

  mdp_tcb_ptr = NULL;
  mdp_in_processing = FALSE;
  mdp_user_cb.cb_func = FALSE;
}


/*===========================================================================

FUNCTION mdp_read_debug_bus()

DESCRIPTION
  Read the specified value from the MDP debug bus.  For internal use only.

DEPENDENCIES
  NONE

RETURN VALUE
  The 32 bit register value.

SIDE EFFECTS
  Actively drives the debug bus, may interfere with other debug bus access
  as well as drive shared debug output.

===========================================================================*/

static uint32 mdp_read_debug_bus(uint32 dbgCode)
{
  uint32 origCode;
  uint32 regVal;

  // get the current debug bus setting to restore after
  origCode = inpdw(MDP_DBGBUS_REG_CFG);

  // set the debug bus to appropriate code
  outpdw(MDP_DBGBUS_REG_CFG, dbgCode);

  // read back the value
  regVal = inpdw(MDP_DBGBUS_REG_VAL);

  // restore the original debug bus value
  outpdw(MDP_DBGBUS_REG_CFG, origCode);

  return regVal;
}
/*===========================================================================

FUNCTION mdp_hclk_active()

DESCRIPTION
  Signal the clock manager that the hclk is needed.  This may have a target
  specific call.  Default to vote for Hclk not to sleep.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mdp_hclk_active(void)
{
  /* Prevent TCXO shutdown for 6280/6260 */
  MDP_VOTE_FOR_MIN_BUS();

  /* Prevent TCXO shutdown for 6550/6800 */ 
#ifdef FEATURE_NEW_SLEEP_API
  sleep_negate_okts(mdp_sleep_handle);
#else
  SLEEP_NEGATE_OKTS(SLEEP_MDP_OKTS_SIG);
#endif /* FEATURE_NEW_SLEEP_API */

}

/*===========================================================================

FUNCTION mdp_hclk_not_active()

DESCRIPTION
  Signal the clock manager that the hclk is not needed.  This may have a
  target specific call.  Default to vote for Hclk to sleep.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mdp_hclk_not_active(void)
{
  /* Allow any clock speed change when MDP HW is inactive */
#ifdef FEATURE_NEW_SLEEP_API
  sleep_assert_okts(mdp_sleep_handle);
#else
  SLEEP_ASSERT_OKTS(SLEEP_MDP_OKTS_SIG);
#endif /* FEATURE_NEW_SLEEP_API */

  MDP_RELEASE_MIN_BUS();
}


/*===========================================================================

FUNCTION mdp_scrlist_init()

DESCRIPTION
  This function is to initialize MDP script list and associated data structures

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdp_scrlist_init()
{
  int i, j;

  mdp_scr_give[MDP_GFX_SCR] = 0;
  mdp_scr_give[MDP_GOG_SCR] = 0;
  mdp_scr_give[MDP_VDO_SCR] = 0;
  mdp_scr_give[MDP_GOV_SCR] = 0;
#ifdef CUST_EDITION
  mdp_scr_give[MDP_LCD_SCR] = 0;
#else
  mdp_scr_give[MDP_EPSON_QCIF_SCR] = 0;
  mdp_scr_give[MDP_EPSON_QVGA_SCR] = 0;
  mdp_scr_give[MDP_TMD_QVGA_SCR] = 0;
  mdp_scr_give[MDP_EP_OP_QCIFP_PRI_SCR] = 0;
  mdp_scr_give[MDP_EP_OP_QCIFP_SEC_SCR] = 0;
#endif
#ifdef FEATURE_MDDI
#error code not present
#endif /* FEATURE_MDDI */

  mdp_scr_lock[MDP_GFX_SCR] = MDP_GFX_SCR_SIZE-1;
  mdp_scr_lock[MDP_GOG_SCR] = MDP_GOG_SCR_SIZE-1;
  mdp_scr_lock[MDP_VDO_SCR] = MDP_VDO_SCR_SIZE-1;
  mdp_scr_lock[MDP_GOV_SCR] = MDP_GOV_SCR_SIZE-1;
#ifdef CUST_EDITION
  mdp_scr_lock[MDP_LCD_SCR] = MDP_LCD_SCR_SIZE-1;
#else
  mdp_scr_lock[MDP_EPSON_QCIF_SCR] = MDP_EPSON_QCIF_SCR_SIZE-1;
  mdp_scr_lock[MDP_EPSON_QVGA_SCR] = MDP_EPSON_QVGA_SCR_SIZE-1;
  mdp_scr_lock[MDP_TMD_QVGA_SCR] = MDP_TMD_QVGA_SCR_SIZE-1;
  mdp_scr_lock[MDP_EP_OP_QCIFP_PRI_SCR] = MDP_EP_OP_QCIFP_PRI_SCR_SIZE-1;
  mdp_scr_lock[MDP_EP_OP_QCIFP_SEC_SCR] = MDP_EP_OP_QCIFP_SEC_SCR_SIZE-1;
#endif
#ifdef FEATURE_MDDI
#error code not present
#endif /* FEATURE_MDDI */

#ifdef CUST_EDITION
{
  uint32 *pScr;
  (void)disp_drv_mdp_getscr(&pScr);
  for (j=1;j<MDP_LCD_SCR_SIZE;j++)
  {
    for (i=0;i<MDP_LCD_SCR_LEN;i++)
    {
      pScr[j*MDP_LCD_SCR_LEN+i] = pScr[i];
    }
  }
}
#else
  for (j=1;j<MDP_EPSON_QCIF_SCR_SIZE;j++)
  {
    for (i=0;i<MDP_EPSON_QCIF_SCR_LEN;i++)
    {
      MDP_DISP_EPSON_QCIF_SCR[j][i] = MDP_DISP_EPSON_QCIF_SCR[0][i];
    }
  }
  
  for (j=1;j<MDP_EPSON_QVGA_SCR_SIZE;j++)
  {
    for (i=0;i<MDP_EPSON_QVGA_SCR_LEN;i++)
    {
      MDP_DISP_EPSON_QVGA_SCR[j][i] = MDP_DISP_EPSON_QVGA_SCR[0][i];
    }
  }
  
  for (j=1;j<MDP_TMD_QVGA_SCR_SIZE;j++)
  {
    for (i=0;i<MDP_TMD_QVGA_SCR_LEN;i++)
    {
      MDP_DISP_TMD_QVGA_SCR[j][i] = MDP_DISP_TMD_QVGA_SCR[0][i];
    }
  }

  for (j=1;j<MDP_EP_OP_QCIFP_PRI_SCR_SIZE;j++)
  {
    for (i=0;i<MDP_EP_OP_QCIFP_PRI_SCR_LEN;i++)
    {
      MDP_DISP_EP_OP_QCIFP_PRI_SCR[j][i] = MDP_DISP_EP_OP_QCIFP_PRI_SCR[0][i];
    }
  }

  for (j=1;j<MDP_EP_OP_QCIFP_SEC_SCR_SIZE;j++)
  {
    for (i=0;i<MDP_EP_OP_QCIFP_SEC_SCR_LEN;i++)
    {
      MDP_DISP_EP_OP_QCIFP_SEC_SCR[j][i] = MDP_DISP_EP_OP_QCIFP_SEC_SCR[0][i];
    }
  }
#endif

#ifdef FEATURE_MDDI
#error code not present
#endif /* FEATURE_MDDI */

  for (j=1;j<MDP_GFX_SCR_SIZE;j++)
  {
    for (i=0;i<MDP_GFX_SCR_LEN;i++)
    {
      MDP_DISP_GFX_SCR[j][i] = MDP_DISP_GFX_SCR[0][i];
    }
  }

  for (j=1;j<MDP_GOG_SCR_SIZE;j++)
  {
    for (i=0;i<MDP_GOG_SCR_LEN;i++)
    {
      MDP_DISP_GOG_SCR[j][i] = MDP_DISP_GOG_SCR[0][i];
    }
  }

  for (j=1;j<MDP_VDO_SCR_SIZE;j++)
  {
    for (i=0;i<MDP_VDO_SCR_LEN;i++)
    {
      MDP_DISP_VDO_SCR[j][i] = MDP_DISP_VDO_SCR[0][i];
    }
  }


  for (j=1;j<MDP_GOV_SCR_SIZE;j++)
  {
    for (i=0;i<MDP_GOV_SCR_LEN;i++)
    {
      MDP_DISP_GOV_SCR[j][i] = MDP_DISP_GOV_SCR[0][i];
    }
  }

}


/*===========================================================================

FUNCTION mdp_scr_get()

DESCRIPTION
  This function is to have a free script from the specified script queue.
  All script queue moves as a circular buffer.

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  Script pointer if successful otherwise NULL

SIDE EFFECTS
  None

===========================================================================*/
uint32 * mdp_scr_get(int q)
{
  uint32 *scrPtr = NULL;
  uint16 max_q_size = 0;

  if (mdp_scr_give[q] == mdp_scr_lock[q])
  {
    return NULL;
  }

  switch (q)
  {
    case MDP_GFX_SCR:
      scrPtr = &MDP_DISP_GFX_SCR[mdp_scr_give[q]][0];
      max_q_size = MDP_GFX_SCR_SIZE;
      break;

    case MDP_GOG_SCR:
      scrPtr = &MDP_DISP_GOG_SCR[mdp_scr_give[q]][0];
      max_q_size = MDP_GOG_SCR_SIZE;
      break;

    case MDP_VDO_SCR:
      scrPtr = &MDP_DISP_VDO_SCR[mdp_scr_give[q]][0];
      max_q_size = MDP_VDO_SCR_SIZE;
      break;

    case MDP_GOV_SCR:
      scrPtr = &MDP_DISP_GOV_SCR[mdp_scr_give[q]][0];
      max_q_size = MDP_GOV_SCR_SIZE;
      break;

#ifdef CUST_EDITION
    case MDP_LCD_SCR:
      max_q_size = disp_drv_mdp_getscr(&scrPtr);
      scrPtr = scrPtr+(mdp_scr_give[q]*MDP_LCD_SCR_LEN);
      break;
#else
    case MDP_EPSON_QCIF_SCR:
      scrPtr = &MDP_DISP_EPSON_QCIF_SCR[mdp_scr_give[q]][0];
      max_q_size = MDP_EPSON_QCIF_SCR_SIZE;
      break;

    case MDP_EPSON_QVGA_SCR:
      scrPtr = &MDP_DISP_EPSON_QVGA_SCR[mdp_scr_give[q]][0];
      max_q_size = MDP_EPSON_QVGA_SCR_SIZE;
      break;

   case MDP_TMD_QVGA_SCR:
      scrPtr = &MDP_DISP_TMD_QVGA_SCR[mdp_scr_give[q]][0];
      max_q_size = MDP_TMD_QVGA_SCR_SIZE;
      break;

    case MDP_EP_OP_QCIFP_PRI_SCR:
      scrPtr = &MDP_DISP_EP_OP_QCIFP_PRI_SCR[mdp_scr_give[q]][0];
      max_q_size = MDP_EP_OP_QCIFP_PRI_SCR_SIZE;
      break;

    case MDP_EP_OP_QCIFP_SEC_SCR:
      scrPtr = &MDP_DISP_EP_OP_QCIFP_SEC_SCR[mdp_scr_give[q]][0];
      max_q_size = MDP_EP_OP_QCIFP_SEC_SCR_SIZE;
      break;
#endif
#ifdef FEATURE_MDDI
#error code not present
#endif /* FEATURE_MDDI */

    default: return NULL;
  }

  if (++mdp_scr_give[q] >= max_q_size)
  {
    mdp_scr_give[q] = 0;
  }

  return scrPtr;
}

/*===========================================================================

FUNCTION mdp_reset_scrcnt()

DESCRIPTION
  This function is to clear all script counter in msc

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdp_reset_scrcnt(MDP_SCRCNT *msc)
{
  msc->gfx_scr_cnt = 0;
  msc->gog_scr_cnt = 0;
  msc->vdo_scr_cnt = 0;
  msc->gov_scr_cnt = 0;
#ifdef CUST_EDITION 
  msc->lcd_scr_cnt = 0;
#else
  msc->epson_qcif_scr_cnt = 0;
  msc->epson_qvga_scr_cnt = 0;
  msc->tmd_qvga_scr_cnt = 0;
  msc->ep_op_qcifp_pri_scr_cnt = 0;
  msc->ep_op_qcifp_sec_scr_cnt = 0;
#endif
#ifdef FEATURE_MDDI
#error code not present
#endif /* FEATURE_MDDI */
}

/*===========================================================================

FUNCTION mdp_scr_free()

DESCRIPTION
  This function is to return scripts back to the script queue

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdp_scr_free(MDP_SCRCNT *msc)
{
  int i;

  for (i=0;i< msc->gfx_scr_cnt;i++)
  {
    if (++mdp_scr_lock[MDP_GFX_SCR] >= MDP_GFX_SCR_SIZE)
    {
      mdp_scr_lock[MDP_GFX_SCR] = 0;
    }
  }

  for (i=0;i< msc->gog_scr_cnt;i++)
  {
    if (++mdp_scr_lock[MDP_GOG_SCR] >= MDP_GOG_SCR_SIZE)
    {
      mdp_scr_lock[MDP_GOG_SCR] = 0;
    }
  }

  for (i=0;i< msc->vdo_scr_cnt;i++)
  {
    if (++mdp_scr_lock[MDP_VDO_SCR] >= MDP_VDO_SCR_SIZE)
    {
      mdp_scr_lock[MDP_VDO_SCR] = 0;
    }
  }

  for (i=0;i< msc->gov_scr_cnt;i++)
  {
    if (++mdp_scr_lock[MDP_GOV_SCR] >= MDP_GOV_SCR_SIZE)
    {
      mdp_scr_lock[MDP_GOV_SCR] = 0;
    }
  }

#ifdef CUST_EDITION
  for (i=0;i< msc->lcd_scr_cnt;i++)
  {
    if (++mdp_scr_lock[MDP_LCD_SCR] >= MDP_LCD_SCR_SIZE)
    {
      mdp_scr_lock[MDP_LCD_SCR]  = 0;
    }
  }
#else
  for (i=0;i< msc->epson_qcif_scr_cnt;i++)
  {
    if (++mdp_scr_lock[MDP_EPSON_QCIF_SCR] >= MDP_EPSON_QCIF_SCR_SIZE)
    {
      mdp_scr_lock[MDP_EPSON_QCIF_SCR]  = 0;
    }
  }

  for (i=0;i< msc->epson_qvga_scr_cnt;i++)
  {
    if (++mdp_scr_lock[MDP_EPSON_QVGA_SCR] >= MDP_EPSON_QVGA_SCR_SIZE)
    {
      mdp_scr_lock[MDP_EPSON_QVGA_SCR]  = 0;
    }
  }


  for (i=0;i< msc->tmd_qvga_scr_cnt;i++)
  {
    if (++mdp_scr_lock[MDP_TMD_QVGA_SCR] >= MDP_TMD_QVGA_SCR_SIZE)
    {
      mdp_scr_lock[MDP_TMD_QVGA_SCR]  = 0;
    }
  }

  for (i=0;i< msc->ep_op_qcifp_pri_scr_cnt;i++)
  {
    if (++mdp_scr_lock[MDP_EP_OP_QCIFP_PRI_SCR] >= MDP_EP_OP_QCIFP_PRI_SCR_SIZE)
    {
      mdp_scr_lock[MDP_EP_OP_QCIFP_PRI_SCR]  = 0;
    }
  }

  for (i=0;i< msc->ep_op_qcifp_sec_scr_cnt;i++)
  {
    if (++mdp_scr_lock[MDP_EP_OP_QCIFP_SEC_SCR] >= MDP_EP_OP_QCIFP_SEC_SCR_SIZE)
    {
      mdp_scr_lock[MDP_EP_OP_QCIFP_SEC_SCR]  = 0;
    }
  }
#endif
#ifdef FEATURE_MDDI
#error code not present
#endif /* FEATURE_MDDI */
  mdp_reset_scrcnt(msc);
}

/*===========================================================================

FUNCTION disp_cls_mdp()

DESCRIPTION
  This function is to clear entire LCD screen

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void disp_cls_mdp(uint8 dst)
{
  if (dst == PRIMARY_LCD_TYPE)
  {
    mdp_cls(dst, 0, 0, _primary_lcd.disp_width, _primary_lcd.disp_height);
  }
  else
  {
    mdp_cls(dst, 0, 0, _secondary_lcd.disp_width, _secondary_lcd.disp_height);
  }
}

/*===========================================================================

FUNCTION mdp_cls()

DESCRIPTION
  This function is to clear the specified region of LCD screen

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdp_cls(LCD_TYPE dest, uint16 x, uint16 y, uint16 width, uint16 height)
{
  mdp_disp_update_gfx(dest,
                      (uint32 *)(white_frame->buffer),
                      MDP_RGB_565,
                      white_frame->pxWidth,
                      0,
                      width-1,
                      0,
                      height-1,
                      x,y,
                      MDPOP_NOP
                     );
}

/*===========================================================================

FUNCTION mdp_mddi_sub_cal()

DESCRIPTION
  This function is to calculate the maximum MDDI display sub-region

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  MDP_SUCCESSFUL if successful otherwise MDP_FAIL

SIDE EFFECTS
  None

===========================================================================*/
int mdp_mddi_sub_cal(MDP_OVER_XYInt *fgXY,
                     MDP_SUBWIN_MDDI *fgSubMddi, int index,
                     uint32 lineNum, int bypass, uint32 alignOffset)
{
  int lineOffset, firstOffset, maxIndex = 100;
  uint32 nextPxAreaUpdate, nextPxWidth;

  // mdp_mddi_sub_cal() is used in many while loops which will not terminate
  //   until this function returns MDP_FAIL.  Put a sanity check in here and
  //   report the failure.
  if (index > maxIndex)
  {
    MDP_MSG_ERROR("Forced failure in MDP_MDDI_SUB_CAL()",0,0,0);
    return MDP_FAIL;
  }

  if (bypass == TRUE)
  {
    if (index > 0)
      return MDP_FAIL;

    fgSubMddi->lcdX = fgXY->x1;
    fgSubMddi->lcdY = fgXY->y1;

    fgSubMddi->x1 = fgXY->x1;
    fgSubMddi->x2 = fgXY->x2;

    fgSubMddi->y1 = fgXY->y1;
    fgSubMddi->y2 = fgXY->y2;

    return MDP_SUCCESSFUL;
  }

  // if lineNum is 16 and not aligned...
  if ((lineNum == 16) && (alignOffset != 0))
  {
    // if it is the first stripe...
    if (index == 0)
    {
      lineOffset = 0;
      firstOffset = lineNum - alignOffset;
    }
    else
    {
      lineOffset = lineNum - alignOffset;
      firstOffset = 0;
    }
  }
  else
  {
    lineOffset = firstOffset = 0;
  }

  if ((lineNum*index - lineOffset) > (fgXY->y2-fgXY->y1))
    return MDP_FAIL;

  // MDDI packets need to be broken into multiple of stripes.
  // Each stripe has (lineNum) lines.
  fgSubMddi->lcdX = fgXY->x1;
  fgSubMddi->lcdY = fgXY->y1+(lineNum*index - lineOffset);


  fgSubMddi->x1 = fgXY->x1;
  fgSubMddi->x2 = fgXY->x2;

  fgSubMddi->y1 = fgXY->y1+(lineNum*index - lineOffset);

  if ((fgSubMddi->y1+(lineNum-firstOffset)) > fgXY->y2)
  {
    fgSubMddi->y2 = fgXY->y2;
  }
  else
  {
    fgSubMddi->y2 = fgSubMddi->y1+(lineNum-firstOffset)-1;
  }


  // Check to see if this is the first area being separated.
  if (index != 0)
  {
    // Look at the leftover sub-window created.  If the pixel area
    //   of this update is 4 or less we will get black or missing
    //   pixels.  Increase the area of the remaining area to overlap
    //   the area sectioned above.
    nextPxAreaUpdate = (1 + fgSubMddi->x2 - fgSubMddi->x1) *
                       (1 + fgSubMddi->y2 - fgSubMddi->y1);
  
  
    // Check for an area that will cause bad pixels to be updated
    //   for MDDI sub division (Should only be E130 MDDI).
    if ((nextPxAreaUpdate > 0) &&
        (nextPxAreaUpdate < MDP_MDDI130_MIN_PX_SZ))
    {
      // Determine the width of the area to update.
      nextPxWidth = 1 + fgSubMddi->x2 - fgSubMddi->x1;
  
      // This will overlap the region by at most MDP_MDDI130_MIN_PX_SZ - 1
      //   1x1, 1x2, 1x3, 1x4 updates become 1x5, 1x6, 1x7, 1x8 respectively.
      //   2x1, 2x2 updates become 2x3, 2x4 respectively.
      //   3x1 updates become 3x2.
      //   4x1 updates become 4x2
      fgSubMddi->lcdY -= ((MDP_MDDI130_MIN_PX_SZ - 1) / nextPxWidth);
      fgSubMddi->y1   -= ((MDP_MDDI130_MIN_PX_SZ - 1) / nextPxWidth);
    }
  }

  return MDP_SUCCESSFUL;
}


/*===========================================================================

FUNCTION mdpCalCropXY()

DESCRIPTION
  This function is to calculate x-y crop value for MDP VDB window

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdpCalCropXY(  uint16 x1,
                    uint16 x2,
                    uint16 y1,
                    uint16 y2,
                    uint16 vdbX1,
                    uint16 vdbX2,
                    uint16 vdbY1,
                    uint16 vdbY2,
                    uint32 mdpOp,
                    MDP_CROP_XY *crobXY)
{
  uint16 a, b, c, d, e, f;

  a = x1-vdbX1;
  b = x2-x1;
  c = vdbX2-x2;
  d = y1-vdbY1;
  e = y2-y1;
  f = vdbY2-y2;

  switch (mdpOp&(MDPOP_FFRMASK))
  {
    case MDPOP_NOP:
      crobXY->cx1 = a;
      crobXY->cx2 = a+b;
      crobXY->cy1 = d;
      crobXY->cy2 = d+e;
      break;

    case MDPOP_LR:
      crobXY->cx1 = c;
      crobXY->cx2 = c+b;
      crobXY->cy1 = d;
      crobXY->cy2 = d+e;
      break;

    case MDPOP_UD:
      crobXY->cx1 = a;
      crobXY->cx2 = a+b;
      crobXY->cy1 = f;
      crobXY->cy2 = f+e;
      break;

    case MDPOP_UD|MDPOP_LR:
      crobXY->cx1 = c;
      crobXY->cx2 = c+b;
      crobXY->cy1 = f;
      crobXY->cy2 = f+e;
      break;

    case MDPOP_ROT90:
      crobXY->cx1 = f;
      crobXY->cx2 = f+e;
      crobXY->cy1 = a;
      crobXY->cy2 = a+b;
      break;

    case MDPOP_ROT90|MDPOP_LR:
      crobXY->cx1 = d;
      crobXY->cx2 = d+e;
      crobXY->cy1 = a;
      crobXY->cy2 = a+b;
      break;

    case MDPOP_ROT90|MDPOP_UD:
      crobXY->cx1 = f;
      crobXY->cx2 = f+e;
      crobXY->cy1 = c;
      crobXY->cy2 = b+c;
      break;

    case MDPOP_ROT90|MDPOP_UD|MDPOP_LR:
      crobXY->cx1 = d;
      crobXY->cx2 = d+e;
      crobXY->cy1 = c;
      crobXY->cy2 = b+c;
      break;

    default:
      crobXY->cx1 = 0;
      crobXY->cx2 = 0;
      crobXY->cy1 = 0;
      crobXY->cy2 = 0;
  }
}


/*===========================================================================

FUNCTION mdpCalFFRdataFg()

DESCRIPTION
  This function is to calculate FFR(Flip Fetch Rotate) data as RGB
  foreground image

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  MDP_SUCCESSFUL if successful otherwise MDP_FAIL

SIDE EFFECTS
  None

===========================================================================*/
int mdpCalFFRdataFg(  uint32 * image_addr,
                      uint32 mdpOp,
                      uint16 width,
                      uint16 vdbX1,
                      uint16 vdbX2,
                      uint16 vdbY1,
                      uint16 vdbY2,
                      MDP_FFR_DATA *ffrPtr,
                      MDPIMG_TYPE imgType
                   )
{

  int16 bypp;

  if (imgType == MDP_XRGB_8888)
  {
    bypp = 4;
  }
//  else if (imgType == MDP_RGB_888)  No 888 packed support
//  {
//    bypp = 3;
//  }
  else
  {
    bypp = 2;
  }

  switch (mdpOp&(MDPOP_FFRMASK))
  {
    case MDPOP_NOP:
      ffrPtr->imgAddr = (uint32)image_addr + bypp*(vdbX1 + vdbY1*width);  //first block pointer
      ffrPtr->line = SET_FRGND_LINE(width*bypp);  // next line = width pixels x 2 bytes
      ffrPtr->block = SET_FRGND_BLOCK(16*bypp);   //5 16 pixels x 2 bytes
      ffrPtr->stripe = SET_FRGND_STRIPE(width*16*bypp); // next stripe = width pixels x 16 lines x 2bytes (each stripe = 16 lines)
      break;

    case MDPOP_LR:
      ffrPtr->imgAddr = (uint32)image_addr + bypp*(vdbX1 + vdbY1*width + (vdbX2-15));
      ffrPtr->line = SET_FRGND_LINE(width*bypp);
      ffrPtr->block = SET_FRGND_BLOCK(-16*bypp);    //5 -16 pixels x 2 bytes
      ffrPtr->stripe = SET_FRGND_STRIPE(width*16*bypp);
      break;

    case MDPOP_UD:
      ffrPtr->imgAddr = (uint32)image_addr + bypp*(vdbX1 + ((vdbY2-15)*width));
      ffrPtr->line = SET_FRGND_LINE(width*bypp);
      ffrPtr->block = SET_FRGND_BLOCK(16*bypp);     //5 -16 pixels x 2 bytes
      ffrPtr->stripe = SET_FRGND_STRIPE(-width*16*bypp);
      break;

    case MDPOP_UD|MDPOP_LR:
      ffrPtr->imgAddr = (uint32)image_addr + bypp*(vdbX1 + ((vdbY2-15)*width) + (vdbX2-15));
      ffrPtr->line = SET_FRGND_LINE(width*bypp);
      ffrPtr->block = SET_FRGND_BLOCK(-16*bypp);    //5 -16 pixels x 2 bytes
      ffrPtr->stripe = SET_FRGND_STRIPE(-width*16*bypp);
      break;

    case MDPOP_ROT90:
      ffrPtr->imgAddr = (uint32)image_addr + bypp*(vdbX1 + ((vdbY2-15)*width));
      ffrPtr->line = SET_FRGND_LINE(width*bypp);      // respective to input
      ffrPtr->block = SET_FRGND_BLOCK(-width*16*bypp);    //5 -16 pixels x 2 bytes
      ffrPtr->stripe = SET_FRGND_STRIPE(16*bypp);     // respective to output
      break;

    case MDPOP_ROT90|MDPOP_LR:
      ffrPtr->imgAddr = (uint32)image_addr + bypp*(vdbX1 + vdbY1*width);
      ffrPtr->line = SET_FRGND_LINE(width*bypp);      // respective to input
      ffrPtr->block = SET_FRGND_BLOCK(width*16*bypp);   //5 -16 pixels x 2 bytes
      ffrPtr->stripe = SET_FRGND_STRIPE(16*bypp);     // respective to output
      break;

    case MDPOP_ROT90|MDPOP_UD:
      ffrPtr->imgAddr = (uint32)image_addr + bypp*(vdbX1 + ((vdbY2-15)*width) + (vdbX2-15));
      ffrPtr->line = SET_FRGND_LINE(width*bypp);
      ffrPtr->block = SET_FRGND_BLOCK(-width*16*bypp);
      ffrPtr->stripe = SET_FRGND_STRIPE(-16*bypp);
      break;

    case MDPOP_ROT90|MDPOP_UD|MDPOP_LR:
      ffrPtr->imgAddr = (uint32)image_addr + bypp*(vdbX1 + vdbY1*width + (vdbX2-15));
      ffrPtr->line = SET_FRGND_LINE(width*bypp);
      ffrPtr->block = SET_FRGND_BLOCK(width*16*bypp);
      ffrPtr->stripe = SET_FRGND_STRIPE(-16*bypp);
      break;

    default: return MDP_FAIL;
  }

  return MDP_SUCCESSFUL;
}

/*===========================================================================

FUNCTION mdpCalFFRdataBg()

DESCRIPTION
  This function is to calculate FFR(Flip Fetch Rotate) data as RGB
  background image

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  MDP_SUCCESSFUL if successful otherwise MDP_FAIL

SIDE EFFECTS
  None

===========================================================================*/
int mdpCalFFRdataBg(  uint32 * image_addr,
                      uint32 mdpOp,
                      uint16 width,
                      uint16 vdbX1,
                      uint16 vdbX2,
                      uint16 vdbY1,
                      uint16 vdbY2,
                      MDP_FFR_DATA *ffrPtr,
                      MDPIMG_TYPE imgType
                   )
{

  int16 bypp;

  if (imgType == MDP_XRGB_8888)
  {
    bypp = 4;
  }
//  else if (imgType == MDP_RGB_888)  No 888 packed support
//  {
//    bypp = 3;
//  }
  else
  {
    bypp = 2;
  }

  switch (mdpOp&(MDPOP_FFRMASK))
  {
    case MDPOP_NOP:
      ffrPtr->imgAddr = (uint32)image_addr + bypp*(vdbX1 + vdbY1*width);
      ffrPtr->line = SET_BKGND_LINE(width*bypp);    // next line = width pixels x 2 bytes
      ffrPtr->block = SET_BKGND_BLOCK(16*bypp);     //5 16 pixels x 2 bytes
      ffrPtr->stripe = SET_BKGND_STRIPE(width*16*bypp); // next stripe = width pixels x 16 lines x 2bytes (each stripe = 16 lines)
      break;

    case MDPOP_LR:
      ffrPtr->imgAddr = (uint32)image_addr + bypp*(vdbX1 + vdbY1*width + (vdbX2-15));
      ffrPtr->line = SET_BKGND_LINE(width*bypp);
      ffrPtr->block = SET_BKGND_BLOCK(-16*bypp);    //5 -16 pixels x 2 bytes
      ffrPtr->stripe = SET_BKGND_STRIPE(width*16*bypp);
      break;

    case MDPOP_UD:
      ffrPtr->imgAddr = (uint32)image_addr + bypp*(vdbX1 + ((vdbY2-15)*width));
      ffrPtr->line = SET_BKGND_LINE(width*bypp);
      ffrPtr->block = SET_BKGND_BLOCK(16*bypp);     //5 -16 pixels x 2 bytes
      ffrPtr->stripe = SET_BKGND_STRIPE(-width*16*bypp);
      break;

    case MDPOP_UD|MDPOP_LR:
      ffrPtr->imgAddr = (uint32)image_addr + bypp*(vdbX1 + ((vdbY2-15)*width) + (vdbX2-15));
      ffrPtr->line = SET_BKGND_LINE(width*bypp);
      ffrPtr->block = SET_BKGND_BLOCK(-16*bypp);    //5 -16 pixels x 2 bytes
      ffrPtr->stripe = SET_BKGND_STRIPE(-width*16*bypp);
      break;

    case MDPOP_ROT90:
      ffrPtr->imgAddr = (uint32)image_addr + bypp*(vdbX1 + ((vdbY2-15)*width));
      ffrPtr->line = SET_BKGND_LINE(width*bypp);      // respective to input
      ffrPtr->block = SET_BKGND_BLOCK(-width*16*bypp);    //5 -16 pixels x 2 bytes
      ffrPtr->stripe = SET_BKGND_STRIPE(16*bypp);     // respective to output
      break;

    case MDPOP_ROT90|MDPOP_LR:
      ffrPtr->imgAddr = (uint32)image_addr + bypp*(vdbX1 + vdbY1*width);
      ffrPtr->line = SET_BKGND_LINE(width*bypp);      // respective to input
      ffrPtr->block = SET_BKGND_BLOCK(width*16*bypp);   //5 -16 pixels x 2 bytes
      ffrPtr->stripe = SET_BKGND_STRIPE(16*bypp);     // respective to output
      break;

    case MDPOP_ROT90|MDPOP_UD:
      ffrPtr->imgAddr = (uint32)image_addr + bypp*(vdbX1 + ((vdbY2-15)*width) + (vdbX2-15));
      ffrPtr->line = SET_BKGND_LINE(width*bypp);
      ffrPtr->block = SET_BKGND_BLOCK(-width*16*bypp);
      ffrPtr->stripe = SET_BKGND_STRIPE(-16*bypp);
      break;

    case MDPOP_ROT90|MDPOP_UD|MDPOP_LR:
      ffrPtr->imgAddr = (uint32)image_addr + bypp*(vdbX1 + vdbY1*width + (vdbX2-15));
      ffrPtr->line = SET_BKGND_LINE(width*bypp);
      ffrPtr->block = SET_BKGND_BLOCK(width*16*bypp);
      ffrPtr->stripe = SET_BKGND_STRIPE(-16*bypp);
      break;

    default: return MDP_FAIL;
  }

  return MDP_SUCCESSFUL;
}

/*===========================================================================

FUNCTION mdpCalFFRdataLuma()

DESCRIPTION
  This function is to calculate FFR(Flip Fetch Rotate) Y/Luma data

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  MDP_SUCCESSFUL if successful otherwise MDP_FAIL

SIDE EFFECTS
  None

===========================================================================*/
/* luma is still 16x16 block size */
int mdpCalFFRdataLuma(  uint32 * image_addr,
                        uint32 mdpOp,
                        uint32 scale,
                        uint16 width,
                        uint16 vdbX1,
                        uint16 vdbX2,
                        uint16 vdbY1,
                        uint16 vdbY2,
                        MDP_FFR_DATA *ffrPtr,
                        int16 bypp    /* bytes per pixel */
                     )
{

  int16 blkSize;

  if (scale == MDPOP_VDO_X2)
  {
    blkSize = 8;
  }
  else
  {
    blkSize = 16;
  }

  switch (mdpOp&(MDPOP_FFRMASK))
  {
    case MDPOP_NOP:
      ffrPtr->imgAddr = (uint32)image_addr + bypp*(vdbX1 + vdbY1*width);
      ffrPtr->line = SET_LUMA_LINE(width*bypp); // next line = width pixels x 2 bytes
      ffrPtr->block = SET_LUMA_BLOCK(blkSize*bypp);   //5 16 pixels x 2 bytes
      ffrPtr->stripe = SET_LUMA_STRIPE(width*blkSize*bypp); // next stripe = width pixels x 16 lines x 2bytes (each stripe = 16 lines)
      break;

    case MDPOP_LR:
      ffrPtr->imgAddr = (uint32)image_addr + bypp*(vdbX1 + vdbY1*width + (vdbX2-(blkSize-1)));
      ffrPtr->line = SET_LUMA_LINE(width*bypp);
      ffrPtr->block = SET_LUMA_BLOCK(-blkSize*bypp);    //5 -16 pixels x 2 bytes
      ffrPtr->stripe = SET_LUMA_STRIPE(width*blkSize*bypp);
      break;

    case MDPOP_UD:
      ffrPtr->imgAddr = (uint32)image_addr + bypp*(vdbX1 + ((vdbY2-(blkSize-1))*width));
      ffrPtr->line = SET_LUMA_LINE(width*bypp);
      ffrPtr->block = SET_LUMA_BLOCK(blkSize*bypp);     //5 -16 pixels x 2 bytes
      ffrPtr->stripe = SET_LUMA_STRIPE(-width*blkSize*bypp);
      break;

    case MDPOP_UD|MDPOP_LR:
      ffrPtr->imgAddr = (uint32)image_addr + bypp*(vdbX1 + ((vdbY2-(blkSize-1))*width) + (vdbX2-(blkSize-1)));
      ffrPtr->line = SET_LUMA_LINE(width*bypp);
      ffrPtr->block = SET_LUMA_BLOCK(-blkSize*bypp);    //5 -16 pixels x 2 bytes
      ffrPtr->stripe = SET_LUMA_STRIPE(-width*blkSize*bypp);
      break;

    case MDPOP_ROT90:
      ffrPtr->imgAddr = (uint32)image_addr + bypp*(vdbX1 + ((vdbY2-(blkSize-1))*width));
      ffrPtr->line = SET_LUMA_LINE(width*bypp);     // respective to input
      ffrPtr->block = SET_LUMA_BLOCK(-width*blkSize*bypp);    //5 -16 pixels x 2 bytes
      ffrPtr->stripe = SET_LUMA_STRIPE(blkSize*bypp);     // respective to output
      break;

    case MDPOP_ROT90|MDPOP_LR:
      ffrPtr->imgAddr = (uint32)image_addr + bypp*(vdbX1 + vdbY1*width);
      ffrPtr->line = SET_LUMA_LINE(width*bypp);     // respective to input
      ffrPtr->block = SET_LUMA_BLOCK(width*blkSize*bypp);   //5 -16 pixels x 2 bytes
      ffrPtr->stripe = SET_LUMA_STRIPE(blkSize*bypp);     // respective to output
      break;

    case MDPOP_ROT90|MDPOP_UD:
      ffrPtr->imgAddr = (uint32)image_addr + bypp*(vdbX1 + ((vdbY2-(blkSize-1))*width) + (vdbX2-(blkSize-1)));
      ffrPtr->line = SET_LUMA_LINE(width*bypp);
      ffrPtr->block = SET_LUMA_BLOCK(-width*blkSize*bypp);
      ffrPtr->stripe = SET_LUMA_STRIPE(-blkSize*bypp);
      break;

    case MDPOP_ROT90|MDPOP_UD|MDPOP_LR:
      ffrPtr->imgAddr = (uint32)image_addr + bypp*(vdbX1 + vdbY1*width + (vdbX2-(blkSize-1)));
      ffrPtr->line = SET_LUMA_LINE(width*bypp);
      ffrPtr->block = SET_LUMA_BLOCK(width*blkSize*bypp);
      ffrPtr->stripe = SET_LUMA_STRIPE(-blkSize*bypp);
      break;

    default: return MDP_FAIL;
  }

  return MDP_SUCCESSFUL;
}

/*===========================================================================

FUNCTION mdpCalFFRdataChroma()

DESCRIPTION
  This function is to calculate FFR(Flip Fetch Rotate) CbCr/Chroma data

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  MDP_SUCCESSFUL if successful otherwise MDP_FAIL

SIDE EFFECTS
  None

===========================================================================*/
/* only chroma block size is 16x8 */
int mdpCalFFRdataChroma(  uint32 * image_addr,
                          uint32 mdpOp,
                          uint32 scale,
                          uint16 width,
                          uint16 vdbX1,
                          uint16 vdbX2,
                          uint16 vdbY1,
                          uint16 vdbY2,
                          MDP_FFR_DATA *ffrPtr,
                          int16 bypp    /* bytes per pixel */
                       )
{

  int16 blkSize, fac;

  if (scale == MDPOP_VDO_X2)
  {
    blkSize = 8;
    fac = 2;
  }
  else
  {
    fac = 2;
    blkSize = 16;
  }

  switch (mdpOp&(MDPOP_FFRMASK))
  {
    case MDPOP_NOP:
      ffrPtr->imgAddr = (uint32)image_addr + bypp*(vdbX1 + (vdbY1*width)/fac);
      ffrPtr->line = SET_CHROMA_LINE(width*bypp);   // next line = width pixels x 2 bytes
      ffrPtr->block = SET_CHROMA_BLOCK(blkSize*bypp);     //5 16 pixels x 2 bytes
      ffrPtr->stripe = SET_CHROMA_STRIPE(width*(blkSize/fac)*bypp); // next stripe = width pixels x 8 lines x 2bytes (each stripe = 16 lines)
      break;

    case MDPOP_LR:
      ffrPtr->imgAddr = (uint32)image_addr + bypp*((vdbX1 + (vdbX2-(blkSize-1))) + (vdbY1*width)/fac);
      ffrPtr->line = SET_CHROMA_LINE(width*bypp);
      ffrPtr->block = SET_CHROMA_BLOCK(-blkSize*bypp);    //5 -16 pixels x 2 bytes
      ffrPtr->stripe = SET_CHROMA_STRIPE(width*(blkSize/fac)*bypp);
      break;

    case MDPOP_UD:
      ffrPtr->imgAddr = (uint32)image_addr + bypp*(vdbX1 + ((vdbY2-(blkSize-1))*width)/fac);
      ffrPtr->line = SET_CHROMA_LINE(width*bypp);
      ffrPtr->block = SET_CHROMA_BLOCK(blkSize*bypp);     //5 -16 pixels x 2 bytes
      ffrPtr->stripe = SET_CHROMA_STRIPE(-width*(blkSize/fac)*bypp);
      break;

    case MDPOP_UD|MDPOP_LR:
      ffrPtr->imgAddr = (uint32)image_addr + bypp*((vdbX1 + (vdbX2-(blkSize-1))) + ((vdbY2-(blkSize-1))*width)/fac);
      ffrPtr->line = SET_CHROMA_LINE(width*bypp);
      ffrPtr->block = SET_CHROMA_BLOCK(-blkSize*bypp);    //5 -16 pixels x 2 bytes
      ffrPtr->stripe = SET_CHROMA_STRIPE(-width*(blkSize/fac)*bypp);
      break;

    case MDPOP_ROT90:
      ffrPtr->imgAddr = (uint32)image_addr + bypp*(vdbX1 + ((vdbY2-(blkSize-1))*width)/fac);
      ffrPtr->line = SET_CHROMA_LINE(width*bypp);     // respective to input
      ffrPtr->block = SET_CHROMA_BLOCK(-width*(blkSize/fac)*bypp);    //5 -8 pixels x 2 bytes
      ffrPtr->stripe = SET_CHROMA_STRIPE(blkSize*bypp);     // respective to output
      break;

    case MDPOP_ROT90|MDPOP_LR:
      ffrPtr->imgAddr = (uint32)image_addr + bypp*(vdbX1 + (vdbY1*width)/fac);
      ffrPtr->line = SET_CHROMA_LINE(width*bypp);     // respective to input
      ffrPtr->block = SET_CHROMA_BLOCK(width*(blkSize/fac)*bypp);   //5 -16 pixels x 2 bytes
      ffrPtr->stripe = SET_CHROMA_STRIPE(blkSize*bypp);     // respective to output
      break;

    case MDPOP_ROT90|MDPOP_UD:
      ffrPtr->imgAddr = (uint32)image_addr + bypp*((vdbX1 + (vdbX2-(blkSize-1))) + ((vdbY2-(blkSize-1))*width)/fac);
      ffrPtr->line = SET_CHROMA_LINE(width*bypp);
      ffrPtr->block = SET_CHROMA_BLOCK(-width*(blkSize/fac)*bypp);
      ffrPtr->stripe = SET_CHROMA_STRIPE(-blkSize*bypp);
      break;

    case MDPOP_ROT90|MDPOP_UD|MDPOP_LR:
      ffrPtr->imgAddr = (uint32)image_addr + bypp*((vdbX1 + (vdbX2-(blkSize-1))) + (vdbY1*width)/fac);
      ffrPtr->line = SET_CHROMA_LINE(width*bypp);
      ffrPtr->block = SET_CHROMA_BLOCK(width*(blkSize/fac)*bypp);
      ffrPtr->stripe = SET_CHROMA_STRIPE(-blkSize*bypp);
      break;

    default: return MDP_FAIL;
  }

  return MDP_SUCCESSFUL;
}

/*===========================================================================

FUNCTION mdp_get_real_xy()

DESCRIPTION
  This function is to adjust user mdpImg data into driver mdpImg data

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdp_get_real_xy(MDPIMG_LIST *fgImg)
{

  int32 fg_x1, fg_x2, fg_y1,fg_y2;

  uint32 fg_mdpOp;

  
  fg_mdpOp = fgImg->img.mdpOp;
  fg_mdpOp &= MDPOP_FFRMASK;

  fg_x1 = fgImg->img.lcd_x;
  fg_y1 = fgImg->img.lcd_y;
  if (fg_mdpOp&MDPOP_ROT90)
  {
    fg_y2 = fg_y1+fgImg->img.cwin.x2-fgImg->img.cwin.x1;
    fg_x2 = fg_x1+fgImg->img.cwin.y2-fgImg->img.cwin.y1;
  }
  else
  {
    fg_x2 = fg_x1+fgImg->img.cwin.x2-fgImg->img.cwin.x1;
    fg_y2 = fg_y1+fgImg->img.cwin.y2-fgImg->img.cwin.y1;
  }

  if (fgImg->img.imgType == MDP_YCBCR420)
  {
    fgImg->scaleInfo.scale = fgImg->img.mdpOp&MDPOP_VDOMASK;
    fgImg->scaleInfo.scale_flag = TRUE;
    fgImg->scaleInfo.odd_scale = FALSE;
    switch (fgImg->scaleInfo.scale)
    {
      case MDPOP_VDO_X2:
        fgImg->scaleInfo.num = 2;
        fgImg->scaleInfo.den = 1;
        fgImg->scaleInfo.lineNum = 16;
        break;

      case MDPOP_VDO_1BY4:
        fgImg->scaleInfo.num = 1;
        fgImg->scaleInfo.den = 4;
        fgImg->scaleInfo.lineNum = 4;
        break;

      case MDPOP_VDO_1BY2:
        fgImg->scaleInfo.num = 1;
        fgImg->scaleInfo.den = 2;
        fgImg->scaleInfo.lineNum = 8;
        break;

      case MDPOP_VDO_3BY4:
        fgImg->scaleInfo.num = 3;
        fgImg->scaleInfo.den = 4;
        fgImg->scaleInfo.odd_scale = TRUE;
        fgImg->scaleInfo.lineNum = 12;
        break;

      case MDPOP_VDO_NOP:
      default:
        fgImg->scaleInfo.num = 1;
        fgImg->scaleInfo.den = 1;
        fgImg->scaleInfo.scale_flag = FALSE;
        fgImg->scaleInfo.lineNum = 16;
        break;
    }

    if (fgImg->scaleInfo.scale_flag)
    {
      fg_x2 = fg_x1 + ((fg_x2-fg_x1+1)*fgImg->scaleInfo.num)/fgImg->scaleInfo.den-1;
      fg_y2 = fg_y1 + ((fg_y2-fg_y1+1)*fgImg->scaleInfo.num)/fgImg->scaleInfo.den-1;
    }
  }
  else /* gfx only needs to define mddi lineNum */
  {
    fgImg->scaleInfo.num = 1;
    fgImg->scaleInfo.den = 1;
    fgImg->scaleInfo.scale_flag = FALSE;
    fgImg->scaleInfo.lineNum = 16;
  }

  /* must accommodate maximum address alignment offset */
  //fgImg->scaleInfo.lineNum -= 4;

  fgImg->mbcOp = fgImg->img.mdpOp&MDPOP_MBCMASK;
  fgImg->img.mdpOp = fg_mdpOp;

  // Let's make sure we don't accidently give a negative area cwin...
  if ( fg_x2 >= fg_x1 ) 
  {
    fgImg->img.cwin.x1 = fg_x1;
    fgImg->img.cwin.x2 = fg_x2;
  }
  else // We should never get this case but just in case...
  {
    // Clip on one of the edges, don't know what else can be done.
    fgImg->img.cwin.x1 = fgImg->img.cwin.x2 = fg_x1;

    MDP_MSG_ERROR("Attempting to set a negative cwinX, clipping for safety!",0,0,0);
  }

  // Checking for the same condition in y plane.
  if ( fg_y2 >= fg_y1 ) 
  {
    fgImg->img.cwin.y1 = fg_y1;
    fgImg->img.cwin.y2 = fg_y2;
  }
  else // We should never get this case but just in case...
  {
    // Clip on one of the edges, don't know what else can be done.
    fgImg->img.cwin.y1 = fgImg->img.cwin.y2 = fg_y1;

    MDP_MSG_ERROR("Attempting to set a negative cwinY, clipping for safety!",0,0,0);
  }
}


/*===========================================================================

FUNCTION mdp_copyLayer565()

DESCRIPTION
  This function is intended to be a helper function for layer1 blending (by
  default this also used for display capture).  Call this function after
  the MDP has updated content to mdpTempImgBuf and you wish to copy the
  contents into either a display capture or a sub layer 1 buffer.  This
  function can only update RGB 565 buffers since it written using uint16 * 's
  to address the 2 bytes per pixel.

DEPENDENCIES
  Global variable mdpTempImgBuf and the macro definition for MAX_WB_WIDTH
  which is assumed to be the width of the display buffer passed in.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mdp_copyLayer565(int32 lcdX,          // Starting cordinates of update.
                      int32 lcdY,
                      uint32 width,        // Not scale/rotation corrected
                      uint32 height,       //   dimensions of update.
                      uint32 mdpOp,        // Rotation info.
                      MDPSCALE_INFO scale, // Fixed scale for both dimen.
                      uint32 wScale,       // Output scale to WQVGA?
                      uint16 *buff565Ptr,  // RGB565 display buffer address.
                      uint32 buffSzBytes)  // Byte size of display buffer.
{
  int pxRow;           // Row pixel position, used in for loop.
  
  // 16 bit pointers for 16 bits per pixel (565) adressing.
  uint16 *srcBuffPtr,  // Address of current row to read.
         *dstBuffPtr,  // Address of current row to write.
         *endBuffPtr;  // Out of bounds address for dest write.

  uint32 roiWidth,
         roiHeight;

  // Check for any MDPOP which includes a 90 or 270 rotation.  Swap the W/H.
  //   Apply any scale factor which may exist...
  if (mdpOp&MDPOP_ROT90)
  {
    roiWidth  = (scale.num * height) / scale.den;
    roiHeight = (scale.num * width ) / scale.den;
  }
  else  // No rotation, don't swap W/H.
  {
    roiWidth  = (scale.num * width ) / scale.den;
    roiHeight = (scale.num * height) / scale.den;
  }

  // Check for the special case when WQVGA scaled output is used.
#ifdef FEATURE_MDP_WSCALE
  if (wScale != 0)
  {
    roiWidth  = MDP_WSCALE_OUT_WIDTH;
    roiHeight = MDP_WSCALE_OUT_HEIGHT;
  }
#endif // #ifdef FEATURE_MDP_WSCALE

  // Source buffer always starts from beginning of temp buffer.
  srcBuffPtr = (uint16*)(mdpTempImgBuf->buffer);

  // Destination buffer needs to be corrected for the LCD ROI needed.
  dstBuffPtr = buff565Ptr + lcdX + (lcdY * mdpTempImgBuf->pxWidth);

  // Get the first position of the OOB memory location.
  endBuffPtr = buff565Ptr + (buffSzBytes/2);

  // Update the display buffer one line at a time.
  for (pxRow=0; pxRow<roiHeight; pxRow++)
  {
    // Check for out of bounds memory write.
    if ( (dstBuffPtr >= buff565Ptr) &&
         ((dstBuffPtr + roiWidth) <= endBuffPtr))  // Use <= instead of (roiWidth-1).
    {
      
		MDP_INVALIDATE_DCACHE_REGION(srcBuffPtr,2*roiWidth); 
      /* DCache Invalidate before giving the buffer to ARM*/
      
        memcpy((char *)dstBuffPtr,(char *)srcBuffPtr, 2*roiWidth);
      
        MDP_FLUSH_DCACHE_REGION(dstBuffPtr,2*roiWidth); 
		/*Flushing the Dcache contents "L1 Buffer" to physical memory for MDP, Not Required for Display Capture*/
		
    }
    else
    {
      MDP_MSG_HIGH("Layer 1 Failure: Not completing update!",0,0,0);
      return;
    }

    // Pointer arithmatic to get the appropriate address offset.
    srcBuffPtr += roiWidth;               // Width of the ROI
    dstBuffPtr += mdpTempImgBuf->pxWidth; // Width of LCD
  }
}


/*===========================================================================

FUNCTION mdp_clearLayer565()

DESCRIPTION
  Take a layer 1 buffer and write to a constant value or clear the buffer.
  Helper function for layer 1 support.

DEPENDENCIES
  Only useful for RGB565 buffers.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mdp_clearLayer565(uint16 *buff565Ptr,
		       uint32 buffSzBytes,
		       uint16 clearValue)
{
  int pixel, lastPixel;
  uint16 *curBuff565Ptr = buff565Ptr;

  // Calculate the last pixel to clear, RGB565 is 2 bytes per pixel.
  lastPixel = buffSzBytes /2;

  // Traverse the entire array, W/H don't really matter so use ptr access.
  for (pixel=0; pixel<lastPixel; pixel++)
  {
    // Clear the image buffer.
    *curBuff565Ptr = clearValue;
    ++curBuff565Ptr;
  }

  MDP_FLUSH_DCACHE_REGION(buff565Ptr,buffSzBytes);
  
}


/*===========================================================================

FUNCTION mdp_disp_update()

DESCRIPTION
  This function is to check if we need to change the current CCS setting

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  MDP_SUCCESSFUL if successful otherwise MDP_FAIL

SIDE EFFECTS
  caller of this function will get blocked until MDP finishes to update
  the image on LCD display

===========================================================================*/
void mdp_check_ccs(MDPIMG *mdpImg)
{
  if (NULL == mdpImg)
    return;
    
  // if it's video image with new ccs
  if (mdpImg->imgType == MDP_YCBCR420)
  {
    if (mdpImg->mdpOp & MDPOP_SEL_CCS)
    {
      if (mdpImg->ccsPtr != mdp_curr_ccs)
        mdp_set_ccs(mdpImg->ccsPtr, FALSE);
    }
    else
    {
      if (&mdp_default_ccs != mdp_curr_ccs)
        mdp_set_ccs(&mdp_default_ccs, FALSE);
    }
  }
}

/*===========================================================================

FUNCTION mdp_disp_update()

DESCRIPTION
  This function is to display the specified image on LCD screen.
  Any overlapped image in the overlay list from other layer also gets
  displayed.

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  MDP_SUCCESSFUL if successful otherwise MDP_FAIL

SIDE EFFECTS
  caller of this function will get blocked until MDP finishes to update
  the image on LCD display

===========================================================================*/
MDP_STATUS mdp_disp_update_proc(MDPIMG *mdpImg, disp_cb_type * user_cb)  
{
  MDPIMG_LIST * mdpListPtrTemp, **mdpImgTailPtr;
  MDPIMG tempImg;
  uint32 result;
  uint16 *buf;
  uint32 bufSzBytes;
  MDP_OVER_XYInt fgXY;
  int x_off, y_off;
  MDPIMG_LIST *mdpSubNodePtr;

#ifdef FEATURE_MDDI
#error code not present
#endif /* FEATURE_MDDI */

  result = MDP_FAIL;
  if (mdp_drv_initialized != TRUE)
    return result;

#ifdef FEATURE_MDDI
#error code not present
#endif /* FEATURE_MDDI */

  if (mdpImg->bmy_addr == NULL)
    return MDP_FAIL;

  if (!mdp_verify_dest(mdpImg->dest))
    return MDP_FAIL;

  if (MDP_INVALID_LAYER(mdpImg->layer))
    return result;

#ifdef FEATURE_MDDI
#error code not present
#endif // FEATURE_MDDI

  MDP_MSG_DEBUG("Event: mdp_disp_update_proc() starting",0,0,0);

  // let's wait until MDP is ready before moving in
  mdp_wait_script_done();    

  // if it's video image with new ccs
  mdp_check_ccs(mdpImg);

  rex_enter_crit_sect(&mdp_crit_sect);

  if (mdpImg->dest == PRIMARY_LCD_TYPE)
  {
    mdpImgTailPtr = mdpImgTailPtr_1;
    buf           = (uint16*)(mdp_slBuf_1->buffer);
    bufSzBytes    = mdp_slBuf_1->byteSize;
    mdpSubNodePtr = mdpSubNode;
  }
  else
  {
    mdpImgTailPtr = mdpImgTailPtr_2;
    buf           = (uint16*)(mdp_slBuf_2->buffer);
    bufSzBytes    = mdp_slBuf_2->byteSize; 
    mdpSubNodePtr = mdpSubNode2;
  }

  if (mdpImgTailPtr[MDP_LAYER_1] != NULL)
  {
    switch (mdpImg->layer)
    {
      case MDP_LAYER_0:
        mdpListPtrTemp = mdpImgTailPtr[MDP_LAYER_2];
        mdpImgTailPtr[MDP_LAYER_2] = mdpSubNodePtr;
#ifdef FEATURE_HS_DISPLAY_CAPTURE
        //if screen capture mode is enabled - capture LCD content to buffer
        if (mdpCaptureMode)
        {
          mdp_capture_display(mdpImg);
        }
#endif /*FEATURE_HS_DISPLAY_CAPTURE*/
        result = mdp_disp_update_sub(mdpImg, FALSE, NULL, 0, user_cb);
        mdpImgTailPtr[MDP_LAYER_2] = mdpListPtrTemp;
        break;

      case MDP_LAYER_1:
        mdpImg->layer = MDP_LAYER_0;
        if ((result = mdp_disp_update_sub(mdpImg, TRUE, buf, bufSzBytes, NULL)) == MDP_SUCCESSFUL)
        {
          // needs to be optimized to display only partial sub region
          // instead of displaying whole screen
#ifdef FEATURE_HS_DISPLAY_CAPTURE
          //if screen capture mode is enabled - capture LCD content to buffer
          if (mdpCaptureMode)
          {
            mdp_capture_display(&mdpSubNodePtr->orig_img);
          }
#endif /*FEATURE_HS_DISPLAY_CAPTURE*/ 
          result = mdp_disp_update_sub(&mdpSubNodePtr->orig_img, FALSE, NULL, 0, user_cb);
        }
        mdpImg->layer = MDP_LAYER_1;
        break;

      case MDP_LAYER_2:
        mdpListPtrTemp = mdpImgTailPtr[MDP_LAYER_0];
        mdpImgTailPtr[MDP_LAYER_0] = mdpImgTailPtr[MDP_LAYER_1];
        result = mdp_disp_update_sub(mdpImg, TRUE, buf, bufSzBytes, NULL);
        mdpImgTailPtr[MDP_LAYER_0] = mdpListPtrTemp;

        //let's do partial update only!
        if (result == MDP_SUCCESSFUL)
        {
          tempImg = mdpSubNodePtr->orig_img;
          if (mdpImg->mdpOp&MDPOP_ROT90)
          {
            x_off = mdpImg->cwin.y2-mdpImg->cwin.y1;
            y_off = mdpImg->cwin.x2-mdpImg->cwin.x1;
          }
          else
          {
            y_off = mdpImg->cwin.y2-mdpImg->cwin.y1;
            x_off = mdpImg->cwin.x2-mdpImg->cwin.x1;
          }

          if (mdpCalOverXYInt(mdpImg->lcd_x+MDP_SYS_OFFSET_X,
                              mdpImg->lcd_x+MDP_SYS_OFFSET_X+x_off,
                              mdpImg->lcd_y+MDP_SYS_OFFSET_Y,
                              mdpImg->lcd_y+MDP_SYS_OFFSET_Y+y_off,
                             mdpSubNodePtr->img.cwin.x1,
                             mdpSubNodePtr->img.cwin.x2,
                             mdpSubNodePtr->img.cwin.y1,
                             mdpSubNodePtr->img.cwin.y2,
                              &fgXY) == MDP_SUCCESSFUL)
          {
            tempImg.cwin = fgXY;
            tempImg.lcd_x = fgXY.x1-MDP_SYS_OFFSET_X;
            tempImg.lcd_y = fgXY.y1-MDP_SYS_OFFSET_Y;
#ifdef FEATURE_HS_DISPLAY_CAPTURE
            ///if screen capture mode is enabled - capture LCD content to buffer
            if (mdpCaptureMode)
            {
              mdp_capture_display(&tempImg);
            }
#endif /*FEATURE_HS_DISPLAY_CAPTURE*/ 
            result = mdp_disp_update_sub(&tempImg, FALSE, NULL, 0, user_cb);
          }
        }
        break;
    }
    // when LAYER_1 gets used, we can't leave crit-section too early.
    rex_leave_crit_sect(&mdp_crit_sect);    
  }
  else
  {
#ifdef FEATURE_HS_DISPLAY_CAPTURE
    //if screen capture mode is enabled - capture LCD content to buffer
    if (mdpCaptureMode)
    {
      mdp_capture_display(mdpImg);
    }
#endif /*FEATURE_HS_DISPLAY_CAPTURE*/


    /* Must not leave critical section to early for a sync call */
    rex_leave_crit_sect(&mdp_crit_sect);
    result = mdp_disp_update_sub(mdpImg, FALSE, NULL, 0, user_cb);
  }
  return result;
}


/*===========================================================================

FUNCTION mdp_valid_img_for_wscale()

DESCRIPTION
  Helper function to limit the use of the Wscale feature to case where it
  will work properly.  Lots of QXDM messages added to help debug the use
  of the wscale feature.

DEPENDENCIES
  None.

RETURN VALUE
  True for success, otherwise FALSE.

SIDE EFFECTS
  None.

===========================================================================*/
#ifdef FEATURE_MDP_WSCALE
boolean mdp_valid_img_for_wscale(uint16 lcd_col_start,
                                 uint16 lcd_col_end,
                                 uint16 lcd_row_start,
                                 uint16 lcd_row_end,
                                 uint32 wScale)
{
  uint32 widthPix, heightPix;  // ROI for MDP output.
  uint32 lcdPixHeightLast;     // Calculate last LCD row to update.
 
  // Find the ROI that MDP will output.
  widthPix  = lcd_col_end - lcd_col_start;
  heightPix = lcd_row_end - lcd_row_start;

  // Find the last row of pixels the w-scale will attempt to output to LCD.
  lcdPixHeightLast = MDP_WSCALE_OUT_HEIGHT + lcd_row_start;

  // Scaling not supported on mdp cores 1.0 and 1.1.
  if ((mdp_rev == MDP_REV_01_00) || (mdp_rev == MDP_REV_01_01))
  {
    MDP_MSG_ERROR("Wscale Aborted! Not supported on this HW!",0,0,0);
    return FALSE;
  }

  if (lcd_col_start != 0)
  {
    MDP_MSG_ERROR("Wscale Aborted! Invalid Region, not located at (0,x).",0,0,0);
    MDP_MSG_ERROR("  Attempted update at (%d, %d)",lcd_col_start,lcd_row_start,0);
    return FALSE;
  }

  if (lcdPixHeightLast > MAX_WB_HEIGHT)
  {
    MDP_MSG_ERROR("Wscale Aborted! Attempting to update outside LCD memory.",0,0,0);
    MDP_MSG_ERROR("  LCD Row %d does not exist!",MAX_WB_HEIGHT + 1,0,0);
    MDP_MSG_ERROR("  Attempted to update rows %d->%d",lcd_row_start,lcdPixHeightLast,0);
    return FALSE;
  }

  // Check for valid CWINs for type of scaling to perform.
  switch(wScale)
  {
    // HW is hardwired (6k only) to accept only 240x320 images for both
    //   linear and edge stretch scaling.
    case MDPOP_WSCALE_LINEAR_STRETCH:
    case MDPOP_WSCALE_EDGE_STRETCH:
      if ( (widthPix  != (MDP_WSCALE_IN_WIDTH_LINEAR_AND_EDGE  - 1)) ||
           (heightPix != (MDP_WSCALE_IN_HEIGHT_LINEAR_AND_EDGE - 1)) )
      {
        MDP_MSG_ERROR("Wscale Aborted! Invalid Region, must be 240x320.",0,0,0);
        MDP_MSG_ERROR("  Attempted update region: %dx%d.",1+lcd_col_end,1+lcd_row_end,0);
        return FALSE;
      }
      break;

    // HW is hardwired (6k only) to accept only 192x320 images for 1.25
    //   fixed scaling.
    case MDPOP_WSCALE_NO_STRETCH:
      if ( (widthPix  != (MDP_WSCALE_IN_WIDTH_NO_STRETCH  - 1)) ||
           (heightPix != (MDP_WSCALE_IN_HEIGHT_NO_STRETCH - 1)) )
      {
        MDP_MSG_ERROR("Wscale Aborted! Invalid Region, must be 192x320.",0,0,0);
        MDP_MSG_ERROR("  Attempted update region: %dx%d.",1+lcd_col_end,1+lcd_row_end,0);
        return FALSE;
      }
      break;

    default:
      MDP_MSG_ERROR("Wscale Aborted! Operation not understood!",0,0,0);
      return FALSE;
  }

  return TRUE;
}
#endif // #ifdef FEATURE_MDP_WSCALE

/*===========================================================================

FUNCTION mdp_calc_sub_width()

DESCRIPTION
  This function is to adjust the sub window update width for MDP HW 16n+1
  issue for MDDI panel

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  MDP_SUCCESSFUL if successful otherwise MDP_FAIL

SIDE EFFECTS
  None
  
===========================================================================*/
#ifdef FEATURE_MDDI                  
#error code not present
#endif


/*===========================================================================

FUNCTION mdp_disp_update_sub()

DESCRIPTION
  This function is to copy the specified image on LCD screen or memory.
  Any overlapped image in the overlay list from other layer also gets
  displayed.

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  MDP_SUCCESSFUL if successful otherwise MDP_FAIL

SIDE EFFECTS
  caller of this function will get blocked until MDP finishes to update
  the image on LCD display

===========================================================================*/
uint32 mdp_disp_update_sub(MDPIMG *mdpImg, boolean mdpMemMode, uint16 *outBuf,
                           uint32 bufSzBytes, disp_cb_type * user_cb)
{
  LCD_TYPE destDev;
  MDPIMG_LIST *node, *bg_node,
  *fgListHead, *next_fgList;

  MDP_SCRCNT mdpScrCnt;
  uint32 *scrPtr, *startScrPtr, *linkAddr;
  uint32 mddi_sub_cnt;
  uint32 fgSubFlag;
  uint32 minLineNum;
  uint32 linkPos;
  MDP_SUBWIN_MDDI fgSubMddi;
  MDP_SUBWIN_MDDI bgSubMddi;
  MDPIMG orig_fgImg;
  MDPSCALE_INFO orig_scale;
  uint32 orig_fgMbcOp;
  uint32 alphaMask;
  uint32 alignOffset;
  uint32 vsync_flag;
  boolean orig_video_flag;
  boolean bg_video_flag;
  boolean orig_front_layer;
  MDP_OVER_XYInt this_cwin;
  boolean this_layer=TRUE;
  boolean mdp_needs_to_wait_till_done=TRUE;
  boolean linkVsync = FALSE;
  MDP_CWIN vsyncCwin = {0, 0, 0, 0};
   
  // Do not feature out this variable, still used in function prototypes
  //   even when FEATURE_MDP_WSCALE disabled.
  uint32 wScaleMdpOp = MDPOP_NOP;

#ifdef FEATURE_MDDI 
#error code not present
#endif /* FEATURE_MDDI */

//store the mem mode variable
  //we only want to calculate and store the hw update time for vsync if mem mode is disabled for update
  // if mem-mode is enabled, we ignore the time for this update
  mdp_update_memMode = mdpMemMode;

  if(!mdpMemMode){
  //store the width
  // we will calculate the height later

  last_update_width = sub_width; 
  last_update_height = 0;

   if(mdp_debug_vsync){
        MDP_MSG_HIGH("globalmemmode is true", 0, 0, 0);
        MDP_MSG_HIGH("update_width at beginning of function is %u", last_update_width, 0, 0);
   }
  }
  else{
     MDP_MSG_HIGH("globalmemmode is false", 0, 0, 0);
  }

 
 

  if ((uint32)(fgListHead = (MDPIMG_LIST *) mdp_cal_ovimg_rec(mdpImg,
                                                              FALSE,
                                                              &fgSubFlag,
                                                              &orig_fgImg,
                                                              &orig_scale,
                                                              &orig_fgMbcOp,
                                                              &alphaMask,
                                                              &vsync_flag,
                                                              &this_cwin,
                                                              this_layer)) == MDP_FAIL)
  {
    MSG_ERROR("mdp_disp_update() Opcode = 0x%x Layer = 0x%x Addr = 0x%x  ",
              mdpImg->mdpOp,mdpImg->layer,mdpImg->bmy_addr);
    MSG_ERROR("**** failed in mdp_cal_ovimg_rec() from mdp_disp_update() ****",0,0,0);
    return MDP_FAIL;
  }

#ifdef FEATURE_MDDI
#error code not present
#endif /* FEATURE_MDDI */

#ifdef FEATURE_MDP_WSCALE
  wScaleMdpOp = mdpImg->mdpOp & MDPOP_WSCALE_MASK;
#endif // #ifdef FEATURE_MDP_WSCALE

  // retrieved list of nodes can be changed through MDP API
  // need crit_sect protection
  rex_enter_crit_sect(&mdp_crit_sect);

  mdp_dither_on = (mdpImg->mdpOp & MDPOP_DITHER_OFF) ? FALSE:TRUE;

/* The following is the workaround for the design of some manufacturer's MDDI 
   client display drivers where the windowing information in the video packet
   is ignored and some extra register write packets are needed to define the 
   window -- psanghvi
*/

#ifdef MDDI_HOST_WINDOW_WORKAROUND
#error code not present
#endif 

/* End workaround -- psanghvi */

  if (mdpMemMode)
  {
    destDev = MEM_WR;
    // disabling ALPHA Blend for memory dump for layer 2 and 1.
    // This is due to the fact that layer 1 transparent color 
    // must be copied into the entire layer 1 buffer.
    // And we don't want to do any alphab with this tp color.
    if (mdpImg->dest == PRIMARY_LCD_TYPE)
    {
      if (mdpImgTailPtr_1[MDP_LAYER_1] != NULL)
      {
        alphaMask = ~MDPOP_ALPHAB;
      }
    }
    else
    {
      if (mdpImgTailPtr_2[MDP_LAYER_1] != NULL)
      {
        alphaMask = ~MDPOP_ALPHAB;
      }
    }

  }
  else
  {
    destDev = mdpImg->dest;
  }

  /* send it out to MDP */
  mdp_reset_scrcnt(&mdpScrCnt);
  startScrPtr = linkAddr = (uint32 *)0;
  scrPtr = (uint32 *) MDP_FAIL;

  next_fgList = fgListHead;
  lcd_y = mdpImg->lcd_y + BREW_OFFSET_Y; 
  /*lcd_y is the estimated LCD read pointer location*/
  /*lcd_x can be thought to be the exact pixel location
  on a line, which we are not bothered about now. 
  So, lcd_x is not being calculated*/

#ifdef MDDI_HOST_WINDOW_WORKAROUND
#error code not present
#endif /* MDDI_HOST_WINDOW_WORKAROUND */

  while (next_fgList != NULL)
  {
    node = next_fgList;
    //---- peak the address alignment -----
    //mddi path has a limitation of 16 line per update(or script)
    //and we must break down the display area into multiple of stripes(16lines/1stripe).
    //And also, MDP requires memory addres to be word-aligned boundary (>0x3)
    //If we detect the starting address is mis-aligned, we need to aligned the first
    //stripe and continue with 16 line break-down for the rest of stripes
    alignOffset = 0;
    if ((mdp_mddi_bypass_16lines != TRUE) && (orig_fgImg.mdpOp&MDPOP_ROT90) && (!mdpMemMode))
    {
      mdp_mddi_sub_cal(&node->img.cwin, &fgSubMddi, 0, 0, TRUE, 0);
      mdp_adjust_XY(mdpImg,
                    &fgSubMddi.x1,
                    &fgSubMddi.x2,
                    &fgSubMddi.y1,
                    &fgSubMddi.y2,
                    &orig_scale);

      // 100 (binary) = 4
      if (orig_fgImg.imgType == MDP_YCBCR420)
      {
        if (orig_fgImg.mdpOp&MDPOP_UD)
        {
          if ((fgSubMddi.x2-15)&0x3)
          {
            alignOffset = (fgSubMddi.x2-15)&0x3;
          }
        }
        else if (fgSubMddi.x1&0x3)
        {
          alignOffset = 4 - ((fgSubMddi.x1)&0x3);
        }
      }
      else //for RGB565
      {
        if (orig_fgImg.mdpOp&MDPOP_UD)
        {
          if ((fgSubMddi.x2-15)&0x1)
          {
            alignOffset = (fgSubMddi.x2-15)&0x1;
          }
        }
        else if (fgSubMddi.x1&0x1)
        {
          alignOffset = 2 - ((fgSubMddi.x1)&0x1);
        }
      }

      alignOffset = (alignOffset*orig_scale.num)/orig_scale.den +
                    ((alignOffset*orig_scale.num)%orig_scale.den?1:0);
    }

#ifdef MDDI_HOST_WINDOW_WORKAROUND
#error code not present
#endif /* MDDI_HOST_WINDOW_WORKAROUND */

    if (node->bgPtr == NULL)
    {
      if (fgSubFlag)
      {
        mddi_sub_cnt = 0;
        while (mdp_mddi_sub_cal(&node->img.cwin, &fgSubMddi, mddi_sub_cnt++,
                                orig_scale.lineNum, (mdpMemMode)?TRUE:mdp_mddi_bypass_16lines, alignOffset)
               == MDP_SUCCESSFUL)
        {
#ifndef FEATURE_MDDI
	  sub_width = fgSubMddi.x2-fgSubMddi.x1+1;   
	  sub_height = fgSubMddi.y2-fgSubMddi.y1+1;

#else /*ifdef FEATURE_MDDI*/
#error code not present
#endif /* FEATURE_MDDI */

              mdp_adjust_XY(mdpImg,
                            &fgSubMddi.x1,
                            &fgSubMddi.x2,
                            &fgSubMddi.y1,
                            &fgSubMddi.y2,
                            &orig_scale);

              if ((fgSubMddi.x2-fgSubMddi.x1+1)*(fgSubMddi.y2-fgSubMddi.y1+1) < mdp_min_pixel_size)
              {
                continue;
              }

              if (orig_fgImg.imgType == MDP_YCBCR420)
              {
                scrPtr = (uint32 *)mdp_disp_vdo_sub(
                                                   destDev,
                                                   orig_fgImg.bmy_addr,
                                                   orig_fgImg.cbcr_addr,
                                                   orig_fgImg.width,
                                                   fgSubMddi.y2+1 /* height */,
                                                   fgSubMddi.x1,
                                                   fgSubMddi.x2,
                                                   fgSubMddi.y1,
                                                   fgSubMddi.y2,
                                                   fgSubMddi.lcdX,
                                                   fgSubMddi.lcdY,
                                                   (uint32 *)(mdpTempImgBuf->buffer),
                                                   orig_fgImg.mdpOp,
                                                   orig_scale.scale, // input VDB video(YUV) scaling
                                                   &wScaleMdpOp,  // VDB output QVGA->WQVGA scaling
                                                   linkAddr,
                                                   &mdpScrCnt);
                linkPos = MDP_VDO_LINK_POS;
              }
              else
              {
                scrPtr = (uint32 *)mdp_disp_gfx_sub(
                                                   destDev,
                                                   orig_fgImg.bmy_addr,
                                                   orig_fgImg.imgType,
                                                   orig_fgImg.width,
                                                   fgSubMddi.y2+1 /* height */,
                                                   fgSubMddi.x1,
                                                   fgSubMddi.x2,
                                                   fgSubMddi.y1,
                                                   fgSubMddi.y2,
                                                   fgSubMddi.lcdX,
                                                   fgSubMddi.lcdY,
                                                   (uint32 *)(mdpTempImgBuf->buffer),
                                                   orig_fgImg.mdpOp,
                                                   &wScaleMdpOp, // VDB output QVGA->WQVGA scaling
                                                   linkAddr,
                                                   &mdpScrCnt);
                linkPos = MDP_GFX_LINK_POS;
              }

              if ((uint32)scrPtr != MDP_FAIL)
              {
                if ((uint32)startScrPtr == 0)
                {
                  startScrPtr = scrPtr;
                }

                linkAddr = &scrPtr[linkPos];

#ifdef MDDI_HOST_WINDOW_WORKAROUND
#error code not present
#endif /* MDDI_HOST_WINDOW_WORKAROUND */

                if (mdpRuntimeDispMode)
                {
                  mdp_start_script(scrPtr, destDev, linkVsync, vsyncCwin);
                  mdp_wait_script_done();
                }

                if (mdpMemMode)
                {
                  mdp_start_script(scrPtr, destDev, linkVsync, vsyncCwin);
                  mdp_wait_script_done();

                  mdp_copyLayer565(fgSubMddi.lcdX,
                                fgSubMddi.lcdY,
                                fgSubMddi.x2 - fgSubMddi.x1+1,
                                fgSubMddi.y2 - fgSubMddi.y1+1,
                                orig_fgImg.mdpOp,
                                orig_scale,
                                wScaleMdpOp,
                                outBuf,
                                bufSzBytes);
                  mdp_scr_free(&mdpScrCnt);
                }
              }
#ifdef FEATURE_MDDI                  
#error code not present
#endif /* FEATURE_MDDI */         
        } // while (mdp_mddi_sub_cal())
      }
    }
    else
    {
      mddi_sub_cnt = 0;
      bg_node = node->bgPtr;

      if (bg_node->img.bmy_addr != NULL)
      {
        minLineNum = (orig_scale.lineNum < bg_node->scaleInfo.lineNum) ?
                     orig_scale.lineNum : bg_node->scaleInfo.lineNum;

        while (mdp_mddi_sub_cal(&node->img.cwin, &fgSubMddi, mddi_sub_cnt++,
                                minLineNum, (mdpMemMode)?TRUE:mdp_mddi_bypass_16lines, alignOffset)
               == MDP_SUCCESSFUL)
        {
#ifndef FEATURE_MDDI                  
          sub_width = fgSubMddi.x2-fgSubMddi.x1+1; //for s/w vsync on SC2x
          sub_height = fgSubMddi.y2-fgSubMddi.y1+1;
          
 #else /*#ifdef FEATURE_MDDI                  */
#error code not present
#endif /* FEATURE_MDDI */

              /* this overlay should in the same position */
              bgSubMddi = fgSubMddi;
              mdp_adjust_XY(mdpImg,
                            &fgSubMddi.x1,
                            &fgSubMddi.x2,
                            &fgSubMddi.y1,
                            &fgSubMddi.y2,
                            &orig_scale);

              if ((fgSubMddi.x2-fgSubMddi.x1+1)*(fgSubMddi.y2-fgSubMddi.y1+1) < mdp_min_pixel_size)
              {
                continue;
              }

              mdp_adjust_XY(&bg_node->orig_img,
                            &bgSubMddi.x1,
                            &bgSubMddi.x2,
                            &bgSubMddi.y1,
                            &bgSubMddi.y2,
                            &bg_node->scaleInfo);

              orig_video_flag = (orig_fgImg.imgType == MDP_YCBCR420);
              bg_video_flag = (bg_node->img.imgType == MDP_YCBCR420);
              orig_front_layer = (orig_fgImg.layer == MDP_LAYER_2);

              /* if this is to clear overlay, we need to place the white image
                 on background always.  This is not true if the opposite image
                 has ROT90 operation since it must go through "peak the address 
                 alignment logic" */
              if ((mdp_mddi_bypass_16lines == TRUE) || !(bg_node->img.mdpOp&MDPOP_ROT90) || (mdpMemMode))
              {
                if (mdpImg->mdpOp & MDPOP_DISABLE_ALPHAB)
                {
                  orig_front_layer = FALSE;          
                  alphaMask = 0;
                }
              }


              if (orig_video_flag&&orig_front_layer)
              {
                scrPtr = (uint32 *)mdp_disp_vdo_sub(
                                                   destDev,
                                                   orig_fgImg.bmy_addr,
                                                   orig_fgImg.cbcr_addr,
                                                   orig_fgImg.width,
                                                   fgSubMddi.y2+1,
                                                   fgSubMddi.x1,
                                                   fgSubMddi.x2,
                                                   fgSubMddi.y1,
                                                   fgSubMddi.y2,
                                                   fgSubMddi.lcdX,
                                                   fgSubMddi.lcdY,
                                                   (uint32 *)(mdpTempImgBuf->buffer),
                                                   orig_fgImg.mdpOp,
                                                   orig_scale.scale, // input VDB video(YUV) scaling
                                                   &wScaleMdpOp, // VDB output QVGA->WQVGA scaling
                                                   linkAddr,
                                                   &mdpScrCnt);
                linkPos = MDP_VDO_LINK_POS;
              }
              else if (bg_video_flag&&!orig_front_layer)
              {
                scrPtr = (uint32 *)mdp_disp_vdo_sub(
                                                   destDev,
                                                   bg_node->img.bmy_addr,
                                                   bg_node->img.cbcr_addr,
                                                   bg_node->img.width,
                                                   bgSubMddi.y2+1,
                                                   bgSubMddi.x1,
                                                   bgSubMddi.x2,
                                                   bgSubMddi.y1,
                                                   bgSubMddi.y2,
                                                   bgSubMddi.lcdX,
                                                   bgSubMddi.lcdY,
                                                   (uint32 *)(mdpTempImgBuf->buffer),
                                                   bg_node->img.mdpOp,
                                                   bg_node->scaleInfo.scale, // input VDB video(YUV) scaling
                                                   &wScaleMdpOp, // VDB output QVGA->WQVGA scaling
                                                   linkAddr,
                                                   &mdpScrCnt);
                linkPos = MDP_VDO_LINK_POS;
              }
              else if (!orig_video_flag&&bg_video_flag&&orig_front_layer)
              {

#ifdef FEATURE_MDP_WSCALE
                // Make sure all updates from FG or BG behave the same for wscale
                wScaleMdpOp |= (bg_node->orig_img.mdpOp | orig_fgImg.mdpOp) & MDPOP_WSCALE_MASK;
#endif // #ifdef FEATURE_MDP_WSCALE

                scrPtr = (uint32 *) mdp_disp_gov_sub
                         (destDev,
                          bg_node->img.bmy_addr,
                          bg_node->img.cbcr_addr,
                          bg_node->img.width,
                          bgSubMddi.y2+1,
                          bgSubMddi.x1, bgSubMddi.x2,
                          bgSubMddi.y1, bgSubMddi.y2,
                          bgSubMddi.lcdX, bgSubMddi.lcdY, 
                          (uint32 *)(mdpTempImgBuf->buffer),
                          bg_node->img.mdpOp,

                          orig_fgImg.bmy_addr,
                          orig_fgImg.imgType,
                          orig_fgImg.width,
                          fgSubMddi.y2+1,
                          fgSubMddi.x1, fgSubMddi.x2,
                          fgSubMddi.y1, fgSubMddi.y2,
                          orig_fgImg.mdpOp,
                          bg_node->scaleInfo.scale, // input VDB video(YUV) scaling
                          &wScaleMdpOp, // VDB output QVGA->WQVGA scaling
                          orig_fgMbcOp&alphaMask,
                          orig_fgImg.tpVal,
                          orig_fgImg.alpha,
                          0x100 - orig_fgImg.alpha,
                          linkAddr, &mdpScrCnt, &linkPos);
              }
              else if (orig_video_flag&&!bg_video_flag&&!orig_front_layer)
              {

#ifdef FEATURE_MDP_WSCALE
                // Make sure all updates from FG or BG behave the same for wscale
                wScaleMdpOp |= (bg_node->orig_img.mdpOp | orig_fgImg.mdpOp) & MDPOP_WSCALE_MASK;
#endif // #ifdef FEATURE_MDP_WSCALE

                scrPtr = (uint32 *) mdp_disp_gov_sub
                         (destDev,
                          orig_fgImg.bmy_addr,
                          orig_fgImg.cbcr_addr,
                          orig_fgImg.width,
                          fgSubMddi.y2+1,
                          fgSubMddi.x1, fgSubMddi.x2,
                          fgSubMddi.y1, fgSubMddi.y2,
                          fgSubMddi.lcdX, fgSubMddi.lcdY,
                          (uint32 *)(mdpTempImgBuf->buffer),
                          orig_fgImg.mdpOp,

                          bg_node->img.bmy_addr,
                          bg_node->img.imgType,
                          bg_node->img.width,
                          bgSubMddi.y2+1,
                          bgSubMddi.x1, bgSubMddi.x2,
                          bgSubMddi.y1, bgSubMddi.y2,
                          bg_node->img.mdpOp,
                          orig_scale.scale, // input VDB video(YUV) scaling
                          &wScaleMdpOp, // VDB output QVGA->WQVGA scaling
                          bg_node->mbcOp&alphaMask,
                          bg_node->img.tpVal,
                          bg_node->img.alpha,                        
                          0x100 - bg_node->img.alpha,
                          linkAddr, &mdpScrCnt, &linkPos);
              }
              else if (!orig_video_flag&&!bg_video_flag&&orig_front_layer)
              {

#ifdef FEATURE_MDP_WSCALE
                // Make sure all updates from FG or BG behave the same for wscale
                wScaleMdpOp |= (bg_node->orig_img.mdpOp | orig_fgImg.mdpOp) & MDPOP_WSCALE_MASK;
#endif // #ifdef FEATURE_MDP_WSCALE

                scrPtr = (uint32 *) mdp_disp_gog_sub
                         (destDev,
                          orig_fgImg.bmy_addr,
                          orig_fgImg.imgType,
                          orig_fgImg.width,
                          fgSubMddi.y2+1,
                          fgSubMddi.x1, fgSubMddi.x2,
                          fgSubMddi.y1, fgSubMddi.y2,
                          fgSubMddi.lcdX, fgSubMddi.lcdY, 
                          (uint32 *)(mdpTempImgBuf->buffer),
                          orig_fgImg.mdpOp,

                          bg_node->img.bmy_addr,
                          bg_node->img.imgType,
                          bg_node->img.width,
                          bgSubMddi.y2+1,
                          bgSubMddi.x1,
                          bgSubMddi.y1,
                          bg_node->img.mdpOp,
                          &wScaleMdpOp, // VDB output QVGA->WQVGA scaling
                          orig_fgMbcOp&alphaMask,
                          orig_fgImg.tpVal,
                          orig_fgImg.alpha,
                          0x100 - orig_fgImg.alpha,
                          linkAddr, &mdpScrCnt, &linkPos);
              }
              else if (!orig_video_flag&&!bg_video_flag&&!orig_front_layer)
              {

#ifdef FEATURE_MDP_WSCALE
                // Make sure all updates from FG or BG behave the same for wscale
                wScaleMdpOp |= (bg_node->orig_img.mdpOp | orig_fgImg.mdpOp) & MDPOP_WSCALE_MASK;
#endif // #ifdef FEATURE_MDP_WSCALE

                scrPtr = (uint32 *) mdp_disp_gog_sub
                         (destDev,
                          bg_node->img.bmy_addr,
                          bg_node->img.imgType,
                          bg_node->img.width,
                          bgSubMddi.y2+1,
                          bgSubMddi.x1, bgSubMddi.x2,
                          bgSubMddi.y1, bgSubMddi.y2,
                          bgSubMddi.lcdX, bgSubMddi.lcdY, 
                          (uint32 *)(mdpTempImgBuf->buffer),
                          bg_node->img.mdpOp,

                          orig_fgImg.bmy_addr,
                          orig_fgImg.imgType,
                          orig_fgImg.width,
                          fgSubMddi.y2+1,
                          fgSubMddi.x1,
                          fgSubMddi.y1,
                          orig_fgImg.mdpOp,
                          &wScaleMdpOp, // VDB output QVGA->WQVGA scaling
                          bg_node->mbcOp&alphaMask,
                          bg_node->img.tpVal,
                          bg_node->img.alpha,
                          0x100 - bg_node->img.alpha,
                          linkAddr, &mdpScrCnt, &linkPos);
              }
              else
              {
                scrPtr = MDP_FAIL;
              }



              if ((uint32)scrPtr != MDP_FAIL)
              {
                if ((uint32)startScrPtr == 0)
                {
                  startScrPtr = scrPtr;
                }
                linkAddr = &scrPtr[linkPos];

#ifdef MDDI_HOST_WINDOW_WORKAROUND
#error code not present
#endif /* MDDI_HOST_WINDOW_WORKAROUND */

                if (mdpRuntimeDispMode)
                {
                  mdp_start_script(scrPtr, destDev, linkVsync, vsyncCwin);
                  mdp_wait_script_done();
                }

                if (mdpMemMode)
                {
                  mdp_start_script(scrPtr, destDev, linkVsync, vsyncCwin);
                  mdp_wait_script_done();

                  mdp_copyLayer565(bgSubMddi.lcdX,
                                bgSubMddi.lcdY,
                                bgSubMddi.x2 - bgSubMddi.x1+1,
                                bgSubMddi.y2 - bgSubMddi.y1+1,
                                bg_node->img.mdpOp,
                                orig_scale,
                                wScaleMdpOp,
                                outBuf,
                                bufSzBytes);
                  mdp_scr_free(&mdpScrCnt);

                }
              }
#ifdef FEATURE_MDDI                  
#error code not present
#endif /* FEATURE_MDDI */                   
        } // while (mdp_mddi_sub_cal())
      }
    }

#ifdef MDDI_HOST_WINDOW_WORKAROUND
#error code not present
#endif /* MDDI_HOST_WINDOW_WORKAROUND */

    next_fgList = next_fgList->next;
  } // while (next_fgList != NULL)

  if ((startScrPtr)&&(!mdpMemMode)&&(!mdpRuntimeDispMode)
#ifdef MDDI_HOST_WINDOW_WORKAROUND
#error code not present
#endif /* MDDI_HOST_WINDOW_WORKAROUND */     
     )
  {
    /* callback setup before we kick off MDP */
    if ((NULL != user_cb) && (NULL != user_cb->cb_func))
    {
      mdp_user_cb = *user_cb;
      user_cb->cb_func = NULL;
      mdp_needs_to_wait_till_done = FALSE;
    }
    else
    {
      mdp_needs_to_wait_till_done = TRUE;    
    }

    /* Add the vsync settings */
    linkVsync = (boolean)(vsync_flag);
    vsyncCwin = fgListHead->img.cwin;

    /* set tcb_ptr, clear sig, vote no sleep, job cnt++ and send */
    mdp_start_script(startScrPtr, destDev, linkVsync, vsyncCwin);

    /* opening MDP API */   
    rex_leave_crit_sect(&mdp_crit_sect);


    /* isr will reset tcb_ptr, set sig, vote to sleep, job cnt-- */
    if (mdp_needs_to_wait_till_done)
      mdp_wait_script_done();
  }
  else
  {
    /* opening MDP API */     
    rex_leave_crit_sect(&mdp_crit_sect);  
  }

  mdp_imglist_purge(fgListHead);
  mdp_scr_free(&mdpScrCnt);

  return MDP_SUCCESSFUL;
}

/*===========================================================================

FUNCTION mdp_mem_block_copy()

DESCRIPTION
  This function is to copy 16bit data from memory(src) to memory(dst)
  Source and Destination memory space is defined in (width*height) and
  the minimum space of (width*height) is 32bit or 4bytes.

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  MDP_SUCCESSFUL if successful otherwise MDP_FAIL

SIDE EFFECTS
  None

===========================================================================*/

int mdp_mem_block_copy(uint16 *src_addr,
                       uint32 src_width,
                       uint32 src_height,
                       uint16 *dst_addr)
{
  MDPIMG mdpImg;
  int result;

  result = MDP_FAIL;

  if (mdp_drv_initialized != TRUE)
    return result;

  rex_enter_crit_sect(&mdp_crit_sect);

  mdpImg.dest = MEM_WR;
  mdpImg.imgType = MDP_RGB565;
  mdpImg.bmy_addr = (uint32 *)src_addr;
  mdpImg.width = src_width;
  mdpImg.cwin.x1 = 0;
  mdpImg.cwin.x2 = src_width-1;
  mdpImg.cwin.y1 = 0;
  mdpImg.cwin.y2 = src_height-1;
  mdpImg.mdpOp = MDPOP_NOP;

  result = mdp_memcpy_img(&mdpImg, dst_addr);

  rex_leave_crit_sect(&mdp_crit_sect);

  return result;
}

/*===========================================================================

FUNCTION mdp_memcpy_img()

DESCRIPTION
  This function is to copy a single MDP image into memory.
  Transparent/AlphaBlend operation is not supported through this interface.

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  MDP_SUCCESSFUL if successful otherwise MDP_FAIL

SIDE EFFECTS
  None

===========================================================================*/

int mdp_memcpy_img(MDPIMG *img, uint16 *dst_addr)
{
  MDP_SCRCNT mdpScrCnt;
  uint32 *scrPtr;
  boolean linkVsync = FALSE;
  MDP_CWIN vsyncCwin = {0, 0, 0, 0};

  // Do not feature out this variable because this is used in function
  //   prototypes even if FEATURE_MDP_WSCALE is disabled.
  uint32 wScaleMdpOp = MDPOP_NOP;

  if (mdp_drv_initialized != TRUE)
    return MDP_FAIL;

  if ((img->cwin.x2-img->cwin.x1+1)*(img->cwin.y2-img->cwin.y1+1) < mdp_min_pixel_size)
  {
    return MDP_FAIL;
  }

#ifdef FEATURE_MDP_WSCALE
  wScaleMdpOp = img->mdpOp & MDPOP_WSCALE_MASK;
#endif // #ifdef FEATURE_MDP_WSCALE

  mdp_check_ccs(img);

  rex_enter_crit_sect(&mdp_crit_sect);

  mdp_reset_scrcnt(&mdpScrCnt);

  scrPtr = NULL;

  if (img->imgType != MDP_YCBCR420)
  {
    scrPtr = (uint32 *)mdp_disp_gfx_sub
             (MEM_WR,
              img->bmy_addr,
              img->imgType,
              img->width,
              img->cwin.y2+1,
              img->cwin.x1,
              img->cwin.x2,
              img->cwin.y1,
              img->cwin.y2,
              0,
              0,
              (uint32 *)dst_addr,
              img->mdpOp&MDPOP_FFRMASK,
              &wScaleMdpOp, // VDB output QVGA->WQVGA scaling
              NULL,
              &mdpScrCnt);
  }
  else
  {
    scrPtr = (uint32 *)mdp_disp_vdo_sub
             (MEM_WR,
              img->bmy_addr,
              img->cbcr_addr,
              img->width,
              img->cwin.y2+1,
              img->cwin.x1,
              img->cwin.x2,
              img->cwin.y1,
              img->cwin.y2,
              0,
              0,
              (uint32 *)dst_addr,
              img->mdpOp&MDPOP_FFRMASK,
              img->mdpOp&MDPOP_VDOMASK,  // input VDB video(YUV) scaling
              &wScaleMdpOp, // VDB output QVGA->WQVGA scaling
              NULL,
              &mdpScrCnt);
  }

  if (scrPtr != NULL)
  {
    /* set tcb_ptr, clear sig, vote no sleep, job cnt++ and send */
    mdp_start_script(scrPtr, MEM_WR, linkVsync, vsyncCwin);
    /* isr will reset tcb_ptr, set sig, vote to sleep, job cnt-- */
    mdp_wait_script_done();
  }
  mdp_scr_free(&mdpScrCnt);

  rex_leave_crit_sect(&mdp_crit_sect);

  return MDP_SUCCESSFUL;
}

/*===========================================================================

FUNCTION mdp_memcpy_dblimg()

DESCRIPTION
  This function is to copy two MDP images(foreground and background)
  into memory.  Transparent/AlphaBlend operation is supported through
  this interface.

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  MDP_SUCCESSFUL if successful otherwise MDP_FAIL

SIDE EFFECTS
  None

===========================================================================*/
int mdp_memcpy_dblimg(MDPIMG *fgImg, MDPIMG *bgImg, uint16 *dst_addr)
{
  MDP_SCRCNT mdpScrCnt;
  uint32 *scrPtr;
  uint32 linkPos;
  boolean linkVsync = FALSE;
  MDP_CWIN vsyncCwin = {0, 0, 0, 0};

  // Do not feature out this variable because it is still used in function
  //   prototypes even if FEATURE_MDP_WSCALE is disabled.
  uint32 wScaleMdpOp = MDPOP_NOP;

  if (mdp_drv_initialized != TRUE)
    return MDP_FAIL;

  if (fgImg->imgType == MDP_YCBCR420)
  {
    return MDP_FAIL;
  }

  if ((fgImg->cwin.x2-fgImg->cwin.x1+1)*(fgImg->cwin.y2-fgImg->cwin.y1+1) < mdp_min_pixel_size)
  {
    return MDP_FAIL;
  }

#ifdef FEATURE_MDP_WSCALE
  // Make sure all updates from FG or BG behave the same for wscale
  wScaleMdpOp = (bgImg->mdpOp | fgImg->mdpOp) & MDPOP_WSCALE_MASK;
#endif // #ifdef FEATURE_MDP_WSCALE

  mdp_check_ccs(bgImg);

  rex_enter_crit_sect(&mdp_crit_sect);

  mdp_reset_scrcnt(&mdpScrCnt);

  scrPtr = NULL;

  if (bgImg->imgType != MDP_YCBCR420)
  {
    scrPtr = (uint32 *)mdp_disp_gog_sub(MEM_WR,
                                        fgImg->bmy_addr,
                                        fgImg->imgType,
                                        fgImg->width,
                                        fgImg->cwin.y2+1,
                                        fgImg->cwin.x1,
                                        fgImg->cwin.x2,
                                        fgImg->cwin.y1,
                                        fgImg->cwin.y2,
                                        0,
                                        0,
                                        (uint32 *)dst_addr,
                                        fgImg->mdpOp&MDPOP_FFRMASK,
                                        bgImg->bmy_addr,
                                        bgImg->imgType,
                                        bgImg->width,
                                        bgImg->cwin.y2+1,
                                        bgImg->cwin.x1,
                                        bgImg->cwin.y1,
                                        bgImg->mdpOp&MDPOP_FFRMASK,
                                        // VDB output QVGA->WQVGA scaling
                                        &wScaleMdpOp,
                                        fgImg->mdpOp&MDPOP_MBCMASK,
                                        MDP_CONVTP(fgImg->tpVal),
                                        fgImg->alpha,
                                        0x100 - fgImg->alpha,
                                        NULL,
                                        &mdpScrCnt,
                                        &linkPos);
  }
  else
  {
    scrPtr = (uint32 *)mdp_disp_gov_sub(MEM_WR,
                                        bgImg->bmy_addr,
                                        bgImg->cbcr_addr,
                                        bgImg->width,
                                        bgImg->cwin.y2+1,
                                        bgImg->cwin.x1,
                                        bgImg->cwin.x2,
                                        bgImg->cwin.y1,
                                        bgImg->cwin.y2,
                                        0,
                                        0,
                                        (uint32 *)dst_addr,
                                        bgImg->mdpOp&MDPOP_FFRMASK,
                                        fgImg->bmy_addr,
                                        fgImg->imgType,
                                        fgImg->width,
                                        fgImg->cwin.y2+1,
                                        fgImg->cwin.x1,
                                        fgImg->cwin.x2,
                                        fgImg->cwin.y1,
                                        fgImg->cwin.y2,
                                        fgImg->mdpOp&MDPOP_FFRMASK,
                                        bgImg->mdpOp&MDPOP_VDOMASK, // input VDB video(YUV) scaling
                                        // VDB output QVGA->WQVGA scaling
                                        &wScaleMdpOp,
                                        fgImg->mdpOp&MDPOP_MBCMASK,
                                        MDP_CONVTP(fgImg->tpVal),
                                        fgImg->alpha,
                                        0x100 - fgImg->alpha,
                                        NULL,
                                        &mdpScrCnt,
                                        &linkPos);
  }

  if (scrPtr != NULL)
  {
    /* set tcb_ptr, clear sig, vote no sleep, job cnt++ and send */
    mdp_start_script(scrPtr, MEM_WR, linkVsync, vsyncCwin);
    /* isr will reset tcb_ptr, set sig, vote to sleep, job cnt-- */
    mdp_wait_script_done();
  }
  mdp_scr_free(&mdpScrCnt);
  rex_leave_crit_sect(&mdp_crit_sect);

  return MDP_SUCCESSFUL;
}

/*===========================================================================

FUNCTION mdp_disp_update_gfx()

DESCRIPTION
  This function is to display the specified image on LCD screen.
  No overlay is supported through this update.

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  MDP_SUCCESSFUL if successful otherwise MDP_FAIL

SIDE EFFECTS
  caller of this function will get blocked until MDP finishes to update
  the image on LCD display

===========================================================================*/

int mdp_disp_update_gfx(LCD_TYPE destDev,
                        uint32 *fg_image_addr,
                        MDPIMG_TYPE fg_imgType,
                        uint16 fg_width,
                        uint16 fg_x1_off,
                        uint16 fg_x2_off,
                        uint16 fg_y1_off,
                        uint16 fg_y2_off,
                        int32  fg_x,
                        int32  fg_y,
                        uint32 fg_mdpOp
                       )
{
  MDPIMG mdpImg;
  MDPSCALE_INFO orig_scale;
  MDP_SUBWIN_MDDI fgSubMddi;
  MDP_SCRCNT mdpScrCnt;
  /* lcd coordinate x&y */
  int32 fg_x1, fg_x2, fg_y1, fg_y2;
  uint16 fg_height = fg_y2_off+1;
  boolean linkVsync = FALSE;
  MDP_CWIN vsyncCwin = {0, 0, 0, 0};

  MDP_OVER_XYInt fgXY;
  uint32 *scrPtr, *startScrPtr, *linkAddr;
  uint32 i;

  // Do not feature out this variable because it is used in function
  //   prototypes even if FEATURE_MDP_WSCALE is disabled.
  uint32 wScaleMdpOp = MDPOP_NOP;

  if (destDev == MEM_WR)
  {
    return MDP_FAIL;
  }

  mdpImg.width = fg_width;
  mdpImg.cwin.x1 = fg_x1_off;
  mdpImg.cwin.x2 = fg_x2_off;
  mdpImg.cwin.y1 = fg_y1_off;
  mdpImg.cwin.y2 = fg_y2_off;
  mdpImg.mdpOp = fg_mdpOp;
  mdpImg.lcd_x = fg_x - MDP_SYS_OFFSET_X;
  mdpImg.lcd_y = fg_y - MDP_SYS_OFFSET_Y;

#ifdef FEATURE_MDP_WSCALE
  wScaleMdpOp = fg_mdpOp & MDPOP_WSCALE_MASK;
#endif // #ifdef FEATURE_MDP_WSCALE

  fg_mdpOp &= MDPOP_FFRMASK;

  rex_enter_crit_sect(&mdp_crit_sect);
  /* adjusting image window */
  fg_x -= fg_x1_off;
  fg_y -= fg_y1_off;

  if (fg_mdpOp&MDPOP_ROT90)
  {
    fg_x1 = fg_x + ((fg_height-1) - fg_y2_off);
    fg_y1 = fg_y + fg_x1_off;

    fg_x2 = fg_x + ((fg_height-1) - fg_y1_off);
    fg_y2 = fg_y + fg_x2_off;
  }
  else
  {

    fg_x1 = fg_x+fg_x1_off;
    fg_y1 = fg_y+fg_y1_off;

    fg_x2 = fg_x + fg_x2_off;
    fg_y2 = fg_y + fg_y2_off;
  }

  if (mdpCalOverXYInt(fg_x1,
                      fg_x2,
                      fg_y1,
                      fg_y2,
                      0,
                      mdpLcdSize[destDev][COL_IDX],
                      0,
                      mdpLcdSize[destDev][ROW_IDX],
                      &fgXY) == MDP_SUCCESSFUL)
  {

    /* send it out to MDP */
    mdp_reset_scrcnt(&mdpScrCnt);
    i=0;
    startScrPtr = linkAddr = (uint32 *)0;
    orig_scale.scale_flag = FALSE;
    while (mdp_mddi_sub_cal(&fgXY, &fgSubMddi, i++, 16, mdp_mddi_bypass_16lines, 0)
           == MDP_SUCCESSFUL)
    {
      mdp_adjust_XY(&mdpImg,
                    &fgSubMddi.x1,
                    &fgSubMddi.x2,
                    &fgSubMddi.y1,
                    &fgSubMddi.y2,
                    &orig_scale);

      if ((fgSubMddi.x2-fgSubMddi.x1+1)*(fgSubMddi.y2-fgSubMddi.y1+1) < mdp_min_pixel_size)
      {
        continue;
      }

      if ((uint32)(scrPtr = (uint32 *)mdp_disp_gfx_sub(destDev, fg_image_addr, fg_imgType,
                                                       fg_width, fg_height, fgSubMddi.x1, fgSubMddi.x2,
                                                       fgSubMddi.y1, fgSubMddi.y2, fgSubMddi.lcdX,
                                                       fgSubMddi.lcdY, NULL,fg_mdpOp,
                                                       &wScaleMdpOp,  // VDB output QVGA->WQVGA scaling
                                                       linkAddr, &mdpScrCnt))
          != MDP_FAIL)
      {
        if ((uint32)startScrPtr == 0)
        {
          startScrPtr = scrPtr;
        }

        linkAddr = &scrPtr[MDP_GFX_LINK_POS];
      }
      else
      {
        mdp_scr_free(&mdpScrCnt);
        rex_leave_crit_sect(&mdp_crit_sect);
        return MDP_FAIL;
      }
    }

    if (startScrPtr)
    {
      /* set tcb_ptr, clear sig, vote no sleep, job cnt++ and send */
      mdp_start_script(startScrPtr, destDev, linkVsync, vsyncCwin);
      /* isr will reset tcb_ptr, set sig, vote to sleep, job cnt-- */
      mdp_wait_script_done();
      mdp_scr_free(&mdpScrCnt);
    }
  }

  rex_leave_crit_sect(&mdp_crit_sect);
  return MDP_SUCCESSFUL;
}


/*===========================================================================

FUNCTION mdp_disp_gfx_sub()

DESCRIPTION
  This function is to compose RGB graphic display scripts

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  MDP_SUCCESSFUL if successful otherwise MDP_FAIL

SIDE EFFECTS
  None

===========================================================================*/
int mdp_disp_gfx_sub( LCD_TYPE destDev,
                      uint32 *image_addr,
                      MDPIMG_TYPE imgType,
                      uint16 width,
                      uint16 height,
                      uint16 x1,
                      uint16 x2,
                      uint16 y1,
                      uint16 y2,
                      uint16 lcd_col_start,
                      uint16 lcd_row_start,
                      uint32 *output_addr,
                      uint32 mdpOp,
                      uint32 *wScale,  // VDB output QVGA->WQVGA scaling
                      uint32 *linkAddr,
                      MDP_SCRCNT *mdpScrCnt)
{
  uint16 lcd_row_end;
  uint16 lcd_col_end;
  uint16 tmpRowEnd;
  uint16 vdbX1, vdbX2, vdbY1, vdbY2;

  uint8  ffr_mode;
  int    bytesPerPixel;

  uint32 *scrPtr, *lcdScrPtr, tmpImageAddr;

  MDP_FFR_DATA mdpFFRData;
  MDP_CROP_XY mdpCrop;
  uint16 x2_align;
  uint32 mbcOp;

#ifdef FEATURE_MDP_WSCALE
  uint32 wScaleOp;
#endif // #ifdef FEATURE_MDP_WSCALE
  
  /*Workarround for MDP hardware limitation when outputing 
    odd number of packed RGB565 pixels to memory*/
  if(lcdPhysDest[destDev] == MEM_WR)
  {
      /*If (x2-x1) is even, 1 is added to x1 and then 
	  (x2-x1+1) will be even, and vice versa.*/
      x1 += ((x2-x1+1)%2);
   
  } 
  lcd_col_end = lcd_col_start + x2-x1;
  lcd_row_end = lcd_row_start + y2-y1;
  
  if(last_update_height + sub_height <= mdp_prim_lcdHeight)  
    last_update_height += sub_height;

  if (mdpOp&MDPOP_ROT90)
  {
    tmpRowEnd = lcd_row_end;
    lcd_row_end = lcd_row_start + (lcd_col_end - lcd_col_start);
    lcd_col_end = lcd_col_start + (tmpRowEnd - lcd_row_start);
  }

#ifdef FEATURE_MDP_WSCALE
  // Set the MDP command for WSCALE output of VDB if applicable.
  switch (*wScale)
  {
    case MDPOP_WSCALE_LINEAR_STRETCH:
      wScaleOp = 0x37000001;
      break;
    case MDPOP_WSCALE_EDGE_STRETCH:
      wScaleOp = 0x37000002;
      break;
    case MDPOP_WSCALE_NO_STRETCH:
      wScaleOp = 0x37000003;
      break;
    case MDPOP_NOP:
      wScaleOp = NOP;
      *wScale = MDPOP_WSCALE_NOP;
      break;
    default:
      MDP_MSG_ERROR("Wscale Aborted! Unable to understand Wscale operations set!",0,0,0);
      MDP_MSG_ERROR("  Attempted wscale with MDPOP = 0x%08x",*wScale,0,0);
      wScaleOp = NOP;
      *wScale = MDPOP_WSCALE_NOP;
  } // switch (wScale)
  
  // Hard-coded values below due to HW being fixed for these values.
  //   WARNING: DO NOT MODIFY COL/ROW SETTINGS FOR WSCALE
  if (wScaleOp != NOP)
  {
    if (mdp_valid_img_for_wscale(lcd_col_start, lcd_col_end, lcd_row_start, lcd_row_end, *wScale))
    {
      lcd_col_end = MDP_WSCALE_OUT_WIDTH  + lcd_col_start - 1;
      lcd_row_end = MDP_WSCALE_OUT_HEIGHT + lcd_row_start - 1;
    }
    else
    {
      wScaleOp = NOP;
      *wScale = MDPOP_WSCALE_NOP;
    } // mdp_valid_img_for_wscale()

    // Work-around for w-scale offset issue.  Reset HW when we start this mode only.
    if (*wScale == MDPOP_WSCALE_NO_STRETCH)
    {
      if (resetMdpForWscaleNoStretch)
      {
        mdp_reset();
        resetMdpForWscaleNoStretch = FALSE;
      }
    }
    else
    {
      resetMdpForWscaleNoStretch = TRUE;
    }
  } // (wScaleOp != NOP)
#endif // #ifdef FEATURE_MDP_WSCALE

  // Find the operation for SET_FFR_MODE below
  // Find the bpp for cropping window adjustment
  if (imgType == MDP_XRGB_8888)
  {
    ffr_mode = 3;       // gfx_xrgb (RGB888 padded) over nothing op
    bytesPerPixel = 4;  // adjust address by 4 byte increments
  }
//  else if (imgType == MDP_RGB_888)  No 888 packed support
//  {
//    ffr_mode = 2;       // gfx_rgb (RGB888) over nothing operation
//    bytesPerPixel = 3;  // adjust address by 3 byte increments
//  }
  else
  {
    ffr_mode = 1;       // gfx_565 over nothing operation
    bytesPerPixel = 2;  // adjust address by 2 byte increments
  }

  /* adjusting fg_image to start at (0,0)*/
  /* this is also software-workaround for MDP bug */
  /* if you have some x/y crop value, MDP doesn't display correctly */

  if (x1 || y1)
  {
    tmpImageAddr = (uint32)image_addr + x1*bytesPerPixel + y1*bytesPerPixel*width;
    image_addr = (uint32 *)tmpImageAddr;

    x2 -= x1;
    y2 -= y1;
    x1 = y1 = 0;
  }

  x2_align = (((uint32)image_addr)&0x03)/2;
  x2 += x2_align;

  vdbX1 = (x1/16)*16;
  vdbX2 = (x2/16)*16+16-1;
  vdbY1 = (y1/16)*16;
  vdbY2 = (y2/16)*16+16-1;

  if ((scrPtr = mdp_scr_get(MDP_GFX_SCR)) == NULL)
  {
    return MDP_FAIL;
  }
  mdpScrCnt->gfx_scr_cnt++;

  if (mdpCalFFRdataFg(image_addr,
                      mdpOp,
                      width,
                      vdbX1,
                      vdbX2,
                      vdbY1,
                      vdbY2,
                      &mdpFFRData,
                      imgType) != MDP_SUCCESSFUL)
  {
    return MDP_FAIL;
  }

#ifdef FEATURE_MDP_WSCALE
  scrPtr[10] = wScaleOp;
#endif // #ifdef FEATURE_MDP_WSCALE

  scrPtr[3] = mdpFFRData.imgAddr;
  scrPtr[4] = mdpFFRData.line;
  scrPtr[5] = mdpFFRData.block;
  scrPtr[6] = mdpFFRData.stripe;

  scrPtr[8] = SET_FFR_MODE(0, ffr_mode, 0, mdpOp); //8 operation - gfx_565 over nothing; gfx and video orient - no flip/rotate

  mbcOp = mdp_dither_on ? SET_MBC_ACTION_DITHER:0;
  mbcOp |= mdp_gamma_enable;

  scrPtr[9] = SET_MBC_ACTION(mbcOp);

  switch (lcdPhysDest[destDev] )
  {
    case MEM_WR:
      scrPtr[7] = SET_LCD_FORMAT(1, 0x11);
      scrPtr[14] = SET_LCD_DATA_ADDR;

      if ((output_addr == 0) || ((uint32)output_addr > MAX_MEM_LIMIT))
        return MDP_FAIL;

      // Add 0x1 for destination address auto-increment
      scrPtr[15] = ((uint32) output_addr) + 0x1;

      scrPtr[18] = NOP;
      scrPtr[22] = WAIT_JOB_DONE;
      break;

#ifndef CUST_EDITION
    case EP_OP_QCIFP_PRI:
      if ((lcdScrPtr = mdp_scr_get(MDP_EP_OP_QCIFP_PRI_SCR)) == NULL)
        return MDP_FAIL;
      mdpScrCnt->ep_op_qcifp_pri_scr_cnt++;
  
      scrPtr[7] = SET_LCD_FORMAT(0, 0);
      scrPtr[14] = SUBROUTINE;
      scrPtr[15] = (uint32) lcdScrPtr;
  
  
      lcdScrPtr[7] = SEND_LCD_DATA(lcd_col_start << 10);
      lcdScrPtr[8] = SEND_LCD_DATA(lcd_row_start << 10);
    
      lcdScrPtr[10] = SEND_LCD_DATA(lcd_col_end << 10);
      lcdScrPtr[11] = SEND_LCD_DATA(lcd_row_end << 10);
      break;


    case EP_OP_QCIFP_SEC:
      if ((lcdScrPtr = mdp_scr_get(MDP_EP_OP_QCIFP_SEC_SCR)) == NULL)
        return MDP_FAIL;
      mdpScrCnt->ep_op_qcifp_sec_scr_cnt++;
  
      scrPtr[7] = SET_LCD_FORMAT(0, 0 );
      scrPtr[14] = SUBROUTINE;
      scrPtr[15] = (uint32) lcdScrPtr;
  
      lcdScrPtr[7] = SEND_LCD_DATA(lcd_col_start << 10);
      lcdScrPtr[8] = SEND_LCD_DATA(lcd_row_start << 10);
  
      lcdScrPtr[10] = SEND_LCD_DATA(lcd_col_end << 10);
      lcdScrPtr[11] = SEND_LCD_DATA(lcd_row_end << 10);
      break;


  case EPSON_QCIF:
      if ((lcdScrPtr = mdp_scr_get(MDP_EPSON_QCIF_SCR)) == NULL)
        return MDP_FAIL;
      mdpScrCnt->epson_qcif_scr_cnt++;

      scrPtr[7] = SET_LCD_FORMAT(0, 8);
      scrPtr[14] = SUBROUTINE;
      scrPtr[15] = (uint32) lcdScrPtr;

      scrPtr[18] = NOP;
      scrPtr[22] = WAIT_JOB_DONE;

      lcdScrPtr[7] = SEND_LCD_DATA(lcd_row_start & 0xFF);
      lcdScrPtr[8] = SEND_LCD_DATA(lcd_row_start >> 8);

      lcdScrPtr[9] = SEND_LCD_DATA(lcd_row_end & 0xFF);
      lcdScrPtr[10] = SEND_LCD_DATA(lcd_row_end >> 8);

      lcdScrPtr[13] = SEND_LCD_DATA((lcd_col_start+2) & 0xFF);
      lcdScrPtr[14] = SEND_LCD_DATA((lcd_col_start+2) >> 8);

      lcdScrPtr[15] = SEND_LCD_DATA((lcd_col_end+2) & 0xFF);
      lcdScrPtr[16] = SEND_LCD_DATA((lcd_col_end+2) >> 8);
      break;
   
	case EPSON_QVGA:
      if ((lcdScrPtr = mdp_scr_get(MDP_EPSON_QVGA_SCR)) == NULL)
        return MDP_FAIL;
      mdpScrCnt->epson_qvga_scr_cnt++;

      scrPtr[7] = SET_LCD_FORMAT(0, 0x2);
      scrPtr[14] = SUBROUTINE;
      scrPtr[15] = (uint32) lcdScrPtr;

      scrPtr[18] = NOP;
      scrPtr[22] = WAIT_JOB_DONE;

      lcdScrPtr[7] = SEND_LCD_DATA((uint8)(lcd_col_start)      << 1);
      lcdScrPtr[8] = SEND_LCD_DATA((uint8)(lcd_row_start >> 8) << 1);
      lcdScrPtr[9] = SEND_LCD_DATA((uint8)(lcd_row_start)      << 1);

      lcdScrPtr[12] = SEND_LCD_DATA((uint8)(lcd_col_end)      << 1);
      lcdScrPtr[13] = SEND_LCD_DATA((uint8)(lcd_row_end >> 8) << 1);
      lcdScrPtr[14] = SEND_LCD_DATA((uint8)(lcd_row_end)      << 1);

      if(mdp_toggle_output_format_enable)
        lcdScrPtr[18] = SET_LCD_FORMAT(0, 0x16);
      else
        lcdScrPtr[18] = NOP;

      break;

    case TMD_QVGA:
      if ((lcdScrPtr = mdp_scr_get(MDP_TMD_QVGA_SCR)) == NULL)
        return MDP_FAIL;
      mdpScrCnt->tmd_qvga_scr_cnt++;

      scrPtr[7] = SET_LCD_FORMAT(0, 0);
      scrPtr[14] = SUBROUTINE;
      scrPtr[15] = (uint32) lcdScrPtr;

      scrPtr[18] = NOP;
      scrPtr[22] = WAIT_JOB_DONE;

      lcdScrPtr[7] = SEND_LCD_DATA(DISP_QVGA_18BPP(lcd_col_start + tmd20qvga_panel_offset));
      lcdScrPtr[9] = SEND_LCD_DATA(DISP_QVGA_18BPP(lcd_col_end + tmd20qvga_panel_offset));
      lcdScrPtr[11] = SEND_LCD_DATA(DISP_QVGA_18BPP(lcd_row_start));
      lcdScrPtr[13] = SEND_LCD_DATA(DISP_QVGA_18BPP(lcd_row_end));

      lcdScrPtr[15] = SEND_LCD_DATA(DISP_QVGA_18BPP(lcd_col_start + tmd20qvga_panel_offset));
      lcdScrPtr[17] = SEND_LCD_DATA(DISP_QVGA_18BPP(lcd_row_start));

      /*
      This is same as above.  This shows actual logic.
      Let's use directly as above for better perofrmance

      lcdScrPtr[15] = SEND_LCD_DATA(DISP_QVGA_18BPP(DISP_QVGA_VAL_IF(dst_start_addr & 0x00080, DISP_QVGA_BITMASK_AD7) | DISP_QVGA_VAL_IF(dst_start_addr & 0x00040, DISP_QVGA_BITMASK_AD6) | DISP_QVGA_VAL_IF(dst_start_addr & 0x00020, DISP_QVGA_BITMASK_AD5) | DISP_QVGA_VAL_IF(dst_start_addr & 0x00010, DISP_QVGA_BITMASK_AD4) | DISP_QVGA_VAL_IF(dst_start_addr & 0x00008, DISP_QVGA_BITMASK_AD3) | DISP_QVGA_VAL_IF(dst_start_addr & 0x00004, DISP_QVGA_BITMASK_AD2) | DISP_QVGA_VAL_IF(dst_start_addr & 0x00002, DISP_QVGA_BITMASK_AD1) | DISP_QVGA_VAL_IF(dst_start_addr & 0x00001, DISP_QVGA_BITMASK_AD0)));
      lcdScrPtr[17] = SEND_LCD_DATA(DISP_QVGA_18BPP(DISP_QVGA_VAL_IF(dst_start_addr & 0x10000, DISP_QVGA_BITMASK_AD16) | DISP_QVGA_VAL_IF(dst_start_addr & 0x08000, DISP_QVGA_BITMASK_AD15) | DISP_QVGA_VAL_IF(dst_start_addr & 0x04000, DISP_QVGA_BITMASK_AD14) | DISP_QVGA_VAL_IF(dst_start_addr & 0x02000, DISP_QVGA_BITMASK_AD13) | DISP_QVGA_VAL_IF(dst_start_addr & 0x01000, DISP_QVGA_BITMASK_AD12) | DISP_QVGA_VAL_IF(dst_start_addr & 0x00800, DISP_QVGA_BITMASK_AD11) | DISP_QVGA_VAL_IF(dst_start_addr & 0x00400, DISP_QVGA_BITMASK_AD10) | DISP_QVGA_VAL_IF(dst_start_addr & 0x00200, DISP_QVGA_BITMASK_AD9)  | DISP_QVGA_VAL_IF(dst_start_addr & 0x00100, DISP_QVGA_BITMASK_AD8)));
      */
      break;
#else

    case MDP_LCD:
      if ((lcdScrPtr = mdp_scr_get(MDP_LCD_SCR)) == NULL)
        return MDP_FAIL;
      mdpScrCnt->lcd_scr_cnt++;
      
      scrPtr[7] = SET_LCD_FORMAT(0, disp_drv_mdp_getformat());
      scrPtr[14] = SUBROUTINE;
      scrPtr[15] = (uint32) lcdScrPtr;

      scrPtr[18] = NOP;
      scrPtr[22] = WAIT_JOB_DONE;
      
      disp_drv_mdp_scrupdate(lcdScrPtr, lcd_row_start, lcd_col_start, lcd_row_end, lcd_col_end);
      break;
#endif

#ifdef FEATURE_MDDI
#error code not present
#endif /* FEATURE_MDDI */

    default: return MDP_FAIL;
  }

  mdpCalCropXY(x1+x2_align, x2, y1, y2, vdbX1, vdbX2, vdbY1, vdbY2, mdpOp, &mdpCrop);

  scrPtr[11] = SET_VDB_CROP_X(mdpCrop.cx1, mdpCrop.cx2);
  scrPtr[12] = SET_VDB_CROP_Y(mdpCrop.cy1, mdpCrop.cy2);

  // 1: enable MDP pipleline, stripe/image -1, #ofBlocks/stripe -1
  // number of lines per stripe = 16
  // 1 block = 16pixel x 16pixel

  if (mdpOp&MDPOP_ROT90)
  {
    scrPtr[13] = SET_VDB_SIZE(mdp_sfd_enable,
                              (vdbX2-vdbX1)/16,
                              (vdbY2-vdbY1)/16);
  }
  else
  {
    scrPtr[13] = SET_VDB_SIZE(mdp_sfd_enable,
                              (vdbY2-vdbY1)/16,
                              (vdbX2-vdbX1)/16);
  }

  scrPtr[17] = MAX_MEM_LIMIT;
  scrPtr[MDP_GFX_LINK_POS] = NOP;
  scrPtr[MDP_GFX_LINK_POS+1] = NOP;

  if (linkAddr)
  {
    linkAddr[0] = JUMP;
    linkAddr[1] = (uint32)scrPtr;
  }

  return(int)scrPtr;
}


/*===========================================================================

FUNCTION mdp_disp_vdo_sub()

DESCRIPTION
  This function is to compose YCbCr video display scripts

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  MDP_SUCCESSFUL if successful otherwise MDP_FAIL

SIDE EFFECTS
  None

===========================================================================*/
int mdp_disp_vdo_sub( LCD_TYPE destDev,
                      uint32 *Y_addr,
                      uint32 *CbCr_addr,
                      uint16 width,
                      uint16 height,
                      uint16 x1,
                      uint16 x2,
                      uint16 y1,
                      uint16 y2,
                      uint16 lcd_col_start,
                      uint16 lcd_row_start,
                      uint32 *output_addr,
                      uint32 mdpOp,
                      uint32 scale,
                      uint32 *wScale,
                      uint32 *linkAddr,
                      MDP_SCRCNT *mdpScrCnt)
{
  uint16 orig_cx1, orig_cy1;
  uint16 lcd_row_end;
  uint16 lcd_col_end;
  uint16 tmpRowEnd, num, den, ufac, vDiv;
  uint16 vdbX1, vdbX2, vdbY1, vdbY2;
  uint8 ffrOpCode;

  uint32 *scrPtr, *lcdScrPtr, tmpImageAddr;

  MDP_FFR_DATA mdpFFRData;
  MDP_CROP_XY mdpCrop;
  uint16 x2_align;
  uint32 mbcOp;

#ifdef FEATURE_MDP_WSCALE
  uint32 wScaleOp;
#endif // #ifdef FEATURE_MDP_WSCALE
 
  /*Workarround for MDP hardware limitation when outputing 
    odd number of packed RGB565 pixels to memory*/
  if(lcdPhysDest[destDev] == MEM_WR)
  {
     /*If (x2-x1+1) is odd, 1 is added to x1 and then 
	 (x2-x1+1) will be even*/
     x1 += ((x2-x1+1)%2);
   
  }
   
  if(last_update_height + sub_height <=  mdp_prim_lcdHeight) 
    last_update_height += sub_height;

  ufac = 1;
  vDiv = 16;  /* vdb crop value can't be more than 16 */
  switch (scale)
  {
    case MDPOP_VDO_X2:
      ufac = 2;
      vDiv /= ufac; /* for X2, we must reduce crop value */
      num = 2;
      den = 1;
      ffrOpCode = 0x3C;
      break;

    case MDPOP_VDO_1BY4:
      num = 1;
      den = 4;
      ffrOpCode = 0x30;
      break;

    case MDPOP_VDO_1BY2:
      num = 1;
      den = 2;
      ffrOpCode = 0x34;
      break;

    case MDPOP_VDO_3BY4:
      num = 3;
      den = 4;
      ffrOpCode = 0x38;
      break;

    case MDPOP_VDO_NOP:
    default:
      num = 1;
      den = 1;
      ffrOpCode = 0x20;
      break;
  }

  lcd_col_end = lcd_col_start + ((x2-x1+1)*num)/den -1;
  lcd_row_end = lcd_row_start + ((y2-y1+1)*num)/den -1;


  if (mdpOp&MDPOP_ROT90)
  {
    tmpRowEnd = lcd_row_end;
    lcd_row_end = lcd_row_start + (lcd_col_end - lcd_col_start);
    lcd_col_end = lcd_col_start + (tmpRowEnd - lcd_row_start);
  }

#ifdef FEATURE_MDP_WSCALE
  // Set the MDP command for WSCALE output of VDB if applicable.
  switch (*wScale)
  {
    case MDPOP_WSCALE_LINEAR_STRETCH:
      wScaleOp = 0x37000001;
      break;
    case MDPOP_WSCALE_EDGE_STRETCH:
      wScaleOp = 0x37000002;
      break;
    case MDPOP_WSCALE_NO_STRETCH:
      wScaleOp = 0x37000003;
      break;
    case MDPOP_NOP:
      wScaleOp = NOP;
      *wScale = MDPOP_WSCALE_NOP;
      break;
    default:
      MDP_MSG_ERROR("Wscale Aborted! Unable to understand Wscale operations set!",0,0,0);
      MDP_MSG_ERROR("  Attempted wscale with MDPOP = 0x%08x",*wScale,0,0);
      wScaleOp = NOP;
      *wScale = MDPOP_WSCALE_NOP;
  } // switch (wScale)
  
  // Hard-coded values below due to HW being fixed for these values.
  //   WARNING: DO NOT MODIFY COL/ROW SETTINGS FOR WSCALE
  if (wScaleOp != NOP)
  {
    if (mdp_valid_img_for_wscale(lcd_col_start, lcd_col_end, lcd_row_start, lcd_row_end, *wScale))
    {
      lcd_col_end = MDP_WSCALE_OUT_WIDTH  + lcd_col_start - 1;
      lcd_row_end = MDP_WSCALE_OUT_HEIGHT + lcd_row_start - 1;
    }
    else
    {
      wScaleOp = NOP;
      *wScale = MDPOP_WSCALE_NOP;
    } // mdp_valid_img_for_wscale()

    // Work-around for w-scale offset issue.  Reset HW when we start this mode only.
    if (*wScale == MDPOP_WSCALE_NO_STRETCH)
    {
      if (resetMdpForWscaleNoStretch)
      {
        mdp_reset();
        resetMdpForWscaleNoStretch = FALSE;
      }
    }
    else
    {
      resetMdpForWscaleNoStretch = TRUE;
    }
  } // (wScaleOp != NOP)
#endif // #ifdef FEATURE_MDP_WSCALE

  /* adjusting fg_image to start at (0,0)*/
  /* this is also software-workaround for MDP bug */
  /* if you have some x/y crop value, MDP doesn't display correctly */

  if (x1 || y1)
  {
    tmpImageAddr = (uint32)Y_addr + x1 + y1*width;
    Y_addr = (uint32 *)tmpImageAddr;

    tmpImageAddr = (uint32)CbCr_addr + x1 + ((y1-(y1%2))*width)/2;
    CbCr_addr = (uint32 *)tmpImageAddr;

    x2 -= x1;
    y2 -= y1;
    x1 = y1 = 0;
  }

  x2_align = ((uint32)Y_addr)&0x03;
  x2 += x2_align;

  vdbX1 = (x1/vDiv)*vDiv;
  vdbX2 = (x2/vDiv)*vDiv+vDiv-1;
  vdbY1 = (y1/vDiv)*vDiv;
  vdbY2 = (y2/vDiv)*vDiv+vDiv-1;

  if ((scrPtr = mdp_scr_get(MDP_VDO_SCR)) == NULL)
  {
    return MDP_FAIL;
  }
  mdpScrCnt->vdo_scr_cnt++;

  if (mdpCalFFRdataLuma(Y_addr,
                        mdpOp,
                        scale,
                        width,
                        vdbX1,
                        vdbX2,
                        vdbY1,
                        vdbY2,
                        &mdpFFRData,
                        1) != MDP_SUCCESSFUL)
  {
    return MDP_FAIL;
  }

  if (scale == MDPOP_VDO_X2)
  {
    mdpFFRData.imgAddr -= (width+4);
  }

  scrPtr[1] = mdpFFRData.imgAddr;
  scrPtr[2] = mdpFFRData.line;
  scrPtr[3] = mdpFFRData.block;
  scrPtr[4] = mdpFFRData.stripe;

  if (mdpCalFFRdataChroma(CbCr_addr,
                          mdpOp,
                          scale,
                          width,
                          vdbX1,
                          vdbX2,
                          vdbY1,
                          vdbY2,
                          &mdpFFRData,
                          1) != MDP_SUCCESSFUL)
  {
    return MDP_FAIL;
  }

  scrPtr[6] = mdpFFRData.imgAddr - width;
  scrPtr[7] = mdpFFRData.line;
  scrPtr[8] = mdpFFRData.block;
  scrPtr[9] = mdpFFRData.stripe; 

  scrPtr[24] = SET_FFR_MODE(0, ffrOpCode, mdpOp, 0);

  mbcOp = mdp_dither_on ? SET_MBC_ACTION_DITHER:0;
  mbcOp |= mdp_gamma_enable;

  scrPtr[25] = SET_MBC_ACTION(mbcOp);

#ifdef FEATURE_MDP_WSCALE
  scrPtr[26] = wScaleOp;
#endif // #ifdef FEATURE_MDP_WSCALE

  switch (lcdPhysDest[destDev])
  {
    case MEM_WR:
      scrPtr[23] = SET_LCD_FORMAT(1, 0x11);
      scrPtr[30] = SET_LCD_DATA_ADDR;

      if ((output_addr == 0) || ((uint32)output_addr > MAX_MEM_LIMIT))
        return MDP_FAIL;

      // Add 0x1 for destination address auto-increment
      scrPtr[31] = ((uint32) output_addr) + 0x1;

      scrPtr[37] = NOP;
      scrPtr[39] = WAIT_JOB_DONE;
      break;

#ifndef CUST_EDITION
    case EP_OP_QCIFP_PRI:
      if ((lcdScrPtr = mdp_scr_get(MDP_EP_OP_QCIFP_PRI_SCR)) == NULL)
        return MDP_FAIL;
      mdpScrCnt->ep_op_qcifp_pri_scr_cnt++;
  
      scrPtr[23] = SET_LCD_FORMAT(0, 0);
      scrPtr[30] = SUBROUTINE;
      scrPtr[31] = (uint32) lcdScrPtr;
  
      lcdScrPtr[7] = SEND_LCD_DATA(lcd_col_start << 10);
      lcdScrPtr[8] = SEND_LCD_DATA(lcd_row_start << 10);
    
      lcdScrPtr[10] = SEND_LCD_DATA(lcd_col_end << 10);
      lcdScrPtr[11] = SEND_LCD_DATA(lcd_row_end << 10);
      break;


    case EP_OP_QCIFP_SEC:
      if ((lcdScrPtr = mdp_scr_get(MDP_EP_OP_QCIFP_SEC_SCR)) == NULL)
        return MDP_FAIL;
      mdpScrCnt->ep_op_qcifp_sec_scr_cnt++;
  
      scrPtr[23] = SET_LCD_FORMAT(0, 0);
      scrPtr[30] = SUBROUTINE;
      scrPtr[31] = (uint32) lcdScrPtr;
  
      lcdScrPtr[7] = SEND_LCD_DATA(lcd_col_start << 10);
      lcdScrPtr[8] = SEND_LCD_DATA(lcd_row_start << 10);
    
      lcdScrPtr[10] = SEND_LCD_DATA(lcd_col_end << 10);
      lcdScrPtr[11] = SEND_LCD_DATA(lcd_row_end << 10);
      break;


    case EPSON_QCIF:
      if ((lcdScrPtr = mdp_scr_get(MDP_EPSON_QCIF_SCR)) == NULL)
        return MDP_FAIL;
      mdpScrCnt->epson_qcif_scr_cnt++;

      scrPtr[23] = SET_LCD_FORMAT(0, 8);
      scrPtr[30] = SUBROUTINE;
      scrPtr[31] = (uint32) lcdScrPtr;

      scrPtr[37] = NOP;
      scrPtr[39] = WAIT_JOB_DONE;

      lcdScrPtr[7] = SEND_LCD_DATA(lcd_row_start & 0xFF);
      lcdScrPtr[8] = SEND_LCD_DATA(lcd_row_start >> 8);

      lcdScrPtr[9] = SEND_LCD_DATA(lcd_row_end & 0xFF);
      lcdScrPtr[10] = SEND_LCD_DATA(lcd_row_end >> 8);

      lcdScrPtr[13] = SEND_LCD_DATA((lcd_col_start+2) & 0xFF);
      lcdScrPtr[14] = SEND_LCD_DATA((lcd_col_start+2) >> 8);

      lcdScrPtr[15] = SEND_LCD_DATA((lcd_col_end+2) & 0xFF);
      lcdScrPtr[16] = SEND_LCD_DATA((lcd_col_end+2) >> 8);
      break;

	case EPSON_QVGA:
      if ((lcdScrPtr = mdp_scr_get(MDP_EPSON_QVGA_SCR)) == NULL)
        return MDP_FAIL;
      mdpScrCnt->epson_qvga_scr_cnt++;

      scrPtr[23] = SET_LCD_FORMAT(0, 0x2);
      scrPtr[30] = SUBROUTINE;
      scrPtr[31] = (uint32) lcdScrPtr;

      scrPtr[37] = NOP;
      scrPtr[39] = WAIT_JOB_DONE;

      lcdScrPtr[7] = SEND_LCD_DATA((uint8)(lcd_col_start)      << 1);
      lcdScrPtr[8] = SEND_LCD_DATA((uint8)(lcd_row_start >> 8) << 1);
      lcdScrPtr[9] = SEND_LCD_DATA((uint8)(lcd_row_start)      << 1);

      lcdScrPtr[12] = SEND_LCD_DATA((uint8)(lcd_col_end)      << 1);
      lcdScrPtr[13] = SEND_LCD_DATA((uint8)(lcd_row_end >> 8) << 1);
      lcdScrPtr[14] = SEND_LCD_DATA((uint8)(lcd_row_end)      << 1);

      if(mdp_toggle_output_format_enable)
        lcdScrPtr[18] = SET_LCD_FORMAT(0, 0x16);
      else
        lcdScrPtr[18] = NOP;

      break;

    case TMD_QVGA:
      if ((lcdScrPtr = mdp_scr_get(MDP_TMD_QVGA_SCR)) == NULL)
        return MDP_FAIL;
      mdpScrCnt->tmd_qvga_scr_cnt++;

      scrPtr[23] = SET_LCD_FORMAT(0, 0);
      scrPtr[30] = SUBROUTINE;
      scrPtr[31] = (uint32) lcdScrPtr;

      scrPtr[37] = NOP;
      scrPtr[39] = WAIT_JOB_DONE;

      lcdScrPtr[7] = SEND_LCD_DATA(DISP_QVGA_18BPP(lcd_col_start + tmd20qvga_panel_offset));
      lcdScrPtr[9] = SEND_LCD_DATA(DISP_QVGA_18BPP(lcd_col_end + tmd20qvga_panel_offset));
      lcdScrPtr[11] = SEND_LCD_DATA(DISP_QVGA_18BPP(lcd_row_start));
      lcdScrPtr[13] = SEND_LCD_DATA(DISP_QVGA_18BPP(lcd_row_end));

      lcdScrPtr[15] = SEND_LCD_DATA(DISP_QVGA_18BPP(lcd_col_start + tmd20qvga_panel_offset));
      lcdScrPtr[17] = SEND_LCD_DATA(DISP_QVGA_18BPP(lcd_row_start));

      /*
      This is same as above.  This shows actual logic.
      Let's use directly as above for better perofrmance

      lcdScrPtr[15] = SEND_LCD_DATA(DISP_QVGA_18BPP(DISP_QVGA_VAL_IF(dst_start_addr & 0x00080, DISP_QVGA_BITMASK_AD7) | DISP_QVGA_VAL_IF(dst_start_addr & 0x00040, DISP_QVGA_BITMASK_AD6) | DISP_QVGA_VAL_IF(dst_start_addr & 0x00020, DISP_QVGA_BITMASK_AD5) | DISP_QVGA_VAL_IF(dst_start_addr & 0x00010, DISP_QVGA_BITMASK_AD4) | DISP_QVGA_VAL_IF(dst_start_addr & 0x00008, DISP_QVGA_BITMASK_AD3) | DISP_QVGA_VAL_IF(dst_start_addr & 0x00004, DISP_QVGA_BITMASK_AD2) | DISP_QVGA_VAL_IF(dst_start_addr & 0x00002, DISP_QVGA_BITMASK_AD1) | DISP_QVGA_VAL_IF(dst_start_addr & 0x00001, DISP_QVGA_BITMASK_AD0)));
      lcdScrPtr[17] = SEND_LCD_DATA(DISP_QVGA_18BPP(DISP_QVGA_VAL_IF(dst_start_addr & 0x10000, DISP_QVGA_BITMASK_AD16) | DISP_QVGA_VAL_IF(dst_start_addr & 0x08000, DISP_QVGA_BITMASK_AD15) | DISP_QVGA_VAL_IF(dst_start_addr & 0x04000, DISP_QVGA_BITMASK_AD14) | DISP_QVGA_VAL_IF(dst_start_addr & 0x02000, DISP_QVGA_BITMASK_AD13) | DISP_QVGA_VAL_IF(dst_start_addr & 0x01000, DISP_QVGA_BITMASK_AD12) | DISP_QVGA_VAL_IF(dst_start_addr & 0x00800, DISP_QVGA_BITMASK_AD11) | DISP_QVGA_VAL_IF(dst_start_addr & 0x00400, DISP_QVGA_BITMASK_AD10) | DISP_QVGA_VAL_IF(dst_start_addr & 0x00200, DISP_QVGA_BITMASK_AD9)  | DISP_QVGA_VAL_IF(dst_start_addr & 0x00100, DISP_QVGA_BITMASK_AD8)));
      */
      break;
#else
    case MDP_LCD:
      if ((lcdScrPtr = mdp_scr_get(MDP_LCD_SCR)) == NULL)
        return MDP_FAIL;
      mdpScrCnt->lcd_scr_cnt++;
      
      scrPtr[23] = SET_LCD_FORMAT(0, disp_drv_mdp_getformat());
      scrPtr[30] = SUBROUTINE;
      scrPtr[31] = (uint32) lcdScrPtr;

      scrPtr[37] = NOP;
      scrPtr[39] = WAIT_JOB_DONE;
      
      disp_drv_mdp_scrupdate(lcdScrPtr, lcd_row_start, lcd_col_start, lcd_row_end, lcd_col_end);
      break;
#endif
#ifdef FEATURE_MDDI
#error code not present
#endif /* FEATURE_MDDI */

    default: return MDP_FAIL;
  }

  mdpCalCropXY(x1+x2_align, x2, y1, y2, vdbX1, vdbX2, vdbY1, vdbY2, mdpOp, &mdpCrop);

  orig_cx1 = mdpCrop.cx1;
  mdpCrop.cx1 = (mdpCrop.cx1)*num/den;
  mdpCrop.cx2 = (mdpCrop.cx2-orig_cx1+1)*num/den-1+mdpCrop.cx1;

  orig_cy1 = mdpCrop.cy1;
  mdpCrop.cy1 = (mdpCrop.cy1)*num/den;
  mdpCrop.cy2 = (mdpCrop.cy2-orig_cy1+1)*num/den-1+mdpCrop.cy1;


  scrPtr[32] = SET_VDB_CROP_X(mdpCrop.cx1, mdpCrop.cx2);
  scrPtr[33] = SET_VDB_CROP_Y(mdpCrop.cy1, mdpCrop.cy2);

  // 1: enable MDP pipleline, stripe/image -1, #ofBlocks/stripe -1
  // number of lines per stripe = 16
  // 1 block = 16pixel x 16pixel
  /* vdb size is repective to output */
  /* output is always 16 lines/stripe, 16pixel/block */

  if (mdpOp&MDPOP_ROT90)
  {
    scrPtr[34] = SET_VDB_SIZE(mdp_sfd_enable,
                              ufac*(vdbX2-vdbX1)/16,
                              ufac*(vdbY2-vdbY1)/16);
  }
  else
  {
    scrPtr[34] = SET_VDB_SIZE(mdp_sfd_enable,
                              ufac*(vdbY2-vdbY1)/16,
                              ufac*(vdbX2-vdbX1)/16);
  }

  scrPtr[36] = MAX_MEM_LIMIT;
  scrPtr[MDP_VDO_LINK_POS] = NOP;
  scrPtr[MDP_VDO_LINK_POS+1] = NOP;

  if (linkAddr)
  {
    linkAddr[0] = JUMP;
    linkAddr[1] = (uint32)scrPtr;
  }

  return(int)scrPtr;
}

/*===========================================================================

FUNCTION mdp_disp_gov_sub()

DESCRIPTION
  This function is to compose RGB graphic over YCbCr video display scripts

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  MDP_SUCCESSFUL if successful otherwise MDP_FAIL

SIDE EFFECTS
  None

===========================================================================*/
int mdp_disp_gov_sub( LCD_TYPE destDev,
                      uint32 *Y_addr,
                      uint32 *CbCr_addr,
                      uint16 width,
                      uint16 height,
                      uint16 x1,
                      uint16 x2,
                      uint16 y1,
                      uint16 y2,
                      uint16 lcd_col_start,
                      uint16 lcd_row_start,
                      uint32 *output_addr,
                      uint32 mdpOp,
                      uint32 *bg_image_addr,
                      MDPIMG_TYPE bg_imgType,
                      uint32 bg_width,
                      uint32 bg_height,
                      uint16 bg_x1,
                      uint16 bg_x2,
                      uint16 bg_y1,
                      uint16 bg_y2,
                      uint32 bg_mdpOp,
                      uint32 scale,   // input VDB video(YUV) scaling
                      uint32 *wScale,  // VDB output QVGA->WQVGA scaling
                      uint32 mbcOp,
                      uint32 tpVal,
                      uint32 alphaFrg,
                      uint32 alphaBkg,
                      uint32 *linkAddr,
                      MDP_SCRCNT *mdpScrCnt,
                      uint32 *linkPos)
{
  uint16 lcd_row_end;
  uint16 lcd_col_end;
  uint16 tmpRowEnd;
  uint16 vdbX1, vdbX2, vdbY1, vdbY2, old_vdbX2, old_vdbY2;
  uint16 bg_vdbX1, bg_vdbX2, bg_vdbY1, bg_vdbY2;

  uint32 *scrPtr, *lcdScrPtr,tmpImageAddr;
  int32 cx1_off, cy1_off;

  MDP_FFR_DATA mdpFFRData;
  MDP_CROP_XY mdpCrop, bg_mdpCrop;

  uint8 ffrOpCode, fringe_cntl;
  uint16 num, den;

  int bytesPerPixel;

#ifdef FEATURE_MDP_WSCALE
  uint32 wScaleOp;
#endif // #ifdef FEATURE_MDP_WSCALE

  /*Workarround for MDP hardware limitation when outputing 
    odd number of packed RGB565 pixels to memory*/
  if(lcdPhysDest[destDev] == MEM_WR)
  {
    /*If (x2-x1+1) is odd, 1 is added to x1 and then 
	(x2-x1+1) will be even*/
    x1 += ((x2-x1+1)%2);  
  }

  if(last_update_height + sub_height <=  mdp_prim_lcdHeight)
    last_update_height += sub_height;

  if ((bg_image_addr == NULL) || (Y_addr == NULL) || (CbCr_addr == NULL))
  {
    return MDP_FAIL;    
  }

  if (mbcOp == MDP_MBC_NOP)
  {
    if (mdp_mddi_bypass_16lines == TRUE)
    {
      *linkPos = MDP_GFX_LINK_POS;
      return mdp_disp_gfx_sub(destDev,
                              bg_image_addr,
                              bg_imgType,
                              bg_width,
                              bg_height,
                              bg_x1,
                              bg_x2,
                              bg_y1,
                              bg_y2,
                              lcd_col_start,
                              lcd_row_start,
                              output_addr,
                              bg_mdpOp,
                              wScale, // VDB output QVGA->WQVGA scaling
                              linkAddr,
                              mdpScrCnt);
    }
    else
    {
      mbcOp = MDPOP_ALPHAB;
      alphaBkg = 0;
      alphaFrg = 0x100;      
    }                            
  }

  *linkPos = MDP_GOV_LINK_POS;


  /* gfx over vdo supports 2x/1x only */
  switch (scale)
  {
    case MDPOP_VDO_X2:
      num = 2;
      den = 1;
      if (bg_imgType == MDP_XRGB_8888)
      {
        ffrOpCode = 0x3f;
        bytesPerPixel = 4;
      }
//      else if (bg_imgType == MDP_RGB_888)  No 888 packed support
//      {
//        ffrOpCode = 0x3E;
//        bytesPerPixel = 3;
//      }
      else // MDP_RGB_565
      {
        ffrOpCode = 0x3D;
        bytesPerPixel = 2;
      }
      break;

    case MDPOP_VDO_NOP:
      num = 1;
      den = 1;
      if (bg_imgType == MDP_XRGB_8888)
      {
        ffrOpCode = 0x23;
        bytesPerPixel = 4;
      }
//      else if (bg_imgType == MDP_RGB_888)  No 888 packed support
//      {
//        ffrOpCode = 0x22;
//        bytesPerPixel = 3;
//      }
      else // MDP_RGB_565
      {
        ffrOpCode = 0x21;
        bytesPerPixel = 2;
      }
      break;

    default: return MDP_FAIL;
  }

  x2 = x1 + ((x2-x1+1)*num)/den-1;
  y2 = y1 + ((y2-y1+1)*num)/den-1;

  lcd_col_end = lcd_col_start + x2-x1;
  lcd_row_end = lcd_row_start + y2-y1;


  if (mdpOp&MDPOP_ROT90)
  {
    tmpRowEnd = lcd_row_end;
    lcd_row_end = lcd_row_start + (lcd_col_end - lcd_col_start);
    lcd_col_end = lcd_col_start + (tmpRowEnd - lcd_row_start);
  }

#ifdef FEATURE_MDP_WSCALE
  // Set the MDP command for WSCALE output of VDB if applicable.
  switch (*wScale)
  {
    case MDPOP_WSCALE_LINEAR_STRETCH:
      wScaleOp = 0x37000001;
      break;
    case MDPOP_WSCALE_EDGE_STRETCH:
      wScaleOp = 0x37000002;
      break;
    case MDPOP_WSCALE_NO_STRETCH:
      wScaleOp = 0x37000003;
      break;
    case MDPOP_NOP:
      wScaleOp = NOP;
      *wScale = MDPOP_WSCALE_NOP;
      break;
    default:
      MDP_MSG_ERROR("Wscale Aborted! Unable to understand Wscale operations set!",0,0,0);
      MDP_MSG_ERROR("  Attempted wscale with MDPOP = 0x%08x",*wScale,0,0);
      wScaleOp = NOP;
      *wScale = MDPOP_WSCALE_NOP;
  } // switch (wScale)
  
  // Hard-coded values below due to HW being fixed for these values.
  //   WARNING: DO NOT MODIFY COL/ROW SETTINGS FOR WSCALE
  if (wScaleOp != NOP)
  {
    if (mdp_valid_img_for_wscale(lcd_col_start, lcd_col_end, lcd_row_start, lcd_row_end, *wScale))
    {
      lcd_col_end = MDP_WSCALE_OUT_WIDTH  + lcd_col_start - 1;
      lcd_row_end = MDP_WSCALE_OUT_HEIGHT + lcd_row_start - 1;
    }
    else
    {
      wScaleOp = NOP;
      *wScale = MDPOP_WSCALE_NOP;
    } // mdp_valid_img_for_wscale()

    // Work-around for w-scale offset issue.  Reset HW when we start this mode only.
    if (*wScale == MDPOP_WSCALE_NO_STRETCH)
    {
      if (resetMdpForWscaleNoStretch)
      {
        mdp_reset();
        resetMdpForWscaleNoStretch = FALSE;
      }
    }
    else
    {
      resetMdpForWscaleNoStretch = TRUE;
    }
  } // (wScaleOp != NOP)
#endif // #ifdef FEATURE_MDP_WSCALE

  fringe_cntl = 0;
  fringe_cntl |= (x2<(width-1))?BIT(0):0;
  fringe_cntl |= (x1>0)?BIT(1):0;
  fringe_cntl |= (y2<(height-1))?BIT(2):0;
  fringe_cntl |= (y1>0)?BIT(3):0;

  /* adjusting bg_image to start at (0,0)*/
  if (x1 || y1)
  {
    tmpImageAddr = (uint32)Y_addr + x1 + y1*width;
    Y_addr = (uint32 *)tmpImageAddr;

    tmpImageAddr = (uint32)CbCr_addr + x1 + ((y1-(y1%2))*width)/2;
    CbCr_addr = (uint32 *)tmpImageAddr;

    x2 -= x1;
    y2 -= y1;
    x1 = y1 = 0;
  }

  /* adjusting fg_image to start at (0,0)*/
  if (bg_x1 || bg_y1)
  {
    tmpImageAddr = (uint32)bg_image_addr + bg_x1*bytesPerPixel + bg_y1*bytesPerPixel*bg_width;
    bg_image_addr = (uint32 *)tmpImageAddr;

    bg_x2 -= bg_x1;
    bg_y2 -= bg_y1;
    bg_x1 = bg_y1 = 0;
  }

  /* this is actual vdb for foregroun image */
  vdbX1 = bg_vdbX1 = (x1/16)*16;
  old_vdbX2 = vdbX2 = bg_vdbX2 = (x2/16)*16+16-1;
  vdbY1 = bg_vdbY1 = (y1/16)*16;
  old_vdbY2 = vdbY2 = bg_vdbY2 = (y2/16)*16+16-1;

  if (mdpOp&MDPOP_ROT90)
  {
    if (bg_mdpOp&MDPOP_ROT90)
    {
      bg_x2 = x2;
      bg_y2 = y2;
    }
    else
    {
      bg_x1 = vdbY2 - y2;
      bg_x2 = vdbY2;
      /* it's already 0... */
      /* bg_y1 = 0;*/
      bg_y2 = x2;

      /* vdbX1 and vdbY1 are same = 0 */
      bg_vdbX2 = vdbY2;
      bg_vdbY2 = vdbX2;

      tmpImageAddr = (uint32)bg_image_addr - bg_x1*bytesPerPixel;
      bg_image_addr = (uint32 *)tmpImageAddr;
    }
  }
  else
  {
    if (bg_mdpOp&MDPOP_ROT90)
    {
      /* it's already 0... */
      /* bg_x1 = 0; */
      bg_x2 = y2;
      bg_y1 = vdbX2 - x2;
      bg_y2 = vdbX2;

      /* vdbX1 and vdbY1 are same = 0 */
      bg_vdbX2 = vdbY2;
      bg_vdbY2 = vdbX2;

      tmpImageAddr = (uint32)bg_image_addr - bg_y1*bg_width*bytesPerPixel;
      bg_image_addr = (uint32 *)tmpImageAddr;
    }
    else
    {
      bg_x2 = x2;
      bg_y2 = y2;
    }
  }

  mdpCalCropXY(x1, x2, y1, y2, vdbX1, vdbX2, vdbY1, vdbY2, mdpOp, &mdpCrop);
  mdpCalCropXY(bg_x1, bg_x2, bg_y1, bg_y2, bg_vdbX1, bg_vdbX2,
               bg_vdbY1, bg_vdbY2, bg_mdpOp, &bg_mdpCrop);

  cx1_off = (int32)(mdpCrop.cx1 - bg_mdpCrop.cx1);
  cy1_off = (int32)(mdpCrop.cy1 - bg_mdpCrop.cy1);


  cx1_off = (cx1_off>0)?cx1_off:-cx1_off;
  cy1_off = (cy1_off>0)?cy1_off:-cy1_off;


  if (cx1_off || cy1_off)
  {
    if (mdpOp&MDPOP_ROT90)
    {
      if (bg_mdpOp&MDPOP_ROT90)
      {
        tmpImageAddr = (uint32)bg_image_addr - cy1_off*bytesPerPixel - cx1_off*bytesPerPixel*bg_width;
        bg_image_addr = (uint32 *)tmpImageAddr;
      }
      else
      {
        tmpImageAddr = (uint32)bg_image_addr + cx1_off*bytesPerPixel - cy1_off*bytesPerPixel*bg_width;
        bg_image_addr = (uint32 *)tmpImageAddr;
      }
    }
    else
    {
      if (bg_mdpOp&MDPOP_ROT90)
      {
        tmpImageAddr = (uint32)bg_image_addr - cy1_off*bytesPerPixel + cx1_off*bytesPerPixel*bg_width;
        bg_image_addr = (uint32 *)tmpImageAddr;
      }
      else
      {
        tmpImageAddr = (uint32)bg_image_addr - cx1_off*bytesPerPixel - cy1_off*bytesPerPixel*bg_width;
        bg_image_addr = (uint32 *)tmpImageAddr;
      }
    }
  }

  if ((scrPtr = mdp_scr_get(MDP_GOV_SCR)) == NULL)
  {
    return MDP_FAIL;
  }
  mdpScrCnt->gov_scr_cnt++;

  vdbX2 = (vdbX2+1)*den/num - 1;
  vdbY2 = (vdbY2+1)*den/num - 1;


  if (mdpCalFFRdataLuma(Y_addr,
                        mdpOp,
                        scale,
                        width,
                        vdbX1,
                        vdbX2,
                        vdbY1,
                        vdbY2,
                        &mdpFFRData,
                        1) != MDP_SUCCESSFUL)
  {
    return MDP_FAIL;
  }

  if (scale == MDPOP_VDO_X2)
  {
    mdpFFRData.imgAddr -= (width+4);
  }

#ifdef FEATURE_MDP_WSCALE
  scrPtr[35] = wScaleOp;
#endif

  scrPtr[1] = mdpFFRData.imgAddr;
  scrPtr[2] = mdpFFRData.line;
  scrPtr[3] = mdpFFRData.block;
  scrPtr[4] = mdpFFRData.stripe;

  if (mdpCalFFRdataChroma(CbCr_addr,
                          mdpOp,
                          scale,
                          width,
                          vdbX1,
                          vdbX2,
                          vdbY1,
                          vdbY2,
                          &mdpFFRData,
                          1) != MDP_SUCCESSFUL)
  {
    return MDP_FAIL;
  }

  scrPtr[6] = mdpFFRData.imgAddr - width;
  scrPtr[7] = mdpFFRData.line;
  scrPtr[8] = mdpFFRData.block;
  scrPtr[9] = mdpFFRData.stripe;


  if (mdpCalFFRdataFg(bg_image_addr,
                      bg_mdpOp,
                      bg_width,
                      bg_vdbX1,
                      bg_vdbX2,
                      bg_vdbY1,
                      bg_vdbY2,
                      &mdpFFRData,
                      bg_imgType) != MDP_SUCCESSFUL)
  {
    return MDP_FAIL;
  }

  scrPtr[11] = mdpFFRData.imgAddr;
  scrPtr[12] = mdpFFRData.line;
  scrPtr[13] = mdpFFRData.block;
  scrPtr[14] = mdpFFRData.stripe;

  scrPtr[29] = SET_FFR_MODE(fringe_cntl, ffrOpCode, mdpOp, bg_mdpOp);

  if (mbcOp != MDP_MBC_NOP)
  {
    scrPtr[33] = SET_MBC_ACTION(mbcOp>>MDPOP_MBC_SHIFT) | mdp_gamma_enable;
    scrPtr[33] |= mdp_dither_on ? SET_MBC_ACTION_DITHER:0;

    if (mbcOp&MDPOP_TRANSP)
    {
      scrPtr[34] = SET_MBC_TV(tpVal); //tp value
    }
    else
    {
      scrPtr[34] = NOP;
    }

    if (mbcOp&MDPOP_ALPHAB)
    {
      if (alphaBkg == 0)
      {
        alphaBkg = 1;
        alphaFrg = 0xff;
      }

      if (alphaFrg == 0)
      {
        alphaBkg = 0xff;
        alphaFrg = 1;
      }

      scrPtr[32] = SET_MBC_ALPHA(alphaBkg, alphaFrg); //tp value
    }
    else
    {
      scrPtr[32] = NOP;
    }

  }
  else
  {
    scrPtr[32] = NOP;
    scrPtr[33] = NOP;
    scrPtr[34] = NOP;
  }

  switch (lcdPhysDest[destDev])
  {
    case MEM_WR:
      scrPtr[28] = SET_LCD_FORMAT(1, 0x11);
      scrPtr[30] = SET_LCD_DATA_ADDR;

      if ((output_addr == 0) || ((uint32)output_addr > MAX_MEM_LIMIT))
        return MDP_FAIL;

      // Add 0x1 for destination address auto-increment
      scrPtr[31] = ((uint32) output_addr) + 0x1;
      scrPtr[41] = NOP;
      scrPtr[43] = WAIT_JOB_DONE;
      break;

#ifndef CUST_EDITION
    case EP_OP_QCIFP_PRI:
      if ((lcdScrPtr = mdp_scr_get(MDP_EP_OP_QCIFP_PRI_SCR)) == NULL)
        return MDP_FAIL;
      mdpScrCnt->ep_op_qcifp_pri_scr_cnt++;
  
      scrPtr[28] = SET_LCD_FORMAT(0, 0);
      scrPtr[30] = SUBROUTINE;
      scrPtr[31] = (uint32) lcdScrPtr;
  
      lcdScrPtr[7] = SEND_LCD_DATA(lcd_col_start << 10);
      lcdScrPtr[8] = SEND_LCD_DATA(lcd_row_start << 10);
    
      lcdScrPtr[10] = SEND_LCD_DATA(lcd_col_end << 10);
      lcdScrPtr[11] = SEND_LCD_DATA(lcd_row_end << 10);
      break;


    case EP_OP_QCIFP_SEC:
      if ((lcdScrPtr = mdp_scr_get(MDP_EP_OP_QCIFP_SEC_SCR)) == NULL)
        return MDP_FAIL;
      mdpScrCnt->ep_op_qcifp_sec_scr_cnt++;
  
      scrPtr[28] = SET_LCD_FORMAT(0, 0);
      scrPtr[30] = SUBROUTINE;
      scrPtr[31] = (uint32) lcdScrPtr;
  
      lcdScrPtr[7] = SEND_LCD_DATA(lcd_col_start << 10);
      lcdScrPtr[8] = SEND_LCD_DATA(lcd_row_start << 10);
    
      lcdScrPtr[10] = SEND_LCD_DATA(lcd_col_end << 10);
      lcdScrPtr[11] = SEND_LCD_DATA(lcd_row_end << 10);
      break;


    case EPSON_QCIF:
      if ((lcdScrPtr = mdp_scr_get(MDP_EPSON_QCIF_SCR)) == NULL)
        return MDP_FAIL;
      mdpScrCnt->epson_qcif_scr_cnt++;

      scrPtr[28] = SET_LCD_FORMAT(0, 8);
      scrPtr[30] = SUBROUTINE;
      scrPtr[31] = (uint32)lcdScrPtr;

      scrPtr[41] = NOP;
      scrPtr[43] = WAIT_JOB_DONE;

      lcdScrPtr[7] = SEND_LCD_DATA(lcd_row_start & 0xFF);
      lcdScrPtr[8] = SEND_LCD_DATA(lcd_row_start >> 8);

      lcdScrPtr[9] = SEND_LCD_DATA(lcd_row_end & 0xFF);
      lcdScrPtr[10] = SEND_LCD_DATA(lcd_row_end >> 8);

      lcdScrPtr[13] = SEND_LCD_DATA((lcd_col_start+2) & 0xFF);
      lcdScrPtr[14] = SEND_LCD_DATA((lcd_col_start+2) >> 8);

      lcdScrPtr[15] = SEND_LCD_DATA((lcd_col_end+2) & 0xFF);
      lcdScrPtr[16] = SEND_LCD_DATA((lcd_col_end+2) >> 8);
      break;
	
	case EPSON_QVGA:
      if ((lcdScrPtr = mdp_scr_get(MDP_EPSON_QVGA_SCR)) == NULL)
        return MDP_FAIL;
      mdpScrCnt->epson_qvga_scr_cnt++;

      scrPtr[28] = SET_LCD_FORMAT(0, 0x2);
      scrPtr[30] = SUBROUTINE;
      scrPtr[31] = (uint32)lcdScrPtr;

      scrPtr[41] = NOP;
      scrPtr[43] = WAIT_JOB_DONE;

      lcdScrPtr[7] = SEND_LCD_DATA((uint8)(lcd_col_start)      << 1);
      lcdScrPtr[8] = SEND_LCD_DATA((uint8)(lcd_row_start >> 8) << 1);
      lcdScrPtr[9] = SEND_LCD_DATA((uint8)(lcd_row_start)      << 1);

      lcdScrPtr[12] = SEND_LCD_DATA((uint8)(lcd_col_end)      << 1);
      lcdScrPtr[13] = SEND_LCD_DATA((uint8)(lcd_row_end >> 8) << 1);
      lcdScrPtr[14] = SEND_LCD_DATA((uint8)(lcd_row_end)      << 1);

      if(mdp_toggle_output_format_enable)
        lcdScrPtr[18] = SET_LCD_FORMAT(0, 0x16);
      else
        lcdScrPtr[18] = NOP;

      break;

    case TMD_QVGA:
      if ((lcdScrPtr = mdp_scr_get(MDP_TMD_QVGA_SCR)) == NULL)
        return MDP_FAIL;
      mdpScrCnt->tmd_qvga_scr_cnt++;

      scrPtr[28] = SET_LCD_FORMAT(0, 0);
      scrPtr[30] = SUBROUTINE;
      scrPtr[31] = (uint32) lcdScrPtr;

      scrPtr[41] = NOP;
      scrPtr[43] = WAIT_JOB_DONE;

      lcdScrPtr[7] = SEND_LCD_DATA(DISP_QVGA_18BPP(lcd_col_start + tmd20qvga_panel_offset));
      lcdScrPtr[9] = SEND_LCD_DATA(DISP_QVGA_18BPP(lcd_col_end + tmd20qvga_panel_offset));
      lcdScrPtr[11] = SEND_LCD_DATA(DISP_QVGA_18BPP(lcd_row_start));
      lcdScrPtr[13] = SEND_LCD_DATA(DISP_QVGA_18BPP(lcd_row_end));

      lcdScrPtr[15] = SEND_LCD_DATA(DISP_QVGA_18BPP(lcd_col_start + tmd20qvga_panel_offset));
      lcdScrPtr[17] = SEND_LCD_DATA(DISP_QVGA_18BPP(lcd_row_start));
      break;
#else
    case MDP_LCD:
      if ((lcdScrPtr = mdp_scr_get(MDP_LCD_SCR)) == NULL)
        return MDP_FAIL;
      mdpScrCnt->lcd_scr_cnt++;
      
      scrPtr[28] = SET_LCD_FORMAT(0, disp_drv_mdp_getformat());
      scrPtr[30] = SUBROUTINE;
      scrPtr[31] = (uint32) lcdScrPtr;

      scrPtr[41] = NOP;
      scrPtr[43] = WAIT_JOB_DONE;

      disp_drv_mdp_scrupdate(lcdScrPtr, lcd_row_start, lcd_col_start, lcd_row_end, lcd_col_end);
      break;
#endif

#ifdef FEATURE_MDDI
#error code not present
#endif /* FEATURE_MDDI */

    default: return MDP_FAIL;
  }

  scrPtr[36] = SET_VDB_CROP_X(mdpCrop.cx1, mdpCrop.cx2);
  scrPtr[37] = SET_VDB_CROP_Y(mdpCrop.cy1, mdpCrop.cy2);


  // 1: enable MDP pipleline, stripe/image -1, #ofBlocks/stripe -1
  // number of lines per stripe = 16
  // 1 block = 16pixel x 16pixel
  if (mdpOp&MDPOP_ROT90)
  {
    scrPtr[38] = SET_VDB_SIZE(mdp_sfd_enable,
                              (old_vdbX2-vdbX1)/16,
                              (old_vdbY2-vdbY1)/16);
  }
  else
  {
    scrPtr[38] = SET_VDB_SIZE(mdp_sfd_enable,
                              (old_vdbY2-vdbY1)/16,
                              (old_vdbX2-vdbX1)/16);
  }

  scrPtr[40] = MAX_MEM_LIMIT;
  scrPtr[MDP_GOV_LINK_POS] = NOP;
  scrPtr[MDP_GOV_LINK_POS+1] = NOP;

  if (linkAddr)
  {
    linkAddr[0] = JUMP;
    linkAddr[1] = (uint32)scrPtr;
  }

  return(int)scrPtr;
}

/*===========================================================================

FUNCTION mdp_disp_gog_sub()

DESCRIPTION
  This function is to compose RGB graphic over RGB graphic scripts

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  MDP_SUCCESSFUL if successful otherwise MDP_FAIL

SIDE EFFECTS
  None

===========================================================================*/
int mdp_disp_gog_sub( LCD_TYPE destDev,
                      uint32 *image_addr,
                      MDPIMG_TYPE imgType,
                      uint16 width,
                      uint16 height,
                      uint16 x1,
                      uint16 x2,
                      uint16 y1,
                      uint16 y2,
                      uint16 lcd_col_start,
                      uint16 lcd_row_start,
                      uint32 *output_addr,
                      uint32 mdpOp,
                      uint32 *bg_image_addr,
                      MDPIMG_TYPE bg_imgType,
                      uint32 bg_width,
                      uint32 bg_height,
                      uint16 bg_x1,
                      uint16 bg_y1,
                      uint32 bg_mdpOp,
                      uint32 *wScale,  // VDB output QVGA->WQVGA scaling
                      uint32 mbcOp,
                      uint32 tpVal,
                      uint32 alphaFrg,
                      uint32 alphaBkg,
                      uint32 *linkAddr,
                      MDP_SCRCNT *mdpScrCnt,
                      uint32 *linkPos)

{
  uint16 lcd_row_end;
  uint16 lcd_col_end;
  uint16 tmpRowEnd;
  uint16 vdbX1, vdbX2, vdbY1, vdbY2;
  uint16 bg_vdbX1, bg_vdbX2, bg_vdbY1, bg_vdbY2, bg_x2, bg_y2;

  uint8  ffr_mode;
  int    fgBpp, bgBpp;

  uint32 *scrPtr, *lcdScrPtr,tmpImageAddr;
  int32 cx1_off, cy1_off;

  MDP_FFR_DATA mdpFFRData;
  MDP_CROP_XY mdpCrop, bg_mdpCrop;

  uint16 x2_align;

#ifdef FEATURE_MDP_WSCALE
  uint32 wScaleOp;
#endif

  /*Workarround for MDP hardware limitation when outputing 
    odd number of packed RGB565 pixels to memory*/
 if(lcdPhysDest[destDev] == MEM_WR)
 {
    /*If (x2-x1) is even, 1 is added to x1 and then 
	(x2-x1+1) will be even, and vice versa.*/
    x1 += ((x2-x1+1)%2);
   
 }

  if(last_update_height + sub_height <=  mdp_prim_lcdHeight)
    last_update_height += sub_height;

  if ((image_addr == NULL) || (bg_image_addr == NULL))
  {
    return MDP_FAIL;
  }

  if (mbcOp == MDP_MBC_NOP)
  {
    if (mdp_mddi_bypass_16lines == TRUE)
    {
      *linkPos = MDP_GFX_LINK_POS;
      return mdp_disp_gfx_sub(destDev,
                              image_addr,
                              imgType,
                              width,
                              height,
                              x1,
                              x2,
                              y1,
                              y2,
                              lcd_col_start,
                              lcd_row_start,
                              output_addr,
                              mdpOp,
                              // VDB output QVGA->WQVGA scaling
                              wScale,
                              linkAddr,
                              mdpScrCnt);
    }
    else
    {
      mbcOp = MDPOP_ALPHAB;
      alphaBkg = 0;
      alphaFrg = 0x100;      
    }
  }

  *linkPos = MDP_GOG_LINK_POS;

  lcd_col_end = lcd_col_start + x2-x1;
  lcd_row_end = lcd_row_start + y2-y1;


  if (mdpOp&MDPOP_ROT90)
  {
    tmpRowEnd = lcd_row_end;
    lcd_row_end = lcd_row_start + (lcd_col_end - lcd_col_start);
    lcd_col_end = lcd_col_start + (tmpRowEnd - lcd_row_start);
  }

#ifdef FEATURE_MDP_WSCALE
  // Set the MDP command for WSCALE output of VDB if applicable.
  switch (*wScale)
  {
    case MDPOP_WSCALE_LINEAR_STRETCH:
      wScaleOp = 0x37000001;
      break;
    case MDPOP_WSCALE_EDGE_STRETCH:
      wScaleOp = 0x37000002;
      break;
    case MDPOP_WSCALE_NO_STRETCH:
      wScaleOp = 0x37000003;
      break;
    case MDPOP_NOP:
      wScaleOp = NOP;
      *wScale = MDPOP_WSCALE_NOP;
      break;
    default:
      MDP_MSG_ERROR("Wscale Aborted! Unable to understand Wscale operations set!",0,0,0);
      MDP_MSG_ERROR("  Attempted wscale with MDPOP = 0x%08x",*wScale,0,0);
      wScaleOp = NOP;
      *wScale = MDPOP_WSCALE_NOP;
  } // switch (wScale)
  
  // Hard-coded values below due to HW being fixed for these values.
  //   WARNING: DO NOT MODIFY COL/ROW SETTINGS FOR WSCALE
  if (wScaleOp != NOP)
  {
    if (mdp_valid_img_for_wscale(lcd_col_start, lcd_col_end, lcd_row_start, lcd_row_end, *wScale))
    {
      lcd_col_end = MDP_WSCALE_OUT_WIDTH  + lcd_col_start - 1;
      lcd_row_end = MDP_WSCALE_OUT_HEIGHT + lcd_row_start - 1;
    }
    else
    {
      wScaleOp = NOP;
      *wScale = MDPOP_WSCALE_NOP;
    } // mdp_valid_img_for_wscale()

    // Work-around for w-scale offset issue.  Reset HW when we start this mode only.
    if (*wScale == MDPOP_WSCALE_NO_STRETCH)
    {
      if (resetMdpForWscaleNoStretch)
      {
        mdp_reset();
        resetMdpForWscaleNoStretch = FALSE;
      }
    }
    else
    {
      resetMdpForWscaleNoStretch = TRUE;
    }
  } // (wScaleOp != NOP)
#endif // #ifdef FEATURE_MDP_WSCALE

  // Find the operation for SET_FFR_MODE below
  // Find the bpp for cropping window adjustment
  ffr_mode = 0;

  if (bg_imgType == MDP_XRGB_8888)
  {
    ffr_mode |= 0x0c;    // set background img type
    bgBpp = 4;           // adjust background address by 4-byte increments
  }
//  else if (bg_imgType == MDP_RGB_888)  No 888 packed support
//  {
//    ffr_mode |= 0x08;
//    bgBpp = 3;
//  }
  else // MDP_RGB_565
  {
    ffr_mode |= 0x04;
    bgBpp = 2;
  }

  if (imgType == MDP_XRGB_8888)
  {
    ffr_mode |= 0x03;    // set foreground img type
    fgBpp = 4;           // adjust fg address by 4-byte increments
  }
//  else if (imgType == MDP_RGB_888)  No 888 packed support
//  {
//    ffr_mode |= 0x02;
//    fgBpp = 3;
//  }
  else // MDP_RGB_565
  {
    ffr_mode |= 0x01;
    fgBpp = 2;
  }

  /* adjusting fg_image to start at (0,0)*/
  if (x1 || y1)
  {
    tmpImageAddr = (uint32)image_addr + x1*fgBpp + y1*fgBpp*width;
    image_addr = (uint32 *)tmpImageAddr;

    x2 -= x1;
    y2 -= y1;
    x1 = y1 = 0;
  }

  /* adjusting bg_image to start at (0,0)*/
  if (bg_x1 || bg_y1)
  {
    tmpImageAddr = (uint32)bg_image_addr + bg_x1*bgBpp + bg_y1*bgBpp*bg_width;
    bg_image_addr = (uint32 *)tmpImageAddr;
    bg_x1 = bg_y1 = 0;
  }

  if (((((uint32)image_addr)&0x03)/2) &&
      ((((uint32)bg_image_addr)&0x03)/2))
  {
    x2_align = (((uint32)image_addr)&0x03)/2;
    x2 += x2_align;
  }
  else
  {
    x2_align = 0;
  }

  /* this is actual vdb for foregroun image */
  vdbX1 = bg_vdbX1 = (x1/16)*16;
  vdbX2 = bg_vdbX2 = (x2/16)*16+16-1;
  vdbY1 = bg_vdbY1 = (y1/16)*16;
  vdbY2 = bg_vdbY2 = (y2/16)*16+16-1;

  if (mdpOp&MDPOP_ROT90)
  {
    if (bg_mdpOp&MDPOP_ROT90)
    {
      bg_x2 = x2;
      bg_y2 = y2;
    }
    else
    {
      bg_x1 = vdbY2 - y2;
      bg_x2 = vdbY2;
      /* it's already 0... */
      /* bg_y1 = 0;*/
      bg_y2 = x2;

      /* vdbX1 and vdbY1 are same = 0 */
      bg_vdbX2 = vdbY2;
      bg_vdbY2 = vdbX2;

      tmpImageAddr = (uint32)bg_image_addr - bg_x1*bgBpp;
      bg_image_addr = (uint32 *)tmpImageAddr;
    }
  }
  else
  {
    if (bg_mdpOp&MDPOP_ROT90)
    {
      /* it's already 0... */
      /* bg_x1 = 0; */
      bg_x2 = y2;
      bg_y1 = vdbX2 - x2;
      bg_y2 = vdbX2;

      /* vdbX1 and vdbY1 are same = 0 */
      bg_vdbX2 = vdbY2;
      bg_vdbY2 = vdbX2;

      tmpImageAddr = (uint32)bg_image_addr - bg_y1*bg_width*bgBpp;
      bg_image_addr = (uint32 *)tmpImageAddr;
    }
    else
    {
      bg_x2 = x2;
      bg_y2 = y2;
    }
  }

  mdpCalCropXY(x1+x2_align, x2, y1, y2, vdbX1, vdbX2, vdbY1, vdbY2, mdpOp, &mdpCrop);
  mdpCalCropXY(bg_x1+x2_align, bg_x2, bg_y1, bg_y2, bg_vdbX1, bg_vdbX2,
               bg_vdbY1, bg_vdbY2, bg_mdpOp, &bg_mdpCrop);

  cx1_off = (int32)(mdpCrop.cx1 - bg_mdpCrop.cx1);
  cy1_off = (int32)(mdpCrop.cy1 - bg_mdpCrop.cy1);


  cx1_off = (cx1_off>0)?cx1_off:-cx1_off;
  cy1_off = (cy1_off>0)?cy1_off:-cy1_off;


  if (cx1_off || cy1_off)
  {
    if (mdpOp&MDPOP_ROT90)
    {
      if (bg_mdpOp&MDPOP_ROT90)
      {
        tmpImageAddr = (uint32)bg_image_addr - cy1_off*bgBpp - cx1_off*bgBpp*bg_width;
        bg_image_addr = (uint32 *)tmpImageAddr;
      }
      else
      {
        tmpImageAddr = (uint32)bg_image_addr + cx1_off*bgBpp - cy1_off*bgBpp*bg_width;
        bg_image_addr = (uint32 *)tmpImageAddr;
      }
    }
    else
    {
      if (bg_mdpOp&MDPOP_ROT90)
      {
        tmpImageAddr = (uint32)bg_image_addr - cy1_off*bgBpp + cx1_off*bgBpp*bg_width;
        bg_image_addr = (uint32 *)tmpImageAddr;
      }
      else
      {
        tmpImageAddr = (uint32)bg_image_addr - cx1_off*bgBpp - cy1_off*bgBpp*bg_width;
        bg_image_addr = (uint32 *)tmpImageAddr;
      }
    }
  }

  if ((scrPtr = mdp_scr_get(MDP_GOG_SCR)) == NULL)
  {
    return MDP_FAIL;
  }
  mdpScrCnt->gog_scr_cnt++;

  if (mdpCalFFRdataBg(bg_image_addr,
                      bg_mdpOp,
                      bg_width,
                      bg_vdbX1,
                      bg_vdbX2,
                      bg_vdbY1,
                      bg_vdbY2,
                      &mdpFFRData,
                      bg_imgType) != MDP_SUCCESSFUL)
  {
    return MDP_FAIL;
  }

#ifdef FEATURE_MDP_WSCALE
  scrPtr[17] = wScaleOp;
#endif

  scrPtr[1] = mdpFFRData.imgAddr;
  scrPtr[2] = mdpFFRData.line;
  scrPtr[3] = mdpFFRData.block;
  scrPtr[4] = mdpFFRData.stripe;

  if (mdpCalFFRdataFg(image_addr,
                      mdpOp,
                      width,
                      vdbX1,
                      vdbX2,
                      vdbY1,
                      vdbY2,
                      &mdpFFRData,
                      imgType) != MDP_SUCCESSFUL)
  {
    return MDP_FAIL;
  }

  scrPtr[6] = mdpFFRData.imgAddr;
  scrPtr[7] = mdpFFRData.line;
  scrPtr[8] = mdpFFRData.block;
  scrPtr[9] = mdpFFRData.stripe;

  scrPtr[11] = SET_FFR_MODE(0, ffr_mode, bg_mdpOp, mdpOp);

  if (mbcOp != MDP_MBC_NOP)
  {
    scrPtr[13] = SET_MBC_ACTION(mbcOp>>MDPOP_MBC_SHIFT) | mdp_gamma_enable;
    scrPtr[13] |= mdp_dither_on ? SET_MBC_ACTION_DITHER:0;

    if (mbcOp&MDPOP_TRANSP)
    {
      scrPtr[16] = SET_MBC_TV(tpVal); //tp value
    }
    else
    {
      scrPtr[16] = NOP;
    }

    if (mbcOp&MDPOP_ALPHAB)
    {
      if (alphaBkg == 0)
      {
        alphaBkg = 1;
        alphaFrg = 0xff;
      }

      if (alphaFrg == 0)
      {
        alphaBkg = 0xff;
        alphaFrg = 1;
      }

      scrPtr[12] = SET_MBC_ALPHA(alphaBkg, alphaFrg); //tp value
    }
    else
    {
      scrPtr[12] = NOP;
    }

  }
  else
  {
    scrPtr[12] = NOP;
    scrPtr[13] = NOP;
    scrPtr[16] = NOP;
  }

  switch (lcdPhysDest[destDev])
  {
    case MEM_WR:
      scrPtr[10] = SET_LCD_FORMAT(1, 0x11);
      scrPtr[14] = SET_LCD_DATA_ADDR;

      if ((output_addr == 0) || ((uint32)output_addr > MAX_MEM_LIMIT))
        return MDP_FAIL;

      // Add 0x1 for destination address auto-increment
      scrPtr[15] = ((uint32) output_addr) + 0x1;

      scrPtr[23] = NOP;
      scrPtr[28] = WAIT_JOB_DONE;
      break;

#ifndef CUST_EDITION
    case EP_OP_QCIFP_PRI:
      if ((lcdScrPtr = mdp_scr_get(MDP_EP_OP_QCIFP_PRI_SCR)) == NULL)
        return MDP_FAIL;
      mdpScrCnt->ep_op_qcifp_pri_scr_cnt++;
  
      scrPtr[10] = SET_LCD_FORMAT(0, 0);
      scrPtr[14] = SUBROUTINE;
      scrPtr[15] = (uint32) lcdScrPtr;
  
      lcdScrPtr[7] = SEND_LCD_DATA(lcd_col_start << 10);
      lcdScrPtr[8] = SEND_LCD_DATA(lcd_row_start << 10);
    
      lcdScrPtr[10] = SEND_LCD_DATA(lcd_col_end << 10);
      lcdScrPtr[11] = SEND_LCD_DATA(lcd_row_end << 10);
      break;


    case EP_OP_QCIFP_SEC:
      if ((lcdScrPtr = mdp_scr_get(MDP_EP_OP_QCIFP_SEC_SCR)) == NULL)
        return MDP_FAIL;
      mdpScrCnt->ep_op_qcifp_sec_scr_cnt++;
  
      scrPtr[10] = SET_LCD_FORMAT(0, 0);
      scrPtr[14] = SUBROUTINE;
      scrPtr[15] = (uint32) lcdScrPtr;
  
      lcdScrPtr[7] = SEND_LCD_DATA(lcd_col_start << 10);
      lcdScrPtr[8] = SEND_LCD_DATA(lcd_row_start << 10);
    
      lcdScrPtr[10] = SEND_LCD_DATA(lcd_col_end << 10);
      lcdScrPtr[11] = SEND_LCD_DATA(lcd_row_end << 10);
      break;


    case EPSON_QCIF:
      if ((lcdScrPtr = mdp_scr_get(MDP_EPSON_QCIF_SCR)) == NULL)
        return MDP_FAIL;
      mdpScrCnt->epson_qcif_scr_cnt++;

      scrPtr[10] = SET_LCD_FORMAT(0, 8);
      scrPtr[14] = SUBROUTINE;
      scrPtr[15] = (uint32) lcdScrPtr;

      scrPtr[23] = NOP;
      scrPtr[28] = WAIT_JOB_DONE;

      lcdScrPtr[7] = SEND_LCD_DATA(lcd_row_start & 0xFF);
      lcdScrPtr[8] = SEND_LCD_DATA(lcd_row_start >> 8);

      lcdScrPtr[9] = SEND_LCD_DATA(lcd_row_end & 0xFF);
      lcdScrPtr[10] = SEND_LCD_DATA(lcd_row_end >> 8);

      lcdScrPtr[13] = SEND_LCD_DATA((lcd_col_start+2) & 0xFF);
      lcdScrPtr[14] = SEND_LCD_DATA((lcd_col_start+2) >> 8);

      lcdScrPtr[15] = SEND_LCD_DATA((lcd_col_end+2) & 0xFF);
      lcdScrPtr[16] = SEND_LCD_DATA((lcd_col_end+2) >> 8);
      break;
	
	case EPSON_QVGA:
      if ((lcdScrPtr = mdp_scr_get(MDP_EPSON_QVGA_SCR)) == NULL)
        return MDP_FAIL;
      mdpScrCnt->epson_qvga_scr_cnt++;

      scrPtr[10] = SET_LCD_FORMAT(0, 0x2);
      scrPtr[14] = SUBROUTINE;
      scrPtr[15] = (uint32) lcdScrPtr;

      scrPtr[23] = NOP;
      scrPtr[28] = WAIT_JOB_DONE;

      lcdScrPtr[7] = SEND_LCD_DATA((uint8)(lcd_col_start)      << 1);
      lcdScrPtr[8] = SEND_LCD_DATA((uint8)(lcd_row_start >> 8) << 1);
      lcdScrPtr[9] = SEND_LCD_DATA((uint8)(lcd_row_start)      << 1);

      lcdScrPtr[12] = SEND_LCD_DATA((uint8)(lcd_col_end)      << 1);
      lcdScrPtr[13] = SEND_LCD_DATA((uint8)(lcd_row_end >> 8) << 1);
      lcdScrPtr[14] = SEND_LCD_DATA((uint8)(lcd_row_end)      << 1);

      if(mdp_toggle_output_format_enable)
        lcdScrPtr[18] = SET_LCD_FORMAT(0, 0x16);
      else
        lcdScrPtr[18] = NOP;

      break;

    case TMD_QVGA:
      if ((lcdScrPtr = mdp_scr_get(MDP_TMD_QVGA_SCR)) == NULL)
        return MDP_FAIL;
      mdpScrCnt->tmd_qvga_scr_cnt++;

      scrPtr[10] = SET_LCD_FORMAT(0, 0);
      scrPtr[14] = SUBROUTINE;
      scrPtr[15] = (uint32) lcdScrPtr;

      scrPtr[23] = NOP;
      scrPtr[28] = WAIT_JOB_DONE;

      lcdScrPtr[7] = SEND_LCD_DATA(DISP_QVGA_18BPP(lcd_col_start + tmd20qvga_panel_offset));
      lcdScrPtr[9] = SEND_LCD_DATA(DISP_QVGA_18BPP(lcd_col_end + tmd20qvga_panel_offset));
      lcdScrPtr[11] = SEND_LCD_DATA(DISP_QVGA_18BPP(lcd_row_start));
      lcdScrPtr[13] = SEND_LCD_DATA(DISP_QVGA_18BPP(lcd_row_end));

      lcdScrPtr[15] = SEND_LCD_DATA(DISP_QVGA_18BPP(lcd_col_start + tmd20qvga_panel_offset));
      lcdScrPtr[17] = SEND_LCD_DATA(DISP_QVGA_18BPP(lcd_row_start));

      break;
#else

    case MDP_LCD:
      if ((lcdScrPtr = mdp_scr_get(MDP_LCD_SCR)) == NULL)
        return MDP_FAIL;
      mdpScrCnt->lcd_scr_cnt++;
      
      scrPtr[10] = SET_LCD_FORMAT(0, disp_drv_mdp_getformat());
      scrPtr[14] = SUBROUTINE;
      scrPtr[15] = (uint32) lcdScrPtr;

      scrPtr[23] = NOP;
      scrPtr[28] = WAIT_JOB_DONE;

      disp_drv_mdp_scrupdate(lcdScrPtr, lcd_row_start, lcd_col_start, lcd_row_end, lcd_col_end);
      break;

#endif
#ifdef FEATURE_MDDI
#error code not present
#endif /* FEATURE_MDDI */


    default: return MDP_FAIL;
  }

  scrPtr[18] = SET_VDB_CROP_X(mdpCrop.cx1, mdpCrop.cx2);
  scrPtr[19] = SET_VDB_CROP_Y(mdpCrop.cy1, mdpCrop.cy2);


  // 1: enable MDP pipleline, stripe/image -1, #ofBlocks/stripe -1
  // number of lines per stripe = 16
  // 1 block = 16pixel x 16pixel
  if (mdpOp&MDPOP_ROT90)
  {
    scrPtr[20] = SET_VDB_SIZE(mdp_sfd_enable,
                              (vdbX2-vdbX1)/16,
                              (vdbY2-vdbY1)/16);
  }
  else
  {
    scrPtr[20] = SET_VDB_SIZE(mdp_sfd_enable,
                              (vdbY2-vdbY1)/16,
                              (vdbX2-vdbX1)/16);
  }

  scrPtr[22] = MAX_MEM_LIMIT;
  scrPtr[MDP_GOG_LINK_POS] = NOP;
  scrPtr[MDP_GOG_LINK_POS+1] = NOP;

  if (linkAddr)
  {
    linkAddr[0] = JUMP;
    linkAddr[1] = (uint32)scrPtr;
  }

  return(int)scrPtr;
}



/*===========================================================================

FUNCTION mdp_vsync_handler()

DESCRIPTION
  This function is to install MDP vsync handler

DEPENDENCIES
  Vsync input device is initialized

RETURN VALUE
  None

SIDE EFFECTS
  MDP AHB clock and interrupts are being enabled

===========================================================================*/
void mdp_vsync_handler(void)
{
  timetick_type mdp_last_vsync_timetick_store ;
  // reset MDP vsync counter to 0 by assigning the current tick counter
  // to LCD line counter 0
  mdp_last_vsync_timetick_store = timetick_get();//mdp_last_vsync_timetick_double[mdp_last_vsync_timetick_index%2];
  INTLOCK()
  mdp_last_vsync_timetick = mdp_last_vsync_timetick_store;
  INTFREE()
  outpdw(MDP_COMMAND, LOAD_RD_PTR);

  if(mdp_debug_vsync){
   MDP_MSG_HIGH("Got vsync timer at %u", mdp_last_vsync_timetick, 0, 0);
  }
#ifndef FEATURE_MDDI
    // disabling GPIO interrupt
    (void) gpio_int_set_handler (MDP_GPIO_BLANKING_SIGNAL, MDP_GPIO_ACTIVE_LEVEL, NULL); 
#endif /*FEATURE_MDDI*/

  
}


/*===========================================================================

FUNCTION mdp_set_vsync()

DESCRIPTION
  This function is to synchronize between MDP vsync counter and LCD refresh
  pulse

DEPENDENCIES
  Vsync input device is initialized

RETURN VALUE
  None

SIDE EFFECTS
  MDP AHB clock and interrupts are being enabled

===========================================================================*/
void mdp_set_vsync(void)
{
#ifdef MDP_HW_VSYNC  
#ifdef FEATURE_MDDI
#error code not present
#else /* FEATURE_MDDI */

    /*For EBI-2 panels where h/w vsync is enabled thru GPIO13*/

     
    // Under HW vsync, MDP VSYNC GPIO ISR needs to call mdp_vsync_handler()
    // GPIO ISR needs to call mdp_vsync_handler() and disable GPIO interrupt
         
  (void) gpio_int_set_handler (MDP_GPIO_BLANKING_SIGNAL, MDP_GPIO_ACTIVE_LEVEL, mdp_vsync_handler); 

#endif /*FEATURE_MDDI*/
#endif /* MDP_SW_VSYNC */
}


/*===========================================================================

FUNCTION mdp_hw_init()

DESCRIPTION
  This function is to initialize MDP HW block

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  MDP AHB clock and interrupts are being enabled

===========================================================================*/
void mdp_hw_init(void)
{
  static uint32 default_cgc = 0x1801;
  static uint32 default_hclk = 0;

  /* enabling the MDP AHB clock */
#ifdef FEATURE_DUAL_CLK_RGM
  clk_regime_msm_enable(CLK_RGM_MDP_M);
#else
  clk_regime_enable(CLK_RGM_MDP_M);
#endif

  // Configure MDP CGCs - Enable clocks to all the MDP subblocks
  outpw(MDP_CGC_CONFIG, default_cgc);

  // Initialize MDP Interrupts
  outpw(MDP_INTR_ENABLE, 0x0);
  outpw(MDP_INTR_CLEAR, 0xFFFF);
  CIP_INTR_EN;  /* enabling command in process interrupt */

  default_hclk = clk_regime_msm_get_clk_freq_khz(CLK_RGM_HCLK_M) * 1000;
#ifndef MDP_SYS_VSYNC_OFF 
  mdp_config_vsync(default_hclk);  
 
#ifdef MDP_HW_VSYNC
  gpio_tlmm_config(MDP_HW_VSYNC_SIGNAL);
#endif  
#endif
}

/*===========================================================================

FUNCTION mdp_config_vsync()

DESCRIPTION
  This function is to configure vsync counter

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void mdp_config_vsync(uint32 hclk)
{
  uint16 SST_TERM_DIV_CNT = 0;

  if(hclk == 0) 
    return;

#ifndef MDP_SYS_VSYNC_OFF
#ifdef MDP_SW_VSYNC

#ifdef FEATURE_MDDI
#error code not present
#else /* FEATURE_MDDI */
  if(!mdp_mddi_enable)
  {
	porch_line = mdp_ebi_rows_per_refresh - mdp_prim_lcdHeight;
    porch_line = (porch_line < 0)?0:porch_line;
    back_porch_line = porch_line; 
    prim_lcd_lines = mdp_prim_lcdHeight + porch_line;
    // Time taken by LCD module to write a line/row of pixels (in terms if AHB clock cycles)
    SST_TERM_DIV_CNT = hclk / (LCD_REFRESH_EBI * LCD_DISPLAY_LINES);
    outpdw(MDP_SYNC_CONFIG, (SW_VSYNC | SST_TERM_DIV_CNT)); // MDDI build SST_TERM_DIV_CNT is 'don't care'    
    outpdw(HWIO_MDP_ADDRESS_14_ADDR, 0);                    // line starts at 0                                                            
    outpdw(HWIO_MDP_ADDRESS_15_ADDR, LCD_DISPLAY_LINES);    // maximum line number    
  }
#endif /*FEATURE_MDDI*/

#elif defined (MDP_HW_VSYNC)

#ifdef FEATURE_MDDI
#error code not present
#endif /* FEATURE_MDDI */
    SST_TERM_DIV_CNT = hclk / (LCD_REFRESH_EBI * LCD_DISPLAY_LINES);

  outpdw(MDP_SYNC_CONFIG, (PRIM_VSYNC | SST_TERM_DIV_CNT) );
  outpdw(HWIO_MDP_ADDRESS_14_ADDR, 0);                       // line starts at 0
  outpdw(HWIO_MDP_ADDRESS_15_ADDR, LCD_DISPLAY_LINES);       // maximum line number
  
#endif /* MDP_SW_VSYNC */

  //mdp_vsync_enabled = TRUE;
  

#endif /* MDP_SYS_VSYNC_OFF */

}

/*===========================================================================

FUNCTION mdp_reset()

DESCRIPTION
  This function is to reset MDP HW block.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdp_reset(void)
{
#ifdef FEATURE_DISPLAY_MDP_HARD_FAILURE
#error code not present
#else /* FEATURE_DISPLAY_MDP_HARD_FAILURE */

#ifdef FEATURE_DUAL_CLK_RGM
  clk_regime_msm_reset(CLK_RGM_MDP_M);
#else
  clk_regime_reset(CLK_RGM_MDP_M);
#endif

  mdp_hw_init();

  /* initialize CCS */
  mdp_set_ccs(&mdp_default_ccs, TRUE);      

  /////////////////////////////////////////  
  // mdp isr clean up 
  /////////////////////////////////////////  
  
  // no need to INTLOCK since HW just got reset  
  if (mdp_tcb_ptr != NULL)
  {
#ifdef FEATURE_MDDI
#error code not present
#endif  
    rex_set_sigs(mdp_tcb_ptr, mdp_task_wait_sig);

    /* let's release the user's buffer */            
    if (NULL != mdp_user_cb.cb_func)
    {
      disp_q_put_complete((disp_packet_type *)mdp_user_cb.arg);      
      rex_set_sigs(&disp_tcb, DISP_CMD_MDP_CB);
      mdp_user_cb.cb_func = NULL;
    }
  }

  mdp_in_processing = FALSE; 
  /////////////////////////////////////////  
#endif /* FEATURE_DISPLAY_MDP_HARD_FAILURE */
}

/*===========================================================================

FUNCTION mdp_isr()

DESCRIPTION
  This function is to handle MDP interrupts.
  This function basically clears MDP signal and asserts SLEEP signal.  Then,
  if the job queue is empty, it sets MDP driver in the idle state.

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdp_isr(void)
{
  mdp_sum_isr++;
  if (CIP_INTR)
  {
    mdp_sum_isr_cip++;
    if (mdp_tcb_ptr != NULL)
    {           
#ifdef FEATURE_MDDI
#error code not present
#endif  
      mdp_sum_tcb_sigclr_isr++;
	  //last_update_width = update_width;
    if(mdp_debug_vsync)
	{
     MDP_MSG_HIGH("mdp_last_dma2_update_width is %u", last_update_width, 0, 0);
     MDP_MSG_HIGH("mdp_last_dma2_update_height is %u", last_update_height, 0, 0);
    }
      rex_set_sigs(mdp_tcb_ptr, mdp_task_wait_sig);

      /* let's release the user's buffer */            
      if (NULL != mdp_user_cb.cb_func)
      {
        disp_q_put_complete((disp_packet_type *)mdp_user_cb.arg);
        // can't callback from ISR context...
        // let's ask disp_task to call it.
        rex_set_sigs(&disp_tcb, DISP_CMD_MDP_CB);
        mdp_user_cb.cb_func = NULL;
      }
    }

    /* check if it is end of the job queue for pipeline processing */
    /* we want to vote for sleep if the job queue is empty */
    mdp_hclk_not_active();
    mdp_in_processing = FALSE;

     mdp_hw_finish_timestamp_ms = timetick_get_ms();

	 if(!mdp_update_memMode)
	 {
      mdp_dma2_update_time_in_usec = timetick_get_elapsed(mdp_dma2_start_time_in_usec, T_USEC);  
      if(mdp_debug_vsync)
	  {
       MDP_MSG_HIGH("mdp_dma2_update_time_in_usec is %u", mdp_hw_finish_timestamp_ms - mdp_hw_start_timestamp_ms, 0, 0);
       MDP_MSG_HIGH("mdp_dma2_update_time_in_usec is %u", mdp_dma2_update_time_in_usec, 0, 0);
      }
      mdp_update_time[last_update_height] = mdp_dma2_update_time_in_usec;
    }
    else{
    mdp_dma2_update_time_in_usec = 0;
    if(mdp_debug_vsync)
	{
      MDP_MSG_HIGH("global memode  is true", 0, 0, 0);
     }
    }
  }
  /* let's clear all INT for now */
  outpw(MDP_INTR_CLEAR, 0xFFFF);
}

/*===========================================================================

FUNCTION mdp_set_lcd_size()

DESCRIPTION
  This function is to store LCD information

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdp_set_lcd_size_API(LCD_TYPE dest, LCD_TYPE_PHYS physDest, uint32 width, uint32 height)
{
  MDPIMG mdpImg;
  uint32 ovId;

  if (mdp_drv_initialized != TRUE)
    return;

  rex_enter_crit_sect(&mdp_crit_sect);

  lcdPhysDest[dest] = physDest;
  mdpLcdSize[dest][COL_IDX] = width-1;
  mdpLcdSize[dest][ROW_IDX] = height-1;

  /* registering white background */
  mdpImg.dest = dest; 
  mdpImg.imgType = MDP_RGB565;
  mdpImg.width = width;
  mdpImg.cwin.x1 = 0;
  mdpImg.cwin.x2 = mdpLcdSize[dest][COL_IDX];
  mdpImg.cwin.y1 = 0;
  mdpImg.cwin.y2 = mdpLcdSize[dest][ROW_IDX];
  mdpImg.lcd_x = 0-BREW_OFFSET_X;
  mdpImg.lcd_y = 0-BREW_OFFSET_Y;
  lcd_y =  mdpImg.lcd_y + BREW_OFFSET_Y ; 
  /*lcd_y is the estimated LCD read pointer location*/
  /*lcd_x can be thought to be the exact pixel location
  on a line, which we are not bothered about now. 
  So, lcd_x is not being calculated*/ 
  mdpImg.mdpOp = MDPOP_NOP;
  mdpImg.layer = MDP_LAYER_0;
  mdpImg.bmy_addr = (uint32*)(white_frame->buffer);

  ovId = mdp_register_ovimg_sub_API(&mdpImg, NULL);   

  if (ovId != NULL)
  {
    mdp_lock_ovimg_API(ovId, TRUE);  
    mdp_enable_locked_ovimg_API(ovId, FALSE);    
    mdpWhiteBgOvId[dest] = ovId;   
    // to prevent the system to remove this overlay  
    ((MDPIMG_LIST *)mdpWhiteBgOvId[dest])->key_id = 0;
  }

  rex_leave_crit_sect(&mdp_crit_sect);
}


/*===========================================================================

FUNCTION mdp_drv_init()

DESCRIPTION
  This function is to intialize MDP HW and driver data.

DEPENDENCIES
  mdp_install() installs all driver entry point functions

RETURN VALUE
  Always return TRUE

SIDE EFFECTS
  MDP AHB clock and interrupts are being enabled

===========================================================================*/
int mdp_drv_init(void)
{
  int32 i;
  MDPIMG mdpImg;
  uint32 chip_rev;
#ifdef FEATURE_CLKREGIM_DEVMAN
  clkrgm_device_param_type param;
#endif /* FEATURE_CLKRGM_DEVMAN */
  if (mdp_drv_initialized != TRUE)
  {
    rex_init_crit_sect(&mdp_crit_sect);

    mdp_tcb_ptr = NULL;
    tramp_set_isr(TRAMP_MDP_ISR, mdp_isr);
    mdp_sys_bmp = NULL;

    // retrieve primary lcd info and determine which LCD type we're using.
    _primary_lcd = disp_get_info();
    mdp_prim_lcdWidth = _primary_lcd.disp_width;
    mdp_prim_lcdHeight = _primary_lcd.disp_height;
	
#ifndef FEATURE_MDDI 
  mdp_ebi_rows_per_refresh = LCD_DISPLAY_LINES; /**mapping mddi and ebi parameters. this keeps the get_lcd_line_counter algo unchanged.**/
  mdp_ebi_rows_per_second = LCD_REFRESH_EBI * LCD_DISPLAY_LINES;
  #ifdef MDP_HW_VSYNC  
  gpio_tlmm_config(MDP_GPIO_BLANKING_SIGNAL_INPUT); //configure gpio as input signal
  #endif
#else /*FEATURE_MDDI*/
#error code not present
#endif /* FEATURE_MDDI */

    // MDP HW needs to be initialized prior to reading revision value
    //
    mdp_hw_init();

    /* Rev ID not supported for 1.0 so default below */
    mdp_rev = mdp_read_debug_bus(MDP_DBGBUS_SEL_REV) & MDP_BIT_REV_MASK;

    /* Get the Chip ID */
    chip_rev = inpdw(HWIO_HW_REVISION_NUMBER_ADDR);

    switch (mdp_rev)
    {
	  case MDP_BIT_REV_01_04: mdp_rev = MDP_REV_01_04; break;
      case MDP_BIT_REV_01_03: mdp_rev = MDP_REV_01_03; break;
      case MDP_BIT_REV_01_02: mdp_rev = MDP_REV_01_02; break;
      case MDP_BIT_REV_01_01: mdp_rev = MDP_REV_01_01; break;
      default:                mdp_rev = MDP_REV_01_00; break;                  
    }

    /* Check to see if we can turn on Simultaneous-Fill-and-Dump */
    /* Display corruption will occur if enabling SFD on unsupported HW */
    if ((mdp_mddi_enable == TRUE) && (mdp_rev == MDP_REV_01_00) || 
        (chip_rev == 0x000c00e1) || /* 6280-R90 ver 1.0 */
        (chip_rev == 0x000d40e1) || /* 6245-F65 ver 1.0 */
        (chip_rev == 0x000d00e1) || /* 6260-F65 ver 1.0 */
        (chip_rev == 0x020700e1) || /* 6800-D65 w/ GSM ver 1.0 */
        (chip_rev == 0x020710e1) || /* 6800-D65 w/o GSM ver 1.0 */
        (chip_rev == 0x040570e1) || /* 6550-E90 ver 1.0 */
        (chip_rev == 0x140570e1) || /* 6550-E90 ver 1.1 */
        (chip_rev == 0x240570e1))   /* 6550-E90 ver 2.0 */
    {
      mdp_sfd_enable = 0;
    }
    else
    {
      mdp_sfd_enable = 1;
    }
    
	/* Software workaround: retrieve HW revision via supported interface, check
    ** if we need to swap between 16-bit vs 32-bit MDP transfers for LCD command
    ** and pixel data respectively since MDP HW assumes same format for both */
    if ((mdp_mddi_enable == FALSE) && 
        (mdp_rev == MDP_REV_01_04) && (chip_rev == 0x002C00E1))
    {
      mdp_toggle_output_format_enable = TRUE;
    }
    else
    {
      mdp_toggle_output_format_enable = FALSE;
    }

   
#ifdef FEATURE_MDDI
#error code not present
#endif /* FEATURE_MDDI */

    // Remove SET_DEFAULT commands from MDP script for HW rev 1.1 and beyond.
    // SET_DEFAULT is required for 1.0 proper operation.
    // Remove so that MDP ending state can be read out on debug bus.
    if (mdp_rev != MDP_REV_01_00)
    {
      // traverse each script and replace SET_DEFAULT commands with NOP
      mdp_script_replace((uint32 *)MDP_DISP_GFX_SCR, /* GFX */
                         MDP_GFX_SCR_SIZE,
                         MDP_GFX_SCR_LEN,
                         SET_DEFAULT,           /* Old command */
                         NOP);                  /* New command */

      mdp_script_replace((uint32 *)MDP_DISP_GOG_SCR, /* GOG */
                         MDP_GOG_SCR_SIZE,
                         MDP_GOG_SCR_LEN,
                         SET_DEFAULT,           /* Old command */
                         NOP);                  /* New command */

      mdp_script_replace((uint32 *)MDP_DISP_VDO_SCR, /* VDO */
                         MDP_VDO_SCR_SIZE,
                         MDP_VDO_SCR_LEN,
                         SET_DEFAULT,           /* Old command */
                         NOP);                  /* New command */

      mdp_script_replace((uint32 *)MDP_DISP_GOV_SCR, /* GOV */
                         MDP_GOV_SCR_SIZE,
                         MDP_GOV_SCR_LEN,
                         SET_DEFAULT,           /* Old command */
                         NOP);                  /* New command */
    }

#ifdef FEATURE_MDDI
#error code not present
#endif /* FEATURE_MDDI */

    // for secondary lcd type
    _secondary_lcd = disp_get_info2(); 

    /* Determine which display is biggest... */
    if((_primary_lcd.disp_width * _primary_lcd.disp_height) >=
       (_secondary_lcd.disp_width * _secondary_lcd.disp_height)) 
    {
      /* Primary panel is larger than secondary */ 
      _biggest_lcd = _primary_lcd;
    } 
    else 
    {
      /* Secondary panel is larger than primary */
      _biggest_lcd = _secondary_lcd;
    } 

    /* Allocate Screen Buffers after these 3 globals have been set:
    ** _primary_lcd, _secondary_lcd, - _biggest_lcd */
    if(mdp_init_system_frames() == FALSE)
    {
      ERR_FATAL("ERROR: mdp_drv_init(): Failed allocating buffer memory!",0,0,0);
      return MDP_FAIL;
    }

    mdp_imglist_init();
    mdp_scrlist_init();

    for (i=0;i<MAX_LCD_NUM;i++)
    {
      mdpLcdSize[i][COL_IDX] = -1;
      mdpLcdSize[i][ROW_IDX] = -1;
    }

    for (i=0;i<MAX_LCD_NUM;i++)
    {
      lcdPhysDest[i] = LCD_NONE;
      mdpLcdSize[i][COL_IDX] = -1;
      mdpLcdSize[i][ROW_IDX] = -1;
    }
    lcdPhysDest[MEM_WR] = MEM_WR;

    mdp_total_vdopkts = 0;
    mdp_in_processing = FALSE;

    /**************************/
    memset(&mdpImg, 0 , sizeof(MDPIMG));	
    mdpImg.imgType = MDP_RGB565;
    mdpImg.cwin.x1 = 0;
    mdpImg.cwin.y1 = 0;
    mdpImg.lcd_x   = 0-MDP_SYS_OFFSET_X;
    mdpImg.lcd_y   = 0-MDP_SYS_OFFSET_Y;
    mdpImg.mdpOp   = MDPOP_NOP;
    mdpImg.layer   = MDP_LAYER_2;
    mdpImg.mdpOp   = MDPOP_TRANSP;

    // primary sub layer buf node initialization
    mdpImg.bmy_addr = (uint32 *)(mdp_slBuf_1->buffer);
    mdpImg.dest     = PRIMARY_LCD_TYPE;
    mdpImg.width    = mdp_slBuf_1->pxWidth;
    mdpImg.cwin.x2  = mdp_slBuf_1->pxWidth - 1;
    mdpImg.cwin.y2  = mdp_slBuf_1->pxHeight -1;

    if ((mdpSubNode = mdp_imglist_get()) == NULL)
      return MDP_FAIL;

    mdpSubNode->mbcOp = MDPOP_TRANSP;
    mdpSubNode->img = mdpImg;
    mdpSubNode->orig_img = mdpImg;

    mdpSubNode->img.lcd_x += MDP_SYS_OFFSET_X;
    mdpSubNode->img.lcd_y += MDP_SYS_OFFSET_Y;

    mdp_touch_img(mdpSubNode);

    // enable and lock it at default
    // since it's locked, appCtx is a don't care
    mdpSubNode->enabled = TRUE;
    mdpSubNode->locked = TRUE;
    /**************************/

#ifdef FEATURE_MDP_SECONDARY_DISPLAY
#error code not present
#endif /* FEATURE_MDP_SECONDARY_DISPLAY */

    //initializing YCbCr Color Coefficient Matrix & Gamma Table   
    MDP_DISP_GAMMA_SCR[1] = (uint32) &GAMMA_R[0];
    MDP_DISP_GAMMA_SCR[3] = (uint32) &GAMMA_G[0];
    MDP_DISP_GAMMA_SCR[5] = (uint32) &GAMMA_B[0];

    /* initialize CCS */    
    mdp_set_ccs(&mdp_default_ccs, FALSE);
    /* MDP HW vsync counter */
    for (i=0;i<MDP_MAX_PIXEL_WIDTH;i++)
    {
      mdp_width_boundary[i] = 0;
    }
  

#ifdef FEATURE_NEW_SLEEP_API
  if((mdp_sleep_handle = sleep_register("MDP", TRUE)) == 0)
    return MDP_FAIL;

#ifdef CLK_REGIME_UP_SLEEP__MCLK_HCLK
  sleep_set_hclk_restriction(mdp_sleep_handle, CLKRGM_HCLK_MDP);
#endif /* CLK_REGIME_UP_SLEEP__MCLK_HCLK */

 #endif /* FEATURE_NEW_SLEEP_API */

#ifdef FEATURE_CLKREGIM_DEVMAN
  /* Register callback for dynamic hclk switching */
  clk_regime_devman_register(&mdp_client, CLKRGM_DEVMAN_CLIENT_GROUP_DEFAULT);

  param.speed_change_handler = mdp_clkrgm_cb;
  clk_regime_devman_request(&mdp_client, 
                            CLKRGM_DEVICE_HBUS, 
                            CLKRGM_DEVICE_REQUEST_SPEED_CHANGE_NOTIFICATION, 
                            &param);
#endif /* FEATURE_CLKREGIM_DEVMAN */

#ifndef FEATURE_MDDI
#ifdef MDP_HW_VSYNC  
(void) gpio_int_set_detect(MDP_GPIO_BLANKING_SIGNAL, DETECT_LEVEL);
#endif
#endif /*FEATURE_MDDI*/
  mdp_vsync_resync_time= VSYNC_RESYNC_TIME_IN_MS;
  mdp_drv_initialized = TRUE;
  #ifndef MDP_SYS_VSYNC_OFF
  mdp_vsync_enabled = TRUE;
  #endif
  }
  return MDP_SUCCESSFUL;
}

/*===========================================================================

FUNCTION mdp_script_replace()

DESCRIPTION
  Traverse through the MDP scripts to replace all instances of a command
  with a new command.

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void mdp_script_replace(uint32 *ptr_script,
                               uint32 script_size,
                               uint32 script_length,
                               uint32 old_command,
                               uint32 new_command)
{
  int i, tot_length;

  tot_length = script_size * script_length;

  for (i = 0; i < tot_length; i++)
  {
    if (*ptr_script == old_command)
    {
      *ptr_script = new_command;
    }
    ptr_script++;
  } // script_size

}

/*===========================================================================

FUNCTION mdp_get_lcd_line_counter()

DESCRIPTION
  This function is to convert the elapsed timetick to the corresponding LCD 
  line number and return it as lcd line counter.

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  LCD current read pointer (line counter)

SIDE EFFECTS
  None
===========================================================================*/
#ifdef FEATURE_MDDI
#error code not present
#endif /*FEATURE_MDDI*/

/*********************S/w Vsync Logic For EBI-2 LCD panels on SC2x ******************************/
#ifndef FEATURE_MDDI 



uint32 mdp_get_lcd_line_counter()
{
  timetick_type elapsed_usec_time;
  uint32 lcd_line =0;
  timetick_type mdp_last_vsync_timetick_local;

   
  if(!mdp_ebi_rows_per_refresh || !mdp_ebi_rows_per_second)
      return 0;

  

  if(mdp_ebi_rows_per_refresh && mdp_ebi_rows_per_second){
   lcd_ref_hz = mdp_ebi_rows_per_second*100/mdp_ebi_rows_per_refresh;
   lcd_ref_usec_time =  NUM_USEC_IN_MSEC / (mdp_ebi_rows_per_second/mdp_ebi_rows_per_refresh);
  }
  else
  {
      MDP_MSG_ERROR("Divide by zero",0,0,0);
  }

  // no need for INTLOCK since the worst case would be reading one before
  // the last update
  INTLOCK()// 
  mdp_last_vsync_timetick_local = mdp_last_vsync_timetick;
  INTFREE()

  if (mdp_last_vsync_timetick_local == 0)
    return 0;

  //get time since the last sync happened
  elapsed_usec_time = timetick_get_elapsed(mdp_last_vsync_timetick_local, T_USEC); 

  if(mdp_debug_vsync){
   MDP_MSG_HIGH("elapsed_usec_time before adjustment %u ",elapsed_usec_time,0,0);
   MDP_MSG_HIGH("lcd_ref_usec_time %u ",lcd_ref_usec_time,0,0); 
  }

  elapsed_usec_time = elapsed_usec_time%lcd_ref_usec_time;

  if(mdp_debug_vsync){
   MDP_MSG_HIGH("elapsed_usec_time %u ",elapsed_usec_time,0,0);
   MDP_MSG_HIGH("prim_lcd_lines %u", prim_lcd_lines, 0, 0);
  }

  // lcd line calculation referencing to line counter = 0
  if(lcd_ref_usec_time != 0)
  {
   lcd_line = (elapsed_usec_time*prim_lcd_lines)/lcd_ref_usec_time;
  }
  else 
  {
	  MDP_MSG_ERROR("lcd_ref_usec_time is ZERO",0,0,0);
  }


  if(mdp_debug_vsync){
   MDP_MSG_HIGH("lcd_line before porch adjustment %u ",lcd_line,0,0);
  }
  // lcd line adjusment referencing to the actual line counter at vsync
  lcd_line = (prim_lcd_lines - back_porch_line +lcd_line)%(mdp_ebi_rows_per_refresh+1);

  if(mdp_debug_vsync){
   MDP_MSG_HIGH("lcd_line after porch adjustment %u ",lcd_line,0,0);
   MDP_MSG_HIGH("mdp_ebi_rows_per_second is %u", mdp_ebi_rows_per_second,0,0);
   MDP_MSG_HIGH("lcd_line is %u", lcd_line,0,0);
  }

  if (lcd_line > prim_lcd_lines)
  {
    MSG_ERROR("mdp_lcd_rd_cnt >= prim_lcd_lines",0,0,0);
  }

  return lcd_line;  
}


#endif /*FEATURE_MDDI*/

#ifdef MDP_SW_VSYNC
/*===========================================================================

FUNCTION mdp_get_vsync_wait_time()

DESCRIPTION
  Modify the vsync script for the next update.  This should consist of 
  determining to values to start before/after.

DEPENDENCIES
  Directly access the global variable MDP_DISP_VSYNC_SCR.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_MDDI
#error code not present
#endif /**#ifdef FEATURE_MDDI**/

/********************S/w vsync logic for EBI-2 LCD panels on SC2x ********************/
#ifndef FEATURE_MDDI
static uint32 mdp_get_vsync_wait_time(MDP_CWIN cwin)
{
  //this variable controls how much tolerance is allowed
  int mdp_vsync_usec_wait_line_too_short = 5; 
  int32 usec_wait_time = 0;

  int32 mdp_lcd_rd_cnt;
  //start line to write
  int32 start_y=0;
 
  if(!mdp_ebi_rows_per_refresh || !mdp_dma2_update_time_in_usec)
      return 0;


   //estimate the lcd line read location
   mdp_lcd_rd_cnt = mdp_get_lcd_line_counter();

   if( !lcd_ref_hz)
       return 0; 

   //if we don't have an update time for hw yet, set default time
   if(!mdp_update_time[last_update_height])
   {  
      //if updating less than half of QVGA display, set update time to 
      //be real fast 
      if(last_update_width * last_update_height < (QVGA_HEIGHT*QVGA_WIDTH/2))
      { 
          mdp_update_time[last_update_height] = 1000;
      }
      else
      {
       //if updating more than half of QVGA display, set update time to 
       // be real slow (close to 16.7 ms)
       mdp_update_time[last_update_height] = 16000;
      }
      if(mdp_debug_vsync){
          MDP_MSG_HIGH("Updating update time for height %u  to be %u", last_update_height, mdp_update_time[last_update_height], 0);
      }
   }

   if(mdp_hw_start_timestamp_ms != 0 )
   {
    uint32 num,den;

    // roi width boundary calculation to know the size of pixel width that MDP can send
    // faster or slower than LCD read pointer        
    // num = (mdpLcdSize[DISPLAY_TYPE_1][COL_IDX]+1)*(mdpLcdSize[DISPLAY_TYPE_1][ROW_IDX]+1);
	// T1= time for LCD update
	// w1 = width of LCD
	// h1 = height of LCD including porch
	// T2 = time for MDP update
	// w2 = unknown (the width mdp can update for it to be the same speed as LCD read)
	// h2 = height of MDP update
	// T1/T2 = (w1*h1)/(w2*h2)
	// w2 = (T2*w1*h1) / (T1*h2)
	// num = (w1*h1)
	// den = ((T1*h2)*(1/T2))

    if(mdp_debug_vsync){ 
     MDP_MSG_HIGH("HW is taking this long start time %u,end time %u, difference %u", mdp_hw_start_timestamp_ms, mdp_hw_finish_timestamp_ms,  mdp_hw_finish_timestamp_ms - mdp_hw_start_timestamp_ms);
     MDP_MSG_HIGH("Time to update height %u is %u", last_update_height,   mdp_update_time[last_update_height], 0);
    }

    num = last_update_width*last_update_height; 
    if (mdp_update_time[last_update_height]) {
      den = (((lcd_ref_hz*mdp_ebi_rows_per_refresh)/1000)*(mdp_update_time[last_update_height]/100 ))/1000;        

      if(mdp_debug_vsync){
        MDP_MSG_HIGH("Rows per refresh mdp_ebi_rows_per_refresh %u", mdp_ebi_rows_per_refresh, 0, 0);
    
        MDP_MSG_HIGH("lcd_ref_hz is %u", lcd_ref_hz, 0, 0);
        MDP_MSG_HIGH("mdp_last_dma2_update_width is %u", last_update_width, 0, 0);
        MDP_MSG_HIGH("mdp_last_dma2_update_height is %u", last_update_height, 0, 0);
        MDP_MSG_HIGH("num/den is %u", num/den, 0, 0);
        MDP_MSG_HIGH("mdp_dma2_update_time_in_usec is %u", mdp_update_time[last_update_height], 0, 0);
      }
      if(den){
       //store the width we can update based on current clock settings
       mdp_width_boundary[last_update_width] = (int)(num/den);
      }
      else
      {
        MDP_MSG_ERROR("0 encountered in calculation ",0,0,0);
      }
    }
    else{
      MDP_MSG_ERROR("0 encountered in calculation",0,0,0);
    }

  }

  if (mdp_width_boundary[last_update_width] > last_update_width)
  {
    // MDP wrp is faster than LCD rdp
    mdp_lcd_rd_cnt += MDP_WRITE_FASTER_VSYNC_OFFSET ;//124;//140;//mdp_lcd_rd_cnt_offset_fast;       
    
    if(mdp_debug_vsync){
     MDP_MSG_HIGH("MDP IS FASTER",0,0,0);
    }
  }
  else
  {
    // MDP wrp is slower than LCD rdp
    mdp_lcd_rd_cnt -= MDP_WRITE_SLOWER_VSYNC_OFFSET; //;//mdp_lcd_rd_cnt_offset_slow;         
    if(mdp_debug_vsync){ 
     MDP_MSG_HIGH("MDP IS SLOWER",0,0,0);
    }
  }

  if (mdp_lcd_rd_cnt < 0)
    mdp_lcd_rd_cnt = mdp_ebi_rows_per_refresh + mdp_lcd_rd_cnt;
  else if (mdp_lcd_rd_cnt > mdp_ebi_rows_per_refresh)
    mdp_lcd_rd_cnt = mdp_lcd_rd_cnt - mdp_ebi_rows_per_refresh-1;
  if(mdp_debug_vsync){
   MDP_MSG_HIGH("mdp_lcd_rd_cnt is %u after all adjustments", mdp_lcd_rd_cnt, 0, 0);
  }
  // get wrt pointer position
  start_y = lcd_y;

  if(start_y <0)
  {
    start_y = 0;
  }
  if(mdp_debug_vsync){
   MDP_MSG_HIGH("start_y line is %u", start_y, 0, 0);
  }
  // measure line difference between start_y and rd counter
  if (start_y > mdp_lcd_rd_cnt)
  {
    // *100 for lcd_ref_hz was already mutiplied by 100 
    // *1000000 is for usec conversion

    if ((start_y - mdp_lcd_rd_cnt) <= mdp_vsync_usec_wait_line_too_short)
      usec_wait_time = 0;
    else
	{
		if(mdp_ebi_rows_per_refresh && lcd_ref_hz)
		{
			usec_wait_time = ((start_y - mdp_lcd_rd_cnt)*NUM_USEC_IN_MSEC)/((mdp_ebi_rows_per_refresh*lcd_ref_hz)/100);
		}
		else
		{
			MDP_MSG_ERROR("mdp_ebi_rows_per_refresh && lcd_ref_hz is ZERO",0,0,0);
		}
		
     if(mdp_debug_vsync){
      MDP_MSG_HIGH("Need to start writing after LCD Read counter",0,0,0);
     }
    }
  }
  else
  {//int32 calculation1=0;

    if ((start_y + (mdp_ebi_rows_per_refresh-mdp_lcd_rd_cnt)) <= mdp_vsync_usec_wait_line_too_short)
      usec_wait_time = 0;
    else
	{
  		if(mdp_ebi_rows_per_refresh && lcd_ref_hz)
		{
        usec_wait_time = (((mdp_ebi_rows_per_refresh-mdp_lcd_rd_cnt) + start_y)*NUM_USEC_IN_MSEC)
                          / ((mdp_ebi_rows_per_refresh*lcd_ref_hz)/100 );
		}
		else 
		{
			MDP_MSG_ERROR("mdp_ebi_rows_per_refresh && lcd_ref_hz is ZERO",0,0,0);
		}
        if(mdp_debug_vsync){
          MDP_MSG_HIGH("Need to start writing before lcd read counter",0,0,0);
        }
    }
  }  

  
   if(mdp_debug_vsync){
    MDP_MSG_HIGH("usec_wait_time is %u" , usec_wait_time, 0, 0);
  }
  return usec_wait_time;
}  // mdp_get_vsync_wait_time(void)


#endif /****#ifndef FEATURE_MDDI****/
#endif /*MDP_SW_VSYNC*/
#ifdef MDP_HW_VSYNC
/*===========================================================================

FUNCTION mdp_modify_vsync_script()

DESCRIPTION
  Modify the vsync script for the next update.  This should consist of 
  determining to values to start before/after.

DEPENDENCIES
  Directly access the global variable MDP_DISP_VSYNC_SCR.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void mdp_modify_vsync_script(MDP_CWIN cwin)
{
  int upper_boundary, lower_boundary, vsync_cnt;
  boolean outside_threshold;

  upper_boundary = cwin.y1;

  // upper_boundary needs to be at least PLH-1 
  // in order to satisfy WAIT_LINE_GTE(upper_boundary)
  if (upper_boundary < (_primary_lcd.disp_height - 1))
  {
    lower_boundary = upper_boundary - _primary_lcd.disp_height + 2;
    lower_boundary = (lower_boundary >= 0)? 
                     lower_boundary:_primary_lcd.disp_height + lower_boundary;

    vsync_cnt = HWIO_MDP_SYNC_STATUS_INM(HWIO_MDP_SYNC_STATUS_LINE_COUNT_BMSK);

    if (lower_boundary <= upper_boundary)
    {
      outside_threshold = 
      ((vsync_cnt > upper_boundary) || (vsync_cnt < lower_boundary));
    }
    else
    {
      outside_threshold = 
      ((vsync_cnt > upper_boundary) && (vsync_cnt < lower_boundary));
    }

    if (outside_threshold)
    {
      MDP_DISP_VSYNC_SCR[2] = NOP;      
      MDP_DISP_VSYNC_SCR[3] = NOP;                            
    }
    else
    {

      if ((cwin.x2-cwin.x1+1) >= (_primary_lcd.disp_height/2))
      {
        if (lower_boundary <= upper_boundary)
        {
          MDP_DISP_VSYNC_SCR[2] = WAIT_LINE_GTE(upper_boundary);
          MDP_DISP_VSYNC_SCR[3] = NOP;                              
        }
        else
        {
          MDP_DISP_VSYNC_SCR[2] = WAIT_LINE_LT(upper_boundary+1);
          MDP_DISP_VSYNC_SCR[3] = WAIT_LINE_GTE(upper_boundary+1);            
        }          
      }
      else
      {
        MDP_DISP_VSYNC_SCR[2] = WAIT_LINE_LT(upper_boundary+1);
        MDP_DISP_VSYNC_SCR[3] = NOP;
      }
    }
  }  // if (upper_boundary < (_primary_lcd.disp_height - 1))
}  // mdp_modify_vsync_script(void)
#endif
/*===========================================================================

FUNCTION mdp_set_ccs()

DESCRIPTION
  This function is to set color conversion coefficents for MDP to use

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdp_set_ccs(MDP_CCS_TYPE *ccs, boolean in_reset)
{
  uint32 cnt = MDP_SET_CCS_WAIT_CNT;

  if (ccs == NULL)
    return;

  rex_enter_crit_sect(&mdp_crit_sect);

  // wait until MDP is not busy
  if (in_reset)
  {
    while ((cnt-- > 0) && (inpdw(MDP_SCRIPT_STATUS)&0x1));
  }
  else
  {
    while (inpdw(MDP_SCRIPT_STATUS)&0x1);    
  }


  MDP_DISP_CCS_SCR[0] = SET_CCS_CCTF(0, ccs->ccs1);
  MDP_DISP_CCS_SCR[1] = SET_CCS_CCTF(1, ccs->ccs2);
  MDP_DISP_CCS_SCR[2] = SET_CCS_CCTF(2, ccs->ccs3);
  MDP_DISP_CCS_SCR[3] = SET_CCS_CCTF(3, ccs->ccs4);
  MDP_DISP_CCS_SCR[4] = SET_CCS_CCTF(4, ccs->ccs5);
  MDP_DISP_CCS_SCR[5] = SET_CCS_CCTF(5, ccs->ccs6);
  MDP_DISP_CCS_SCR[6] = SET_CCS_CCTF(6, ccs->ccs7);          
  MDP_DISP_CCS_SCR[7] = SET_CCS_CCTF(7, ccs->ccs8);
  MDP_DISP_CCS_SCR[8] = SET_CCS_CCTF(8, ccs->ccs9);
  MDP_DISP_CCS_SCR[9] = SET_CCS_CCTF(9, ccs->ccs10);
  MDP_DISP_CCS_SCR[10] = SET_CCS_CCTF(10, ccs->ccs11);   
  MDP_DISP_CCS_SCR[11] = SET_CCS_CCTF(11, ccs->ccs12);               

  outpdw(MDP_SCRIPT_ADDR, MDP_DISP_CCS_SCR);
  outpdw(MDP_COMMAND, BEGIN_SCRIPT);    

  // wait until MDP is not busy
  cnt = MDP_SET_CCS_WAIT_CNT;
  if (in_reset)
  {
    while ((cnt-- > 0) && (inpdw(MDP_SCRIPT_STATUS)&0x1));
  }
  else
  {
    while (inpdw(MDP_SCRIPT_STATUS)&0x1);    
  }

  mdp_curr_ccs = ccs;

  rex_leave_crit_sect(&mdp_crit_sect);  
}

/*===========================================================================

FUNCTION mdp_set_sys_bmp()

DESCRIPTION
  This function is to set the system bitmap pointer

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  Only registered area of this bitmap will be displayed

===========================================================================*/
void mdp_set_sys_bmp(uint32 *bmp)
{
  if (bmp == NULL)
  {
    mdp_sys_bmp = NULL;    
  }
  else
  {
    mdp_sys_bmp = (uint32 *)((uint32)bmp - MDP_SYS_BMP_OFFSET);
  }
}

/*===========================================================================

FUNCTION mdp_ioctl()

DESCRIPTION
  This function is to handle all MDP IO Control

DEPENDENCIES
  mdp_install() installs all driver entry point functions

RETURN VALUE
  returns 1 if successful otherwise returns -1

SIDE EFFECTS
  None

===========================================================================*/

int mdp_ioctl(int cmd, void *arg)
{
  uint32 cast;
  LCD_TYPE dest;
  MDPIMG fgImg, fgImg_base;
  uint32 fgSubFlag;
  MDPIMG orig_fgImg;
  MDPSCALE_INFO orig_scale;
  uint32 orig_fgMbcOp;
  uint32 alphaMask;
  uint32 vsync_flag;
  MDPIMG_LIST * fgListHead, *next_fgList, *node;
  MDP_OVER_XYInt this_cwin;
  boolean this_layer=TRUE;

#ifdef FEATURE_HS_DISPLAY_CAPTURE
  uint16 *dest_address;
  uint16 *sourceStartAddress;
  uint32 width, height;
#endif /* FEATURE_HS_DISPLAY_CAPTURE */

#ifdef FEATURE_MDDI
#error code not present
#endif /* FEATURE_MDDI */

  if (mdp_drv_initialized != TRUE)
    return -1;

  switch (cmd)
  {
    case IOCTL_DISP_UPDATE:
      fgImg.dest = ((disp_update_type *)arg)->dest;
      fgImg.imgType = MDP_RGB565;
      fgImg.bmy_addr = (uint32 *)((disp_update_type *)arg)->buf_ptr;
      fgImg.width = ((disp_update_type *)arg)->src_width;
      fgImg.cwin.x1 = ((disp_update_type *)arg)->src_starting_column;
      fgImg.cwin.x2 = ((disp_update_type *)arg)->src_starting_column
                      +((disp_update_type *)arg)->num_of_columns-1;
      fgImg.cwin.y1 = ((disp_update_type *)arg)->src_starting_row;
      fgImg.cwin.y2 = ((disp_update_type *)arg)->src_starting_row
                      +((disp_update_type *)arg)->num_of_rows-1;
      fgImg.lcd_x = ((disp_update_type *)arg)->dst_starting_column
                    - MDP_SYS_OFFSET_X;
      fgImg.lcd_y = ((disp_update_type *)arg)->dst_starting_row
                    - MDP_SYS_OFFSET_Y;
      fgImg.mdpOp = MDPOP_BREW;
      fgImg.layer = MDP_LAYER_2;
      fgImg_base = fgImg;


      if (mdp_log_enable)
      {
        MSG_HIGH("mdp_ioctl() Opcode = 0x%x Layer = 0x%x Addr = 0x%x  ",
                 fgImg.mdpOp,fgImg.layer,fgImg.bmy_addr);
      }

      if ((uint32)(fgListHead = (MDPIMG_LIST *) mdp_cal_ovimg_rec(&fgImg,
                                                                  TRUE,
                                                                  &fgSubFlag,
                                                                  &orig_fgImg,
                                                                  &orig_scale,
                                                                  &orig_fgMbcOp,
                                                                  &alphaMask,
                                                                  &vsync_flag,
                                                                  &this_cwin,
                                                                  this_layer)) != MDP_FAIL)
      {
        next_fgList = fgListHead;
        while (next_fgList != NULL)
        {
          node = next_fgList;

          // need to save LCD(x,y) location before the adjustment
          fgImg.lcd_x = node->img.cwin.x1 - MDP_SYS_OFFSET_X;
          fgImg.lcd_y = node->img.cwin.y1 - MDP_SYS_OFFSET_Y;

          mdp_adjust_XY(&fgImg_base,
                        &node->img.cwin.x1,
                        &node->img.cwin.x2,
                        &node->img.cwin.y1,
                        &node->img.cwin.y2,
                        &orig_scale);

          fgImg.cwin = node->img.cwin;
          // IBitmap must be registered in IDisplay
          if (node->bgPtr != NULL)
          {
            fgImg.mdpOp = node->bgPtr->orig_img.mdpOp|MDPOP_BREW;
            fgImg.tpVal = node->bgPtr->orig_img.tpVal;
            fgImg.alpha = node->bgPtr->orig_img.alpha;
            mdp_disp_update(&fgImg);
          }
          // if it's not registered and it doesn't belong to the system bitmap
          else if (mdp_sys_bmp != orig_fgImg.bmy_addr)
          {
            fgImg.mdpOp = MDPOP_BREW;
            mdp_disp_update(&fgImg);
          }

          next_fgList = next_fgList->next;
        }
        mdp_imglist_purge(fgListHead);
      }
      else
      {
        MSG_ERROR("mdp_ioctl() Opcode = 0x%x Layer = 0x%x Addr = 0x%x  ",
                  fgImg.mdpOp,fgImg.layer,fgImg.bmy_addr);
        MSG_ERROR("**** failed in mdp_cal_ovimg_rec() from mdp_ioctl() ****",0,0,0);
      }

#ifdef FEATURE_MDDI
#error code not present
#endif /* FEATURE_MDDI */
      break;

    case IOCTL_DISP_CLEAR_SCREEN_AREA:
      fgImg.dest = ((disp_cls_type *)arg)->dest;
      fgImg.imgType = MDP_RGB_565;
      fgImg.bmy_addr = (uint32 *)(white_frame->buffer);
      fgImg.width = white_frame->pxWidth;
      fgImg.cwin.x1 = ((disp_cls_type *)arg)->start_column;
      fgImg.cwin.x2 = ((disp_cls_type *)arg)->end_column-1;
      fgImg.cwin.y1 = ((disp_cls_type *)arg)->start_row;
      fgImg.cwin.y2 = ((disp_cls_type *)arg)->end_row-1;
      fgImg.lcd_x = fgImg.cwin.x1 - MDP_SYS_OFFSET_X;
      fgImg.lcd_y = fgImg.cwin.y1 - MDP_SYS_OFFSET_Y;
      fgImg.mdpOp = MDPOP_BREW;
      fgImg.layer = MDP_LAYER_2;
      mdp_disp_update(&fgImg);

      break;

    case IOCTL_DISP_CLEAR_WHOLE_SCREEN:
      cast = (uint32)arg;
      dest = (LCD_TYPE)cast;
      fgImg.dest = dest;
      fgImg.imgType = MDP_RGB_565;
      fgImg.bmy_addr = (uint32 *)(white_frame->buffer);
      fgImg.width = white_frame->pxWidth;
      fgImg.cwin.x1 = 0;
      fgImg.cwin.x2 = _primary_lcd.disp_width - 1;
      fgImg.cwin.y1 = 0;
      fgImg.cwin.y2 = _primary_lcd.disp_height - 1;
      fgImg.lcd_x = 0 - MDP_SYS_OFFSET_X;
      fgImg.lcd_y = 0 - MDP_SYS_OFFSET_Y;
      fgImg.mdpOp = MDPOP_BREW;
      fgImg.layer = MDP_LAYER_2;
      mdp_disp_update(&fgImg);

      break;

    case IOCTL_DISP_ON:
      cast = (uint32)arg;
      dest = (LCD_TYPE)cast;    
      mdp_dest_power_on[dest] = TRUE;
      break;

    case IOCTL_DISP_OFF:
      cast = (uint32)arg;
      dest = (LCD_TYPE)cast;    
      mdp_dest_power_on[dest] = FALSE;    
      break;

#ifdef FEATURE_HS_DISPLAY_CAPTURE
    case IOCTL_DISP_ENABLE_CAPTURE_MODE:

      //enable or disable capture mode based on flag
      cast = (uint32)arg;

      if (cast)
        mdp_enable_capture_mode(TRUE);
      else
        mdp_enable_capture_mode(FALSE); 

      break;

    case IOCTL_DISP_CAPTURE_SCREEN:

      //capture current screen content
      //MSG_ERROR("****STARTING TO COPY****",0,0,0);

      //if capture mode is disabled - enable (first time access)
      if (!mdpCaptureMode)
        mdp_enable_capture_mode(TRUE);

      //make sure that buffer status is valid (frame has been captured)- otherwise return fail
      //note: do not wait for buffer to be valid, if screen is idle buffer will not be validated
      if (!mdpCaptureScreenBufferValid)
      {
        MSG_ERROR("****Failed in IOCTL_DISP_CAPTURE_SCREEN - Buffer Not ready****",0,0,0);
        return -1;
      }

      //ensure we are not currently writing to the buffer or using MDP
      rex_enter_crit_sect(&mdp_crit_sect);

      //MSG_ERROR("****STARTING TO COPY- Post Critical Section****",0,0,0);

      //get arguments passed to function call
      width = ((disp_update_type *)arg)->num_of_columns;
      height = ((disp_update_type *)arg)->num_of_rows;

      dest_address = (uint16 *)((disp_update_type *)arg)->buf_ptr;

      //check parameters and fail if we have wrong parameters
      if (dest_address == NULL )
      {
        //return failure
        MSG_ERROR("****Failed in IOCTL_DISP_CAPTURE_SCREEN - Invalid Parameters ****",0,0,0);
        rex_leave_crit_sect(&mdp_crit_sect);
        return -1;
      }

      //set the source starting address - for partial buffer capture
      //Todo - ensure that we are not accessing invalid source address - beyond array size
      sourceStartAddress = (uint16 *)mdpScreenCaptureBuf->buffer + (((disp_update_type *)arg)->src_starting_column) 
                           + (mdpScreenCaptureBuf->pxWidth * ((disp_update_type *)arg)->src_starting_row) ;

      //use MDP to copy captured LCD content to output bufferr
      //note - application responsible for incrmenting returned buffer pointer
     
      memcpy((char *)(((disp_update_type *)arg)->buf_ptr),(char *)sourceStartAddress, width*height*2);        
      
    
	/*	RK: It's not rquired on any of the targets. Will be deleted. 
	mmu_invalidate_data_cache_lines((uint32 *)(((disp_update_type *)arg)->buf_ptr), (uint32)width*height*2);       
           
	*/
      rex_leave_crit_sect(&mdp_crit_sect);

      //MSG_ERROR("****COPY FINISHED****",0,0,0);

      //return result of operation
      return 1;
#endif /*FEATURE_HS_DISPLAY_CAPTURE*/

    default:  return -1;
  }

  return 1;
}

/*===========================================================================

FUNCTION mdp_install()

DESCRIPTION
  This function is to install MDP driver

DEPENDENCIES
  None

RETURN VALUE
  returned value from drv_install()

SIDE EFFECTS
  None

===========================================================================*/
int mdp_install(char * str)
{

  return drv_install(str,
                     NULL,
                     NULL,
                     mdp_ioctl,
                     mdp_drv_init,
                     NULL);
}

/*===========================================================================

FUNCTION mdp_verify_dest()

DESCRIPTION
  This function is to verify for the valid physical destination

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  Edge

SIDE EFFECTS
  None
===========================================================================*/
boolean mdp_verify_dest(LCD_TYPE dest)
{
  if ((dest == PRIMARY_LCD_TYPE) && (lcdPhysDest[PRIMARY_LCD_TYPE] != LCD_NONE))
    return TRUE;

  if ((dest == SECONDARY_LCD_TYPE) && (lcdPhysDest[SECONDARY_LCD_TYPE] != LCD_NONE))
    return TRUE;

  return FALSE;
}

#ifdef FEATURE_MDDI
#error code not present
#endif /* FEATURE_MDDI */


/*===========================================================================

FUNCTION mdp_is_dest_mddi()

DESCRIPTION
  This function is to determine whether or not it's MDDI destination 

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  True if it's destined to MDDI, otherwise FALSE

SIDE EFFECTS
  None
===========================================================================*/

int mdp_is_dest_mddi(LCD_TYPE dest)
{
  int retVal = FALSE;

#ifdef FEATURE_MDDI
#error code not present
#endif /* FEATURE_MDDI */

  return retVal;
}

  #ifdef FEATURE_HS_DISPLAY_CAPTURE
/*===========================================================================

FUNCTION mdp_enable_capture_mode()

DESCRIPTION
  This function is to set the screen capture mode on or off

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  Capture mode will slow system down due to copying LCD content to buffer

===========================================================================*/
void mdp_enable_capture_mode(boolean flag)
{

  //set display capture mode
  mdpCaptureMode = flag;

  //set buffer to not valid
  if (!mdpCaptureMode)
  {
    mdpCaptureScreenBufferValid = FALSE;
  }
}

/*===========================================================================

FUNCTION mdp_capture_display()

DESCRIPTION
  This function is used to 

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  Capture mode will slow system down due to copying LCD content to buffer

===========================================================================*/

uint32 mdp_capture_display(MDPIMG *Image)
{
  uint32 result = MDP_FAIL;


  //copy current MDP update to screen buffer - support only primary LCD
  if (Image->dest == PRIMARY_LCD_TYPE)
  {
    rex_enter_crit_sect(&mdp_crit_sect);    
    result = mdp_disp_update_sub(Image, TRUE, (uint16*)mdpScreenCaptureBuf->buffer,
                                 mdpScreenCaptureBuf->byteSize, NULL);
    rex_leave_crit_sect(&mdp_crit_sect);    

    //validate buffer if operation is completed
    if (result)
    {
      mdpCaptureScreenBufferValid = TRUE;
    }
  }

  return result;
}
  #endif /*FEATURE_HS_DISPLAY_CAPTURE*/

#ifdef FEATURE_CLKREGIM_DEVMAN
/*===========================================================================

FUNCTION mdp_reconfig_vsync()

DESCRIPTION
  This function is to configure vsync counter during dynamic hclk change,
  and can run in any task contect or ISR. Hence, it is a stripped down
  version of mdp_config_vsync()

DEPENDENCIES
  Requires mdp_config_vsync() to be run first and mdp_drv_initialized = TRUE

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void mdp_reconfig_vsync(uint32 hclk)
{
  uint16 SST_TERM_DIV_CNT = 0; /* Time taken by LCD module to write a row  
                               ** of pixels in terms if AHB clock cycles */

  if((hclk == 0) || (mdp_drv_initialized == FALSE)) 
    return;

#ifdef FEATURE_MDDI
#error code not present
#endif /* FEATURE_MDDI */

#ifndef MDP_SYS_VSYNC_OFF
#ifdef MDP_SW_VSYNC

#ifdef FEATURE_MDDI
#error code not present
#else /* FEATURE_MDDI */
  if(!mdp_mddi_enable)
  {
	  SST_TERM_DIV_CNT = hclk / (LCD_REFRESH_EBI * LCD_DISPLAY_LINES);
  }
#endif
  outpdw(MDP_SYNC_CONFIG, (SW_VSYNC | SST_TERM_DIV_CNT));

#elif defined (MDP_HW_VSYNC)

#ifdef FEATURE_MDDI
#error code not present
#endif /* FEATURE_MDDI */
    SST_TERM_DIV_CNT = hclk / (LCD_REFRESH_EBI * LCD_DISPLAY_LINES);

  outpdw(MDP_SYNC_CONFIG, (PRIM_VSYNC | SST_TERM_DIV_CNT) );
#endif /* MDP_SW_VSYNC */

#endif /* MDP_SYS_VSYNC_OFF */

}

/*===========================================================================

FUNCTION mdp_clkrgm_cb()

DESCRIPTION
  This is a clock manager initiated callback function to flag for vsync 
  recalculation. Occurs in the context of the task that requested hclk change

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
static void mdp_clkrgm_cb(clkrgm_device_type device, 
                          clkrgm_device_speed_change_type position,
                          uint32 old_speed_khz, uint32 new_speed_khz)
{
  if(position == CLKRGM_DEVICE_POST_SPEED_CHANGE)
  {
    if((device == CLKRGM_DEVICE_HBUS) && (old_speed_khz != new_speed_khz))
    {
      mdp_reconfig_vsync(new_speed_khz * 1000);  
    }
  }
}
#endif /* FEATURE_CLKREGIM_DEVMAN */


/*===========================================================================

FUNCTION mdp_init_system_frames()

DESCRIPTION
  This function is responsible for allocating ALL the screen buffers needed
  by the MDP driver.  Buffers are global pointers to heap blocks.
  
  This function is to be called from mdp_drv_init()

DEPENDENCIES
  1. The following globals should be declared
  - mdp_slBuf_1, mpd_slBuf_2
  - mdp_L1buf_1, mdp_L1buf_2
  - white_frame
  - mdpTempImgBuf
  - mdpScreenCaptureBuf
 
  2. The following globals should have already been set:
    - _primary_lcd
    - _secondary_lcd
    - _biggest_lcd

RETURN VALUE
  boolean => TRUE if successful, FALSE otherwise

SIDE EFFECTS
  if fails no screen buffers are created and global pointers are NULL.

===========================================================================*/
boolean mdp_init_system_frames(void)
{
  static boolean inited = FALSE;

  // Heap buffers should only be allocated once or memory leak will occur.
  if (inited)
  {
    return TRUE;
  }

  /*---------------------------------
  ** COMMON Buffers
  **-------------------------------*/

  /* White Buffer */
  if((white_frame = mdp_create_largest_frame(DISP_16BPP)) == NULL)
    goto MDP_CREATE_BUFFER_FAILURE;

  /* Initialize buffer using white pixels */
  memset(white_frame->buffer, MDP_WB_CLR, SCR_BYTE_SIZE(white_frame->pxWidth, white_frame->pxHeight, 16));
  mdpTempImgBuf = white_frame;
  /* Temporary Buffer */
  #if !defined(FEATURE_VERSION_K212_12832)
  if((mdpTempImgBuf = mdp_create_largest_frame(DISP_16BPP)) == NULL)
    goto MDP_CREATE_BUFFER_FAILURE;
  #endif

  /* SECURITY MEASURE: All unused buffers should have their associated global pointers
  ** point to the white buffer.  This is to ensure that any 'rogue' code will not write
  ** to unknown memory location.  As a result, we initialize all primary/secondary
  ** display buffer pointers before we allocate those we need */

//  mdpScreenCaptureBuf = white_frame;
  mdp_slBuf_1 = mdp_L1buf_1 = white_frame;
  mdp_slBuf_2 = mdp_L1buf_2 = white_frame;

  /* Capture Buffer */
#ifdef FEATURE_HS_DISPLAY_CAPTURE
  mdpScreenCaptureBuf = white_frame;
  if((mdpScreenCaptureBuf = mdp_create_largest_frame( DISP_16BPP )) == NULL)
    goto MDP_CREATE_BUFFER_FAILURE;
#endif

  /*---------------------------------
  ** PRIMARY Display
  **-------------------------------*/

  /*  Sub-layer buffer */
#if !defined(FEATURE_VERSION_K212_12832)
  if((mdp_slBuf_1 = mdp_create_primary_frame(DISP_16BPP)) == NULL )
    goto MDP_CREATE_BUFFER_FAILURE;
#endif
  /* Layer-1 buffer	*/
#ifdef FEATURE_MDP_LAYER1_PRIMARY
#if !defined(FEATURE_VERSION_K212_12832)
  if((mdp_L1buf_1 = mdp_create_primary_frame(DISP_16BPP)) == NULL )
    goto MDP_CREATE_BUFFER_FAILURE;    
#endif
#endif

  /*---------------------------------
  ** SECONDARY Display 
  **-------------------------------*/

#ifdef FEATURE_MDP_SECONDARY_DISPLAY
#error code not present
#endif /* FEATURE_MDP_SECONDARY_DISPLAY */

  inited = TRUE;

  /* All buffers allocated successfully */
  return TRUE;

  /* FAILURE HANDLER: Free aLL allocated buffers before return */
  MDP_CREATE_BUFFER_FAILURE:    
  
  if(mdp_slBuf_1 && (mdp_slBuf_1 != white_frame)) 
    mdp_free_frame(mdp_slBuf_1); 

#ifdef FEATURE_MDP_LAYER1_PRIMARY
  if(mdp_L1buf_1 && (mdp_L1buf_1 != white_frame)) 
    mdp_free_frame(mdp_L1buf_1); 
#endif /* FEATURE_MDP_LAYER1_PRIMARY */

#ifdef FEATURE_MDP_SECONDARY_DISPLAY
#error code not present
#endif /* FEATURE_MDP_SECONDARY_DISPLAY */

#ifdef FEATURE_HS_DISPLAY_CAPTURE
  if(mdpScreenCaptureBuf && (mdpScreenCaptureBuf != white_frame)) 
    mdp_free_frame(mdpScreenCaptureBuf); 
#endif /* FEATURE_HS_DISPLAY_CAPTURE */

  if(mdpTempImgBuf) 
    mdp_free_frame(mdpTempImgBuf);

  if(white_frame)
    mdp_free_frame(white_frame); 

  // reset buffer pointers
  mdp_slBuf_1 = mdp_L1buf_1 = NULL;
  mdp_slBuf_2 = mdp_L1buf_2 = NULL;
  white_frame = NULL;
  mdpTempImgBuf = NULL;
  #ifdef FEATURE_HS_DISPLAY_CAPTURE
  mdpScreenCaptureBuf = NULL;
  #endif
  /* All buffers freed, return failure status */
  return FALSE;	
}

#endif /* FEATURE_MDP */
