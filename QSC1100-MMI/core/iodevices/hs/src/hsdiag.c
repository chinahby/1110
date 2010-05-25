/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Diagnostics Packet Processing

General Description
  Diagnostic packet processing routines for -LD

  Copyright (c) 2000-2008 QUALCOMM Incorporated. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

$Header: //depot/asic/qsc6055/services/hs/hsdiag.c#8 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/08/08   sg      Fixing File change history and Copyright info
06/16/08   pp      Fixed Klocwork errors
01/17/08   gdp     Fixed Truncation of display capture file name to
01/17/08   gdp     fix the QPST display capture utility CR 121471
07/18/07   kma     Fixed Compiler Warnings
05/16/07   gdp     Fixed Banned API
04/11/07   gmy     Conditionally compile disp_capture() calls
06/16/05   as      Added Dual processor Diag support.
03/08/05   ty      Added display capture support in HS
02/28/05   ar      Added HSKM_IKDELAY & HSKM_KPDELAY support.
04/22/04   jas     Removed call to appdiag_init, which was replaced by the
                   IDIAG extension.
10/28/03   as      Changes to fix warnings on ads12 compiler
08/27/03   rsm     Added clk_regime_register.. before calling lcd disp_capture
08/12/03   dle     In hsdisg_keymap_action, release the pressed key instead
                   of NULL key.
08/07/03   gr      Integration from the services/diag version
07/01/03   rsm     added display capture request/response/
01/08/03   ma      featurization for GSM PLT
12/11/02   pd      Added multiple key press support for simulate key input
11/06/01   jal     Fixed DIAG_HS_KEY_F support
08/20/01   jal     Updated for core VU
12/08/00   lad     Created file.
===========================================================================*/
#ifdef __cplusplus
  extern "C" {
#endif

#include "comdef.h"
#include "customer.h"
#include "diag.h"
#include "diagbuf.h"
#include "diagpkt.h"

#include "diagi.h"

#include "hs.h"
#include "hsdiag.h"
#include "rex.h"

#include "task.h"
#include "fs.h"
#include "disp.h"
#include "tmc.h"
#include "event.h"
#ifdef FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING
#include "clkregim.h"
#endif /* FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING */

#include "AEEstd.h"
#include <stdlib.h>

#ifdef FEATURE_HS_DISPLAY_CAPTURE
LOCAL hsdiag_disp_cap_type hsdiag_disp_cap_data={DISP_CAP_IDLE};
#endif

hs_key_type hsdiag_keymap_keys[] = {
  /* DIAG_KEY_NONE       */ HS_NONE_K,
  /* DIAG_KEY_POWER      */ HS_PWR_K,
  /* DIAG_KEY_UP         */ HS_UP_K,
  /* DIAG_KEY_DOWN       */ HS_DOWN_K,
#ifdef FEATURE_NAV_KEYPAD
  /* DIAG_KEY_LEFT       */ HS_LEFT_K,
  /* DIAG_KEY_RIGHT      */ HS_RIGHT_K,
#endif
  /* DIAG_KEY_UP_LEFT    */ HS_NONE_K,
  /* DIAG_KEY_UP_RIGHT   */ HS_NONE_K,
  /* DIAG_KEY_DOWN_LEFT  */ HS_NONE_K,
  /* DIAG_KEY_DOWN_RIGHT */ HS_NONE_K,
  /* DIAG_KEY_NAV_SEL    */ HS_NONE_K,
#if defined (FEATURE_NAV_KEYPAD) || defined (FEATURE_RESISTIVE_KEYPAD)
  /* DIAG_KEY_SEL        */ HS_SEL_K,
#endif
  /* DIAG_KEY_CLR        */ HS_CLR_K,
  /* DIAG_KEY_0          */ HS_0_K,
  /* DIAG_KEY_1          */ HS_1_K,
  /* DIAG_KEY_2          */ HS_2_K,
  /* DIAG_KEY_3          */ HS_3_K,
  /* DIAG_KEY_4          */ HS_4_K,
  /* DIAG_KEY_5          */ HS_5_K,
  /* DIAG_KEY_6          */ HS_6_K,
  /* DIAG_KEY_7          */ HS_7_K,
  /* DIAG_KEY_8          */ HS_8_K,
  /* DIAG_KEY_9          */ HS_9_K,
  /* DIAG_KEY_POUND      */ HS_POUND_K,
  /* DIAG_KEY_STAR       */ HS_STAR_K,
  /* DIAG_KEY_PF1        */ HS_PF1_K,
  /* DIAG_KEY_PF2        */ HS_PF2_K,
  /* DIAG_KEY_SEND       */ HS_SEND_K,
  /* DIAG_KEY_END        */ HS_END_K,
  /* DIAG_KEY_INFO       */ HS_INFO_K,
  /* DIAG_KEY_VOL_UP     */ HS_NONE_K, //?
  /* DIAG_KEY_VOL_DWN    */ HS_NONE_K  //?
};

typedef struct
{
  uint32 action;
  const uint32 *table;
} hsdiag_keymap_execute_type;

//TODO: Consider exposing this in diag.h.
extern void * diag_blocking_malloc (unsigned int num_bytes);

static void hsdiag_keymap_execute (void *action);
static void hsdiag_keymap_action (uint32 action, const uint32 * table);

#ifndef FEATURE_UI_CORE_REMOVED
#ifdef FEATURE_PORTMAP_TEST_AUTOMATION
  extern void portmapdiag_init( void );
#endif
#ifdef FEATURE_MEDIAPLAYER_TEST_AUTOMATION
  extern void mediaplayerdiag_init( void );
#endif
#ifdef FEATURE_QCAMERA_TEST_AUTOMATION
  extern void qcameradiag_init( void );
#endif
#ifdef FEATURE_AUDIOSETTINGS_TEST_AUTOMATION
  extern void audiodiag_init( void );
#endif
#ifdef FEATURE_MOBIMON_TEST_AUTOMATION
  extern void mobimondiag_init( void );
#endif
#ifdef FEATURE_GUNIMON_TEST_AUTOMATION
  extern void gunimondiag_init( void );
#endif
#endif // FEATURE_UI_CORE_REMOVED

#ifdef FEATURE_SETTINGS_TEST_AUTOMATION
  extern void settingsdiag_init( void );
#endif

#ifndef FEATURE_UI_CORE_REMOVED
#ifdef FEATURE_MP4_DECODER
  extern void qtvdiag_init ( void );
#endif
#endif // FEATURE_UI_CORE_REMOVED

#ifndef FEATURE_GSM_PLT   

/* Tell lint that we are OK with the fact that pkt_len and req are not 
** used in some of the packets, as this is dictated by the awesome design */
/*lint -esym(715,pkt_len,req) */


/*===========================================================================

FUNCTION HSDIAG_CMD_INIT

DESCRIPTION
  This procedure initializes the HS processing portion of DIAG.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

============================================================================*/
void hsdiag_cmd_init (hs_packets_type *cmd)
{  
  /* Initialize the hs_cmd buffer and lockout state. */
  (void) q_link (cmd, &cmd->hdr.link);
  
  cmd->hdr.done_q_ptr = NULL;
  cmd->hdr.task_ptr = &diag_tcb;
  cmd->hdr.sigs = DIAG_HS_CMD_SIG;

  return;

} /* diag_hs_init */


/*===========================================================================

FUNCTION HSDIAG_SEND_KEY
DESCRIPTION
  This procedure sends the key press command to hs and waits for the 
  response.

============================================================================*/
static void hsdiag_send_key (
#ifdef FEATURE_KEYPAD_MULTI_KEY
  byte key_code,
  byte key_parm
#else
#error code not present
#endif
)
{
  hs_packets_type cmd;  

  hsdiag_cmd_init(&cmd);

  cmd.hdr.cmd = HS_SKI;
  cmd.key.keycode = key_code;
#ifdef FEATURE_KEYPAD_MULTI_KEY
  cmd.key.keyparm = key_parm;
#endif

  (void) rex_clr_sigs (&diag_tcb, DIAG_HS_CMD_SIG);

  hs_cmd (&cmd);

  (void) diag_wait (DIAG_HS_CMD_SIG);

  return;
} /* hsdiag_send_key */



/*===========================================================================

FUNCTION HSDIAG_KEY

DESCRIPTION
  This procedure processes a handset emulation keystroke input request.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  An ersatz keypress and release are generated by diag. These events are
  placed into the handset drivers input queue.  Genuine keystroke input
  must be locked out.

============================================================================*/
PACKED void * hsdiag_key (
  PACKED void *req_pkt,
  uint16 pkt_len
)
{
  DIAG_HS_KEY_F_req_type *req = (DIAG_HS_KEY_F_req_type *) req_pkt;
  DIAG_HS_KEY_F_rsp_type *rsp;
  const int rsp_len = sizeof( DIAG_HS_KEY_F_rsp_type );

  /*--------------------------------------------------------------------------
    Queue the keystroke command. If the packet does NOT request hold, then
    queue two commands, one after another to the hs driver.  The commands are
    the keystroke input, and the key-up input.  Wait here for command
    completion.  If hold is requested, then omit the key-up.
  -------------------------------------------------------------------------*/
#ifdef FEATURE_KEYPAD_MULTI_KEY
  hsdiag_send_key (req->key, HS_NONE_K);
#else
#error code not present
#endif

  /*-------------------------------------------------------------------------
    Send an HS_RELEASE_K if hold not specified.
  -------------------------------------------------------------------------*/
  if (!req->hold)
  {
#ifdef FEATURE_KEYPAD_MULTI_KEY    
     hsdiag_send_key (HS_RELEASE_K, req->key);
#else
#error code not present
#endif
  }

  rsp = (DIAG_HS_KEY_F_rsp_type *) diagpkt_alloc( DIAG_HS_KEY_F, rsp_len );

  if (rsp != NULL)
  {
	 rsp->hold = req->hold;
	 rsp->key = req->key;
  }


  return (rsp);

  (void) pkt_len; /* suppress compiler warning */

} /* hsdiag_key */



/*===========================================================================

FUNCTION HSDIAG_LOCK
DESCRIPTION
  This procedure processes a handset emulation request to lock or unlock
  the keyboard. The keyboard should be locked externally before doing
  handset emulation, and unlocked when done, or through restart.

============================================================================*/
PACKED void * hsdiag_lock (
  PACKED void *req_pkt,
  uint16 pkt_len
)
{
  hs_packets_type cmd;  
  DIAG_HS_LOCK_F_req_type *req = (DIAG_HS_LOCK_F_req_type *)req_pkt;
  DIAG_HS_LOCK_F_rsp_type *rsp;
  const int rsp_len = sizeof(DIAG_HS_LOCK_F_rsp_type);

  hsdiag_cmd_init (&cmd);
  
  /*--------------------------------------------------------------------------
    Send command to lock or unlock the handset.
  --------------------------------------------------------------------------*/
  if ( req->lock == HS_LOCK_F ) {
    cmd.lock.lock = TRUE;
  } else {
    cmd.lock.lock = FALSE;
  }
  
  cmd.hdr.cmd = HS_DIAG_LOCK;         /* command: HS_DIAG_LOCK */
  (void) rex_clr_sigs (&diag_tcb, DIAG_HS_CMD_SIG);
  hs_cmd (&cmd);                    /* Queue up the command */

  (void) diag_wait (DIAG_HS_CMD_SIG);       /* Wait for completion */

  rsp = (DIAG_HS_LOCK_F_rsp_type *)diagpkt_alloc (DIAG_HS_LOCK_F, rsp_len);

  if (rsp != NULL)
  {
	 *rsp = *req;
  }
	  
  
  return (rsp);
  
  (void) pkt_len; /* suppress compiler warning */

} /* hsdiag_lock */



#if !defined(FEATURE_BITMAP_RASTER)
/*===========================================================================

FUNCTION HSDIAG_SCREEN
DESCRIPTION
  This procedure processes a handset emulation request for screen data
  request.

============================================================================*/
PACKED void * hsdiag_screen (
  PACKED void *req_pkt,
  uint16 pkt_len
)
{
  hs_packets_type cmd;  
  hs_diag_type hs_screen;
  DIAG_HS_SCREEN_F_rsp_type *rsp;
  const int rsp_len = sizeof (DIAG_HS_SCREEN_F_rsp_type);

  hsdiag_cmd_init (&cmd);
  
  /*------------------------------------------------------------------------
    Queue a request for screen data.
  ------------------------------------------------------------------------*/
  cmd.hdr.cmd = HS_DIAG;               /* command: HS_DIAG */
  cmd.diag.diag_ptr = &hs_screen;      /* Put info in pkt  */

  (void) rex_clr_sigs (rex_self (), DIAG_HS_CMD_SIG);
  hs_cmd (&cmd);

  (void) diag_wait (DIAG_HS_CMD_SIG);

  /*------------------------------------------------------------------------
    Copy values byte by byte to avoid packing issues and to make it more 
    efficient.
  -------------------------------------------------------------------------*/
  rsp = (DIAG_HS_SCREEN_F_rsp_type *)diagpkt_alloc (DIAG_HS_SCREEN_F, rsp_len);

  if (rsp != NULL)
  {
     rsp->backlight_on = hs_screen.backlight_on;
  
     memcpy ((void*) rsp->scrn,
          (void*) &hs_screen.scrn, 
          HS_SCRN_HIGH * HS_SCRN_WIDE);

     rsp->annun = hs_screen.annun;
  
     memcpy ((void*) rsp->blink_scrn, 
          (void*) &hs_screen.blink_scrn, 
          HS_SCRN_HIGH * HS_SCRN_WIDE);
  
     rsp->blink_annun = hs_screen.blink_annun;
     rsp->rssi = hs_screen.rssi;
     rsp->blink_rate = hs_screen.blink_rate;
  }

  return (rsp);

  (void) req_pkt; /* suppress compiler warning */
  (void) pkt_len; /* suppress compiler warning */

} /* hsdiag_screen */


#else /* FEATURE_BITMAP_RASTER */
#error code not present
#endif /* defined(FEATURE_BITMAP_RASTER) */


#ifdef FEATURE_BITMAP
/*===========================================================================

FUNCTION HSDIAG_DISPLAY_GET_PARAMS

DESCRIPTION
  Performs the command specified in the request packet.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/
PACKED void * hsdiag_display_get_params (
  PACKED void *req,  /* pointer to request packet  */
  word pkt_len       /* length of request packet   */
)

{
  hsdiag_disp_get_params_rsp_type *rsp  = NULL;
  hsdiag_disp_cmd_type command_code = HS_DISPLAY_GET_PARAMS_CMD;
  disp_info_type   dinfo;
  const int rsp_len = sizeof(hsdiag_disp_get_params_rsp_type);

  if (req)
  {
    /* get the command code */
    command_code = diagpkt_subsys_get_cmd_code (req);  

    /* create a response */
    rsp = (hsdiag_disp_get_params_rsp_type *) 
      diagpkt_subsys_alloc (DIAG_SUBSYS_HS, command_code, rsp_len);
    if (rsp)
    {
      dinfo = disp_get_info();
      rsp->disp_height = dinfo.disp_height;
      rsp->disp_width  = dinfo.disp_width;
      switch(dinfo.bpp) {
      case DISP_16BPP:
        rsp->pixel_depth=16;
        break;
      case DISP_8BPP:
        rsp->pixel_depth=8;
        break;
      default:
        rsp->pixel_depth=24;
        break;
      }
      rsp->panel_type  = 0;
      if (dinfo.disp_width > dinfo.disp_height) {
        rsp->panel_type = 1;
      }
    }
  }
  return (rsp);
}

/* generic response */
typedef PACKED struct
{
  uint8  sig1;
  uint8  sig2;
  uint32 file_size;
  uint32 reserved;
  uint32 offset;
  uint32 header_size;
  uint32 width;
  uint32 height;
  uint16 planes;
  uint16 bpp;
  uint32 compression;
  uint32 data_size;
  uint32 hor_res;
  uint32 ver_res;
  uint32 colors;
  uint32 imp_colors;
} hsdiag_bmp_header;

static void hsdiag_populate_24bit_bmp_hdr(uint16 height, uint16 width, uint32 dsize, hsdiag_bmp_header* pHdr)
{
  if (!pHdr)
  {
    return;
  }
  pHdr->sig1 = 'B';
  pHdr->sig2 = 'M';
  pHdr->file_size = (dsize * 3) + 54;
  pHdr->reserved = 0;
  pHdr->offset = 54;
  pHdr->header_size = 40;
  pHdr->height = height;
  pHdr->width = width;
  pHdr->planes = 1;
  pHdr->bpp = 24;
  pHdr->compression = 0;
  pHdr->data_size = dsize * 3;
  pHdr->hor_res = 7874;
  pHdr->ver_res = 7874;
  pHdr->colors = 0;
  pHdr->imp_colors = 0;
}

static  uint16   s_panel_width  = 0;
static  byte*    s_bmp_buf = NULL;

#ifdef FEATURE_HS_DISPLAY_CAPTURE
static  uint16   s_panel_height = 0;
static  word*    s_full_disp_buf = NULL;
static  uint32   s_xid = 1;
#endif /* FEATURE_HS_DISPLAY_CAPTURE */

void hsdiag_24bit_convert(word *src, byte *dst, uint16 starting_row, uint16 starting_column, uint16 height, uint16 width)
{
  word   tmp_word;
  byte  *tmp_ptr;
  uint16 i, j;
  uint32 idx;

  ASSERT(src != NULL);
  ASSERT(dst != NULL);

  tmp_ptr=s_bmp_buf;
  for(i =0; i < height; i++) 
  {
    for(j =0; j < width; j++) 
    {
      tmp_word = src[(i + starting_row) * s_panel_width + (j + starting_column)];
      idx = 3*( (width * (height-i-1)) + j);
      tmp_ptr = (dst + idx);
      *(tmp_ptr++)=(tmp_word & 0x1f)<<3;   /* Blue */
      *(tmp_ptr++)=(tmp_word & 0x7e0)>>3;  /* Green */
      *(tmp_ptr++)=(tmp_word & 0xf800)>>8; /* Red */
    }
  }
}

boolean hsdiag_write_bmp_file(const char *filename, uint16 height, uint16 width, byte *buf, uint32 dsize)
{
  hsdiag_bmp_header   bmphdr;
  fs_rsp_msg_type     fso_rsp;
  fs_rsp_msg_type     fsw_rsp;
  fs_rsp_msg_type     fsc_rsp;
  fs_open_xparms_type open_parms;
  boolean             result = FALSE;
  
  /* remove and then open file, if already existing */
  fs_remove(filename, NULL, &fso_rsp);
  open_parms.create.cleanup_option = FS_OC_CLOSE;
  open_parms.create.buffering_option = FS_OB_PROHIBIT;
  fs_open(filename, FS_OA_CREATE, &open_parms, NULL, &fso_rsp );
  if (fso_rsp.open.status == FS_OKAY_S) 
  {
    hsdiag_populate_24bit_bmp_hdr(height, width, dsize, &bmphdr );
    fs_write(fso_rsp.open.handle, (void*)&bmphdr, sizeof(bmphdr), NULL, &fsw_rsp);
    fs_write(fso_rsp.open.handle, (void*)buf, dsize*3, NULL, &fsw_rsp);
    if (fsw_rsp.write.status == FS_OKAY_S && fsw_rsp.write.count == (dword)(dsize*3))
    {
      result = TRUE;
    } 
    fs_close(fso_rsp.open.handle, NULL, &fsc_rsp);
  }
  return result;
}

#ifdef FEATURE_HS_DISPLAY_CAPTURE
/*===========================================================================

FUNCTION HSDIAG_DISPLAY_CAPTURE

DESCRIPTION
  Performs the command specified in the request packet.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/
PACKED void * hsdiag_display_capture (
  PACKED void *req,  /* pointer to request packet  */
  word pkt_len       /* length of request packet   */
)

{
  hsdiag_disp_capture_req_type* hsreq = NULL;
  hsdiag_disp_status_type cmd_status;  
  uint32 dsize;
  hsdiag_disp_capture_event status_event;
  hsdiag_disp_cmd_rsp_type *rsp  = NULL;
  hsdiag_disp_cmd_type command_code = HS_DISPLAY_CAPTURE_CMD;
  const int rsp_len = sizeof(hsdiag_disp_cmd_rsp_type);
  disp_info_type      dinfo;
  extern dword disp_capture(void*,dword,int16,int16,int16,int16);
  
  dinfo = disp_get_info();
  
  if (!req) return(rsp);
  
  hsreq = (hsdiag_disp_capture_req_type*)req;
  
  /* check for valid columns/row */
  ASSERT(hsreq->starting_row < dinfo.disp_height);
  ASSERT(hsreq->starting_column < dinfo.disp_width);
  
  if ((hsreq->starting_row + hsreq->height) > dinfo.disp_height)
    hsreq->height=dinfo.disp_height-hsreq->starting_row;
  
  if ((hsreq->starting_column + hsreq->width) > dinfo.disp_width)
    hsreq->width=dinfo.disp_width-hsreq->starting_column;
  
  ASSERT(hsreq->height>0);
  ASSERT(hsreq->width>0);
  
  /* allocate display buffer, if not already done */
  s_panel_height = dinfo.disp_height;
  s_panel_width  = dinfo.disp_width;
  if (!s_full_disp_buf) 
    s_full_disp_buf = (word*)malloc(dinfo.disp_width*dinfo.disp_height*sizeof(word));
  if (!s_bmp_buf)
    s_bmp_buf = (byte*)malloc(dinfo.disp_width*dinfo.disp_height*3);
  
  command_code = diagpkt_subsys_get_cmd_code (req);
  
  rsp = (hsdiag_disp_cmd_rsp_type *) 
    diagpkt_subsys_alloc (DIAG_SUBSYS_HS, command_code, rsp_len);
  if (!rsp) return(rsp);
  
  rsp->xid = 0;
  
  rsp->status = HS_DISPLAY_INVALID_COORD; 
  /* width has to be a multiple of 4 (BMP)*/
  if (hsreq->width % 4) return rsp;
  if (hsreq->height*hsreq->width == 0) return rsp;
  
#ifdef FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING
  clk_regime_register_for_cpu_resource
      (
        CLKRGM_HSDIAG_DISP_CAPTURE_SERVICES
      );
#endif /* FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING */
  
  /* allocate buffer for 24-bit representation */
  rsp->status = HS_DISPLAY_CAPTURE_FAIL;
  if (!disp_capture
      (
        s_full_disp_buf,
        dinfo.disp_width*dinfo.disp_height,
        0,
        0,
        dinfo.disp_height-1,
        dinfo.disp_width-1
      )) return (rsp);

#ifdef FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING
  clk_regime_deregister_for_cpu_resource
      (
        CLKRGM_HSDIAG_DISP_CAPTURE_SERVICES
      );

#endif /* FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING */

  dsize = hsreq->height * hsreq->width;

  hsdiag_24bit_convert(s_full_disp_buf, s_bmp_buf, hsreq->starting_row, hsreq->starting_column, hsreq->height, hsreq->width);

  if (hsreq->non_blocking) { 
    rsp->status = HS_DISPLAY_CAPTURE_OK;
    rsp->xid = s_xid; /* assign the current transaction id */
    diagpkt_commit (rsp);
    rsp = NULL;
  }

  cmd_status = HS_DISPLAY_FILE_WRITE_FAIL;

  if (hsdiag_write_bmp_file((const char *)hsreq->filename, hsreq->height, hsreq->width, s_bmp_buf, dsize))
  {
    cmd_status = HS_DISPLAY_CAPTURE_OK;
  }

  /* send an event if non-blocking is set */
  if (hsreq->non_blocking) { 
    status_event.status = cmd_status;
    status_event.xid = s_xid++;
    /* increment the transaction id */
    if (!s_xid) s_xid = 1;

    event_report_payload ( 
      EVENT_HS_DISPLAY_BMP_CAPTURE_STATUS, 
      sizeof(status_event), 
      (void*)&status_event 
    );
  } 
  
  /* update status */
  if (rsp) rsp->status = cmd_status;
    
  return (rsp);
}

/*===========================================================================

FUNCTION HSDIAG_DISPLAY_CAPTURE_DELAYED

DESCRIPTION
  Performs display capture for DIAG

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  Allocates memory for filename, must be free-ed by caller.

===========================================================================*/
PACKED void * hsdiag_display_capture_delayed (
  PACKED void *req,  /* pointer to request packet  */
  word pkt_len       /* length of request packet   */
)
{
  char * filename;
  hsdiag_disp_cmd_delayed_rsp_type *rsp = NULL;
  hsdiag_disp_cmd_type subsys_cmd_code = HS_DISPLAY_CAPTURE_CMD;
  const int rsp_len = sizeof(hsdiag_disp_cmd_delayed_rsp_type);
  hsdiag_disp_capture_delayed_req_type *hsreq;
  disp_info_type      dinfo;

  /* if NULL req, return */
  if (!req) return(rsp);
  
  hsreq = (hsdiag_disp_capture_delayed_req_type*)req;
  
  subsys_cmd_code = diagpkt_subsys_get_cmd_code (req);
  
  rsp = (hsdiag_disp_cmd_delayed_rsp_type *) 
    diagpkt_subsys_alloc_v2 (DIAG_SUBSYS_HS, subsys_cmd_code, rsp_len);
  /* Ensure packet allocated before continuation */
  if (!rsp) return(rsp);
  
  dinfo = disp_get_info();

  /* ensure sane height and width values */
  if ((hsreq->starting_row + hsreq->height) > dinfo.disp_height)
    hsreq->height = dinfo.disp_height - hsreq->starting_row;
  
  if ((hsreq->starting_column + hsreq->width) > dinfo.disp_width)
    hsreq->width = dinfo.disp_width - hsreq->starting_column;
  
  /* check for valid columns/row */
  /* width has to be a multiple of 4 (BMP)*/
  if ((hsreq->starting_row >= dinfo.disp_height) || \
      (hsreq->starting_column >= dinfo.disp_width) || \
      (hsreq->starting_row + hsreq->height > dinfo.disp_height) || \
      (hsreq->starting_column + hsreq->width > dinfo.disp_width) || \
      (hsreq->width % 4))
  {
    diagpkt_subsys_set_status(rsp, HS_DISPLAY_INVALID_COORD);
    return (rsp);
  }

  /* Now, queue the display capture command to HS */  

  /* If not enough memory for filename, return */
  filename = (char*) mem_malloc(&tmc_heap, strlen( (char*)(hsreq->filename) ) * sizeof(char));
  if ( 
  		(!filename) || 
  		( std_strlcpy( filename, (const char *)hsreq->filename, ((strlen( (char*)(hsreq->filename) ) * sizeof(char)) +1) ) == 0 )
  	 )
  {
    diagpkt_subsys_set_status(rsp, HS_DISPLAY_NO_MEM);
    return (rsp);
  }
  /* allocate display buffer, if not already done */
  s_panel_height = dinfo.disp_height;
  s_panel_width  = dinfo.disp_width;  
  if (!s_full_disp_buf) 
    s_full_disp_buf = (word*)calloc(dinfo.disp_width * dinfo.disp_height, sizeof(word));
  if (!s_bmp_buf)
    s_bmp_buf = (byte*)calloc(dinfo.disp_width * dinfo.disp_height*3 ,1);

  if (!s_full_disp_buf || !s_bmp_buf)
  {
    /*rsp->status = HS_DISPLAY_NO_MEM;*/
    diagpkt_subsys_set_status(rsp, HS_DISPLAY_NO_MEM);
    return (rsp);
  }
  
  /* If busy, fail and return */ 
  if (DISP_CAP_IDLE != hsdiag_disp_cap_data.state)
  {
    diagpkt_subsys_set_status(rsp, HS_DISPLAY_CAPTURE_FAIL);
    return (rsp);
  }

  hsdiag_disp_cap_data.state = DISP_CAP_FIRST_READ; /* set false in hs_target.c */
  
  hsdiag_disp_cap_data.starting_row = hsreq->starting_row;
  hsdiag_disp_cap_data.starting_column = hsreq->starting_column;
  hsdiag_disp_cap_data.height = hsreq->height;
  hsdiag_disp_cap_data.width = hsreq->width;
  hsdiag_disp_cap_data.subsys_cmd_code = subsys_cmd_code;
  hsdiag_disp_cap_data.filename = filename; /* freed by HS */
  hsdiag_disp_cap_data.bmp_buf = s_bmp_buf;
#ifdef FEATURE_HS_DISPLAY_CAPTURE_LIMITED_READ_PKT_DBG
  hsdiag_disp_cap_data.cont_row = hsreq->starting_row;
  hsdiag_disp_cap_data.read_limit = hsreq->lines_to_read;
  hsdiag_disp_cap_data.yield_time = hsreq->time_to_yield;
#endif
  hsdiag_disp_cap_data.full_disp_buf = s_full_disp_buf;
  hsdiag_disp_cap_data.delayed_rsp_id = diagpkt_subsys_get_delayed_rsp_id(rsp);
  
  rex_set_sigs(&hs_tcb, HS_DISP_CAP_SIG);

  diagpkt_subsys_set_status((PACKED void *)rsp, HS_DISPLAY_CAPTURE_OK);
  diagpkt_subsys_set_rsp_cnt ((PACKED void *)rsp, 0);
  
  diagpkt_commit (rsp);
  rsp = NULL;
  return (rsp);
}

/*===========================================================================

FUNCTION HSDIAG_HS_DISPLAY_CAPTURE

DESCRIPTION
  This is a call-back routine. It results from DIAG command sent by the DM
  to capture the display screen. 
   read_limit : the max number of rows to read in one go
   yield_time : requested time to yield, if capture not complete

DEPENDENCIES
  None

RETURN VALUE
  Amount of time in ms to wait before calling display capture again. 
  A return value of 0 implies that the display capture process is done.

SIDE EFFECTS
  Frees memory allocated in hsdiag for filename after display capture is
  complete.
  Sends a Diag delayed response to the DM indicating display capture 
  status.

===========================================================================*/
#ifdef FEATURE_HS_DISPLAY_CAPTURE_LIMITED_READ
uint8 hsdiag_hs_display_capture(uint16 read_limit, uint8 yield_time)
#else
uint8 hsdiag_hs_display_capture()
#endif /* FEATURE_HS_DISPLAY_CAPTURE_LIMITED_READ */
{
  hsdiag_disp_cap_type *dispcap = &hsdiag_disp_cap_data;
  uint32 dsize;
  uint32 rsplen, retval;
  disp_info_type dinfo;
  hsdiag_disp_cmd_delayed_rsp_type *rsp;
  hsdiag_disp_status_type result;
#ifdef FEATURE_HS_DISPLAY_CAPTURE_LIMITED_READ  
#ifdef FEATURE_HS_DISPLAY_CAPTURE_DISABLE_MDDI
#error code not present
#endif /* FEATURE_HS_DISPLAY_CAPTURE_DISABLE_MDDI */
  boolean limited_read_done = TRUE;
  uint16 start_row, end_row;
#else /* FEATURE_HS_DISPLAY_CAPTURE_LIMITED_READ */
  const uint8 yield_time = 0;
#endif /* FEATURE_HS_DISPLAY_CAPTURE_LIMITED_READ */
  
  extern dword disp_capture(void*,dword,int16,int16,int16,int16);

  ASSERT(dispcap != NULL);

  dinfo = disp_get_info();

  do {
    result = HS_DISPLAY_CAPTURE_FAIL;

#ifdef FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING
    clk_regime_register_for_cpu_resource
        (
          CLKRGM_HSDIAG_DISP_CAPTURE_SERVICES
        );
#endif /* FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING */
    
#ifdef FEATURE_HS_DISPLAY_CAPTURE_LIMITED_READ 
    /* Perform limited read, there are extra fields to read the
     * LCD framebuffer in portions.
     *  read_limit : specifies # of rows to read (inclusive)
     *  yield_time : specifies amount of time to yield for
     *               other tasks after reading each portion
     */
#ifdef FEATURE_HS_DISPLAY_CAPTURE_LIMITED_READ_PKT_DBG
    /* If the packet won't supply read limits and yield times, reset it */
    yield_time = (dispcap->yield_time == 0) ? yield_time : dispcap->yield_time;
    read_limit = (dispcap->read_limit == 0) ? read_limit : dispcap->read_limit;
#endif /* FEATURE_HS_DISPLAY_CAPTURE_LIMITED_READ_PKT_DBG */

    if (read_limit != 0)
    {
      /* check if first read, in the first read, 
       * 
       * before beginning screen capture. HS has higher priority than
       * Diag, so this has to be done */
      if (DISP_CAP_FIRST_READ == dispcap->state)
      {
#ifdef FEATURE_HS_DISPLAY_CAPTURE_DISABLE_MDDI
#error code not present
#endif /* FEATURE_HS_DISPLAY_CAPTURE_DISABLE_MDDI */

        dispcap->state = DISP_CAP_BUSY;
        dispcap->cont_row = dispcap->starting_row;
        limited_read_done = FALSE;
        break; /* make it wait prior to starting */
      }
      /* Now, start or continue reading */
      
      /* calculate amount to read for this read */
      start_row = dispcap->cont_row;
      end_row = (start_row + read_limit < dinfo.disp_height) ? 
                                    start_row + read_limit : 
                                    dinfo.disp_height-1;
      /* reset the cont_row, this is also used as exit condition */
      dispcap->cont_row = end_row + 1;
      
      retval = disp_capture
                (
                  dispcap->full_disp_buf + start_row * dinfo.disp_width,
                  dinfo.disp_width * dinfo.disp_height - start_row * dinfo.disp_width,
                  start_row,
                  0,
                  end_row,
                  dinfo.disp_width-1
                );
    }
    else
    {
      /* Not reading the LCD framebuffer in portions, read whole screen */
      retval = disp_capture
                (
                  dispcap->full_disp_buf,
                  dinfo.disp_width * dinfo.disp_height,
                  0,
                  0,
                  dinfo.disp_height-1,
                  dinfo.disp_width-1
                );
      dispcap->cont_row = dinfo.disp_height;
    }
#else /* FEATURE_HS_DISPLAY_CAPTURE_LIMITED_READ */ 

    /* Not using limited read, capture whole screen */
    retval = disp_capture
              (
                dispcap->full_disp_buf,
                dinfo.disp_width * dinfo.disp_height,
                0,
                0,
                dinfo.disp_height-1,
                dinfo.disp_width-1
              );

#endif /* FEATURE_HS_DISPLAY_CAPTURE_LIMITED_READ */

#ifdef FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING
    clk_regime_deregister_for_cpu_resource
        (
          CLKRGM_HSDIAG_DISP_CAPTURE_SERVICES
        );

#endif /* FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING */

    /* Capture failed */
    if (!retval) break;

#ifdef FEATURE_HS_DISPLAY_CAPTURE_LIMITED_READ
    /* check if more reads are necessary to fill buffer */
    if (dispcap->cont_row < dinfo.disp_height-1)
    {
      limited_read_done = FALSE;
      break;
    }
#ifdef FEATURE_HS_DISPLAY_CAPTURE_DISABLE_MDDI
#error code not present
#endif /* FEATURE_HS_DISPLAY_CAPTURE_DISABLE_MDDI */
#endif /* FEATURE_HS_DISPLAY_CAPTURE_LIMITED_READ */
    /* perform 24 bit conversion */
    dsize = dispcap->height * dispcap->width;

    hsdiag_24bit_convert(dispcap->full_disp_buf, dispcap->bmp_buf,  \
                         dispcap->starting_row, dispcap->starting_column, \
                         dispcap->height, dispcap->width);

    result = HS_DISPLAY_CAPTURE_OK;
    /* disp_capture got some screenshot, write to EFS */
    
    if (!hsdiag_write_bmp_file(dispcap->filename, \
                               dispcap->height, dispcap->width, \
                               dispcap->bmp_buf, dsize))
    {
      result = HS_DISPLAY_FILE_WRITE_FAIL;
    }
  
  } while ( 0 );

#ifdef FEATURE_HS_DISPLAY_CAPTURE_LIMITED_READ
  if (limited_read_done)
  {
#ifdef FEATURE_HS_DISPLAY_CAPTURE_DISABLE_MDDI
#error code not present
#endif /* FEATURE_HS_DISPLAY_CAPTURE_DISABLE_MDDI */
#endif /* FEATURE_HS_DISPLAY_CAPTURE_LIMITED_READ */

    /* free filename memory, allocated in hsdiag */
    if (dispcap->filename) tmc_free(dispcap->filename);
    dispcap->filename = NULL;
    
    /* allocate diag pkt */
    rsplen = sizeof(hsdiag_disp_cmd_delayed_rsp_type);
    rsp = (hsdiag_disp_cmd_delayed_rsp_type *) \
          diagpkt_subsys_alloc_v2_delay(DIAG_SUBSYS_HS,  \
                                        dispcap->subsys_cmd_code,  \
                                        dispcap->delayed_rsp_id, \
                                        rsplen); 
    if (rsp != NULL)
    {
      diagpkt_subsys_set_status((PACKED void *)rsp, result);
      diagpkt_subsys_set_rsp_cnt ((PACKED void *)rsp, 1);
      diagpkt_delay_commit(rsp);
    }
    dispcap->state = DISP_CAP_IDLE;
#ifdef FEATURE_HS_DISPLAY_CAPTURE_LIMITED_READ
    yield_time = 0; /* capture done, do not yield */
  }
#endif /* FEATURE_HS_DISPLAY_CAPTURE_LIMITED_READ */
  return yield_time;
}

#endif /* FEATURE_HS_DISPLAY_CAPTURE */

#endif /* defined FEATURE_BITMAP */


static const diagpkt_user_table_entry_type hsdiag_tbl[] =
{
  {DIAG_HS_KEY_F, DIAG_HS_KEY_F, hsdiag_key},
  {DIAG_HS_LOCK_F, DIAG_HS_LOCK_F, hsdiag_lock},
#if !defined (FEATURE_BITMAP_RASTER)
  {DIAG_HS_SCREEN_F, DIAG_HS_SCREEN_F, hsdiag_screen},
#else
#error code not present
#endif  
};

/*  Map the diag display functions to the proper commands */
static const diagpkt_user_table_entry_type hsdiag_subsys_tbl[] = {
#ifdef FEATURE_BITMAP
  {HS_DISPLAY_GET_PARAMS_CMD, HS_DISPLAY_GET_PARAMS_CMD, hsdiag_display_get_params},
  #ifdef FEATURE_HS_DISPLAY_CAPTURE
    {HS_DISPLAY_CAPTURE_CMD, HS_DISPLAY_CAPTURE_CMD, hsdiag_display_capture}
  #endif
#else
  /* Subsystem dispatch table is empty */
  NULL
#endif
};

#ifdef FEATURE_HS_DISPLAY_CAPTURE
static const diagpkt_user_table_entry_type hsdiag_subsys_delayed_tbl[] = {
  {HS_DISPLAY_CAPTURE_DELAYED_CMD, HS_DISPLAY_CAPTURE_DELAYED_CMD, hsdiag_display_capture_delayed}
};
#endif

#ifdef __cplusplus

  DIAGPKT_DISPATCH_AUTOREGISTER (DIAGPKT_NO_SUBSYS_ID, hsdiag_tbl);
  DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_HS, hsdiag_subsys_tbl);

#else

void hsdiag_init (void)
{
#if defined(FEATURE_MULTIPROCESSOR)
#error code not present
#else
  DIAGPKT_DISPATCH_TABLE_REGISTER (DIAGPKT_NO_SUBSYS_ID, hsdiag_tbl);
  DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_HS, hsdiag_subsys_tbl);
  #ifdef FEATURE_HS_DISPLAY_CAPTURE
    DIAGPKT_DISPATCH_TABLE_REGISTER_V2_DELAY(DIAG_SUBSYS_CMD_VER_2_F, DIAG_SUBSYS_HS, 
      hsdiag_subsys_delayed_tbl);
  #endif
#endif

#ifndef FEATURE_UI_CORE_REMOVED
#ifdef FEATURE_PORTMAP_TEST_AUTOMATION
#ifndef CUST_EDITION
  portmapdiag_init();
#endif
#endif
#ifdef FEATURE_MEDIAPLAYER_TEST_AUTOMATION
  mediaplayerdiag_init();
#endif
#ifdef FEATURE_QCAMERA_TEST_AUTOMATION
  qcameradiag_init();
#endif
#ifdef FEATURE_AUDIOSETTINGS_TEST_AUTOMATION
#ifndef CUST_EDITION
  audiodiag_init();
#endif
#endif
#ifdef FEATURE_MOBIMON_TEST_AUTOMATION
  mobimondiag_init();
#endif
#ifdef FEATURE_GUNIMON_TEST_AUTOMATION
  gunimondiag_init();
#endif
#endif // FEATURE_UI_CORE_REMOVED
#ifdef FEATURE_SETTINGS_TEST_AUTOMATION
  settingsdiag_init();
#endif
#ifndef FEATURE_UI_CORE_REMOVED
#ifdef FEATURE_MP4_DECODER
  qtvdiag_init ();
#endif
#endif // FEATURE_UI_CORE_REMOVED

}
#endif


/* Restore error reporting for these symbols */
/*lint +esym(715,pkt_len,req) */

#ifdef __cplusplus
  }
#endif
#endif // FEATURE_GSM_PLT

/*=========================================================================*/
void
hsdiag_keymap (uint32 action, const uint32 * table)
{
  hsdiag_keymap_execute_type *exec;

  exec = diag_blocking_malloc (sizeof (hsdiag_keymap_execute_type));

  if (exec)
  {
    exec->action = action;
    exec->table = table;
    diag_idle_processing_register (hsdiag_keymap_execute, (void *) exec);
  }
}

/*===========================================================================

FUNCTION HSDIAG_KEYMAP_EXECUTE

DESCRIPTION
   This routine, called via DIAG idle processing dispatch, processes a 
   keymap action.
    
===========================================================================*/
static void 
hsdiag_keymap_execute (void *action)
{
  hsdiag_keymap_execute_type *exec = (hsdiag_keymap_execute_type *) action;

  if (exec)
  {
    hsdiag_keymap_action (exec->action, exec->table);


    /* The keymap action is complete.  Unregister and free the action
       buffer. */
    diag_idle_processing_unregister (hsdiag_keymap_execute, action);
    diag_free (action);
  }
  return;
}        /* END hsdiag_keymap_execute */

/*===========================================================================

FUNCTION HSDIAG_KEYMAP_ACTION

DESCRIPTION
   Given a table of key sequences and an action, finds the action in the
   table and injects the associated sequence of keys into the keypad driver
   emulating the sequence as though entered physically on the keypad

===========================================================================*/
//XXX
unsigned int hsdiag_delay    = HSDIAG_KEYMAP_DEFAULT_DELAY;
unsigned int hsdiag_bp_delay = HSDIAG_KEYMAP_BUTTPRESS_DELAY;

static void
hsdiag_keymap_action (uint32 action, const uint32 * table)
{
  const uint32 *tbl = table;
  uint32 key;
  uint32 delay;
  
  if (tbl)
  {
    while ((key = *tbl++) != HSDIAG_KEYMAP_TABLE_END)
    {
      if (key == HSKM_ACT (action))
      {
        while (((key = *tbl++) & HSDIAG_KEYMAP_ACTION_BIT) == 0)
        {
          if ((key & HSDIAG_KEYMAP_SEQUENCE_BIT) != 0)
          {
            hsdiag_keymap_action (key & ~HSDIAG_KEYMAP_SEQUENCE_BIT, table);
          }
          else if ((key & HSDIAG_KEYMAP_KPDELAY_BIT) != 0)
          {
            hsdiag_bp_delay = (key & HSDIAG_KEYMAP_MS_MASK);
          }
          else if ((key & HSDIAG_KEYMAP_IKDELAY_BIT) != 0)
          {
            hsdiag_delay = (key & HSDIAG_KEYMAP_MS_MASK);
          }
          else if ((key & HSDIAG_KEYMAP_DELAY_BIT) != 0)
          {
            delay = (key & HSDIAG_KEYMAP_MS_MASK);
            diag_pause (delay, TRUE);
          }
          else
          {
#ifdef FEATURE_KEYPAD_MULTI_KEY
            hsdiag_send_key (hsdiag_keymap_keys[key], HS_NONE_K);
            diag_pause(hsdiag_bp_delay, TRUE);
            hsdiag_send_key (HS_RELEASE_K, hsdiag_keymap_keys[key]);
#else
#error code not present
#endif
            diag_pause (hsdiag_delay, TRUE);
          }
        }
      }
    }

    /* Reset delay values to defaults */
    hsdiag_delay    = HSDIAG_KEYMAP_DEFAULT_DELAY;
    hsdiag_bp_delay = HSDIAG_KEYMAP_BUTTPRESS_DELAY;
  }
}



