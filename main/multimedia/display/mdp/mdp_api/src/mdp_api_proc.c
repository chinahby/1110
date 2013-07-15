/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  MDP Device Driver API Processor

  GENERAL DESCRIPTION
  This module contains functions and data blocks pertaining to the
  Mobile Display Processor (MDP) Driver API

  EXTERNALIZED FUNCTIONS

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

  Copyright (c) 2005 - 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*============================================================================

  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/display/mdp1/mdp_api/main/latest/src/mdp_api_proc.c#4 $ $DateTime: 2008/11/13 06:35:20 $ $Author: rkashyap $

when      who     what, where, why
--------  -----   ----------------------------------------------------------
10/31/08  rk      Corrected the image height calculation before passing to IPL
09/19/08  gs      Cache to be flushed only if image operation are enabled and 
                  compiler warning & Lint fixes
09/09/08  rk      Added fix for ULC Cache write back
11/07/07  dw      Add support for dynamic memory allocation
07/25/07  cjr     Add mdp_refresh_viewport_async() API
06/26/07  dw      Added support for display updates with powered-down lcd
06/05/07  cjr     Fixed mdp_map_dest() dest.width&height check
05/29/07  trg     Add fix for WM timer emulation.
05/29/07  cl      Fix mdp_proifiling_start to include mapping time reset.
04/26/07  cl      Fixed mdp_profiling_stop() to print the correct update times.
04/23/07  dw      Remove enabling of adaptive backlight within display driver.
03/30/07  trg     Add interface for qvga to wqvga scaling for 6k.  Use 
                  FEATURE_MDP_WSCALE to enable.
03/20/07  cl      Added MDP profiling support.
03/01/07  trg     Add compile flag to remove timetick_get_ms() for WM.
02/14/07  dw      Enable ABC support for testing within MDP drivers
02/01/07  trg     Modify verify_img to include cwin checks with scaling.
01/11/06  trg     Add Jinny's changes which combine all IPL calls in to one,
                  and handle odd pixel dimensions. 
01/03/06  cjr     Fixed mdp_get_mdpImg() to use node's orignal image
12/15/06  dw      Added support for adaptive backlight control
11/20/06  cjr     Fixed active dest verification under MDPOP_QUEUE_COPY
10/27/06  cjr     Warning removal
07/17/06  dw      Remove FEATURE_DISP_TASK
07/13/06  cjr     mdp_map_dest under FEATURE_MDP2 only
06/26/06  cjr     Added MDP_MAP_INFO under mdp_map_dest()
05/16/06  trg     Add MDP QXDM messages
05/10/06  cjr     Added support for display API queue
05/09/06  cjr     MDP1 mdp_register_ovimg() fix for layer1
04/05/06  cjr     MDP ascale max/min scale factor verification
03/27/06  cjr     MDP imglist critical section protection
03/13/06  cjr     Added ascale support for MDP1
02/23/06  cjr     Added mdp_refresh_viewport()
02/24/06  trg     Add XRGB support for MDP 1
02/22/06  trg     Added clear async mode (mdp error mode) function
01/04/06  cjr     Display Task bypass at the boot-up
12/19/05  trg     Changed ipl function call, save ~10% on update
10/13/05  cjr     Added MDPOP_QUEUE_EXPAND
09/21/05  trg     Added async fallout call to sync for error messaging
09/09/05  cjr     Added MDP dest-map
08/09/05  cjr     disp task & async API support
05/16/05  cjr     7500 MDP integration
02/10/05  cjr     Completed source file re-architect
02/09/05  cjr     Initial code
============================================================================ */

/*============================================================================
INCLUDE FILES FOR MODULE
============================================================================*/
#include <math.h>
#include "customer.h"

#if defined (FEATURE_MDP) || defined (FEATURE_MDP2)

  #include "mdp_drv.h"
  #include "mdp_buffers.h"
  #include "mdp_api_proc.h"
  #include "mdp_layer_proc.h"
  #include "mdp_sys_api.h"
  #include "rex.h"
  #include "armasm.h"
  
  #include "disp_task.h"
  #include "task.h"
  #include "msg.h"
  #include "ipl.h"
  
#if defined (FEATURE_MDP2) && defined (FEATURE_DISPLAY_ADAPTIVE_BACKLIGHT)
#error code not present
#endif /* FEATURE_MDP2 && FEATURE_DISPLAY_ADAPTIVE_BACKLIGHT */

/***************/
/* DEBUG FLAGS */
/***************/

boolean mdp_debug_calcFps = FALSE;    /* Do the FPS at runtime? */

/* rex task associated data */
extern rex_crit_sect_type   mdp_crit_sect;
extern uint32 mdp_drv_initialized;

  #ifndef FEATURE_MDP2
extern uint32 mdpWhiteBgOvId[MAX_LCD_NUM];
extern uint32 disp_imglist_total;
  #endif

#if defined (FEATURE_MDP2) && defined (FEATURE_DISPLAY_ADAPTIVE_BACKLIGHT)
#error code not present
#endif /* FEATURE_MDP2 && FEATURE_DISPLAY_ADAPTIVE_BACKLIGHT */


// BREW multi-application support
  #ifdef FEATURE_BREW
    #ifndef FEATURE_MDP2
      #define MDP_MAX_SYSCTX 8
void * mdpSysCtx[MDP_MAX_SYSCTX];
    #endif
  #endif

int mdp_active_dest[MAX_LCD_NUM];
boolean mdp_dest_power_on[MAX_LCD_NUM];

#ifdef FEATURE_DISPLAY_POWER_DEBUG
#error code not present
#endif /* FEATURE_DISPLAY_POWER_DEBUG */

#ifdef FEATURE_MDP2
#error code not present
#endif

// MDP ImgList Pool
MDPIMG_LIST mdpImgList[MDP_MAX_IMGLIST];

// All image list tail pointer
MDPIMG_LIST * mdpImgListTailPtr;
uint32 mdp_imglist_total;

MDPIMG_LIST * mdpImgTailPtr_1[MDP_MAX_LAYER];
MDPIMG_LIST * mdpImgTailPtr_2[MDP_MAX_LAYER];
uint32 mdp_list_total_1[MDP_MAX_LAYER];
uint32 mdp_list_total_2[MDP_MAX_LAYER];

  #ifdef FEATURE_MDP2
#error code not present
  #endif


// let's allocate 128 cb argument space
// this will ensure no overlapping between tasks
#define MDP_MAX_CB_ARG 128
MDP_CB_DATA mdp_cb_arg[MDP_MAX_CB_ARG];
MDP_CB mdp_cb;
static int mdp_cbarg_cnt = 0;

rex_crit_sect_type  mdp_async_crit_sect;
rex_crit_sect_type  mdp_disp_API_crit_sect;
rex_crit_sect_type  mdp_disp_get_cb_arg_crit_sect;

rex_tcb_type * mdp_last_tcb=NULL;
rex_tcb_type * mdp_last_tcb_API=NULL;

extern boolean disp_task_initialized;

#ifdef FEATURE_DISPLAY_PROFILING
MDP_PROFILE_TYPE mdp_pf;
#endif /* FEATURE_DISPLAY_PROFILING */

#define MDP_KEYID  0xABCD0722

// mdp timer setting for checking queue in msec
#define MDP_DISP_API_PACKET_WAIT_TIME 100
#define MDP_DISP_DATA_PACKET_WAIT_TIME 100
// mdp timer setting for releasing the calling task in msec
#define MDP_COMPLETE_SIGNAL_WAIT_TIME 1000

/*===========================================================================

FUNCTION mdp_add_sysCtx_list()

DESCRIPTION
  This function is to set a system privilege of MDP overlays.

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
  #ifdef FEATURE_BREW
    #ifndef FEATURE_MDP2
void mdp_add_sysCtx_list(void)
{
  void * sysCtx;
  int i;

  sysCtx = (void *) AEE_GetAppContext();

  //if there is a match, just return  
  for (i=0;i<MDP_MAX_SYSCTX;i++)
  {
    if (sysCtx == mdpSysCtx[i])
    {
      return;
    }
  }

  //find an empty slot
  for (i=0;i<MDP_MAX_SYSCTX;i++)
  {
    if ((void *)0xDEADDEAD == mdpSysCtx[i])
    {
      mdpSysCtx[i] = sysCtx;
      return;
    }
  }  
}
    #endif
  #endif

/*===========================================================================

FUNCTION mdp_remove_sysCtx_list()

DESCRIPTION
  This function is to remove system context id.

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
  #ifdef FEATURE_BREW
    #ifndef FEATURE_MDP2
void mdp_remove_sysCtx_list(void * sysCtx)
{
  int i;

  //if there is a match, remove it from the list
  for (i=0;i<MDP_MAX_SYSCTX;i++)
  {
    if (sysCtx == mdpSysCtx[i])
    {
      mdpSysCtx[i] = (void *)0xDEADDEAD;
      return;
    }
  }
}
    #endif
  #endif

/*===========================================================================

FUNCTION mdp_is_sysCtx()

DESCRIPTION
  This function is to verify the caller's system privilege.

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
  #ifdef FEATURE_BREW
    #ifndef FEATURE_MDP2
int mdp_is_sysCtx(void * appCtx)
{
  int i;

  for (i=0;i<MDP_MAX_SYSCTX;i++)
  {
    if (appCtx == mdpSysCtx[i])
    {
      return TRUE;
    }
  }

  return FALSE;
}
    #endif
  #endif

/*===========================================================================

FUNCTION mdp_refresh_viewport()

DESCRIPTION
  This function is to refresh the entire registered/enabled overlays into 
  the destined display area

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
  #ifdef FEATURE_MDP2
#error code not present
  #endif

/*===========================================================================

FUNCTION mdp_proxy_cb()

DESCRIPTION
  This function is to be used in the place of NULL callback function.
  We need to assign proxy cb in order to avoid any future synchronization.

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
MDP_CB mdp_proxy_cb;

void mdp_proxy_cb_func(void *arg, MDP_STATUS status)
{
  MDP_MSG_DEBUG("MDP proxy cb called!",0,0,0);
}


/*===========================================================================

FUNCTION mdp_update_ovimg()

DESCRIPTION
  This function is to update the pre-registered ovimg from the list.

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
MDP_STATUS mdp_update_ovimg(uint32 id)
{
  MDPIMG_LIST *node;

  if (mdp_drv_initialized != TRUE)
    return MDP_FAIL;

  if ((id==0) || (id==0xffffffff))
    return MDP_FAIL;

  node = (MDPIMG_LIST *)id;

  if (node->key_id != MDP_KEYID)
    return MDP_FAIL;

  if (node->enabled != TRUE)
    return MDP_FAIL;

#ifdef DISP_TASK_BUFFER_SHARING
  if (node->disp_packet != NULL)
  {
    return mdp_disp_update_async_sub(&node->orig_img, NULL, node->disp_packet);      
  }
#endif

  return mdp_disp_update(&node->orig_img);
}

/*===========================================================================

FUNCTION mdp_get_mdpImg()

DESCRIPTION
  This function is to update the pre-registered ovimg from the list for 
  the crop window area as defined.

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
MDP_STATUS mdp_get_mdpImg(uint32 id, MDPIMG *mdpImg)
{
  MDPIMG_LIST *node;

  if (mdp_drv_initialized != TRUE)
    return MDP_FAIL;

  if ((id==0) || (id==0xffffffff))
    return MDP_FAIL;

  node = (MDPIMG_LIST *)id;

  if (node->key_id != MDP_KEYID)
    return MDP_FAIL;

  if (node->enabled != TRUE)
    return MDP_FAIL;
  
#ifdef DISP_TASK_BUFFER_SHARING            
  if (node->orig_img.mdpOp&MDPOP_QUEUE_COPY)
  {
    *mdpImg = node->orig_img_before_queue_copy;        
  }
  else
  {
    *mdpImg = node->orig_img;      
  }
#else
  *mdpImg = node->orig_img;
    
#endif
  
  return MDP_SUCCESSFUL;
}

/*===========================================================================

FUNCTION mdp_update_ovimg_async()

DESCRIPTION
  This function is to update the pre-registered ovimg from the list.

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
MDP_STATUS mdp_update_ovimg_async(uint32 id, MDP_CB *cb)
{
  MDPIMG_LIST *node;

  if (mdp_drv_initialized != TRUE)
    return MDP_FAIL;

  if ((id==0) || (id==0xffffffff))
    return MDP_FAIL;

  node = (MDPIMG_LIST *)id;

  if (node->key_id != MDP_KEYID)
    return MDP_FAIL;

  if (node->enabled != TRUE)
    return MDP_FAIL;

#ifdef DISP_TASK_BUFFER_SHARING
  if (node->disp_packet != NULL)
  {
    MDP_STATUS ret;

    ret = mdp_disp_update_async_sub(&node->orig_img, cb, node->disp_packet);        

    return ret;
  }
#endif

  return mdp_disp_update_async(&node->orig_img, cb);
}


/*===========================================================================

FUNCTION mdp_clear_ovimg()

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
void mdp_clear_ovimg(uint32 id)
{
  MDPIMG_LIST *node;

  if (mdp_drv_initialized != TRUE)
    return;

  if ((id==0) || (id==0xffffffff))
    return;

  node = (MDPIMG_LIST *)id;
  if (node->key_id != MDP_KEYID)
    return;

  mdp_clear_ovimg_sub(node);
}

/*===========================================================================

FUNCTION mdp_get_cb_arg()

DESCRIPTION
  This function is to get one callback argument buffer

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
MDP_CB_DATA * mdp_get_cb_arg(void)
{
  MDP_CB_DATA * cb_arg;

  rex_enter_crit_sect(&mdp_disp_get_cb_arg_crit_sect);      
  cb_arg = &mdp_cb_arg[mdp_cbarg_cnt++%MDP_MAX_CB_ARG];
  rex_leave_crit_sect(&mdp_disp_get_cb_arg_crit_sect);        

  return cb_arg;
}

/*===========================================================================

FUNCTION mdp_send_disp_API_packet()

DESCRIPTION
  This function is to send an API processing request to display task.
  Display task will ensure all APIs and Updates get processed in order.

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/ 
int mdp_send_disp_API_packet(MDP_API_IOCTL id, uint32 *arg, uint32 argc)
{
  disp_packet_type_API * packet;
  MDP_CB_DATA * cb_arg;
  uint32 i;
  rex_timer_type mdp_disp_API_packet_wait_timer;

  if (&disp_tcb == rex_self())
  {
    MSG_ERROR("ERROR: mdp_send_disp_API_packet within DISP TASK !!!", 0, 0, 0);    
    return 0;  
  }

  if (argc > DISP_API_MAX_ARG)
  {
    MSG_ERROR("argc out of range", 0, 0, 0);    
    return 0;
  }

  rex_enter_crit_sect(&mdp_disp_API_crit_sect);  
  packet = disp_imglist_get_API();

  if (packet == NULL)
    rex_def_timer(&mdp_disp_API_packet_wait_timer, rex_self(), MDP_COMPLETE_SIG);  
    
  while (packet == NULL)
  {
#ifdef FEATURE_WINCE
#error code not present
#endif

    // OK...no more packets.
    // wait until packet available signal from disp task
    rex_set_timer(&mdp_disp_API_packet_wait_timer, MDP_DISP_API_PACKET_WAIT_TIME);

    mdp_last_tcb_API = rex_self();
    rex_wait(MDP_COMPLETE_SIG);
    rex_clr_timer(&mdp_disp_API_packet_wait_timer);
    mdp_last_tcb_API = NULL;
    // if task is switched to disp task and it sets a signal 
    // before the above line
    rex_clr_sigs(rex_self(),MDP_COMPLETE_SIG);

    packet = disp_imglist_get_API();
    if (packet == NULL)
    {
      MSG_ERROR("Ran Out of DISP Task API Packets!", 0, 0, 0);  
    }
  }  
  rex_leave_crit_sect(&mdp_disp_API_crit_sect);  

  if (NULL == packet)
  {
    MSG_ERROR("mdp_send_disp_API_packet returning NULL !!!",0,0,0);
    return 0;
  }

  cb_arg = mdp_get_cb_arg();  
  cb_arg->result = MDP_FAIL;
  cb_arg->tcb = rex_self();

  packet->cmd = DISP_CMD_API_UPDATE;   
  packet->cb.cb_func = &mdp_task_release_cb;
  packet->cb.arg = (void *)cb_arg;
  packet->api_id = id;

  for (i=0;i<argc;i++)
    packet->arg[i] = arg[i];

  // clear complete sig before queueing this packet
  rex_clr_sigs(rex_self(),MDP_COMPLETE_SIG);  

  // push it to display queue for this api command execution
  disp_q_put_API(packet);    /* Put on command queue*/

  // notify to display task
  (void) rex_set_sigs(&disp_tcb, DISP_CMD_Q_SIG);  /* Signal a queue event*/    

  // wait for the completion
  rex_wait(MDP_COMPLETE_SIG);    

  // clear signal  
  rex_clr_sigs(rex_self(),MDP_COMPLETE_SIG);

  return cb_arg->result;  
}

/*===========================================================================

FUNCTION mdp_disp_update_async()

DESCRIPTION
  This function is to display the specified image on LCD screen asynchronously.
  (The calling task will immediately get returned right after the job is inserted 
  into display command queue). Any overlapped image in the overlay list from 
  other layer also gets displayed.

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  MDP_SUCCESSFUL if successful otherwise MDP_FAIL

SIDE EFFECTS
  caller of this function will get blocked until MDP finishes to update
  the image on LCD display

===========================================================================*/

#ifndef FEATURE_MDP2
static int mdpIPLenable = 1;
#endif

static uint32 mdpUpdateCntLogIdx_RGB = 0;
static uint32 mdpUpdateCntLog_RGB[10];
static uint32 mdpRGBcnt=0;
static uint32 mdpRGBCntOld=0;

static uint32 mdpUpdateCntLogIdx_VDO = 0;
static uint32 mdpUpdateCntLog_VDO[10];
static uint32 mdpVDOcnt=0;
static uint32 mdpVDOCntOld=0;

static timetick_type mdp_old_ms=0;
static timetick_type mdp_new_ms=0;

MDP_STATUS mdp_disp_update_async(MDPIMG * mdpImg, MDP_CB *cb)
{  
  return mdp_disp_update_async_sub(mdpImg,cb, NULL);
}

/*===========================================================================

FUNCTION mdp_disp_update_async_sub()

DESCRIPTION
  This function is to handle mdp_disp_update_async()

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  MDP_SUCCESSFUL if successful otherwise MDP_FAIL

SIDE EFFECTS
  caller of this function will get blocked until MDP finishes to update
  the image on LCD display

===========================================================================*/  
MDP_STATUS mdp_disp_update_async_sub(MDPIMG * mdpImg, MDP_CB *cb, disp_packet_type *packet)
{     
  MDP_MSG_PROFILE("MDP_DISP_UPDATE START:", 0,0,0);

  if (disp_task_initialized != TRUE)
    return MDP_FAIL;

  if (mdp_drv_initialized != TRUE)
    return MDP_FAIL;

  if (!mdp_is_dest_active(mdpImg->dest))
    return MDP_FAIL;

#if defined(FEATURE_MDP2) && defined(FEATURE_DISPLAY_ADAPTIVE_BACKLIGHT)
#error code not present
#endif /* FEATURE_MDP2 && FEATURE_DISPLAY_ADAPTIVE_BACKLIGHT */

  if (packet == NULL)
  {
    if (mdp_verify_img(mdpImg) != MDP_SUCCESSFUL)
      return MDP_FAIL;

    if ((packet = mdp_disp_packet_proc(mdpImg)) == NULL)
      return MDP_FAIL;
  }

  packet->cmd = DISP_CMD_DISP_UPDATE;   
  packet->cb.cb_func = NULL;

  if ((NULL != cb) && (NULL != cb->cb_func))
  {
    // if it's already copied (done), let's return it to callback
    if (packet->queue_copy_done)
    {
      cb->cb_func(cb->arg, MDP_SUCCESSFUL);
      // we need to assign proxy cb otherwise, disp taks will wait
      // until MDP completes the transfer.
      packet->cb = mdp_proxy_cb;                    
    }
    // otherwise, let's callback when it's done
    else
    {
      packet->cb = *cb;
    }
  }
  else
  {
    packet->cb = mdp_proxy_cb;              
  }

  disp_q_put (packet);                             /* Put on command queue*/

  MDP_MSG_PROFILE("SEND signal -> display_task", 0,0,0);  
  (void) rex_set_sigs(&disp_tcb, DISP_CMD_Q_SIG);  /* Signal a queue event*/    
  MDP_MSG_PROFILE("MDP_DISP_UPDATE END ---- ", 0,0,0);
  MDP_MSG_PROFILE("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ", 0,0,0);

  // Remove timetick_get_ms() for specific WinMob builds.
#ifndef uldr
  if (mdp_debug_calcFps)
  {
    if (mdp_is_vdo_image(mdpImg->imgType))
      mdpVDOcnt++;
    else
      mdpRGBcnt++;

    mdp_new_ms = timetick_get_ms();

    if ((mdp_new_ms-mdp_old_ms)>= 1000)
    {
      mdpUpdateCntLog_RGB[mdpUpdateCntLogIdx_RGB] = mdpRGBcnt-mdpRGBCntOld;
      mdpUpdateCntLog_VDO[mdpUpdateCntLogIdx_VDO] = mdpVDOcnt-mdpVDOCntOld;
            
      MDP_MSG_PROFILE("**** x10 VDO FPS = %d/10 - RGB FPS = %d/10 ****", 
               (10000*mdpUpdateCntLog_VDO[mdpUpdateCntLogIdx_VDO])/(mdp_new_ms-mdp_old_ms),
               (10000*mdpUpdateCntLog_RGB[mdpUpdateCntLogIdx_RGB])/(mdp_new_ms-mdp_old_ms),0);

      mdp_old_ms = mdp_new_ms;

      mdpUpdateCntLogIdx_RGB = (++mdpUpdateCntLogIdx_RGB < 10)?mdpUpdateCntLogIdx_RGB:0;       
      mdpRGBCntOld = mdpRGBcnt;      

      mdpUpdateCntLogIdx_VDO = (++mdpUpdateCntLogIdx_VDO < 10)?mdpUpdateCntLogIdx_VDO:0;       
      mdpVDOCntOld = mdpVDOcnt;            
    }
  }
#endif // #ifndef uldr

#ifdef FEATURE_DISPLAY_PROFILING
  mdp_profiling_calc_interframe(mdpImg);
#endif /* FEATURE_DISPLAY_PROFILING */

  return MDP_SUCCESSFUL;
}

/*===========================================================================

FUNCTION mdp_disp_packet_proc()

DESCRIPTION
  This function is to wait until receiving one display packet and go through
  IPL conversion if it's necessary.

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  MDP_SUCCESSFUL if successful otherwise MDP_FAIL

SIDE EFFECTS
  caller of this function will get blocked until MDP finishes to update
  the image on LCD display

===========================================================================*/  
disp_packet_type * mdp_disp_packet_proc(MDPIMG * mdpImg)
{
  disp_packet_type *packet;
  MDP_CWIN old_cwin;
  rex_timer_type mdp_disp_data_packet_wait_timer;
  const mdp_frame_type* disp_frame;

#ifndef FEATURE_MDP2
  int img_width, img_height;
  int row;
  uint8 *src_ptr=NULL, *dst_ptr=NULL;
  int inBpp;
  ipl_status_type iplStatus = IPL_FAILURE;
  ipl_image_type inImgPtr, outImgPtr;
  ipl_rect_type  inRect, outRect;
  int scale_flag = MDPOP_VDO_X2|MDPOP_VDO_1BY4|MDPOP_VDO_1BY2|MDPOP_VDO_3BY4|MDPOP_ASCALE;    
  uint32 x_num, x_den, y_num, y_den;
  uint32 tempVal;
  uint32 noImageBytes2MDP=0;  
  /*number of bytes in the image which are to be flushed from the cache to the phy mem for MDP*/
 
#endif

  rex_enter_crit_sect(&mdp_async_crit_sect);  

  old_cwin.x1 = -1; //check to see if it's changed
  packet = disp_imglist_get(mdpImg, &old_cwin);
  
  if (packet == NULL)
    rex_def_timer(&mdp_disp_data_packet_wait_timer, rex_self(), MDP_COMPLETE_SIG);  
    
  while (packet == NULL)
  {
#ifdef FEATURE_WINCE
#error code not present
#endif

    // OK...no more packets.
    // wait until packet available signal from disp task
    rex_set_timer(&mdp_disp_data_packet_wait_timer, MDP_DISP_DATA_PACKET_WAIT_TIME);

    mdp_last_tcb = rex_self();
    rex_wait(MDP_COMPLETE_SIG);
    rex_clr_timer(&mdp_disp_data_packet_wait_timer);
    mdp_last_tcb = NULL;
    // if task is switched to disp task and it sets a signal 
    // before the above line
    rex_clr_sigs(rex_self(),MDP_COMPLETE_SIG);

    packet = disp_imglist_get(mdpImg, &old_cwin);
    if (packet == NULL)
    {
      MSG_ERROR("Ran Out of DISP Task Packets!", 0, 0, 0);  
    }
  }

  disp_frame = packet->disp_frame; /* holder for cleaner code */

  if ((mdpImg->mdpOp&MDPOP_QUEUE_EXPAND)&&(old_cwin.x1 != -1)
#ifdef DISP_TASK_BUFFER_SHARING
      && !(mdpImg->mdpOp&MDPOP_ASCALE)
#endif   
     )
  {
    if (old_cwin.x1 < mdpImg->cwin.x1)
    {
      mdpImg->lcd_x -= (mdpImg->cwin.x1 - old_cwin.x1);
      mdpImg->cwin.x1 = old_cwin.x1;
    }
    if (old_cwin.y1 < mdpImg->cwin.y1)
    {
      mdpImg->lcd_y -= (mdpImg->cwin.y1 - old_cwin.y1);
      mdpImg->cwin.y1 = old_cwin.y1;
    }
    if (old_cwin.x2 > mdpImg->cwin.x2)
      mdpImg->cwin.x2 = old_cwin.x2;
    if (old_cwin.y2 > mdpImg->cwin.y2)
      mdpImg->cwin.y2 = old_cwin.y2;
  }

  packet->mdpImg = *mdpImg;
  packet->queue_copy_done = FALSE;

  rex_leave_crit_sect(&mdp_async_crit_sect);  

// for MDP2, we're using driver queue (ibuf) for color conversion and blending
// for MDP1, we're using display queue for color conversion only
#ifndef FEATURE_MDP2
  if (mdpImg->mdpOp & MDPOP_QUEUE_COPY)
  {
    MDP_MSG_PROFILE("mdp_disp_packet_proc - IPL Start", 0,0,0);

    img_width = mdpImg->cwin.x2 - mdpImg->cwin.x1 + 1;
    img_height = mdpImg->cwin.y2 - mdpImg->cwin.y1 + 1;

    dst_ptr = disp_frame->buffer;
    src_ptr = (uint8 *)mdpImg->bmy_addr;
    // orig_bmy_addr gets used for MDP_QUEUE_EXPAND to compare the bitmaps
    packet->orig_bmy_addr = packet->mdpImg.bmy_addr;

    switch (mdpImg->mdpOp & scale_flag)
    {
      case MDPOP_VDO_X2:
        x_num = 2;
        x_den = 1;
        y_num = 2;
        y_den = 1;      
        break;

      case MDPOP_VDO_1BY4:
        x_num = 1;
        x_den = 4;
        y_num = 1;
        y_den = 4;      
        break;

      case MDPOP_VDO_1BY2:
        x_num = 1;
        x_den = 2;
        y_num = 1;
        y_den = 2;      
        break;

      case MDPOP_VDO_3BY4:
        x_num = 3;
        x_den = 4;
        y_num = 3;
        y_den = 4;      
        break;

      case MDPOP_ASCALE:
        x_num = mdpImg->ascale.width;
        x_den = 
        (mdpImg->cwin.x2-mdpImg->cwin.x1+1);
        y_num = mdpImg->ascale.height;
        y_den = 
        (mdpImg->cwin.y2-mdpImg->cwin.y1+1);  
        break; 

      default:
        x_num = 1;
        x_den = 1;
        y_num = 1;
        y_den = 1;      
        break;      
    }

    if ((mdpIPLenable) && 
        ( (mdpImg->imgType == MDP_Y_CBCR_H2V2)
#ifdef FEATURE_MDP2      
#error code not present
#endif       
          ||((mdpImg->mdpOp & scale_flag)&&((x_num!=x_den)||(y_num!=y_den)))
        )
       ) /* IPL crop-scale support */
    {
      inImgPtr.dx = mdpImg->width;    
	  /*Assuming the height of the source image to be upto cwin bottom*/
      inImgPtr.dy = (mdpImg->cwin.y2 + 1); 

      inRect.x = mdpImg->cwin.x1;
      inRect.y = mdpImg->cwin.y1;
      inRect.dx = (mdpImg->cwin.x2 - mdpImg->cwin.x1 + 1);
      inRect.dy = (mdpImg->cwin.y2 - mdpImg->cwin.y1 + 1);
      outRect = inRect;

      outRect.x = 0;
      outRect.y = 0;
      outRect.dx = (2*inRect.dx*x_num+x_den)/(2*x_den);
      outRect.dy = (2*inRect.dy*y_num+y_den)/(2*y_den);

      if (mdpImg->mdpOp & MDPOP_ROT90)
      {
        if (outRect.dx > disp_frame->pxHeight)
        {
          tempVal = outRect.dx - disp_frame->pxHeight;
          tempVal = (2*tempVal*x_den+x_num)/(2*x_num);
          inRect.dx -= tempVal;
          outRect.dx = disp_frame->pxHeight;
        }
        if (outRect.dy > disp_frame->pxWidth)
        {
          tempVal = outRect.dy - disp_frame->pxWidth;
          tempVal = (2*tempVal*y_den+y_num)/(2*y_num);
          inRect.dy -= tempVal;
          outRect.dy = disp_frame->pxWidth;
        }
      }
      else
      {
        if (outRect.dx > disp_frame->pxWidth)
        {
          tempVal = outRect.dx - disp_frame->pxWidth;
          tempVal = (2*tempVal*x_den+x_num)/(2*x_num);
          inRect.dx -= tempVal;
          outRect.dx = disp_frame->pxWidth;
        }
        if (outRect.dy > disp_frame->pxHeight)
        {
          tempVal = outRect.dy - disp_frame->pxHeight;
          tempVal = (2*tempVal*y_den+y_num)/(2*y_num);
          inRect.dy -= tempVal;
          outRect.dy = disp_frame->pxHeight;
        }
      }

      if (mdpImg->imgType == MDP_RGB_565)
        inImgPtr.cFormat = IPL_RGB565;

      else if (mdpImg->imgType == MDP_Y_CBCR_H2V2)
        inImgPtr.cFormat = IPL_YCrCb420_LINE_PK;

#ifdef FEATURE_MDP2      
#error code not present
#endif
      /*
      03-13-06
      cjr: currently, there is no support for this color format through IPL
      
      else if (mdpImg->imgType == MDP_XRGB_8888)
      {
        inImgPtr.cFormat = IPL_RGB888_PAD;
      }
      */
      else
      {
#ifdef DISP_TASK_BUFFER_SHARING            
        if (mdpImg->mdpOp&MDPOP_ASCALE)
        {
          disp_imglist_free(packet);          
          packet = NULL;
        }
#endif        
        return packet;        
      }

      inImgPtr.imgPtr = (unsigned char *) mdpImg->bmy_addr;
      inImgPtr.clrPtr = (uint8 *) mdpImg->cbcr_addr;

      outImgPtr.dx = outRect.dx;
      outImgPtr.dy = outRect.dy;
      outImgPtr.cFormat = IPL_RGB565;
      outImgPtr.imgPtr = (unsigned char *) dst_ptr;

      MDP_MSG_PROFILE("IPL-Entering @@@", 0,0,0);

      iplStatus = ipl_crop_resize_rot(&inImgPtr, &outImgPtr,&inRect, &outRect,
												IPL_NOROT,IPL_QUALITY_MEDIUM);

      noImageBytes2MDP = outImgPtr.dx * outImgPtr.dy * 2;
      /*noImageBytes2MDP - No. of image bytes to be flushed from cache to phy mem after ipl operation on it.*/
     

      MDP_MSG_PROFILE("IPL-Exiting @@@", 0,0,0);

      // can't use this routine since the input and output can be RGB
      // IPL needs to be optimized to handle all cases
      //iplStatus = ipl_crop_YCbCrToRGB(&inImgPtr, &outImgPtr,&inRect, &outRect);


      if (iplStatus == IPL_SUCCESS)
      { // success
        packet->mdpImg.bmy_addr = (uint32 *)dst_ptr;      
        packet->mdpImg.imgType = MDP_RGB_565;
        packet->mdpImg.width = outRect.dx;      
        packet->mdpImg.cwin.x1 = 0;
        packet->mdpImg.cwin.y1 = 0;      
        packet->mdpImg.cwin.x2 = outRect.dx-1;
        packet->mdpImg.cwin.y2 = outRect.dy-1;              
        packet->queue_copy_done = TRUE;
      }
      else
      {
        MDP_MSG_ERROR("IPL FAILURE from mdp_disp_packet_proc()!!!", 0,0,0);      
#ifdef DISP_TASK_BUFFER_SHARING            
        if (mdpImg->mdpOp&MDPOP_ASCALE)
        {
          disp_imglist_free(packet);          
          packet = NULL;
        }
#endif        
        return packet;
      }
    }
    else
    {
      switch (mdpImg->imgType)
      {
        case MDP_RGB_565: inBpp = 2; break;
        case MDP_XRGB_8888: inBpp = 4; break;   
        case MDP_Y_CBCR_H2V2:inBpp = 1; break;        
#ifdef FEATURE_MDP2        
#error code not present
#endif    
        default: return packet;
      }

      packet->mdpImg.bmy_addr = (uint32 *)dst_ptr;
      packet->queue_copy_done = TRUE;

      src_ptr += (mdpImg->cwin.y1 * mdpImg->width + mdpImg->cwin.x1)*inBpp;
      dst_ptr += (mdpImg->cwin.y1 * mdpImg->width + mdpImg->cwin.x1)*inBpp;

      if (mdpImg->width == img_width)
      {
        memcpy((void *)dst_ptr, (void *)src_ptr, img_width * img_height * inBpp);
      }
      else
      {
        for (row = 0 ; row < img_height ; row++)
        {
          memcpy((void *)dst_ptr, (void *)src_ptr, img_width * inBpp);
          src_ptr += mdpImg->width*inBpp;
          dst_ptr += mdpImg->width*inBpp;
        }
      }

      noImageBytes2MDP = img_width * img_height * inBpp; //size of image for RGB image or Y component
      if (inBpp == 1)
      {
        src_ptr = (uint8 *)mdpImg->cbcr_addr;  
        dst_ptr = disp_frame->buffer;
        dst_ptr += (mdpImg->cwin.y2+1)*mdpImg->width*inBpp;
        packet->mdpImg.cbcr_addr = (uint32 *)dst_ptr;

        src_ptr += ((mdpImg->cwin.y1 * mdpImg->width)/2 + mdpImg->cwin.x1)*inBpp;
        dst_ptr += ((mdpImg->cwin.y1 * mdpImg->width)/2 + mdpImg->cwin.x1)*inBpp;            

        if (mdpImg->width == img_width)
        {
          memcpy((void *)dst_ptr, (void *)src_ptr, (img_width * img_height * inBpp)/2);
        }
        else
        {
          for (row = 0 ; row < (img_height/2) ; row++)
          {
            memcpy((void *)dst_ptr, (void *)src_ptr, img_width * inBpp);
            src_ptr += mdpImg->width*inBpp;
            dst_ptr += mdpImg->width*inBpp;
          }
        }
        noImageBytes2MDP +=  noImageBytes2MDP >> 2 ;  /*adding half byte for chroma component*/
        
      }
    }    
    /*flush the cache contents after IPL operation so that MDP hardware can get the correct image from phy mem*/
  MDP_FLUSH_DCACHE_REGION(dst_ptr,noImageBytes2MDP);	
  } //if (mdpImg->mdpOp & MDPOP_QUEUE_COPY)



#endif //#ifndef FEATURE_MDP2

  return packet;
}

/*===========================================================================

FUNCTION mdp_map_dest()

DESCRIPTION
  This function is to re-direct one logical display destination.  

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  All updates through "src" will transfer through "dest" and all "dest" 
  updates will NOT transfer through "dest".

===========================================================================*/
#ifdef FEATURE_MDP2
#error code not present
#endif

/*===========================================================================

FUNCTION mdp_is_dest_active()

DESCRIPTION
  This function is to verify the destination is active.

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  TRUE if it's active otheriwse, returns FALSE.

SIDE EFFECTS
  caller of this function will get blocked until MDP finishes to update
  the image on LCD display

===========================================================================*/
boolean mdp_is_dest_active(LCD_TYPE dest)
{
  LCD_TYPE idest;
  boolean ret=FALSE;

  rex_enter_crit_sect(&mdp_crit_sect);  

#ifdef FEATURE_DISPLAY_POWER_DEBUG
#error code not present
#endif /* FEATURE_DISPLAY_POWER_DEBUG */
  {  
    for (idest=DISPLAY_TYPE_1; idest<MAX_LCD_NUM; idest++)
    {
      if ((mdp_active_dest[idest] == dest) && mdp_dest_power_on[idest])
        ret = TRUE;
    }
  }

  rex_leave_crit_sect(&mdp_crit_sect);

  return ret;
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

boolean mdp_disp_fatal_err_flag = FALSE;

MDP_STATUS mdp_disp_update(MDPIMG * mdpImg)
{
  MDP_CB_DATA * cb_arg;
  rex_timer_type mdp_disp_update_timer;

  if (mdp_drv_initialized != TRUE)
    return MDP_FAIL;

  if (mdp_verify_img(mdpImg) != MDP_SUCCESSFUL)
    return MDP_FAIL;

  // user calls for sync update and disp taks is not started yet
  // therfore, we're calling mdp drvier update directly
  /*
  if (!disp_task_initialized)
    return mdp_disp_update_proc(mdpImg);
  */

  if (!disp_task_initialized)
    return MDP_FAIL;

  if (mdp_disp_fatal_err_flag)
    return mdp_disp_update_proc(mdpImg, NULL);

  if (!mdp_is_dest_active(mdpImg->dest))
    return MDP_FAIL;

  rex_enter_crit_sect(&mdp_async_crit_sect);  

  cb_arg = mdp_get_cb_arg();
  cb_arg->result = MDP_FAIL;
  mdp_cb.cb_func = &mdp_task_release_cb;   
  mdp_cb.arg = (void *)cb_arg;        

  ////////////////////////////////
  // MDP Rex timer setup 
  ////////////////////////////////
  cb_arg->tcb = rex_self();
  rex_def_timer(&mdp_disp_update_timer, cb_arg->tcb, MDP_COMPLETE_SIG);

#ifdef FEATURE_WINCE
#error code not present
#endif

  // timer to kick off and send MDP_COMPLETE_SIG signal
  rex_set_timer(&mdp_disp_update_timer, MDP_COMPLETE_SIGNAL_WAIT_TIME);

  ////////////////////////////////
  // MDP inserts one job into async queue
  ////////////////////////////////
  mdp_disp_update_async(mdpImg, &mdp_cb);

  ////////////////////////////////
  // MDP Rex timer wait
  ////////////////////////////////
  rex_wait(MDP_COMPLETE_SIG);
  rex_clr_timer(&mdp_disp_update_timer);        

  if (NULL != cb_arg->tcb)  
  {
    MSG_ERROR("ERROR: mdp_disp_update() rex_wait() timeout!!!",0,0,0);
    cb_arg->tcb = NULL;  
  }

  // if task is switched to disp task and it sets a signal 
  // before the above line
  rex_clr_sigs(rex_self(),MDP_COMPLETE_SIG);

  rex_leave_crit_sect(&mdp_async_crit_sect);    

  return cb_arg->result;  
}


/*===========================================================================

FUNCTION mdp_set_fatal_err()

DESCRIPTION
  Set MDP variable to flag an error message update.  Call before displaying
  updates where task switching will not be supported.

DEPENDENCIES
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  All future mdp updates will occur syncrounously until
  mdp_clear_fatal_err() is called.  Running the MDP in error mode will
  significantly impact performance.

===========================================================================*/
void mdp_set_fatal_err(void)
{
  mdp_disp_fatal_err_flag = TRUE;
}

/*===========================================================================

FUNCTION mdp_clear_fatal_err()

DESCRIPTION
  Clear MDP variable to flag an error message update.

DEPENDENCIES
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  See mdp_set_fatal_err() above.

===========================================================================*/
void mdp_clear_fatal_err(void)
{
  mdp_disp_fatal_err_flag = FALSE;
}

/*===========================================================================

FUNCTION mdp_task_release_cb()

DESCRIPTION
  This function is to handle default async callback

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  MDP_SUCCESSFUL if successful otherwise MDP_FAIL

SIDE EFFECTS
  caller of this function will get blocked until MDP finishes to update
  the image on LCD display

===========================================================================*/
void mdp_task_release_cb(void *arg, MDP_STATUS status)
{
  MDP_CB_DATA * cb_arg = (MDP_CB_DATA *)arg;
  rex_tcb_type * tcb_temp;

  cb_arg->result = status;
  // cb_arg->tcb to signal the task waiting for the completion of update
  if (cb_arg->tcb != NULL)
  {    
    tcb_temp = cb_arg->tcb;
    cb_arg->tcb = NULL;    
    rex_set_sigs(tcb_temp, MDP_COMPLETE_SIG);
  }
}

/*===========================================================================

FUNCTION mdp_verify_img()

DESCRIPTION
  This function is to verify the specified mdpImg is valid

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  Returns MDP_SUCCESSFUL if valid, otherwise returns MDP_FAIL

SIDE EFFECTS
  None

===========================================================================*/

uint32 mdp_verify_img(MDPIMG *mdpImg)
{
  int    cntScaleFactorOps = 0; // Total scaling operations set.
  uint32 minPixWidth       = 0, // Cwin minimum values (with scaling).
         minPixHeight      = 0;
  uint32 cwinWidth,             // Get input Cwin size.
         cwinHeight;

  if (MDP_INVALID_DEST(mdpImg))
  {
    MDP_MSG_ERROR("ERROR: Invalid destination!", 0,0,0);
    return MDP_FAIL;
  }

  if (MDP_INVALID_LAYER(mdpImg->layer))
  {
    MDP_MSG_ERROR("ERROR: Invalid layer!", 0,0,0);
    return MDP_FAIL;
  }

  /*=================================
  **--- LAYER-1 Support Check ---- */
#ifndef FEATURE_MDP_LAYER1_PRIMARY
  if((mdpImg->dest == PRIMARY_LCD_TYPE) && (mdpImg->layer == MDP_LAYER_1)) 
  {
	  MDP_MSG_ERROR("ERROR: Layer-1 not supported for Primary display!", 0,0,0);
    return MDP_FAIL;
  }
#endif /* FEATURE_MDP_LAYER1_PRIMARY */

#ifndef FEATURE_MDP_LAYER1_SECONDARY
  if ( (mdpImg->dest == SECONDARY_LCD_TYPE) && (mdpImg->layer == MDP_LAYER_1) )
  {
	  MDP_MSG_ERROR("ERROR: Layer-1 not supported for Secondary display!", 0,0,0);
    return MDP_FAIL;
  }
#endif /* FEATURE_MDP_LAYER1_SECONDARY */
  /*------- End LAYER-1 ---------- */

  /* bitmap/Y address must be 4-bytes aligned and not NULL */
  if ((NULL == mdpImg->bmy_addr) || (((uint32)mdpImg->bmy_addr)&0x3))
  {
    MDP_MSG_ERROR("ERROR: Bitmap/Y-address not 4-byte aligned!", 0,0,0);
    return MDP_FAIL;
  }

  // Get the dimensions of the CWIN, inclusive so add 1.
  cwinWidth  = mdpImg->cwin.x2 - mdpImg->cwin.x1 + 1;
  cwinHeight = mdpImg->cwin.y2 - mdpImg->cwin.y1 + 1;
  MDP_MSG_ERROR("source imagecwinWidth==%d,cwinHeight===%d,mdpImg->cwin.x2==%d", cwinWidth,cwinHeight,mdpImg->cwin.x2);
  MDP_MSG_ERROR("source image ==%d,,,===%d,=====%d", mdpImg->cwin.x1,mdpImg->cwin.y2,mdpImg->cwin.y1);
  if ((mdpImg->cwin.x2<0) || (mdpImg->cwin.x1<0) || (mdpImg->cwin.y2<0) || (mdpImg->cwin.y1<0))
  {
    MDP_MSG_ERROR("ERROR: Cwin specified outside of source image!", 0,0,0);
    return MDP_FAIL;
  }

  if ((mdpImg->cwin.x2 < mdpImg->cwin.x1) || (mdpImg->cwin.y2 < mdpImg->cwin.y1))
  {
    MDP_MSG_ERROR("ERROR: Cwin has negative area!", 0,0,0);
    return MDP_FAIL;
  }

  if ( (cwinWidth * cwinHeight) < MDP_MIN_PIXEL_SIZE )
  {
    MDP_MSG_ERROR("ERROR: Cwin to small to be supported by MDP!", 0,0,0);
    return MDP_FAIL;
  }

  if (mdpImg->cwin.x2 >= (int32)mdpImg->width)
  {
    MDP_MSG_ERROR("ERROR: Cwin specified outside of source image!", 0,0,0);
    return MDP_FAIL;
  }

  // Make sure we are not trying to output sub 1-pixel widths.
  // Also, check for more than one scale operation being set.
  if ((mdpImg->mdpOp&MDPOP_VDO_X2) != 0) 
  {
    ++cntScaleFactorOps;
    minPixWidth = minPixHeight = 1;
  }

  if ((mdpImg->mdpOp&MDPOP_VDO_1BY4) != 0)
  {
    ++cntScaleFactorOps;
    minPixWidth = minPixHeight = 4;
  }

  if ((mdpImg->mdpOp&MDPOP_VDO_1BY2) != 0)
  {
    ++cntScaleFactorOps;
    minPixWidth = minPixHeight = 4;
  }

  if ((mdpImg->mdpOp&MDPOP_VDO_3BY4) != 0)      
  {
    ++cntScaleFactorOps;
    minPixWidth = minPixHeight = 4;
  }

  if ((mdpImg->mdpOp&MDPOP_ASCALE) != 0)
  {
    ++cntScaleFactorOps;
    minPixWidth = minPixHeight = 1;
  }

#ifdef FEATURE_MDP_WSCALE
  if ((mdpImg->mdpOp&MDPOP_WSCALE_LINEAR_STRETCH) != 0)
  {
    ++cntScaleFactorOps;
    minPixWidth = minPixHeight = 1;
  }

  if ((mdpImg->mdpOp&MDPOP_WSCALE_EDGE_STRETCH) != 0)
  {
    ++cntScaleFactorOps;
    minPixWidth = minPixHeight = 1;
  }

  if ((mdpImg->mdpOp&MDPOP_WSCALE_NO_STRETCH) != 0)
  {
    ++cntScaleFactorOps;
    minPixWidth = minPixHeight = 1;
  }
#endif

  if (cntScaleFactorOps > 1) 
  {
    MDP_MSG_ERROR("ERROR: More than one scaling factor specified!", 0,0,0);
    return MDP_FAIL;
  }

  // Check for scaling conditions which are trying to update less
  //   than 1 pixel widths in either dimension.  This test doesn't check
  //   upscale or ascale, because they don't need to be tested.
  if (cntScaleFactorOps == 1)
  {
    if ( (cwinWidth < minPixWidth) || (cwinHeight < minPixHeight) )
    {
      MDP_MSG_ERROR("ERROR: Scaling cannot be applied with Cwin specified!", 0,0,0);
      return MDP_FAIL;
    }
  }

#ifdef FEATURE_MDP2
#error code not present
#endif

  if (mdpImg->imgType >= MDP_IMGTYPE_LIMIT)
  {
    MDP_MSG_ERROR("ERROR: Invalid image type!", 0,0,0);
    return MDP_FAIL;
  }

  return MDP_SUCCESSFUL;
}


/*===========================================================================

FUNCTION mdp_is_ovimg_valid()

DESCRIPTION
  This function is to verify the given ID points to valid registration

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  TRUE if valid otherwise FALSE

SIDE EFFECTS
  None

===========================================================================*/
int32 mdp_is_ovimg_valid(uint32 id)
{
  MDPIMG_LIST *node;

  if ((id==0) || (id==0xffffffff))
    return FALSE;

  node = (MDPIMG_LIST *)id;
  if (node->key_id != MDP_KEYID)
    return FALSE;

  return TRUE;
}

/*===========================================================================

FUNCTION mdp_is_vdo_image()

DESCRIPTION
  This function is to verify the input image is YUV data type.

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  TRUE if valid otherwise FALSE

SIDE EFFECTS
  None

===========================================================================*/
boolean mdp_is_vdo_image(MDPIMG_TYPE imgType)
{
  if (
#ifdef FEATURE_MDP2 
#error code not present
#endif
     (imgType == MDP_RGB_565)  ||
     (imgType == MDP_XRGB_8888)     
     )
    return FALSE;
  else
    return TRUE;
}

/*===========================================================================

FUNCTION mdp_register_ovimg()

DESCRIPTION
  This function is to register an overlay image into the primary or secondary
  display overlay list

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  Returns the registration ID if successful, otherwise returns NULL

SIDE EFFECTS
  None

===========================================================================*/
uint32 mdp_register_ovimg(MDPIMG *mdpImg)
{
  if (mdp_drv_initialized != TRUE)
    return 0;

  if (mdpImg == NULL)
    return 0;

  if (mdp_verify_img(mdpImg) != MDP_SUCCESSFUL)
    return 0;

#ifndef CUST_EDITION
#ifdef DISP_TASK_BUFFER_SHARING
  if ((mdpImg->mdpOp&MDPOP_ASCALE) || (mdpImg->mdpOp&MDPOP_QUEUE_COPY))
  {
    disp_packet_type * mdp_ascale_packet;
    uint32 ret;

    mdpImg->mdpOp |= MDPOP_QUEUE_COPY;

    // to have one display packet for IPL conversion storage
    if ((mdp_ascale_packet = mdp_disp_packet_proc(mdpImg)) == NULL)
      return NULL;

    // we need to have at least one packet available for a regular update 
    // => we can't keep holding the last packet
    if (disp_imglist_total == 0)
    {
      disp_imglist_free(mdp_ascale_packet);          
      return NULL;
    }

    // to register with the above display packet
    ret = mdp_register_ovimg_sub(&mdp_ascale_packet->mdpImg, mdp_ascale_packet);        

    if (ret == NULL)
    {
      disp_imglist_free(mdp_ascale_packet);        
    }
    else
    {
      ((MDPIMG_LIST *)ret)->orig_img_before_queue_copy = *mdpImg;
      mdp_ascale_packet->registered =  TRUE;
    }

    return ret;
  }
#endif
#endif

  return mdp_register_ovimg_sub(mdpImg, NULL);
}

/*===========================================================================

FUNCTION mdp_register_ovimg_sub()

DESCRIPTION
  This function is to handle mdp_register_ovimg()

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  MDP_SUCCESSFUL if successful otherwise MDP_FAIL

SIDE EFFECTS
  None

===========================================================================*/
uint32 mdp_register_ovimg_sub_API(MDPIMG *mdpImg, disp_packet_type * d_packet)
{
  MDPIMG_LIST *node, **mdpBITPtr;

  if ((node = mdp_imglist_get()) == NULL)
    return 0;

  node->img = *mdpImg;
  node->orig_img = *mdpImg;

  // adjusting to BREW Screen Offset
  node->img.lcd_x += MDP_SYS_OFFSET_X;
  node->img.lcd_y += MDP_SYS_OFFSET_Y;  

#ifdef FEATURE_BREW  
  // this is app context pointer to verify the ownership of 
  // the overlay

#ifndef FEATURE_MDP2  
  node->appCtx = (void *) AEE_GetAppContext(); 
#endif
#endif

  mdp_touch_img(node);

  if (mdp_verify_dispArea(&node->img) != MDP_SUCCESSFUL)
  {
    mdp_imglist_remove(node);
    return 0;
  }

  rex_enter_crit_sect(&mdp_crit_sect);
  switch (mdpImg->dest)
  {
    case DISPLAY_TYPE_1:
      mdpBITPtr = &mdpImgTailPtr_1[node->img.layer];
      mdp_list_total_1[node->img.layer]++;    
      break;

    case DISPLAY_TYPE_2:
      mdpBITPtr = &mdpImgTailPtr_2[node->img.layer];
      mdp_list_total_2[node->img.layer]++;  
      break;

#ifdef FEATURE_MDP2
#error code not present
#endif

    default:  
      mdp_imglist_remove(node);    
      rex_leave_crit_sect(&mdp_crit_sect);      
      return 0;
  }

#ifndef FEATURE_MDP2
  if (node->img.layer == MDP_LAYER_1)
  {
    if (*mdpBITPtr != NULL)
    {
      mdp_imglist_remove(node);
      rex_leave_crit_sect(&mdp_crit_sect);      
      return 0;    
    }
    // to prevent the system to remove this overlay
    ((MDPIMG_LIST *)mdpWhiteBgOvId[node->img.dest])->key_id = MDP_KEYID;      

    mdp_enable_locked_ovimg_API(mdpWhiteBgOvId[mdpImg->dest], TRUE);        

    ((MDPIMG_LIST *)mdpWhiteBgOvId[node->img.dest])->key_id = 0;         
    // we need to set layer 1 here in order to bypass alpha blending
    // For layer 1, we're under memcpy mode and need to disable alphab
    *mdpBITPtr = node;      
    mdp_sub_layer_update(mdpImg, node);    
    *mdpBITPtr = NULL;          
  }
#endif

#ifdef DISP_TASK_BUFFER_SHARING
  node->disp_packet = d_packet;
#endif  


  // enable and unlock it at default
  node->enabled = TRUE;
  node->locked = FALSE;
  mdp_imglist_append(*mdpBITPtr, node);
  *mdpBITPtr = node;  
  rex_leave_crit_sect(&mdp_crit_sect);

  return(uint32)node;
}

/*===========================================================================

FUNCTION mdp_replace_ovimg()

DESCRIPTION
  This function is to replace the specified overlay image with the new image

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  MDP_SUCCESSFUL if successful otherwise MDP_FAIL

SIDE EFFECTS
  None

===========================================================================*/
uint32 mdp_replace_ovimg(MDPIMG *mdpImg, uint32 id)
{
  MDPIMG_LIST *node;

  if (mdp_drv_initialized != TRUE)
    return 0;

  if (mdpImg == NULL)
    return 0;

  if (mdp_verify_img(mdpImg) != MDP_SUCCESSFUL)
    return 0;

  if ((id == 0) || (id==0xffffffff))
  {
    return mdp_register_ovimg(mdpImg);
  }
  else
  {
    node = (MDPIMG_LIST *)id;
    if (node->key_id != MDP_KEYID)
      return 0;

    /* It needs to be on the same layer.  otherwise, the image 
       tail pointer will not get updated at deregisteration */
    if (node->img.layer != mdpImg->layer)
      return 0;

#ifdef DISP_TASK_BUFFER_SHARING
    if (node->disp_packet != NULL)
    {
      boolean node_free;

      node_free = FALSE;
      TASKLOCK();
      if (node->disp_packet->done)
        node_free = TRUE;
      else
        node->disp_packet->registered = FALSE;
      TASKFREE();

      if (node_free)
        disp_imglist_free(node->disp_packet);

      node->disp_packet = NULL;
    }

    if ((mdpImg->mdpOp&MDPOP_ASCALE) || (mdpImg->mdpOp&MDPOP_QUEUE_COPY))
    {
      disp_packet_type * mdp_ascale_packet;
      uint32 ret;

      mdpImg->mdpOp |= MDPOP_QUEUE_COPY;

      // to have one display packet for IPL conversion storage      
      if ((mdp_ascale_packet = mdp_disp_packet_proc(mdpImg)) == NULL)
        return NULL;

      // to replace with the above display packet    
      ret = mdp_replace_ovimg_sub(&mdp_ascale_packet->mdpImg, id, mdp_ascale_packet);        
      if (ret == NULL)
      {
        disp_imglist_free(mdp_ascale_packet);        
      }
      else
      {
       ((MDPIMG_LIST *)ret)->orig_img_before_queue_copy = *mdpImg;      
        mdp_ascale_packet->registered =  TRUE;
      }

      return ret;
    }
#endif

    return mdp_replace_ovimg_sub(mdpImg, id, NULL);
  }
}

/*===========================================================================

FUNCTION mdp_replace_ovimg_sub()

DESCRIPTION
  This function is to handle mdp_replace_ovimg()

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  MDP_SUCCESSFUL if successful otherwise MDP_FAIL

SIDE EFFECTS
  None

===========================================================================*/
uint32 mdp_replace_ovimg_sub_API(MDPIMG *mdpImg, uint32 id, disp_packet_type * d_packet)
{
  MDPIMG_LIST *node;

  node = (MDPIMG_LIST *)id;

  rex_enter_crit_sect(&mdp_crit_sect);

  node->img = *mdpImg;
  node->orig_img = *mdpImg;
  node->img.lcd_x += MDP_SYS_OFFSET_X;
  node->img.lcd_y += MDP_SYS_OFFSET_Y;
#ifdef FEATURE_BREW      
#ifndef FEATURE_MDP2  
  node->appCtx = (void *) AEE_GetAppContext(); 
#endif
#endif

  mdp_touch_img(node);

  if (mdp_verify_dispArea(&node->img) != MDP_SUCCESSFUL)
  {
    mdp_deregister_ovimg_API((uint32)node);
    rex_leave_crit_sect(&mdp_crit_sect);      
    return 0;
  }

#ifndef FEATURE_MDP2        
  if (node->img.layer == MDP_LAYER_1)
  {
    mdp_sub_layer_update(mdpImg, node);          
  }
#endif

#ifdef DISP_TASK_BUFFER_SHARING  
  node->disp_packet = d_packet;
#endif

  rex_leave_crit_sect(&mdp_crit_sect);

  return(uint32)node;
}


/*===========================================================================

FUNCTION mdp_enable_ovimg()

DESCRIPTION
  This function is to enable/disable the specified overlay image
  At the default, the overlay image is enabled from mdp_register_ovimg()

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  Returns the registration ID if successful, otherwise returns NULL

SIDE EFFECTS
  None

===========================================================================*/
MDP_STATUS mdp_enable_ovimg_API(uint32 id, int32 flag)
{
  MDPIMG_LIST *node;

  if (mdp_drv_initialized != TRUE)
    return MDP_FAIL;

  if ((id==0) || (id==0xffffffff))
    return MDP_FAIL;

  rex_enter_crit_sect(&mdp_crit_sect); 

  node = (MDPIMG_LIST *)id;
  if (node->key_id != MDP_KEYID)
  {
    rex_leave_crit_sect(&mdp_crit_sect);
    return MDP_FAIL;
  }

  if (node->locked != TRUE)
  {
    node->enabled = flag;
  }

  rex_leave_crit_sect(&mdp_crit_sect);

  return MDP_SUCCESSFUL;
}

/*===========================================================================

FUNCTION mdp_enable_all_ovimg()

DESCRIPTION
  This function is to enable/disable all overlay images in all layers

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdp_enable_all_ovimg_API(LCD_TYPE dest, int32 flag)
{
  MDP_LAYER i;

  if (mdp_drv_initialized != TRUE)
    return;

  rex_enter_crit_sect(&mdp_crit_sect);    
  for (i=MDP_LAYER_0;i<MDP_MAX_LAYER;i++)
  {
    mdp_enable_all_ovimg_in_layer_API(dest, flag, i);
  }
  rex_leave_crit_sect(&mdp_crit_sect);    
}

/*===========================================================================

FUNCTION mdp_enable_all_ovimg_in_layer()

DESCRIPTION
  This function is to enable/disable all overlay images in the specified layer

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdp_enable_all_ovimg_in_layer_API(LCD_TYPE dest, int32 flag, MDP_LAYER layer)
{
  MDPIMG_LIST *node;

  if (mdp_drv_initialized != TRUE)
    return;

  if (MDP_INVALID_LAYER(layer))
    return;

  rex_enter_crit_sect(&mdp_crit_sect);      

  switch (dest)
  {
    case DISPLAY_TYPE_1:
      node = mdpImgTailPtr_1[layer];
      break;

    case DISPLAY_TYPE_2:
      node = mdpImgTailPtr_2[layer];
      break;

#ifdef FEATURE_MDP2
#error code not present
#endif

    default:  
      rex_leave_crit_sect(&mdp_crit_sect);      
      return;
  }

  while (node != NULL)
  {
    mdp_enable_ovimg_API((uint32)node, flag);
    node = node->prev;
  }
  rex_leave_crit_sect(&mdp_crit_sect);    
}


/*===========================================================================

FUNCTION mdp_lock_ovimg()

DESCRIPTION
  This function is to lock an overlay image.  This lock will prevent apps
  from disabling/deregistering system overlay images.

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  MDP_SUCCESSFUL if successful otherwise MDP_FAIL

SIDE EFFECTS
  None

===========================================================================*/
MDP_STATUS mdp_lock_ovimg_API(uint32 id, int32 flag)
{
  MDPIMG_LIST *node;

  if (mdp_drv_initialized != TRUE)
    return MDP_FAIL;

  if ((id==0) || (id==0xffffffff))
    return MDP_FAIL;

  node = (MDPIMG_LIST *)id;

  if (node->key_id != MDP_KEYID)
    return MDP_FAIL;

  node->locked = flag;

  return MDP_SUCCESSFUL;
}

/*===========================================================================

FUNCTION mdp_lock_all_ovimg()

DESCRIPTION
  This function is to lock all overlay images.

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdp_lock_all_ovimg_API(LCD_TYPE dest, int32 flag)
{
  MDPIMG_LIST *node;
  int i;

  if (mdp_drv_initialized != TRUE)
    return;

  rex_enter_crit_sect(&mdp_crit_sect);  
  for (i=0;i<MDP_MAX_LAYER;i++)
  {
    switch (dest)
    {
      case DISPLAY_TYPE_1:
        node = mdpImgTailPtr_1[i];
        break;

      case DISPLAY_TYPE_2:
        node = mdpImgTailPtr_2[i];
        break;

#ifdef FEATURE_MDP2
#error code not present
#endif     
      default:  
        rex_leave_crit_sect(&mdp_crit_sect);      
        return;
    }

    while (node != NULL)
    {
      mdp_lock_ovimg_API((uint32)node, flag);
      node = node->prev;
    }
  }
  rex_leave_crit_sect(&mdp_crit_sect);  
}

/*===========================================================================

FUNCTION mdp_enable_locked_ovimg()

DESCRIPTION
  This function is to enable/disable locked overlay image

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdp_enable_locked_ovimg_API(uint32 id, int32 flag)
{
  if (mdp_drv_initialized != TRUE)
    return;

  rex_enter_crit_sect(&mdp_crit_sect);  

  if (mdp_lock_ovimg_API(id, FALSE) == MDP_SUCCESSFUL)
  {
    mdp_enable_ovimg_API(id, flag);
    mdp_lock_ovimg_API(id, TRUE);
  }

  rex_leave_crit_sect(&mdp_crit_sect);  
}

/*===========================================================================

FUNCTION mdp_enable_all_locked_ovimg()

DESCRIPTION
  This function is to enable/disable all locked overlay image

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdp_enable_all_locked_ovimg_API(LCD_TYPE dest, int32 flag)
{
  if (mdp_drv_initialized != TRUE)
    return;

  rex_enter_crit_sect(&mdp_crit_sect);  

  mdp_lock_all_ovimg_API(dest, FALSE);
  mdp_enable_all_ovimg_API(dest, flag);
  mdp_lock_all_ovimg_API(dest, TRUE);  

  rex_leave_crit_sect(&mdp_crit_sect);  
}

/*===========================================================================

FUNCTION mdp_deregister_locked_ovimg()

DESCRIPTION
  This function is to deregister locked overlay image

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdp_deregister_locked_ovimg_API(uint32 id)
{
  if (mdp_drv_initialized != TRUE)
    return;

  rex_enter_crit_sect(&mdp_crit_sect);  

  if (mdp_lock_ovimg_API(id, FALSE) == MDP_SUCCESSFUL)
  {
    mdp_deregister_ovimg_API(id);
  }

  rex_leave_crit_sect(&mdp_crit_sect);  
}

/*===========================================================================

FUNCTION mdp_deregister_all_locked_ovimg()

DESCRIPTION
  This function is to deregister all locked overlay image

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdp_deregister_all_locked_ovimg_API(LCD_TYPE dest)
{
  if (mdp_drv_initialized != TRUE)
    return;

  rex_enter_crit_sect(&mdp_crit_sect);  

  mdp_lock_all_ovimg_API(dest, FALSE);
  mdp_deregister_all_ovimg_API(dest);  

  rex_leave_crit_sect(&mdp_crit_sect);  
}

/*===========================================================================

FUNCTION mdp_deregister_ovimg()

DESCRIPTION
  This function is to remove the specified overlay image

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdp_deregister_ovimg_API(uint32 id)
{
  MDPIMG_LIST *node;


  if (mdp_drv_initialized != TRUE)
    return;

  if ((id==0) || (id==0xffffffff))
    return;

  node = (MDPIMG_LIST *)id;

  if (node->key_id != MDP_KEYID)
    return;

  if (node->locked == TRUE)
    return;


  rex_enter_crit_sect(&mdp_crit_sect);

  // all id with the valid key_id should contain a valid layer
  switch (node->img.dest)
  {
    case DISPLAY_TYPE_1:
      mdp_list_total_1[node->img.layer]--;
      if (mdpImgTailPtr_1[node->img.layer] == node)
        mdpImgTailPtr_1[node->img.layer] = node->prev;
      break;

    case DISPLAY_TYPE_2:
      mdp_list_total_2[node->img.layer]--;
      if (mdpImgTailPtr_2[node->img.layer] == node)
        mdpImgTailPtr_2[node->img.layer] = node->prev;
      break;

#ifdef FEATURE_MDP2
#error code not present
#endif

    default:  
      rex_leave_crit_sect(&mdp_crit_sect);      
      return;
  }

#ifndef FEATURE_MDP2
  if (node->img.layer == MDP_LAYER_1)
  {
    ((MDPIMG_LIST *)mdpWhiteBgOvId[node->img.dest])->key_id = MDP_KEYID;      
    mdp_enable_locked_ovimg_API(mdpWhiteBgOvId[node->img.dest], FALSE);        

    // to prevent the system to remove this overlay
    ((MDPIMG_LIST *)mdpWhiteBgOvId[node->img.dest])->key_id = 0;      
  }
#endif

#ifdef DISP_TASK_BUFFER_SHARING  
  if (node->disp_packet != NULL)
  {
    boolean node_free;

    node_free = FALSE;
    TASKLOCK();
    if (node->disp_packet->done)
      node_free = TRUE;
    else
      node->disp_packet->registered = FALSE;
    TASKFREE();  

    if (node_free)
      disp_imglist_free(node->disp_packet);

    node->disp_packet = NULL;
  }
#endif  

  mdp_imglist_remove(node);
  rex_leave_crit_sect(&mdp_crit_sect);
}

/*===========================================================================

FUNCTION mdp_deregister_all_ovimg()

DESCRIPTION
  This function is to remove all overlay images in all layers

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdp_deregister_all_ovimg_API(LCD_TYPE dest)
{
  MDP_LAYER i;

  if (mdp_drv_initialized != TRUE)
    return;

  rex_enter_crit_sect(&mdp_crit_sect);

  for (i=MDP_LAYER_0;i<MDP_MAX_LAYER;i++)
  {
    mdp_deregister_all_ovimg_in_layer_API(dest, i);
  }

  rex_leave_crit_sect(&mdp_crit_sect);
}

/*===========================================================================

FUNCTION mdp_deregister_all_ovimg_in_layer()

DESCRIPTION
  This function is to remove all overlay images in the specified layer

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdp_deregister_all_ovimg_in_layer_API(LCD_TYPE dest, MDP_LAYER layer)
{
  MDPIMG_LIST *node, *prev_node;

  if (mdp_drv_initialized != TRUE)
    return;

  if (MDP_INVALID_LAYER(layer))
  {
    return;
  }

  rex_enter_crit_sect(&mdp_crit_sect);

  switch (dest)
  {
    case DISPLAY_TYPE_1:
      node = mdpImgTailPtr_1[layer];
      break;

    case DISPLAY_TYPE_2:
      node = mdpImgTailPtr_2[layer];  
      break;

#ifdef FEATURE_MDP2
#error code not present
#endif

    default:  
      rex_leave_crit_sect(&mdp_crit_sect);      
      return;
  }

  while (node != NULL)
  {
    prev_node = node->prev;
    mdp_deregister_ovimg_API((uint32)node);
    node = prev_node;
  }
  rex_leave_crit_sect(&mdp_crit_sect);  
}

/*===========================================================================

FUNCTION mdp_imglist_remove()

DESCRIPTION
  This function is to remove a mdpImg list node

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  Previous and next nodes are linked together

===========================================================================*/
void mdp_imglist_remove(MDPIMG_LIST *node)
{
  rex_enter_crit_sect(&mdp_crit_sect);
  if (node != NULL)
  {
    if (node->prev != NULL)
    {
      node->prev->next = node->next;
    }
    if (node->next != NULL)
    {
      node->next->prev = node->prev;
    }

    mdp_imglist_free(node);
  }
  rex_leave_crit_sect(&mdp_crit_sect);    
}

/*===========================================================================

FUNCTION mdp_imglist_append()

DESCRIPTION
  This function is to append a node to the list

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdp_imglist_append(MDPIMG_LIST *bgPtr, MDPIMG_LIST *node)
{
  rex_enter_crit_sect(&mdp_crit_sect);
  if ((bgPtr != NULL) && (node != NULL))
  {
    if (bgPtr == node)
    {
      MSG_ERROR("mdp_imglist_append error !!! -> two same node (0x%x)",node,0,0);   
      rex_leave_crit_sect(&mdp_crit_sect);      
      return;   
    }

    bgPtr->next = node;
    node->prev = bgPtr;
    node->next = NULL;
  }
  rex_leave_crit_sect(&mdp_crit_sect);      
}

/*===========================================================================

FUNCTION mdp_imglist_insert_back()

DESCRIPTION
  This function is to insert a node to the back of the list

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdp_imglist_insert_back(MDPIMG_LIST *list, MDPIMG_LIST *node)
{
  rex_enter_crit_sect(&mdp_crit_sect);
  if ((list != NULL) && (node != NULL))
  {
    node->next = list->next;
    if (list->next != NULL)
    {
      list->next->prev = node;
    }

    list->next = node;
    node->prev = list;
  }
  rex_leave_crit_sect(&mdp_crit_sect);  
}

/*===========================================================================

FUNCTION mdp_imglist_insert_front()

DESCRIPTION
  This function is to insert a node to the front of the list

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdp_imglist_insert_front(MDPIMG_LIST *list, MDPIMG_LIST *node)
{
  rex_enter_crit_sect(&mdp_crit_sect);
  if ((list != NULL) && (node != NULL))
  {
    node->prev = list->prev;
    if (list->prev != NULL)
    {
      list->prev->next = node;
    }

    list->prev = node;
    node->next = list;
  }
  rex_leave_crit_sect(&mdp_crit_sect);
}

/*===========================================================================

FUNCTION mdp_imglist_get()

DESCRIPTION
  This function is to have a free node from the mdpImg list

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  mdpImg list node if successful otherwise NULL

SIDE EFFECTS
  None

===========================================================================*/
MDPIMG_LIST * mdp_imglist_get()
{
  MDPIMG_LIST *node;

  rex_enter_crit_sect(&mdp_crit_sect);  
  if (mdpImgListTailPtr != NULL)
  {
    mdp_imglist_total--;
    node = mdpImgListTailPtr;
    node->key_id = MDP_KEYID;

    if (mdpImgListTailPtr->prev != NULL)
    {
      mdpImgListTailPtr->prev->next = NULL;
    }
    mdpImgListTailPtr = mdpImgListTailPtr->prev;

    node->prev = NULL;
    node->next = NULL;
  }
  else
  {
    node = NULL;
  }
  rex_leave_crit_sect(&mdp_crit_sect);  

  return node;
}

/*===========================================================================

FUNCTION mdp_imglist_free()

DESCRIPTION
  This function is to return a node to the mdpImg list

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdp_imglist_free(MDPIMG_LIST *node)
{
  rex_enter_crit_sect(&mdp_crit_sect);  

  mdp_imglist_total++;

  node->key_id = 0;
  node->enabled = FALSE;
  node->locked = FALSE;
#ifdef FEATURE_BREW
#ifndef FEATURE_MDP2  
  node->appCtx = NULL;
#endif  
#endif

  if (mdpImgListTailPtr == NULL)
  {
    mdpImgListTailPtr = node;
    mdpImgListTailPtr->prev = NULL;
    mdpImgListTailPtr->next = NULL;
  }
  else
  {
    node->prev = mdpImgListTailPtr;
    node->next = NULL;
    mdpImgListTailPtr->next = node;
    mdpImgListTailPtr = node;
  }

  rex_leave_crit_sect(&mdp_crit_sect);    
}

/*===========================================================================

FUNCTION mdp_imglist_purge()

DESCRIPTION
  This function is to free all nodes from the head list

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdp_imglist_purge(MDPIMG_LIST *head)
{
  MDPIMG_LIST *node;

  rex_enter_crit_sect(&mdp_crit_sect);
  while (head != NULL)
  {
    node = head;
    head = head->next;
    mdp_imglist_free(node);
  }
  rex_leave_crit_sect(&mdp_crit_sect);  
}

/*===========================================================================

FUNCTION mdp_imglist_init()

DESCRIPTION
  This function is to initialize the MDP image list

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdp_imglist_init()
{
  int i;
  MDPIMG_LIST *node;
  LCD_TYPE idest;

  rex_init_crit_sect(&mdp_async_crit_sect);
  rex_init_crit_sect(&mdp_disp_API_crit_sect);  
  rex_init_crit_sect(&mdp_disp_get_cb_arg_crit_sect);

  mdp_proxy_cb.arg = (void *)0;
  mdp_proxy_cb.cb_func = &mdp_proxy_cb_func;

  for (i=0;i<MDP_MAX_LAYER;i++)
  {
    mdpImgTailPtr_1[i] = NULL;
    mdpImgTailPtr_2[i] = NULL;
    mdp_list_total_1[i] = 0;
    mdp_list_total_2[i] = 0;

#ifdef FEATURE_MDP2
#error code not present
#endif    
  }

  mdpImgListTailPtr = NULL;
  mdp_imglist_total = 0;

  for (i=0;i<MDP_MAX_IMGLIST;i++)
  {
    node = &mdpImgList[i];
    node->prev = NULL;
    node->next = NULL;
    mdp_imglist_free(node);
  }

#ifdef FEATURE_BREW
#ifndef FEATURE_MDP2
  for (i=0;i<MDP_MAX_SYSCTX;i++)
  {
    mdpSysCtx[i] = (void *)0xDEADDEAD;
  }
#endif
#endif

  for (idest=DISPLAY_TYPE_1;idest<MAX_LCD_NUM;idest++)
  {
    mdp_active_dest[idest] = idest;
    mdp_dest_power_on[idest] = TRUE;
  }
}


/*===========================================================================
FUNCTION mdp_register_ovimg() API wrapper
===========================================================================*/
uint32 mdp_register_ovimg_sub(MDPIMG *mdpImg, disp_packet_type * d_packet)
{
  uint32 arg[2], ret;

  if (mdp_drv_initialized != TRUE)
  {
    MSG_ERROR("MDP driver is not initialized",0,0,0);  
    return 0;
  }
  
  if (disp_task_initialized != TRUE)
    return mdp_register_ovimg_sub_API(mdpImg, d_packet);

  arg[0] = (uint32) mdpImg;      
  arg[1] = (uint32) d_packet;
  ret = (uint32) mdp_send_disp_API_packet(MDP_REGISTER_OVIMG_SUB, arg, 2);  

  return ret;
}

/*===========================================================================
FUNCTION mdp_register_ovimg() API wrapper
===========================================================================*/
uint32 mdp_replace_ovimg_sub(MDPIMG *mdpImg, uint32 id, disp_packet_type * d_packet)
{
  uint32 arg[3], ret;  

  if (mdp_drv_initialized != TRUE)
  {
    MSG_ERROR("MDP driver is not initialized",0,0,0);  
    return 0;
  }
  
  if (disp_task_initialized != TRUE)
    return mdp_replace_ovimg_sub_API(mdpImg, id, d_packet);

  arg[0] = (uint32) mdpImg;      
  arg[1] = (uint32) id;  
  arg[2] = (uint32) d_packet;
  ret = (uint32) mdp_send_disp_API_packet(MDP_REPLACE_OVIMG_SUB, arg, 3);    

  return ret;
}

/*===========================================================================
FUNCTION mdp_register_ovimg() API wrapper
===========================================================================*/
void mdp_deregister_ovimg(uint32 id)
{
  uint32 arg;

  if (mdp_drv_initialized != TRUE)
  {
    MSG_ERROR("MDP driver is not initialized",0,0,0);    
    return;
  }
  
  if (disp_task_initialized != TRUE)
  {
    mdp_deregister_ovimg_API(id);
    return;
  }

  arg = (uint32) id;      
  mdp_send_disp_API_packet(MDP_DEREGISTER_OVIMG, &arg, 1);  
}

/*===========================================================================
FUNCTION mdp_register_ovimg() API wrapper
===========================================================================*/
void mdp_deregister_all_ovimg(LCD_TYPE dest)
{
  uint32 arg;

  if (mdp_drv_initialized != TRUE)
  {
    MSG_ERROR("MDP driver is not initialized",0,0,0);    
    return;
  }
  
  if (disp_task_initialized != TRUE)
  {
    mdp_deregister_all_ovimg_API(dest);
    return;
  }

  arg = (uint32) dest;      
  mdp_send_disp_API_packet(MDP_DEREGISTER_ALL_OVIMG, &arg, 1);  
}

/*===========================================================================
FUNCTION mdp_register_ovimg() API wrapper
===========================================================================*/
void mdp_deregister_all_ovimg_in_layer(LCD_TYPE dest, MDP_LAYER layer)
{
  uint32 arg[2];  

  if (mdp_drv_initialized != TRUE)
  {
    MSG_ERROR("MDP driver is not initialized",0,0,0);  
    return;
  }
  
  if (disp_task_initialized != TRUE)
  {
    mdp_deregister_all_ovimg_in_layer_API(dest, layer);
    return;
  }

  arg[0] = (uint32) dest;      
  arg[1] = (uint32) layer;  
  mdp_send_disp_API_packet(MDP_DEREGISTER_ALL_OVIMG_IN_LAYER, arg, 2);    
}

/*===========================================================================
FUNCTION mdp_register_ovimg() API wrapper
===========================================================================*/
MDP_STATUS mdp_enable_ovimg(uint32 id, int32 flag)
{
  uint32 arg[2];
  MDP_STATUS ret;  

  if (mdp_drv_initialized != TRUE)
  {
    MSG_ERROR("MDP driver is not initialized",0,0,0);
    return MDP_FAIL;
  }
  
  if (disp_task_initialized != TRUE)
    return mdp_enable_ovimg_API(id, flag);

  arg[0] = (uint32) id;      
  arg[1] = (uint32) flag;  
  ret = (MDP_STATUS) mdp_send_disp_API_packet(MDP_ENABLE_OVIMG, arg, 2);    

  return ret;
}

/*===========================================================================
FUNCTION mdp_register_ovimg() API wrapper
===========================================================================*/
void mdp_enable_all_ovimg(LCD_TYPE dest, int32 flag)
{
  uint32 arg[2];

  if (mdp_drv_initialized != TRUE)
  {
    MSG_ERROR("MDP driver is not initialized",0,0,0);
    return;
  }
  
  if (disp_task_initialized != TRUE)
  {
    mdp_enable_all_ovimg_API(dest, flag);
    return;
  }

  arg[0] = (uint32) dest;      
  arg[1] = (uint32) flag;  
  mdp_send_disp_API_packet(MDP_ENABLE_ALL_OVIMG, arg, 2);    
}

/*===========================================================================
FUNCTION mdp_register_ovimg() API wrapper
===========================================================================*/
void mdp_enable_all_ovimg_in_layer(LCD_TYPE dest, int32 flag, MDP_LAYER layer)
{
  uint32 arg[3];

  if (mdp_drv_initialized != TRUE)
  {
    MSG_ERROR("MDP driver is not initialized",0,0,0);
    return;
  }
  
  if (disp_task_initialized != TRUE)
  {
    mdp_enable_all_ovimg_in_layer_API(dest, flag, layer);
    return;
  }

  arg[0] = (uint32) dest;      
  arg[1] = (uint32) flag;  
  arg[2] = (uint32) layer;
  mdp_send_disp_API_packet(MDP_ENABLE_ALL_OVIMG_IN_LAYER, arg, 3);    
}

/*===========================================================================
FUNCTION mdp_register_ovimg() API wrapper
===========================================================================*/
MDP_STATUS mdp_lock_ovimg(uint32 id, int32 flag)
{
  uint32 arg[2];
  MDP_STATUS ret;  

  if (mdp_drv_initialized != TRUE)
  {
    MSG_ERROR("MDP driver is not initialized",0,0,0);
    return MDP_FAIL;
  }
  
  if (disp_task_initialized != TRUE)
    return mdp_lock_ovimg_API(id, flag);

  arg[0] = (uint32) id;      
  arg[1] = (uint32) flag;  
  ret = (MDP_STATUS) mdp_send_disp_API_packet(MDP_LOCK_OVIMG, arg, 2);  

  return ret;    
}

/*===========================================================================
FUNCTION mdp_register_ovimg() API wrapper
===========================================================================*/
void mdp_lock_all_ovimg(LCD_TYPE dest, int32 flag)
{
  uint32 arg[2];

  if (mdp_drv_initialized != TRUE)
  {
    MSG_ERROR("MDP driver is not initialized",0,0,0);
    return;
  }
  
  if (disp_task_initialized != TRUE)
  {
    mdp_lock_all_ovimg_API(dest, flag);
    return;
  }

  arg[0] = (uint32) dest;      
  arg[1] = (uint32) flag;  
  mdp_send_disp_API_packet(MDP_LOCK_ALL_OVIMG, arg, 2);      
}

/*===========================================================================
FUNCTION mdp_register_ovimg() API wrapper
===========================================================================*/
void mdp_enable_locked_ovimg(uint32 id, int32 flag)
{
  uint32 arg[2];

  if (mdp_drv_initialized != TRUE)
  {
    MSG_ERROR("MDP driver is not initialized",0,0,0);
    return;
  }
  
  if (disp_task_initialized != TRUE)
  {
    mdp_enable_locked_ovimg_API(id, flag);  
    return;
  }

  arg[0] = (uint32) id;      
  arg[1] = (uint32) flag;  
  mdp_send_disp_API_packet(MDP_ENABLE_LOCKED_OVIMG, arg, 2);      
}

/*===========================================================================
FUNCTION mdp_register_ovimg() API wrapper
===========================================================================*/
void mdp_enable_all_locked_ovimg(LCD_TYPE dest, int32 flag)
{
  uint32 arg[2];

  if (mdp_drv_initialized != TRUE)
  {
    MSG_ERROR("MDP driver is not initialized",0,0,0);
    return;
  }
  
  if (disp_task_initialized != TRUE)
  {
    mdp_enable_all_locked_ovimg_API(dest, flag);  
    return;
  }

  arg[0] = (uint32) dest;      
  arg[1] = (uint32) flag;  
  mdp_send_disp_API_packet(MDP_ENABLE_ALL_LOCKED_OVIMG, arg, 2);      
}

/*===========================================================================
FUNCTION mdp_register_ovimg() API wrapper
===========================================================================*/
void mdp_deregister_locked_ovimg(uint32 id)
{
  uint32 arg;

  if (mdp_drv_initialized != TRUE)
  {
    MSG_ERROR("MDP driver is not initialized",0,0,0);
    return;
  }
  
  if (disp_task_initialized != TRUE)
  {
    mdp_deregister_locked_ovimg_API(id);  
    return;
  }

  arg = (uint32) id;      
  mdp_send_disp_API_packet(MDP_DEREGISTER_LOCKED_OVIMG, &arg, 1);  
}

/*===========================================================================
FUNCTION mdp_register_ovimg() API wrapper
===========================================================================*/
void mdp_deregister_all_locked_ovimg(LCD_TYPE dest)
{
  uint32 arg;

  if (mdp_drv_initialized != TRUE)
  {
    MSG_ERROR("MDP driver is not initialized",0,0,0);
    return;
  }
  
  if (disp_task_initialized != TRUE)
  {
    mdp_deregister_all_locked_ovimg_API(dest);
    return;
  }

  arg = (uint32) dest;      
  mdp_send_disp_API_packet(MDP_DEREGISTER_ALL_LOCKED_OVIMG, &arg, 1);  
}

/*===========================================================================
FUNCTION mdp_register_ovimg() API wrapper
===========================================================================*/
void mdp_set_lcd_size(LCD_TYPE dest, LCD_TYPE_PHYS physDest, uint32 width, uint32 height)
{
  uint32 arg[4];

  if (mdp_drv_initialized != TRUE)
  {
    MSG_ERROR("MDP driver is not initialized",0,0,0);
    return;
  }
  
  if (disp_task_initialized != TRUE)
  {
    mdp_set_lcd_size_API(dest,physDest,width,height);
    return;
  }

  arg[0] = (uint32) dest;      
  arg[1] = (uint32) physDest;  
  arg[2] = (uint32) width;
  arg[3] = (uint32) height;
  mdp_send_disp_API_packet(MDP_SET_LCD_SIZE, arg, 4);      
}

    #ifdef FEATURE_MDP2
#error code not present
    #endif

    #ifdef FEATURE_MDP2
#error code not present
    #endif

/*===========================================================================

FUNCTION mdp_api_ioctl()

DESCRIPTION
  This function is to process the actual API request from display task.

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
int mdp_api_ioctl(MDP_API_IOCTL api, uint32 * arg)
{
  int ret=0;

  MDP_MSG_DEBUG("API IOCTL = %d", api,0,0);
  switch (api)
  {
    // user level APIs
    case MDP_REGISTER_OVIMG_SUB:
      ret = (int) mdp_register_ovimg_sub_API((MDPIMG *)arg[0], 
                                             (disp_packet_type *)arg[1]);   
      break;      
    case MDP_REPLACE_OVIMG_SUB:
      ret = (int) mdp_replace_ovimg_sub_API((MDPIMG *)arg[0], (uint32)arg[1],
                                            (disp_packet_type *)arg[2]);
      break;
    case MDP_DEREGISTER_OVIMG:
      mdp_deregister_ovimg_API((uint32) arg[0]);
      break;
    case MDP_DEREGISTER_ALL_OVIMG:
      mdp_deregister_all_ovimg_API((LCD_TYPE)arg[0]);
      break;
    case MDP_DEREGISTER_ALL_OVIMG_IN_LAYER:
      mdp_deregister_all_ovimg_in_layer_API((LCD_TYPE)arg[0], (MDP_LAYER)arg[1]);    
      break;
    case MDP_ENABLE_OVIMG:
      ret = mdp_enable_ovimg_API((uint32)arg[0], (int32)arg[1]);
      break;
    case MDP_ENABLE_ALL_OVIMG:
      mdp_enable_all_ovimg_API((LCD_TYPE)arg[0], (int32)arg[1]);    
      break;      
    case MDP_ENABLE_ALL_OVIMG_IN_LAYER:
      mdp_enable_all_ovimg_in_layer_API((LCD_TYPE)arg[0], (int32)arg[1], (MDP_LAYER)arg[2]);    
      break;

      // system level APIs
    case MDP_LOCK_OVIMG:
      ret = (int) mdp_lock_ovimg_API((uint32)arg[0], (int32)arg[1]);
      break;      
    case MDP_LOCK_ALL_OVIMG:
      mdp_lock_all_ovimg_API((LCD_TYPE)arg[0], (int32)arg[1]);    
      break;      
    case MDP_ENABLE_LOCKED_OVIMG:
      mdp_enable_locked_ovimg_API((uint32)arg[0], (int32)arg[1]);    
      break;      
    case MDP_ENABLE_ALL_LOCKED_OVIMG:
      mdp_enable_all_locked_ovimg_API((LCD_TYPE)arg[0], (int32)arg[1]);    
      break;

    case MDP_DEREGISTER_LOCKED_OVIMG:
      mdp_deregister_locked_ovimg_API((uint32)arg[0]);    
      break;

    case MDP_DEREGISTER_ALL_LOCKED_OVIMG:
      mdp_deregister_all_locked_ovimg_API((LCD_TYPE)arg[0]);
      break;

    case MDP_SET_LCD_SIZE:
      mdp_set_lcd_size_API((LCD_TYPE)arg[0], (LCD_TYPE_PHYS)arg[1], 
                           (uint32)arg[2], (uint32) arg[3]);
      break;

#ifdef FEATURE_MDP2
#error code not present
#endif

    default:
      break;
  }

  return ret;
}

#ifdef FEATURE_DISPLAY_PROFILING
/*===========================================================================

FUNCTION mdp_profiling_start()

DESCRIPTION
  This function starts MDP profiling.

DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void mdp_profiling_start(void)
{
  uint32 i;

  mdp_pf.enabled = TRUE;

  for (i=0; i<MDP_MAX_LAYER; i++)
  {
    mdp_pf.num_blending[i] = 0;
    mdp_pf.min_blending[i] = 0xFFFFFFFF;
    mdp_pf.max_blending[i] = 0;
    mdp_pf.avg_blending[i] = 0;
  }
  for (i=0; i<MAX_LCD_NUM; i++)            
  {
    mdp_pf.num_mapping[i] = 0;
    mdp_pf.min_mapping[i] = 0xFFFFFFFF;
    mdp_pf.max_mapping[i] = 0;
    mdp_pf.avg_mapping[i] = 0;
  }
  for (i=0; i<MAX_LCD_NUM; i++)
  {
    mdp_pf.num_update[i] = 0;
    mdp_pf.min_update[i] = 0xFFFFFFFF;
    mdp_pf.max_update[i] = 0;
    mdp_pf.avg_update[i] = 0;
  }
  mdp_pf.min_interframe = 0xFFFFFFFF;
  mdp_pf.max_interframe = 0;
  mdp_pf.avg_interframe = 0;
  mdp_pf.num_interframe = 0;
}

/*===========================================================================

FUNCTION mdp_profiling_stop()

DESCRIPTION
  This function stops MDP profiling & return profiling result to caller.

DEPENDENCIES
  mdp_profiling_start() is called to start a MDP profiling session and 
  initialize the associated mdp profiling structures

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void mdp_profiling_stop(MDP_PROFILE_RESULT *result)
{
  uint32 i;

  mdp_pf.enabled = FALSE;

  memset(result, 0, sizeof(MDP_PROFILE_RESULT));
  result->num_sample = mdp_pf.num_interframe;
  result->vsync_enabled = mdp_vsync_enabled;

  for (i=0; i<MDP_MAX_LAYER; i++)
  {
    result->num_blending[i] = mdp_pf.num_blending[i];
    result->avg_blending[i] = mdp_pf.avg_blending[i];
    if (mdp_pf.min_blending[i] != 0xFFFFFFFF)
      result->min_blending[i] = mdp_pf.min_blending[i];
    result->max_blending[i] = mdp_pf.max_blending[i];
  }
  for (i=0; i<MAX_LCD_NUM; i++)
  {
    result->num_mapping[i] = mdp_pf.num_mapping[i];
    result->avg_mapping[i] = mdp_pf.avg_mapping[i];
    if (mdp_pf.min_mapping[i] != 0xFFFFFFFF)
      result->min_mapping[i] = mdp_pf.min_mapping[i];
    result->max_mapping[i] = mdp_pf.max_mapping[i];

    result->num_update[i] = mdp_pf.num_update[i];
    result->avg_update[i] = mdp_pf.avg_update[i];
    if (mdp_pf.min_update[i] != 0xFFFFFFFF)
      result->min_update[i] = mdp_pf.min_update[i];
    result->max_update[i] = mdp_pf.max_update[i];
  }
  result->avg_interframe = mdp_pf.avg_interframe;
  if (mdp_pf.min_interframe != 0xFFFFFFFF)
    result->min_interframe = mdp_pf.min_interframe;
  result->max_interframe = mdp_pf.max_interframe;

  MDP_MSG_PROFILE("==============================",0,0,0);
  MDP_MSG_PROFILE("%d samples, vsync_enabled=%d", result->num_sample, result->vsync_enabled,0);
  MDP_MSG_PROFILE("avg interframe = %dusec (%d-%d)",result->avg_interframe,result->min_interframe,result->max_interframe);
  for (i=0; i<MDP_MAX_LAYER; i++)
  { 
    if (result->num_blending[i] > 0)
    {
      MDP_MSG_PROFILE("- - - - - - - - - - - - - -",0,0,0);
      MDP_MSG_PROFILE("Layer %d - total of %d blendings occurred",i,result->num_blending[i],0);
      MDP_MSG_PROFILE("blending time = %dusec (%d-%d)",result->avg_blending[i],result->min_blending[i],result->max_blending[i]);
    }
  }
  for (i=0; i<MAX_LCD_NUM; i++)
  { 
    if ((result->num_mapping[i] > 0) || (result->num_update[i] > 0))
    {
      MDP_MSG_PROFILE("- - - - - - - - - - - - - -",0,0,0);
      if (result->num_mapping[i] > 0)
      {
        MDP_MSG_PROFILE("LCD #%d - total of %d occurrences",i,result->num_mapping[i],0);
      }
      else 
      {
        MDP_MSG_PROFILE("LCD #%d - total of %d occurrences",i,result->num_update[i],0);
      }
    }
    if (result->num_mapping[i] > 0)
      MDP_MSG_PROFILE("mapping time = %dusec (%d-%d)",result->avg_mapping[i],result->min_mapping[i],result->max_mapping[i]);
    if (result->num_update[i] > 0)
      MDP_MSG_PROFILE("update time = %dusec (%d-%d)",result->avg_update[i],result->min_update[i],result->max_update[i]);
  }
  MDP_MSG_PROFILE("==============================",0,0,0);
}

/*===========================================================================

FUNCTION mdp_profiling_calc_interframe()

DESCRIPTION
  This function calculates interframe times during a MDP profiling.

DEPENDENCIES
  mdp_profiling_start() is called to start a MDP profiling session and 
  initialize the associated mdp profiling structures

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void mdp_profiling_calc_interframe(MDPIMG * mdpImg)
{
  timetick_type interframe_time = 0;

  if ((mdp_pf.enabled) && (mdpImg->mdpOp & MDPOP_PROFILING))
  {
    if (mdp_pf.num_interframe > 0)
    {
      interframe_time = timetick_get_elapsed(mdp_pf.prev_time, T_USEC);

      if (interframe_time < mdp_pf.min_interframe)
        mdp_pf.min_interframe = interframe_time;
      if (interframe_time > mdp_pf.max_interframe)
        mdp_pf.max_interframe = interframe_time;
      mdp_pf.avg_interframe = ((mdp_pf.avg_interframe * (mdp_pf.num_interframe-1)) + interframe_time) 
                            / mdp_pf.num_interframe;
    }
    mdp_pf.prev_time = timetick_get();
    mdp_pf.num_interframe++;
  }
}
#endif /* FEATURE_DISPLAY_PROFILING */


#endif /* (FEATURE_MDP) || (FEATURE_MDP2) */
