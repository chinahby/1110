#ifndef HS_MB6550_H
#define HS_MB6550_H
/*===========================================================================

     M B 6 5 5 0   H A N D S E T   I N T E R N A L   H E A D E R   F I L E

DESCRIPTION
  Internal definitions specific to the MSM6550 handset module.


Copyright (c) 2004, 2005, 2006, 2007, 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //depot/asic/qsc1100/services/hs/hs_mb6550.h#1 $ $DateTime: 2008/07/29 17:00:16 $ $Author: ehong $

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/02/08   hs      Added support for FEATURE_CHG_TASK.
03/28/08   gdp     FEATURE_NEW_SLEEP_API
02/05/07   fs      Adapted HS_HEADSET_ON for QSC6055
03/08/05   ty      Added support for display capture in HS
09/20/04   jkl     Fixed HEADSET STEREO Detection.
09/02/04   trc     Slingshot stereo headset detection support
08/31/04   jkl     Initial MSM6550 release

===========================================================================*/

#include "comdef.h"     /* Definitions for byte, word, etc.        */
#include "bio.h"        /* Bit I/O macros                          */
#ifdef T_SLEEP
#include "sleep.h"      /* Sleep task signaling                    */
#endif
#if defined(FEATURE_PMIC)
#include "pm.h"         /* LCD and KBD driver on PMIC              */
#endif

#if defined(FEATURE_BATTERY_CHARGER) || defined(FEATURE_CHG_TASK) 
#include "charger.h"
#endif


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------

  Task specific macros, commands, definitions, and types

---------------------------------------------------------------------------*/
#ifdef FEATURE_LCD_TOUCH_ENABLE

#define HS_TOUCHPAD_TIMER_SIG   0x0800
#endif

/* Handset Task Signals
*/
#define HS_DISP_CAP_SIG         0x0200  /* Display capture signal         */
#define HS_NV_DONE_SIG          0x0100  /* NV command "done" signal       */
#define HS_GPIO_TIMER_SIG       0x0080  /* GPIO 'key' poll timer signal   */
#define HS_BLINK_TIMER_SIG      0x0008  /* Blink on/off timer             */
#define HS_CMD_Q_SIG            0x0004  /* Queued commands pending        */
#define HS_RPT_TIMER_SIG        0x0001  /* Time to report to the watchdog */

/* How often (in milliseconds) to poll the GPIO 'keys' for their state.
*/
#define HS_GPIO_POLL_TIME       100

/* How long to delay (in microseconds) when resetting the one's detector
*/
#define HS_ONES_DET_DELAY       200

/* How often (in HS_GPIO_POLL_TIME) to check the battery charging state.
*/
#define HS_POLL_CHG_LOOP        50

#ifdef FEATURE_ANALOG_TV
#define HS_ATV_SET_CHN_SIG      0x0400  /*tv set channel signal*/
#endif


/*---------------------------------------------------------------------------

  LCD Controller macros, commands, definitions, and types

---------------------------------------------------------------------------*/

/* Mask of GPIO bits for the LCD controller serial interface
*/
#define HS_LCD_MASK     ( BIO_LCD_DATA_M   \
                        | BIO_LCD_CLK_M    \
                        | BIO_LCD_A0_M     \
                        | BIO_LCD_SELECT_M )

/* A0 bit of an LCD command
*/
#define HS_LCD_A0       0x0100

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Epson SED1230 Instructions
*/

/* Cursor Home:  clear the address counter and move the cursor to
** the left edge of the first line.
*/
#define HS_LCD_HOME_I           0x0010

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

/* Static Display Control:  control COMS1 and SEGS1 on/off/blink
** Set as All display segments ON
*/
#define HS_LCD_STATIC_I         0x0023

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

/* Display on/off control:  on/off control for the display and cursors
** Set the display on/off, with no cursors.
*/
#define HS_LCD_DISPLAY_ON_I     0x0031
#define HS_LCD_DISPLAY_OFF_I    0x0030

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

/* Power Save control:  setup or cancel powersave mode
** Powersave with the oscillator on, or non-power-save w/oscillator
*/
#define HS_LCD_POWERSAVE_OFF_I  0x0042
#define HS_LCD_POWERSAVE_ON_I   0x0043

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

/* Power supply control:  control the internal power supply circuit.
** Turn on all of the supplies
*/
#define HS_LCD_POW_SUPPLY_I     0x005F

#define HS_LCD_POW_SUPPLY(p)    (0x0050 | (p))

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

/* RAM control:  configure the RAM shape and CGRAM on/off
** 4 lines, use CGRAM
*/
#define HS_LCD_RAM_CTL_I        0x0069

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

/* Contrast control:  electronic contrast control
** Set the contrast to midscale
*/
#define HS_LCD_CONTRAST_I       0x0078

#define HS_LCD_CONTRAST(c)      (0x0070 | (c))

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

/* Address select:  set the DDRAM, CGRAM, symbol address
*/
#define HS_LCD_ADDR_I(addr)     ( 0x0080 | (addr) )

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

/* Write Data:  load data into the DDRAM, CGRAM, or symbol register
*/
#define HS_LCD_DATA_I(data)     ( HS_LCD_A0 | (data) )

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Number of rows in a character (characters are 5x7)
*/
#define HS_LCD_FONT_ROWS        7

/* Size to step to next character
*/
#define HS_LCD_FONT_CSTEP       (7+1)

/* Number of characters in the user-defined font
*/
#define HS_LCD_FONT_CHARS       4

/* Size of the entire user-defined font
*/
#define HS_LCD_FONT_SIZE        (HS_LCD_FONT_CSTEP * HS_LCD_FONT_CHARS)

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#define HS_LCD_MAX_LINES        4       /* Number of lines on the display    */
#define HS_LCD_MAX_CHARS        12      /* Number of characters per line     */

#define HS_LCD_LINE0_ADDR       0x0030  /* Start address of display line 0   */
#define HS_LCD_LINE1_ADDR       0x0040  /* Start address of display line 1   */
#define HS_LCD_LINE2_ADDR       0x0050  /* Start address of display line 2   */
#define HS_LCD_LINE3_ADDR       0x0060  /* Start address of display line 3   */

#define HS_LCD_SYMB_ADDR        0x0070  /* Start address of display symbols  */

#define HS_LCD_CGRAM_ADDR       0x0000  /* Start address of RAM font         */

/* Useful values for specifying line numbers
*/
#define HS_LCD_LINE0            0
#define HS_LCD_LINE1            1
#define HS_LCD_LINE2            2
#define HS_LCD_LINE3            3


/*===========================================================================

MACRO HS_EXT_PWR_ON

DESCRIPTION
  Return the ignition on/off status when in the hands-free car kit.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  True if external power is supplied
  False is external power is not supplied

SIDE EFFECTS
  None

===========================================================================*/
#if defined (FEATURE_BATTERY_CHARGER) || defined(FEATURE_CHG_TASK) 
#define HS_EXT_PWR_ON() chg_is_charger_valid()
#else
#define HS_EXT_PWR_ON() FALSE
#endif



/*===========================================================================

MACRO HS_HEADSET_ON

DESCRIPTION
  Return the headset status

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  True if the headset is connected
  False if the headset is disconnected

SIDE EFFECTS
  None

===========================================================================*/
#define HS_HEADSET_ON() \
  ( gpio_in( HEADSET_DET_N ) == BIO_HEADSET_ON_V ) 


/*===========================================================================

MACRO HS_HEADSET_TYPE

DESCRIPTION
  Return the headset type

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  True if a stereo headset is detected
  False if a mono headset is detected

SIDE EFFECTS
  None

===========================================================================*/
#ifdef T_SURF
  #define HS_HEADSET_TYPE() (1)
#else
  #define HS_HEADSET_TYPE() (1)
#endif


/*===========================================================================

MACRO HS_SDAC_ON

DESCRIPTION
  Return the SDAC status

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  True if the SDAC is connected
  False if the SDAC is disconnected

SIDE EFFECTS
  None

===========================================================================*/
#ifdef T_SURF
  #define HS_SDAC_ON() (0)
#else
  #define HS_SDAC_ON() (0)
#endif


/*---------------------------------------------------------------------------

  Backlight initialization and control

---------------------------------------------------------------------------*/


/*===========================================================================

MACRO HS_BACKLIGHT_INIT

DESCRIPTION
  Initialize the GPIO output to drive the LCD and keypad backlight

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define HS_BACKLIGHT_INIT() \
  BIO_TRISTATE( BIO_BACKLIGHT_REG, BIO_BACKLIGHT_M, BIO_OUTPUT_ENA_V)


/*===========================================================================

MACRO HS_BACKLIGHT_ON

DESCRIPTION
  Turn on the LCD and keypad backlight

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_PMIC_LCDKBD_LED_DRIVER
#define HS_BACKLIGHT_ON() \
  pm_set_led_intensity(PM_LCD_LED, hs_lcd_level);\
  pm_set_led_intensity(PM_KBD_LED, hs_kpd_level);\
  BIO_OUT( BIO_BACKLIGHT_REG, BIO_BACKLIGHT_M, BIO_BACKLIGHT_ON_V )
#else
#define HS_BACKLIGHT_ON() \
  BIO_OUT( BIO_BACKLIGHT_REG, BIO_BACKLIGHT_M, BIO_BACKLIGHT_ON_V )
#endif


/*===========================================================================

MACRO HS_BACKLIGHT_OFF

DESCRIPTION
  Turn off the LCD and keypad backlight

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_PMIC_LCDKBD_LED_DRIVER
#define HS_BACKLIGHT_OFF() \
  pm_set_led_intensity(PM_LCD_LED, 0);\
  pm_set_led_intensity(PM_KBD_LED, 0);\
  BIO_OUT( BIO_BACKLIGHT_REG, BIO_BACKLIGHT_M, BIO_BACKLIGHT_OFF_V )
#else
#define HS_BACKLIGHT_OFF() \
  BIO_OUT( BIO_BACKLIGHT_REG, BIO_BACKLIGHT_M, BIO_BACKLIGHT_OFF_V )
#endif


/*---------------------------------------------------------------------------

  Call LED initialization and control

---------------------------------------------------------------------------*/


/*===========================================================================

MACRO HS_CALL_LED_INIT

DESCRIPTION
  Initialize the GPIO output to drive the Call LED.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define HS_CALL_LED_INIT() \
  BIO_TRISTATE( BIO_CALL_LED_REG, BIO_CALL_LED_M, BIO_OUTPUT_ENA_V)


/*===========================================================================

MACRO HS_CALL_LED_ON

DESCRIPTION
  Turn on the Call LED

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define HS_CALL_LED_ON() \
  BIO_OUT( BIO_CALL_LED_REG, BIO_CALL_LED_M, BIO_CALL_LED_ON_V )


/*===========================================================================

MACRO HS_CALL_LED_OFF

DESCRIPTION
  Turn off the Call LED.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define HS_CALL_LED_OFF() \
  BIO_OUT( BIO_CALL_LED_REG, BIO_CALL_LED_M, BIO_CALL_LED_OFF_V )


/*---------------------------------------------------------------------------

  SLEEP allow/disallow macros

---------------------------------------------------------------------------*/


/*===========================================================================

MACRO HS_SLEEP_ALLOW

DESCRIPTION
  Tell the sleep task that we are in a state where we can sleep.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The sleep task may take us up on our offer.

===========================================================================*/
#if  defined( FEATURE_NEW_SLEEP_API )
#define HS_SLEEP_ALLOW() \
  { sleep_assert_okts ( hs_sleep_handle ); }
#elif defined( T_SLEEP )
#define HS_SLEEP_ALLOW() \
  { (void) rex_set_sigs( &sleep_tcb, SLEEP_HS_OKTS_SIG ); }
#else
#define HS_SLEEP_ALLOW()
#endif


/*===========================================================================

MACRO HS_SLEEP_FORBID

DESCRIPTION
  Tell the sleep task that we are NOT in a state where we can sleep.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#if defined( FEATURE_NEW_SLEEP_API )
#define HS_SLEEP_FORBID() \
  { sleep_negate_okts ( hs_sleep_handle ); }
#elif defined( T_SLEEP )
#define HS_SLEEP_FORBID() \
  { (void) rex_clr_sigs( &sleep_tcb, SLEEP_HS_OKTS_SIG ); }
#else
#define HS_SLEEP_FORBID()
#endif

#endif /* HS_MB6550_H */

