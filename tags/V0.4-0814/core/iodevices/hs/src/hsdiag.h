#ifndef HSDIAG_H
#define HSDIAG_H
/*==========================================================================

                      Diagnostic Packet Definitions

  Description: Packet definitions between the diagnostic subsystem
  and the external device.

  !!! NOTE: All member structures of DIAG packets must be PACKED.
  
  !!! WARNING: Each command code number is part of the externalized
  diagnostic command interface.  This number *MUST* be assigned
  by a member of QCT's tools development team.

Copyright (c) 2001 - 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/
/*==========================================================================
                              HSDIAG Key Map
                              
Maps key press sequences from invariant diag commands to a table of sequences
provided by clients

Clients create a mapping of client defined actions to sequences of key presses.
The client defined action is a 32 bit id with the upper 16 bits not used.  
A sequence of key presses is defined using the key defined in this header file.
A sequence can refer to other sequences and to a delay action.  The delay
action pauses between key presses for the user provided number of ms.  Note
that the keymap handler function provides a default delay between key presses
of DIAG_KEYMAP_DEFAULT_DELAY milli-seconds.                

DIAG packet handlers calling this function should first commit their response
since the processes is dependent on the particular sequence chosen and could
incorporate delays.  Also the natural default delay pushes out responses from
being timely.

Users create tables like the following:

#define USER_DEFINED_ACTION_1  1
#define USER_DEFINED_ACTIOM_2  2
#define USER_DEFINED_ACTION_3  3
#define USER_DEFINED_ACTIOM_4  4

HSDIAG_KEYMAP_DECLARE( my_keymap, 20 )
   HSKM_ACT( USER_DEFINED_ACTION_1 ), HSKM( UP ), HSKM( UP ), HSKM( UP ), HSKM( SEL ), HSKM( DOWN ), HSKM( SEL ),
   HSKM_ACT( USER_DEFINED_ACTION_2 ), HSKM( DOWN ), HSKM( UP_LEFT ), HSKM( SEL ),
   HSKM_ACT( USER_DEFINED_ACTION_3 ), HSKM( DOWN ), HSKM( UP_LEFT ), HSKM_SEQ( USER_DEFINED_ACTION_2 ),
   HSKM_ACT( USER_DEFINED_ACTION_4 ), HSKM( DOWN ), HSKM_DELAY( 1000 ), HSKM( SEL ),
HSDIAG_KEYMAP_END

Uers define and implement DIAG ICD command packet handlers following
the nominal procedure.  Seperate handlers can be created for the above
four actions, or all the actions can be consolodate into a single action
handler.  The following assumes that the above USER_DEFINED_ACTION_X id's
are also used in the definition of the DIAG ICD command.

packet_handler_pseudo_code
   action = get_action_from_req
   commit_response
   hsdiag_keymap( action, my_keymap ); 
end

Copyright (c) 2002, 2003 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                            Edit History

  $Header: //depot/asic/qsc6055/services/hs/hsdiag.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/11/07   gmy     Conditionally compile disp_capture() calls
03/08/05   ty      Added display capture support in HS
02/18/05   ar      Added HSKM_IKDELAY & HSKM_KPDELAY; updated keycodes for 
                   Sirius platform.
07/01/03   rsm     Removed #define EVENT_HS_DISPLAY_BMP_CAPTURE_STATUS
08/07/03   gr      Integration from the services/diag version
07/01/03   rsm     Added bitmap capture request/response structures
02/24/03   kap     Introduced hs key map utility.
08/20/01   jal     Updated for core VU
01/26/01   lad     Created file from what used to be diagpkt.h
===========================================================================*/

#include "customer.h"
#include "comdef.h"
#include "diagcmd.h"
#include "diagpkt.h"


/* -------------------------------------------------------------------------
** Packet definitions for logging services.
** ------------------------------------------------------------------------- */

#define DIAGPKT_HS_SCRN_HIGH 4
#define DIAGPKT_HS_SCRN_WIDE 12

#if defined(T_WINNT)
#error code not present
#endif

/* Default number of milliseconds to wait between subsequent key presses
** in a keypress map  
*/
#define HSDIAG_KEYMAP_DEFAULT_DELAY 500
#define HSDIAG_KEYMAP_BUTTPRESS_DELAY 100

/* Masks for action identifiers
**   Client must not define action id's with the MS 16 bits used.
**   Bits are used to indicate to the keymap routines such actions as referring
**     to other action sequences or inserting additional delay
*/
#define HSDIAG_KEYMAP_DELAY_BIT    0x80000000
#define HSDIAG_KEYMAP_ACTION_BIT   0x40000000
#define HSDIAG_KEYMAP_SEQUENCE_BIT 0x20000000
#define HSDIAG_KEYMAP_KPDELAY_BIT  0x08000000
#define HSDIAG_KEYMAP_IKDELAY_BIT  0x04000000

/* Delay mask - to assure user does not clobber the reserved bits
*/
#define HSDIAG_KEYMAP_MS_MASK      0x0008FFFF

/* Last action in a table indicating the end
*/
#define HSDIAG_KEYMAP_TABLE_END    0xFFFFFFFF

#define HS_DISPLAY_STATUS_OK 0


/* -------------------------------------------------------------------------
** Invariant Keys
**   Defined here for DIAG clients to be independent of the keypad and
**   handset implementations.
**   !!! NOTE: This enumeration indexes an array          !!!
**   !!! NOTE: The values must be representable in 16bits !!!
** ------------------------------------------------------------------------- */
typedef enum
{
  HSDIAG_KEY_NONE = 0,
  HSDIAG_KEY_POWER,
  HSDIAG_KEY_UP,
  HSDIAG_KEY_DOWN,
  HSDIAG_KEY_LEFT,
  HSDIAG_KEY_RIGHT,
  HSDIAG_KEY_UP_LEFT,
  HSDIAG_KEY_UP_RIGHT,
  HSDIAG_KEY_DOWN_LEFT,
  HSDIAG_KEY_DOWN_RIGHT,
  HSDIAG_KEY_NAV_SEL,
  HSDIAG_KEY_SEL,
  HSDIAG_KEY_CLR,
  HSDIAG_KEY_0,
  HSDIAG_KEY_1,
  HSDIAG_KEY_2,
  HSDIAG_KEY_3,
  HSDIAG_KEY_4,
  HSDIAG_KEY_5,
  HSDIAG_KEY_6,
  HSDIAG_KEY_7,
  HSDIAG_KEY_8,
  HSDIAG_KEY_9,
  HSDIAG_KEY_POUND,
  HSDIAG_KEY_STAR,
  HSDIAG_KEY_PF1,
  HSDIAG_KEY_PF2,
  HSDIAG_KEY_SEND,
  HSDIAG_KEY_END,
  HSDIAG_KEY_INFO,
  HSDIAG_KEY_VOL_UP,
  HSDIAG_KEY_VOL_DWN,

#if defined(SIRIUS_PLATFORM) && defined(WOLF_5)
#error code not present
#endif /* SIRIUS_PLATFORM && WOLF_5 */

  HSDIAG_KEY_MAX            /* Internal use only */
}
hsdiag_keymap_keys_type;

/*===========================================================================

STRUCT   hsdiag_disp_cap_type

PURPOSE  Keeps data necessary for a screen capture done in HS task context
         for Diag.

===========================================================================*/
#ifdef FEATURE_HS_DISPLAY_CAPTURE
typedef enum{
  DISP_CAP_IDLE,
  DISP_CAP_FIRST_READ,
  DISP_CAP_BUSY
} disp_cap_state;

typedef struct{
  disp_cap_state           state;
  uint16                   starting_row;
  uint16                   starting_column;
  uint16                   height;
  uint16                   width;
  char                    *filename;
  word                    *full_disp_buf;
  byte                    *bmp_buf;
#ifdef FEATURE_HS_DISPLAY_CAPTURE_LIMITED_READ
  uint16                   cont_row;
  uint16                   read_limit;
  uint8                    yield_time;
#endif

  uint16                   delayed_rsp_id;
  uint16                   subsys_cmd_code;
}hsdiag_disp_cap_type;
#endif /* FEATURE_HS_DISPLAY_CAPTURE */

/* -------------------------------------------------------------------------
** MACRO HSKM
** DESCRIPTION
**   Map a slightly short hand name to actual key names.  Basically expands
**   to HSDIAG_KEY_##<name>
** ------------------------------------------------------------------------- */
#define HSKM( xx_key )\
   (HSDIAG_KEY_##xx_key)

/* -------------------------------------------------------------------------
** MACRO HSKM_DELAY
** DESCRIPTION
**    Requests a delay in milliseconds before the next action.  Must be
**    less than 5 minutes (300,000ms) (Chosen somewhat arbitrarily, but also
**    made arbitrarily large)
** ------------------------------------------------------------------------- */
#define HSKM_DELAY( xx_ms )\
   (((xx_ms) & HSDIAG_KEYMAP_MS_MASK) | HSDIAG_KEYMAP_DELAY_BIT)

/* -------------------------------------------------------------------------
** MACRO HSKM_IKDELAY
** DESCRIPTION
**    Sets delay in milliseconds between successive key actions (interkey).
**    Value in effect from set point until later reset or duration of
**    keymap sequence.  Must be less than 5 minutes (300,000ms)
**    (Chosen somewhat arbitrarily, but also made arbitrarily large)
** ------------------------------------------------------------------------- */
#define HSKM_IKDELAY( xx_ms )\
   (((xx_ms) & HSDIAG_KEYMAP_MS_MASK) | HSDIAG_KEYMAP_IKDELAY_BIT)

/* -------------------------------------------------------------------------
** MACRO HSKM_KPDELAY
** DESCRIPTION
**    Sets delay in milliseconds between keypress and release events.
**    Value in effect from set point until later reset or duration of
**    keymap sequence.  Must be less than 5 minutes (300,000ms)
**    (Chosen somewhat arbitrarily, but also made arbitrarily large)
** ------------------------------------------------------------------------- */
#define HSKM_KPDELAY( xx_ms )\
   (((xx_ms) & HSDIAG_KEYMAP_MS_MASK) | HSDIAG_KEYMAP_KPDELAY_BIT)

/* -------------------------------------------------------------------------
** MACRO HSKM_ACT
** DESCRIPTION
**    Defines or refers to a sequnce of key presses.  When used in the
**    action column of a DIAG key map table this defines a key sequence
**    to execute when the action is referred from another sequence.
** ------------------------------------------------------------------------- */
#define HSKM_ACT( xx_id )\
   ((xx_id) | HSDIAG_KEYMAP_ACTION_BIT)

/* -------------------------------------------------------------------------
** MACRO HSKM_SEQ
** DESCRIPTION
**    Refers to an action entry in the table.
** ------------------------------------------------------------------------- */
#define HSKM_SEQ( xx_id )\
   ((xx_id) | HSDIAG_KEYMAP_SEQUENCE_BIT)

/* -------------------------------------------------------------------------
** MACRO HSDIAG_KEYMAP_DECLARE
** DESCRIPTION
**   Starts a key map declaration
**      xx_name            - Name of the table, used in call to hsdiag_keymap()
** ------------------------------------------------------------------------- */
#define HSDIAG_KEYMAP_DECLARE( xx_name )\
   static const uint32 xx_name[] = {

/* -------------------------------------------------------------------------
** MACRO HSDIAG_KEYMAP_END
** DESCRIPTION
**   Ends a key map declaration
** ------------------------------------------------------------------------- */
#define HSDIAG_KEYMAP_END\
      ,HSDIAG_KEYMAP_TABLE_END\
   };


/*===========================================================================

PACKET   diag_hs_key_req_type

ID       DIAG_HS_KEY_F

PURPOSE  Sent by DM to cause a keypress input to the handset interface.

RESPONSE Diag queues a keypress to hsi and echos the request packet.

===========================================================================*/
DIAGPKT_REQ_DEFINE( DIAG_HS_KEY_F )

 boolean hold;                     /* If true, diag witholds key release */
 byte    key;                      /* enumerated key, e.g. HS_DOWN_K */

DIAGPKT_REQ_END

DIAGPKT_DEFINE_RSP_AS_REQ( DIAG_HS_KEY_F )


/*===========================================================================

PACKET   diag_hs_lock_req_type

ID       DIAG_HS_LOCK_F

PURPOSE  Sent by DM to lock or unlock the real handset.

RESPONSE Diag queues a command to hsi with new hs lock state, then
         echos the packet to the DM.

===========================================================================*/
typedef enum {
  HS_LOCK_F,                       /* command to lock real handset */
  HS_REL_F                         /* command to release real handset */
} diag_hs_lock_type;

DIAGPKT_REQ_DEFINE( DIAG_HS_LOCK_F )

 word        lock;        /* diag locks or unlocks real hs
                             Use diag_hs_lock_type.          */

DIAGPKT_REQ_END

DIAGPKT_DEFINE_RSP_AS_REQ( DIAG_HS_LOCK_F )

/*===========================================================================

PACKET   diag_screen_req_type

ID       DIAG_HS_SCREEN_F 

PURPOSE  Sent by DM to request an image of the handset screen and annun-
         ciators.

RESPONSE DMSS returns diag_screen_rsp_type packet that contains handset
         screen and annunciators.

===========================================================================*/

DIAGPKT_REQ_DEFINE( DIAG_HS_SCREEN_F )
DIAGPKT_REQ_END

DIAGPKT_RSP_DEFINE( DIAG_HS_SCREEN_F )

  boolean backlight_on;         /* Backlight state         */
  byte scrn[ DIAGPKT_HS_SCRN_HIGH * DIAGPKT_HS_SCRN_WIDE ];
  byte reserved1;
  word annun;                   /* Annunciators            */
  byte blink_scrn[ DIAGPKT_HS_SCRN_HIGH * DIAGPKT_HS_SCRN_WIDE ];
  word blink_annun;             /* Annunciators for blink  */
  byte rssi;                    /* RSSI indicator          */
  byte reserved2;
  word blink_rate;              /* Blink rate for blinking */

DIAGPKT_RSP_END


#if defined(FEATURE_BITMAP_RASTER)
#error code not present
#endif

/* DIAG commands for HS display */
typedef enum
{
  HS_DISPLAY_GET_PARAMS_CMD = 0,
#ifdef FEATURE_HS_DISPLAY_CAPTURE
  HS_DISPLAY_CAPTURE_CMD = 1,
  HS_DISPLAY_CAPTURE_DELAYED_CMD = 2,
#endif
  HS_DISPLAY_MAX_CMD
} hsdiag_disp_cmd_type;


typedef PACKED struct
{
  uint8  command_code;
  uint8  subsys_id;
  uint16 subsys_cmd_code;
  byte   display_id; /* support for devices with 2 displays */
} hsdiag_get_params_req_type;

/* get params response packet */
/*============================================================================

PACKET   hsdiag_get_params_rsp_type

ID       HS_DISPLAY_GET_PARAMS_CMD

PURPOSE  Sent by DMSS, contains the panel information

RESPONSE DMSS returns hsdiag_get_params_rsp_type packet that contains panel
         information (height, width, depth, type). The depth is actually
     the number of bits-per-pixel . The type gives info as to whether
     the panel orientation is portrait or landscape.
       
============================================================================*/
typedef PACKED struct
{
  uint8   command_code;
  uint8   subsys_id;
  uint16  subsys_cmd_code;
  uint16  disp_height;
  uint16  disp_width;
  uint8   pixel_depth;
  uint8   panel_type;
} hsdiag_disp_get_params_rsp_type;

#ifdef FEATURE_HS_DISPLAY_CAPTURE
/* capture request packet */
/*============================================================================

PACKET   hsdiag_disp_capture_req_type

ID       HS_DISPLAY_CAPTURE_CMD

PURPOSE  Sent to DMSS, requesting display capture to a 24-bit Windows BMP
         format

RESPONSE Upon request DMSS creates a Windows BMP (24-bit) file corresponding
         to the capture co-ordinates requested.
       
============================================================================*/
typedef PACKED struct
{
  uint8   command_code;
  uint8   subsys_id;
  uint16  subsys_cmd_code;
  byte    display_id; /* support for devices with 2 displays */
  uint16  starting_row;
  uint16  starting_column;
  uint16  height;
  uint16  width;
  byte    non_blocking;
  byte    filename[1]; /* variable length .. null terminated */
} hsdiag_disp_capture_req_type;

/* capture request packet, delayed response */
/*============================================================================

PACKET   hsdiag_disp_capture_delayed_req_type

ID       HS_DISPLAY_CAPTURE_DELAYED_CMD

PURPOSE  Sent to DMSS, requesting display capture to a 24-bit Windows BMP
         format with delayed response. DIAG hands off the request to HS, and 
         returns an immediate response. HS sends a delayed response once
         display capture is complete.

RESPONSE Upon request DMSS creates a Windows BMP (24-bit) file corresponding
         to the capture co-ordinates requested.
       
============================================================================*/
typedef PACKED struct
{
  /*standard hdr;*/
  uint8 command_code;
  uint8 subsys_id;
  uint16 subsys_cmd_code;

  byte    display_id; /* support for devices with 2 displays */
  uint16  starting_row;
  uint16  starting_column;
  uint16  height;
  uint16  width;
#ifdef FEATURE_HS_DISPLAY_CAPTURE_LIMITED_READ_PKT_DBG
  uint16  lines_to_read;
  uint8   time_to_yield; 
#endif 
  byte    filename[1]; /* variable length .. null terminated */
} hsdiag_disp_capture_delayed_req_type;
#endif /* FEATURE_HS_DISPLAY_CAPTURE */

/* CAPTURE command status code */
typedef enum
{
  HS_DISPLAY_CAPTURE_OK = 0,
  HS_DISPLAY_INVALID_COORD = 1,
  HS_DISPLAY_CAPTURE_FAIL = 2,
  HS_DISPLAY_FILE_WRITE_FAIL = 3,
  HS_DISPLAY_NO_MEM = 4
} hsdiag_disp_status_type;

/* generic response */
/*============================================================================

PACKET   hsdiag_disp_cmd_rsp_type

ID       HS_DISPLAY_CAPTURE_CMD

PURPOSE  Sent by DMSS, contains the display capture status

RESPONSE DMSS returns hsdiag_disp_cmd_rsp_type packet that contains status
         of the display capture command request. If the 'non_blocking' mode
         is set to '1', the response will be sent as soon as the display
         capture is done, but before writing into file. Once the bitmap
         file is created EVENT_HS_DISPLAY_BMP_CAPTURE_STATUS is sent. The
         response will also contain a transaction id (xid) that could be 
         cross-verified with the event generated (xid) is also part of the
         generated event.
       
============================================================================*/
typedef PACKED struct
{
  uint8 command_code;
  uint8 subsys_id;
  uint16 subsys_cmd_code;
  uint32 xid;
  uint32 status;
} hsdiag_disp_cmd_rsp_type;

/*============================================================================

PACKET   hsdiag_disp_cmd_delayed_rsp_type

ID       HS_DISPLAY_CAPTURE_DELAYED_CMD

PURPOSE  Sent by DMSS, contains the display capture status

RESPONSE DMSS returns hsdiag_disp_cmd_rsp_type packet that contains status
         of the display capture command request. There will be an immediate
         response sent by DIAG, and a delayed response sent by HS once the
         display capture is complete. Status from both packets needs to be
         success for the capture to be successful.
       
============================================================================*/
#ifdef FEATURE_HS_DISPLAY_CAPTURE
typedef PACKED struct
{
  /*diagpkt_subsys_hdr_type_v2 hdr;*/
  uint8 command_code;
  uint8 subsys_id;
  uint16 subsys_cmd_code;
  uint32 status;  
  uint16 delayed_rsp_id;
  uint16 rsp_cnt; /* 0, means one response and 1, means two responses */
} hsdiag_disp_cmd_delayed_rsp_type;

/* event report payload for EVENT_HS_DISPLAY_BMP_CAPTURE_STATUS event */
/*============================================================================

PACKET   hsdiag_disp_capture_event

ID       EVENT_HS_DISPLAY_BMP_CAPTURE_STATUS

PURPOSE  Sent by DMSS, contains the display capture status

RESPONSE If hsdiag_disp_capture_req_type was sent with non_blocking set to 1, 
         an event is generated asynchronously to notify the status of the
         capture. This event contains transaction id (xid) and the status fields
       
============================================================================*/
typedef PACKED struct
{
  uint32  xid;
  uint32  status;
} hsdiag_disp_capture_event;
#endif /* FEATURE_HS_DISPLAY_CAPTURE */ 

/* -------------------------------------------------------------------------
** Functions 
** ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C"
{
#endif

/*===========================================================================

FUNCTION DIAG_KEYMAP

DESCRIPTION
   This routine queues a keymap processing action to be handled by the DIAG
   task in the background.  The caller should commit its response packet
   prior to calling this routine.
    
   Given a table of key sequences and an action, finds the action in the
   table and injects the associated sequence of keys into the keypad driver
   emulating the sequence as though entered physically on the keypad

===========================================================================*/
  void hsdiag_keymap (uint32 action, const uint32 * table);

/*===========================================================================

FUNCTION HSDIAG_HS_DISPLAY_CAPTURE

DESCRIPTION
   This routine is called by HS to perform screen capture for Diag in 
   HS task context. 
   
===========================================================================*/
#ifdef FEATURE_HS_DISPLAY_CAPTURE
#ifdef FEATURE_HS_DISPLAY_CAPTURE_LIMITED_READ
  uint8 hsdiag_hs_display_capture(uint16 read_limit, uint8 yield_time);
#else /* FEATURE_HS_DISPLAY_CAPTURE_LIMITED_READ */
  uint8 hsdiag_hs_display_capture(void);
#endif /* FEATURE_HS_DISPLAY_CAPTURE_LIMITED_READ */
#endif /* FEATURE_HS_DISPLAY_CAPTURE */

#ifdef __cplusplus
}
#endif

#if defined(T_WINNT)
#error code not present
#endif


#endif /* HSDIAG_H */

