#ifndef HS_H_
#define HS_H_
/*===========================================================================

              H A N D S E T   T A S K   H E A D E R   F I L E

DESCRIPTION
  This file contains the data structures used to interface to the Handset
  Task for the various DMSS phones.

REFERENCES
  ALPS, "A Cellular Bus Operation"
  NEC LCD Controller "uPD7228G" Spec Sheet
  Epson LCD Controller SED1230 Spec Sheet
  Hitachi LCD COntroller HS66712 Spec Sheet

Copyright (c) 1992, 1993, 1994 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1995, 1997, 1998 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1999, 2000, 2001 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2002, 2003, 2004 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2005, 2006, 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc6055/services/hs/hs.h#3 $ $DateTime: 2007/08/29 15:56:07 $ $Author: gparekh $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/29/07   gdp     Added HS_TV_K
09/20/05   ty      Added HS_USB_STEREO_HFK_ON_K key
06/13/05   ty      Added keys for smart USB car kit
05/11/05   jkl     Reduced HS_DISP_CAP_READ_LIMIT.
03/08/05   ty      Changes to support display capture in HS
03/07/05   ty      Corrected HS_HEADSET_K, HS_RSK_K, HS_LSK_K, and comments
01/07/05   ty      Included keys from MSM6275, Sirius
12/03/04   ty      Fixed HS_DEV_UNKNOWN_K, HS_EXT_DEV_K to conform to ICD
11/25/04   jac     added HS_LS1_K HS_LS2_K
11/25/04   jac     carkit support
09/30/04   trc     Added clamshell support for EFFA
07/26/04   ap      Added clamshell query function.
07/26/04   ap      Added clamshell states.
08/13/03   kar     Merged in support for Carmen FFA
07/24/03   sg      Added key for STEREO_HEADSET detection.
12/11/02   pd      Added multiple key press support for simulate key input
12/04/02   nxb     Added HWTC wrapper.
12/03/02   sg      Added keys for Headset switch
10/10/02   pd      Added multiple key press support
08/02/02   gtb     Added in resistive keypad support
02/11/02   dl      Added HS_LEFT_K, HS_RIGHT_K and HS_SEL_K.
01/30/02   rmd     Created hs_is_power_key_pressed() which returns the status
                   of the power key.
01/21/02   rmd     Replaced PMIC_HAS_LCDKBD_LED_DRIVER with
                   FEATURE_PMIC_LCDKBD_LED_DRIVER.
01/07/02   rmd     Added support for the new PM60X0 feature names.
04/30/01   kar     Added USB keys under FEATURE_USB_ISOC_AUDIO.
01/15/01   fas     Added lcd_ann_enum_type and removed non-enumeration types
                   of the same name if FFA Clouseau III.
10/06/00   dsb     Removed TG== code.  Target-dependent handset files are
                   automatically included by a make macro.  Renamed include
                   header file protector to avoid clashes with the new
                   make macro name.
06/22/00   dsb     Merged with PP3.0c: Added 2 new keys to support SDAC
                   device.
06/08/00   dsb     Added 5GP LCD support.
10/28/99   jc      Added 2 new keys to support battery charging.
10/27/99   jkl     Added key when Headset is disconnected.
10/19/99   jc      Added code for BSP.
09/09/99   jc      Added structs for lcd/kpd backlight intensity for PM1000
04/22/99   hrk     added keyword PACKED to struct hs_diag_type
01/21/99   hrk      removed FEATURE_UI_SNI around macro HS_AN_SNI as this
                    was causing LED on LCD display to stay on.
11/02/98   dhh     Clear the extra comma in enum so that ARM compiler does
                   not complain.
10/22/98   dhh     Added HFK message support for HFK interface.
10/08/98   vhp     Added external function prototype for passing a DTMF
                   keycode to the receiver.
08/25/98   dhh     Moved HS_EXT_DEV_K to the end of the enum list.
07/13/98   dhh     Added HS_EXT_DEV_K.
06/26/98   dhh     Removed the feature condition in handset key definitions.
                   Added Padlock/Slash Icons for 5GP-1.
03/27/98   dhh     Corrected HS_GLYPH_CNT definition for Odie.
03/27/98   skh     Added FEATURE_UI_SNI.
02/06/98   dhh     Include customer.h directly.
01/27/98   dhh     Added T_O for Odie.
                   Added condition to include the code for Device ID support.
01/16/98   dhh     Added support for Device ID.
07/01/97   dhh     Changed the nest MAX Macros to avoid compile error
                   when building for T_DM, T_SP or T_PC.
06/26/97   dhh     Fixed the error in the Q target change.
06/17/97   dhh     Added T_Q (Q Phones) support.
06/10/97   fkm     Another set of Code Review Changes
05/23/97   jjn     Configured for Module
05/20/97   fkm     Another set of Code Review Changes
04/29/97   fkm     First set of Code Review Changes
03/14/97   jjn     Defined new target called T_MD for the Module and
                   added (TG==T_MD) to all the appropriate conditional
                   compile statements
03/03/97   fkm     Documentation Updates
01/06/97   fkm     TGP and Test Block additions
08/21/95   jmk     Removed T_SP cond comp, added change warning next to keycodes
08/01/95   jmk     Added T_SP conditional compilation (exclude rex, protos, etc)
05/24/95   jah     Added T_I2 (ISS-2) Support.
05/02/95   jah     Fixed Max RSSI for Beta to be 7 instead of 8.
03/30/95   jah     Added T_G (Gemini) Support.
10/06/94   jah     Changed height/width defines to have a base definition
                   per target, to facilitate the DM.
09/19/94   jah     Added HS_SET_GLYPH command
07/05/94   jah     Added HS_DIAG_SET_SCRN and HS_DIAG_SCRN_LOCK commands and
                   associated packet data types to support diag screen loads
06/28/94   jah     Added ISS1 Annunciators, updated comments
05/11/94   jah     Added HS_ON_HOOK_K and HS_OFF_HOOK_K.
09/22/93   jah     Added HS_EXT_PWR_ON_K and HS_EXT_PWR_OFF_K for Portable
                   external power sensing.
05/14/93   jah     Added more T_B2 (Beta II) conditional compilation
05/11/93   jah     Added T_B2 (Beta II) conditional compilation
03/18/93   jah     Added prototype for hs_set_char(), Portable-only.
03/15/93   jah     Added prototype for hs_poll_gpio.
01/26/93   jah     Added comment to header per code review.
01/15/93   jah     Moved the Portable "Special graphics characters" from 0..7
                   to 8..F (duplicate map).  Added HS_HALF_BOX.
12/07/92   jah     Added HS_BAD_BATT_K for reporting a bad battery in the
                   mobile.
10/23/92   jah     Added commands HS_ALL_ANNUN and HS_ALL_SCRN, and the assoc.
                   data structures.
10/20/92   jah     Added include of target.h.  Added definitons for the
                   Portable annunciators.
10/06/92   jah     Comment updates for portable/mobile differences.  Added
                   portable user-defined characters and screen size.
08/19/92   jah     Changed HS_PWR_K to HS_PWR_ON/OFF_K's for UI convenience.
08/06/92   jah     Converted entirely over to the new ALPS handset
08/02/92   jah     Added keys_free to hs_key_packet_type, for returning the
                   amount of free space in the input buffer.
07/08/92   jah     Initial revision

===========================================================================*/

#include "comdef.h"     /* Definitions for byte, word, etc.     */
#include "queue.h"      /* Definition of Queue services         */
#include "rex.h"        /* definition of REX data types         */
#include "customer.h"   /* Customer specific definitions        */
#include "target.h"     /* Target specific definitions          */


#ifdef HS_H
#include HS_H
#else
#error Must Specify /DHS_H=HS_xxxx.H on the compile line
#endif


/*===========================================================================

                        DEFINITIONS

===========================================================================*/

/* Set defaults for read limits in hs_display_capture() if not defined */
/* These values are used to read the LCD framebuffer in smaller portions */
#ifdef FEATURE_HS_DISPLAY_CAPTURE_LIMITED_READ
 #ifndef HS_DISP_CAP_READ_LIMIT
  #define HS_DISP_CAP_READ_LIMIT 30 /* lines to read */
  #define HS_DISP_CAP_YIELD_TIME 25 /* time (ms) to yield for each portion */
 #endif
#endif /* FEATURE_HS_DISPLAY_CAPTURE_LIMITED_READ */


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Commands and Command Header
---------------------------------------------------------------------------*/

/* Handset Operations which can be performed/requested.  Note that not all
** targets support all operations.  For backwards compatability, we never
** delete items from this table, so some of these operations do not apply
** to newer phones and visa versa.
*/
typedef enum {
  HS_ACTIVE = 0,        /* unused                                            */
  HS_SLEEP,             /* unused                                            */
  HS_SCRN_CTL,          /* Turn on/off the LCD screen                        */
  HS_BACKLIGHT,         /* Turn on/off the screen & keypad backlights        */
  HS_SCRN_UPDATE,       /* Update the screen image                           */
  HS_ANNUN,             /* Turn on/off the annunciators                      */
  HS_BLINK_ANNUN,       /* Set which annunciators blink                      */
  HS_SCRN_RSSI,         /* Set the level of the RSSI bar                     */
  HS_AUD_IN,            /* Set the audio input path                          */
  HS_AUD_OUT,           /* Set the audio output path                         */
  HS_SKI,               /* Simulate keypad input                             */
  HS_UPDATE_BLINK,      /* Update the screen used for blinking               */
  HS_BLINK_RATE,        /* Set the blink rate                                */
  HS_DIAG,              /* Diag state dump                                   */
  HS_DIAG_LOCK,         /* Keypad lock                                       */
  HS_DTMF_DET,          /* Control polling of the DTMF detector              */
  HS_ALL_ANNUN,         /* Set both regular and blinking annunciators        */
  HS_ALL_SCRN,          /* Update both the blink and regular screen          */
  HS_DIAG_SET_SCRN,     /* Set the contents of the screen                    */
  HS_DIAG_SCRN_LOCK,    /* Lock screen updates, restrict diag commands       */
  HS_SET_GLYPH,         /* Set the bitmap of a glyph                         */
  HS_SCRN_BATTERY,      /* Set the battery level display                     */
  HS_SEVEN_SEG,         /* Set Seven Segment Display(s)                      */
#ifdef FEATURE_IDS
#error code not present
#endif
#if defined(FEATURE_PMIC_LCDKBD_LED_DRIVER)
  HS_SET_LCD_LEVEL,     /* Set intensity level for LCD backlight            */
  HS_SET_KPD_LEVEL,      /* Set intensity level for KPD backlight            */
#endif
#ifdef FEATURE_BSPUI_API
  HS_SNAPSHOT,
  HS_KPD_SRVC,
  HS_LCD_SRVC,
  HS_KPD_KEY_REG,
#endif

#ifdef CUST_EDITION
  HS_SET_VIB_MOTO_ONOFF, /* Set sig led on/off profile                       */
#endif
  HS_OP_MAX
} hs_op_type;

/* Command header common to all command packets
*/
typedef struct {
  q_link_type   link;        /* Queue link                            */
  q_type        *done_q_ptr; /* Queue to put 'this' on when done      */
  rex_tcb_type  *task_ptr;   /* Task to signal when done              */
  rex_sigs_type sigs;        /* Signals to signal *task_ptr when done */
  hs_op_type    cmd;         /* Handset Command                       */
} hs_hdr_type;

/*---------------------------------------------------------------------------
  Keypad declarations
---------------------------------------------------------------------------*/

/* These are the keycodes returned by calls to hs_key_get().  Do not depend
** on their numeric value, just use the symbolic values.  Some of these have
** been set to specific values to make decoding the hardware responses
** more convenient.  Note that not all keys apply to all targets and in fact
** some are very ancient in origin.  However, for backward compatiblity
** we never delete keys.
**
** NOTE: These values are used in the interface to external devices.  When
** defining new keys, be sure to choose a unique new value, to preserve the
** interface.
**
** In the interest of preserving a consistent interface to facilitate 
** testing, enumerations should keep a constant key value whether a
** feature is defined, or not.
*/
typedef enum {            /* KEYS                                        */
  HS_NONE_K        = 0x00,/* No more keycodes available          0x00    */
  HS_ON_HOOK_K     = 0x01,/* phone has gone on-hook              0x01    */
  HS_OFF_HOOK_K    = 0x02,/* phone has gone off-hook             0x02    */
  HS_RING_VOL_0_K  = 0x03,/* ringer volume 0 (Keep 0-2 in order) 0x03    */
  HS_RING_VOL_1_K  = 0x04,/* ringer volume 1 ( and sequential!!) 0x04    */
  HS_RING_VOL_2_K  = 0x05,/* ringer volume 2                     0x05    */
  HS_EAR_UP_K      = 0x06,/* earpiece up                         0x06    */
  HS_EAR_UP_END_K  = 0x07,/* earpiece up + end pressed           0x07    */
  HS_EAR_DOWN_K    = 0x08,/* earpiece down                       0x08    */
  HS_PF1_K         = 0x09,/* softkey #1 (left-most)              0x09    */
#ifdef FEATURE_NAV_KEYPAD                                          
  HS_LEFT_K = HS_PF1_K,   /* left key                            0x09    */
#endif /* FEATURE_NAV_KEYPAD */                                    
  HS_PF2_K         = 0x0A,/* softkey #2 (right-most)             0x0A    */
#ifdef FEATURE_NAV_KEYPAD                                          
  HS_RIGHT_K = HS_PF2_K,  /* right key                           0x0A    */
#endif /* FEATURE_NAV_KEYPAD */                                    
  HS_MSG_K         = 0x0B,/* message waiting                     0x0B    */
  HS_POUND_K       = 0x23,/* '#' key, ASCII '#'                  0x23    */
  HS_STAR_K        = 0x2A,/* '*' key, ASCII '*'                  0x2A    */
  HS_0_K           = 0x30,/* '0' key, ASCII '0'                  0x30    */
  HS_1_K           = 0x31,/* '1' key, ASCII '1'                  0x31    */
  HS_2_K           = 0x32,/* '2' key, ASCII '2'                  0x32    */
  HS_3_K           = 0x33,/* '3' key, ASCII '3'                  0x33    */
  HS_4_K           = 0x34,/* '4' key, ASCII '4'                  0x34    */
  HS_5_K           = 0x35,/* '5' key, ASCII '5'                  0x35    */
  HS_6_K           = 0x36,/* '6' key, ASCII '6'                  0x36    */
  HS_7_K           = 0x37,/* '7' key, ASCII '7'                  0x37    */
  HS_8_K           = 0x38,/* '8' key, ASCII '8'                  0x38    */
  HS_9_K           = 0x39,/* '9' key, ASCII '9'                  0x39    */
  HS_SEND_K        = 0x50,/* Send key                            0x50    */
  HS_END_K         = 0x51,/* End key or Power key (Target Based) 0x51    */
  HS_CLR_K         = 0x52,/* Clear key                           0x52    */
  HS_STO_K         = 0x53,/* Store key                           0x53    */
  HS_UP_K          = 0x54,/* Up-arrow key was pressed            0x54    */
  HS_DOWN_K        = 0x55,/* Down-arrow key was pressed          0x55    */
  HS_MUTE_K        = 0x56,/* Mute Key                            0x56    */
  HS_RCL_K         = 0x57,/* Recall key                          0x57    */
  HS_SD1_K         = 0x58,/* speed dial #1                       0x58    */
  HS_SD2_K         = 0x59,/* speed dial #2                       0x59    */
  HS_SD3_K         = 0x5A,/* speed dial #3                       0x5A    */
  HS_MENU_K        = 0x5B,/* Menu key                            0x5B    */
  HS_ALPHA_K       = 0x5C,/* Alpha key                           0x5C    */
  HS_DEC_POUND_K   = 0x5D,/* Decoded # DTMF                      0x5D    */
  HS_DEC_STAR_K    = 0x5E,/* Decoded * DTMF                      0x5E    */
  HS_DEC_0_K       = 0x5F,/* Decoded 0 DTMF                      0x5F    */
  HS_DEC_1_K       = 0x60,/* Decoded 1 DTMF                      0x60    */
  HS_DEC_2_K       = 0x61,/* Decoded 2 DTMF                      0x61    */
  HS_DEC_3_K       = 0x62,/* Decoded 3 DTMF                      0x62    */
  HS_DEC_4_K       = 0x63,/* Decoded 4 DTMF                      0x63    */
  HS_DEC_5_K       = 0x64,/* Decoded 5 DTMF                      0x64    */
  HS_DEC_6_K       = 0x65,/* Decoded 6 DTMF                      0x65    */
  HS_DEC_7_K       = 0x66,/* Decoded 7 DTMF                      0x66    */
  HS_DEC_8_K       = 0x67,/* Decoded 8 DTMF                      0x67    */
  HS_DEC_9_K       = 0x68,/* Decoded 9 DTMF                      0x68    */
  HS_DEC_A_K       = 0x69,/* Decoded A DTMF                      0x69    */
  HS_DEC_B_K       = 0x6A,/* Decoded B DTMF                      0x6A    */
  HS_DEC_C_K       = 0x6B,/* Decoded C DTMF                      0x6B    */
  HS_DEC_D_K       = 0x6C,/* Decoded D DTMF                      0x6C    */
  HS_PWR_ON_K      = 0x6D,/* Power key is switched 'on'          0x6D    */
  HS_PWR_OFF_K     = 0x6E,/* Power key is switched 'off'         0x6E    */
  HS_PWR_K         = 0x6F,/* Power key                           0x6F    */
  HS_INFO_K        = 0x70,/* Info key                            0x70    */
#if defined(FEATURE_NAV_KEYPAD) || defined(FEATURE_RESISTIVE_KEYPAD)
  //HS_SEL_K = HS_INFO_K,   /* Select key                          0x70    */
  HS_SEL_K         = 0x3A,   /* Select key   */
#endif /* FEATURE_NAV_KEYPAD */
  HS_FREE_K        = 0x71,/* Phone placed in hands-free cradle   0x71    */
  HS_PHONE_K       = 0x72,/* Phone lifted from hands-free cradle 0x72    */
  HS_IGN_OFF_K     = 0x73,/* Ignition was turned off             0x73    */
  HS_IGN_ON_K      = 0x74,/* Ignition was turned on              0x74    */
  HS_TMR_ON_K      = 0x75,/* Power-up timer 'on' pseudo-key      0x75    */
  HS_TMR_OFF_K     = 0x76,/* Power-up timer 'off' pseudo-key     0x76    */
  HS_BAD_BATT_K    = 0x77,/* The car battery is failing          0x77    */
  HS_EXT_PWR_ON_K  = 0x78,/* External power was turned on        0x78    */
  HS_EXT_PWR_OFF_K = 0x79,/* External power was turned off       0x79    */
  HS_REDIAL_K      = 0x7A,/* Redial key                          0x7A    */
  HS_RSSI_K        = 0x7B,/* RSSI key                            0x7B    */
  HS_HFK_CORD_K    = 0x7C,/* Coil cord carkit                    0x7C    */
  HS_HFK_NOCORD_K  = 0x7D,/* Non-coil cord carkit                0x7D    */
  HS_HEADSET_K     = 0x7E,/* (Mono) Headset connected            0x7E    */
  HS_DEV_UNKNOWN_K = 0x7F,/* Unknown device ID                   0x7F    */
  HS_EXT_DEV_K     = 0x80,/* ONES detector finds external device 0x80    */
  HS_STEREO_HEADSET_K = 0x82,/* Stereo Headset connected         0x82    */
  HS_HEADSET_OFF_K = 0x83,/* Headset disconnected                0x83    */

  HS_HEADSET_SWITCH_K = 0x84,/* Headset Switch on                0x84    */
  HS_HEADSET_SWITCH_OFF_K = 0x85,/* Headset Switch off           0x85    */

  HS_LSPKR_K       = 0x86,/* Loudspeaker key                     0x86    */
  HS_PTT_K         = 0x87,/* PTT key                             0x87    */
  HS_CHARGING_ON_K = 0x88,/* Key to note battery charging active 0x88    */
  HS_CHARGING_OFF_K= 0x89,/* Key to note battery charging stopped0x89    */

  HS_SDAC_ON_K     = 0x8A,/* Key to note SDAC in use             0x8A    */
  HS_SDAC_OFF_K    = 0x8B,/* Key to note SDAC not in use         0x8B    */

  HS_USB_HFK_ON_K  = 0x8C,/* Key to note USB in use              0x8C    */
  HS_USB_HFK_OFF_K = 0x8D,/* Key to note USB not in use          0x8D    */
  HS_USB_HFK_BUTTON_K = 0x8E,/* Basic carkit button              0x8E    */
  HS_USB_HFK_GENERIC_BUTTON_K = HS_USB_HFK_BUTTON_K,
  /* rename key, other USB keys start at 0xA6 */

  HS_VOL_UP_K      = 0x8F,/* Volume Up Key                       0x8F    */
  HS_VOL_DOWN_K    = 0x90,/* Volume Down Key                     0x90    */
  HS_FA_K          = 0x91,/* FA Key                              0x91    */
  HS_FB_K          = 0x92,/* FB Key                              0x92    */
  HS_SF1_K         = 0x93,/* SF1 Key                             0x93    */
  HS_SF2_K         = 0x94,/* SF2 Key                             0x94    */
  HS_R_UP_K        = 0x95,/* R+ Key                              0x95    */
  HS_R_DOWN_K      = 0x96,/* R- Key                              0x96    */
  HS_F1_K          = 0x97,/* F1 Key                              0x97    */
  HS_F2_K          = 0x98,/* F2 Key                              0x98    */
  HS_F3_K          = 0x99,/* F3 Key                              0x99    */
  HS_F4_K          = 0x9A,/* F4 Key                              0x9A    */
  HS_F5_K          = 0x9B,/* F5 Key                              0x9B    */
  HS_FLIP_OPEN_K   = 0x9C,/* Flip Open Key (Clamshell)           0x9C    */
  HS_FLIP_CLOSE_K  = 0x9D,/* Flip close Key (Clamshell)          0x9D    */
  
  HS_LS1_K         = 0x9E,/* Left soft Key                       0x9E    */
  HS_LSK_K         = HS_LS1_K,/* Left front key                  0x9E    */

  HS_LS2_K         = 0x9F,/* Right soft Key                      0x9F    */
  HS_RSK_K         = HS_LS2_K,/* Right front key                 0x9F    */

  HS_TASK_K        = 0xA0,/* Task Key                            0xA0    */
  HS_VIDEO_K       = 0xA1,/* Video Key                           0xA1    */
  HS_CAMERA_K      = 0xA2,/* Camera Key                          0xA2    */
  HS_WEB_K         = 0xA5,/* Web Key                             0xA5    */

  HS_USB_HFK_SEND_BUTTON_K = 0xA6,
                          /* Smart carkit send                   0xA6    */
  HS_USB_HFK_END_BUTTON_K = 0xA7,
                          /* Smart carkit end                    0xA7    */
  HS_USB_HFK_VOICEDIAL_BUTTON_K = 0xA8,
                          /* Smart carkit voice dial             0xA8    */
  HS_USB_HFK_RETURN_TO_CRADLE_K = 0xA9,
                          /* Smart carkit return to cradle       0xA9    */
  HS_USB_HFK_REMOVE_FROM_CRADLE_K = 0xAA,
                          /* Smart carkit remove from cradle     0xAA    */
  HS_USB_STEREO_HFK_ON_K = 0xAB,
                          /* Stereo carkit attached              0xAB    */
#if defined(FEATURE_PMIC_HS_SEND_END_DETECT)
  HS_HSED_K = 0xAC,
							/* Headset Send End Key              0xAC    */
#endif
#ifdef CUST_EDITION
  HS_TV_K          = 0xB2,
  HS_FM_K          = 0xB3,
  HS_MUSIC_K	   = 0xb4,
  HS_BLUETOOTH_K   = 0xb5,
  HS_FFWD_K        = 0xb6,
  HS_RWD_K         = 0xb7,
  HS_PAUSE_K       = 0xb8, 
#endif  
                          /* TV Key on JAFFA FFA for 6280+UBM    0xB2    */
#ifdef FEATURE_ALL_KEY_PAD
	  HS_Q_K = 0xD0,
	  HS_W_K = 0xD1,
	  HS_E_K = 0xD2,
	  HS_R_K = 0xD3,
	  HS_T_K = 0xD4,
	  HS_Y_K = 0xD5,
	  HS_U_K = 0xD6,
	  HS_I_K = 0xD7,
	  HS_O_K = 0xD8,
	  HS_P_K = 0xD9,
	  HS_A_K = 0xDA,
	  HS_S_K = 0xDB,
	  HS_D_K = 0xDC,
	  HS_F_K = 0xDD,
	  HS_G_K = 0xDE,
	  HS_H_K = 0xDF,
	  HS_J_K = 0xE0,
	  HS_K_K = 0xE1,
	  HS_L_K = 0xE2,
	  HS_SHIFT_K = 0xE3,
	  HS_Z_K = 0xE4,
	  HS_X_K = 0xE5,
	  HS_C_K = 0xE6,
	  HS_V_K = 0xE7,
	  HS_B_K = 0xE8,
	  HS_N_K = 0xE9,
	  HS_M_K = 0xEA,
	  HS_DOLLAR_K = 0xEB,
	  HS_ENTER_K = 0xEC,
	  HS_CAPLK_K = 0xED,
	  HS_SPACE_K = 0xEE,
	  HS_SYMBOL_K = 0xEF,
	  HS_LCTRL_K = 0xF0,
#endif

  HS_RELEASE_K     = 0xFF /* Key to note that all keys are up    0xFF    */
} hs_key_type;

/*---------------------------------------------------------------------------
  Screen declarations
---------------------------------------------------------------------------*/

/* Screen dimensions for EACH TARGET.  This are exposed in this way for
** some of the support tools.  Phone software should NEVER use these
** definition directly.  Use HS_SCRN_{HIGH,WIDE} instead.
*/

/* T_G Gemini */
#define HS_SCRN_HIGH_G  4       /* Screen height in characters */
#define HS_SCRN_WIDE_G  12      /* Screen width in characters  */

/* T_O Odie Phones */
#define HS_SCRN_HIGH_O  4       /* Screen height in characters */
#define HS_SCRN_WIDE_O  12      /* Screen width in characters  */

/* Define the maximum size for both Hight and Width accross all targets */
/* These MAX values need to be selected from the above definitions,     */
/* whichever is the largest value.                                      */

#define HS_SCRN_HIGH_MAX  4
#define HS_SCRN_WIDE_MAX  12


#ifdef T_UI_S510X
#define HS_SCRN_HIGH    HS_SCRN_HIGH_G
#define HS_SCRN_WIDE    HS_SCRN_WIDE_G
#elif defined(T_UI_V2428)
#define HS_SCRN_HIGH    HS_SCRN_HIGH_O
#define HS_SCRN_WIDE    HS_SCRN_WIDE_O
#else
#error Unsupported handset type
#endif

#define HS_SCRN_ROW_0   0                       /* Index of 0th row */
#if HS_SCRN_HIGH > 1
#define HS_SCRN_ROW_1   (1 * HS_SCRN_WIDE)      /* Index of 1st row */
#endif
#if HS_SCRN_HIGH > 2
#define HS_SCRN_ROW_2   (2 * HS_SCRN_WIDE)      /* Index of 2nd row */
#endif
#if HS_SCRN_HIGH > 3
#define HS_SCRN_ROW_3   (3 * HS_SCRN_WIDE)      /* Index of 3rd row */
#endif

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

/* Screen character image type
*/
typedef byte hs_scrn_type[ HS_SCRN_HIGH * HS_SCRN_WIDE ];

/* Special graphics characters (or best approximation)
*/
#ifdef T_UI_S510X
#define HS_EMPTY_BOX    ((byte) 0xDC)   /* Empty box            */
#define HS_FILL_BOX     ((byte) 0x8F)   /* Filled box           */
#elif defined(T_UI_V2428)
#define HS_EMPTY_BOX    ((byte) 0x20)   /* Empty box            */
#define HS_FILL_BOX     ((byte) 0x8F)   /* Filled box           */
#endif

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

/* The LCD controllers contain fixed representations of the various characters
** (A-Z, 0-9, etc) as well as any special characters we had the manufacturer
** define in silicon.  However, each controller also allows us to define
** several characters on the fly as we run.  We call these "Glyph" characters
** and they are used for "late breaking" characters that where not anticipated
** at the time the LCD control was specified.  So far, these characters are all
** 5 columns packed 1 row per byte, 7 rows.  For a given row, 0x00 turns all
** points off, 0x1F turns all (five) points on.
*/
#define HS_GLYPH_WIDTH   5
#define HS_GLYPH_HEIGHT  7
typedef byte hs_glyph_type[ HS_GLYPH_HEIGHT ];

/* Number of glyphs supported
*/
#ifdef T_UI_V2428
#define HS_GLYPH_CNT     8
#elif defined (T_UI_S510X)
#define HS_GLYPH_CNT     4
#else
#error Unsupported handset type
#endif

/* Character definition command packet for HS_SET_GLYPH
*/
typedef struct {
  hs_hdr_type hdr;      /* Command header                         */
  byte index;           /* character index of glyph being defined */
  hs_glyph_type bitmap; /* bitmap of glyph being defined          */
} hs_glyph_packet_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

/* Screen/Backlight on/off command packet for HS_SCRN_CTL and HS_BACKLIGHT
*/
typedef struct {
  hs_hdr_type hdr;      /* Command header */
  boolean on_off;       /* on/off control */
} hs_ctl_packet_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

/* Screen update command packet for HS_SCRN_UPDATE and HS_UPDATE_BLINK
*/
typedef struct {
  hs_hdr_type hdr;        /* Command header */
  hs_scrn_type *scrn_ptr; /* Screen image   */
} hs_scrn_packet_type;

/* Screen update command packet for HS_ALL_SCRN
*/
typedef struct {
  hs_hdr_type hdr;              /* Command header     */
  hs_scrn_type *scrn_ptr;       /* Screen image       */
  hs_scrn_type *blink_scrn_ptr; /* Blink screen image */
} hs_scrna_packet_type;

/* Blink command packet for HS_BLINK_RATE
*/
typedef struct {
  hs_hdr_type hdr;      /* Command header                           */
  word blink_rate;      /* rate for blinking milliseconds per image */
} hs_blink_packet_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

/* RSSI level command packet for HS_SCRN_RSSI
*/
typedef struct {
  hs_hdr_type hdr;      /* Command header   */
  byte rssi;            /* RSSI level       */
  byte blink_rssi;      /* Blink RSSI level */
} hs_rssi_packet_type;

/* Define how RSSI level is displayed for targets which have
** an actual RSSI display
*/
#ifdef T_UI_S510X
#define HS_MAX_RSSI     5     /* RSSI range is 0..5 */
#elif defined(T_UI_V2428)
#define HS_MAX_RSSI     7     /* RSSI range is 0..7 */
#else
#error Unsupported handset type
#endif

/* Battery level command packet for HS_SCRN_BATTERY
*/
typedef struct {
  hs_hdr_type hdr;      /* Command header      */
  byte batt;            /* Battery level       */
  byte blink_batt;      /* Blink battery level */
} hs_batt_packet_type;

/* Define how Battery level is displayed for targets which have
** an actual Battery display
*/
#ifdef T_UI_V2428
#define HS_MAX_BATTERY  5     /* Battery range is 0..5 */
#endif

/*---------------------------------------------------------------------------
  Annunciator declarations (Both on-screen and LED)
---------------------------------------------------------------------------*/

/* Annunciator Masks/Values (comment on each is a "rough" meaning of each)
*/

#ifndef FEATURE_BSPUI_API

#define HS_AN_ALL       0xffff                  /* Mask of all annunciators */
#define HS_ANNUNCIATORS 12

#endif

#ifdef T_UI_S510X

#ifdef FEATURE_BSPUI_API
typedef enum{
  HS_AN_CDMA         =0x0001,
  HS_AN_INUSE        =0x0002,
  HS_AN_NOSVC        =0x0004,
  HS_AN_ROAM         =0x0008,
  HS_AN_MSG          =0x0010,
  HS_AN_VMAIL        =0x0020,
  HS_AN_SNI          =0x0040,
  HS_AN_ALL          =0xFFFF,
  HS_AN_RSSI         =0x1000,
  HS_AN_BATTERY      =0x2000,
  NUM_ANNUN          =10
}lcd_ann_enum_type;
#else
#define HS_AN_CDMA      0x0001  /* CDMA                */
#define HS_AN_INUSE     0x0002  /* In Use              */
#define HS_AN_NOSVC     0x0004  /* No Svc (no service) */
#define HS_AN_ROAM      0x0008  /* Roam                */
#define HS_AN_MSG       0x0010  /* Message             */
#define HS_AN_VMAIL     0x0020  /* Voice mail          */
#define HS_AN_SNI       0x0040  /* Silent Notification */
#endif

#elif defined(T_UI_V2428)
typedef enum{
  HS_AN_CDMA         =0x0001,
  HS_AN_INUSE        =0x0002,
  HS_AN_NOSVC        =0x0004,
  HS_AN_ROAM         =0x0008,
  HS_AN_MSG          =0x0010,
  HS_AN_VMAIL        =0x0020,
  HS_AN_SNI          =0x0040,
  HS_AN_ALL          =0xFFFF,
  HS_AN_RSSI         =0x1000,
  HS_AN_BATTERY      =0x2000,
  NUM_ANNUN          =10
}lcd_ann_enum_type;

//#define HS_AN_CDMA      0x0001  /* CDMA                */
//#define HS_AN_INUSE     0x0002  /* In Use              */
//#define HS_AN_NOSVC     0x0004  /* No Svc (no service) */
//#define HS_AN_ROAM      0x0008  /* Roam                */
//#define HS_AN_MSG       0x0010  /* Message             */
//#define HS_AN_PADLOCK   0x0020  /* Padlock             */
//#define HS_AN_VMAIL     0x0020  /* Voice mail          */
//#define HS_AN_NOLOCK    0x0040  /* Padlock and Slash   */

#else
#error Unsupported handset type
#endif

/* Annunciator control type
*/
typedef struct {
  word mask;            /* Mask of annunciators to modify   */
  word value;           /* Values of annunciators to modify */
} hs_ann_type;

/* Annunciator command packet for HS_ANNUN and HS_BLINK_ANNUN
*/
typedef struct {
  hs_hdr_type hdr;      /* Command header     */
  hs_ann_type annun;    /* Annunciator change */
} hs_ann_packet_type;

/* Annunciator command packet for HS_ALL_ANNUN
*/
typedef struct {
  hs_hdr_type hdr;          /* Command header     */
  hs_ann_type annun;        /* Annunciator change */
  hs_ann_type blink_annun;  /* Annunciator change */
} hs_anna_packet_type;

/*---------------------------------------------------------------------------
  Audio control (for the Sound Services)
---------------------------------------------------------------------------*/

/* Handset audio paths
** Not all of the audio paths, just the ones via the handset/cradle.
*/
typedef enum {
  HS_AUD_HS = 0, /* Audio path is the handset         */
  HS_AUD_HF,     /* Audio path is the hands-free path */
  HS_AUD_MUTE    /* Audio path is muted               */
} hs_aud_type;

/* Audio path command packet for HS_AUD_IN and HS_AUD_OUT
*/
typedef struct {
  hs_hdr_type hdr;      /* Command header */
  hs_aud_type apath;    /* Audio path     */
} hs_aud_packet_type;

/* DTMF detector polling on/off command packet for HS_DTMF_DET
*/
typedef struct {
  hs_hdr_type hdr;      /* Command header */
  boolean on_off;       /* on/off control */
} hs_dtmf_packet_type;

/*---------------------------------------------------------------------------
  Diagnostic input/output/control
---------------------------------------------------------------------------*/

/* Simulate-keypad-input command packet for HS_SKI (for DM HS simulator)
*/
typedef struct {
  hs_hdr_type hdr;      /* Command header                  */
  byte keycode;         /* Keycode input                   */
#ifdef FEATURE_KEYPAD_MULTI_KEY
  byte keyparm;         /* which key is released           */
#endif
  word keys_free;       /* Number of free key buffer slots */
} hs_key_packet_type;

/* Description of the buffer of information describing the
** handset's screen/annunciator state.
*/
typedef PACKED struct {
  boolean backlight_on;         /* Backlight state         */
  hs_scrn_type scrn;            /* Screen                  */
  word annun;                   /* Annunciators            */
  hs_scrn_type blink_scrn;      /* Screen for blinking     */
  word blink_annun;             /* Annunciators for blink  */
  byte rssi;                    /* RSSI indicator          */
  word blink_rate;              /* Blink rate for blinking */
} hs_diag_type;

/* Handset information dump command packet for HS_DIAG (diag snapshot),
** and screen information command packet for HS_DIAG_SET_SCRN.
*/
typedef struct {
  hs_hdr_type hdr;              /* Command header */
  hs_diag_type *diag_ptr;       /* Handset state snapshot */
} hs_diag_packet_type;

/* Handset keypad lock command packet for HS_DIAG_LOCK and HS_DIAG_SCRN_LOCK
*/
typedef struct {
  hs_hdr_type hdr;      /* Command header */
  boolean lock;         /* Lock/un-lock   */
} hs_lock_packet_type;

/* Seven Segment Display Command Packet for HS_SEVEN_SEG.  Each of the
** seven segments is controlled by one bit in a 8 bit byte (sign bit unused).
** Build characters by ORing together these individual Segment values.
** (e.g. "7" = (HS_SEGMENT_HTOP_V|HS_SEGMENT_VTOP_R_V|HS_SEGMENT_VBOT_R_V)
**
** 0x40 ->  ---
** 0x20 ->  | |  <- 0x10
** 0x08 ->  ---
** 0x04 ->  | |  <- 0x02
** 0x01 ->  ---
*/

/* This typedef defines a structure that can hold values for all Seven
** Segment Displays on the current target.
*/
#define HS_NUM_7SEGS            3       /* # of Seven Segment Displays  */
typedef byte hs_7seg_type[HS_NUM_7SEGS];

/* These values can be ORed together to make symbols from on the Seven
** Segment displays.  Just OR in the segments you want "on".  (E.G. the
** number "7" would be:
**
** (HS_SEGMENT_HTOP|HS_SEGMENT_VTOP_R_V|HS_SEGMENT_VBOT_R_V)
*/
#define HS_SEGMENT_HTOP_V       0x40    /* Horizontal Top Segment       */
#define HS_SEGMENT_HMID_V       0x08    /* Horizontal Middle Segment    */
#define HS_SEGMENT_HBOT_V       0x01    /* Horizontal Bottom Segment    */

#define HS_SEGMENT_VTOP_L_V     0x20    /* Veritical Top Left Segment   */
#define HS_SEGMENT_VTOP_R_V     0x10    /* Veritical Top Right Segment  */

#define HS_SEGMENT_VBOT_L_V     0x04    /* Veritical Bottom Left Seg    */
#define HS_SEGMENT_VBOT_R_V     0x02    /* Veritical Bottom Right Seg   */

typedef struct {
  hs_hdr_type hdr;                      /* Command header               */
  hs_7seg_type seven_seg;               /* Non-Blink 7 Seg Values       */
  hs_7seg_type blink_7seg;              /* Blink 7 Seg Values           */
} hs_7seg_packet_type;

#ifdef FEATURE_IDS
#error code not present
#endif



#if defined(FEATURE_PMIC_LCDKBD_LED_DRIVER)
/* LCD/KPD intensity command packet for HS_SET_LCD_LEVEL and HS_SET_KPD_LEVEL
*/
typedef struct {
  hs_hdr_type hdr;      /* Command header  */
  byte  level;          /* intensity level */
} hs_ctl2_packet_type;
#endif



#if defined(FEATURE_BSPUI_API) || defined(FEATURE_HWTC)
/* kpd handle returned by the subscription function */
typedef struct{
  byte id;
  byte status;
}kpd_handle_type;

/* lcd handle returned by the subscription function */
typedef struct{
  byte id;
  byte status;
}lcd_handle_type;

/* the handle returned by the register key event function */
#ifdef FEATURE_KEYPAD_MULTI_KEY
typedef struct{
  byte key_code; /* The current key code                               */
  byte key_parm; /* if key_code is HS_RELEASE_K, then the release key  */
}keypad_key_event_type;
#endif /* FEATURE_KEYPAD_MULTI_KEY */

typedef struct{
  byte key_code;    /* pointer to the key code buffer.*/
}kpd_key_event_type;

/* the snapshot of the display */
typedef struct{
  boolean      backlight_on;
  hs_scrn_type scrn;         /* non blinking screen */
  hs_scrn_type blink_scrn;   /*blinking screen */
  hs_ann_type  annun;
  hs_ann_type  blink_annun;
  byte         rssi;
  byte         blink_rssi;
  byte         batt;
  byte         blink_batt;
  word         blink_rate;
}lcd_snapshot_type;


/* enumeration used in the subscribe and cancel services */
typedef enum{
  HS_SUBSCRIBE,
  HS_CANCEL
}sub_command;

/* enumeration used in the subscribe and cancel services */
typedef enum{
  HS_SUBSCRIBED,
  HS_CANCELLED,
  HS_MAX_HANDLES_EXCEEDED
}handle_status;


/* types of devices supported.
** Currently only one key pad device and one lcd device are supported
*/
typedef enum{
  HS_KPD_DEVICE=1,
  HS_LCD_DEVICE=2
}device_type;


/* Annunciator value types */
typedef word   lcd_ann_value_enum_type;


/* callback function declarations
**
** for subscribing to lcd services
** for registering for key pad events
** for obtaining lcd snapshots
*/
typedef void (kpd_cb_f_handle_type)   ( kpd_handle_type handle);
typedef void (lcd_cb_f_handle_type)   ( lcd_handle_type handle);
#ifdef FEATURE_KEYPAD_MULTI_KEY
typedef void (keypad_key_event_cb_f_type)( keypad_key_event_type key_event);
#else /* FEATURE_KEYPAD_MULTI_KEY */
#error code not present
#endif /* FEATURE_KEYPAD_MULTI_KEY */
typedef void (lcd_cb_f_snapshot_type) ( lcd_snapshot_type *snapshot);

typedef struct{
  hs_hdr_type            hdr;
  lcd_snapshot_type      *snapshot;         /* pointer to the  structure */
  lcd_cb_f_snapshot_type *lcd_snapshot_func;/* pointer to  callback function */
}hs_snapshot_packet_type;


typedef struct{
  hs_hdr_type              hdr;
#ifdef FEATURE_KEYPAD_MULTI_KEY
  keypad_key_event_cb_f_type *cb_ptr; /*key event callback function*/
#else /* FEATURE_KEYPAD_MULTI_KEY */
#error code not present
#endif /* FEATURE_KEYPAD_MULTI_KEY */
}hs_kpd_reg_key_packet_type;


typedef struct{
  hs_hdr_type              hdr;
  sub_command              cmd;
  lcd_handle_type          handle;
  lcd_cb_f_handle_type    *cb_ptr; /* pointer to callback function */
}hs_lcd_sub_srvc_packet_type;


typedef struct{
  hs_hdr_type              hdr;
  sub_command              cmd;
  kpd_handle_type          handle;
  kpd_cb_f_handle_type    *cb_ptr; /* pointer to callback function */
}hs_kpd_sub_srvc_packet_type;

#ifdef CUST_EDITION
typedef struct {
  hs_hdr_type               hdr;      /* Command header  */
  boolean                   onoff;    /* On/Off          */
} hs_vib_moto_ctrl_type;
#endif

typedef struct{
  hs_hdr_type              hdr;
  kpd_handle_type          handle;
#ifdef FEATURE_KEYPAD_MULTI_KEY
  keypad_key_event_cb_f_type *cb_ptr; /* pointer to callback function */
#else   /* FEATURE_KEYPAD_MULTI_KEY */
#error code not present
#endif  /* FEATURE_KEYPAD_MULTI_KEY */
}hs_kpd_event_packet_type;

#endif

/*---------------------------------------------------------------------------
  Command packet definition
---------------------------------------------------------------------------*/

/* Handset command packet which is passed to hs_cmd()
*/
typedef union {
  hs_hdr_type          hdr;     /* Just a command header                   */
  hs_ctl_packet_type   ctl;     /* LCD control, HS_SCRN_CTL, HS_BACKLIGHT  */
  hs_scrn_packet_type  scrn;    /* Screen, HS_SCRN_UPDATE, HS_UPDATE_BLINK */
  hs_scrna_packet_type scrna;   /* All screens, HS_ALL_SCRN                */
  hs_rssi_packet_type  rssi;    /* Set RSSI, HS_SCRN_RSSI                  */
  hs_ann_packet_type   ann;     /* Annunciator change, HS_ANNUN            */
  hs_anna_packet_type  anna;    /* Annunciator change all, HS_ALL_ANNUN    */
  hs_aud_packet_type   aud;     /* Set audio path, HS_AUD_IN, HS_AUD_OUT   */
  hs_key_packet_type   key;     /* Simulate-keypad-input, HS_SKI           */
  hs_blink_packet_type blink;   /* Screen blink, HS_BLINK_RATE             */
  hs_diag_packet_type  diag;    /* Diag state dump, HS_DIAG                */
  hs_lock_packet_type  lock;    /* Keypad lock, HS_DIAG_LOCK               */
  hs_dtmf_packet_type  dtmf;    /* DTMF detector control, HS_DTMF_DET      */
  hs_diag_packet_type  dscrn;   /* Diag screen load, HS_DIAG_SET_SCRN      */
  hs_lock_packet_type  scrlock; /* Screen lock, HS_DIAG_SCRN_LOCK          */
  hs_glyph_packet_type glyph;   /* glyph packet for HS_SET_GLYPH           */
  hs_batt_packet_type  batt;    /* Set Battery level, HS_SCRN_BATTERY      */
  hs_7seg_packet_type  sevseg;  /* Seven Segment, HS_SEVEN_SEG             */
#ifdef FEATURE_IDS
#error code not present
#endif
#if defined(FEATURE_PMIC_LCDKBD_LED_DRIVER)
  hs_ctl2_packet_type   ctl2;  /*LCD levels,HS_SET_LCD_LEVEL,HS_SET_KPD_LEVEL*/
#endif
#ifdef FEATURE_BSPUI_API
  hs_snapshot_packet_type     snpsht;   /* LCD snapshot, HS_SNAPSHOT       */
  hs_kpd_sub_srvc_packet_type kpd_srvc; /* KPD subscription , HS_KPD_SRVC  */
  hs_lcd_sub_srvc_packet_type lcd_srvc; /* LCD subscription , HS_LCD_SRVC  */
  hs_kpd_event_packet_type    kpd_evt;  /* KPD event subsc, , HS_KPD_EVENT */
#endif

#ifdef CUST_EDITION
    hs_vib_moto_ctrl_type vib_moto_ctrl;  /* Vibration moto control packet       */   
#endif

} hs_packets_type;

/* Queue of free command buffers.  These can be retrieved by calling
** q_get( &hs_cmd_free_q ).  Note:  *done_q_ptr is set to point to this
** queue so that on completion the commands will be placed back on this
** queue.  Do not alter *done_q_ptr.
*/
extern q_type hs_cmd_free_q;


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION HS_KEY_INIT

DESCRIPTION
  This procedure receives and remembers what task and signal to use when
  keycodes become available.

DEPENDENCIES
  hs_key_buffer

RETURN VALUE
  None

SIDE EFFECTS
  hs_key_rcvr_sig
  hs_key_rcvr_tcb_ptr

===========================================================================*/

extern void hs_key_init
(
  rex_tcb_type  *task_ptr,     /* Task to signal when keycodes arrive      */
  rex_sigs_type key_signal     /* Signal to send task when keycodes arrive */
);

/*===========================================================================

FUNCTION HS_KEY_GET

DESCRIPTION
  Read a keycode from the input buffer.

  NOTE: This routine is not called by the Handset task, but rather is called
  by "another task" to retrieve keys that Handset has collected on its
  behalf.  Note that this routine is NOT multi-thread safe, and is expected
  to be called by only ONE other task.

DEPENDENCIES
  None

RETURN VALUE
  This procedure returns the oldest unread keycode.  If no keycode
  is currently available, HS_NONE_K is returned.

SIDE EFFECTS
  None

===========================================================================*/
extern byte hs_key_get( void );

/*===========================================================================

FUNCTION HS_KEY_PUT

DESCRIPTION
  Write a keycode into the input buffer.  This is a regional routine.  Make
  sure the keys are added to the buffer in compliance with the hs_key_state
  variables.

  NOTE: depending on the configuration (Dial Shuttle and/or IDS) this routine
  can be called from contexts OTHER THAN Handset.

DEPENDENCIES
  hs_key_state

RETURN VALUE
  None

SIDE EFFECTS
  If there is no room for the key in hs_key_buffer, then an ERR() is logged
  and the key is thrown away.

===========================================================================*/
void hs_key_put
(
  hs_key_type new_key
    /* the Key to be put into the Key Buffer */
);

/*===========================================================================

FUNCTION HS_CMD

DESCRIPTION
  Queue a command for execution by the Handset Task.

DEPENDENCIES
  The queue will not be processed until the Handset Task has started.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hs_cmd
(
  hs_packets_type *cmd_ptr
    /* pointer to a handset command.  When the command has finished, the
    ** command structure is put on the "done queue".  If there is no "done
    ** queue", or this operation is not desirable, set the "done queue"
    ** pointer to NULL.  Also when the command has completed, the specified
    ** task is signaled with the sig parameter.  If no signaling is required,
    ** the task pointer parameter can be set to NULL.
    */
);

/*===========================================================================

FUNCTION HS_TASK

DESCRIPTION
  The Main Control Task calls this procedure via rex_def_task() to start up
  this function as the Handset Task.  This procedure calls hs_init() which
  initializes the handset and handles startup handshaking with the Main Control
  Task.

DEPENDENCIES
  This procedure must be called by rex_def_task.  The startup handshaking
  must be completed in order for the handset task to begin normal operations.

RETURN VALUE
  This procedure does not return.

SIDE EFFECTS
  The handset display will change as a result of handset task processing.

===========================================================================*/
void hs_task
(
  dword ignored
    /* parameter from REX -- ignored */
    /*lint -esym(715,ignored)
    ** Have lint not complain about the ignored parameter 'ignored' which is
    ** specified to make this routine match the template for rex_def_task().
    */
);

/*===========================================================================

FUNCTION HS_POLL_GPIO

DESCRIPTION
  This procedure polls the GPIOs which are keys, handling key event
  signals for new keycodes as needed.

  NOTE: this routine is NOT just called the Handset Task!  It is also
  called by other tasks in some cases!

DEPENDENCIES
  hs_ext_pwr
  hs_locked
  hs_on_hook

RETURN VALUE
  True if anything changed or if keys are waiting.
  False if no changes/keys occurred.

  Note: while not used by the Handset task, this return values IS USED when
    this routine is called by othoer tasks!

SIDE EFFECTS
  hs_ext_pwr
  hs_on_hook

===========================================================================*/
extern boolean hs_poll_gpio( void );

/*===========================================================================

FUNCTION hs_is_power_key_pressed

DESCRIPTION
  This function returns the status of the power key.

DEPENDENCIES
  If FEATURE_PMIC is defined then pm_init must have been called.

RETURN VALUE
  Boolean: TRUE,    power on key is pressed.
           FALSE    power on key is not pressed.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hs_is_power_key_pressed(void);

#if defined(T_MSM6550) && (defined(T_SURF) || defined(T_FFA6550_SS))
/*===========================================================================

FUNCTION hs_query_clamshell_state

DESCRIPTION
  This function returns the status of the clamshell state.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean: HS_FLIP_OPEN_K:    clamshell is open
           HS_FLIP_CLOSE_K:   clamshell is closed

SIDE EFFECTS
  None

===========================================================================*/
hs_key_type hs_query_clamshell_state(void);
#endif /* T_MSM6550 && ( T_SURF || T_FFA6550_SS ) */

#ifdef T_FFA6550_SS
#error code not present
#endif /* T_FFA6550_SS */


#endif /* HS_H_ */

