#ifndef MDP_API_PROC_H
#define MDP_API_PROC_H
/*============================================================================

                   MDP Device Driver Header File

  DESCRIPTION
  This file contains the definitions needed for the MDP device driver.

  Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/display/mdp1/mdp_api/main/latest/inc/mdp_api_proc.h#2 $ $DateTime: 2008/10/15 01:28:38 $ $Author: gauravs $

when      who    what, where, why
--------  -----  ----------------------------------------------------------
05/16/07  dw      Replace header file for including updated BREW offsets
03/20/07  cl      Added MDP profiling support
07/17/06  dw      Remove FEATURE_DISP_TASK
06/26/06  cjr     Added MDP_MAP_INFO under mdp_map_dest()
05/10/06  cjr     Added support for display API queue
03/13/06  cjr     Added ascale support for MDP1
10/03/05  cjr     Added MDPOP_BREW for all BREW MDP operation
02/10/05  cjr     Completed source file re-architect
02/08/05  cjr	    Initial code
============================================================================*/

//============================================================================
//                        INCLUDE FILES
//============================================================================
#ifndef FEATURE_WINCE
 #include "OEMFeatures.h"
#endif /* NOT FEATURE_WINCE */

#include "mdp_sys_api.h"

#ifndef DISPLAY_TYPE_1
#define DISPLAY_TYPE_1 PRIMARY_LCD_TYPE
#endif

#ifndef DISPLAY_TYPE_2
#define DISPLAY_TYPE_2 SECONDARY_LCD_TYPE
#endif

#ifndef DISPLAY_TYPE_3
#define DISPLAY_TYPE_3 (DISPLAY_TYPE_2+1)
#endif

#ifndef DISPLAY_TYPE_4
#define DISPLAY_TYPE_4 (DISPLAY_TYPE_3+1)
#endif

#ifdef FEATURE_UI_CORE_REMOVED
    #define MDP_SYS_OFFSET_X    0
    #define MDP_SYS_OFFSET_Y    0
#else
  #ifdef FEATURE_BREW
    #include "AEE_OEM.h"
  
    #define MDP_SYS_OFFSET_X    BREW_OFFSET_X
    #define MDP_SYS_OFFSET_Y    BREW_OFFSET_Y
  #else
    #define MDP_SYS_OFFSET_X    0
    #define MDP_SYS_OFFSET_Y    0
  #endif
#endif

#define MDPOP_BREW (MDPOP_QUEUE_COPY|MDPOP_VSYNC_OFF|MDPOP_QUEUE_EXPAND)

#ifdef FEATURE_MDP2
#error code not present
#endif

#define MDP_OVIMG_REFRESH  0x4

// MDP API IOCTL Enum
typedef enum
{
  // user level APIs
  MDP_REGISTER_OVIMG_SUB,
  MDP_REPLACE_OVIMG_SUB,
  MDP_DEREGISTER_OVIMG,
  MDP_DEREGISTER_ALL_OVIMG,
  MDP_DEREGISTER_ALL_OVIMG_IN_LAYER,
  MDP_ENABLE_OVIMG,
  MDP_ENABLE_ALL_OVIMG,
  MDP_ENABLE_ALL_OVIMG_IN_LAYER,
  
  // system level APIs
  MDP_LOCK_OVIMG,
  MDP_LOCK_ALL_OVIMG,
  MDP_ENABLE_LOCKED_OVIMG,
  MDP_ENABLE_ALL_LOCKED_OVIMG,
  MDP_DEREGISTER_LOCKED_OVIMG,
  MDP_DEREGISTER_ALL_LOCKED_OVIMG,
  MDP_MAP_DEST,  
#ifdef FEATURE_MDP2
#error code not present
#else
  MDP_SET_LCD_SIZE
#endif  
}MDP_API_IOCTL;

typedef struct mdp_cb_data_
{
  rex_tcb_type * tcb;
  MDP_STATUS result;
} MDP_CB_DATA;


#ifdef FEATURE_DISPLAY_PROFILING
typedef struct mdp_profile_type_
{
  boolean enabled;               // mdp profiling is enabled
  uint32 num_blending[MDP_MAX_LAYER]; // number of ppp times
  uint32 min_blending[MDP_MAX_LAYER]; // min blending time in usec
  uint32 max_blending[MDP_MAX_LAYER]; // max blending time in usec
  float avg_blending[MDP_MAX_LAYER];  // avg blending time in usec
  uint32 num_mapping[MAX_LCD_NUM];  // number of mapping times
  uint32 min_mapping[MAX_LCD_NUM];  // min mapping time in usec
  uint32 max_mapping[MAX_LCD_NUM];  // max mapping time in usec
  float avg_mapping[MAX_LCD_NUM];   // avg mapping time in usec
  uint32 num_update[MAX_LCD_NUM];  // number of update times
  uint32 min_update[MAX_LCD_NUM];  // min update time in usec
  uint32 max_update[MAX_LCD_NUM];  // max update time in usec
  float avg_update[MAX_LCD_NUM];   // avg update time in usec
  uint32 prev_time;              // previous frame time in usec
  uint32 num_interframe;         // number of interframe times
  uint32 min_interframe;         // min interframe time in usec
  uint32 max_interframe;         // max interframe time in usec
  float avg_interframe;          // avg interframe time in usec
} MDP_PROFILE_TYPE;
#endif /* FEATURE_DISPLAY_PROFILING */

/*===========================================================================
    LOCAL FUNCTIONS PROTOTYPES
============================================================================*/
void mdp_add_sysCtx_list(void);
void mdp_remove_sysCtx_list(void * sysCtx);
int mdp_is_sysCtx(void * appCtx);
uint32 mdp_verify_img(MDPIMG *mdpImg);
void mdp_imglist_remove(MDPIMG_LIST *node);
void mdp_imglist_append(MDPIMG_LIST *bgPtr, MDPIMG_LIST *node);
void mdp_imglist_insert_back(MDPIMG_LIST *list, MDPIMG_LIST *node);
void mdp_imglist_insert_front(MDPIMG_LIST *list, MDPIMG_LIST *node);
MDPIMG_LIST * mdp_imglist_get(void);
void mdp_imglist_free(MDPIMG_LIST *node);
void mdp_imglist_purge(MDPIMG_LIST *head);
void mdp_imglist_init(void);

boolean mdp_is_dest_active(LCD_TYPE dest);
boolean mdp_is_vdo_image(MDPIMG_TYPE imgType);

uint32 mdp_replace_ovimg_sub(MDPIMG *mdpImg, uint32 id, disp_packet_type * d_packet);
uint32 mdp_register_ovimg_sub(MDPIMG *mdpImg, disp_packet_type * d_packet);
MDP_STATUS mdp_disp_update_async_sub(MDPIMG * mdpImg, MDP_CB *cb, disp_packet_type *packet);

void mdp_proxy_cb_func(void *arg, MDP_STATUS status);
void mdp_task_release_cb(void *arg, MDP_STATUS status);
disp_packet_type * mdp_disp_packet_proc(MDPIMG * mdpImg);
int mdp_api_ioctl(MDP_API_IOCTL id, uint32 * arg);
MDP_CB_DATA * mdp_get_cb_arg(void);

uint32 mdp_register_ovimg_sub_API(MDPIMG *mdpImg, disp_packet_type * d_packet);
uint32 mdp_replace_ovimg_sub_API(MDPIMG *mdpImg, uint32 id, disp_packet_type * d_packet);
void mdp_deregister_ovimg_API(uint32 id);
void mdp_deregister_all_ovimg_API(LCD_TYPE dest);
void mdp_deregister_all_ovimg_in_layer_API(LCD_TYPE dest, MDP_LAYER layer);
MDP_STATUS mdp_enable_ovimg_API(uint32 id, int32 flag);
void mdp_enable_all_ovimg_API(LCD_TYPE dest, int32 flag);
void mdp_enable_all_ovimg_in_layer_API(LCD_TYPE dest, int32 flag, MDP_LAYER layer);

MDP_STATUS mdp_lock_ovimg_API(uint32 id, int32 flag);
void mdp_lock_all_ovimg_API(LCD_TYPE dest, int32 flag);
void mdp_enable_locked_ovimg_API(uint32 id, int32 flag);
void mdp_enable_all_locked_ovimg_API(LCD_TYPE dest, int32 flag);
void mdp_deregister_locked_ovimg_API(uint32 id);
void mdp_deregister_all_locked_ovimg_API(LCD_TYPE dest);

MDP_STATUS mdp_map_dest_API(LCD_TYPE src, LCD_TYPE dest, MDP_MAP_INFO *mapInfo);
void mdp_set_lcd_size_API(LCD_TYPE dest, LCD_TYPE_PHYS physDest, uint32 width, uint32 height);

#ifdef FEATURE_MDP2
#error code not present
#endif

#ifdef FEATURE_DISPLAY_PROFILING
void mdp_profiling_calc_interframe(MDPIMG * mdpImg);
#endif /* FEATURE_DISPLAY_PROFILING */

/*===========================================================================
============================================================================*/

/**************/
/* QXDM Flags */
/**************/
typedef struct _dbg_flags
{
  boolean qxdmEnAll;    /* =0 do nothing, else ignore flags, turn all on */

  /* THE FLAGS BELOW APPLY ONLY IF ABOVE FLAG IS SET TO ZERO */

  boolean qxdmEnFlags;  /* =0 disable all MSG, superceded by above flag */
  boolean qxdmFatal;    /* individual enable/disable each Msg level */
  boolean qxdmError;
  boolean qxdmHigh;
  boolean qxdmMed;
  boolean qxdmLow;
  boolean qxdmDebug;
  boolean qxdmProfile;
} dbg_flags;

extern dbg_flags mdp_debug;

/************************/
/* QXDM DEBUG MESSAGING */
/************************/
#ifdef MSG_SSID_APPS_MDP_GENERAL

#define MDP_MSG_FATAL(x,y,z,w)   {                               \
if ((mdp_debug.qxdmFatal && mdp_debug.qxdmEnFlags) ||            \
    (mdp_debug.qxdmEnAll != 0))                                  \
    MSG_3(MSG_SSID_APPS_MDP_GENERAL,MSG_MASK_4,x,y,z,w);         \
                                 }
#define MDP_MSG_ERROR(x,y,z,w)   {                               \
if ((mdp_debug.qxdmError && mdp_debug.qxdmEnFlags) ||            \
    (mdp_debug.qxdmEnAll != 0))                                  \
    MSG_3(MSG_SSID_APPS_MDP_GENERAL,MSG_MASK_3,x,y,z,w);         \
                                 }
#define MDP_MSG_HIGH(x,y,z,w)    {                               \
if ((mdp_debug.qxdmHigh && mdp_debug.qxdmEnFlags) ||             \
    (mdp_debug.qxdmEnAll != 0))                                  \
    MSG_3(MSG_SSID_APPS_MDP_GENERAL,MSG_MASK_2,x,y,z,w);         \
                                 }
#define MDP_MSG_MED(x,y,z,w)     {                               \
if ((mdp_debug.qxdmMed && mdp_debug.qxdmEnFlags) ||              \
    (mdp_debug.qxdmEnAll != 0))                                  \
    MSG_3(MSG_SSID_APPS_MDP_GENERAL,MSG_MASK_1,x,y,z,w);         \
                                 }
#define MDP_MSG_LOW(x,y,z,w)     {                               \
if ((mdp_debug.qxdmLow && mdp_debug.qxdmEnFlags) ||              \
    (mdp_debug.qxdmEnAll != 0))                                  \
    MSG_3(MSG_SSID_APPS_MDP_GENERAL,MSG_MASK_0,x,y,z,w);         \
                                 }
#define MDP_MSG_DEBUG(x,y,z,w)   {                               \
if ((mdp_debug.qxdmDebug && mdp_debug.qxdmEnFlags) ||            \
    (mdp_debug.qxdmEnAll != 0))                                  \
    MSG_3(MSG_SSID_APPS_MDP_GENERAL,MSG_MASK_5,x,y,z,w);         \
                                 }
#define MDP_MSG_PROFILE(x,y,z,w) {                               \
if ((mdp_debug.qxdmProfile && mdp_debug.qxdmEnFlags) ||          \
    (mdp_debug.qxdmEnAll != 0))                                  \
    MSG_3(MSG_SSID_APPS_MDP_GENERAL,MSG_MASK_6,x,y,z,w);         \
                                 }

#else  // MSG_SSID_APPS_MDP_GENERAL

#define MDP_MSG_FATAL(x,y,z,w)   {                               \
if ((mdp_debug.qxdmFatal && mdp_debug.qxdmEnFlags) ||            \
    (mdp_debug.qxdmEnAll != 0))                                  \
    MSG_FATAL(x,y,z,w);                                          \
                                 }
#define MDP_MSG_ERROR(x,y,z,w)   {                               \
if ((mdp_debug.qxdmError && mdp_debug.qxdmEnFlags) ||            \
    (mdp_debug.qxdmEnAll != 0))                                  \
    MSG_ERROR(x,y,z,w);                                          \
                                 }
#define MDP_MSG_HIGH(x,y,z,w)    {                               \
if ((mdp_debug.qxdmHigh && mdp_debug.qxdmEnFlags) ||             \
    (mdp_debug.qxdmEnAll != 0))                                  \
    MSG_HIGH(x,y,z,w);                                           \
                                 }
#define MDP_MSG_MED(x,y,z,w)     {                               \
if ((mdp_debug.qxdmMed && mdp_debug.qxdmEnFlags) ||              \
    (mdp_debug.qxdmEnAll != 0))                                  \
    MSG_HIGH(x,y,z,w);                                            \
                                 }
#define MDP_MSG_LOW(x,y,z,w)     {                               \
if ((mdp_debug.qxdmLow && mdp_debug.qxdmEnFlags) ||              \
    (mdp_debug.qxdmEnAll != 0))                                  \
    MSG_HIGH(x,y,z,w);                                            \
                                 }
#define MDP_MSG_DEBUG(x,y,z,w)   {                               \
if ((mdp_debug.qxdmDebug && mdp_debug.qxdmEnFlags) ||            \
    (mdp_debug.qxdmEnAll != 0))                                  \
    MSG_HIGH(x,y,z,w);                                            \
                                 }
#define MDP_MSG_PROFILE(x,y,z,w) {                               \
if ((mdp_debug.qxdmProfile && mdp_debug.qxdmEnFlags) ||          \
    (mdp_debug.qxdmEnAll != 0))                                  \
    MSG_HIGH(x,y,z,w);                                            \
                                 }

#endif // MSG_SSID_APPS_MDP_GENERAL
	
#endif /* MDP_API_PROC_H */

