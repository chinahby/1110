#ifndef VIDEO_ENC_SPECIAL_EFFECTS_H
#define VIDEO_ENC_SPECIAL_EFFECTS_H


/*===========================================================================

                   S P E C I A L    E F F E C T S    A P I

DESCRIPTION
  This header file contains special effects API for raw encoding.


DEFINED TYPES

       Name      Definition
       -------   --------------------------------------------------------



DEFINED CONSTANTS

       Name      Definition
       -------   --------------------------------------------------------
Copyright (c) 2004,2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/camcorder/services/recorder/6k/common/main/latest/inc/video_enc_special_effects.h#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/14/09   ps      Changes done after changed in IPL for frame overlay
09/09/08   ps      Featurization of Timestamp special effects support.
04/05/06   jdas	   Removed lint error
03/11/05   wyh     Overlay can specify transparent color.
03/02/05   wyh     Added Timestamp.
02/24/05   wyh     Added Fade-In/Fade-Out
02/22/05   wyh     More efficient overlay.
02/09/05   wyh     Don't use videoeng's malloc() and free()
01/25/05   wh      Cosmetic changes from code review of video_eng_frame api
01/17/05   ali/wyh initial version.

===========================================================================*/

#include "comdef.h"
#include "queue.h"
#include "video_common.h"
#include "ipl.h"
#include "camcorder_util.h"  /* for memory leak detection code */

/*===========================================================================

                            Data Declarations

===========================================================================*/


/* ------------------------------------------------------------------------
** Types
** ------------------------------------------------------------------------*/


/* Special effects are called with a packet input. The packet contains
** an input buffer (from the upper layers) and an output buffer (initialized
** by the special effects engine. 
**
** The vse_release_buffer_ind_type indicates which buffer contains the valid
** output buffer of the special effect. Depending on the specific special
** effects engine algorithm, the output might be over-written in the input
** buffer or it may be in the output buffer.
*/
typedef enum {
  BUFFER_FREE_STATUS_NOT_SET,
  RELEASE_INPUT_BUFFER,            /* input buffer can be freed */
  INPUT_BUFFER_USED                /* input_buffer pointer contains outout */
} vse_release_buffer_ind_type;


/* status of the special effects engine
*/
typedef enum {
  VSE_ENGINE_UNINITIALIZED=0,    /* engine is not initialized */
  VSE_ENGINE_INITIALIZED         /* special effects engine is initialized */
} vse_engine_status_type;


/* this structure contains global information relevant to all packets
** of a special effects engine
*/
typedef struct {
  vse_engine_status_type engine_status;
                             /* internal: initialize to 0,               */
  uint32            npix_x;  /* input: nr. of pixels width               */
  uint32            npix_y;  /* input: nr. of pixels height              */
  void             *param;   /* internal: special effects specific data  */
#ifdef FEATURE_CAMCORDER_MALLOC
  camcorderutil_malloc_context_type
                   *malloc_context;  
                            /* internal for memory leak detection        */
#endif
} vse_type;

/*
** possible return values of the special effects engine
*/
typedef enum {
  VSE_SUCCESS,
  VSE_FAILURE,
  VSE_ALREADY_INITIALIZED,
  VSE_ALREADY_TERMINATED,
  VSE_COMPLETED,
  VSE_MALLOC_FAILED
} vse_return_type;

/* commands from client to the special effects engine
*/
typedef enum {
  VSE_RUN,      /* run special effects on a frame (a packet)    */
  VSE_INIT_PACKET,/* initialize a packet.  (eg malloc necessary buffers) */
  VSE_INIT_ENG, /* initialize the vse-engine                            */
  VSE_TERMINATE, /* undo the INIT_ENG.                                   */
  VSE_TERMINATE_PACKET /* undo INIT_PACKET (eg free buffers)            */
} vse_command_type;

/* status of a packet
*/
typedef enum {
  VSE_PACKET_STATUS_UNINITIALIZED = 0, /* packet is uninitialized */
  VSE_PACKET_STATUS_INITIALIZED   = 1  /* packet is initialized */
} vse_packet_status_type;


/* A packet contains a frame in time, including both the input and output 
** pointers and other relevant information.
** The special effects engine operate on the information contained in the
** packet.
*/
typedef struct {
  ipl_image_type *output_buffer;       /* output buffer: initialized,       */
                                       /* malloc'ed and maintained by se    */
                                       /* engine                            */
  ipl_image_type *input_buffer;        /* Input: iamge to VFE set by client */
  uint32          msec;                /* Input: render time                */

  vse_packet_status_type   status;     /* output: status set by engine      */
  vse_release_buffer_ind_type
                    free_buffer_flag; /* output: set by effects engine to   */
                                      /* tell the                           */
                                      /* client where the output is placed  */
                                      /* whether the output = input (over-  */
                                      /* written) or the ycrcb_buffer       */
  void             *param;            /* Input: se params for this packet   */

} vse_packet_data_type;


/*  This is the template for all special effects routine in this API.
**  packet contain data pertinent to a particular frame at a time.
**  vse_struct contain global information for the specail effects engine
**  that are relevant to all packets. 
*/
typedef vse_return_type  (*vse_ptr_type) (
  vse_packet_data_type   *packet,
  vse_type               *vse_struct,
  vse_command_type        command
);


/* This is the rex- queue compatible packet used by higher layers to
** queue data from the VFE to the special effects engine, to the 
** DSP backend, and back to the VFE.
*/
typedef struct {
  q_link_type     link;                   /* for rex queue function */

  vse_packet_data_type  data;
  vse_ptr_type          vse_ptr;          /* function ptr to se engine */
  video_cb_func_ptr_type  callback_ptr;   /* to send release frame info */
  void                    *client_data;   /* corresponding to callback_ptr*/
  uint16                  frame_done_buf[4]; /* dsp if related */
} vse_packet_type;



/* The special effects Q should be 3. This handles the worst-case
** scenario of simultaneously 1 packet stuck in the Backend, 1 packet 
** stuck in the special effects engine, and one packet containing new
** VFE data waiting for the special effects engine to free up.
**
** Unless the special effects engine processing time is very uneven,
** there should be no need to go beyond 3 packets.
*/
#define VIDEO_ENG_SPECIAL_EFFECTS_NUM_PACKETS  3

/* ********************************************************
**   IMPLEMENTED SPEICAL EFFECTS ENGINES
** ********************************************************
*/

/* <EJECT> */
/*===========================================================================

FUNCTION vse_identity

DESCRIPTION
  A Do nothing special effect using memcpy.
  
DEPENDENCIES
  None
 
ARGUMENTS IN    
  None
   
ARGUMENTS IN/OUT   
  
ARGUMENTS OUT
  None
      
RETURN VALUE
  None
        
SIDE EFFECTS
  None

===========================================================================*/
extern vse_return_type vse_identity( 
  vse_packet_data_type   *packet,
  vse_type               *vse_struct,
  vse_command_type        command
);

/* <EJECT> */
/*===========================================================================

FUNCTION vse_identity_sans_memcpy

DESCRIPTION
  A Do nothing special effect that does not use memcpy.
  
DEPENDENCIES
  None
 
ARGUMENTS IN    
  None
   
ARGUMENTS IN/OUT   
  
ARGUMENTS OUT
  None
      
RETURN VALUE
  None
        
SIDE EFFECTS
  None

===========================================================================*/
extern vse_return_type vse_identity_sans_memcpy( 
  vse_packet_data_type   *packet,
  vse_type               *vse_struct,
  vse_command_type        command
);

#define VSE_OVERLAY_VALID_MAGIC  0x15a94afeL

typedef struct {
  ipl_image_type   *frame;
  uint16          transparent_value;
  uint32          valid_check;
  boolean         new_frame; //TRUE when the frame is being set for the first time because we will need color conversion at that time
  ipl_image_type   ycbcrframe; 
  uint8          y,cr,cb;
} vse_overlay_param_type;

/* <EJECT> */
/*===========================================================================

FUNCTION vse_overlay

DESCRIPTION
  Overlay demonstration function.
  
DEPENDENCIES
  None
 
ARGUMENTS IN    
  None
   
ARGUMENTS IN/OUT   
  
ARGUMENTS OUT
  None
      
RETURN VALUE
  None
        
SIDE EFFECTS
  None

===========================================================================*/


extern vse_return_type vse_overlay( 
  vse_packet_data_type   *packet,
  vse_type               *vse_struct,
  vse_command_type        command
);



typedef enum {
  VSE_FADE_IN_COMMAND,
  VSE_FADE_OUT_COMMAND
} vse_fadeInOutCommandType;

typedef struct {
  uint32      nmsecs;    /* nr. of msecs to fade in or out */
  vse_fadeInOutCommandType
              fadeInOutCommand;

} vse_fadeInOutParamsType;

/* <EJECT> */
/*===========================================================================

FUNCTION vse_fadeInOut

DESCRIPTION
  First call with the VSE_INIT_ENG command with a packet. The param of the
  input packet structure contains the nr. of msecs to fade in/out, and
  the function ID (either fade in or fade out).

  After the last frame of fade-in subsequent calls to vse_fadeInOut will
  return the identity function.

  After the last frame of fade-out subsequent calls to vse_fadeInOut will
  return the white image.  
  
DEPENDENCIES
  None
 
ARGUMENTS IN    
  None
   
ARGUMENTS IN/OUT   
  
ARGUMENTS OUT
  None
      
RETURN VALUE
  None
        
SIDE EFFECTS
  None

===========================================================================*/
vse_return_type vse_fadeInOut( 
  vse_packet_data_type   *packet,
  vse_type               *vse_struct,
  vse_command_type        command
);

#ifdef FEATURE_CAMCORDER_OVERLAY_TIMESTAMP
vse_return_type vse_timestamp( 
  vse_packet_data_type   *packet,
  vse_type               *vse_struct,
  vse_command_type        command
);
#endif /*FEATURE_CAMCORDER_OVERLAY_TIMESTAMP*/  

#endif //VIDEO_ENC_SPECIAL_EFFECTS_H
