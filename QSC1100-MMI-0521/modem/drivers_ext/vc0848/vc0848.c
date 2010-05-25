/*===========================================================================

                   VC    M A N A G E R  T A S K
DESCRIPTION
  All the declarations and definitions necessary to support the table  
  lookup of mapping between an NV item and the Flash File System files
  which simulate storage in an EEPROM.  External tasks queue requests for
  this task for VC Task operations.  This tasks then 
  performs the necessary internal operations to read, write
  and control access to the VC0848 device.   
  This task includes the following files:
    vc0848.h 
    vc0848.c    

EXTERNALIZED FUNCTIONS
    vc_cmd
      This is the applications read and write interface to this task.
    vc_task
      This is the vc task initialization entry, started up with rex_def_task.
    
INITIALIZATION AND SEQUENCING REQUIREMENTS
  This task should be started up with rex_def_task.  Caution must be
  taken when invoking error services from this task.  These services invoke
  this task again (to log errors) and could cause an infinite loop.
  
REFERENCES

Copyright (c)  2008-2010 by Anylook Com. .  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

   
when             who      what, where, why
----------   ----     ---------------------------------------------------------
12/12/2008   laur      Created vc0848.c
===========================================================================*/
 
/* ======================================================================

                       INCLUDE FILES FOR MODULE

========================================================================*/


#include "target.h"
#include "comdef.h"

#include "rex.h"
#include "queue.h"
#include "qw.h"
#include "vc0848.h"

#include "fs.h"
#include "msg.h"
#include "err.h"
#include "task.h"
#include "fs.h"
#include "dog.h"
#include "mc.h"
#include "crc.h"
#include "memory.h"

#include "hs_mb6100.h"

#include "mmd_sys_driver.h"
#include "mmd_user_depend.h"
#include "vim_sys.h"
#include "vim_common_debug.h"
#include "mmd_file_driver.h"

#include "aeesound.h"
#include "aeeshell.h"
#include "aeeclassids.h"
#include "aee_oem.h"
#include "aeeerror.h"
#include "aee.h"
#include "vc08xxhrdk\apps\media\media_player.h"
#include "aeestdlib.h"

#include "oemui.h"
#include "aeebacklight.h"
#include "db.h"

#include "vc08xxhrdk\drivers\fat\fat_thin.h"
#include "mmd_ply_driver.h"

#include "OEMMedia.h"
#ifdef FEATURE_SUPPORT_BT_APP
#include "MusicPlayer.h"
#endif
#include "OEMCameraVC0848.h"

#ifdef T_SLEEP
#include "sleep.h"      /* Sleep task signaling        */
#endif
#include "aeecamera.h"
#include "adie_regdefs.h"
#include "adie.h"
#include "codec.h"
#include "davincicode.h"
#include "OEMCFGI.h"

#define MIN_FILE_NAME_LEN   64
#define DEDAULT_CAPTURE_SIZE_WIDTH  176  //640	//modify by wang jian 2009.4.15
#define DEDAULT_CAPTURE_SIZE_HEIGHT 220  //480	//modify by wang jian 2009.4.15

#define UTF8_B0            0xEF
#define UTF8_B1            0xBB
#define UTF8_B2            0xBF

#define VC_DEV_STACK_MAX   7

#ifdef T_SLEEP
#define VC_SLEEP_ALLOW() \
  { (void) rex_set_sigs( &sleep_tcb, SLEEP_VC_OKTS_SIG ); }
#else
#define VC_SLEEP_ALLOW()
#endif

#ifdef T_SLEEP
#define VC_SLEEP_FORBID() \
  { (void) rex_clr_sigs( &sleep_tcb, SLEEP_VC_OKTS_SIG ); }
#else
#define VC_SLEEP_FORBID()
#endif

#define VIDEO_LCD_HEIGHT 144;  //add jian.wang  2009.1.9  实际视频高度
/* ======================================================================= 
  The status while vc_init finished or not, 
  note:
    1. if no sd, forbid any operation on SD.
    2. if no sensor, forbid any camera operation and return NO_ENSOR while camera_new
    
  even there's no sd or no sensor, it does no effect on:
    1. FM PA func, 
    2. audio record func, 
    3. BT MP3 play func
    4. Wallpaper Zoom func with VC0848 hardware
======================================================================= */  
typedef enum{
  VC_INIT_DONE_S,
  VC_INIT_BYPASS_ONLY,  
  VC_INIT_NO_SD_S,
  VC_INIT_NO_SENSOR_S
} vc_init_status_type;

typedef enum{
  VC_DEV_STAT_CLOSED,
  VC_DEV_STAT_OPENED
} vc_dev_state;

typedef enum{
  VC_PREVIEW_OFF,
  VC_PREVIEW_ON
} vc_preview_state;

typedef enum {
  VC_FILE_OPEN_NONE, 
  VC_FILE_OPEN_REC,
  VC_FILE_OPEN_PLY,
  VC_FILE_OPEN_MAX
} vc_file_open_type;

typedef enum {
   MMD_FILE_ATTR_NORMAL   = 0x00,  //normal
   MMD_FILE_ATTR_READONLY = 0x01,  //read only
   MMD_FILE_ATTR_HIDDEN   = 0x02,  //hidden
   MMD_FILE_ATTR_SYSTEM   = 0x04, //system
   MMD_FILE_ATTR_VOLUME   = 0x08, //VOLUME
   MMD_FILE_ATTR_DIR      = 0x10,   //directory
   MMD_FILE_ATTR_ARC      = 0x20  //archive
} vc_file_attr;

typedef struct {
   SCENE_IDX  scene; 
   CODEC_IDX  codec;
} vc_mode_codec_type;

/* The command on the VC queue, this is used for parm interchanging  */
typedef struct {
  q_link_type           link;              /* Queue field */
  vc_items_type         item;            /* Item to access */
  vc_func_type          cmd;             /* READ, WRITE, PEEK, POKE, etc. */
  vc_union_type         *data_ptr;    /* Pointer to read or write data */
  rex_tcb_type          *tcb_ptr;       /* Task to signal */
  rex_sigs_type         sigs;           /* Rex signals for task */
  q_type                *done_q_ptr;    /* Where to place done buffer */
  vc_status_type        status;         /* Status of request */
} vc_cmd_type;



rex_timer_type         vc_rpt_timer;         /* VC watchdog report timer. */
rex_timer_type         vc_user_timer;        /* VC user timer. */
rex_timer_type         vc_pmu_switch_timer;  /* VC user timer. */
LOCAL q_type           vc_cmd_q;             /* VC task command queue. */

TimerISR               g_VC0848TimerISR = NULL;
extern dword           g_TimerInterval;
//IBacklight             *gpVCBacklight = NULL;

LOCAL uint8            g_preview_stat = VC_PREVIEW_OFF;  /* save the current preview state for backlight manage */
LOCAL uint8            g_init_stat   = VC_INIT_DONE_S;      /* init result */
LOCAL MMD_PMU_MODE     g_dev_mode    = PMU_MODE_BYPASS;   /* device working mode, normal, bypass or sleep mode */
LOCAL vc_fspos_type    g_fs_pos = VC_FSPOS_SD;    /* fs_pos select on native or sd */
LOCAL vc_play_mode     g_play_mode = VC_NONE_M; /* current play mode: play, pause, resume or stop */
LOCAL vc_file_open_type g_file_open = VC_FILE_OPEN_NONE; /* save current file opon type */ 

LOCAL vc_dev_info      g_dev_info = {0}; 
LOCAL vc_dev_info      vc_dev_stack[VC_DEV_STACK_MAX] = {0};
LOCAL int              vc_dev_stack_idx = 0;
// Camera mode.
MMD_PLYFILE       pFileHandle  = NULL;
   
extern void  vc_init(void);
LOCAL void  vc_cmd(vc_cmd_type  *cmd_ptr);
LOCAL void vc_task_start(rex_sigs_type rpt_sig, dog_report_type  dog_rpt, rex_timer_type *rpt_timer_ptr);
LOCAL vc_status_type vc_control_device(vc_union_type  *union_ptr, vc_items_type item);
LOCAL vc_status_type vc_set_parm(vc_union_type  *union_ptr, vc_items_type item);
LOCAL vc_status_type vc_get_parm(vc_cmd_type  *cmd_ptr);
LOCAL vc_status_type vc_preview_on(vc_cmd_type  *cmd_ptr);
LOCAL vc_status_type vc_camera_on(vc_union_type  *union_ptr, vc_items_type item);
LOCAL vc_status_type vc_play_on(vc_union_type  *union_ptr, vc_items_type item);
LOCAL vc_status_type vc_fs_on(vc_cmd_type  *cmd_ptr);
LOCAL vc_status_type vc_open_dev(vc_union_type * union_ptr);
LOCAL vc_status_type vc_stop_dev(vc_union_type * union_ptr);
LOCAL vc_status_type vc_disp_on(vc_cmd_type  *cmd_ptr);

LOCAL void vc_set_audiobias(void);
LOCAL void vc_file_close(void);
LOCAL void vc_set_curr_file_type(vc_file_open_type nOpenType);
LOCAL vc_file_open_type vc_get_curr_file_type(void);

LOCAL MMD_PMU_MODE vc_get_dev_pmu_mode(void);
LOCAL void vc_set_dev_pmu_mode(MMD_PMU_MODE mode);

LOCAL vc_dev_pri_level vc_get_dev_pri(void);
LOCAL vc_dev_pri_level vc_dev_convert_pri(vc_dev_type dev_type);

LOCAL boolean vc_dev_push_stack(vc_dev_info * curr_dev_info, vc_dev_info * next_dev);
LOCAL boolean vc_dev_pop_stack(vc_dev_info * dev_info);
LOCAL void vc_set_device_state(vc_status_type new_stat);
LOCAL boolean vc_dev_validate_for_push_stack(vc_dev_type dev_type);

LOCAL char vc_attr2efs(uint8 attr);

LOCAL boolean vc_get_date2filename(char * pszDest, int nSize, unsigned int fileType);
LOCAL boolean UnicodeToUtf8(const uint16 * pSrc,int nLen, byte * pDst, int nSize);
LOCAL boolean Utf8ToUnicode(const byte * pSrc,int nLen, uint16 * pDst, int nSize);


LOCAL vc_dev_type vc_dev_scene_switch_on(vc_dev_type dev_type);
LOCAL boolean vc_dev_scene_switch_off(vc_dev_type dev_type);
LOCAL void vc_set_media_volume(uint8 vol);
LOCAL int  vc_get_media_volume(void);

LOCAL void vc_pmu_switch_bypass(void);
LOCAL void vc_set_pmu_flag_run_first_time(boolean flg); 
LOCAL boolean vc_get_pmu_running_flag(void); 
LOCAL void vc_set_camera_init_parm(vc_prev_parm_type parm);
LOCAL boolean vc_get_pmu_switch_bypass_ready(void);
LOCAL void vc_switch_mode_codec(vc_mode_codec_type modcodec);
LOCAL void vc_play_jpeg(unsigned int command);
LOCAL vc_timer_state vc_get_timer_state(void);
LOCAL uint32 vc_datetime_to_second(uint32 nDate, uint32 nTime);
LOCAL void vc_video_recorder_cb(unsigned int command, unsigned int param1, unsigned int param2);

LOCAL void vc_disp_update_vc0848(const void *buf_ptr, // source buffer point
                 signed short src_width,  
                 signed short src_starting_row, // start row on frame buffer
                 signed short src_starting_column, // start col on frame buffer
                 signed short num_of_rows, // width need to update
                 signed short num_of_columns, // column need to update
                 signed short dst_starting_row, // start row on screen
                 signed short dst_starting_column); // start col on screen


static vc_func_parm  fmpa_vol;
static vc_func_parm  media_vol;
static vc_frame_parm cap_frame;
static vc_union_type vc_data;
static boolean g_run_first_time = TRUE;
static vc_mode_codec_type mode_codec = {0};
static vc_timer_state vc_timer = VC_TIMER_RELEASE;
static boolean vc_recorder_3gp_done = FALSE;

boolean vc808_pa_enable_flag = 0;  //0:disable, 1:enable
vc_jpeg_decode display_jpg_done = VC_JPEG_DECODE_INIT;
static vc_status_type vc_dev_stat = VC_STAT_DONE;
static vc_dev_async_mod  vc_async_mod = VC_DEV_ASYNC_NONE;

extern void clkrgm_gpio30_clk_control(boolean bEnable);
extern void disppriv_set_window(byte left, byte top, byte width, byte height);
extern void SetVCBypassMode(boolean bMode);
extern void CMediaBg_CallbackNotify(void * pUser);
extern void OEMMediaMPEG4_CallbackNotify(AEEMediaCallback * pcb);

#define KICK_WATCHDOG() \
   dog_report(DOG_VC_RPT); \
   (void) rex_set_timer(&vc_rpt_timer, DOG_VC_RPT_TIME)

extern void VC_ACQUIRE_DISPLAY(void);
extern void VC_RELEASE_DISPLAY(void);

extern void btu_rex_wait(rex_sigs_type waitSig);
extern boolean  FullScreen;
extern boolean GetDispUpdatingFlag(void); 

/*===========================================================================

FUNCTION VC_TASK_START

DESCRIPTION
  This function performs VC task initialization.  The function
  first sets the acknowledge signal back to MC to acknowledge that it has
  been created.  It then waits for the start signal back to it.  While
  waiting for the start signal and keeps kicking the watchdog timer
  everytime its interval timer has expired.  When it gets the start
  signal it then performs file initialization.
  Once initialization completes, it then  acknowledges it back to MC 
  and returns to the caller.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The calling task stays here until it receives the START signal.

===========================================================================*/

LOCAL void vc_task_start
(
  rex_sigs_type    rpt_sig,        /* report timer signal for task */
  dog_report_type  dog_rpt,        /* Watchdog report for the task */
  rex_timer_type   *rpt_timer_ptr  /* pointer to report timer */
)
{
  rex_sigs_type sigs;               /* Signals returned by rex_timed_wait. */

  /* Acknowledge task creation. */
  (void) rex_set_sigs(&tmc_tcb, TMC_ACK_SIG);

  /* Continue to set watchdog timer while waiting for start signal from MC. */
  for (;;)
  {
    /* Set a timed wait for OR of signals and a watchdog report timer. */
    sigs = rex_timed_wait(TASK_START_SIG | rpt_sig, rpt_timer_ptr, DOG_VC_RPT_TIME);

    /* If start signal is set then perform file initialization and  
       signal back to MC.     */
    if ((sigs & TASK_START_SIG) != 0)
    {
       (void) rex_clr_sigs(&vc_tcb, TASK_START_SIG);
       
       MSG_MED("NVIM initialization successfull", 0, 0, 0);
       /* Signal back to TMC acknowledgement.  */
       (void) rex_set_sigs(&tmc_tcb, TMC_ACK_SIG);
       break;
    }
    /* Otherwise timer has expired.  Kick the watchdog and loop.   */
    else
    {
       (void) rex_clr_sigs(&vc_tcb, rpt_sig);
       dog_report(dog_rpt);
    }
  } /* for */
} /* vc_task_start */

/* <EJECT> */
/*===========================================================================

FUNCTION vc_init

DESCRIPTION
  init processing by the VC Task.

DEPENDENCIES
  This routine must not be called until the VC Task has been started.

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void  vc_init (void)
{
   MMD_U32 result = MMD_RET_NOERR;
   /* Init VC0848 here for Native LCD update in Bypass mode */
   //VC_GPIO_CTRL(42, HI);
   
   result = MMD_VC08XX_Init();
   switch(result)
   {
    case MMD_RET_NOERR:
       cap_frame.cx = DEDAULT_CAPTURE_SIZE_WIDTH;
       cap_frame.cy = DEDAULT_CAPTURE_SIZE_HEIGHT;
       g_init_stat = VC_INIT_DONE_S;
       break;

    case VIM_ERROR_SENSOR_AUTOFIND:
      
    default:
        g_init_stat = VC_INIT_BYPASS_ONLY;
        break;
   }
   vc_set_media_volume(80); // default volume for mp3/mp4 play
} /* vc_init */


/*===========================================================================

FUNCTION vc_cmd

DESCRIPTION
  Queue a command for processing by the VC Task.

DEPENDENCIES
  This routine must not be called until the VC Task has been started.

RETURN VALUE
  None

SIDE EFFECTS
  vc_cmd_q
  vc_tcb

===========================================================================*/
void  vc_cmd (
  vc_cmd_type  *cmd_ptr                   /* Pointer to VC command buffer */
)
{
  /* Set the request status to busy. initialize and link the command  */
  /* onto the VC command queue, set a signal to the VC task and exit. */
  
  //cmd_ptr->status = VC_STAT_BUSY;
  (void) q_link(cmd_ptr, &cmd_ptr->link);
  q_put(&vc_cmd_q, &cmd_ptr->link);
  (void) rex_set_sigs(&vc_tcb, VC_CMD_Q_SIG);
//  vc_set_device_state(VC_STAT_BUSY);     // wangjian disabled for Buf_play  2009.04.02
} /* vc_cmd */


/*===========================================================================

FUNCTION VC_TASK

DESCRIPTION
  This is the VC task, created via rex_def_task from the Main Control task.
  The VC task sets MC_ACK_SIG when it is done with initialization and it
  then waits for VC_START_SIG before starting its normal operations.  The
  VC task then enters a main loop, awaiting for commands received on its
  input queue.  Each command received is processed and the loop continues.
  While in the main loop the task kicks the watchdog periodically.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void 
vc_task 
(
  dword param      /* parameter from REX, unused */
)
{
  vc_cmd_type      *cmd_ptr;   /* Pointer to command received on vc_cmd_q */
  rex_sigs_type     rex_sigs;   /* REX signals word */

  rex_tcb_type     *task_ptr;  /* Task to signal back on request completion. */
  rex_sigs_type    task_sigs;  /* Signals to set back to requesting task */
  
/* Keep Lint happy */
#ifdef _lint
  param = param;
#endif

  MSG_MED("VC task created", 0, 0, 0);
  
  /* Initialize the watchdog report timer and VC  timer. */
  rex_def_timer(&vc_rpt_timer, &vc_tcb, VC_RPT_TIMER_SIG);
  rex_def_timer(&vc_user_timer, &vc_tcb, VC_USER_TIMER_SIG);
  rex_def_timer(&vc_pmu_switch_timer, &vc_tcb, VC_PMU_SWITCH_BYPASS_SIG);
  
  /* Initialize the VC command queue. */
  (void) q_init(&vc_cmd_q);

  /* Wait for start signal. */
  vc_task_start (VC_RPT_TIMER_SIG, DOG_VC_RPT, &vc_rpt_timer);

  /* Init Device VC0848 for LCD Bypass update func*/
  //(void) rex_set_sigs( &vc_tcb, VC_INIT_SIG );
  g_dev_info.curr_dev = VC_DEV_NONE;
  g_dev_info.dev_pri  = VC_DEV_PRI_LEV_0;
  
  /* Kick the wathcdog and start task operations. */
  KICK_WATCHDOG();

  for (;;)  /*  while (TRUE) and Lint happy */
  {
    /* Set REX wait with mask of all possible input signals. */
    rex_sigs = rex_wait(  VC_RPT_TIMER_SIG      /* Watchdog timer elapsed */
                        | VC_INIT_SIG                      /* init queue */
                        | VC_CMD_Q_SIG                      /* Input of command queue */
                        | VC_USER_TIMER_SIG                /* Input of command queue */
                        | VC_TIMER_STOP_SIG                 /* Stop Play */
                        | VC_TIMER_START_SIG                /* Start Play */
                        | VC_PMU_SWITCH_BYPASS_SIG           /* PMU switch bypass */
                        | TASK_STOP_SIG                     /* Stop the VC task */
                        | TASK_OFFLINE_SIG);               /* Go offline */

    /* Kick watchdog and start the watchdog report timer.   */
    /* Note that the watchdog is kicked after every wakeup, */
    /* following input of any signal to the NV task.        */
    (void) rex_clr_sigs(rex_self(), rex_sigs);
    
    KICK_WATCHDOG();

/*-------------------------------------------------------------------------*/
/*                           COMMAND QUEUE SIGNAL                                                               */
/*-------------------------------------------------------------------------*/
    /* If watchdog report timer expired, report and restart the timer                                  */
    if ( rex_sigs & VC_RPT_TIMER_SIG ) {
      dog_report(DOG_VC_RPT);
      (void) rex_set_timer( &vc_rpt_timer, DOG_VC_RPT_TIME );
    }

/*-------------------------------------------------------------------------*/
/*                         OFFLINE COMMAND SIGNAL                                                               */
/*-------------------------------------------------------------------------*/
    /* Handle the offline signal, process task offline procedures and continue                       */
    if ( (rex_sigs & TASK_OFFLINE_SIG) != 0 ) {
      MSG_MED( "TASK_OFFLINE_SIG received", 0,0,0 );
      (void) rex_clr_sigs( &vc_tcb, TASK_OFFLINE_SIG );
      vc_pmu_switch_bypass();
#ifdef FEATURE_MULTIMODE_ARCH
      /* Process task stop procedure.*/
      task_offline();
#else
      (void) rex_set_sigs( &mc_tcb, MC_ACK_SIG );
#endif
    }

/*-------------------------------------------------------------------------*/
/*                     POWERDOWN (STOP) COMMAND SIGNAL                                                */
/*-------------------------------------------------------------------------*/
    /* Check if powerdown command signal was set.  If set then clear signal and proceeed. */
    if ((rex_sigs & TASK_STOP_SIG) != 0) {
      (void) rex_clr_sigs( &vc_tcb, TASK_STOP_SIG );
#ifdef FEATURE_MULTIMODE_ARCH
      task_stop();
#else
      (void) rex_set_sigs( &mc_tcb, MC_ACK_SIG );
#endif /*FEATURE_MULTIMODE_ARCH*/

    } /* if stop command signal */

    if ((rex_sigs & VC_INIT_SIG) != 0) {
      (void) rex_clr_sigs(&vc_tcb, VC_INIT_SIG);
      //vc_init();
    }

    if ((rex_sigs & VC_TIMER_STOP_SIG) != 0) {
      MSG_ERROR("VC_PLAY_STOP_SIG here", 0, 0, 0);
      (void)rex_clr_timer(&vc_user_timer);
      (void) rex_clr_sigs(&vc_tcb, VC_TIMER_STOP_SIG);
      (void)rex_clr_sigs( &vc_tcb, VC_USER_TIMER_SIG);
    }

    /* Handle the VC0848 Timer signal, process the callback g_VC0848TimerISR pointed.
    */
    if ((rex_sigs & VC_USER_TIMER_SIG) != 0) 
    {
      (void)rex_clr_sigs( &vc_tcb, VC_USER_TIMER_SIG);
      if(vc_timer == VC_TIMER_RUN)
      {
       if(g_VC0848TimerISR != NULL)
       {
          //(void)rex_clr_sigs( &vc_tcb, VC_USER_TIMER_SIG);
          g_VC0848TimerISR();
          (void)rex_set_timer(&vc_user_timer, g_TimerInterval);
       }
      }
      else if(vc_timer & VC_TIMER_RELEASE)
      {
        g_VC0848TimerISR = NULL;
        g_TimerInterval = 0;
        MSG_ERROR("VC_USER_TIMER_SIG is released...", 0, 0, 0);
      }
    }
   
    if ((rex_sigs & VC_PMU_SWITCH_BYPASS_SIG)!= 0)
    {
      (void)rex_clr_sigs(&vc_tcb, VC_PMU_SWITCH_BYPASS_SIG);
      if(vc_get_pmu_switch_bypass_ready() == TRUE)
      {
         (void)rex_clr_timer(&vc_pmu_switch_timer);
         vc_set_pmu_flag_run_first_time(TRUE);
         vc_pmu_switch_bypass();
      }
      else
      {
         (void)rex_set_timer(&vc_pmu_switch_timer, 5000); // 5s later vc_task will switch vc0848 to bypass mode
         vc_set_pmu_flag_run_first_time(FALSE);
      }
    }


    /* Check if command queue signal was set.  If set then clear signal and proceeed. */
    if ((rex_sigs & VC_CMD_Q_SIG) != 0) 
    {
      MSG_LOW("VC_CMD_Q_SIG received", 0, 0, 0);
      (void) rex_clr_sigs(&vc_tcb, VC_CMD_Q_SIG);
      /* Enter a loop where the command queue is checked and */
      if(g_init_stat == VC_INIT_DONE_S) // only vc0848 init finished normally, can it process the cmd from external
      {
         while ((cmd_ptr = (vc_cmd_type *) q_get(&vc_cmd_q)) != NULL) 
         {
           /* Save for a moment the requesting task TCB and signal to set. */
           task_ptr = cmd_ptr->tcb_ptr;
           task_sigs = cmd_ptr->sigs;
             
           // if lcd is in disp_updating while vc0848 is in normal mode, wait for a while till disp_update is finished to avoid white screen issue
           if(GetDispUpdatingFlag() == TRUE)
           {
              rex_sleep(1);
           }
           
           switch (cmd_ptr->cmd) 
           {
             case VC_FUNC_CONTROL:
               memcpy(&vc_data, (const void *)cmd_ptr->data_ptr, sizeof(vc_data)); 
               cmd_ptr->status = vc_control_device(&vc_data, cmd_ptr->item);
               break;
     
             case VC_FUNC_SET_PARM:
                memcpy(&vc_data, (const void *)cmd_ptr->data_ptr, sizeof(vc_data)); 
               cmd_ptr->status = vc_set_parm(&vc_data, cmd_ptr->item);
               break;
     
             case VC_FUNC_GET_PARM:
               cmd_ptr->status = vc_get_parm(cmd_ptr);
               break;

             case VC_FUNC_CAMERA_ON:
                memcpy((void *)&vc_data, (const void *)cmd_ptr->data_ptr, sizeof(vc_data));
                cmd_ptr->status = vc_camera_on(&vc_data, cmd_ptr->item);
                break;

             case VC_FUNC_PLAY_ON:
                memcpy((void *)&vc_data, (const void *)cmd_ptr->data_ptr, sizeof(vc_data));
                cmd_ptr->status = vc_play_on(&vc_data, cmd_ptr->item);
                break;

             case VC_FUNC_FS_ON:
                cmd_ptr->status = vc_fs_on(cmd_ptr);
                break;
               
             case VC_FUNC_DISP_ON:
                cmd_ptr->status = vc_disp_on(cmd_ptr);
                break;
                 
             default:
               //cmd_ptr->status = VC_STAT_BADCMD;
               break;
           } /* end of switch */

           /* If specified in request then return buffer to specified queue. */

           if (cmd_ptr->done_q_ptr != NULL)
           q_put(cmd_ptr->done_q_ptr, &cmd_ptr->link);

           /* If specified in request then signal the requesting task. */
           if (task_ptr != NULL)
           (void) rex_set_sigs(task_ptr, task_sigs);
     
           /* At the end of the loop kick the watchdog timer. */
           KICK_WATCHDOG();
         } /* end of while */
      } /* g_init_stat == VC_INIT_DONE_S */
    } /* if command VC_CMD_Q_SIG queue signal */

    /* if vc0848 is working in some device, it needs the clock so forbid TCXO sleep */
    if(g_dev_info.curr_dev != VC_DEV_NONE)
    {
       db_items_value_type db_item;
       
       if((g_dev_info.curr_dev  == VC_DEV_CAMERA || g_dev_info.curr_dev == VC_DEV_PLY_AMR) &&
         (g_preview_stat == VC_PREVIEW_ON || g_play_mode == VC_PLAY_M || g_play_mode == VC_PAUSE_M))
       {
          db_item.bBacklightOn = TRUE;
          db_put(DB_BACKLIGHT_ON, &db_item);
       }
       else
       {
          db_item.bBacklightOn = FALSE;
          db_put(DB_BACKLIGHT_ON, &db_item);
       }
    }
    //MSG_FATAL("in vc_task for(...)", 0, 0, 0);
#ifdef FEATURE_OEM_DEBUG
   {
       extern uint32 AEEkHeap_GetMemStatsExt(uint32 * pdwTotal, uint32 * pdwMax);
       uint32 dwFree = 0, dwTotal = 0, dwMaxBolckSize = 0;
       
       dwFree = AEEkHeap_GetMemStatsExt(&dwTotal, &dwMaxBolckSize);
       DBGPRINTF("Free = %ld  Total = %ld  MaxBolckSize = %ld", dwFree, dwTotal, dwMaxBolckSize);
   }
#endif
    /* here, we need to deal with the backlight while vc0848 is in working status*/
    // TBD...
    
  } /* for (;;) */
} /* vc_task */

/*===========================================================================
FUNCTION: VC_DeviceControl

DESCRIPTION
  Control the device except parm GET/SET opertion, this func is designed for external application accessing out side of vc_task.
  but now,  we still put the implement of GET/SET operation in this func.
  
RETURN VALUE
  The VC return code, which is handled internally.

DEPENDENCIES
  This func will change the item status, so be careful.
===========================================================================*/
vc_status_type VC_DeviceControl
(
  vc_items_type   item,                  /* which item */
  vc_func_type    func,                    /* which function */
  vc_union_type   *data_ptr           /* what the action is */
)
{
   vc_cmd_type   vcf;
   
   vcf.tcb_ptr = rex_self();        /* Notify this task when done */
   vcf.done_q_ptr = NULL;           /* command goes to no queue when done */

   vcf.sigs = UI_VC_SIG;
   vcf.item = item;  
   vcf.cmd = func;
   
   vc_set_device_state(VC_STAT_BUSY);
   vcf.data_ptr = data_ptr;
   /* Call VC and wait for a response */
   (void)rex_clr_sigs(rex_self(), UI_VC_SIG);
   vc_cmd(&vcf); 
   if(vcf.tcb_ptr == &ui_tcb) 
   {
       oemui_rex_wait(UI_VC_SIG);
   }
   else
   {
       btu_rex_wait(UI_VC_SIG);
   }
   return vcf.status;
}

vc_status_type VC_DeviceControlAsync
(
  vc_items_type   item,                  /* which item */
  vc_func_type    func,                    /* which function */
  vc_union_type   *data_ptr           /* what the action is */
)
{
   vc_cmd_type   vcf;
   
   vcf.tcb_ptr = rex_self();        /* Notify this task when done */
   vcf.done_q_ptr = NULL;           /* command goes to no queue when done */

   vcf.sigs = 0;
   vcf.item = item;  
   vcf.cmd = func;
   
   vcf.data_ptr = data_ptr;
   vc_cmd(&vcf); 
   
   vc_set_device_state(VC_STAT_BUSY);
   return vcf.status;
}

vc_status_type VC_GetDeviceState()
{
   return vc_dev_stat;
}

void VC_SetDeviceAsyncMode(vc_dev_async_mod async_mod)
{
   if(async_mod < VC_DEV_CAMERA_ASYNC_DIS)
      vc_async_mod |= async_mod; // enable async, bit | operate
   else
      vc_async_mod &= async_mod; // disable async, bit & operate
}

vc_dev_async_mod VC_GetDeviceAsyncMode(void)
{
   return vc_async_mod;
}

extern int Appscomm_is_incoming_state(int bb);// mp3 incoming ring.
/*on=0:disable audio speaker, 1:enable */
void audio_speaker_ctrl(boolean on)   
{ 
    boolean headset_status;
    
    //note:must not switch SPEAKER_SEL here, because SPEAKER_SEL is in VC848 anytime except for baseband needing.
    // and baseband audio is top priority.
    
    OEM_GetConfig(CFGI_HEADSET_PRESENT,&headset_status, sizeof(headset_status));
    if((0 == headset_status)  //headset is plug-out
        ||(0 == on))    //for making channel to speaker if audio is off.
    {
        vc808_pa_enable_flag = on;  
        if(GetVCBypassMode() == FALSE)
        {
            MMD_Set_SpeakerType();  //loud-spk channel        
        }
        sndhw_ext_pa_speaker_ctl_com();    
    }
    else//headset is plug-in
    {
        if(1 == Appscomm_is_incoming_state(-1))
        {
            vc808_pa_enable_flag = on;  
            if(GetVCBypassMode() == FALSE)
            {
                MMD_Set_SpeakerType();  //MMD_Set_HpSpkType();  //Output headphone and speaker      
            }
            sndhw_ext_pa_speaker_ctl_com();   
        }
        else
        {
        vc808_pa_enable_flag = 0;
        sndhw_ext_pa_speaker_ctl_com(); 
        if(GetVCBypassMode() == FALSE)
        {
            MMD_Set_HeadphoneType();  //headset channel
            }         
        }    
    }
}

LOCAL vc_status_type vc_control_device(vc_union_type  *union_ptr, vc_items_type item)
{
  int nRet;
  if(g_init_stat == VC_INIT_BYPASS_ONLY){
     return VC_STAT_BADSTATE;
  }
   
  if (item >= VC_ITM_MAX_I) {
     return VC_STAT_BADPARM;
  }

  switch(item)
  {
    // device VC0848 open func, no matter what cmd_ptr.data_ptr is, 
    // open it if has not been opened, else return without do nothing
    case VC_ITM_DEV_OPEN_I: 
      // stop timer and keep vc0848 in normal mode
      (void)rex_clr_timer(&vc_pmu_switch_timer);
      vc_set_pmu_flag_run_first_time(TRUE);
        
      if(vc_get_dev_pmu_mode() != PMU_MODE_NORMAL)
      {
         VC_ACQUIRE_DISPLAY();
         disppriv_set_window(0, 0, MMD_LCD_WIDTH, MMD_LCD_HEIGHT);

         MMD_SYS_PMU_SWITCH(PMU_MODE_NORMAL);
         KICK_WATCHDOG();
         vc_set_dev_pmu_mode(PMU_MODE_NORMAL);
         SetVCBypassMode(FALSE);
         MMD_USER_WaitMICRS(150);
         VC_RELEASE_DISPLAY();
      }
      return vc_open_dev(union_ptr);
	 
    case VC_ITM_DEV_STOP_I:        /* device VC0848 stop func, now it's the open func implement as follows */
      if(vc_get_dev_pmu_mode() != PMU_MODE_NORMAL)
      {
         return VC_STAT_BADSTATE;
      }
      return vc_stop_dev(union_ptr);  
	 
    case VC_ITM_PREVIEW_I:               /* camera preview mode control func */
       //vc_data.preview = union_ptr->preview;
       //cmd_ptr->data_ptr = &vc_data;
       if(union_ptr->preview == VC_OP_OFF)
       {
         nRet = (int)MMD_Media_Preview_Off();
         g_preview_stat = VC_PREVIEW_OFF;
       }
       vc_set_device_state(VC_STAT_DONE);
       return VC_STAT_DONE;

    case VC_ITM_REC_VIDEO_I:               /* camera preview mode control func */
       if(union_ptr->preview == VC_OP_OFF)
       {
          audio_speaker_ctrl(0);
          nRet=MMD_Recorder_File_Stop(pFileHandle);
          MMD_USER_WaitMSEC(100);//(5000);
          nRet=MMD_Media_Preview_Off();
          nRet = MMD_Recorder_File_Close(NULL, pFileHandle);
          pFileHandle = NULL;
          g_preview_stat = VC_PREVIEW_OFF;
       }
       vc_set_device_state(VC_STAT_DONE);
       return VC_STAT_DONE;
	 
    case VC_ITM_REC_SNAPSHOT_I:               /* camera capture operation func */
       if(union_ptr->preview == VC_OP_OFF)
      {
         MMD_Media_Preview_Off(); // in snapshot, file has been closed.
         //nRet = MMD_Recorder_File_Close(NULL, pFileHandle);
         pFileHandle = NULL;
         g_preview_stat = VC_PREVIEW_OFF;
       }
       vc_set_device_state(VC_STAT_DONE);
       return VC_STAT_DONE;

    case VC_ITM_FS_POS_I:                /* fs position control on native efs or sd func */
	    return VC_STAT_DONE;

    default:
       return VC_STAT_BADPARM;
  } /* switch(cmd_ptr->item) */
}


LOCAL vc_status_type vc_set_parm(vc_union_type  *union_ptr, vc_items_type item)
{
    FILE_PROPERTY  g_fileproperty;
   if (item >= VC_ITM_MAX_I) {
      return VC_STAT_BADPARM;
  }
   MSG_FATAL("vc_set_parm", 0,0,0);

  switch(item)
  {
    case VC_ITM_SENSOR_STAT_I:               /* device VC0848 init func */
	 break;
	 
    case VC_ITM_SD_STAT_I:              /* device VC0848 stop func */
	 break;

    case VC_ITM_SET_CAP_SIZE_I:
       cap_frame.cx = union_ptr->parm.p1; // cx
       cap_frame.cy = union_ptr->parm.p2; // cy
       break;

    case VC_ITM_SET_FMPA_VOL_I: 
       fmpa_vol.p1 = union_ptr->parm.p1;
       fmpa_vol.p2 = union_ptr->parm.p2;
       MMD_Media_Codec_Set_Play_Volume(fmpa_vol.p1, fmpa_vol.p2);  //VC0848 MUTE
       break;

    case VC_ITM_SET_MEDIA_VOL_I:
       vc_set_media_volume(union_ptr->parm.p1);
       MMD_Media_Codec_Set_Play_Volume(vc_get_media_volume(), vc_get_media_volume());
       break;

    case VC_ITM_SET_EFFECT_I:
      MMD_Media_Set_Effect(union_ptr->parm.p1);
      break;

    case VC_ITM_SET_BRIGHTNESS_I:
      MMD_Media_Set_Brightness(union_ptr->parm.p1);
      break;

    case VC_ITM_SET_ENVIRONMENT_I:
      MMD_Media_Set_Environment(union_ptr->parm.p1);
      break;

    case VC_ITM_SET_BANDING_I:
      MMD_Media_Set_Banding(union_ptr->parm.p1);
      break;

    case VC_ITM_SET_AUDIO_CHANNEL_I:
        memset(&g_fileproperty,0,sizeof(FILE_PROPERTY));
        sprintf(g_fileproperty.stype, VIM_MEDIA_FILE_PROPERTY_AUDIO_INFOR);
        g_fileproperty.size = sizeof(VIM_AUDIO_INPUTINFOR); 
        MMD_Player_File_GetProperty(pFileHandle,&g_fileproperty);
      switch(union_ptr->parm.p1) 
      {
       
      default:  
      case VC_AUDIO_CHN_SPEAKER:
	  MSG_FATAL("VC_ITM_SET_AUDIO_CHANNEL_I-VC_AUDIO_CHN_SPEAKER", 0, 0, 0);
         //MMD_Media_Switch_AudioMode(1);
	     //MMD_Media_Codec_Set_Play_Mode(0, 0);
         MMD_Media_Switch_Play_Mode(pFileHandle,g_fileproperty.property.audioinfor.audio_smplrate,MMD_MEDIA_AUDIO_I2S);
         break;
         
      case VC_AUDIO_CHN_EARPHONE:
	  MSG_FATAL("VC_ITM_SET_AUDIO_CHANNEL_I-VC_AUDIO_CHN_EARPHONE", 0, 0, 0);
         //MMD_Media_Switch_AudioMode(1);
	     //MMD_Media_Codec_Set_Play_Mode(0, 0);
         MMD_Media_Switch_Play_Mode(pFileHandle,g_fileproperty.property.audioinfor.audio_smplrate,MMD_MEDIA_AUDIO_I2S);
         break;
         
      case VC_AUDIO_CHN_PCM:
	  MSG_FATAL("VC_ITM_SET_AUDIO_CHANNEL_I-VC_AUDIO_CHN_PCM", 0, 0, 0);
         //MMD_Media_Switch_AudioMode(0);
        // MMD_Media_Codec_Set_Play_Mode(MMD_MEDIA_STEROECHANNEL, MMD_MEDIA_SPEAKEROUT);
         MMD_Media_Switch_Play_Mode(pFileHandle,8000,MMD_MEDIA_AUDIO_PCM); 
	     
         break;
      }
      break;
    default:
        return VC_STAT_BADPARM;
  }   
  return VC_STAT_DONE;
}


LOCAL vc_status_type vc_get_parm(vc_cmd_type  *cmd_ptr)
{
   PMEDIA_FILE  plyMediaFile;
   char wszFileName[EFS_FILE_NAME_LEN*2];
   int nRet;
   MMD_U32 uPlayTime;
   if (cmd_ptr->item >= VC_ITM_MAX_I) {
       return VC_STAT_BADPARM;
  }

  switch(cmd_ptr->item)
  {
     case VC_ITM_SENSOR_STAT_I:      /*  get sensor status if it exist or not func */
     {
        VIM_RESULT result;
        // ensure current dev_mode is in NORMAL mode for SD status getting
        if(vc_get_dev_pmu_mode() != PMU_MODE_NORMAL)
        {
           VC_ACQUIRE_DISPLAY();
           disppriv_set_window(0, 0, MMD_LCD_WIDTH, MMD_LCD_HEIGHT);
           
           MMD_SYS_PMU_SWITCH(PMU_MODE_NORMAL);
           KICK_WATCHDOG();
           vc_set_dev_pmu_mode(PMU_MODE_NORMAL);
           SetVCBypassMode(FALSE);
           MMD_USER_WaitMICRS(150);
           VC_RELEASE_DISPLAY();
           
           MMD_SYS_Mode_Switch(SCENE_MEDIAPLAY);
           MMD_SYS_Codec_Switch(CODEC_PCM);
           MMD_USER_WaitMICRS(100);
           
        }
        // first work if getting open sig, is to clear pmu timer if exists.
        else if(vc_get_pmu_switch_bypass_ready() == TRUE) // means pmu_timer is running
        {
           // stop timer and keep vc0848 in normal mode
           (void)rex_clr_timer(&vc_pmu_switch_timer);
           vc_set_pmu_flag_run_first_time(TRUE);
        }

        VC_GPIO_CTRL(42, HI); //enable camera power
        result = VIM_MAPI_AutoFindSensor(); // fing sensor first
        if(result == VIM_SUCCEED)
        {
           // try to push current dev_sd into stack
           vc_dev_info dev_info;
           dev_info.curr_dev = VC_DEV_CAMERA;
           dev_info.dev_pri = vc_dev_convert_pri(VC_DEV_CAMERA);
           dev_info.sub_item = VC_ITM_NONE_I;
           (void)vc_dev_push_stack(&g_dev_info, &dev_info);
           cmd_ptr->data_ptr->sensor= VC_SENSOR_FIND_OK;
        }
        else
        {
           cmd_ptr->data_ptr->sensor= VC_SENSOR_FIND_ERR;
           if(g_dev_info.curr_dev == VC_DEV_NONE)
           {
              VC_GPIO_CTRL(42, LO); //disable camera power while in bypass mode
              vc_pmu_switch_bypass();
           }
        }
     }
        return VC_STAT_DONE;
     
     case VC_ITM_SD_STAT_I:               /*  get sd card status if it exist or not func */
     {
        VIM_RESULT result = VIM_SUCCEED;
        
        // ensure to stop timer and keep vc0848 in normal mode
        (void)rex_clr_timer(&vc_pmu_switch_timer);
        vc_set_pmu_flag_run_first_time(TRUE);
        
        // ensure current dev_mode is in NORMAL mode for SD status getting
        if(vc_get_dev_pmu_mode() != PMU_MODE_NORMAL)
        {
           VC_ACQUIRE_DISPLAY();
           disppriv_set_window(0, 0, MMD_LCD_WIDTH, MMD_LCD_HEIGHT);
           
           MMD_SYS_PMU_SWITCH(PMU_MODE_NORMAL);
           KICK_WATCHDOG();
           SetVCBypassMode(FALSE);
           vc_set_dev_pmu_mode(PMU_MODE_NORMAL);
           MMD_USER_WaitMICRS(150);
           VC_RELEASE_DISPLAY();

           MMD_SYS_Mode_Switch(SCENE_MEDIAPLAY);
           MMD_SYS_Codec_Switch(CODEC_PCM);
           MMD_USER_WaitMICRS(100);
        }
        // first work if getting open sig, is to clear pmu timer if exists.
        else if(vc_get_pmu_switch_bypass_ready() == TRUE) // means pmu_timer is running
        {
           // stop timer and keep vc0848 in normal mode
           (void)rex_clr_timer(&vc_pmu_switch_timer);
           vc_set_pmu_flag_run_first_time(TRUE);
        }
        if(VC_GetCurrentDevice() != VC_DEV_SD)
        {
           result = MMD_FILE_Mount();
        }
        if(result == VIM_SUCCEED)
        {
           // try to push current dev_sd into stack
           vc_dev_info dev_info;
           dev_info.dev_pri = vc_dev_convert_pri(VC_DEV_SD);
           dev_info.sub_item = VC_ITM_NONE_I;
           dev_info.curr_dev = VC_DEV_SD;
           cmd_ptr->data_ptr->sd = VC_SD_FIND_OK;
           (void)vc_dev_push_stack(&g_dev_info, &dev_info);
        }
        else
        {
           cmd_ptr->data_ptr->sd = VC_SD_FIND_ERR;
           if(g_dev_info.curr_dev == VC_DEV_NONE)
           {
              vc_pmu_switch_bypass();
           }
        }
        return VC_STAT_DONE;
     }
     
     case VC_ITM_DEV_STAT_I:
        cmd_ptr->data_ptr->dev_run.curr_dev = g_dev_info.curr_dev;
        cmd_ptr->status   = VC_STAT_DONE;
        break;

     case VC_ITM_BACKLIGHT_STAT_I:
        // if in amr(.mp4, .3gp play_m or in camera record movie, preview on mode, backlight should be on)
        if((g_play_mode == VC_PLAY_M && g_dev_info.curr_dev == VC_DEV_PLY_AMR) || g_dev_info.curr_dev == VC_DEV_CAMERA)
        {
           cmd_ptr->data_ptr->backlight = VC_BACKLIGHT_FORBID;
        }
        else
        {
           cmd_ptr->data_ptr->backlight = VC_BACKLIGHT_ALLOW;
        }
        return VC_STAT_DONE;
        
     case VC_ITM_GET_PLAY_TOTALTIME_I:
	 {
	   MMD_PLYFILE   pTOTALTIMEFHandle = NULL;
	   char  *pFilex = NULL;
	   
          MEMSET(&plyMediaFile,0,sizeof(PMEDIA_FILE));  
          MEMSET(wszFileName,0,EFS_FILE_NAME_LEN*2);
          pFilex = STRRCHR(cmd_ptr->data_ptr->play_info.szFileName,'.');
	   pFilex++;
	   if(0 == STRICMP(pFilex,"aac"))
	   {
	   	plyMediaFile.fs_filetype = MMD_MEDIA_FILE_TYPE_AAC;
	   }
          Utf8ToUnicode((byte *)&cmd_ptr->data_ptr->play_info.szFileName, STRLEN(cmd_ptr->data_ptr->play_info.szFileName), (uint16 *)&plyMediaFile.fs_filename, sizeof(plyMediaFile.fs_filename));
          plyMediaFile.fs_filestore = cmd_ptr->data_ptr->play_info.file_scr;//storetype;FILE_IN_CORE_FILE
          MMD_Media_Codec_Set_Play_Mode(0, 0);
          nRet = MMD_Player_File_Open(&plyMediaFile, &pTOTALTIMEFHandle);
          nRet = MMD_Player_File_GetTotalTime(pTOTALTIMEFHandle,&uPlayTime);
          cmd_ptr->data_ptr->parm.p1 = uPlayTime;

          MMD_Player_File_Close(pTOTALTIMEFHandle, NULL, NULL);
          return VC_STAT_DONE;
     	 }
         
     case VC_ITM_GET_PLAY_TIME_I:
        MMD_Player_File_GetPlayTime(pFileHandle, &uPlayTime);
        cmd_ptr->data_ptr->parm.p1 = uPlayTime;
        return VC_STAT_DONE;
    
     default:
        return VC_STAT_BADPARM;
  }   
}

LOCAL vc_status_type vc_open_dev(vc_union_type * union_ptr)
{
   vc_dev_type nDeviceType;
   if(g_dev_info.curr_dev == union_ptr->dev_run.curr_dev)
   {
      vc_set_device_state(VC_STAT_DONE);
      return VC_STAT_DONE;
   }
   
   if(vc_dev_push_stack(&g_dev_info, &union_ptr->dev_run) == FALSE)
   {
      // in vc_dev_push_stack(), it will set device state, so need not deal with device state here.
      return VC_STAT_DONE;
   }
   
   nDeviceType = vc_dev_scene_switch_on(g_dev_info.curr_dev);

   // set the return result 
   if(nDeviceType == g_dev_info.curr_dev)
      vc_set_device_state(VC_STAT_DONE);
   else
      vc_set_device_state(VC_STAT_FAIL);
   
   return VC_STAT_DONE;
}

LOCAL vc_status_type vc_stop_dev(vc_union_type * union_ptr)
{
   dword dev_running;
   boolean nRet;
   vc_dev_type nDeviceType;
   
   // dev_running is to judge whether the stop operation is on the running device or not.
   dev_running = g_dev_info.curr_dev & union_ptr->dev_run.curr_dev;

   /* want to stop the device not running, return badstate */
   if(!dev_running)
   {
      vc_set_device_state(VC_STAT_BADSTATE);
      return VC_STAT_BADSTATE;
   }

   // switch off current running device scene
   nRet = vc_dev_scene_switch_off(dev_running);
   if(nRet == FALSE)
   {
      vc_set_device_state(VC_STAT_BADPARM);
      return VC_STAT_BADPARM;
   }
   
   // pop the previous device from stack
   if(vc_dev_pop_stack(&g_dev_info) == FALSE)
   {
      vc_set_device_state(VC_STAT_FAIL);
      return VC_STAT_FAIL;
   }
   
   // switch on the previous device scene, g_dev_info.curr_dev was got from vc_dev_pop_stack.
   nDeviceType = vc_dev_scene_switch_on(g_dev_info.curr_dev);
   
   if(nDeviceType == g_dev_info.curr_dev)
      vc_set_device_state(VC_STAT_DONE);
   else
      vc_set_device_state(VC_STAT_FAIL);

   if(nDeviceType == VC_DEV_NONE)
   {
      // set timer with 5s to switch vc0848 from normal to bypass.
      rex_set_sigs(&vc_tcb, VC_PMU_SWITCH_BYPASS_SIG);
   }
   return VC_STAT_DONE;
}

LOCAL void vc_set_audiobias(void)
{
   ISound           *pSound; 
   AEESoundInfo     soundInfo;

   if (ISHELL_CreateInstance(AEE_GetShell(), AEECLSID_SOUND, (void **)&pSound)!=AEE_SUCCESS)
   {
      return;
   }

   soundInfo.eDevice = AEE_SOUND_DEVICE_HANDSET;
   soundInfo.eMethod = AEE_SOUND_METHOD_VOICE;
   soundInfo.eAPath = AEE_SOUND_APATH_BOTH;

   soundInfo.eEarMuteCtl = AEE_SOUND_MUTECTL_UNMUTED;
   soundInfo.eMicMuteCtl = AEE_SOUND_MUTECTL_MUTED;

   (void)ISOUND_Set(pSound,  &soundInfo);
   ISOUND_SetDevice(pSound);

   //ISOUND_SetVolume(pSound, GET_ISOUND_VOL_LEVEL(0));
   MSG_ERROR("==finished MMD_USER_SetAudio_Power setting==", 0, 0, 0);
}

LOCAL vc_status_type vc_preview_on(vc_cmd_type  *cmd_ptr)
{
   if (cmd_ptr->item >= VC_ITM_MAX_I) {
      return VC_STAT_BADPARM;
  }
   MSG_FATAL("vc_preview_on", 0, 0, 0);
  switch(cmd_ptr->item)
  {
    case VC_ITM_PREVIEW_I:               /* device VC0848 init func */
       MMD_Media_System_Init();
       if(cmd_ptr->data_ptr->prev_parm.prev_type == VC_PREV_SNAPSHOT)
       {
          media_work_mode_exit(VIM_MEDIA_MODE_RECORD_AUDIO_AND_VIDEO);
          media_work_mode_initialize(VIM_MEDIA_MODE_CAPTURE);
          /*0 means preview with not frame,1 means preview with frame*/
          MMD_Media_Set_Capture_FrameMode( cmd_ptr->data_ptr->prev_parm.prev_frametype); 
          MMD_Media_Set_Capture_Resolution(cmd_ptr->data_ptr->prev_parm.frame.cx, cmd_ptr->data_ptr->prev_parm.frame.cy);
          MMD_Media_Set_Capture_Quality(cmd_ptr->data_ptr->prev_parm.quality);
       }
       else // preview mode type is MOVIE
       {
          MSG_FATAL("come in video preview", 0, 0, 0);
          media_work_mode_exit(VIM_MEDIA_MODE_CAPTURE);
          media_work_mode_initialize(VIM_MEDIA_MODE_RECORD_AUDIO_AND_VIDEO);
          MMD_Media_Set_Video_Resolution(cmd_ptr->data_ptr->prev_parm.frame.cx, cmd_ptr->data_ptr->prev_parm.frame.cy );
       }
       MMD_Media_Set_Preview_Parameter((MMD_PREVIEW_OUTINFOR *)&cmd_ptr->data_ptr->prev_parm.prev_info);
       MMD_Media_Preview_On();
       g_preview_stat = VC_PREVIEW_ON;
       break;
	 
    default:
       return VC_STAT_BADPARM;
  }   
  return VC_STAT_DONE;
}


LOCAL vc_status_type vc_camera_on(vc_union_type  *union_ptr, vc_items_type item)
{
   static RMEDIA_FILE  recMediaFile;
   db_items_value_type  value_ptr;
   //static MMD_PLYFILE  pFileHandle;
   char szFileName[FILE_LEN_UTF8], wszFileName[FILE_LEN_UNICODE];
   int nRet;
   if (item >= VC_ITM_MAX_I) {
      return VC_STAT_BADPARM;
  }
  MSG_FATAL("vc_camera_on, item:%d", item, 0, 0);
  switch(item)
  {
     case VC_ITM_PREVIEW_I:               /* device VC0848 init func */
        MMD_Media_System_Init();
        MMD_LCD_BLY_Release(); 
        // ... call vc_camrea_init_parm() here
        vc_set_camera_init_parm(union_ptr->prev_parm);
        // note: from OEM Layer, setParm didn't call VC_DeviceControl to set cx and cy.
        cap_frame.cx = union_ptr->prev_parm.frame.cx;
        cap_frame.cy = union_ptr->prev_parm.frame.cy;
        
        MMD_Media_Set_Preview_Parameter((MMD_PREVIEW_OUTINFOR *)&union_ptr->prev_parm.prev_info);
        MMD_Media_Preview_On();
        // add by xinggang.xu 20090121 start
        //db_get(DB_ENVIRONMENT_MODE,&value_ptr); 
        //modify xinggang.xu 20090210 start
        //if(value_ptr.nEnvironMode == ENVIRONMENT_NIGHT)
        //{
           // MMD_Media_Set_Environment(value_ptr.nEnvironMode); //tcl_lb removed for camera preview 2009.04.01
        //}
        vc_set_device_state(VC_STAT_DONE);
        if(g_preview_stat == VC_PREVIEW_OFF) 
        {
           g_preview_stat = VC_PREVIEW_ON;
        }
        return VC_STAT_DONE;
        //break;
   
    case VC_ITM_REC_VIDEO_I:   /* device VC0848 init func, some init has been finished in PREVIEW_ON */
       ADIE_OUTM(ADIE_CODEC_EN2_R, ADIE_CODEC_EN2_EN_MICBIAS_M, ADIE_CODEC_EN2_EN_MICBIAS_ENA_V);
       VIM_HAPI_SetLCDSize(VIM_HAPI_B0_LAYER,0,0,0,0);
       VIM_HAPI_SetLCDSize(VIM_HAPI_B1_LAYER,0,0,0,0);
       MMD_LCD_BLY_Release();
       
       vc_set_audiobias(); // enable mic_bias
       MEMSET(szFileName,0,FILE_LEN_UTF8);
       MEMSET(&recMediaFile,0,sizeof(RMEDIA_FILE));
       recMediaFile.fs_filetype   = MMD_MEDIA_FILE_TYPE_3GPP;
       recMediaFile.fs_filestore  = union_ptr->parm.file_scr;  
       recMediaFile.f_limitedsize = 32*1024*1024; // 32M free space
       recMediaFile.f_limitedtime = 1800000; // 30 mins
        
       recMediaFile.a_codec       = MEDIA_AUDIO_TYPE_SAMR;
       
       //AMR=4750, 5150, 5900, 6700, 7400, 7950, 10200, 12200\n, MP3=96000,128000,192000
       recMediaFile.a_recbitrate  = 12200;
       recMediaFile.a_recbits     = 16;
       recMediaFile.a_recchn      = 1;
       recMediaFile.a_recsmplrate = 8000;
       
       recMediaFile.v_recfps      = 15;
       recMediaFile.v_recwidth    = MMD_FRAME_WIN_WIDTH;
       recMediaFile.v_recheight   = MMD_FRAME_WIN_HEIGHT;
       recMediaFile.v_codec       = MEDIA_VIDEO_TYPE_MP4V;
       if(union_ptr->parm.file_scr == VC_IN_CORE_FILE)
       {
            nRet = MMD_FILE_Mount();
            if(nRet)
            {
              vc_set_device_state(VC_STAT_NO_SD);
              return VC_STAT_NO_SD;
            }
       }
       
       Utf8ToUnicode((const byte *)&union_ptr->parm.szStr, EFS_FILE_NAME_LEN, (uint16 *)&recMediaFile.fs_filename, EFS_FILE_NAME_LEN*2);
       MMD_Media_SetRecodeChannel(MMD_AUDIOCODEC_LCHANNEL);
       MMD_Media_Codec_Set_Record_Mode(0, CODEC_MICPHONEIN);
       MMD_Media_Codec_Set_Mute(0, 0);
       MMD_Media_Codec_Set_Record_Volume(100, 100);
       MSG_FATAL("here start record movie", 0, 0, 0);
       VC_SetVideoRecorderDoneState(FALSE);
       nRet = MMD_Recorder_File_Creat((RECCallback)vc_video_recorder_cb, &recMediaFile, &pFileHandle);
       if(nRet!=0)
       {
          vc_set_device_state(VC_STAT_FAIL);
       }
       else
       {
          MMD_Recorder_File_Start(pFileHandle,NULL);
          vc_set_device_state(VC_STAT_DONE);
       }

       vc_set_curr_file_type(VC_FILE_OPEN_REC);
       return VC_STAT_DONE;

    case VC_ITM_REC_SNAPSHOT_I:
    {
       
       if(MMD_Media_Get_Capture_FrameMode()==0)
            MMD_LCD_BLY_Release(); 
       
       recMediaFile.v_recwidth   = cap_frame.cx;
       recMediaFile.v_recheight  = cap_frame.cy;
       recMediaFile.v_codec      = MEDIA_VIDEO_TYPE_JPG;
       recMediaFile.a_recchn      = 0;
       recMediaFile.v_recfps     = 0;
       recMediaFile.fs_filetype  = MMD_MEDIA_FILE_TYPE_JPEG;
       recMediaFile.fs_filestore = union_ptr->parm.file_scr;
       if(union_ptr->parm.file_scr == VC_IN_CORE_FILE)
       {
            nRet = MMD_FILE_Mount();
            if(nRet)
            {
              vc_set_device_state(VC_STAT_NO_SD);
              return VC_STAT_NO_SD;
            }
       }
       Utf8ToUnicode((const byte *)&union_ptr->parm.szStr, EFS_FILE_NAME_LEN, (uint16 *)&recMediaFile.fs_filename, EFS_FILE_NAME_LEN*2);
       //MMD_USER_WaitMICRS(2000);
       MSG_FATAL("here start capture, cx:%d, cy:%d ", cap_frame.cx, cap_frame.cy, 0);
       nRet = MMD_Recorder_File_Creat(NULL, &recMediaFile, &pFileHandle);
       vc_set_curr_file_type(VC_FILE_OPEN_REC);
       if(nRet!=0)
       {
         
         vc_set_device_state(VC_STAT_FAIL);
         return VC_STAT_FAIL;
       }

       nRet = MMD_Recorder_File_Start(pFileHandle,NULL);
       if(nRet!=0)
       {
         #if 1
         nRet = MMD_Recorder_File_Close(NULL,pFileHandle);
         MSG_FATAL("MMD_Recorder_File_Close=%d ",nRet, 0, 0);
         nRet=MMD_FILE_Delete((MMD_U08*)recMediaFile.fs_filename);
         MSG_FATAL("MMD_FILE_Delete=%d ",nRet, 0, 0);
         #endif
         vc_set_device_state(VC_STAT_FAIL);
         return VC_STAT_FAIL;
       }
       // Check the capturing state until it has stopped.

       nRet = MMD_Media_Preview_Off();
       if(nRet!=0)
       {
         vc_set_device_state(VC_STAT_FAIL);
         return VC_STAT_FAIL;
       }
       nRet = MMD_Recorder_File_Close(NULL,pFileHandle);
       if(nRet!=0)
       {
         vc_set_device_state(VC_STAT_FAIL);
         return VC_STAT_FAIL;
       }
       pFileHandle = NULL;
       vc_set_curr_file_type(VC_FILE_OPEN_NONE);
       g_preview_stat = VC_PREVIEW_OFF;
       
       vc_set_device_state(VC_STAT_DONE);
       return VC_STAT_DONE;
    }
       
    case VC_ITM_REC_AUDIO_I:
       ADIE_OUTM(ADIE_CODEC_EN2_R, ADIE_CODEC_EN2_EN_MICBIAS_M, ADIE_CODEC_EN2_EN_MICBIAS_ENA_V); // add by xinggang.xu 20090109
       //MMD_SYS_Mode_Switch(SCENE_REC_AMR); //add by xinggan.xu 20090109
      
       nRet = MMD_FILE_Mount();
       if(nRet)
       {
         vc_set_device_state(VC_STAT_NO_SD);
         return VC_STAT_NO_SD;
       }
       vc_set_audiobias(); // enable mic_bias
       MMD_SYS_Mode_Switch(SCENE_REC_AMR);
       MMD_Media_System_Init();
       nRet = MMD_Media_Mode_Initialzie(MMD_MEDIA_MODE_RECORD_AUDIO_ONLY);
       MEMSET(szFileName,0,FILE_LEN_UTF8);
       MEMSET(&recMediaFile,0,sizeof(RMEDIA_FILE));
       
       recMediaFile.fs_filetype=MMD_MEDIA_FILE_TYPE_AMR;
       recMediaFile.fs_filestore = union_ptr->parm.file_scr;  
       recMediaFile.f_limitedsize=5*1024*1024;//free space
       recMediaFile.f_limitedtime=86400000;//0x7fffffff;
        
       recMediaFile.a_codec=MEDIA_AUDIO_TYPE_SAMR;
       //AMR=4750, 5150, 5900, 6700, 7400, 7950, 10200, 12200\n, MP3=96000,128000,192000
       recMediaFile.a_recbitrate=12200;
       recMediaFile.a_recbits=16;
       recMediaFile.a_recchn=1;
       recMediaFile.a_recsmplrate=8000;
       recMediaFile.v_recwidth   = 0;
       recMediaFile.v_recheight  = 0;
       //end
       vc_get_date2filename(szFileName, EFS_FILE_NAME_LEN, VIM_FILE_TYPE_3GP);
       
       MEMSET(wszFileName,0,EFS_FILE_NAME_LEN*2);
      
       Utf8ToUnicode((const byte *)&union_ptr->parm.szStr, EFS_FILE_NAME_LEN, (uint16 *)&recMediaFile.fs_filename, EFS_FILE_NAME_LEN*2);

       nRet = MMD_Recorder_File_Creat(NULL, &recMediaFile, &pFileHandle);
       if(nRet != 0) 
       {
          vc_set_device_state(VC_STAT_FAIL);
          return VC_STAT_FAIL;
       }
       MMD_Recorder_File_Start(pFileHandle,NULL);
       
       vc_set_device_state(VC_STAT_DONE);
       vc_set_curr_file_type(VC_FILE_OPEN_REC);
       return VC_STAT_DONE;
      
    default:
       vc_set_device_state(VC_STAT_BADPARM);
       return VC_STAT_BADPARM;
  }   
}
int Jpeg_Entry=VC_JPEG_ENTRY_CAM;
LOCAL void vc_play_jpeg(unsigned int command)
{
  vc_dev_type nDeviceType;
  
  switch(command)
  {
    case MMD_PLY_END: // MMD_D2H_PLY_FILE_END
         g_play_mode = VC_PLAY_M;
         vc_set_curr_file_type(VC_FILE_OPEN_PLY);
         
         if(pFileHandle) 
            MMD_Player_File_Close(pFileHandle, NULL, NULL);
         
         vc_set_curr_file_type(VC_FILE_OPEN_NONE);
         g_play_mode = VC_NONE_M;
         pFileHandle = NULL;
         (void)vc_dev_pop_stack(&g_dev_info);
         
         nDeviceType = vc_dev_scene_switch_on(g_dev_info.curr_dev);
         
         if(nDeviceType == g_dev_info.curr_dev)
            vc_set_device_state(VC_STAT_DONE);
         else
            vc_set_device_state(VC_STAT_FAIL);
         
         if(nDeviceType == VC_DEV_NONE)
         {
            // set timer with 5s to switch vc0848 from normal to bypass.
            rex_set_sigs(&vc_tcb, VC_PMU_SWITCH_BYPASS_SIG);
         }
         display_jpg_done = VC_JPEG_DECODE_DONE;
         break;

    default: 
         break;     
  }
}

LOCAL vc_status_type vc_play_on(vc_union_type  *union_ptr, vc_items_type item)
{
   PMEDIA_FILE  plyMediaFile;
   char wszFileName[FILE_LEN_UNICODE];
   int nRet;
   vc_dev_info dev_info;
   vc_dev_type nDeviceType;

   if (item >= VC_ITM_MAX_I) {
      vc_set_device_state(VC_STAT_BADPARM);
      return VC_STAT_BADPARM;
  }
  MSG_ERROR("vc_play_on,item:%d, play_mode:%d, g_play_mode:%d", item, union_ptr->play_info.play_mode, g_play_mode);
  switch(item)
  {
    case VC_ITM_PLAY_MODE_I:               /* device VC0848 init func */
       switch(union_ptr->play_info.play_mode)
       {
       case VC_PLAY_M:
          if(g_play_mode == VC_PLAY_M)
          {
             vc_set_device_state(VC_STAT_BADSTATE);
             return VC_STAT_BADSTATE;
          }
          
          MEMSET(&plyMediaFile,0,sizeof(PMEDIA_FILE));
	   if(NULL == union_ptr->play_info.pFileaddr)
	   {
              MEMSET(wszFileName,0,FILE_LEN_UNICODE);
    
              //memcpy((char*)plyMediaFile.fs_filename, (const char *)wszFileName, FILE_LEN_UTF8*2);
              Utf8ToUnicode((byte *)&union_ptr->play_info.szFileName, STRLEN(union_ptr->play_info.szFileName), (uint16 *)&plyMediaFile.fs_filename, sizeof(plyMediaFile.fs_filename));
              
              plyMediaFile.fs_filestore = union_ptr->play_info.file_scr; //storetype;FILE_IN_CORE_FILE
	   }
	   else
	   {
	   	plyMediaFile.mem_fileaddr = (MMD_U32)union_ptr->play_info.pFileaddr;
		plyMediaFile.mem_filelength = union_ptr->play_info.fileLen;
		plyMediaFile.fs_filestore = FILE_IN_TCL_RAM;	   
	   }
          plyMediaFile.a_outbits = 16;
          plyMediaFile.a_outchn = 2;
          plyMediaFile.a_outsmplrate = 48000;//8000;//48000;
          plyMediaFile.v_outheight = VIDEO_LCD_HEIGHT;//MMD_LCD_HEIGHT;//270;//180;
          plyMediaFile.v_outwidth = MMD_LCD_WIDTH;
          plyMediaFile.v_outeffect = 8;
          plyMediaFile.v_outxpos = 0;
          plyMediaFile.v_outypos = 20;  //20 为预留视频标题空间
          plyMediaFile.v_outrotation = MMD_MEDIA_ROTATE_0;
          plyMediaFile.v_outeffect = MMD_MEDIA_SPECIAL_NORMAL;
#if 0
          if(g_dev_info.curr_dev == VC_DEV_PLY_MP3)
          {
             plyMediaFile.fs_filetype = MMD_MEDIA_FILE_TYPE_MP3;
          }
#endif
          switch(g_dev_info.curr_dev)
          {
          	case VC_DEV_PLY_MP3:
		    plyMediaFile.fs_filetype = MMD_MEDIA_FILE_TYPE_MP3;
		    break;

              case VC_DEV_PLY_AMR:
                  break;

		case VC_DEV_PLY_AMRAUDIO:
		    plyMediaFile.fs_filetype = MMD_MEDIA_FILE_TYPE_AMR;
		    break;

		case VC_DEV_PLY_AAC:
		    plyMediaFile.fs_filetype = MMD_MEDIA_FILE_TYPE_AAC;
		    break;

		default:
		    break;
          }
         // DBGPRINTF("g_bPlayMP3ByBt is %d",g_bPlayMP3ByBt);
#ifdef FEATURE_SUPPORT_BT_APP
          if(FALSE == GetIsPlayMP3ByBt())
          {
             MMD_Media_Set_AudioMode(1);
          }
          else
          {
            MMD_Media_Set_AudioMode(0);
          }
#else
          MMD_Media_Set_AudioMode(1);
#endif
          MMD_Media_Codec_Set_Play_Mode(0, 0);
          MMD_Media_Codec_Set_Mute(1,0);
          
          nRet = MMD_Player_File_Open(&plyMediaFile, &pFileHandle);
          if(g_dev_info.curr_dev == VC_DEV_PLY_AMR)
          {
             VIM_HAPI_SetLCDWorkMode(VIM_HAPI_LCDMODE_OVERLAY, 0x0000);
             VIM_HAPI_SetLCDSize(VIM_HAPI_B1_LAYER,0,0,0,0);
             VIM_HAPI_SetLCDSize(VIM_HAPI_B0_LAYER,0,164,176,56);
             VIM_HAPI_DrawLCDPureColor(VIM_HAPI_B0_LAYER, 0,164,176,56,0x0000);
          }
          nRet = MMD_Player_File_Play(pFileHandle, (PLYCallback)OEMMedia_VC_PlayCB);
          MMD_Media_Codec_Set_Play_Volume(vc_get_media_volume(), vc_get_media_volume());
          audio_speaker_ctrl(1);
          vc_set_curr_file_type(VC_FILE_OPEN_PLY);
          if(0 == nRet)
          {
             g_play_mode = VC_PLAY_M;
             vc_set_device_state(VC_STAT_DONE);
             return VC_STAT_DONE;
          }
          else
          {
              if(pFileHandle) MMD_Player_File_Close(pFileHandle,NULL,NULL);
              MSG_ERROR("Play file Failed, MMD_Player_File_Close return:%d", nRet, 0, 0);
              vc_set_curr_file_type(VC_FILE_OPEN_NONE);
              pFileHandle = NULL;
              g_play_mode = VC_NONE_M;
             vc_set_device_state(VC_STAT_FAIL);
             return VC_STAT_FAIL;
          }
          
       case VC_PAUSE_M:
          if(g_play_mode != VC_PLAY_M)
          {
             vc_set_device_state(VC_STAT_BADSTATE);
             return VC_STAT_BADSTATE;
          }
          audio_speaker_ctrl(0);
          if(pFileHandle) MMD_Player_File_Pause( pFileHandle);
          g_play_mode = VC_PAUSE_M;
          vc_set_device_state(VC_STAT_DONE);
          return VC_STAT_DONE;

       case VC_RESUME_M:
          if(g_play_mode != VC_PAUSE_M)
          {
             vc_set_device_state(VC_STAT_BADSTATE);
             return VC_STAT_BADSTATE;
          }
          g_play_mode = VC_PLAY_M;
          if(pFileHandle) MMD_Player_File_Resume( pFileHandle);
          audio_speaker_ctrl(1); 
          vc_set_device_state(VC_STAT_DONE);
          return VC_STAT_DONE;

       case VC_STOP_M:
          if(pFileHandle == NULL || g_play_mode == VC_NONE_M)
          {
            vc_set_device_state(VC_STAT_DONE);
            return VC_STAT_DONE;
          }
          
          if((g_play_mode != VC_PLAY_M) && (g_play_mode != VC_PAUSE_M))
          {
             vc_set_device_state(VC_STAT_BADSTATE);
             return VC_STAT_BADSTATE;
          }
          
          if(g_play_mode == VC_PLAY_M)
          {
             audio_speaker_ctrl(0);
             MMD_Player_File_Stop(pFileHandle);
             Lcd_Rotate(VIM_HAPI_ROTATE_0);
             VIM_SetRT(0);
          }
          
          MMD_Player_File_Close(pFileHandle,NULL,NULL);
          vc_set_curr_file_type(VC_FILE_OPEN_NONE);
          pFileHandle = NULL;
          g_play_mode = VC_NONE_M;
          MMD_LCD_BLY_Release();
          vc_set_device_state(VC_STAT_DONE);
          return VC_STAT_DONE;

       case VC_SEEK_M:
	   if(!pFileHandle)
	    {
	         return VC_STAT_BADPARM;
	    }
          MMD_Player_File_Stop(pFileHandle);
          nRet = MMD_Player_File_Seek(pFileHandle,(MMD_U32)union_ptr->parm.p1,NULL);
          nRet = MMD_Player_File_Play(pFileHandle,(PLYCallback)OEMMedia_VC_PlayCB);
          break;	
		  
       default:
         break;
       }
       break;

    case VC_ITM_JPG_DISPCLR_I:
       {
            db_items_value_type db_item;          
            db_item.nCamMode = 5; // CAM_MODE_DISPJPEG = 5
            db_put(DB_CAMERA_MODE, &db_item);
       }
       break;
	 
    case VC_ITM_JPG_DECODE_I:
        display_jpg_done = VC_JPEG_DECODE_DOING;
      //  if(union_ptr->play_info.entry == VC_JPEG_ENTRY_FS)
      //  {
           // db_items_value_type db_item;          
        //    db_item.nCamMode = 1; // CAM_MODE_READY = 1
       //     db_put(DB_CAMERA_MODE, &db_item);
      //  }
        Jpeg_Entry=union_ptr->play_info.entry;
        dev_info.curr_dev = VC_DEV_JPEG;
        dev_info.dev_pri = vc_dev_convert_pri(VC_DEV_JPEG);
        dev_info.sub_item = VC_ITM_NONE_I;
        (void)vc_dev_push_stack(&g_dev_info, &dev_info);

        MMD_SYS_Mode_Switch(SCENE_MEDIAPLAY);
        MMD_SYS_Codec_Switch(CODEC_PCM);
        //MMD_LCD_BLY_Release();
        //MMD_LCD_SetOverlay(VM_DISABLE, MMD_LCD_B0LAYER, OVL_TSP, 0xffffff); // 20090303     20090304   
        MMD_Media_Mode_Exit(MMD_MEDIA_MODE_CAPTURE);
        MMD_Media_Mode_Initialzie(MMD_MEDIA_MODE_PLAY_AUDIO_AND_VIDEO);
        
        MEMSET(&plyMediaFile,0,sizeof(PMEDIA_FILE));
        Utf8ToUnicode((byte *)&union_ptr->play_info.szFileName, STRLEN(union_ptr->play_info.szFileName), (uint16 *)&plyMediaFile.fs_filename, sizeof(plyMediaFile.fs_filename));

        plyMediaFile.fs_filestore = FILE_IN_CORE_FILE;
        plyMediaFile.a_outbits = 16;
        plyMediaFile.a_outchn = 2;
        plyMediaFile.a_outsmplrate = 48000;//8000;//48000;
        plyMediaFile.v_outrotation = MMD_MEDIA_ROTATE_0;
        plyMediaFile.v_outeffect = MMD_MEDIA_SPECIAL_NORMAL;
        plyMediaFile.v_jpgmode=MMD_MEDIA_JPEG_MODE_DISPLAY;
        plyMediaFile.v_jpgdecodeaddr=0;
        plyMediaFile.v_jpgdecodelen=0;
        if(union_ptr->play_info.entry == VC_JPEG_ENTRY_CAM)
        {
            if(union_ptr->play_info.cam_size == VC_CAM_SIZE_176_220)
            {
                plyMediaFile.v_outheight = MMD_LCD_HEIGHT;//TST_LCD_HEIGHT;//270;//180;
                plyMediaFile.v_outwidth = MMD_LCD_WIDTH;//TST_LCD_WIDTH;
                
                plyMediaFile.v_outxpos = 0;
                plyMediaFile.v_outypos = 0;
            }
            else
            {
                plyMediaFile.v_outheight = 132;//TST_LCD_HEIGHT;//270;//180;
                plyMediaFile.v_outwidth = MMD_LCD_WIDTH;//TST_LCD_WIDTH;
                
                plyMediaFile.v_outxpos = 0;
                plyMediaFile.v_outypos = 28;
            }
        }
        else if(union_ptr->play_info.entry == VC_JPEG_ENTRY_FS)
        {            
            MMD_LCD_BLY_Release();
            MMD_LCD_SetOverlay(VM_DISABLE, MMD_LCD_B0LAYER, OVL_TSP, 0xffffff);
            plyMediaFile.v_outheight = MMD_LCD_HEIGHT;//MMD_LCD_HEIGHT;//TST_LCD_HEIGHT;//270;//180;
            plyMediaFile.v_outwidth = MMD_LCD_WIDTH;//TST_LCD_WIDTH;
            
            plyMediaFile.v_outxpos = 0;
            plyMediaFile.v_outypos = 0;
        }
        
        nRet = MMD_Player_File_Open(&plyMediaFile, &pFileHandle);
        if(nRet)
        	display_jpg_done=VC_JPEG_DECODE_DONE;
        nRet = MMD_Player_File_Play(pFileHandle, (PLYCallback)vc_play_jpeg);
        if(nRet)
        	display_jpg_done=VC_JPEG_DECODE_DONE;
        break;
     
    case VC_ITM_JPG_STOP_DECODE_I:
        if(display_jpg_done == VC_JPEG_DECODE_DOING)
        {            
            if(pFileHandle)             
               MMD_Player_File_Close(pFileHandle, NULL, NULL);
            
            vc_set_curr_file_type(VC_FILE_OPEN_NONE);
            g_play_mode = VC_NONE_M;
            pFileHandle = NULL;
            (void)vc_dev_pop_stack(&g_dev_info);
            
            nDeviceType = vc_dev_scene_switch_on(g_dev_info.curr_dev);
            
            if(nDeviceType == g_dev_info.curr_dev)
               vc_set_device_state(VC_STAT_DONE);
            else
               vc_set_device_state(VC_STAT_FAIL);
            
            if(nDeviceType == VC_DEV_NONE)
            {
               // set timer with 5s to switch vc0848 from normal to bypass.
               rex_set_sigs(&vc_tcb, VC_PMU_SWITCH_BYPASS_SIG);
            }
            display_jpg_done = VC_JPEG_DECODE_DONE;
            //MMD_LCD_SetOverlay (VM_ENABLE,MMD_LCD_B0LAYER,OVL_TSP, 0xffffff);// 20090303 modify20090304
            
        }        
        break;
        
    default:
        vc_set_device_state(VC_STAT_BADPARM);
        return VC_STAT_BADPARM;
  }
  vc_set_device_state(VC_STAT_DONE);
  return VC_STAT_DONE;
}

LOCAL vc_status_type vc_disp_on(vc_cmd_type  *cmd_ptr)
{
    if (cmd_ptr->item >= VC_ITM_MAX_I) {
        vc_set_device_state(VC_STAT_BADPARM);
        return VC_STAT_BADPARM;
    }
    switch(cmd_ptr->item)
    {
      case VC_ITM_DISP_UPDATE_I:               /* device VC0848 init func */
          vc_disp_update_vc0848(cmd_ptr->data_ptr->disp_info.buf_ptr,
                        cmd_ptr->data_ptr->disp_info.src_width,
                        cmd_ptr->data_ptr->disp_info.src_starting_row,
                        cmd_ptr->data_ptr->disp_info.src_starting_column,
                        cmd_ptr->data_ptr->disp_info.num_of_rows,
                        cmd_ptr->data_ptr->disp_info.num_of_columns,
                        cmd_ptr->data_ptr->disp_info.dst_starting_row,
                        cmd_ptr->data_ptr->disp_info.dst_starting_column);
        
        break;

      default:
        break;
    }
    
}


LOCAL vc_status_type vc_fs_on(vc_cmd_type  *cmd_ptr)
{
   int nRet;
   vc_status_type status;
   // for VC0848 it needs the fixed F_FIND address for FindNext operation, so define fileFind as static type
   static F_FIND fileFind;
   char szExtName[8]={'*',0, '.',0,'*',0,0,0}; // use double byte in unicode mode not utf8
   
   char *BPath = NULL;
   if(vc_get_dev_pmu_mode() != PMU_MODE_NORMAL)
   {
      VC_ACQUIRE_DISPLAY();
      MMD_SYS_PMU_SWITCH(PMU_MODE_NORMAL);
      KICK_WATCHDOG();
      SetVCBypassMode(FALSE);
      vc_set_dev_pmu_mode(PMU_MODE_NORMAL);
      MMD_USER_WaitMICRS(150);
      VC_RELEASE_DISPLAY();
      
      MMD_SYS_Mode_Switch(SCENE_MEDIAPLAY);
      MMD_SYS_Codec_Switch(CODEC_PCM);
      nRet = MMD_FILE_Mount();

      if(nRet == MMD_FILE_NO_ERROR)
      {
         // try to push current dev_sd into stack
         vc_dev_info dev_info;
         dev_info.curr_dev = VC_DEV_SD;
         dev_info.dev_pri = vc_dev_convert_pri(VC_DEV_SD);
         dev_info.sub_item = VC_ITM_NONE_I;
         (void)vc_dev_push_stack(&g_dev_info, &dev_info);
      }
      else
      {
         if(g_dev_info.curr_dev == VC_DEV_NONE)
         {
            vc_pmu_switch_bypass();
         }
         return VC_STAT_NO_SD;
      }
   }
   
   {
       // stop timer and keep vc0848 in normal mode
       (void)rex_clr_timer(&vc_pmu_switch_timer);
       vc_set_pmu_flag_run_first_time(TRUE);
       //vc_set_delaytimer(30000); // set 30s for sd mode to switch off vc0848
   }
   
   switch(cmd_ptr->item)
   {
   case VC_ITM_FS_ENUM_INIT_I:
    {
      char szPath[EFS_FILE_NAME_LEN*2] = {'0'};  
      // every time before FindFirst, it should MOUNT operation for VC0848
      if(VC_GetCurrentDevice() != VC_DEV_SD)
      {
         nRet = MMD_FILE_Mount();
      }
      
      memset(&fileFind, 0, sizeof(F_FIND));
      Utf8ToUnicode((byte *)&cmd_ptr->data_ptr->fs_info.szDir, STRLEN(cmd_ptr->data_ptr->fs_info.szDir), (uint16 *)&szPath, EFS_FILE_NAME_LEN*2);
      WSTRCAT((uint16 *)&szPath, (uint16 *)&szExtName);
      BPath = (char *)&szPath;
      cmd_ptr->data_ptr->fs_info.efs2.nErr = MMD_FILE_FindFirst((MMD_U08*)BPath, &fileFind);
      
      // remove fold "." and ".." from enumerate list
      if(fileFind.filename[0] == '.')
      {
          memset(fileFind.filename, 0, F_MAXLNAME);
          cmd_ptr->data_ptr->fs_info.efs2.nErr = MMD_FILE_FindNext(&fileFind);
          if(fileFind.filename[0] == '.' && cmd_ptr->data_ptr->fs_info.efs2.nErr == SUCCESS)
          {
              memset(fileFind.filename, 0, F_MAXLNAME);
              cmd_ptr->data_ptr->fs_info.efs2.nErr = MMD_FILE_FindNext(&fileFind);
          }
      }
      // enumerate dir or file according to flag cmd_ptr->data_ptr->fs_info.bDirs
      if(cmd_ptr->data_ptr->fs_info.efs2.nErr == SUCCESS)
      {
        if(cmd_ptr->data_ptr->fs_info.bDirs)
        {
            while(!(fileFind.attr & MMD_FILE_ATTR_DIR) && cmd_ptr->data_ptr->fs_info.efs2.nErr == SUCCESS)
            {
                memset(fileFind.filename, 0, F_MAXLNAME);
                cmd_ptr->data_ptr->fs_info.efs2.nErr = MMD_FILE_FindNext(&fileFind);
            }
            if(cmd_ptr->data_ptr->fs_info.efs2.nErr == SUCCESS)
            {
                cmd_ptr->data_ptr->fs_info.efs2.attrib = vc_attr2efs(fileFind.attr); 
                UnicodeToUtf8((uint16 *)fileFind.filename, EFS_FILE_NAME_LEN*2, (byte *)cmd_ptr->data_ptr->fs_info.efs2.szName, EFS_FILE_NAME_LEN);
                cmd_ptr->data_ptr->fs_info.efs2.szName[EFS_FILE_NAME_LEN-1] = 0x0;
                cmd_ptr->data_ptr->fs_info.efs2.dwSize = fileFind.filesize;
                cmd_ptr->data_ptr->fs_info.efs2.dwCreationDate = vc_datetime_to_second(fileFind.cdate, fileFind.ctime); 
            }
        }
        else
        {
            while((fileFind.attr & MMD_FILE_ATTR_DIR) && cmd_ptr->data_ptr->fs_info.efs2.nErr == SUCCESS)
            {
                memset(fileFind.filename, 0, F_MAXLNAME);
                cmd_ptr->data_ptr->fs_info.efs2.nErr = MMD_FILE_FindNext(&fileFind);
            }
            if(cmd_ptr->data_ptr->fs_info.efs2.nErr == SUCCESS)
            {
                cmd_ptr->data_ptr->fs_info.efs2.attrib = vc_attr2efs(fileFind.attr); 
                UnicodeToUtf8((uint16 *)fileFind.filename, EFS_FILE_NAME_LEN*2, (byte *)cmd_ptr->data_ptr->fs_info.efs2.szName, EFS_FILE_NAME_LEN);
                cmd_ptr->data_ptr->fs_info.efs2.szName[EFS_FILE_NAME_LEN-1] = 0x0;
                cmd_ptr->data_ptr->fs_info.efs2.dwSize = fileFind.filesize;
                cmd_ptr->data_ptr->fs_info.efs2.dwCreationDate = vc_datetime_to_second(fileFind.cdate, fileFind.ctime); 
            }
        }
      }
   }
      return VC_STAT_DONE;

   case VC_ITM_FS_ENUM_NEXT_I:
      memset(fileFind.filename, 0, F_MAXLNAME);
      cmd_ptr->data_ptr->fs_info.efs2.nErr = MMD_FILE_FindNext(&fileFind);

    // enumerate dir or file according to flag cmd_ptr->data_ptr->fs_info.bDirs
    if(cmd_ptr->data_ptr->fs_info.efs2.nErr == SUCCESS)
    {
      if(cmd_ptr->data_ptr->fs_info.bDirs)
      {
          while(!(fileFind.attr & MMD_FILE_ATTR_DIR) && cmd_ptr->data_ptr->fs_info.efs2.nErr == SUCCESS)
          {
              memset(fileFind.filename, 0, F_MAXLNAME);
              cmd_ptr->data_ptr->fs_info.efs2.nErr = MMD_FILE_FindNext(&fileFind);
          }
          if(cmd_ptr->data_ptr->fs_info.efs2.nErr == SUCCESS)
          {
              cmd_ptr->data_ptr->fs_info.efs2.attrib = vc_attr2efs(fileFind.attr); 
              UnicodeToUtf8((uint16 *)fileFind.filename, EFS_FILE_NAME_LEN*2, (byte *)cmd_ptr->data_ptr->fs_info.efs2.szName, EFS_FILE_NAME_LEN);
              cmd_ptr->data_ptr->fs_info.efs2.szName[EFS_FILE_NAME_LEN-1] = 0x0;
              cmd_ptr->data_ptr->fs_info.efs2.dwSize = fileFind.filesize;
              cmd_ptr->data_ptr->fs_info.efs2.dwCreationDate = vc_datetime_to_second(fileFind.cdate, fileFind.ctime); 
          }
      }
      else
      {
          while((fileFind.attr & MMD_FILE_ATTR_DIR) && cmd_ptr->data_ptr->fs_info.efs2.nErr == SUCCESS)
          {
              memset(fileFind.filename, 0, F_MAXLNAME);
              cmd_ptr->data_ptr->fs_info.efs2.nErr = MMD_FILE_FindNext(&fileFind);
          }
          if(cmd_ptr->data_ptr->fs_info.efs2.nErr == SUCCESS)
          {
              cmd_ptr->data_ptr->fs_info.efs2.attrib = vc_attr2efs(fileFind.attr); 
              UnicodeToUtf8((uint16 *)fileFind.filename, EFS_FILE_NAME_LEN*2, (byte *)cmd_ptr->data_ptr->fs_info.efs2.szName, EFS_FILE_NAME_LEN);
              cmd_ptr->data_ptr->fs_info.efs2.szName[EFS_FILE_NAME_LEN-1] = 0x0;
              cmd_ptr->data_ptr->fs_info.efs2.dwSize = fileFind.filesize;
              cmd_ptr->data_ptr->fs_info.efs2.dwCreationDate = vc_datetime_to_second(fileFind.cdate, fileFind.ctime); 
          }
      }
    }

      status = VC_STAT_DONE;
      break;

   case VC_ITM_FS_MKDIR_I:
    {
      char szPath[EFS_FILE_NAME_LEN*2] = {'0'};  
      
      Utf8ToUnicode((byte *)&cmd_ptr->data_ptr->fs_info.szDir, STRLEN(cmd_ptr->data_ptr->fs_info.szDir), (uint16 *)&szPath, EFS_FILE_NAME_LEN*2);
      cmd_ptr->data_ptr->fs_info.efs2.nErr = MMD_FILE_MkDir((uint8 *)&szPath);
      status = VC_STAT_DONE;
   }
      break;

   case VC_ITM_FS_RMDIR_I:
    {
       char szPath[EFS_FILE_NAME_LEN*2] = {'0'}; 
       Utf8ToUnicode((byte *)&cmd_ptr->data_ptr->fs_info.szDir, STRLEN(cmd_ptr->data_ptr->fs_info.szDir), (uint16 *)&szPath, EFS_FILE_NAME_LEN*2);
       cmd_ptr->data_ptr->fs_info.efs2.nErr = MMD_FILE_RmDir((uint8 *)&szPath);
       status = VC_STAT_DONE;
   }
       break;

   case VC_ITM_FS_TEST_I:
    {
      char szPath[EFS_FILE_NAME_LEN*2] = {'0'};  
      char szNewPath[EFS_FILE_NAME_LEN*2];  
      memset((void *)&szPath, 0, EFS_FILE_NAME_LEN*2);
      memset((void *)&szNewPath, 0, EFS_FILE_NAME_LEN*2);
      cmd_ptr->data_ptr->fs_info.efs2.nErr = MMD_FILE_GetCWD((uint8 *)&szNewPath); // save current working dir
      
      Utf8ToUnicode((byte *)&cmd_ptr->data_ptr->fs_info.szDir, STRLEN(cmd_ptr->data_ptr->fs_info.szDir), (uint16 *)&szPath, EFS_FILE_NAME_LEN*2);
      cmd_ptr->data_ptr->fs_info.efs2.nErr = MMD_FILE_ChDir((uint8 *)&szPath);
      
      if(cmd_ptr->data_ptr->fs_info.efs2.nErr) // if nErr != 0 means specified DIR is not existed, try to test file then.
      {
         int existFlag = 0;
         cmd_ptr->data_ptr->fs_info.efs2.attrib = EFS2_FA_NODIR; 
         cmd_ptr->data_ptr->fs_info.efs2.nErr = MMD_FILE_IsFileExisted((uint8 *)&szPath, (uint8 *)&existFlag);
         if(cmd_ptr->data_ptr->fs_info.efs2.nErr == SUCCESS) // if existFlag == 0 means file not existed, if 1 means file existed
         {
            if(existFlag)
               cmd_ptr->data_ptr->fs_info.efs2.attrib = EFS2_FA_READONLY;
            else
               cmd_ptr->data_ptr->fs_info.efs2.attrib = EFS2_FA_NOFILE; 
         }
         // if process case else here, means no dir and no file existed.
      }
      else // if efs2.nErr == 0, means dir existed
      {
         cmd_ptr->data_ptr->fs_info.efs2.attrib = EFS2_FA_DIR;
         (void)MMD_FILE_ChDir((uint8 *)&szNewPath); // restore to the working dir
         //cmd_ptr->data_ptr->fs_info.efs2.nErr = SUCCESS;
      }
      status = VC_STAT_DONE;
   }
      break;

   case VC_ITM_FS_CREATE_FILE_I:

      break;

   case VC_ITM_FS_DELETE_FILE_I:
    {
      char szPath[EFS_FILE_NAME_LEN*2] = {'0'};  
      memset((char *)&szPath, 0, EFS_FILE_NAME_LEN*2);
      Utf8ToUnicode((byte *)&cmd_ptr->data_ptr->fs_info.szDir, STRLEN(cmd_ptr->data_ptr->fs_info.szDir), (uint16 *)&szPath, EFS_FILE_NAME_LEN*2);
      cmd_ptr->data_ptr->fs_info.efs2.nErr = MMD_FILE_Delete((uint8 *)&szPath);
      status = VC_STAT_DONE;
   }
      break;

   case VC_ITM_FS_RENAME_I:
   {
      char szPath[EFS_FILE_NAME_LEN*2] = {'0'};
      char szNewPath[EFS_FILE_NAME_LEN*2] = {'0'}; // the name to be renamed
      memset((char *)&szPath, 0, EFS_FILE_NAME_LEN*2);
      memset((char *)&szNewPath, 0, EFS_FILE_NAME_LEN*2);
      Utf8ToUnicode((byte *)&cmd_ptr->data_ptr->fs_info.szDir, STRLEN(cmd_ptr->data_ptr->fs_info.szDir), (uint16 *)&szPath, EFS_FILE_NAME_LEN*2);
      Utf8ToUnicode((byte *)&cmd_ptr->data_ptr->fs_info.szNewDir, STRLEN(cmd_ptr->data_ptr->fs_info.szNewDir), (uint16 *)&szNewPath, EFS_FILE_NAME_LEN*2);
      cmd_ptr->data_ptr->fs_info.efs2.nErr = MMD_FILE_Rename((uint8 *)&szPath, (uint8 *)&szNewPath);
      status = VC_STAT_DONE;
   }
      break;

   case VC_ITM_FS_GET_ATTR_I:
    {
      char szPath[EFS_FILE_NAME_LEN*2] = {'0'};  
      memset((char *)&szPath, 0, EFS_FILE_NAME_LEN*2);
      Utf8ToUnicode((byte *)&cmd_ptr->data_ptr->fs_info.szDir, STRLEN(cmd_ptr->data_ptr->fs_info.szDir), (uint16 *)&szPath, EFS_FILE_NAME_LEN*2);
      cmd_ptr->data_ptr->fs_info.efs2.nErr = MMD_FILE_GetAttr((uint8 *)&szPath, (uint8 *)&cmd_ptr->data_ptr->fs_info.efs2.attrib);
      MSG_ERROR("MMD_FILE_GetAttr return nRet:%d", cmd_ptr->data_ptr->fs_info.efs2.nErr, 0, 0);
      
      if(cmd_ptr->data_ptr->fs_info.efs2.nErr == SUCCESS)
      {
         cmd_ptr->data_ptr->fs_info.efs2.attrib = vc_attr2efs(cmd_ptr->data_ptr->fs_info.efs2.attrib);
      }
      status = VC_STAT_DONE;
   }
      break;    

   case VC_ITM_FS_SEEK_I:
   {
      F_FILE * pFile = NULL;
      pFile = (F_FILE *)cmd_ptr->data_ptr->fs_info.efs2.handle;
      cmd_ptr->data_ptr->fs_info.efs2.nErr = MMD_FILE_Seek(pFile, (MMD_U32)cmd_ptr->data_ptr->fs_info.nOffset, (MMD_U08)cmd_ptr->data_ptr->fs_info.nSeekMode);
      status = VC_STAT_DONE;
   }
      break;

   case VC_ITM_FS_SEEK_APPEND_I:
   {
      F_FILE * pFile = NULL;
      uint8 * pBuf = NULL;
      pBuf = MALLOC(cmd_ptr->data_ptr->fs_info.nOffset);
      if(pBuf)
      {
         memset(pBuf, 0x30, cmd_ptr->data_ptr->fs_info.nOffset);
         pFile = (F_FILE *)cmd_ptr->data_ptr->fs_info.efs2.handle;
         cmd_ptr->data_ptr->fs_info.efs2.nErr = MMD_FILE_Seek(pFile, 0, VC_F_SEEK_END);
         cmd_ptr->data_ptr->fs_info.efs2.nErr = MMD_FILE_Write(pFile, pBuf, cmd_ptr->data_ptr->fs_info.nOffset, (MMD_U32 *)&cmd_ptr->data_ptr->fs_info.efs2.dwSize);
         free(pBuf);
         pBuf = NULL;
      }
      status = VC_STAT_DONE;
   }
      break;

   case VC_ITM_FS_TRUNCATE_I:
   {
      F_FILE *pFile = NULL;
      pFile = (F_FILE *)cmd_ptr->data_ptr->fs_info.efs2.handle;
      cmd_ptr->data_ptr->fs_info.efs2.nErr = MMD_FILE_SetEOF(pFile, cmd_ptr->data_ptr->fs_info.nOffset);
      status = VC_STAT_DONE;
   }
      break;

   case VC_ITM_FS_GET_INFO_I:
      //MMD_FILE_GetAttr((char *)&cmd_ptr->data_ptr->fs_info.szDir, *(cmd_ptr->data_ptr->fs_info.efs2.attrib));
      // here, we need CreateDate and dwSize info for OEMFS
      // ...
      status = VC_STAT_DONE;
      break;

   case VC_ITM_FS_OPEN_I:
   {
      F_FILE *pFile = NULL;
      char * szPath = NULL; //[EFS_FILE_NAME_LEN] = {'0'};
      szPath = (char *)MALLOC(EFS_FILE_NAME_LEN*2);
      if(szPath == NULL)
      {
        cmd_ptr->data_ptr->fs_info.efs2.nErr = EFAILED;
        return (status = VC_STAT_DONE);
      }
      memset(szPath, 0, EFS_FILE_NAME_LEN*2);
      Utf8ToUnicode((byte *)&cmd_ptr->data_ptr->fs_info.szDir, STRLEN(cmd_ptr->data_ptr->fs_info.szDir), (uint16 *)szPath, EFS_FILE_NAME_LEN*2);
      cmd_ptr->data_ptr->fs_info.efs2.nErr = MMD_FILE_Open((uint8 *)szPath, (uint8 *)&cmd_ptr->data_ptr->fs_info.nMode, &pFile);
      cmd_ptr->data_ptr->fs_info.efs2.handle = (uint32)pFile;
      FREEIF(szPath);
      status = VC_STAT_DONE;
   }
      break;

   case VC_ITM_FS_CLOSE_I:
   {
      F_FILE * pFile = NULL;
      pFile = (F_FILE *)cmd_ptr->data_ptr->fs_info.efs2.handle;
      cmd_ptr->data_ptr->fs_info.efs2.nErr = MMD_FILE_Close(pFile);
      MSG_ERROR("F_FILE *pFile CLOSE_I address:0x%08x, handle:0x%08x, nErr:%d", &pFile, cmd_ptr->data_ptr->fs_info.efs2.handle, cmd_ptr->data_ptr->fs_info.efs2.nErr);
      status = VC_STAT_DONE;
   }
      break;

   case VC_ITM_FS_READ_I:
   {
       F_FILE * pFile = NULL;
       pFile = (F_FILE *)cmd_ptr->data_ptr->fs_info.efs2.handle;
       
       cmd_ptr->data_ptr->fs_info.efs2.nErr = MMD_FILE_Read(pFile, (uint8 *)cmd_ptr->data_ptr->fs_info.efs2.buffer,
                      (MMD_U32)cmd_ptr->data_ptr->fs_info.efs2.dwReadWrite, (MMD_U32 *)&cmd_ptr->data_ptr->fs_info.efs2.dwSize);
       status = VC_STAT_DONE;
   }
      break;

   case VC_ITM_FS_WRITE_I:
   {
       F_FILE * pFile = NULL;
       pFile = (F_FILE *)cmd_ptr->data_ptr->fs_info.efs2.handle;
       cmd_ptr->data_ptr->fs_info.efs2.nErr = MMD_FILE_Write(pFile, (uint8 *)cmd_ptr->data_ptr->fs_info.efs2.buffer, \
          (MMD_U32)cmd_ptr->data_ptr->fs_info.efs2.dwReadWrite, (MMD_U32 *)&cmd_ptr->data_ptr->fs_info.efs2.dwSize);
       status = VC_STAT_DONE;
   }
      break;

   case VC_ITM_FS_SPACE_I:
      cmd_ptr->data_ptr->fs_info.efs2.nErr = MMD_FILE_GetSpace((F_SPACE *)&cmd_ptr->data_ptr->fs_info.efs2.space);
      status = VC_STAT_DONE;
      break;

   case VC_ITM_FS_TELL_I:
   {
      F_FILE * pFile = NULL;
      pFile = (F_FILE *)cmd_ptr->data_ptr->fs_info.efs2.handle;
      cmd_ptr->data_ptr->fs_info.efs2.nErr = MMD_FILE_Tell(pFile, (MMD_U32 *)&cmd_ptr->data_ptr->fs_info.efs2.dwSize);
      status = VC_STAT_DONE;
   }
      break;

   case VC_ITM_FS_GET_OPENFILE_SIZE_I:
   {
      F_FILE * pFile = NULL;
      uint32 nRestore, nStart, nEnd;
      pFile = (F_FILE *)cmd_ptr->data_ptr->fs_info.efs2.handle;
      cmd_ptr->data_ptr->fs_info.efs2.nErr = MMD_FILE_Tell(pFile, (MMD_U32 *)&cmd_ptr->data_ptr->fs_info.efs2.dwSize);
      nRestore = cmd_ptr->data_ptr->fs_info.efs2.dwSize;
      //MSG_ERROR("GET_OPENFILE_SIZE, nErr:%d, nRestore:%d", cmd_ptr->data_ptr->fs_info.efs2.nErr, nRestore, 0);
      cmd_ptr->data_ptr->fs_info.efs2.nErr = MMD_FILE_Seek(pFile, 0, VC_F_SEEK_END);
      cmd_ptr->data_ptr->fs_info.efs2.nErr = MMD_FILE_Tell(pFile, (MMD_U32 *)&cmd_ptr->data_ptr->fs_info.efs2.dwSize);
      nEnd = cmd_ptr->data_ptr->fs_info.efs2.dwSize;
      //MSG_ERROR("GET_OPENFILE_SIZE, nErr:%d, nEnd:%d", cmd_ptr->data_ptr->fs_info.efs2.nErr, nEnd, 0);

      cmd_ptr->data_ptr->fs_info.efs2.nErr = MMD_FILE_Seek(pFile, 0, VC_F_SEEK_SET);
      cmd_ptr->data_ptr->fs_info.efs2.nErr = MMD_FILE_Tell(pFile, (MMD_U32 *)&cmd_ptr->data_ptr->fs_info.efs2.dwSize);
      nStart = cmd_ptr->data_ptr->fs_info.efs2.dwSize;
      MSG_ERROR("GET_OPENFILE_SIZE, nErr:%d, nStart:%d", cmd_ptr->data_ptr->fs_info.efs2.nErr, nStart, 0);

      //MSG_ERROR("nRestore:0x%08x, nStart:0x%08x, nEnd:0x%08x", nRestore, nStart, nEnd);
      cmd_ptr->data_ptr->fs_info.efs2.nErr = MMD_FILE_Seek(pFile, nRestore, VC_F_SEEK_SET);
      cmd_ptr->data_ptr->fs_info.efs2.dwSize = nEnd - nStart;
      status = VC_STAT_DONE;
   }
      break;

   case VC_ITM_FS_GET_DATETIME_I:
   {
      MMD_DATE nDate;
      MMD_TIME nTime;
      JulianType julian;
      char szPath[EFS_FILE_NAME_LEN*2] = {'0'};
      memset((char *)&szPath, 0, EFS_FILE_NAME_LEN*2);
      Utf8ToUnicode((byte *)&cmd_ptr->data_ptr->fs_info.szDir, STRLEN(cmd_ptr->data_ptr->fs_info.szDir), (uint16 *)&szPath, EFS_FILE_NAME_LEN*2);
      cmd_ptr->data_ptr->fs_info.efs2.nErr = MMD_FILE_GetTimeDate((uint8 *)&szPath, &nDate, &nTime);
      //DBGPRINTF("GET_DATETIME, nTime.hour%d, .minute%d, .second:%d", nTime.hour, nTime.minute, nTime.second);
      julian.wYear = nDate.year;
      julian.wMonth = nDate.month;
      julian.wDay = nDate.day;
      julian.wHour = nTime.hour;
      julian.wMinute = nTime.minute;
      julian.wSecond = nTime.second;
      cmd_ptr->data_ptr->fs_info.efs2.dwCreationDate = JULIANTOSECONDS(&julian);
      status = VC_STAT_DONE;
   }
      break;
      
   default:
      status = VC_STAT_FAIL;
      break;
   }
   return status;
}

LOCAL char vc_attr2efs(uint8 attr)
{
   char attrib = EFS2_FA_NORMAL; 
   
   if(attr & MMD_FILE_ATTR_NORMAL)
      attrib |= EFS2_FA_NORMAL;
   
   if(attr & MMD_FILE_ATTR_HIDDEN)
      attrib |= EFS2_FA_HIDDEN;
   
   if(attr & MMD_FILE_ATTR_DIR)
      attrib |= EFS2_FA_DIR;
   
   if(attr & MMD_FILE_ATTR_READONLY)
      attrib |= EFS2_FA_READONLY;
   
   if(attr & MMD_FILE_ATTR_SYSTEM)
      attrib |= EFS2_FA_SYSTEM;
   
   if(attr & MMD_FILE_ATTR_VOLUME)
      attrib |= EFS2_FA_CONST;
   
   if(attr & MMD_FILE_ATTR_ARC)
      attrib |= EFS2_FA_FIXED;
   
   return attrib;
}

/* This func is to get the current date and detail time for saving file */
LOCAL boolean vc_get_date2filename(char * pszDest, int nSize, unsigned int fileType)
{
   JulianType julian;
   if(!pszDest || nSize < MIN_FILE_NAME_LEN) return FALSE;

   GETJULIANDATE(GETTIMESECONDS(), &julian);
   switch(fileType)
   {
   default:
   case VIM_FILE_TYPE_JPG:
      SPRINTF(pszDest, "%04d%02d%02d%02d%02d%02d.jpg", julian.wYear, julian.wMonth, julian.wDay, julian.wHour, julian.wMinute, julian.wSecond);
      break;

   case VIM_FILE_TYPE_AMR:
      SPRINTF(pszDest, "%04d%02d%02d%02d%02d%02d.amr", julian.wYear, julian.wMonth, julian.wDay, julian.wHour, julian.wMinute, julian.wSecond);
       break;
	   
   case VIM_FILE_TYPE_3GP:
      SPRINTF(pszDest, "%04d%02d%02d%02d%02d%02d.3gp", julian.wYear, julian.wMonth, julian.wDay, julian.wHour, julian.wMinute, julian.wSecond);
      break;
   }
   return TRUE;
}


LOCAL void vc_file_close(void)
{
   if(pFileHandle)
   {
      vc_file_open_type file_type = vc_get_curr_file_type();
      switch(file_type)
      {
      default: 
      case VC_FILE_OPEN_PLY:
         MMD_Player_File_Close(pFileHandle, NULL, NULL);
         break;

      case VC_FILE_OPEN_REC:
         MMD_Recorder_File_Close(NULL, pFileHandle);
         break;
      }
      pFileHandle = NULL;
      vc_set_curr_file_type(VC_FILE_OPEN_NONE);
   }
}

LOCAL void vc_set_curr_file_type(vc_file_open_type nOpenType)
{
   if(nOpenType >= VC_FILE_OPEN_NONE && nOpenType <  VC_FILE_OPEN_MAX)
   {
      g_file_open = nOpenType;
   }
}

LOCAL vc_file_open_type vc_get_curr_file_type(void)
{
   return g_file_open;
}

boolean UnicodeToUtf8(const uint16 * pSrc,int nLen, byte * pDst, int nSize)
{
   int      i;
   int      nBytes = 0;
   uint16   wChar;

   if(!pSrc || !pDst || nSize <= 0)
      return FALSE;

   for (i = 0; i < nLen; ++i) {

      wChar = pSrc[i]; 

      if (wChar < 0x80) {
         ++nBytes;
         if (nBytes > nSize)
            break;
         *pDst++ = (byte)(wChar & 0x7F);
      }
      else {
         if(wChar < 0x0800) {
            nBytes += 2;
            if (nBytes > nSize)
               break;
            *pDst++ = (byte)((wChar >> 6) & 0x1F) | 0xC0;
            *pDst++ = (byte)(wChar & 0x3F) | 0x80;
         }
         else {
            nBytes += 3;
            if (nBytes > nSize)
               break;
            *pDst++ = (byte)((wChar >> 12) & 0x0F) | 0xE0;
            *pDst++ = (byte)((wChar >> 6) & 0x3F) | 0x80;
            *pDst++ = (byte)(wChar & 0x3F) | 0x80;
         }
      }
   }
   return(TRUE);
}

LOCAL boolean Utf8ToUnicode(const byte * pSrc,int nLen, uint16 * pDst, int nSize)
{
   byte    b;
   uint16  wChar;

   if(!pSrc || !pDst || nSize <= 0)
      return FALSE;

   if (nLen > 3 && pSrc[0] == UTF8_B0 && pSrc[1] == UTF8_B1 && pSrc[2] == UTF8_B2){
      nLen -= 3;
      pSrc += 3;
   }

   while (nLen > 0) {

      b = *pSrc++; 

      if (b & 0x80) {
         if (b & 0x40) {
            if (b & 0x20) {
               wChar = (unsigned short)(b&0x0F);
               b = *pSrc++;
               if ((b & 0xC0) != 0x80) 
                  return(FALSE);
               wChar = ((wChar << 6)|(b & 0x3F)) & 0xffff;
               b = *pSrc++;
               if ((b & 0xC0) != 0x80)
                  return(FALSE);
               wChar = ((wChar << 6)|(b & 0x3F)) & 0xffff;
               nLen -= 3;
            }
            else {
               wChar = (unsigned short)(b & 0x1F);
               b = *pSrc++;
               if ((b & 0xc0) != 0x80) 
                  return(FALSE);
               wChar = ((wChar << 6)|( b & 0x3F)) & 0xffff;
               nLen -= 2;
            }
         }
         else 
            return(FALSE);
      }
      else {
         wChar = (unsigned short)b;
         --nLen;
      }

      if(nSize < (int)sizeof(AECHAR))
         return(FALSE);
 
      *pDst = wChar;
      pDst++;
      nSize -= (int)sizeof(AECHAR);
   }
   return(TRUE);
}

LOCAL vc_dev_pri_level vc_get_dev_pri()
{
   return g_dev_info.dev_pri;
}

// how to define the VC_DEV in the enum list?
// how to classify the priority for all the vc devices?
LOCAL vc_dev_pri_level vc_dev_convert_pri(vc_dev_type dev_type)
{
   switch(dev_type)
   {
   // if mp3 or fm_pa is in background play mode, no other device could be switched
   case VC_DEV_PLY_MP3:
   case VC_DEV_PLY_AAC:
   case VC_DEV_PLY_AMRAUDIO:	//    add by jian.wang 09.3.11
   case VC_DEV_FM_PA:
      return VC_DEV_PRI_LEV_5;

   // no matter what level the current device is, the same priority level could be switched with device stack
   case VC_DEV_CAMERA:
   case VC_DEV_PLY_AMR:   
      return VC_DEV_PRI_LEV_4;

   case VC_DEV_UDISK:
      return VC_DEV_PRI_LEV_3;

   case VC_DEV_JPEG:
      return VC_DEV_PRI_LEV_2; // note: jpeg decode device could be single device, or embeded in camera device
      
   // sd is the base device, if the first device is not sd, then sd could be pushed into stack
   // only the first device is sd, can sd device could be pushed into stack
   case VC_DEV_SD:
      return VC_DEV_PRI_LEV_1;

   case VC_DEV_NONE:   
   default:   
      return VC_DEV_PRI_LEV_0;
   }
}


/*===========================================================================
FUNCTION vc_dev_pop_stack

DESCRIPTION
  Pop the recent device from vc_dev_stack to curr_dev. curr_dev should be current
  running device in vc_task
  
DEPENDENCIES
  None

dev_type: input parm in vc_dev_type

Note: all dev_pri in vc_dev_info should be in VC_DEV_PRIORITY(x) type

SIDE EFFECTS
  None
==========================================================================*/
LOCAL boolean vc_dev_pop_stack(vc_dev_info * curr_dev)
{
   if(vc_dev_stack_idx <= 0 || !curr_dev)
   {
      return FALSE;
   }
   vc_dev_stack_idx -= 1;
   curr_dev->curr_dev = vc_dev_stack[vc_dev_stack_idx].curr_dev;
   curr_dev->dev_pri = vc_dev_stack[vc_dev_stack_idx].dev_pri;
   curr_dev->sub_item = vc_dev_stack[vc_dev_stack_idx].sub_item;
   return TRUE;
}


/*===========================================================================
FUNCTION vc_dev_push_stack

DESCRIPTION
  Push curr_dev in the vc_dev_stack, then set next_dev as curr_dev. cyrr_dev should be 
  current running device in vc_task.
  
DEPENDENCIES
  None

dev_type: input parm in vc_dev_type

Note: all dev_pri in vc_dev_info should be in VC_DEV_PRIORITY(x) type

SIDE EFFECTS
  None
==========================================================================*/
LOCAL boolean vc_dev_push_stack(vc_dev_info * curr_dev_info, vc_dev_info * next_dev)
{
   vc_dev_pri_level temp_pri;

   if(!curr_dev_info || !next_dev)
   {
      return FALSE;
   }
   
   if(vc_dev_stack_idx >= VC_DEV_STACK_MAX - 1)
   {
      ERR_FATAL("VC Stack Overflow",0,0,0);
      
      vc_set_device_state(VC_STAT_FAIL);
      return FALSE;
   }

   if(vc_dev_validate_for_push_stack(next_dev->curr_dev) == FALSE)
   {
      //MSG_ERROR("vc_dev_validate_for_push_stack failed", 0, 0, 0);
      vc_set_device_state(VC_STAT_INUSE);
      return FALSE;
   }
   
   temp_pri = vc_dev_convert_pri(next_dev->curr_dev);
   if(temp_pri < (VC_DEV_STACK_MAX - 1))
   {
      vc_dev_stack[vc_dev_stack_idx].curr_dev = curr_dev_info->curr_dev;
      vc_dev_stack[vc_dev_stack_idx].dev_pri = curr_dev_info->dev_pri;
      vc_dev_stack[vc_dev_stack_idx].sub_item = curr_dev_info->sub_item;
      // if there's VC_DEV_NONE in the stack, then we should clear all the other device in stack before this NONE device
      // ...
      vc_dev_stack_idx++;
      curr_dev_info->curr_dev = next_dev->curr_dev;
      curr_dev_info->dev_pri = temp_pri;
      curr_dev_info->sub_item = next_dev->sub_item;
      return TRUE;
   }
   else
   {
      ERR_FATAL("VC Device Priority Error",0,0,0);
      vc_set_device_state(VC_STAT_FAIL);
      return FALSE;
   }
}

/*===========================================================================
FUNCTION vc_dev_validate_for_push_stack

DESCRIPTION
  Verifies the validity values entered vc_dev_stack. 
  If the priority is great than current running device, validate will return TRUE,
  else, return FALSE

DEPENDENCIES
  None

dev_type: input parm in vc_dev_type

Note: all dev_pri in vc_dev_info should be in VC_DEV_PRIORITY(x) type
  1. if current device priority is higher than udisk, forbid switching device to udisk
  2. if current device priority is lower than udisk, allow switching device to udisk
  3. if current device is udisk, forbid switching device to other devices
  4. allow the device switching among the same priority, but mp3 and fmpa
  5. if current device is sd, forbid push sd into stack
SIDE EFFECTS
  None
==========================================================================*/
LOCAL boolean vc_dev_validate_for_push_stack(vc_dev_type dev_type)
{
   vc_dev_pri_level pri_level;
   pri_level = vc_dev_convert_pri(dev_type);
   
   // lower priority can not be prior than higher priority, if the same priority, allows to switch device
   if(pri_level < g_dev_info.dev_pri)
   {
      // but VC_DEV_JPEG should be allowed to switch device.
      if(dev_type == VC_DEV_JPEG)
         return TRUE;
      
      //FARF(TRACE, "priority <= current device, can not be pushed to vc_dev_stack");
      return FALSE;
   }

   // but if udisk is in use, other higher priority device can not occupy the current device too.
   // this means udisk can not be pushed into stack.
   if(g_dev_info.curr_dev == VC_DEV_UDISK)
   {
      return FALSE;
   }

   // if current device and next device are the same priority level of MP3 and FM, forbid push stack request.
   //if(g_dev_info.dev_pri == pri_level && pri_level == VC_DEV_PRI_LEV_5)
   if(g_dev_info.dev_pri == pri_level && (dev_type == g_dev_info.curr_dev || pri_level == VC_DEV_PRI_LEV_5))
   {
      return FALSE;
   }
   
   return TRUE;
}

LOCAL void vc_set_device_state(vc_status_type new_stat)
{
   vc_dev_stat = new_stat;
}

LOCAL void vc_switch_mode_codec(vc_mode_codec_type modcodec)
{
    //if(mode_codec.scene != modcodec.scene || mode_codec.codec != modcodec.codec)
    {
        MMD_SYS_Mode_Switch(modcodec.scene);                             
        MMD_SYS_Codec_Switch(modcodec.codec);
        //mode_codec.scene = modcodec.scene;
        //mode_codec.codec = modcodec.codec;
    }
}

LOCAL vc_dev_type vc_dev_scene_switch_on(vc_dev_type dev_type)
{
   db_items_value_type  db_item;  //add by xinggag.xu 20090109
   vc_dev_type nDeviceType = dev_type;
   vc_mode_codec_type mod; 
   
   switch(dev_type)
   {
   case VC_DEV_CAMERA:
      db_get(DB_CAMERA_MODE,&db_item);
      switch(db_item.nCamMode)
      {            
         case CAM_MODE_MOVIE:               
            MSG_FATAL("set the speaker pa ", 0, 0, 0);
            mod.scene = SCENE_REC_AMR;
            mod.codec = CODEC_AMR;
            vc_switch_mode_codec(mod);
            MSG_FATAL("vc_switch_mode_codec CAM_MODE_MOVIE",0,0,0);
            audio_speaker_ctrl(1);
            break;
         
         case CAM_MODE_SNAPSHOT:
         default:
             mod.scene = SCENE_MEDIAPLAY;
             mod.codec = CODEC_PCM;
             vc_switch_mode_codec(mod);
             MSG_FATAL("vc_switch_mode_codec CAM_MODE_SNAPSHOT",0,0,0);
            //MMD_SYS_Mode_Switch(SCENE_MEDIAPLAY);
            //MMD_SYS_Codec_Switch(CODEC_PCM);
            MMD_LCD_BLY_Release();
            break;         
      }
      // now, not detect sensor for camera ui because ICamera_New will failed if no sensor exists
      break;
         
   case VC_DEV_PLY_MP3: // while IMedia_new, init hardware first, if play, set another sig to vc_task
      //MMD_LCD_BLY_Release();
      mod.scene = SCENE_MEDIAPLAY;
      mod.codec = CODEC_MP3;
      vc_switch_mode_codec(mod);
      MMD_USE_FM_Flag(0);
      
      //MMD_SYS_Mode_Switch(SCENE_MEDIAPLAY);
      //MMD_SYS_Codec_Switch(CODEC_MP3);
      MMD_LCD_BLY_Release();

      if(g_fs_pos == VC_FSPOS_SD) // set fs_pos first or default play media files on sd
      {
         (void)MMD_FILE_Mount();
      }
      break;

   case VC_DEV_PLY_AAC: // while IMedia_new, init hardware first, if play, set another sig to vc_task     add by jian.wang 09.3.20
      //MMD_LCD_BLY_Release();
      mod.scene = SCENE_MEDIAPLAY;
      mod.codec = CODEC_AAC;
      vc_switch_mode_codec(mod);
      MMD_USE_FM_Flag(0);//zhangxiang add
      
      MMD_LCD_BLY_Release();

      if(g_fs_pos == VC_FSPOS_SD) // set fs_pos first or default play media files on sd
      {
         (void)MMD_FILE_Mount();
      }
      break;

   case VC_DEV_PLY_AMRAUDIO: // while IMedia_new, init hardware first, if play, set another sig to vc_task
      //MMD_LCD_BLY_Release();
      mod.scene = SCENE_MEDIAPLAY;
      mod.codec = CODEC_AMR;
      vc_switch_mode_codec(mod);
      MMD_USE_FM_Flag(0);//zhangxiang add
      
      //MMD_SYS_Mode_Switch(SCENE_MEDIAPLAY);
      //MMD_SYS_Codec_Switch(CODEC_MP3);
      MMD_LCD_BLY_Release();

      if(g_fs_pos == VC_FSPOS_SD) // set fs_pos first or default play media files on sd
      {
         (void)MMD_FILE_Mount();
      }
      break;

   case VC_DEV_PLY_AMR:
      mod.scene = SCENE_MEDIAPLAY;
      mod.codec = CODEC_AMR;
      vc_switch_mode_codec(mod);
      //MMD_SYS_Mode_Switch(SCENE_MEDIAPLAY);
      //MMD_SYS_Codec_Switch(CODEC_AMR);
      MMD_USE_FM_Flag(0);//zhangxiang add
      
      MMD_LCD_BLY_Release();
      
      if(g_dev_info.sub_item == VC_ITM_REC_AUDIO_I) // sub_item is audio amr
      {
         media_work_mode_initialize(MMD_MEDIA_MODE_PLAY_AUDIO_ONLY);
      }
      else // other case: sub_dev is mp4 or 3gp
      {
         media_work_mode_initialize(MMD_MEDIA_MODE_PLAY_AUDIO_AND_VIDEO);
      }

      if(g_fs_pos == VC_FSPOS_SD) // set fs_pos first or default play media files on sd
      {
         (void)MMD_FILE_Mount();
      }
      break;
      
   case VC_DEV_FM_PA:
      mod.scene = SCENE_MEDIAPLAY;
      mod.codec = CODEC_PCM;
      vc_switch_mode_codec(mod);
      //MMD_SYS_Mode_Switch(SCENE_MEDIAPLAY);
      //MMD_SYS_Codec_Switch(CODEC_PCM);
      MMD_USE_FM_Flag(1);
      //MMD_Media_Codec_Set_MuxMode(0x01010101); // set pa channel
      Internal_Codec_ADC_Volume(20,20);
      MMD_Media_AudioCodec_FM_Mode(44100, MMD_AUDIOCODEC_FM_MIX_EN);
      break;
      
   case VC_DEV_SD:
      VC_ACQUIRE_DISPLAY();
      disppriv_set_window(0,0,MMD_LCD_WIDTH, MMD_LCD_HEIGHT);
      
      if(vc_get_dev_pmu_mode() != PMU_MODE_NORMAL)
      {
          MMD_SYS_PMU_SWITCH(PMU_MODE_NORMAL);
          KICK_WATCHDOG();
          vc_set_dev_pmu_mode(PMU_MODE_NORMAL);
          MMD_USER_WaitMICRS(150);
          SetVCBypassMode(FALSE);
      }
      VC_RELEASE_DISPLAY();
      
      mod.scene = SCENE_MEDIAPLAY;
      mod.codec = CODEC_PCM;
      vc_switch_mode_codec(mod);
      //MMD_SYS_Mode_Switch(SCENE_MEDIAPLAY);
      //MMD_SYS_Codec_Switch(CODEC_PCM);

      if(MMD_FILE_Mount())
      {
         nDeviceType = VC_DEV_NONE;
      }
      break;

   case VC_DEV_UDISK:
      {
        USB_MASS_INFO usb_info;
        
        VC_GPIO_CTRL(51, HI); // VCORE_1V2 enable
   
        // now, we need enable vc main clock
        VC_GPIO_CTRL(43, HI); // set usb_sel high to enable UDisk func
        
        mod.scene = SCENE_USB;
        mod.codec = CODEC_UNCHANGED;
        vc_switch_mode_codec(mod);
        //MMD_SYS_Mode_Switch(SCENE_USB);
   
        usb_info.device =0;
        usb_info.speed =0;
        MMD_USB_Enable(USB_MASS_STORAGE, NULL);
        MMD_USER_WaitMSEC(100);
   
        MMD_USB_SetProperty(USB_MASS_STORAGE, &usb_info);
        MMD_USER_WaitMSEC(100);
      }
      break;

   case VC_DEV_JPEG:
      MMD_SYS_Mode_Switch(SCENE_MEDIAPLAY);
      MMD_SYS_Codec_Switch(CODEC_PCM);
       
      //MMD_LCD_BLY_Release();
      MMD_Media_Mode_Exit(MMD_MEDIA_MODE_CAPTURE);
      MMD_Media_Mode_Initialzie(MMD_MEDIA_MODE_PLAY_AUDIO_AND_VIDEO);
      break;    

   case VC_DEV_NONE:
      break;
      
   default:
      VC_SetDeviceAsyncMode(FALSE); // restore async mode as FALSE while no device is in working.
      //dev_type = VC_DEV_NONE;
   } /* switch(dev_type) */
   
   return nDeviceType;
}

LOCAL boolean vc_dev_scene_switch_off(vc_dev_type dev_type)
{
   switch(dev_type)
   {
   case VC_DEV_CAMERA:
      VC_GPIO_CTRL(42, LO); // enable sensor power
      
   case VC_DEV_PLY_MP3:
   case VC_DEV_PLY_AAC: //   add by jian.wang 09.3.20	
   case VC_DEV_PLY_AMRAUDIO:
   case VC_DEV_PLY_AMR:
      audio_speaker_ctrl(0);
      //MMD_LCD_BLY_Release();
      vc_file_close();
      break;

   case VC_DEV_FM_PA:
      MMD_USE_FM_Flag(0);
      MMD_LCD_BLY_Release();
      break;

   case VC_DEV_JPEG:
   case VC_DEV_SD:
      MMD_LCD_BLY_Release();
      break;

   case VC_DEV_UDISK:
      MMD_USB_Disable(USB_MASS_STORAGE); 
      VC_GPIO_CTRL(43, LO); // set usb_sel low to disable UDisk func
      break;

   default:
      return FALSE;
   }
   return TRUE;
}

LOCAL void vc_set_media_volume(uint8 vol)
{
   media_vol.p1 = vol; 
}

LOCAL int vc_get_media_volume(void)
{
   return media_vol.p1;
}

LOCAL void vc_pmu_switch_bypass(void)
{
   VC_ACQUIRE_DISPLAY();
   MMD_SYS_PMU_SWITCH(PMU_MODE_BYPASS);
   KICK_WATCHDOG();
   SetVCBypassMode(TRUE);
   vc_set_dev_pmu_mode(PMU_MODE_BYPASS);
   MMD_USER_WaitMICRS(50);
   VC_RELEASE_DISPLAY();
   VC_SLEEP_ALLOW();
   vc_dev_stack_idx = 0;
   memset(&g_dev_info, 0, sizeof(g_dev_info));
   memset(&vc_dev_stack, 0, sizeof(vc_dev_stack));
}

LOCAL MMD_PMU_MODE vc_get_dev_pmu_mode()
{
   return g_dev_mode;
}

LOCAL void vc_set_dev_pmu_mode(MMD_PMU_MODE mode)
{
   if(mode >= PMU_MODE_NORMAL && mode < PMU_MODE_NUM)
   {
      g_dev_mode = mode;
   }
}

vc_dev_type VC_GetCurrentDevice(void)
{
   return g_dev_info.curr_dev;
}

LOCAL void vc_set_pmu_flag_run_first_time(boolean flg)
{
   g_run_first_time = flg;
}

LOCAL boolean vc_get_pmu_switch_bypass_ready(void)
{
   return !g_run_first_time;
}

LOCAL void vc_set_camera_init_parm(vc_prev_parm_type parm)
{
   static vc_preview_type prev_type = VC_PREV_NONE;
   if(prev_type == parm.prev_type)
   {
      MSG_FATAL("VC_PREV_SNAPSHOT return", 0, 0, 0);
      //return; // same preview mode, do not re-init and return directly
   }
   MSG_FATAL("vc_set_camera_init_parm=%d", parm.prev_type , 0, 0);
   if(parm.prev_type == VC_PREV_SNAPSHOT)
   {
      MSG_FATAL("VC_PREV_SNAPSHOT preview=%d", parm.quality, 0, 0);
      prev_type = parm.prev_type;
      media_work_mode_exit(VIM_MEDIA_MODE_RECORD_AUDIO_AND_VIDEO);
      media_work_mode_initialize(VIM_MEDIA_MODE_CAPTURE);
      /*0 means preview with not frame,1 means preview with frame*/
      MMD_Media_Set_Capture_FrameMode( parm.prev_frametype); 
      MSG_FATAL("parm.prev_frametype = %d,%d,%d", parm.prev_frametype, parm.frame.cx, parm.frame.cy);
      MMD_Media_Set_Capture_Resolution(parm.frame.cx, parm.frame.cy);
      MMD_Media_Set_Capture_Quality(parm.quality);
      #if 1
      if(parm.frame.cx == CAMERA_PREVIEW_WIDTH)
      {
         MMD_Media_Set_Thumbnail_Resolution(0,0);
      }
      else
      {
        MMD_Media_Set_Thumbnail_Resolution(CAMERA_PREVIEW_WIDTH,CAMERA_PREVIEW_HEIGHT); //zhangxiang add 09/02/26
      }
      #endif
   }
   else if(parm.prev_type == VC_PREV_MOVIE)// preview mode type is MOVIE
   {
      prev_type = parm.prev_type;
      MMD_Media_Set_Thumbnail_Resolution(0,0);
      media_work_mode_exit(VIM_MEDIA_MODE_CAPTURE);
      media_work_mode_initialize(VIM_MEDIA_MODE_RECORD_AUDIO_AND_VIDEO);
      MMD_Media_Set_Video_Resolution(parm.frame.cx, parm.frame.cy );
   }
}

LOCAL vc_timer_state vc_get_timer_state(void)
{
    return vc_timer;
}

void VC_SetTimerSig(unsigned long sig)
{
   (void)rex_set_sigs(&vc_tcb, sig);
}

vc_play_mode VC_GetPlayMode(void)
{
    return g_play_mode;
}

void VC_SetPlayMode(vc_play_mode mode)
{
    g_play_mode = mode;
}

void VC_SetTimerState(vc_timer_state timer_state)
{
    vc_timer = timer_state;
}

LOCAL uint32 vc_datetime_to_second(uint32 nDate, uint32 nTime)
{
    JulianType julian;
    
    uint32 unSecond = 0;
    julian.wDay = (nDate & F_CDATE_DAY_MASK);
    julian.wMonth = ((nDate & F_CDATE_MONTH_MASK) >> F_CDATE_MONTH_SHIFT);
    julian.wYear= ((nDate & F_CDATE_YEAR_MASK) >> F_CDATE_YEAR_SHIFT) + 1980 ;

    julian.wSecond = ((nTime & F_CTIME_SEC_MASK) << 1);
    julian.wMinute = ((nTime & F_CTIME_MIN_MASK) >> F_CTIME_MIN_SHIFT);
    julian.wHour = ((nTime & F_CTIME_HOUR_MASK) >> F_CTIME_HOUR_SHIFT) ;
    
    unSecond = JULIANTOSECONDS(&julian);
    return unSecond;
}

LOCAL void vc_video_recorder_cb(unsigned int command, unsigned int param1, unsigned int param2)
{
    switch(command)
    {
        case MMD_REC_FILE_END:
            MSG_ERROR("RecordVideoCallBack", 0, 0, 0);
            VC_SetVideoRecorderDoneState(TRUE);
            break;

        default:
            break;
    }
}

boolean VC_GetVideoRecorderDoneState(void)
{
    return vc_recorder_3gp_done; 
}

void VC_SetVideoRecorderDoneState(boolean bState)
{
    vc_recorder_3gp_done = bState;
}

LOCAL void vc_disp_update_vc0848(const void *buf_ptr, // source buffer point
                 signed short src_width,  
                 signed short src_starting_row, // start row on frame buffer
                 signed short src_starting_column, // start col on frame buffer
                 signed short num_of_rows, // width need to update
                 signed short num_of_columns, // column need to update
                 signed short dst_starting_row, // start row on screen
                 signed short dst_starting_column) // start col on screen
{
    static blayer_params b_property;
    db_items_value_type  db_item;  //TCL jinqiang add for Annunciator flash ,2008/05/29

#if 1    
#ifdef DISP_DEVICE_16BPP
    // 16bpp
   // uint16  *data = (uint16 *)buf_ptr;
#endif
#if 0    
#ifdef FEATURE_SCREEN_CAPTURE
    if ( disp_state_info.capture_on )
    {
       int16 i;
       uint16 *tmp_buf = data;
       uint16 *tmp_disp_capture_buf = (uint16 *)disp_capture_buf;
           
       tmp_disp_capture_buf += DISP_WIDTH*dst_starting_row+dst_starting_column;
       for (i=0; i<num_of_rows; i++) 
       {
          memcpy( tmp_disp_capture_buf, tmp_buf, num_of_columns*sizeof(uint16) );
          tmp_buf += src_width;
          tmp_disp_capture_buf += DISP_WIDTH;
       }
    }
#endif /* FEATURE_SCREEN_CAPTURE */
           
#endif
#endif  
    //ERR("num_of_rows=%d",num_of_rows,0,0);

    MMD_LCD_Init();
    //MMD_Set_LCDIFParm(MMD_HAPI_LCDPANEL_MAIN);
    db_get(DB_CAMERA_MODE, &db_item);
    //if(num_of_rows >= 150) num_of_rows = 80;
    //dst_starting_column =0; //zhangxiang del
   // num_of_columns = 176;  //zhangxiang del
    MSG_FATAL("dst_starting_column=%d,dst_starting_row=%d,num_of_columns=%d",dst_starting_column,dst_starting_row,num_of_columns);
    MSG_FATAL("num_of_rows=%d, db_item.nCamMode:%d",num_of_rows,db_item.nCamMode,0);
    switch(db_item.nCamMode)
    {
        case CAM_MODE_READY:
            //if(overlay==1)
            // {
             		MMD_LCD_SetOverlay_Flag(0);
             		
            // }
            MMD_LCD_Refresh(MMD_LCD_B0LAYER,dst_starting_column,dst_starting_row,num_of_columns,num_of_rows,(MMD_U08*)buf_ptr,0);                        
            break;
            
        case CAM_MODE_SNAPSHOT:
            {
                extern boolean refreshJPG;
                
                if(refreshJPG == FALSE)
                {                    
                  MMD_LCD_Refresh(MMD_LCD_B0LAYER,dst_starting_column,dst_starting_row,num_of_columns,num_of_rows,(MMD_U08*)buf_ptr,0);
                }
                else
                {
                  MMD_LCD_Refresh_BackJpeg(MMD_LCD_B0LAYER,dst_starting_column,dst_starting_row,num_of_columns,num_of_rows,(MMD_U08*)buf_ptr,0);
                }
            }         
            break;
        case CAM_MODE_DISPJPEG:
            MMD_LCD_Refresh(MMD_LCD_B0LAYER,dst_starting_column,dst_starting_row,num_of_columns,num_of_rows,(MMD_U08*)buf_ptr,0);
            break;
        case CAM_MODE_PREVIEW:   
            MMD_LCD_Refresh_ZX(MMD_LCD_B0LAYER,dst_starting_column,dst_starting_row,num_of_columns,num_of_rows,(MMD_U08*)buf_ptr);
            break;
            
        case CAM_MODE_MOVIE: 
            MMD_LCD_Refresh_Video(MMD_LCD_B0LAYER,0,160,176,60,(MMD_U08*)buf_ptr,0);
            break;
            
        default:
            if(VC_GetCurrentDevice() == VC_DEV_PLY_AMR && FullScreen == FALSE)
            {
    	        MMD_LCD_Refresh_MP4(MMD_LCD_B0LAYER,0,128,176,92,(MMD_U08*)buf_ptr,0);
            }
            else
            {
                MMD_LCD_Refresh(MMD_LCD_B0LAYER,dst_starting_column,dst_starting_row,num_of_columns,num_of_rows,(MMD_U08*)buf_ptr,0);
            }
            break;
    }
     // MMD_LCD_BLY_Release();
}

void VC_Sleep_Allow(void)
{
    VC_SLEEP_ALLOW();
}

void VC_Sleep_Forbid(void)
{
    VC_SLEEP_FORBID();
}

