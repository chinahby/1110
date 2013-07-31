/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  M B 6 5 5 0   H A N D S E T   T A S K

GENERAL DESCRIPTION
  This task provides an interface between the handset and the other software
  tasks.  It processes input received from the handset keypad, and displays
  information on the handset display.

EXTERNALIZED FUNCTIONS
  hs_cmd
    This procedure enqueues an item onto the hs_cmd_q for the Handset
    Task to process.
  hs_poll_gpio
    Poll the gpio 'keys'.
  hs_task
    This is the handset task, started up with rex_def_task

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Hs_task() must be started up before commands queued by calling hs_cmd()
  will be processed.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  REX must be running, then start by calling rex_def_task() to start hs_task().

  Copyright (c) 2004-2006, 2008, 2009 QUALCOMM Incorporated. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/qsc1100/services/hs/hs_mb6550.c#6 $ $DateTime: 2009/05/12 00:36:19 $ $Author: goels $

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.


when       who      what, where, why
--------   ---      ---------------------------------------------------------
05/11/09   sg       Backout redundant event changes
02/12/09   ejh      Power savings when LCD backlight wakes up
11/20/08   rt       Remove redundant HSSD/HSD events
07/02/08   hs       Added support for FEATURE_CHG_TASK
03/28/08   gdp      FEATURE_NEW_SLEEP_API
09/12/06   adb      Force headset to be stereo on Pandora
09/12/05   ejv      Remove include of srch.h.
07/12/05   tb       Powerdown all FFA LCDs.
05/17/05   sj       Changes to support L4 target.
03/08/05   ty       Added support for display capture in HS
02/22/05   trc      Added orderly display powerdown for EFFA
09/30/04   trc      Added clamshell support for EFFA
08/31/04   jkl      Modified FPGA macros. Added STEREO HEADSET support.
08/25/04   trc      Added EFFA articulated flip support
07/26/04   ap       Added check for clamshell state for SURF builds.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "target.h"     /* Target specific definitions             */

#include "comdef.h"     /* Definitions for byte, word, etc.        */
#include "processor.h"  /* Processor architecture definitions      */
#include "rex.h"        /* REX services                            */
#include "customer.h"   /* Customer specific definitions           */
#include "hs.h"         /* Handset declarations                    */
#include "dog.h"        /* Watchdog signal and time definitions    */
#include "err.h"        /* Error macros                            */
#include "msg.h"        /* Message macros                          */
#include <memory.h>     /* Memory manipulation routines            */
#include "queue.h"      /* Queue declaration package               */
#include "task.h"       /* Global task declarations                */
#include "voc.h"        /* Vocoder prototypes and definitions      */
#include "db.h"         /* Phone Data Base Interface               */
#include "snd.h"        /* Sound Interface                         */
#include "keypad.h"
#include "hsdiag.h"
#ifdef FEATURE_LCD_TOUCH_ENABLE

extern void touchpad_init(void);
extern void touchpad_polling(int4 ms_interval);
#endif

#if defined(FEATURE_BATTERY_CHARGER) || defined(FEATURE_CHG_TASK) 
#include "charger.h"    /* Battery Charger                         */
#endif

#ifdef FEATURE_BITMAP
#include "chartoimg.h"  /* Functions used to convert character based
                           functionality to image format */
#ifdef FEATURE_HWTC
#error code not present
#else
#include "disp.h"
#endif /* FEATURE_HWTC */
#endif /* FEATURE_BITMAP */

#ifdef FEATURE_HEADSET_SWITCH
#include "adc.h"       /* Analog to digital converter definitions  */
#endif

#ifdef T_FFA6550_SS
#error code not present
#endif /* T_FFA6550_SS */

/*!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*

As switching between UI clients is only a temporary requirement this prototype
is not added to a header file

!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!*/
#ifdef FEATURE_IG_UI_CLIENT_SELECT
#include "hw.h"
#endif

#include "GlobalData.h"
/*!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*!!*/

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/* Set local message level
*/
word hs_msg_level = MSG_LVL_LOW;

#undef MSG_THRESHOLD
#define MSG_THRESHOLD   hs_msg_level

/*-----------------------------------------------------------------------
                         General Task Definitions
-----------------------------------------------------------------------*/

/* REX timers
*/
LOCAL rex_timer_type hs_rpt_timer;      /* Timer for the handset task      */
LOCAL rex_timer_type hs_gpio_timer;     /* Timer for gpio 'key' polling    */
#ifdef FEATURE_HS_DISPLAY_CAPTURE
LOCAL rex_timer_type hs_disp_cap_timer; /* Timer for display capture when
                                            doing limited polls */
#endif

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* Handset command queue
*/
LOCAL q_type hs_cmd_q;

/* Handset "free" command queue, holds unused/not-busy command
** structures for other tasks to use.
*/
q_type hs_cmd_free_q;

/* Buffers to link onto the "free" command queue.  The size is based on
** the consensus of the users of this queue.  UI wants to be able to use
** 7 or 8 "at once".
*/
#define                            HS_NUM_CMDS          8
LOCAL hs_packets_type hs_cmd_bufs[ HS_NUM_CMDS ];

/*-----------------------------------------------------------------------
                           Keypad Definitions
-----------------------------------------------------------------------*/

LOCAL rex_tcb_type   *hs_key_rcvr_tcb_ptr;/* Task receiving keypad signals   */
LOCAL rex_sigs_type   hs_key_rcvr_sig;    /* Signal to send to key_rcvr task */

/* Keep track if the Handset is currently locked so the user cannot use it.
** this functionality is used via the DM.
*/
LOCAL boolean hs_locked = FALSE;        /* The keypad starts out unlocked  */
#ifdef FEATURE_HEADSET
LOCAL boolean hs_headset_on   = FALSE;    /* Headset detection TRUE=yes      */
LOCAL boolean hs_headset_type = FALSE;    /* Headset type TRUE=stereo        */
#ifdef FEATURE_HEADSET_SWITCH
LOCAL boolean hs_headset_switch_on = FALSE; /* Headset switch on           */
#endif
#else
LOCAL boolean hs_ear_up     = FALSE;    /* Earpiece up                     */
#endif
#ifdef FEATURE_STEREO_DAC
LOCAL boolean hs_sdac_on = FALSE;       /* SDAC on                         */
#endif

#ifdef FEATURE_NEW_SLEEP_API
sleep_okts_handle hs_sleep_handle;
#endif

#ifdef T_FFA6550_SS
#error code not present
#endif /* T_FFA6550_SS */

/* Clock call-back structure used to register the keypad scanning routine
** to be called at regular intervals.
*/
LOCAL clk_cb_type hs_cb_keypad;

/* Clock call-back structure used to register the blinking call-back routine
 */
LOCAL clk_cb_type hs_cb_blinking;

#ifdef FEATURE_BITMAP
/* Determine whether screen is already initialized */
LOCAL boolean is_screen_initialized = FALSE;
#endif /* FEATURE_BITMAP */

#if defined(T_MSM6550) && defined(T_SURF)
LOCAL hs_key_type  clamshell_state = HS_RELEASE_K;
#endif
/*-----------------------------------------------------------------------
                    External Device Interface Data
-----------------------------------------------------------------------*/

/* Flags to indicate that handset initiated GPIO polling the very
 * first time.
 */
boolean hs_first_gpio_poll = FALSE;

/* Variable to hold the HS_FREE_K or individual device ID key
 */
byte hs_device_id_key = HS_FREE_K;

/* Flag to indicate if it is a coil cord carkit
 */
boolean hs_coil_cord_hfk = FALSE;

/*-----------------------------------------------------------------------
                                LCD data
-----------------------------------------------------------------------*/

/* Screen Data
*/
#ifndef FEATURE_BITMAP
LOCAL hs_scrn_type hs_lcd_screen;          /* LCD screen contents            */
#endif /* !FEATURE_BITMAP */
LOCAL hs_scrn_type hs_screen;              /* non-blink screen contents      */
LOCAL hs_scrn_type hs_blink_screen;        /* blink screen contents          */
                                           /* Current screen contents        */
LOCAL byte *hs_crnt_screen_ptr = hs_screen;

LOCAL word hs_annun;                       /* non-blink annunciators         */
LOCAL word hs_blink_annun;                 /* Blink annunciators             */

LOCAL byte hs_rssi;                        /* Non-blink RSSI level           */
LOCAL byte hs_blink_rssi;                  /* Blink RSSI level               */

LOCAL byte hs_batt;                        /* Non-blink Battery level        */
LOCAL byte hs_blink_batt;                  /* Blink Battery level            */

LOCAL byte *hs_crnt_rssi_ptr = &hs_rssi;   /* Current RSSI level             */
LOCAL byte *hs_crnt_batt_ptr = &hs_batt;   /* Current Battery level          */
LOCAL word *hs_crnt_annun_ptr = &hs_annun; /* Current annunciators           */

LOCAL word hs_blink_rate = 0;              /* Rate for blinking, ms / image  */
LOCAL word hs_blink_active = FALSE;        /* Is blink currently displayed   */

LOCAL boolean hs_backlight = FALSE;        /* Backlight is currently on      */

#if defined(FEATURE_PMIC_LCDKBD_LED_DRIVER)
LOCAL boolean hs_lcd_level = 2;            /* LCD Backlight intensity        */
LOCAL boolean hs_kpd_level = 1;            /* KPD Backlight intensity        */
#endif


/*-----------------------------------------------------------------------
                                BSPUI API data
-----------------------------------------------------------------------*/

#ifdef FEATURE_BSPUI_API
LOCAL lcd_handle_type  lcd_handle ={NULL}; /* Handle used for callback       */
#endif

#ifndef FEATURE_BITMAP
/* This array is used to derive a physical address from x,y coords
*/
LOCAL const word hs_lcd_row_address[ HS_LCD_MAX_LINES ] = {
  HS_LCD_ADDR_I( HS_LCD_LINE0_ADDR ),    /* address command for 1st line */
  HS_LCD_ADDR_I( HS_LCD_LINE1_ADDR ),    /* address command for 2nd line */
  HS_LCD_ADDR_I( HS_LCD_LINE2_ADDR ),    /* address command for 3rd line */
  HS_LCD_ADDR_I( HS_LCD_LINE3_ADDR )     /* address command for 4th line */
};

static byte const empty_glyph[ HS_GLYPH_HEIGHT ] =
  { 0, 0, 0, 0, 0, 0, 0 };             /* Empty (safe) glyph */
#endif /* !FEATURE_BITMAP */

#ifdef T_FFA6550_SS
#error code not present
#endif /* T_FFA6550_SS */

/*===========================================================================

                                 C O D E

===========================================================================*/

#ifdef T_FFA6550_SS
#error code not present
#endif /* T_FFA6550_SS */

/*-----------------------------------------------------------------------
                         LCD Controller Routines
-----------------------------------------------------------------------*/

#ifndef FEATURE_BITMAP
#ifdef FEATURE_THINUI
#define  BIO_LCD_REG 0
#define  BIO_LCD_A0_M 0
#define  BIO_LCD_A0_LO_V 0
#define  BIO_LCD_A0_HI_V 0
#define  BIO_LCD_SELECT_M 0
#define  BIO_LCD_SELECT_V 0
#define  BIO_LCD_DATA_M 0
#define  BIO_LCD_CLK_M 0
#define  BIO_LCD_DATA_HI_V 0
#define  BIO_LCD_CLK_LO_V 0
#define  BIO_LCD_DATA_LO_V 0
#define  BIO_LCD_CLK_HI_V 0
#define  BIO_LCD_DESELECT_V 0
#endif // FEATURE_THINUI
/*===========================================================================

LOCAL FUNCTION LCD_OUT

DESCRIPTION
  Output command to the LCD controller.

DEPENDENCIES
  Clock dependent delays which rely on a 13.5 MHz '186 in an MSM 2p.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hs_lcd_out
(
  word lcd_command
    /* 9-bit LCD command to output to the LCD controller */
)
{
  word mask;      /* Bit mask 0x80..0x01 */

  INTLOCK();

  /* Set A0 from the command, A0 is held in this state for
  ** the duration of the transfer.
  */
  if ( (lcd_command & HS_LCD_A0) != 0 ) {
    BIO_OUT( BIO_LCD_REG, BIO_LCD_A0_M, BIO_LCD_A0_HI_V );
  } else {
    BIO_OUT( BIO_LCD_REG, BIO_LCD_A0_M, BIO_LCD_A0_LO_V );
  }

  /* Select to the LCD controller, the device must be selected
  ** >150ns before the clock goes high.
  */
  BIO_OUT( BIO_LCD_REG, BIO_LCD_SELECT_M, BIO_LCD_SELECT_V );

  /* Clock out the data MSB to LSB.
  */
  for (mask = 0x80; mask != 0; mask >>= 1) {

    /* Set serial data to match this data bit
    ** The data must be valid for >50ns on either side of the clock's
    ** rising edge.  The clock cycle time must be >1000ns.
    */
    if ( (lcd_command & mask) != 0 ) {
      BIO_OUT( BIO_LCD_REG, BIO_LCD_DATA_M    | BIO_LCD_CLK_M,
                           BIO_LCD_DATA_HI_V | BIO_LCD_CLK_LO_V );
    } else {
      BIO_OUT( BIO_LCD_REG, BIO_LCD_DATA_M    | BIO_LCD_CLK_M,
                           BIO_LCD_DATA_LO_V | BIO_LCD_CLK_LO_V );
    }

    /* Latch the data by setting the clock high.
    ** The clock must be low for >300ns before going high, high for >300ns
    ** after going high.
    */
    BIO_OUT( BIO_LCD_REG, BIO_LCD_CLK_M, BIO_LCD_CLK_HI_V );
  }

  /* Put the serial bus back into a non-active low-power state
  ** Deselect the device, clock low, data low, A0 low
  */
  BIO_OUT( BIO_LCD_REG,
           BIO_LCD_SELECT_M   | BIO_LCD_DATA_M |
           BIO_LCD_CLK_M      | BIO_LCD_A0_M,
           BIO_LCD_DESELECT_V | BIO_LCD_DATA_LO_V |
           BIO_LCD_CLK_LO_V   | BIO_LCD_A0_LO_V );

  INTFREE();

} /* end of hs_lcd_out */

/*===========================================================================

LOCAL FUNCTION HS_LCD_ON

DESCRIPTION
  This function controls the on/off state of the display.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hs_lcd_on
(
  boolean on
    /* Whether the display is to be 'on' (True) or 'off' (False) */
)
{
  /* Update the current state
  */
  if (on) {
    hs_lcd_out( HS_LCD_DISPLAY_ON_I );
  } else {
    hs_lcd_out( HS_LCD_DISPLAY_OFF_I );
  }
} /* end of hs_lcd_on */

/*===========================================================================

FUNCTION HS_SET_GLYPH

DESCRIPTION
  Set the bitmap of a specified character.  The data format of a bitmap
  is a map of each row of the character, 5 bits wide (LSB justified) and
  7 rows tall.  For example:

    0x04, 0x0e, 0x1f, 0x03, 0x03, 0x18, 0x18 is

        ..X..
        .XXX.
        XXXXX
        ...XX
        ...XX
        XX...
        XX...

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  By changing the bitmap of the character, any characters of that type
  currently being displayed will change to the new character.

===========================================================================*/
LOCAL void hs_set_glyph
(
  byte index,           /* Index of the character to set the bitmap of */
  byte *bitmap_ptr      /* Pointer to 7 bytes defining the bitmap      */
)
{
  byte row_addr;

  /* Set the address to the CGRAM character whose bitmap we want to load.
  ** The 0th character is at HS_LCD_CGRAM_ADDR, and each successive character
  ** is HS_LCD_FONT_STEP farther down.
  */
  index = index % HS_GLYPH_CNT;
  hs_lcd_out(HS_LCD_ADDR_I(HS_LCD_CGRAM_ADDR + (index * HS_LCD_FONT_CSTEP)));

  /* For each row of the character, set the row contents
  */
  for (row_addr = 0; row_addr < HS_LCD_FONT_ROWS; row_addr++, bitmap_ptr++) {

    hs_lcd_out( HS_LCD_DATA_I( *bitmap_ptr ) );
  }

} /* end of hs_set_glyph */

/*===========================================================================

LOCAL FUNCTION HS_UPDATE_SCREEN

DESCRIPTION
  This procedure updates the screen, making the screen appear the
  same as new_screen.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Changes the display RAM address and contents.

===========================================================================*/
LOCAL void hs_update_screen( void )
{
  byte *new_ptr, *old_ptr, running;
  word i, j, addr_op;

  /* Avoid excessive updates, if no difference, don't update the screen
  */
#ifndef FEATURE_HWTC /* lcl for now */
  if (memcmp(hs_lcd_screen,hs_crnt_screen_ptr,sizeof(hs_lcd_screen)) == 0) {
    return;
  }
#endif /* FEATURE_HWTC */

  new_ptr = hs_crnt_screen_ptr; /* Pointer to new screen             */
  old_ptr = hs_lcd_screen;      /* Pointer to current screen         */

  /* For each row of the LCD
  */
  for (j = 0; j < HS_SCRN_HIGH; j++) {

    running = FALSE;            /* Not currently writing a substring */

    /* For each column of the LCD
    */
    for (i = 0; i < HS_SCRN_WIDE; i++, new_ptr++, old_ptr++) {

      /* If the character is different, write it.  Note: that substrings
      ** do not span rows, as the address space is discontinuous.
      */
#ifndef FEATURE_HWTC
      if (*new_ptr != *old_ptr) {
#endif /* FEATURE_HWTC */

        /* If not in the process of writing a substring, set the address
        */
        if ( ! running ) {
          running = TRUE;

          /* Index into the array to find the starting logical row address.
          ** Add the x coordinate to derive the physical address.
          */
          addr_op = hs_lcd_row_address[ j ] + i;

          /* Or the address with the instruction to build a command
          */
          hs_lcd_out( addr_op );

          /* wait unspecified duration 10 * Tcyc for command to complete
          */
          clk_busy_wait( 10 );
        }

        hs_lcd_out( HS_LCD_DATA_I( *new_ptr ) ); /* Output to the LCD       */

        /* Wait unspecified 10 * Tcyc for command to complete
        */
        clk_busy_wait( 10 );

        *old_ptr = *new_ptr;                     /* Update the screen image */

#ifndef FEATURE_HWTC
      } else {
        running = FALSE;
      }
#endif /* FEATURE_HWTC */
    }
  }

} /* end of hs_update_screen */

/*===========================================================================

LOCAL FUNCTION HS_UPDATE_ANNUN

DESCRIPTION
  This procedure updates the annunciators, both LED and LCD.

DEPENDENCIES
  Not called from an ISR

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void hs_update_annun( void )
{
  word lcd_annun;
  byte lcd_rssi;
  byte lcd_batt;

  /* Get the new values to update the display with
  */
  lcd_annun = *hs_crnt_annun_ptr;              /* Fetch new setting */
  lcd_rssi  = *hs_crnt_rssi_ptr;               /* Fetch RSSI        */
  lcd_batt  = *hs_crnt_batt_ptr;               /* Fetch Battery     */

  MSG_LOW("Annunciators 0x%x, rssi %d, batt %d", lcd_annun, lcd_rssi, lcd_batt);

  /* May want to optimize this later...
  **
  ** Set the address of the symbols, and write the current values
  */
  hs_lcd_out( HS_LCD_ADDR_I( HS_LCD_SYMB_ADDR ) );

  /* Digital
  */
  if ((lcd_annun & HS_AN_CDMA) != 0)
    hs_lcd_out( HS_LCD_DATA_I( 0x08 ) );
  else
    hs_lcd_out( HS_LCD_DATA_I( 0x00 ) );

  /* RSSI
  */
  if (lcd_rssi == 0)
    hs_lcd_out( HS_LCD_DATA_I( 0x00 ) );
  else if (lcd_rssi == 1)
    hs_lcd_out( HS_LCD_DATA_I( 0x08 ) );
  else
    hs_lcd_out( HS_LCD_DATA_I( 0x0A ) );

  if (lcd_rssi == 3)
    hs_lcd_out( HS_LCD_DATA_I( 0x08 ) );
  else if (lcd_rssi >= 4)
    hs_lcd_out( HS_LCD_DATA_I( 0x0A ) );
  else
    hs_lcd_out( HS_LCD_DATA_I( 0x00 ) );

  /* RSSI bar 5 and In-Use -or- No Service (phone)
  */
  if ((lcd_annun & (HS_AN_INUSE | HS_AN_NOSVC)) != 0) {
    if (lcd_rssi >= 5)
      hs_lcd_out( HS_LCD_DATA_I( 0x0A ) );
    else
      hs_lcd_out( HS_LCD_DATA_I( 0x02 ) );
  }
  else if (lcd_rssi >= 5)
    hs_lcd_out( HS_LCD_DATA_I( 0x08 ) );
  else
    hs_lcd_out( HS_LCD_DATA_I( 0x00 ) );

  /* No Service (slash)
  */
  if ((lcd_annun & HS_AN_NOSVC) != 0)
    hs_lcd_out( HS_LCD_DATA_I( 0x08 ) );
  else
    hs_lcd_out( HS_LCD_DATA_I( 0x00 ) );

  /* Roam
  */
  if ((lcd_annun & HS_AN_ROAM) != 0)
    hs_lcd_out( HS_LCD_DATA_I( 0x02 ) );
  else
    hs_lcd_out( HS_LCD_DATA_I( 0x00 ) );

  /* Message
  */
  if ((lcd_annun & HS_AN_MSG) != 0)
    hs_lcd_out( HS_LCD_DATA_I( 0x02 ) );
  else
    hs_lcd_out( HS_LCD_DATA_I( 0x00 ) );

  /* Space holder in the symbol memory address space
  */
  hs_lcd_out( HS_LCD_DATA_I( 0x00 ) );

  /* Voice Mail
  */
  if ((lcd_annun & HS_AN_VMAIL) != 0)
    hs_lcd_out( HS_LCD_DATA_I( 0x08 ) );
  else
    hs_lcd_out( HS_LCD_DATA_I( 0x00 ) );

  /* Battery level
  */
  switch (lcd_batt) {
    case 0:
      hs_lcd_out( HS_LCD_DATA_I( 0x00 ) );
      hs_lcd_out( HS_LCD_DATA_I( 0x00 ) );
      hs_lcd_out( HS_LCD_DATA_I( 0x00 ) );
      break;
    case 1:
      hs_lcd_out( HS_LCD_DATA_I( 0x02 ) );
      hs_lcd_out( HS_LCD_DATA_I( 0x00 ) );
      hs_lcd_out( HS_LCD_DATA_I( 0x00 ) );
      break;
    case 2:
      hs_lcd_out( HS_LCD_DATA_I( 0x02 ) );
      hs_lcd_out( HS_LCD_DATA_I( 0x00 ) );
      hs_lcd_out( HS_LCD_DATA_I( 0x08 ) );
      break;
    case 3:
      hs_lcd_out( HS_LCD_DATA_I( 0x02 ) );
      hs_lcd_out( HS_LCD_DATA_I( 0x02 ) );
      hs_lcd_out( HS_LCD_DATA_I( 0x08 ) );
      break;
    case 4:
      hs_lcd_out( HS_LCD_DATA_I( 0x02 ) );
      hs_lcd_out( HS_LCD_DATA_I( 0x02 ) );
      hs_lcd_out( HS_LCD_DATA_I( 0x0A ) );
      break;
    case 5:
      hs_lcd_out( HS_LCD_DATA_I( 0x02 ) );
      hs_lcd_out( HS_LCD_DATA_I( 0x0A ) );
      hs_lcd_out( HS_LCD_DATA_I( 0x0A ) );
      break;
    default:
      ERR_FATAL( "Illegal battery level (%d)", lcd_batt, 0, 0 );
  }

  /* Silent Notification Indicator
  */
  if ((lcd_annun & HS_AN_SNI) != 0) {
    HS_CALL_LED_ON();
  } else {
    HS_CALL_LED_OFF();
  }

} /* end of hs_update_annun */

/*===========================================================================

LOCAL FUNCTION HS_LCD_INIT

DESCRIPTION
  This function performs power up initialization on the HD44780 LCD
  controller. It sets up the controller in 8 bit mode, selects 2 line
  display mode. It also sets the LCD into a default mode and initializes
  the instruction states for all instructions which have bit fields which
  may vary. The default mode is to turn the display ON and set it up to
  increment the address counter when DDRAM data is written.  No other modes
  are enabled at this time.


DEPENDENCIES
  Must be called at least 15ms after power is applied to the LCD.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hs_lcd_init( void )
{
  int i;

  /* Init the LCD serial interface outputs
  ** Deselect the device, clock low, data low, A0 low
  */
   BIO_OUT( BIO_LCD_REG,
            BIO_LCD_SELECT_M   | BIO_LCD_DATA_M |
            BIO_LCD_CLK_M      | BIO_LCD_A0_M,
            BIO_LCD_DESELECT_V | BIO_LCD_DATA_LO_V |
            BIO_LCD_CLK_LO_V   | BIO_LCD_A0_LO_V );


   /* Enable the LCD serial interface outputs
   */
   BIO_TRISTATE( BIO_LCD_REG, HS_LCD_MASK, HS_LCD_MASK );

  /* System set (RAM Control) to enable DDRAM and CGRAM
  */
  hs_lcd_out( HS_LCD_RAM_CTL_I );

  /* Home the cursor
  */
  hs_lcd_out( HS_LCD_HOME_I );

  /* Enable the static display drivers
  */
  hs_lcd_out( HS_LCD_STATIC_I );

  /* Turn off power-save
  */
  hs_lcd_out( HS_LCD_POWERSAVE_OFF_I );

  /* Turn on the power supplies
  */
  hs_lcd_out( HS_LCD_POW_SUPPLY_I );

  /* Set the contrast
  */
  hs_lcd_out( HS_LCD_CONTRAST_I );

  /* Start with the display 'on' so when I fill the screen with HS_FILL_BOX
  ** the user can see the "lamp test" and get a warm fuzzy that the phone is
  ** powering on.
  */
  hs_lcd_on( TRUE );

  /* Load CGRAM with a 'safe' default
  */
  for (i = 0; i < HS_GLYPH_CNT; i++) {
    hs_set_glyph( i, (byte *) empty_glyph);
  }

  /* Initialize the screens.  The lcd_screen is 0's from being in BSS.
  ** Set the hs_screen and the blink screen to the lamp-test value.
  ** Setup the LCD from hs_screen by calling hs_update_screen().
  */
  (void) memset(hs_screen, HS_FILL_BOX, sizeof(hs_screen));
  (void) memset(hs_blink_screen, HS_FILL_BOX, sizeof(hs_blink_screen));
  hs_update_screen();

  /* Set the initial value of the annunciators
  */
  hs_update_annun();

} /* end of hs_lcd_init */
#endif /* FEATURE_BITMAP */

/*-----------------------------------------------------------------------
                External Device and GPIO Polling Routines
-----------------------------------------------------------------------*/

/*===========================================================================

FUNCTION HS_IGNITION_KEY

DESCRIPTION
  This routine checks the current status of the External Power and the
  Ones Detector to decide if we need to send a HS_IGN_ON_K or HS_IGN_OFF_K.
  This is an interim routine and will eventually be replaced by an actual
  status message from the Car Kit.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hs_ignition_key
(
  boolean if_car_kit,
    /* TRUE if a Car Kit has been detected at the ONES Detector */

  boolean if_ext_pwr
    /* TRUE if External Power is currently detected */
)
{
  static boolean last_ign_key = HS_IGN_OFF_K;
    /* Keep the value of the last Ignition Key we have sent here.  Default
    ** to HS_IGN_OFF_K which means we are waiting to send an ON as soon
    ** as both External Power and Ones are detected. */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If there is no External Power, then there is no possiblity that this
  ** is a Car Kit regardless of what the Ones Detector says.  In this case,
  ** set the "Last Key" to HS_IGN_OFF_K (just in case we had sent a
  ** HS_IGN_ON_K) and return.
  */
  if (!if_ext_pwr) {
     last_ign_key = HS_IGN_OFF_K;
     return;
  }

  /* If both Ones and External Power are present, then there must be a Car
  ** Kit and it must have ignition on.  Send the HS_IGN_ON_K if we haven't
  ** done so already.
  */
  if (if_car_kit) {
    if (last_ign_key == HS_IGN_OFF_K) {
      KEYPAD_PASS_KEY_CODE(HS_IGN_ON_K, HS_NONE_K);
      last_ign_key = HS_IGN_ON_K;
    }
    return;
  }

  /* If Ones has been lost, but External Power is still present, then this
  ** is a modified Car Kit that is about to power off because it lost
  ** ignition.  Inform the UI if we haven't done so already.
  */
  if (last_ign_key != HS_IGN_OFF_K) {
    MSG_MED("Sending IGN OFF key to UI", 0, 0, 0);
    KEYPAD_PASS_KEY_CODE(HS_IGN_OFF_K, HS_NONE_K);
    last_ign_key = HS_IGN_OFF_K;
  }

}
/*===========================================================================

FUNCTION HS_PASS_ACCESSSORY_KEYS

DESCRIPTION
  This routine decides what keys to pass to UI based on the status of
  attached accessory.

DEPENDENCIES
  hs_first_gpio_poll

RETURN VALUE
  None

SIDE EFFECTS
  hs_first_gpio_poll

===========================================================================*/
LOCAL void hs_pass_accessory_keys
(
  boolean ext_dev,       /* TRUE if an external device has     */
                         /* been detected at the ONES Detector */

  boolean ext_pwr        /* TRUE if External Power is          */
                         /* currently detected                 */
)
{
  /* Remember the status of last time
   */
  static boolean last_ext_dev = FALSE;
  static boolean last_ext_pwr = FALSE;

  /* Pass accessory keys for initial power up
   */
  if ( hs_first_gpio_poll )
  {
    /* Pass device key first if it is initial power up
     */
    if ( ext_dev )
    {
      KEYPAD_PASS_KEY_CODE( hs_device_id_key, HS_NONE_K );
      KEYPAD_PASS_KEY_CODE( HS_RELEASE_K, hs_device_id_key );
    }
    else
    {
      KEYPAD_PASS_KEY_CODE( HS_PHONE_K, HS_NONE_K );
      KEYPAD_PASS_KEY_CODE( HS_RELEASE_K, HS_PHONE_K );
    }

    /* Pass external power key
     */
    if ( ext_pwr )
    {
      KEYPAD_PASS_KEY_CODE( HS_EXT_PWR_ON_K, HS_NONE_K );
      KEYPAD_PASS_KEY_CODE( HS_RELEASE_K, HS_EXT_PWR_ON_K );
    }

    /* Update the status flags
     */
    last_ext_pwr = ext_pwr;
    last_ext_dev = ext_dev;
    hs_first_gpio_poll = FALSE;

    return;
  }

  /* Decide whether External Power key
   */
  if ( ext_pwr != last_ext_pwr )
  {
    KEYPAD_PASS_KEY_CODE( (ext_pwr ? HS_EXT_PWR_ON_K : HS_EXT_PWR_OFF_K), HS_NONE_K );

    last_ext_pwr = ext_pwr;
  }

  /* Decide whether to pass HS_FREE_K / Device ID key
   */
  if ( ext_dev != last_ext_dev && ext_dev )
  {
    KEYPAD_PASS_KEY_CODE( hs_device_id_key, HS_NONE_K );
    last_ext_dev = ext_dev;
  }

  /* This routine tests for the need to send a "fake" ignition key to the
  ** UI.  Note that it is NOT enclosed in a ONES detect change if().  This
  ** is because the state of BOTH External Power and Ones detect figure
  ** into the decision.  Also note that we don't worry about state_changed
  ** here because this routine only sends a key if SOMETHING ELSE has
  ** also changed and thus state_changed will already be TRUE.
  */
  if ( !hs_coil_cord_hfk )
    hs_ignition_key( ext_dev, ext_pwr );

  /* Pass Phone Key if no external device is detected
   */
  if ( ext_dev != last_ext_dev )
  {
    KEYPAD_PASS_KEY_CODE( HS_PHONE_K, HS_NONE_K );
    last_ext_dev = ext_dev;
  }
}

#if defined(T_MSM6550) && ( defined(T_SURF) || defined(T_FFA6550_SS) )
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
hs_key_type hs_query_clamshell_state(void)
{
  #ifdef T_FFA6550_SS
#error code not present
  #else /* ! T_FFA6550_SS */
  return (FPGA_INM(FPGA_GPIO_IN, FLIP_OPEN)) ?
            HS_FLIP_OPEN_K : HS_FLIP_CLOSE_K;
  #endif /* ! T_FFA6550_SS */
}
#endif /* T_MSM6550 && ( T_SURF || T_FFA6550_SS ) */

#ifdef T_FFA6550_SS
#error code not present
#endif /* T_FFA6550_SS */

/*===========================================================================

FUNCTION HS_POLL_GPIO

DESCRIPTION
  This procedure polls the GPIOs which are keys, handling key event
  signals for new keycodes as needed.

  NOTE: this routine is NOT just called by the Handset Task!  It is also
  called by other tasks in some cases!

DEPENDENCIES
  hs_locked
  hs_accessory_state
  hs_first_gpio_poll

RETURN VALUE
  True if anything changed or if keys are waiting.
  False if no changes/keys occurred.

  Note: while not used by the Handset task, this return values IS USED when
    this routine is called by othoer tasks!

SIDE EFFECTS
  hs_accessory_state

===========================================================================*/
boolean hs_poll_gpio( void )
{
  /* Status of last poll
   */
  static boolean last_ext_pwr = FALSE;      /* external power status    */
  static boolean last_ext_dev = FALSE;      /* external device status   */
  static boolean last_ONES_status = FALSE;  /* ONES detector status     */

  /* Status of this poll
   */
  boolean ext_pwr = FALSE;                  /* external power status    */
  boolean ext_dev = FALSE;                  /* external device status   */
#ifdef FEATURE_HEADSET
  boolean headset_on = FALSE;
  boolean headset_type = FALSE;
  boolean headset_state_changed = FALSE;
  byte    headset_key = HS_HEADSET_OFF_K;
#ifdef FEATURE_HEADSET_SWITCH
  boolean headset_switch_on = FALSE;
  ADC_RETURN_TYPE headset_sw_adc_result = 0;
  byte headset_switch_key;
#endif /* FEATURE_HEADSET_SWITCH */
#else
  boolean ear_up = FALSE;
  byte    ear_key;
#endif /* FEATURE_HEADSET */

#ifdef FEATURE_STEREO_DAC
  boolean sdac_on = FALSE;
  byte    sdac_key;
#endif
  boolean curr_ONES_status;                 /* ONES detector status     */
  boolean ONES_consistent = FALSE;          /* ONES status is consitent */

  /* Keep track of this routine's return value in here
   */
  boolean state_changed;

  /* To remember we need to send handset keys when everything is OK.
   */
  static boolean accessory_key_required = FALSE;

  static boolean first_ONES_poll = TRUE;    /* Indicates the first poll */

#if defined(T_MSM6550) && defined(T_SURF)
  hs_key_type new_clamshell_state     = HS_RELEASE_K;
#endif /* T_MSM6550 && T_SURF */

	//MSG_FATAL("hs_poll_gpio....................",0,0,0);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Assume nothing interesting will happen as the default return value.
   */
  state_changed = FALSE;

  /* Even though these are not "keys" on the keypad, they are treated as
  ** key events to the UI.  If the handset is locked then don't look at
  ** these keys here.  (Use DM keys ONLY in that case.)
  */
  if ( ! hs_locked )
  {
    /* Read the status of External Power.
    */
    ext_pwr = HS_EXT_PWR_ON();

    if ( ext_pwr != last_ext_pwr )
    {
      MSG_HIGH("External power is %d", ext_pwr, 0, 0);

      last_ext_pwr = ext_pwr;         /* Update status                   */
      accessory_key_required = TRUE;  /* Need to send external power key */
      HS_SLEEP_ALLOW();               /* Allow sleep                     */
      state_changed = TRUE;
    }
#ifdef FEATURE_HEADSET
    /* Check to see if the headset state has changed
    */
    headset_on = HS_HEADSET_ON();
	
    if ( headset_on != hs_headset_on ) {
      headset_state_changed = TRUE;
    }

    if (headset_on) {
      /* Determine the headset type */
      headset_type = HS_HEADSET_TYPE();
      headset_key = (headset_type)? HS_STEREO_HEADSET_K : HS_HEADSET_K;
      if (headset_type != hs_headset_type) {
        headset_state_changed = TRUE;
      }
    }

    if (headset_state_changed == TRUE) {
      hs_headset_on = headset_on;
      hs_headset_type = headset_type;

      /* Pass key to show that the state changed, on/off */

      KEYPAD_PASS_KEY_CODE( headset_key, HS_NONE_K );
      MSG_HIGH("Headset is %d", headset_on, 0, 0);

      KEYPAD_PASS_KEY_CODE( HS_RELEASE_K, headset_key );

      HS_SLEEP_ALLOW();               /* No keys waited on, sleep now      */
      state_changed = TRUE;
    }

#ifdef FEATURE_HEADSET_SWITCH
    if(headset_on == TRUE) {

      /* Get the switch state using the HKADC */
      headset_sw_adc_result = adc_read(ADC_HEADSET_SWITCH);
      headset_switch_on = (headset_sw_adc_result >= 100)? TRUE: FALSE;

      if ( headset_switch_on != hs_headset_switch_on ) {
        hs_headset_switch_on = headset_switch_on;

        /* Select headset switch on or off key
        */
        headset_switch_key = (byte) ((headset_switch_on) ? HS_HEADSET_SWITCH_K:
                                                       HS_HEADSET_SWITCH_OFF_K );

        /* Pass key to show that the state changed, on/off */
        KEYPAD_PASS_KEY_CODE( headset_switch_key, HS_NONE_K );
        MSG_HIGH("Headset switch is %d", (int)headset_switch_on, 0, 0);

        if ( hs_headset_switch_on == FALSE ) {
          KEYPAD_PASS_KEY_CODE( HS_RELEASE_K, headset_switch_key );
        }

        state_changed = TRUE;
      }
    }
    else {
      /* Ensure that we send a RELEASE if the headset was unplugged while the
       * switch was depressed (ON). */
      if ( hs_headset_switch_on == TRUE )
      {
        hs_headset_switch_on = FALSE;
        KEYPAD_PASS_KEY_CODE( HS_HEADSET_SWITCH_OFF_K, HS_NONE_K );
        KEYPAD_PASS_KEY_CODE( HS_RELEASE_K, HS_HEADSET_SWITCH_OFF_K );
        state_changed = TRUE;
      }
    }
#endif /* FEATURE_HEADSET_SWITCH */

#else  /* FEATURE_HEADSET */
    /* Check to see if the earpiece switch has changed
    */
    ear_up = HS_EAR_UP();
    if ( ear_up != hs_ear_up ) {

      hs_ear_up = ear_up;     /* Invert the state */

      /* Select ear up or down key
      */
      ear_key = (byte) ((ear_up) ? HS_EAR_UP_K : HS_EAR_DOWN_K);

      /* Pass key to show that the state changed, on/off
      */
      KEYPAD_PASS_KEY_CODE( ear_key, HS_NONE_K );
      MSG_HIGH("Earpiece switch is %d", ear_up, 0, 0);
      KEYPAD_PASS_KEY_CODE( HS_RELEASE_K, ear_key );

      HS_SLEEP_ALLOW();               /* No keys waited on, sleep now      */
      state_changed = TRUE;
    }
#endif /* FEATURE_HEADSET */

#ifdef FEATURE_STEREO_DAC
    /* Check to see if the SDAC state has changed
    */

    sdac_on = HS_SDAC_ON();

    if ( sdac_on != hs_sdac_on ) {

      hs_sdac_on = sdac_on;     /* Invert the state */

      /* Select headset on or off key
      */
      sdac_key = (byte) ((sdac_on) ? HS_SDAC_ON_K : HS_SDAC_OFF_K);

      /* Pass key to show that the state changed, on/off
      */
      KEYPAD_PASS_KEY_CODE( sdac_key, HS_NONE_K );
      MSG_HIGH("SDAC is %d", sdac_on, 0, 0);

      KEYPAD_PASS_KEY_CODE( HS_RELEASE_K, sdac_key );

      HS_SLEEP_ALLOW();               /* No keys waited on, sleep now      */
      state_changed = TRUE;
    }
#endif  /* FEATURE_STEREO_DAC */

    /* Sample the state of the on-hook/hands-free sensor over the last
    ** sampling period, HS_GPIO_POLL_TIME.  This sensor is looking at the
    ** PCM data in line, seeing if there are any 0's.  Since there is a
    ** pullup resistor on the PCM data line, there will only be 1's if
    ** there is no Car Kit.  The latch is reset by Setting "Zero Detect"
    ** again.
    */

    /* Make sure ONES detector status is valid to read.
     * Also, read the ONES detector twice to determine real status
     * in case it has just been reset by other task.
     */
    if (rex_self() == &hs_tcb)
    {
      /* Handles power up condition
       */
      if ( first_ONES_poll )
      {
        first_ONES_poll = FALSE;
        hs_first_gpio_poll = TRUE;
      }

      curr_ONES_status = voc_read_ones_detect();

      voc_reset_ones_detect();

      /* Update the external device status only if we have two consistent
       * ONES detector readings in a row.
       */
      if (curr_ONES_status != last_ONES_status)
      {
        last_ONES_status = curr_ONES_status;
        ext_dev = last_ext_dev;
      }
      else
      {
        ext_dev = curr_ONES_status;
        ONES_consistent = TRUE;
      }
    }
    else
      ext_dev = last_ext_dev;

    /* See if external device status changed
     */
    if ( ext_dev != last_ext_dev )
    {
      hs_coil_cord_hfk = FALSE;       /* Presume not a coil cord HFK */
      last_ext_dev = ext_dev;         /* Update the status           */
      accessory_key_required = TRUE;  /* Need to send accessory key  */
      HS_SLEEP_ALLOW();               /* Allow sleep                 */
      state_changed = TRUE;
    }

    if ( (hs_first_gpio_poll || accessory_key_required) && ONES_consistent )
    {
       hs_pass_accessory_keys( ext_dev, ext_pwr );
       accessory_key_required = FALSE;
    }

#if defined(FEATURE_BATTERY_CHARGER) || defined(FEATURE_CHG_TASK) 
    /* Check to see if battery charging is active
    */
    {
    static byte    loop_cnt=0;
    static boolean last_charging_state=FALSE;

    /* Only check every 5secs or so */
    if ((++loop_cnt % HS_POLL_CHG_LOOP) == 0)
    {
      if (ext_pwr || last_charging_state==TRUE)
      {
         boolean charging_state;

         charging_state = chg_is_charging();

         if (charging_state != last_charging_state)
         {
            last_charging_state = charging_state;
            if (charging_state == TRUE)
            {
               KEYPAD_PASS_KEY_CODE(  HS_CHARGING_ON_K, HS_NONE_K );
            }
            else
            {
               KEYPAD_PASS_KEY_CODE(  HS_CHARGING_OFF_K, HS_NONE_K );
            }
            HS_SLEEP_ALLOW();               /* Allow sleep                 */
         }
      }
    }

    }
#endif /* FEATURE_BATTERY_CHARGER || FEATURE_CHG_TASK */


  }
#ifdef FEATRUE_DEVICETYPE_FLIP
#if defined(T_MSM6550) && defined(T_SURF)
  /* Check the state of the clamshell (open/closed)
  */
  if ((new_clamshell_state = hs_query_clamshell_state()) != clamshell_state)
  {
    /* state changed, notify clients */
    clamshell_state = new_clamshell_state;
    KEYPAD_PASS_KEY_CODE( new_clamshell_state, HS_NONE_K );
  }
#endif /* T_MSM6550 && T_SURF */
#endif
#ifdef T_FFA6550_SS
#error code not present
#endif /* T_FFA6550_SS */

  return( state_changed );

} /* End of hs_poll_gpio */

/*-----------------------------------------------------------------------
                        General Task Routines
-----------------------------------------------------------------------*/
/*===========================================================================

FUNCTION HS_BLINKING_CALLBACK

DESCRIPTION
  This is a call-back routine. Once called, it sets the HS_BLINK_TIMER_SIG
  signal for Handset to blink the screen.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hs_blinking_callback
(
  int4 ms_interval_unused
    /* parameter from Clock services -- ignored */
    /*lint -esym(715,ms_interval_unused)
    ** Have lint not complain about the ignored parameter which is
    ** specified to make this routine match the template for Clock.
    */
)
{
  (void) rex_set_sigs( &hs_tcb, HS_BLINK_TIMER_SIG);    /* Set the signal */
}

/*===========================================================================

LOCAL FUNCTION HS_REQ

DESCRIPTION
  This procedure processes commands sitting in the command queue.

DEPENDENCIES
  hs_init() must have been called to initialize the queue, and this
  routine should not be called from an ISR.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hs_req( void )
{

  hs_packets_type *cmd_ptr;     /* Pointer to the current command     */
  rex_tcb_type *task_ptr;       /* Pointer to task to signal          */
  rex_sigs_type task_sig;       /* Signal for task                    */

  /* Clear the signal prior to emptying the queue, to avoid the
  ** race condition where an item is added just as we are leaving.
  */
  (void) rex_clr_sigs( &hs_tcb, HS_CMD_Q_SIG );

  /* Get a command from the command queue
  */
  while ( (cmd_ptr = (hs_packets_type *) q_get( &hs_cmd_q )) != NULL ) {

    task_ptr = cmd_ptr->hdr.task_ptr;   /* task pointer    */
    task_sig = cmd_ptr->hdr.sigs;       /* signal for task */

#ifdef FEATURE_IG_UI_CLIENT_SELECT
    if (!hw_ig_ui_client_is_wombat())
       /* Wombat (Linux) is running the keypad and display */
#endif
    /* Decode/process the command as per the operation requested
    */
    switch ( cmd_ptr->hdr.cmd ) {

      case HS_ACTIVE:       /* Setup for CDMA or Analog calls         */
        ERR("HS_ACTIVE command not supported",0,0,0);
        break;

      case HS_SLEEP:        /* Setup for sleep mode                   */
        ERR("HS_SLEEP command not supported",0,0,0);
        break;

      case HS_SCRN_CTL:     /* Turn on/off LCD screen                 */
#ifdef FEATURE_BITMAP
        if (cmd_ptr->ctl.on_off)
          disp_powerup();
        else
          disp_powerdown();
#else /* FEATURE_BITMAP */
        hs_lcd_on( cmd_ptr->ctl.on_off );
#endif /* FEATURE_BITMAP */
        break;

      case HS_BACKLIGHT:    /* Turn on/off screen & keypad backlights */
        hs_backlight = cmd_ptr->ctl.on_off;

        MSG_LOW("Backlight set to %d", hs_backlight, 0, 0);
        if ( hs_backlight ) {
          HS_BACKLIGHT_ON();    /* Turn on the backlight  */
        } else {
          HS_BACKLIGHT_OFF();   /* Turn off the backlight */
        }
        break;

      case HS_SCRN_UPDATE:  /* Update the screen image                */
        (void) memcpy(hs_screen, *cmd_ptr->scrn.scrn_ptr, sizeof(hs_screen));

        /* If the non-blink screen is displayed, refresh it
        */
        if (! hs_blink_active) {
#ifdef FEATURE_BITMAP
          /* Update the screen */
          chartoimg_update_screen(hs_crnt_screen_ptr);
#else /* FEATURE_BITMAP */
          hs_update_screen();
#endif /* FEATURE_BITMAP */
        }
        break;

      case HS_ANNUN:        /* Turn on/off the annunciators           */
        hs_annun = (hs_annun & ~(cmd_ptr->ann.annun.mask)) |
                   cmd_ptr->ann.annun.value;
#ifdef FEATURE_BITMAP
        chartoimg_update_annunciators(hs_crnt_annun_ptr, hs_crnt_rssi_ptr,
                                           hs_crnt_batt_ptr);
#else /* FEATURE_BITMAP */
        hs_update_annun();
#endif /* FEATURE_BITMAP */
        break;

      case HS_BLINK_ANNUN:  /* Set which annunciators blink            */
        hs_blink_annun = (hs_blink_annun & ~(cmd_ptr->ann.annun.mask)) |
                         cmd_ptr->ann.annun.value;
#ifdef FEATURE_BITMAP
        /* Update the annunciator */
        chartoimg_update_annunciators(hs_crnt_annun_ptr, hs_crnt_rssi_ptr,
                                     hs_crnt_batt_ptr);
#else /* FEATURE_BITMAP */
        hs_update_annun();
#endif /* FEATURE_BITMAP */
        break;

      case HS_SCRN_RSSI:    /* Set the level of the RSSI bar          */
        hs_crnt_rssi_ptr = &hs_rssi;
        hs_rssi = cmd_ptr->rssi.rssi;
        hs_blink_rssi = cmd_ptr->rssi.blink_rssi;
#ifdef FEATURE_BITMAP
           chartoimg_update_annunciators(hs_crnt_annun_ptr, hs_crnt_rssi_ptr,
                                         hs_crnt_batt_ptr);
#else /* FEATURE_BITMAP */
        hs_update_annun();
#endif /* FEATURE_BITMAP */
        break;

      case HS_AUD_IN:       /* Set the audio input path               */
        ERR("No HS_AUD_IN in gemini",0,0,0);
        break;

      case HS_AUD_OUT:      /* Set the audio output path              */
        ERR("No HS_AUD_OUT in gemini",0,0,0);
        break;

      case HS_SKI:          /* Simulate keypad input                  */
        KEYPAD_PASS_KEY_CODE( cmd_ptr->key.keycode, cmd_ptr->key.keyparm );
        break;

      case HS_UPDATE_BLINK: /* Update the screen used for blinking    */
        (void) memcpy(hs_blink_screen, *cmd_ptr->scrn.scrn_ptr,
                                                    sizeof(hs_blink_screen));

        /* If the blink screen is displayed, refresh it
        */
        if (hs_blink_active) {
#ifdef FEATURE_BITMAP
          chartoimg_update_screen(hs_crnt_screen_ptr);
#else /* FEATURE_BITMAP */
          hs_update_screen();
#endif /* FEATURE_BITMAP */
        }
        break;

      case HS_BLINK_RATE:   /* Set the blink rate                     */
        hs_blink_rate = cmd_ptr->blink.blink_rate;

        /* If the rate is 0 milliseconds, blinking has stopped,
        ** redisplay the non-blink screen/annunciators
        */
        if (hs_blink_rate == 0) {

          /* De-register the call-back and clear the blink timer signal
          */
          clk_dereg( &hs_cb_blinking );
          (void) rex_clr_sigs( &hs_tcb, HS_BLINK_TIMER_SIG );

          /* Reset the outputs to point at 'regular' screen/annunciators
          */
          hs_crnt_screen_ptr = hs_screen;               /* Current screen  */
          hs_crnt_annun_ptr = &hs_annun;                /* Current annun.  */
          hs_crnt_rssi_ptr = &hs_rssi;                  /* Current RSSI    */
          hs_crnt_batt_ptr = &hs_batt;                  /* Current Battery */

          /* Update the view if it's not the non-blink version
          */
          if (hs_blink_active) {
#ifdef FEATURE_BITMAP
            chartoimg_update_screen(hs_crnt_screen_ptr);
            chartoimg_update_annunciators(hs_crnt_annun_ptr, hs_crnt_rssi_ptr,
                                         hs_crnt_batt_ptr);
#else /* FEATURE_BITMAP */
            hs_update_screen();         /* Redisplay non-blink screen    */
            hs_update_annun();          /* Redisplay annunciators        */
#endif /* FEATURE_BITMAP */
          }

          hs_blink_active = FALSE;      /* Not displaying 'blink' output */

        } else {                        /* Starting blinking             */
          /* De-register any old call-back and register call-back
           * with new timer parameters.
           */
          clk_dereg( &hs_cb_blinking );
          clk_reg( &hs_cb_blinking, hs_blinking_callback,
            (long)hs_blink_rate, (long)hs_blink_rate, TRUE );
        }
        break;

      case HS_DIAG:         /* Diag state dump                        */

        /* Buffer of information describing the handset's display state.
        */
        {
          hs_diag_type *diag_ptr;       /* Handset state snapshot         */

          diag_ptr = cmd_ptr->diag.diag_ptr;
          diag_ptr->backlight_on = hs_backlight;/* Backlight state         */
                                                /* Screen                  */
          (void) memcpy((void *)diag_ptr->scrn, hs_screen, sizeof(hs_screen));
          diag_ptr->annun = hs_annun;           /* Annunciators            */
                                                /* Screen for blinking     */
          (void) memcpy((void *)diag_ptr->blink_scrn,
                        hs_blink_screen,sizeof(hs_blink_screen));
          diag_ptr->blink_annun = hs_blink_annun;/* Annunciators for blink */
          diag_ptr->rssi = hs_rssi;             /* No RSSI indicator       */
          diag_ptr->blink_rate = hs_blink_rate; /* Blink rate for blinking */
        }
        break;

      case HS_DIAG_LOCK:    /* Keypad lock                            */
        keypad_lock_keypad(cmd_ptr->lock.lock);
        hs_locked = (cmd_ptr->lock.lock);
        break;

      case HS_DTMF_DET:     /* DTMF detector control                  */
        ERR("No HS_DTMF_DET in gemini",0,0,0);
        break;

      case HS_ALL_ANNUN:    /* Set both regular and blinking annunciators  */

        hs_annun = (hs_annun &
          ~(cmd_ptr->anna.annun.mask)) | cmd_ptr->anna.annun.value;

        hs_blink_annun = (hs_blink_annun &
          ~(cmd_ptr->anna.blink_annun.mask)) | cmd_ptr->anna.blink_annun.value;

#ifdef FEATURE_BITMAP
        /* Update the annunciator */
        chartoimg_update_annunciators(hs_crnt_annun_ptr, hs_crnt_rssi_ptr,
                                     hs_crnt_batt_ptr);
#else /* FEATURE_BITMAP */
        hs_update_annun();      /* Update the screen */
#endif /* FEATURE_BITMAP */
        break;

      case HS_ALL_SCRN:     /* Update both the blink and regular screen    */
        (void) memcpy(hs_screen, *cmd_ptr->scrna.scrn_ptr, sizeof(hs_screen));
        (void) memcpy(hs_blink_screen, *cmd_ptr->scrna.blink_scrn_ptr,
                                       sizeof(hs_blink_screen));
#ifdef FEATURE_BITMAP
        /* Update the screen */
        chartoimg_update_screen(hs_crnt_screen_ptr);
#else /* FEATURE_BITMAP */
        hs_update_screen();     /* Update the display */
#endif /* FEATURE_BITMAP */
        break;

      case HS_SET_GLYPH:        /* Set the bitmap of a glyph */
#ifdef FEATURE_BITMAP
        /* This is called at power by character-based UI to set the
           screen to these black rectangles, instead in the bitmap
           version we will just clear the screen to one color
           Since depending on how UI did things, this function might
           be called more than once, but we only clear the screen once
         */
        if (!is_screen_initialized)
        {
          chartoimg_init();
          is_screen_initialized = TRUE;
        }
#else /* FEATURE_BITMAP */
        hs_set_glyph( cmd_ptr->glyph.index, cmd_ptr->glyph.bitmap );
#endif /* FEATURE_BITMAP */
        break;

      case HS_SCRN_BATTERY:     /* set the battery level display */
        hs_crnt_batt_ptr = &hs_batt;
        hs_batt = cmd_ptr->batt.batt;
        hs_blink_batt = cmd_ptr->batt.blink_batt;
#ifdef FEATURE_BITMAP
        chartoimg_update_annunciators(hs_crnt_annun_ptr, hs_crnt_rssi_ptr,
                                      hs_crnt_batt_ptr);
#else /* FEATURE_BITMAP */
        hs_update_annun();
#endif /* FEATURE_BITMAP */
        break;

#if defined(FEATURE_PMIC_LCDKBD_LED_DRIVER)
    case HS_SET_LCD_LEVEL:     /* LCD intensity level */
        hs_lcd_level = cmd_ptr->ctl2.level;
        {
          /* Change the intensity now*/
//hvo          pm_set_led_intensity(PM_LCD_LED, hs_lcd_level);
        }
        break;

    case HS_SET_KPD_LEVEL:     /* KPD intensity level */
        hs_kpd_level = cmd_ptr->ctl2.level;
        {
          /* Change the intensity now*/
          pm_set_led_intensity(PM_KBD_LED, hs_kpd_level);
        }
        break;
#endif

#ifdef FEATURE_BSPUI_API

    case HS_KPD_SRVC:         /* Register for KPD services */
        ERR("HS_KPD_SRVC command not supported",0,0,0);
        break;

    case HS_LCD_SRVC:         /* Register for LCD services */
        /* At this time there is only support for one device */
        if (lcd_handle.id == NULL) {
           lcd_handle.id = HS_LCD_DEVICE;
           lcd_handle.status = HS_SUBSCRIBED;
        }
        else {
           lcd_handle.id = NULL;
           lcd_handle.status = HS_MAX_HANDLES_EXCEEDED;
        }
        /*
        cmd_ptr->lcd_srvc.cb_ptr(lcd_handle);
        */

        break;

    case HS_KPD_KEY_REG:      /* Register for KPD events */
        keypad_register(cmd_ptr->kpd_evt.cb_ptr);
        break;

#endif /* FEATURE_BSPUI_API */
#ifdef CUST_EDITION
    case HS_SET_VIB_MOTO_ONOFF:
        if(cmd_ptr->vib_moto_ctrl.onoff)
        {
            pm_vib_mot_set_volt(3100);
        }
        else
        {
            pm_vib_mot_set_volt(0);
        }
        break;
#endif
      default:
        ERR_FATAL("Invalid command: %d", cmd_ptr->hdr.cmd, 0, 0 );
        /* does not return */
    }

    /* Save the tcb/sig of the commander, so that we can signal that task
    ** after we've freed the command structure.  If there is a 'done queue',
    ** queue the command into that queue.
    */
    if (cmd_ptr->hdr.done_q_ptr) {
      q_put( cmd_ptr->hdr.done_q_ptr, &(cmd_ptr->hdr.link) );
    }

    /* If there's a task to signal, signal it with the saved tcb/sig
    */
    if (task_ptr) {
      (void) rex_set_sigs( task_ptr, task_sig );
    }

    /* Report to the watchdog, we may be in this loop a long time
    */
    dog_report(DOG_HS_RPT);
  }


} /* end of hs_req */

/*===========================================================================

LOCAL FUNCTION HS_INIT

DESCRIPTION
  This procedure initializes the handset and handles startup handshaking
  with the Main Control Task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Initialization causes the display to be cleared and the backlight turned on

===========================================================================*/
LOCAL void hs_init( void )
{
  int i;                       /* Index count           */
#ifndef FEATURE_BITMAP
  nv_cmd_type    nv_cmd_buf;   /* Command buffer to NV  */
  nv_item_type   nv_lcd_cal;   /* NV data buffer        */
#endif /* FEATURE_BITMAP */

  /* Initialize timers - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  rex_def_timer(&hs_rpt_timer,   &hs_tcb, HS_RPT_TIMER_SIG);   /* Dog report */
  rex_def_timer(&hs_gpio_timer,  &hs_tcb, HS_GPIO_TIMER_SIG);  /* GPIO poll  */
#ifdef FEATURE_HS_DISPLAY_CAPTURE
  rex_def_timer(&hs_disp_cap_timer,  &hs_tcb, HS_DISP_CAP_SIG);/* Disp cap   */
#endif

  /* Initialize command and "free" queues - - - - - - - - - - - - - - - - - -*/

  (void) q_init( &hs_cmd_q );
  (void) q_init( &hs_cmd_free_q );

  /* Place "free" commands on hs_cmd_free_q
  */
  for (i = 0; i < HS_NUM_CMDS; i++ ) {
    hs_cmd_bufs[i].hdr.done_q_ptr = &hs_cmd_free_q;
    q_put( &hs_cmd_free_q,
                      q_link( &hs_cmd_bufs[i], &hs_cmd_bufs[i].hdr.link ) );
  }

#ifdef FEATURE_NEW_SLEEP_API
  hs_sleep_handle = sleep_register( "HS", TRUE );
#endif

#ifdef FEATURE_IG_UI_CLIENT_SELECT
   if (hw_ig_ui_client_is_wombat())
   { /* Wombat (Linux) is running the keypad and display */

     /* ACK task creation and wait for start signal from the TMC/MC */
     task_start( HS_RPT_TIMER_SIG, DOG_HS_RPT, &hs_rpt_timer );

     return;
   }
#endif

#if defined(T_FFA6500) && !defined(T_FFA6550_SS)
#error code not present
#endif // #ifdef T_FFA6500 && !T_FFA6550_SS
#ifdef CUST_EDITION
  /* Initialize keypad */
  keypad_init();

  {
    db_items_value_type  db_item;
    boolean status  = FALSE;
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;

    errFlag = pm_get_rt_status(PM_RTC_ALRM_IRQ_HDL,&status);//判断是否是闹钟启动方式
    if ((errFlag == PM_ERR_FLAG__SUCCESS) && (status == TRUE))
    {
        db_item.db_poweruptype = DB_POWERUP_BYRTCTIMER;
    }
    else
    {
        errFlag = pm_get_rt_status(PM_VALID_CHG_IRQ_HDL,&status);
        if ((errFlag != PM_ERR_FLAG__SUCCESS) || (status != TRUE))
        {
            errFlag = pm_get_rt_status(PM_VALID_USB_CHG_IRQ_HDL,&status);
        }
        if ((errFlag == PM_ERR_FLAG__SUCCESS) && (status == TRUE))
        {
            db_item.db_poweruptype = DB_POWERUP_BYEXTERNALPWR;
        }
        else
        {          
            if (g_BootResetFlag == BOOT_RESET_MAGIC)
            {
                db_item.db_poweruptype = DB_POWERUP_BYRESET;//重启方式
                g_BootResetFlag = 0;
            }
            else
            {
                db_item.db_poweruptype = DB_POWERUP_BYKEY;//按键启动
            }           
        }
    }
    
    db_put(DB_POWERUPTYPE, &db_item);
	
    if (db_item.db_poweruptype == DB_POWERUP_BYKEY)
    {
        clk_busy_wait(1500*1000);//等2秒
        if (!keypad_is_power_key_pressed())//如果没有按住开机键，那就关机，否则开机
        {
            hw_power_off();
        }
    }    
  }

#endif
#ifdef FEATURE_BITMAP
  /* Initialize the display */
  dog_report(DOG_HS_RPT);
  disp_init();
  dog_report(DOG_HS_RPT);
  /* For color LCD we're testing the backlight has to be on to see
     anything on the screen, so don't wait for UI, do it here */
#ifndef CUST_EDITION
  disp_powerup();
#endif
#ifdef FEATURE_HWTC
#error code not present
#endif /* FEATURE_HWTC */
#else /* FEATURE_BITMAP */
  /* Initialize the LCD controller - - - - - - - - - - - - - - - - - - - - - */
  hs_lcd_init();
#endif /* FEATURE_BITMAP */

#ifndef CUST_EDITION
  /* Initialize keypad */
  keypad_init();
#endif
  /* Initialize the GPIO outputs - - - - - - - - - - - - - - - - - - - - - - */

  HS_CALL_LED_INIT();           /* Call LED control GPIO  */
  HS_BACKLIGHT_INIT();          /* Backlight control GPIO */

  /* Set the ones detect latch to detect 0's on PCM the Data-In line.
   */
  voc_reset_ones_detect();

#ifdef T_FFA6550_SS
#error code not present
#endif /* T_FFA6550_SS */

#ifndef FEATURE_HWTC
#ifdef FEATURE_BSPUI_API
  HS_SLEEP_ALLOW();
#else /* FEATURE_BSPUI_API */

#ifdef FEATURE_HEADSET
  /* Passing on the HS_EAR_UP_K.
  */
  PASS_KEY_CODE( HS_EAR_UP_K );
  PASS_KEY_CODE( HS_RELEASE_K );
#else
  /* Sample the state of the earpiece switch and pass it on.
  */
  hs_ear_up = HS_EAR_UP();
  PASS_KEY_CODE( (byte) ((hs_ear_up) ? HS_EAR_UP_K : HS_EAR_DOWN_K) );
  PASS_KEY_CODE( HS_RELEASE_K );
#endif /* FEATURE_HEADSET */

  /* Sample the state of the ignition switch and pass it on.
  ** If we are not in the hands-free adapter, ignore the ignition sensor.
  */
  if ( !HS_EXT_PWR_ON() )
  {
    PASS_KEY_CODE( (byte) HS_EXT_PWR_OFF_K );
    PASS_KEY_CODE( HS_RELEASE_K );
  }

  HS_SLEEP_ALLOW();
#endif /* FEATURE_BSPUI_API */
#endif /* FEATURE_HWTC */

  /* Define the blinking timer call-back. Will register the call-back
   * routine later when hs_blink_rate is set.
   */
  clk_def( &hs_cb_blinking );

  /* Synchronize task start-up with the Main Control Task - - - - - - - - - -*/

  /* ACK task creation and wait for start signal from the TMC/MC
  */
  task_start( HS_RPT_TIMER_SIG, DOG_HS_RPT, &hs_rpt_timer );

#ifndef FEATURE_BITMAP
  /* Clear start signal
  */
  (void) rex_clr_sigs( &hs_tcb, TASK_START_SIG );

  /* Calibrate the LCD from data stored in NV RAM - - - - - - - - - - - - - -*/
#ifndef FEATURE_HWTC
  /* Prepare command buffer to NV
  */
  nv_cmd_buf.cmd = NV_READ_F;                 /* Command is Read       */
  nv_cmd_buf.tcb_ptr = &hs_tcb;               /* Signal HS when done   */
  nv_cmd_buf.sigs = HS_NV_DONE_SIG;           /* With this signal      */
  nv_cmd_buf.done_q_ptr = NULL;               /* Do not re-queue       */
  nv_cmd_buf.item = NV_LCD_I;                 /* Get LCD cal value     */
  nv_cmd_buf.data_ptr = &nv_lcd_cal;

  /* Clear signal, issue the command, and wait for the respone.
  */

  (void) rex_clr_sigs(&hs_tcb, HS_NV_DONE_SIG); /* Clear signal for NV */
  nv_cmd(&nv_cmd_buf);                          /* Issue the request   */

  /* While waiting, report to the watchdog periodically
  ** Only check for the NV_DONE signal, other signals may also get sent.
  */
  while ( (rex_timed_wait( HS_RPT_TIMER_SIG | HS_NV_DONE_SIG, &hs_rpt_timer,
                          DOG_HS_RPT_TIME  ) & HS_NV_DONE_SIG) != HS_NV_DONE_SIG ) {
    dog_report(DOG_HS_RPT);
  }

  (void) rex_clr_sigs(&hs_tcb, HS_NV_DONE_SIG); /* Clear signal from NV  */

  /* Check to see if calibration data is available.  If it is, use it.
  */
  if (nv_cmd_buf.status != NV_DONE_S) {
     nv_lcd_cal.lcd = 0x75;
  }

#endif /* FEATURE_HWTC - DMSS initialization uses nv items which HWTC does not use */

#ifndef FEATURE_HWTC  /* dmss below */
  /* Power supply control
  */
  hs_lcd_out( HS_LCD_POW_SUPPLY( ( (nv_lcd_cal.lcd >> 4) & 0x0f) ) );

  /* Contrast control
  */
  hs_lcd_out( HS_LCD_CONTRAST( (nv_lcd_cal.lcd & 0x0f) ) );

#else /* FEATURE_HWTC below */
#error code not present
#endif /* FEATURE_HWTC */
#endif /* FEATURE_BITMAP */
#ifdef FEATURE_LCD_TOUCH_ENABLE

  /* Initialize touchpad*/
  touchpad_init();
#endif
} /* end of hs_init */

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
)
{
  rex_sigs_type sigs;           /* our task's signals */

  /* Init the handset and wait for a startup message from Main Control task
  */
  hs_init();

  /* Set the watchdog report timer signal so that we'll
  ** reset the watchdog as soon as we start checking for events
  */
  (void) rex_set_sigs( &hs_tcb, HS_RPT_TIMER_SIG );

  /* Setup the timer for the GPIO polling
  */
  (void) rex_set_timer( &hs_gpio_timer, HS_GPIO_POLL_TIME );

  /* Loop forever
  */
  for (;;) {

    /* Wait for something to happen
    */
    sigs = rex_wait( HS_RPT_TIMER_SIG |   /* Watchdog report timer    */
    #ifdef FEATURE_LCD_TOUCH_ENABLE
    				 HS_TOUCHPAD_TIMER_SIG |
    				 #endif
                     HS_CMD_Q_SIG |       /* Command Queue signal     */
                     HS_GPIO_TIMER_SIG |  /* GPIO 'key' poll          */
                     HS_BLINK_TIMER_SIG | /* Output blink signal      */
                     HS_DISP_CAP_SIG |    /* Display Capture Signal   */
                     TASK_OFFLINE_SIG |   /* Command to go offline    */
                     TASK_STOP_SIG );     /* Command to shutdown      */

    /* If watchdog report timer expired, report and restart the timer
    */
    if ( sigs & HS_RPT_TIMER_SIG ) {
      dog_report(DOG_HS_RPT);
      (void) rex_set_timer( &hs_rpt_timer, DOG_HS_RPT_TIME );
    }

	//add by miaoxiaoming
#ifdef CUST_EDITION
#ifdef FEATURE_ANALOG_TV
		if ((sigs & HS_ATV_SET_CHN_SIG)!= 0) 
		{
			extern void hs_start_set_chn(void);
			hs_start_set_chn();
		}	 
#endif
#endif


    /* Handle the stop signal, process task stop procedures and continue
    */
    if ( sigs & TASK_STOP_SIG ) {
      #ifdef T_FFA
#error code not present
      #endif /* T_FFA */
      task_stop();
      (void) rex_clr_sigs( &hs_tcb, TASK_STOP_SIG);    /* Clear the signal */
    }

    /* Handle the offline signal, process task offline procedures and continue
    */
    if ( (sigs & TASK_OFFLINE_SIG) != 0 ) {
      task_offline();
      (void) rex_clr_sigs( &hs_tcb, TASK_OFFLINE_SIG); /* Clear the signal */
    }
	#ifdef FEATURE_LCD_TOUCH_ENABLE
	if ( (sigs & HS_TOUCHPAD_TIMER_SIG) != 0 )
	{
		(void) rex_clr_sigs( &hs_tcb, HS_TOUCHPAD_TIMER_SIG); /* Clear the signal */ 
		(void)touchpad_polling(0);
		//(void) rex_set_timer( &hs_pen_timer, HS_PEN_SCAN_TIME );
	}
	#endif

    /* Blink the screen/annunciators
    ** This means display the 'blink' copy if the 'non-blink' copy is
    ** currently displayed, and visa versa.  This applys to both the
    ** annunciators and the contents of the LCD display.
    */
    if ( sigs & HS_BLINK_TIMER_SIG ) {
      (void) rex_clr_sigs( &hs_tcb, HS_BLINK_TIMER_SIG );

      hs_blink_active = ! hs_blink_active;           /* Invert the logic    */

      /* Set the current screen/annunciator contents
      */
      if (hs_blink_active) {
        hs_crnt_screen_ptr = hs_screen;              /* Current screen       */
        hs_crnt_annun_ptr  = &hs_annun;              /* Current annunciators */
        hs_crnt_rssi_ptr   = &hs_rssi;               /* Current RSSI         */
        hs_crnt_batt_ptr   = &hs_batt;               /* Current Battery      */
      } else {
        hs_crnt_screen_ptr = hs_blink_screen;        /* Blink screen         */
        hs_crnt_annun_ptr  = &hs_blink_annun;        /* Blink annun's        */
        hs_crnt_rssi_ptr   = &hs_blink_rssi;         /* Blink RSSI           */
        hs_crnt_batt_ptr   = &hs_blink_batt;         /* Blink Battery        */
      }

      /* Update the views */
#ifdef FEATURE_BITMAP
      /* Update the screen */
      chartoimg_update_screen(hs_crnt_screen_ptr);
      /* Update the annuncaitor */
      chartoimg_update_annunciators(hs_crnt_annun_ptr, hs_crnt_rssi_ptr,
                                   hs_crnt_batt_ptr);
#else /* FEATURE_BITMAP */
      hs_update_screen();                         /* display screen       */
      hs_update_annun();                          /* display annunciators */
#endif /* FEATURE_BITMAP */

    }

    /* If we have commands to process, do so.  We may poll the handset,
     * which would clear the signal HS_POLL_TIMER_SIG.
     */
    if ( sigs & HS_CMD_Q_SIG ) {
      hs_req();
    }

    /* Poll the keys which are signals, and not actually in the handset
     */
    if ( (sigs & HS_GPIO_TIMER_SIG) != 0 )
    {
      (void) hs_poll_gpio();
      (void) rex_set_timer( &hs_gpio_timer, HS_GPIO_POLL_TIME );
    }

#ifdef FEATURE_HS_DISPLAY_CAPTURE
    /* Handle display capture for DIAG
     */
    if ( (sigs & HS_DISP_CAP_SIG) != 0 ) {
#ifdef FEATURE_HS_DISPLAY_CAPTURE_LIMITED_READ
      uint8 yield_time;
      rex_clr_sigs(&hs_tcb, HS_DISP_CAP_SIG);
      yield_time = hsdiag_hs_display_capture(HS_DISP_CAP_READ_LIMIT, HS_DISP_CAP_YIELD_TIME);
      if (yield_time)
      {
        rex_set_timer(&hs_disp_cap_timer, yield_time);
      }
#else /* FEATURE_HS_DISPLAY_CAPTURE_LIMITED_READ */
      hsdiag_hs_display_capture();
      rex_clr_sigs(&hs_tcb, HS_DISP_CAP_SIG);
#endif /* FEATURE_HS_DISPLAY_CAPTURE_LIMITED_READ */
    }
#endif /* FEATURE_HS_DISPLAY_CAPTURE */
  } /* end of infinite loop */

} /* end of hs_task */

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
void hs_cmd
(
  hs_packets_type *cmd_ptr
    /* pointer to a handset command.  When the command has finished, the
    ** command structure is put on the "done queue".  If there is no "done
    ** queue", or this operation is not desirable, set the "done queue"
    ** pointer to NULL.  Also when the command has completed, the specified
    ** task is signaled with the sig parameter.  If no signaling is required,
    ** the task pointer parameter can be set to NULL.
    */
)
{

  (void) q_link(cmd_ptr, &cmd_ptr->hdr.link );  /* Initialize link field */

  q_put( &hs_cmd_q, &cmd_ptr->hdr.link );       /* Put on command queue */
  (void) rex_set_sigs( &hs_tcb, HS_CMD_Q_SIG ); /* Signal a queue event */


} /* end of hs_cmd */
