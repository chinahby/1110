/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               K E Y P A D   D E V I C E   D R I V E R

GENERAL DESCRIPTION
  The keypad device driver code.
  The keypad_scan_keypad is executed from the interrupt context.
  All other functions are executed from the callers context. There is no
  keypad task.

  When a key is pressed, srch_keypress_isr is executed. This wakes up the
  ARM.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  keypad_init must be called to initialize the keypad operation.
  keypad_register is then called to register for service.


  Copyright (c) 2001-2008 QUALCOMM Incorporated. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc6055/drivers/kpd/keypad.c#14 $ $DateTime: 2008/12/05 12:12:15 $ $Author: rtilak $

when       who      what, where, why
--------   ---      ---------------------------------------------------------
11/20/08   rt       Add support for PMIC based headset insertion detection
11/10/08   sg       Add check to avoid initializing keypad multiple times
11/03/08   rt       Make keypad_init() atomic with respect to interrupts
07/15/08   rt       Make keypad_init() atomic with respect to task switch
05/18/07   gdp      Removed pm_hsed_init. This is already initialized by PMIC
02/12/08   cr       keypad_init() itself has to check the static initialized
                    var to prevent two passes through the init function.
05/18/07   gdp      Changes to support "*"/"#" while in phone is crashed state
03/14/07   gmy      Lint cleanup
03/14/07   gdp      Added new sleep APIs
12/20/06   gmy      Added keypad_init_iface() & keypad_is_key_pressed() for SC2x
12/14/06   gmy      SC2x 'i' key (FFA's blank key) now F1 making <SELECT> unique
11/13/06   gmy      Refactor improvements in driver identified during bringup
11/09/06   jhs      Added support for new sleep voting mechanism.
10/30/06   gmy      Support for SC2x (5x5 matrix), removed extraneous code,
                    convert BIO references to gpio_ interface calls.
07/30/06   adb      Merged changes for PANDORA keys
06/22/05   gr       Merged in FEATURE_IG_KEYPAD changes
06/15/05   cep      Fix several issues to allow cross-usage with L4
10/18/04   trc      Remap side-keys, add PTT(spkrphone key) for EFFA
09/09/04   jkl      Increased FFA polling time.
08/17/04   trc      Restructured to accomodate various hardware mappings in
                    a more general way.
01/06/04   dle      Modified panic key cord for FEATURE_KEYPAD_5_BY_6_MATRIX.
08/27/03   tbg      Merged in changes from 6250 branch.
08/15/03   kar      Support for 6300 FFA
01/07/03   gtb      Resistive keypad now checks GPIO for detect instead of polling ADC.
10/15/02   dle      Removed FEATURE_HWTC.
10/14/02   dle      Restored PM operation. Restored non-resistive, non-
                    navigational keypad KEYPAD_COLUMNs to 6. Merged in
                    key assignment for resistive keypad.
07/15/02   gtb      Added in resistive keypad support
06/19/02   rmd      Condensed the PMIC power key code into one function.
06/12/02   rmd      Provided the ability to detect a power key press through
                    an ISR instead of polling the power key status all the
                    time.
06/04/02   jwh      Added HWTC support.
05/31/02   dlb      Don't enable backlight through BIO macro if PMIC control
                    is defined.
11/12/01   dl       Ported from hs_v2428.c

===========================================================================*/

/*===========================================================================

                       INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"   /* Customer specific definitions        */
#include "comdef.h"
#ifdef FEATURE_IG_KEYPAD
#error code not present
#endif /* FEATURE_IG_KEYPAD */

#include "keypad.h"
#ifndef FEATURE_IG_KEYPAD
#include "clk.h"
#include "err.h"
#include "msg.h"
#ifdef BUILD_BOOT_CHAIN
#include "bio.h"
#endif
#if defined(FEATURE_PMIC)
  #include "pm.h"
#endif /* defined(FEATURE_PMIC) */
#endif /* NOT FEATURE_IG_KEYPAD */

/*===========================================================================

                 DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/* The following logic will define a per-target set of macros that
 * implement various keypad hardware implementations. These are as follows:
 *
 * KEYPAD_POLLING_DELAY_USEC - This is the fixed delay between driving a
 *                             column scan line low and actually reading the
 *                             row sense input.  This delay needs to become
 *                             larger in proportion to parasitic line capacitance.
 *
 * KEYPAD_5_BY_?_MATRIX - This macro defines the actual scan row x column
 *                        matrix style.  It is consumed below to generate the
 *                        key mapping and explicit row and column size macros.
 */
#ifdef T_QSC60X5
/* SC2x SURF and FFA share keypad driver. The SURF's FPGA remaps SURF keypad
 * hardware to emulate the FFA keypad. This keypad is derived from the MSM6550
 * Pandora FFA. */
#define KEYPAD_POLLING_DELAY_USEC 10

//#if !(defined(FEATURE_PROJECT_W203) || defined(FEATURE_PROJECT_W204) || defined(FEATURE_PROJECT_W021_220X176))
#if !defined (FEATURE_ALL_KEY_PAD)
#define KEYPAD_5_BY_5_MATRIX
#endif



/* Given a row and column, decode it into a keycode.  The comment above
** each entry is its row/column address.
*/

#if defined(KEYPAD_5_BY_5_MATRIX)

/* Define the rows/columns constants */
  #define KEYPAD_ROWS             5
  #define KEYPAD_COLUMNS          5

#ifndef BUILD_BOOT_CHAIN
#ifdef CUST_EDITION
#ifdef FEATURE_LCD_TOUCH_ENABLE   //add by yangdecai
static const hs_key_type keys[ KEYPAD_ROWS ][ KEYPAD_COLUMNS ] = {
   /* JoyStick keys [up, down, left, right] uses 2 simultaneous row inputs
    * on KEYSENSE0_N routed by SC2x FFA's Panasonic MultiFunction Switch 
    * [EVQWH] as follows:
    *     Direction | Switches
    *       up          1,4
    *       down        2,3
    *       left        3,4
    *       right       1,2
    *
    * Switches are connected to Keypad signals:
    *     Switch   |  Keypad Signal
    *       1           KYPD_17
    *       2           KYPD_9
    *       3           KYPD_15
    *       4           KYPD_11
    *
    * Which gives us-
    *     KeyScan Code     Keypad Signals
    *     HS_KEY_UP_K      KYPD_17, KYPD_11
    *     HS_KEY_DOWN_K    KYPD_9,  KYPD_15
    *     HS_KEY_LEFT_K    KYPD_15, KYPD_11
    *     HS_KEY_RIGHT_K   KYPD_17, KYPD_9
    *
    * Mapping keypad signal to Keyscan codes:
    *     Keypad Signal              KeyScan Codes
    *     KYPD_17           HS_KEY_UP_K    | HS_KEY_RIGHT_K
    *     KYPD_15           HS_KEY_DOWN_K  | HS_KEY_LEFT_K
    *     KYPD_11           HS_KEY_LEFT_K  | HS_KEY_UP_K
    *     KYPD_9            HS_KEY_RIGHT_K | HS_KEY_DOWN_K
    */

   /* KEYSENSE_0 */
   /* 1,17          1,15        1,11         1,9          1,Memo*/
   { HS_SEND_K,      HS_DOWN_K,  HS_7_K,      HS_2_K,      HS_NONE_K/*HS_5_K*/},

   #ifdef FEATURE_VERSION_X3
   /* KEYSENSE_1 */
   /* 3,17          3,15        3,11         3,9          3,Memo*/
   { HS_SEL_K,      HS_RIGHT_K, HS_8_K,      HS_DOWN_K,      HS_NONE_K/*HS_0_K*/},

   /* KEYSENSE_2 */
   /* 5,17          5,15        5,11         5,9          5,Memo*/
   { HS_1_K,        HS_LEFT_K,  HS_9_K,      HS_UP_K,      HS_NONE_K/*HS_DOWN_K*/},
   #else
   /* KEYSENSE_1 */
   /* 3,17          3,15        3,11         3,9          3,Memo*/
   { HS_SEL_K,      HS_RIGHT_K, HS_8_K,      HS_3_K,      HS_NONE_K/*HS_0_K*/},

   /* KEYSENSE_2 */
   /* 5,17          5,15        5,11         5,9          5,Memo*/
   { HS_1_K,        HS_LEFT_K,  HS_9_K,      HS_4_K,      HS_NONE_K/*HS_DOWN_K*/},
   #endif
   /* KEYSENSE_3 */
   /* 7,17          7,15        7,11         7,9          7,Memo*/
   { HS_CLR_K,      HS_INFO_K,  HS_0_K,      HS_5_K,      HS_NONE_K/*HS_CLR_K*/},

   /* KEYSENSE_4 */
   /* 9,17          9,15        9,11         9,9          9,Memo*/
   { HS_6_K,  	    HS_UP_K,    HS_POUND_K,  HS_STAR_K,   HS_NONE_K}
};

#else
static const hs_key_type keys[ KEYPAD_ROWS ][ KEYPAD_COLUMNS ] = {
   /* JoyStick keys [up, down, left, right] uses 2 simultaneous row inputs
    * on KEYSENSE0_N routed by SC2x FFA's Panasonic MultiFunction Switch 
    * [EVQWH] as follows:
    *     Direction | Switches
    *       up          1,4
    *       down        2,3
    *       left        3,4
    *       right       1,2
    *
    * Switches are connected to Keypad signals:
    *     Switch   |  Keypad Signal
    *       1           KYPD_17
    *       2           KYPD_9
    *       3           KYPD_15
    *       4           KYPD_11
    *
    * Which gives us-
    *     KeyScan Code     Keypad Signals
    *     HS_KEY_UP_K      KYPD_17, KYPD_11
    *     HS_KEY_DOWN_K    KYPD_9,  KYPD_15
    *     HS_KEY_LEFT_K    KYPD_15, KYPD_11
    *     HS_KEY_RIGHT_K   KYPD_17, KYPD_9
    *
    * Mapping keypad signal to Keyscan codes:
    *     Keypad Signal              KeyScan Codes
    *     KYPD_17           HS_KEY_UP_K    | HS_KEY_RIGHT_K
    *     KYPD_15           HS_KEY_DOWN_K  | HS_KEY_LEFT_K
    *     KYPD_11           HS_KEY_LEFT_K  | HS_KEY_UP_K
    *     KYPD_9            HS_KEY_RIGHT_K | HS_KEY_DOWN_K
    */

   /* KEYSENSE_0 */
   /* 1,17          1,15        1,11        1,9          1,Memo*/
   #ifndef FEATURE_VERSION_C180
   { HS_CAMERA_K,   HS_SEND_K,  HS_DOWN_K,  HS_MUSIC_K,  HS_NONE_K},
    #else
     { HS_NONE_K,   HS_SEND_K,  HS_DOWN_K,  HS_NONE_K,  HS_NONE_K},
    #endif
   
   /* KEYSENSE_1 */
   /* 3,17          3,15        3,11        3,9          3,Memo*/
   { HS_LEFT_K,     HS_1_K,     HS_2_K,     HS_3_K,      HS_NONE_K},

   /* KEYSENSE_2 */
   /* 5,17          5,15        5,11        5,9          5,Memo*/
   { HS_INFO_K,     HS_4_K,     HS_5_K,     HS_6_K,      HS_NONE_K},

   /* KEYSENSE_3 */
   /* 7,17          7,15        7,11        7,9          7,Memo*/
   { HS_RIGHT_K,    HS_7_K,     HS_8_K,     HS_9_K,      HS_CLR_K},

   /* KEYSENSE_4 */
   /* 9,17          9,15        9,11        9,9          9,Memo*/
   { HS_UP_K,  		HS_STAR_K,  HS_0_K,     HS_POUND_K,  HS_SEL_K}
};
#endif
#else
static const hs_key_type keys[ KEYPAD_ROWS ][ KEYPAD_COLUMNS ] = {
   /* JoyStick keys [up, down, left, right] uses 2 simultaneous row inputs
    * on KEYSENSE0_N routed by SC2x FFA's Panasonic MultiFunction Switch 
    * [EVQWH] as follows:
    *     Direction | Switches
    *       up          1,4
    *       down        2,3
    *       left        3,4
    *       right       1,2
    *
    * Switches are connected to Keypad signals:
    *     Switch   |  Keypad Signal
    *       1           KYPD_17
    *       2           KYPD_9
    *       3           KYPD_15
    *       4           KYPD_11
    *
    * Which gives us-
    *     KeyScan Code     Keypad Signals
    *     HS_KEY_UP_K      KYPD_17, KYPD_11
    *     HS_KEY_DOWN_K    KYPD_9,  KYPD_15
    *     HS_KEY_LEFT_K    KYPD_15, KYPD_11
    *     HS_KEY_RIGHT_K   KYPD_17, KYPD_9
    *
    * Mapping keypad signal to Keyscan codes:
    *     Keypad Signal              KeyScan Codes
    *     KYPD_17           HS_KEY_UP_K    | HS_KEY_RIGHT_K
    *     KYPD_15           HS_KEY_DOWN_K  | HS_KEY_LEFT_K
    *     KYPD_11           HS_KEY_LEFT_K  | HS_KEY_UP_K
    *     KYPD_9            HS_KEY_RIGHT_K | HS_KEY_DOWN_K
    */

   /* KEYSENSE_0 */
   /* 1,17          1,15        1,11        1,9          1,Memo*/
   { HS_UP_K,       HS_DOWN_K,  HS_LEFT_K,  HS_RIGHT_K,  HS_VOL_DOWN_K},

   /* KEYSENSE_1 */
   /* 3,17          3,15        3,11        3,9          3,Memo*/
   { HS_SEND_K,     HS_1_K,     HS_2_K,     HS_3_K,      HS_F1_K},

   /* KEYSENSE_2 */
   /* 5,17          5,15        5,11        5,9          5,Memo*/
   { HS_SF1_K,      HS_4_K,     HS_5_K,     HS_6_K,      HS_SEL_K},

   /* KEYSENSE_3 */
   /* 7,17          7,15        7,11        7,9          7,Memo*/
   { HS_SF2_K,      HS_7_K,     HS_8_K,     HS_9_K,      HS_CLR_K},

   /* KEYSENSE_4 */
   /* 9,17          9,15        9,11        9,9          9,Memo*/
   {  HS_VOL_UP_K,  HS_STAR_K,  HS_0_K,     HS_POUND_K,  HS_LSPKR_K}
};
#endif /*CUST_EDITION*/
#endif /*BUILD_BOOT_CHAIN*/

#ifdef CUST_EDITION
/*
 * Table position of keys contributing to the "panic chord" 
 */
/* '1' Key */
#define ONE_KEY_ROW      1
#define ONE_KEY_COLUMN   1

/* '*' Key */
#define STAR_KEY_ROW     2
#define STAR_KEY_COLUMN  2

/* '#' Key */
#define POUND_KEY_ROW    3
#define POUND_KEY_COLUMN 3

/* 
 * Table positions of navigation keys 
 */
/* Nav-Up Key */
#define UP_KEY_ROW         4
#define UP_KEY_COLUMN      0

/* Nav-Down Key */
#define DOWN_KEY_ROW       0
#define DOWN_KEY_COLUMN    2

/* Nav-Left Key */
#define LEFT_KEY_ROW       1
#define LEFT_KEY_COLUMN    0

/* Nav-Right Key */
#define RIGHT_KEY_ROW      3
#define RIGHT_KEY_COLUMN   0
#else
/*
 * Table position of keys contributing to the "panic chord" 
 */
/* '1' Key */
#define ONE_KEY_ROW      1
#define ONE_KEY_COLUMN   3

/* '*' Key */
#define STAR_KEY_ROW     4
#define STAR_KEY_COLUMN  1

/* '#' Key */
#define POUND_KEY_ROW    4
#define POUND_KEY_COLUMN 3

/* 
 * Table positions of navigation keys 
 */
/* Nav-Up Key */
#define UP_KEY_ROW         0
#define UP_KEY_COLUMN      0

/* Nav-Down Key */
#define DOWN_KEY_ROW       0
#define DOWN_KEY_COLUMN    1

/* Nav-Left Key */
#define LEFT_KEY_ROW       0
#define LEFT_KEY_COLUMN    2

/* Nav-Right Key */
#define RIGHT_KEY_ROW      0
#define RIGHT_KEY_COLUMN   3

#endif
#if !defined(KEYPAD_ROWS) || !defined(KEYPAD_COLUMNS)
//  #error A keypad matrix mapping must be defined to derive row/column constants
#endif

#ifdef FEATURE_KEYPAD_USES_GPIO_FOR_PWR_KEY
#error code not present
#endif


static const GPIO_SignalType keytbl_col_to_keyscan_gpio_map[KEYPAD_COLUMNS] = 
{
   KYPD_17,
   KYPD_15,
   KYPD_11,
   KYPD_9,
   KYPD_MEMO
};

/* The order of the following array must match row assignments in the
 * key matrix mapping table. */
static const GPIO_SignalType keytbl_row_to_keysense_gpio_map[KEYPAD_ROWS] = 
{
   KEYSENSE_0_N,
   KEYSENSE_1_N,
   KEYSENSE_2_N,
   KEYSENSE_3_N,
   KEYSENSE_4_N
};


#else /* !KEYPAD_5_BY_5_MATRIX */
//#error code not present
#endif



#ifdef FEATURE_ALL_KEY_PAD

#define KEYPAD_ROWS             5
#define KEYPAD_COLUMNS          8
#ifndef BUILD_BOOT_CHAIN

#ifdef SJDFJSADKF
static const hs_key_type keys[ KEYPAD_ROWS ][ KEYPAD_COLUMNS+1] =
{
	/* KIN0 */
	/* KOUT1 */		 /*KOUT2*/   /*KOUT3*/	 /*KOUT4*/    /*KOUT5*/     /*KOUT6*/   /*KOUT7*/
    { HS_SEND_K,     HS_TV_K,    HS_1_K,     HS_4_K,      HS_7_K,       HS_STAR_K, HS_STAR_K},

    /* KIN1 */
    { HS_SEL_K,      HS_DOWN_K,  HS_2_K,     HS_5_K,      HS_8_K,       HS_0_K, 	HS_0_K},

    /* KIN2 */
    { HS_CLR_K,      HS_END_K,   HS_3_K,     HS_6_K,      HS_9_K,       HS_POUND_K, HS_POUND_K },

    /* KIN3 */
    { HS_LS1_K,      HS_LEFT_K,  HS_RIGHT_K, HS_UP_K,     HS_LS2_K,     HS_NONE_K, 	HS_NONE_K},

    /* KIN4 */
    { HS_NONE_K,     HS_NONE_K,  HS_NONE_K,  HS_VOL_UP_K, HS_VOL_DOWN_K, HS_NONE_K ,HS_NONE_K},

    /* KIN5 */
    { HS_NONE_K,     HS_NONE_K,  HS_NONE_K,  HS_VOL_UP_K, HS_VOL_DOWN_K, HS_NONE_K ,HS_NONE_K},

    /* KIN6 */
    { HS_NONE_K,     HS_NONE_K,  HS_NONE_K,  HS_VOL_UP_K, HS_VOL_DOWN_K, HS_NONE_K ,HS_NONE_K},
};
#endif
#ifndef FEATURE_LANG_THAI
#if defined FEATURE_NUM_KEY_MID   //add by yang decai 2010-12-03
static const hs_key_type keys[ KEYPAD_ROWS ][ KEYPAD_COLUMNS +1] =
{
    /*KYPD_9            KYPD_11         KYPD_15         KYPD_17     KYPD_MEMO       GPIO_OUTPUT_41  GPIO_OUTPUT_42  GPIO_OUTPUT_34   GND*/
    /*KEYSENSE_0_N*/
    {HS_RIGHT_K,        HS_UP_K,        HS_INFO_K,      HS_S_K,     HS_DOWN_K,      HS_LEFT_K,      HS_SEL_K,       HS_W_K,         HS_SEND_K},
     /*KEYSENSE_1_N*/
    {HS_CLR_K,          HS_LCTRL_K,     HS_SYMBOL_K,    HS_F2_K,    HS_SPACE_K,     HS_Z_K,         HS_F1_K,        HS_F5_K,        HS_CAPLK_K},
     /*KEYSENSE_2_N*/
    {HS_ENTER_K,        HS_DOLLAR_K,    HS_M_K,         HS_0_K,     HS_9_K,         HS_8_K,         HS_7_K,         HS_X_K,         HS_SHIFT_K},
     /*KEYSENSE_3_N*/
    {HS_F3_K,           HS_L_K,         HS_K_K,         HS_POUND_K, HS_6_K,         HS_5_K,         HS_4_K,         HS_D_K,         HS_A_K},
     /*KEYSENSE_4_N*/
    {HS_P_K,            HS_O_K,         HS_I_K,         HS_STAR_K,  HS_3_K,         HS_2_K,         HS_1_K,         HS_E_K,         HS_Q_K}
};

#define ONE_KEY_ROW      4
#define ONE_KEY_COLUMN   6

/* '*' Key */
#define STAR_KEY_ROW     3
#define STAR_KEY_COLUMN  5

/* '#' Key */
#define POUND_KEY_ROW    2
#define POUND_KEY_COLUMN 4

#elif defined(FEATURE_VERSION_M8P)
static const hs_key_type keys[ KEYPAD_ROWS ][ KEYPAD_COLUMNS +1] =
{
    /*KYPD_9            KYPD_11         KYPD_15         KYPD_17     KYPD_MEMO       GPIO_OUTPUT_41  GPIO_OUTPUT_42  GPIO_OUTPUT_34   GND*/
    /*KEYSENSE_0_N*/
    {HS_RIGHT_K,        HS_UP_K,        HS_INFO_K,      HS_4_K,     HS_DOWN_K,      HS_LEFT_K,      HS_SEL_K,       HS_1_K,         HS_SEND_K},
     /*KEYSENSE_1_N*/
    {HS_CLR_K,          HS_LCTRL_K,     HS_SYMBOL_K,    HS_BLUETOOTH_K,  HS_SPACE_K,HS_7_K,         HS_MUSIC_K,     HS_0_K,         HS_CAPLK_K},
     /*KEYSENSE_2_N*/
    {HS_ENTER_K,        HS_DOLLAR_K,    HS_M_K,         HS_N_K,     HS_B_K,         HS_V_K,         HS_9_K,         HS_8_K,         HS_SHIFT_K},
     /*KEYSENSE_3_N*/
    {HS_F3_K,           HS_L_K,         HS_K_K,         HS_J_K,     HS_H_K,         HS_G_K,         HS_6_K,         HS_5_K,         HS_STAR_K},
     /*KEYSENSE_4_N*/
    {HS_P_K,            HS_O_K,         HS_I_K,         HS_U_K,     HS_Y_K,         HS_T_K,         HS_3_K,         HS_2_K,         HS_POUND_K}
};

#define ONE_KEY_ROW      0
#define ONE_KEY_COLUMN   7

/* '*' Key */
#define STAR_KEY_ROW     3
#define STAR_KEY_COLUMN  7

/* '#' Key */
#define POUND_KEY_ROW    2
#define POUND_KEY_COLUMN 6

#else
static const hs_key_type keys[ KEYPAD_ROWS ][ KEYPAD_COLUMNS +1] =
{
    /*KYPD_9            KYPD_11         KYPD_15         KYPD_17     KYPD_MEMO       GPIO_OUTPUT_41  GPIO_OUTPUT_42  GPIO_OUTPUT_34   GND*/
    /*KEYSENSE_0_N*/
    {HS_RIGHT_K,        HS_UP_K,        HS_INFO_K,      HS_4_K,     HS_DOWN_K,      HS_LEFT_K,      HS_SEL_K,       HS_1_K,         HS_SEND_K},
     /*KEYSENSE_1_N*/
    {HS_CLR_K,          HS_LCTRL_K,     HS_SYMBOL_K,    HS_F2_K,    HS_SPACE_K,     HS_7_K,         HS_F1_K,        HS_0_K,         HS_CAPLK_K},
     /*KEYSENSE_2_N*/
    {HS_ENTER_K,        HS_DOLLAR_K,    HS_M_K,         HS_N_K,     HS_B_K,         HS_V_K,         HS_9_K,         HS_8_K,         HS_SHIFT_K},
     /*KEYSENSE_3_N*/
    {HS_F3_K,           HS_L_K,         HS_K_K,         HS_J_K,     HS_H_K,         HS_G_K,         HS_6_K,         HS_5_K,         HS_STAR_K},
     /*KEYSENSE_4_N*/
    {HS_P_K,            HS_O_K,         HS_I_K,         HS_U_K,     HS_Y_K,         HS_T_K,         HS_3_K,         HS_2_K,         HS_POUND_K}
};

#define ONE_KEY_ROW      0
#define ONE_KEY_COLUMN   7

/* '*' Key */
#define STAR_KEY_ROW     3
#define STAR_KEY_COLUMN  7

/* '#' Key */
#define POUND_KEY_ROW    2
#define POUND_KEY_COLUMN 6

#endif
#else
static const hs_key_type keys[ KEYPAD_ROWS ][ KEYPAD_COLUMNS +1] =
{
    /*KYPD_9            KYPD_11         KYPD_15         KYPD_17     KYPD_MEMO       GPIO_OUTPUT_41  GPIO_OUTPUT_42  GPIO_OUTPUT_34   GND*/
    /*KEYSENSE_0_N*/
    {HS_RIGHT_K,        HS_UP_K,        HS_INFO_K,      HS_4_K,     HS_DOWN_K,      HS_LEFT_K,      HS_SEL_K,       HS_1_K,         HS_SEND_K},
     /*KEYSENSE_1_N*/
    {HS_CLR_K,          HS_LCTRL_K,     HS_SYMBOL_K,    HS_F2_K,    HS_SPACE_K,     HS_7_K,         HS_F1_K,        HS_0_K,         HS_SHIFT_K},
     /*KEYSENSE_2_N*/
    {HS_ENTER_K,        HS_F4_K,        HS_DOLLAR_K,    HS_M_K,     HS_N_K,         HS_B_K,         HS_9_K,         HS_8_K,         HS_Z_K},
     /*KEYSENSE_3_N*/
    {HS_F3_K,           HS_L_K,         HS_K_K,         HS_J_K,     HS_H_K,         HS_G_K,         HS_6_K,         HS_5_K,         HS_STAR_K},
     /*KEYSENSE_4_N*/
    {HS_P_K,            HS_O_K,         HS_I_K,         HS_U_K,     HS_Y_K,         HS_T_K,         HS_3_K,         HS_2_K,         HS_POUND_K}
};

#define ONE_KEY_ROW      0
#define ONE_KEY_COLUMN   7

/* '*' Key */
#define STAR_KEY_ROW     3
#define STAR_KEY_COLUMN  7

/* '#' Key */
#define POUND_KEY_ROW    2
#define POUND_KEY_COLUMN 6

#endif

/* 
 * Table positions of navigation keys 
 */
/* Nav-Up Key */
#define UP_KEY_ROW         0
#define UP_KEY_COLUMN      1

/* Nav-Down Key */
#define DOWN_KEY_ROW       0
#define DOWN_KEY_COLUMN    4

/* Nav-Left Key */
#define LEFT_KEY_ROW       0
#define LEFT_KEY_COLUMN    5

/* Nav-Right Key */
#define RIGHT_KEY_ROW      0
#define RIGHT_KEY_COLUMN   0

/* The order of the following array must match column assignments in the
 * key matrix mapping table. */
#endif /* BUILD_BOOT_CHAIN*/

static const GPIO_SignalType keytbl_col_to_keyscan_gpio_map[KEYPAD_COLUMNS] = 
{
   KYPD_9,
   KYPD_11,
   KYPD_15,
   KYPD_17,
   KYPD_MEMO,
   KYPD_EX1,
   KYPD_EX2,
   KYPD_EX3
   
};

/* The order of the following array must match row assignments in the
 * key matrix mapping table. */
static const GPIO_SignalType keytbl_row_to_keysense_gpio_map[KEYPAD_ROWS] = 
{
   KEYSENSE_0_N,
   KEYSENSE_1_N,
   KEYSENSE_2_N,
   KEYSENSE_3_N,
   KEYSENSE_4_N
};



#endif

/* Sanity check - we need rows/columns to be defined from here onward */


#else
#error code not present
#endif /* T_QSC60X5 */


/*---------------------------------------------------------------------------
      Target-specific macros and variables end.
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  Keypad polling macros, commands, definitions, and types
---------------------------------------------------------------------------*/

/* Interval at which the keypad is repeatedly polled */
#define KEYPAD_POLL_TIME 20

/* Interval between init and first poll */
#define KEYPAD_FIRST_POLL_TIME 3000

/* Interval after re-enabling polling before polling begins */
#define KEYPAD_RESUME_POLL_TIME 20

/* If amount of delay (in USEC) has not been defined, use default value of 1
 */
#ifndef KEYPAD_POLLING_DELAY_USEC
  #define KEYPAD_POLLING_DELAY_USEC 1
#endif

/*---------------------------------------------------------------------------
                    Keypad State and Key Buffer Data
---------------------------------------------------------------------------*/

/* Keypad-input Processing States
**
** While processing key inputs, we need to know what "state" we are
** in so that we can process events properly.
**
**  KS_NO_KEY     There is no key at this coordinate.
**
**  KS_KEY_UP     Key is in up position. When key is pressed, it is
**                change to KS_DEBOUNCE.
**
**  KS_DEBOUNCE   We have a key, delay for the debounce interval.
**                If the key is still pressed at the end of the interval,
**                go to KS_UP_WAIT. If not, go back to KS_KEY_UP.
**
**  KS_UP_WAIT    Report key press and wait for key to be up, then go
**                to KS_KEY_UP.
*/
typedef enum
{
  KS_NO_KEY,
  KS_KEY_UP,
#ifdef FEATURE_KEYPAD_DEBOUNCE
#error code not present
#endif /* FEATURE_KEYPAD_DEBOUNCE */
  KS_UP_WAIT
} keypad_state_type;

#ifndef BUILD_BOOT_CHAIN
/* current key processing state */

#ifdef FEATURE_ALL_KEY_PAD
static keypad_state_type keypad_key_state[KEYPAD_ROWS][KEYPAD_COLUMNS+1];
#else
static keypad_state_type keypad_key_state[KEYPAD_ROWS][KEYPAD_COLUMNS];
#endif
static keypad_state_type keypad_power_key_state;


/* Ring buffer for holding keycodes. It is used to hold keycode when callback
** has not been registered. When read index == write index, it is
** empty.  When read index is 1 ahead of write index, the buffer is full.
** Note:  this "burns" 1 datum (a byte) in order to tell full from empty.
*/
#define KEYPAD_KEY_BUF_SIZE 32             /* Max keycodes to buffer    */
#define KEYPAD_KEY_BUF_MASK 0x1f           /* Mask for modulus in ring  */

static struct
{
  byte rd_idx;                         /* read index                */
  byte wr_idx;                         /* write index               */
  byte key_code[KEYPAD_KEY_BUF_SIZE];  /* key code buffer           */
  byte key_parm[KEYPAD_KEY_BUF_SIZE];  /* key parm buffer           */
} keypad_key_buffer;                   /* keycode input ring buffer */


/* If debug is true, and callback is installed, key is also kept in the ring
** buffer
*/
static boolean keypad_debug = FALSE;

#ifndef FEATURE_IG_KEYPAD
/* Clock call-back structure used to register the keypad scanning routine
** to be called at regular intervals.
*/
static clk_cb_type keypad_clk_cb_keypad;
#endif /* NOT FEATURE_IG_KEYPAD */

static keypad_key_event_cb_f_type *keypad_event_cb = NULL;

/* If end key hold time equals power key duration, then power key
** pressed event is reported. Time in milli-seconds.
*/
static word keypad_power_key_hold_time;
static word keypad_power_key_duration;

/* Power key should only be passed once.
*/
static boolean keypad_power_key_passed = FALSE;

/* Keep track if the Handset is currently locked so the user cannot use it.
 * this functionality is used via the DM.
 */
static boolean keypad_locked = FALSE;       /* The keypad starts out unlocked */

#if defined(FEATURE_PMIC_HS_SEND_END_DETECT)
  boolean hsed_key_pressed = FALSE;
  boolean headset_is_present = FALSE;

  static keypad_state_type keypad_hsed_key_state;
  static keypad_state_type keypad_headset_state;
#endif /* FEATURE_PMIC_HS_SEND_END_DETECT */

#ifdef FEATURE_KEYPAD_OWNS_KEYPRESS_ISR
/* Keep track of whether keypad polling is enabled, or whether the keypress
 * detection ISR is installed.
 */
static boolean keypad_polling;
#endif /* FEATURE_KEYPAD_OWNS_KEYPRESS_ISR */
#ifdef FEATURE_ALL_KEY_PAD
static boolean keys_pressed[KEYPAD_ROWS][KEYPAD_COLUMNS+1];
#else
static boolean keys_pressed[KEYPAD_ROWS][KEYPAD_COLUMNS];
#endif

#if defined(T_SLEEP) && defined(FEATURE_NEW_SLEEP_API)
/* Variable to use the new sleep API */
char keypadDeviceName[4] = {"KPD"};
sleep_okts_handle keypad_sleep_handle;
#endif /* T_SLEEP && FEATURE_NEW_SLEEP_API */

static boolean keypad_initialized = FALSE;

/*-----------------------------------------------------------------------
                    External Device Interface Data
-----------------------------------------------------------------------*/


/*===========================================================================
  Forward Declarations
===========================================================================*/
static void keypad_drive_all_scan_cols_low(void);
static void keypad_drive_one_scan_col_low(int column);
static void keypad_scan_keypad(int4 ms_interval_unused);

/*===========================================================================

                                C  O  D  E

===========================================================================*/


#ifdef FEATURE_KEYPAD_OWNS_KEYPRESS_ISR
  #ifndef FEATURE_IG_KEYPAD
/*===========================================================================

FUNCTION KEYPAD_ENABLE_POLLING

DESCRIPTION
  Installs a keypad polling timer.
  Tell the sleep task that we are NOT in a state where we can sleep.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Uninstalls the keypress ISR, if installed.

===========================================================================*/

void keypad_enable_polling(void)
{
#ifdef T_SLEEP
  /* Let the sleep task know it is not ok to sleep
   */
  #ifdef FEATURE_NEW_SLEEP_API
  sleep_negate_okts(keypad_sleep_handle);
  #else
  (void) rex_clr_sigs( &sleep_tcb, SLEEP_KYPD_OKTS_SIG );
  #endif

  /* If polling is not currently enabled, then uninstall the keypress
   * detection ISR, and reenabled the keypad polling timer
   */
  INTLOCK();
  if ( !keypad_polling )
  {
    /* Clear the keypress detection ISR
    */
    tramp_set_isr( TRAMP_KEY_SENSE_ISR, NULL );

    keypad_polling = TRUE;

    /* Resume polling of the keypad.
    */
    clk_reg( &keypad_clk_cb_keypad, keypad_scan_keypad,
             KEYPAD_RESUME_POLL_TIME, KEYPAD_POLL_TIME, TRUE );
  }
  INTFREE();

#endif /* T_SLEEP */

} /* end of keypad_enable_polling */


/*===========================================================================

FUNCTION KEYPAD_KEYPRESS_ISR

DESCRIPTION
  Called when TRAMP detects a keypress

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  - turns on timer processing.
  - tells the sleep task that we are NOT in a state where we can sleep.
  - installs a keypad polling timer.

===========================================================================*/

static void keypad_keypress_isr( void )
{
  /* Ensure TCXO is enabled.
  */
  clk_unexpected_enable();

  /* If a keypress is detected, start polling keypad.
  */
  keypad_enable_polling();

} /* end of keypad_keypress_isr */


/*===========================================================================

FUNCTION KEYPAD_DISABLE_POLLING

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
  Keypad polling is turned off.
  A keypress ISR is installed in order to resume polling on a keypress.

===========================================================================*/

static void keypad_disable_polling()
{
  #ifdef T_SLEEP
  /* Let the sleep task know it is now ok to sleep
  */
  #ifdef FEATURE_NEW_SLEEP_API
    sleep_assert_okts(keypad_sleep_handle);
  #else
    (void) rex_set_sigs( &sleep_tcb, SLEEP_KYPD_OKTS_SIG );
  #endif


  /* If polling is currently enabled, disabled the keypad polling timer,
  ** and install the keypress detection ISR.
  */
  INTLOCK();
  if ( keypad_polling )
  {
    /* Uninstall the keypad polling timer
    */
    clk_dereg( &keypad_clk_cb_keypad );

    keypad_polling = FALSE;

    /* Install an ISR to detect when a key has been pressed
    */
    tramp_set_isr( TRAMP_KEY_SENSE_ISR, keypad_keypress_isr );
  }
  INTFREE();

  #endif /* T_SLEEP */

} /* end of keypad_disable_polling */

#endif /* NOT FEATURE_IG_KEYPAD */
#else /* FEATURE_KEYPAD_OWNS_KEYPRESS_ISR */
#error code not present
#endif /* !FEATURE_KEYPAD_OWNS_KEYPRESS_ISR */


/*===========================================================================

FUNCTION PASS_KEY_CODE

DESCRIPTION
  Pass a keycode from the Handset task to the receiver.  This involves
  making the callback if callback function is registered. Otherwise
  the keycode is queued on the ring buffer. It there is no room in the buffer,
  an error is logged.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void keypad_pass_key_code( byte key_code, byte key_parm )
{
  if ( keypad_event_cb )
  {
    keypad_key_event_type key_event;
    key_event.key_code = key_code;
    key_event.key_parm = key_parm;

    keypad_event_cb ( key_event );
    if ( keypad_debug )
    {
      keypad_key_buffer.key_code[keypad_key_buffer.wr_idx] = key_code;
      keypad_key_buffer.key_parm[keypad_key_buffer.wr_idx] = key_parm;
      keypad_key_buffer.wr_idx = (keypad_key_buffer.wr_idx+1) &
                                 KEYPAD_KEY_BUF_MASK;
    }
  }
  else
  {
    word isave;                   /* PSW parameter for INTLOCK/FREE_SAV */
    /* If the ring buffer is not full (full is when the write index is
    ** circularly 1 less than the read index), put the keycode into the
    ** ring buffer.
    */
    INTLOCK_SAV( isave );         /* Disable interrupts and save PSW */
    /* Queue it in the buffer and send it when a client registers */
    if ( ((keypad_key_buffer.wr_idx + 1) & KEYPAD_KEY_BUF_MASK) != keypad_key_buffer.rd_idx )
    {
      keypad_key_buffer.key_code[keypad_key_buffer.wr_idx] = key_code;
      keypad_key_buffer.key_parm[keypad_key_buffer.wr_idx] = key_parm;

      keypad_key_buffer.wr_idx = (keypad_key_buffer.wr_idx+1) & KEYPAD_KEY_BUF_MASK;
    }
    else
    {
      ERR( "Input buffer overflow (size %d)", KEYPAD_KEY_BUF_SIZE, 0, 0 );
    }
    INTFREE_SAV( isave );         /* Restore interrupts (PSW) */
  }
} /* end of pass_key_code */
#endif /*  BUILD_BOOT_CHAIN*/

/*===========================================================================

FUNCTION KEYPAD_DRIVE_ALL_SCAN_COLS_LOW

DESCRIPTION
  This function drives all of the scan column GPIOs low and ensures that
  they are set as outputs.

DEPENDENCIES
  The proper GPIO bank descriptions must be set up (above) so that the
  compile-time logic wiggles the right GPIOs.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void keypad_drive_all_scan_cols_low(void)
{
   int i;

  /* Drive all column scan lines low */
  for (i = 0; i < KEYPAD_COLUMNS; i++)
  {

     gpio_tristate(keytbl_col_to_keyscan_gpio_map[i], GPIO_TRISTATE_DISABLE);
     gpio_out(keytbl_col_to_keyscan_gpio_map[i], GPIO_LOW_VALUE);
  }
}

#ifdef FEATURE_ALL_KEY_PAD
static void keypad_drive_all_scan_cols_high(void)
{
   int i;

  /* Drive all column scan lines high */
  for (i = 0; i < KEYPAD_COLUMNS; i++)
  {
     gpio_out(keytbl_col_to_keyscan_gpio_map[i], GPIO_HIGH_VALUE);
     gpio_tristate(keytbl_col_to_keyscan_gpio_map[i], GPIO_TRISTATE_ENABLE);
  }
}
#endif

/*===========================================================================

FUNCTION KEYPAD_DRIVE_ONE_SCAN_COL_LOW

DESCRIPTION
  This function drives one scan column GPIO low (column parameter) and ensures
  that the rest of the GPIOs are hi-Z.

DEPENDENCIES
  The proper GPIO bank descriptions must be set up (above) so that the
  compile-time logic wiggles the right GPIOs.
  The passed-in column value must be < KEYPAD_COLUMNS, as defined above.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void keypad_drive_one_scan_col_low(int column)
{
  int i;

  /* Sanity check the range */
  if ( column < KEYPAD_COLUMNS )
  {
    /* Drive given column scan line low, all others tristate */
    for (i = 0; i < KEYPAD_COLUMNS; i++)
    {
      if (i == column)
      {
        /* Found given scan line, drive it low */
        gpio_tristate(keytbl_col_to_keyscan_gpio_map[i], 
                      GPIO_TRISTATE_DISABLE);
        gpio_out(keytbl_col_to_keyscan_gpio_map[i], GPIO_LOW_VALUE);
      }
      else
      {
        /* Tristate all other scan lines 
         *
         * Drive the signal high briefly before tristating the pin.
         * This avoids approx. 200ns floating rise time that might
         * occur on SC2x SURF due to 100kOhm pull-up in SURF FPGA. 
         * This rise time is likely not necessary on FFA, but it
         * shouldn't hurt. */
        gpio_out(keytbl_col_to_keyscan_gpio_map[i], GPIO_HIGH_VALUE);
        gpio_tristate(keytbl_col_to_keyscan_gpio_map[i], 
                      GPIO_TRISTATE_ENABLE);
      }
    }
  }
}

#ifndef BUILD_BOOT_CHAIN
#ifdef FEATURE_KEYPAD_USES_GPIO_FOR_PWR_KEY
#error code not present
#endif /* FEATURE_KEYPAD_USES_GPIO_FOR_PWR_KEY */


/*===========================================================================

FUNCTION KEYPAD_IS_POWER_KEY_PRESSED

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
boolean keypad_is_power_key_pressed(void)
{
#if defined(FEATURE_PMIC) && !defined (FEATURE_KEYPAD_USES_GPIO_FOR_PWR_KEY)
  /* If the PMIC is available then use the PMIC to find out if the
     Power Key is pressed or not */
  return(pm_is_power_key_pressed());
#else
  boolean pwr_key_status = FALSE;

  #ifndef FEATURE_IG_KEYPAD
  /* The PMIC is not available so the power key most be connected to the
     MSM. Read the corresponding GPIO */
    #ifdef FEATURE_KEYPAD_USES_GPIO_FOR_PWR_KEY
#error code not present
    #else
       if ( BIO_INM(BIO_ON_SW_SENSE_REG, BIO_ON_SW_SENSE_M) == BIO_ON_SW_ON_V )
       {
          /* The power key is pressed */
          pwr_key_status = TRUE;
       }
       else
       {
         /* The power key is not pressed */
         pwr_key_status = FALSE;
       }
    #endif /* FEATURE_KEYPAD_USES_GPIO_FOR_PWR_KEY */
  #endif /* FEATURE_IG_KEYPAD */
  return(pwr_key_status);
#endif
}


/*============================================================

LOCAL FUNCTION KEYPAD_SCAN_KEYPAD_MATRIX

DESCRIPTION
  This is a subroutine to perform keypad matrix hardware
  scanning.

  First drive all scan lines low, and see if the sense line indicates any key
  press. If there is key press, then drive one scan line (column) low at a
  time, with all other scan lines in tri-state mode, and read the sense lines
  (row). If a sense bit is 0, then the key at that row and column (driven low)
  is considered pressed.

  Sense all the keys that are pressed.

  Using a key as a vertex, we can draw many rectangles. For each rectangle,
  when three of the vertices are pressed, the forth is also sensed to be
  pressed, even if it was not. This is the limitation of hardware.

DEPENDENCIES
  This routine is called from interrupt context, and HS task cannot preempt
  it's access to the key buffer or the keypad_key_state variable.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void keypad_scan_keypad_matrix(void)
{
  int row, column;

  /* Keys are scanned by changing all except one column to tri-state,
   * and drive one column to low at a time, read the sense lines,
   * and look for 0 value in the sense lines. If 0 is found, then
   * the corresponding key is pressed.
   */

#ifdef FEATURE_ALL_KEY_PAD  
  keypad_drive_all_scan_cols_high();

  clk_busy_wait(KEYPAD_POLLING_DELAY_USEC);
  
  for ( row = 0; row < KEYPAD_ROWS; row++ )
  {
    if (GPIO_LOW_VALUE == gpio_in(keytbl_row_to_keysense_gpio_map[row]))
    {
       // A low keysense reading indicates a closed switch (pressed) 
       keys_pressed[row][KEYPAD_COLUMNS] = TRUE;
       keypad_drive_all_scan_cols_low();
       return;
    }
    else
    {
       //A high keysense reading indicates an open switch (unpressed) 
       keys_pressed[row][KEYPAD_COLUMNS] = FALSE;
    }
  }
#endif

  for ( column = 0; column < KEYPAD_COLUMNS; column++ )
  {
    /* Set the column LOW (0), the pressed key will show a LOW (0)
     * in it's position in the row being scanned.  This sets the other
     * columns HI-Z.
     */
    keypad_drive_one_scan_col_low(column);

    /* The following delays the polling of sense signals to make sure the
     * signals are stable. You may tweak the delay if you wish.
     */
    clk_busy_wait(KEYPAD_POLLING_DELAY_USEC);

    /* Scan across the rows looking for a key press
     */
    for ( row = 0; row < KEYPAD_ROWS; row++ )
    {
      if (GPIO_LOW_VALUE == gpio_in(keytbl_row_to_keysense_gpio_map[row]))
      {
         /* A low keysense reading indicates a closed switch (pressed) */
         keys_pressed[row][column] = TRUE;
      }
      else
      {
         /* A high keysense reading indicates an open switch (unpressed) */
         keys_pressed[row][column] = FALSE;
      }
    }
  }
 

  /* When done scanning, leave the columns as 0.  This costs ~0.05ma
   * during key presses, but allows for 0 setup when going to sleep.
   */
  keypad_drive_all_scan_cols_low();

  #if defined(T_QSC60X5)
  /* 
   * Convert any joystick signals to single keyscan code.  
   * See JoyStick info in keys[] init section for more info.
   */
#ifndef CUST_EDITION
  {
    boolean up     = keys_pressed[UP_KEY_ROW][UP_KEY_COLUMN]       && 
                     keys_pressed[LEFT_KEY_ROW][LEFT_KEY_COLUMN];
    boolean left   = keys_pressed[LEFT_KEY_ROW][LEFT_KEY_COLUMN]   && 
                     keys_pressed[DOWN_KEY_ROW][DOWN_KEY_COLUMN];
    boolean right  = keys_pressed[RIGHT_KEY_ROW][RIGHT_KEY_COLUMN] && 
                     keys_pressed[UP_KEY_ROW][UP_KEY_COLUMN];
    boolean down   = keys_pressed[DOWN_KEY_ROW][DOWN_KEY_COLUMN]   &&
                     keys_pressed[RIGHT_KEY_ROW][RIGHT_KEY_COLUMN];

    keys_pressed[UP_KEY_ROW][UP_KEY_COLUMN]       = up;
    keys_pressed[LEFT_KEY_ROW][LEFT_KEY_COLUMN]   = left;
    keys_pressed[RIGHT_KEY_ROW][RIGHT_KEY_COLUMN] = right;
    keys_pressed[DOWN_KEY_ROW][DOWN_KEY_COLUMN]   = down;
  }
#endif
  #endif /* T_QSC60X5 */

} /* end of keypad_scan_keypad_matrix */


/*===========================================================================

LOCAL FUNCTION KEYPAD_SCAN_KEYPAD

DESCRIPTION

  First drive all scan lines low, and see if the sense line indicates any key
  press. If there is key press, then drive one scan line (column) low at a
  time, with all other scan lines in tri-state mode, and read the sense lines
  (row). If a sense bit is 0, then the key at that row and column (driven low)
  is considered pressed.

  Sense all the keys that are pressed, then execute the state machine for each
  key.

  Using a key as a vertex, we can draw many rectangles. For each rectangle,
  when three of the vertices are pressed, the forth is also sensed to be
  pressed, even if it was not. This is the limitation of hardware.

DEPENDENCIES
  This routine is called from interrupt context, and HS task cannot preempt
  it's access to the key buffer or the keypad_key_state variable.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void keypad_scan_keypad
  (
  int4 ms_interval_unused
  /* parameter from Clock services -- ignored */
  /*lint -esym(715,ms_interval_unused)
  ** Have lint not complain about the ignored parameter which is
  ** specified to make this routine match the template for Clock.
  */
  )
{
  int row, column;
  boolean sleep_allowed = TRUE;
  boolean power_key_pressed = FALSE;

  /* If the keypad is locked, don't scan any keys.
  */
  if ( keypad_locked )
    return;

  if (!keypad_initialized)
  {
     keypad_init();
     return;
  }

#ifdef FEATURE_KEYPAD_OWNS_KEYPRESS_ISR
  keypad_enable_polling();        /* Keys being processed, no napping */
#else
#error code not present
#endif /* FEATURE_KEYPAD_OWNS_KEYPRESS_ISR */

  /* Scan matrix hardware and collect array of closed switches (pressed keys) */
  keypad_scan_keypad_matrix();

  /* Process the key based on the previous state
  */
  for (row=0; row < KEYPAD_ROWS; row++)
  {
#ifdef FEATURE_ALL_KEY_PAD 
    for (column=0; column < KEYPAD_COLUMNS+1; column++)
#else
	for (column=0; column < KEYPAD_COLUMNS; column++)
#endif	
    {
      switch ( keypad_key_state[row][column] )
      {

        /* No key here, do nothing.*/
        case KS_NO_KEY:
          break;

        /* If key was up, de-bounce it if key is now down.*/
        case KS_KEY_UP:
        #ifdef FEATURE_KEYPAD_DEBOUNCE
#error code not present
        #endif /* FEATURE_KEYPAD_DEBOUNCE */

          if ( keys_pressed[row][column] == TRUE )
          {
            KEYPAD_PASS_KEY_CODE( keys[row][column], HS_NONE_K );
            keypad_key_state[row][column] = KS_UP_WAIT;
            MSG_MED( "Debounced %d", (int)keys[row][column], 0, 0 );
            sleep_allowed = FALSE;
          }
          else
          {
            keypad_key_state[row][column] = KS_KEY_UP;
          }
          break;

        /* If waiting for key to be up */
        case KS_UP_WAIT:
          if ( keys_pressed[row][column] == TRUE )
          {
            sleep_allowed = FALSE;
          } else
          {
            keypad_key_state[row][column] = KS_KEY_UP;
            KEYPAD_PASS_KEY_CODE( HS_RELEASE_K, keys[row][column] );
            MSG_MED( "Key up %d", (int)keys[row][column], 0, 0 );
          }
          break;

        default:
          ERR_FATAL( "Illegal key state %d", keypad_key_state[row][column], 0, 0 );
          /* does not return */
      }
    }
  }
#ifdef CUST_EDITION
  /* Sample then process PWR/END key */
  if ( keypad_is_power_key_pressed() )
  {
    power_key_pressed = TRUE;
  }
  else
  {
    power_key_pressed = FALSE;
  }

  switch ( keypad_power_key_state )
  {
    /* If key was up, de-bounce it if key is now down.*/
    case KS_KEY_UP:
    #ifdef FEATURE_KEYPAD_DEBOUNCE
#error code not present
    #endif /* FEATURE_KEYPAD_DEBOUNCE */

      if ( power_key_pressed )
      {
      #ifdef FEATURE_COMBINED_PWR_END_KEYS
        KEYPAD_PASS_KEY_CODE( HS_END_K, HS_NONE_K );
      #else /* FEATURE_COMBINED_PWR_END_KEYS */
        KEYPAD_PASS_KEY_CODE( HS_PWR_K, HS_NONE_K );
      #endif /* FEATURE_COMBINED_PWR_END_KEYS */
        keypad_power_key_state = KS_UP_WAIT;
        keypad_power_key_passed = TRUE;
        keypad_power_key_hold_time = KEYPAD_POLL_TIME;
        MSG_MED( "Debounced %d", (int)HS_END_K, 0, 0 );
        sleep_allowed = FALSE;
      }
      else
      {
        keypad_power_key_state = KS_KEY_UP;
      }
      break;

      /* If waiting for key to be up
      */
    case KS_UP_WAIT:
      if ( power_key_pressed )
      {
        sleep_allowed = FALSE;
#ifndef CUST_EDITION
        if ( !keypad_power_key_passed )
        {
          keypad_power_key_hold_time += KEYPAD_POLL_TIME;
          if ( keypad_power_key_hold_time >= keypad_power_key_duration )
          {
            KEYPAD_PASS_KEY_CODE( HS_PWR_K, HS_NONE_K );
            keypad_power_key_passed = TRUE;
            /* Enable PMIC WDOG to allow a power down if PWR key is pressed. */
            pm_watchdog_reset_detect_switch(PM_OFF_CMD);

          }
        }
#endif
      }
      else
      {
        keypad_power_key_state = KS_KEY_UP;
      #ifdef FEATURE_COMBINED_PWR_END_KEYS
        KEYPAD_PASS_KEY_CODE( HS_RELEASE_K, HS_END_K );
      #else /* FEATURE_COMBINED_PWR_END_KEYS */
        KEYPAD_PASS_KEY_CODE( HS_RELEASE_K, HS_PWR_K );
      #endif /* FEATURE_COMBINED_PWR_END_KEYS */
        keypad_power_key_passed = FALSE;
        MSG_MED( "Key up %d", (int)HS_END_K, 0, 0 );
      }
      break;

    default:
      ERR_FATAL( "Illegal key state %d", keypad_power_key_state, 0, 0 );
      /* does not return */
  }
#endif // #ifndef CUST_EDITION

#if defined(FEATURE_PMIC_HS_SEND_END_DETECT)

  if (keypad_is_headset_present())
  {
    headset_is_present = TRUE;
  }
  else
  {
    headset_is_present = FALSE;
  }


  switch ( keypad_headset_state )
  {
    /* If key was up, de-bounce it if key is now down.*/
    case KS_KEY_UP:
    #ifdef FEATURE_KEYPAD_DEBOUNCE
#error code not present
    #endif /* FEATURE_KEYPAD_DEBOUNCE */
      if ( headset_is_present )
      {
        KEYPAD_PASS_KEY_CODE( HS_STEREO_HEADSET_K, HS_NONE_K );
        keypad_headset_state = KS_UP_WAIT;
        //hsed_key_hold_time = KEYPAD_POLL_TIME;
        MSG_MED( "Debounced %d", (int)HS_STEREO_HEADSET_K, 0, 0 );
        sleep_allowed = FALSE;
      }
      else
      {
        keypad_headset_state = KS_KEY_UP;
      }
      break;

    /* If waiting for key to be up
    */
    case KS_UP_WAIT:
      if ( headset_is_present )
      {
        sleep_allowed = FALSE;        
        //  hsed_key_hold_time += KEYPAD_POLL_TIME;
        //  if (hsed_key_hold_time >= hsed_key_duration)
        //  {
        //    KEYPAD_PASS_KEY_CODE( HS_HSED_K, HS_NONE_K );
        //  }        
      }
      else
      {
        keypad_headset_state = KS_KEY_UP;
        KEYPAD_PASS_KEY_CODE( HS_RELEASE_K, HS_STEREO_HEADSET_K );
        MSG_MED( "Key up", HS_STEREO_HEADSET_K, 0, 0 );
      }
      break;

    default:
      /* change here to recover */
      ERR_FATAL( "Illegal key state %d", keypad_headset_state, 0, 0 );
      /* does not return */
  }


  if (keypad_is_headset_key_pressed())
  {
    hsed_key_pressed = TRUE;
  }
  else
  {
    hsed_key_pressed = FALSE;
  }

  switch ( keypad_hsed_key_state )
  {
    /* If key was up, de-bounce it if key is now down.
    */
    case KS_KEY_UP:
#ifdef FEATURE_KEYPAD_DEBOUNCE
#error code not present
#endif /* FEATURE_KEYPAD_DEBOUNCE */
      if ( hsed_key_pressed )
      {
        KEYPAD_PASS_KEY_CODE( HS_HEADSET_SWITCH_K, HS_NONE_K );

        keypad_hsed_key_state = KS_UP_WAIT;
        //hsed_key_hold_time = KEYPAD_POLL_TIME;
        MSG_MED( "Debounced %d", (int)HS_HSED_K, 0, 0 );
        sleep_allowed = FALSE;
      }
      else
      {
        keypad_hsed_key_state = KS_KEY_UP;
      }
      break;

    /* If waiting for key to be up
    */
    case KS_UP_WAIT:
      if ( hsed_key_pressed )
      {
        sleep_allowed = FALSE;        
        //  hsed_key_hold_time += KEYPAD_POLL_TIME;
        //  if (hsed_key_hold_time >= hsed_key_duration)
        //  {
        //    KEYPAD_PASS_KEY_CODE( HS_HEADSET_SWITCH_K, HS_NONE_K );
        //    keypad_hsed_key_cleared();
        //  }        
      }
      else
      {
        keypad_hsed_key_state = KS_KEY_UP;
        
        KEYPAD_PASS_KEY_CODE( HS_RELEASE_K, HS_HEADSET_SWITCH_K );
        MSG_MED( "Key up", HS_HSED_K, 0, 0 );
      }
      break;

    default:
      /* change here to recover */
      ERR_FATAL( "Illegal key state %d", keypad_hsed_key_state, 0, 0 );
      /* does not return */
  }

#endif /* FEATURE_PMIC_HS_SEND_END_DETECT */

  if ( keypad_key_buffer.wr_idx != keypad_key_buffer.rd_idx )
  {
    sleep_allowed = FALSE;
  }

  if ( sleep_allowed )
  {
#ifdef FEATURE_KEYPAD_OWNS_KEYPRESS_ISR
    keypad_disable_polling();
#else
#error code not present
#endif /* FEATURE_KEYPAD_OWNS_KEYPRESS_ISR */
  }
} /* end of keypad_scan_keypad */


/*===========================================================================

FUNCTION KEYPAD_SIMULATE_KEY

DESCRIPTION
  This procedure simulates a key entry and key release.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void keypad_simulate_key( byte keycode)
{
  KEYPAD_PASS_KEY_CODE(keycode, HS_NONE_K);
}


/*===========================================================================

FUNCTION KEYPAD_LOCK_KEYPAD

DESCRIPTION
  This procedure locks the keypad.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void keypad_lock_keypad( boolean lock )
{
  word isave, i, j;

  keypad_locked = lock;

  /* If Locking the keypad, close out the previous key state by
  ** stuffing in a key-up if needed
  */
  if ( keypad_locked )
  {
    INTLOCK_SAV( isave );            /* Disable interrupts, save PSW  */

    for ( i=0; i<KEYPAD_ROWS; i++ )
    {
      for ( j=0; j<KEYPAD_COLUMNS; j++ )
      {
        /* If key is waiting for UP, simulate key up */
        if ( keypad_key_state[i][j] == KS_UP_WAIT )
        {
          KEYPAD_PASS_KEY_CODE( HS_RELEASE_K, keys[i][j] );
        }
        /* If position has key, change state to key up */
        if ( keypad_key_state[i][j] != KS_NO_KEY )
        {
          keypad_key_state[i][j] = KS_KEY_UP;
        }
      }
    }
  #ifdef FEATURE_KEYPAD_OWNS_KEYPRESS_ISR
    keypad_disable_polling();            /* No keys waited on, sleep now  */
  #else
#error code not present
  #endif /* FEATURE_KEYPAD_OWNS_KEYPRESS_ISR */

    INTFREE_SAV( isave );            /* Restore interrupts (PSW)      */
  }
}


/*===========================================================================

FUNCTION KEYPAD_GET_KEY

DESCRIPTION
  Read a keycode from the input buffer.

DEPENDENCIES
  None

RETURN VALUE
  This procedure returns the oldest unread keycode.  If no keycode
  is currently available, HS_NONE_K is returned.

SIDE EFFECTS
  None

===========================================================================*/
keypad_key_event_type keypad_get_key( void )
{
  keypad_key_event_type key_event;

  if ( keypad_key_buffer.wr_idx == keypad_key_buffer.rd_idx )
  {
    /* buffer is empty */
    key_event.key_code = (byte)HS_NONE_K;
    key_event.key_parm = NULL;
  }
  else
  {
    /* get code & increment ring read index */
    key_event.key_code = keypad_key_buffer.key_code[keypad_key_buffer.rd_idx];
    key_event.key_parm = keypad_key_buffer.key_parm[keypad_key_buffer.rd_idx];
    keypad_key_buffer.rd_idx = (keypad_key_buffer.rd_idx+1) & KEYPAD_KEY_BUF_MASK;
  }
  return( key_event );
} /* end of keypad_key_get */


/*===========================================================================

FUNCTION KEYPAD_SET_POWER_KEY_DURATION

DESCRIPTION
  This procedure set the power key duration in milli-seconds. When END key
  is pressed for over this duration, HS_PWR_K pressed event is generated.
  Default duration is KEYPAD_DEFAULT_POWER_KEY_DURATION msec.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void keypad_set_power_key_duration( word duration)
{
  keypad_power_key_duration = duration;
}


/*===========================================================================

FUNCTION KEYPAD_SET_BACKLIGHT

DESCRIPTION
  This procedure sets the back light level. If level is 0, back light is
  turned off.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void keypad_set_backlight( byte level )
{
#ifndef FEATURE_IG_KEYPAD
  #ifdef FEATURE_PMIC_LCDKBD_LED_DRIVER
  // Assume failure
  pm_err_flag_type pefRet = (pm_err_flag_type)~PM_ERR_FLAG__SUCCESS;

  pefRet = pm_set_led_intensity(PM_KBD_LED, (uint8)level);
  if (PM_ERR_FLAG__SUCCESS != pefRet)
  {
    MSG_MED("PMIC KBD backlight set failed: pefRet=%d, level=%d", 
             pefRet, level, 0);
  }
  #else
  if ( level )
  {
    gpio_out(GPIO_BACKLIGHT, GPIO_HIGH_VALUE);
  }
  else
  {
    gpio_out(GPIO_BACKLIGHT, GPIO_LOW_VALUE);
  }
  #endif /* FEATURE_PMIC_LCDKBD_LED_DRIVER */
#endif /* NOT FEATURE_IG_KEYPAD */
}

/*===========================================================================

FUNCTION KEYPAD_INIT_IFACE

DESCRIPTION
  This procedure initializes the keypad physical interface.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  All keypad matrix scan lines are driven low.

===========================================================================*/
void keypad_init_iface( void )
{
  /* Make all the column scan lines outputs and drive them low */
  keypad_drive_all_scan_cols_low();
}

/*===========================================================================

FUNCTION KEYPAD_INIT_MATRIX

DESCRIPTION
  This procedure initializes basic keypad resources required to scan the 
  keypad matrix. It does not initialize PMIC, clock, or sleep resources.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Keypad related globals are initialized and all matrix scan lines are
  driven low.

===========================================================================*/
void keypad_init_matrix( void )
{
  int i, j;

  /* Initialize the keypad physical interface (i.e. make all the column scan
   * lines outputs and drive them low */
  keypad_init_iface();

#ifdef FEATURE_COMBINED_PWR_END_KEYS
  keypad_power_key_duration = KEYPAD_DEFAULT_POWER_KEY_DURATION;
#else /* FEATURE_COMBINED_PWR_END_KEYS */
  /* If PWR and END keys are not combined, pressing PWR key will deliver
  ** HS_PWR_K right away.
  */
  keypad_power_key_duration = 0;
#endif /* FEATURE_COMBINED_PWR_END_KEYS */

  /* For a coordinate that has no key, init key state to KS_NO_KEY.
   * All other keys are in KS_KEY_UP state.
   * Initialize end key row and column variables. End key is not
   * processed using keys matrix, so make sure it says KS_NO_KEY
   */
  for ( i = 0; i < KEYPAD_ROWS; i++ )
  {
#ifdef FEATURE_ALL_KEY_PAD
    for ( j = 0; j < KEYPAD_COLUMNS+1; j++ )
#else
	for ( j = 0; j < KEYPAD_COLUMNS; j++ )
#endif
    {
      if ( keys[i][j] == HS_NONE_K )
      {
        keypad_key_state[i][j] = KS_NO_KEY;
      }
      else
      {
        keypad_key_state[i][j] = KS_KEY_UP;
      }
    }
  }

  /* END key has its own state variable */
  keypad_power_key_state = KS_KEY_UP;

  /* Initialize the key buffer */
  keypad_key_buffer.rd_idx = 0;
  keypad_key_buffer.wr_idx = 0;

  /* Init state of signals we have to look for edges on
   */
  keypad_locked = FALSE;
}

/*===========================================================================

FUNCTION KEYPAD_INIT

DESCRIPTION
  This procedure initializes the entire keypad driver.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void keypad_init( void )
{
  /* keypad_initialized variable keeps tracks of whether the keypad module
   * has been initialized or not and is then checked to make sure that we
   * do not try to initialize it multiple times
   */

  /* To save locking interrupts un-necessarily, check keypad_initialzed flag
   * before locking the interrupt. Note that we do not depend on this check
   * to decide if we need to initialize or not. After locking the interrupt
   * this flag is checked again in an atomic way to determine if we need to
   * to initialize.
   */
  if(FALSE == keypad_initialized)
  {
    /* Since init routine can be called from multiple places by multiple
     * entities, make sure that the routine is atomic. Since the tasks are
     * just initializing, TASKLOCK() will suffice. If the assumption that
     * none of the interrupts will fire durign this time is not valid, using
     * INTLOCK() is recommended.*/
    INTLOCK();

    if (FALSE == keypad_initialized)
    {
  
      /* Initialize basic keypad matrix scanning resources including data 
       * structures and the physical interface (i.e. make all the column scan
       * lines outputs and drive them low) */
      keypad_init_matrix();

    #if defined(FEATURE_PMIC) && !defined (FEATURE_KEYPAD_USES_GPIO_FOR_PWR_KEY)
      /* Initialize the Power Key ISR */
      pm_pwr_key_init();

      #if defined(FEATURE_PMIC_HS_SEND_END_DETECT)
        keypad_hsed_key_state = KS_KEY_UP;
        keypad_headset_state = KS_KEY_UP;
      #endif /* FEATURE_PMIC_HS_SEND_END_DETECT */

    #endif

    #ifndef FEATURE_IG_KEYPAD
      /* Register the keypad scanning routine with the Clock services,
       * so that it is called at regular intervals to poll the keypad.
       */
      clk_def( &keypad_clk_cb_keypad );       /* Init call-back */
    #endif /* NOT FEATURE_IG_KEYPAD */

    #ifdef FEATURE_KEYPAD_OWNS_KEYPRESS_ISR
      keypad_polling = TRUE;
    #endif /* FEATURE_KEYPAD_OWNS_KEYPRESS_ISR */

    #ifndef FEATURE_IG_KEYPAD
      clk_reg( &keypad_clk_cb_keypad, keypad_scan_keypad,
               KEYPAD_FIRST_POLL_TIME, KEYPAD_POLL_TIME, TRUE );
    #endif /* NOT FEATURE_IG_KEYPAD */

  #ifdef T_SLEEP
    #ifdef FEATURE_NEW_SLEEP_API
      keypad_sleep_handle = sleep_register(keypadDeviceName, FALSE);
      if (0 == keypad_sleep_handle)
      {
        MSG_ERROR("%s failed sleep_register", keypadDeviceName, 0, 0);
     }
    #endif
  #endif

      keypad_initialized = TRUE;
    } // if (FLASE == ...)

    /* Initialization is complete. Allow being scheduled out */
    INTFREE();
  }
  return;
}


/*===========================================================================

FUNCTION KEYPAD_REGISTER

DESCRIPTION
  This procedure registers the keypad callback function.
  Call this function with a function pointer to register.
  Call this function with NULL pointer to de-register.
  Only one client is supported.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if register successfully
  FALSE if keypad service was already registered.

SIDE EFFECTS
  None

===========================================================================*/
boolean keypad_register( keypad_key_event_cb_f_type *cb)
{
  if (cb)
  {
    /* this is an attempt to register a new callback */
    if ( keypad_event_cb ) return(FALSE);
    keypad_event_cb = cb;
    return(TRUE);
  }
  else
  {
    /* this is an attempt to de-register a callback, it always succeeds */
    keypad_event_cb = NULL;
    return(TRUE);
  }
}

/*===========================================================================

FUNCTION KEYPAD_IS_PANIC_CHORD_PRESSED

DESCRIPTION
  This function returns if the panic key chord is pressed.

DEPENDENCIES

RETURN VALUE
  Boolean: TRUE,    panic chord is pressed.
           FALSE    panic chord is not pressed.

SIDE EFFECTS
  None

===========================================================================*/
boolean keypad_is_panic_chord_pressed(void)
{
  boolean panic_chord_pressed = FALSE;

  keypad_init();

#ifdef FEATURE_KEYPAD_OWNS_KEYPRESS_ISR
  keypad_enable_polling();        /* Keys being processed, no napping */
#else
#error code not present
#endif /* FEATURE_KEYPAD_OWNS_KEYPRESS_ISR */

  /* Scan matrix hardware and collect array of closed switches (pressed keys) */
  keypad_scan_keypad_matrix();

  /* the panic key chord is the combination of the 1 key, the star key, and
     the pound key */
  if ( keys_pressed[ONE_KEY_ROW][ONE_KEY_COLUMN]     == TRUE && /* "1" key */
       keys_pressed[STAR_KEY_ROW][STAR_KEY_COLUMN]   == TRUE && /* "*" key */
       keys_pressed[POUND_KEY_ROW][POUND_KEY_COLUMN] == TRUE )  /* "#" key */
  {
    panic_chord_pressed = TRUE;
  }

#ifdef FEATURE_KEYPAD_OWNS_KEYPRESS_ISR
  keypad_disable_polling();
#else
#error code not present
#endif /* FEATURE_KEYPAD_OWNS_KEYPRESS_ISR */

  return(panic_chord_pressed);
}

/*===========================================================================

FUNCTION KEYPAD_IS_KEY_PRESSED

DESCRIPTION
  This function returns if the specified key is pressed.

DEPENDENCIES
  Keypad initialization is expected to have been performed prior to calling
  this routine. Keypad init is not required for calls from boot since that
  functionality does not need the full capability of the keypad driver.

RETURN VALUE
  Boolean: TRUE,    key is pressed.
           FALSE    key is not pressed.

SIDE EFFECTS
  Sleep control is not manipulated by this routine.

===========================================================================*/
boolean keypad_is_key_pressed(hs_key_type key)
{
  boolean key_pressed = FALSE;

  /* Scan matrix hardware and collect array of closed switches (pressed keys) */
  keypad_scan_keypad_matrix();

  /* To limit code space footprint, only necessary keys are supported 
   * in the following switch statement */
  switch (key)
  {
     case HS_STAR_K:
        key_pressed = keys_pressed[STAR_KEY_ROW][STAR_KEY_COLUMN];
        break;

     case HS_POUND_K:
        key_pressed = keys_pressed[POUND_KEY_ROW][POUND_KEY_COLUMN];
        break;
        
#ifdef CUST_EDITION
     case HS_DOWN_K:
        key_pressed = keys_pressed[DOWN_KEY_ROW][DOWN_KEY_COLUMN];
        break;
#endif
     default:
        key_pressed = FALSE;
        break;
  }

  return(key_pressed);
}

#ifdef FEATURE_PMIC_HS_SEND_END_DETECT
/*===========================================================================

FUNCTION KEYPAD_IS_HEADSET_KEY_PRESSED

DESCRIPTION
  This function returns the status of the hsed key.

DEPENDENCIES
  If FEATURE_PMIC and FEATURE_PMIC_HS_SEND_END_DETECT is defined then pm_init
  must have been called.

RETURN VALUE
  Boolean: TRUE,    HSED key is pressed.
           FALSE    HSED key is not pressed.

SIDE EFFECTS
  None

===========================================================================*/
boolean keypad_is_headset_key_pressed(void)
{
  /* If the PMIC is available then use the PMIC to find out if the
   * HSED Key is pressed or not */
  return pm_hsed_get_key_pressed();
}

/*===========================================================================

FUNCTION KEYPAD_IS_HEADSET_PRESENT

DESCRIPTION
  This function queries headset presence.

DEPENDENCIES
  If FEATURE_PMIC and FEATURE_PMIC_HS_SEND_END_DETECT is defined then pm_init
  must have been called.

RETURN VALUE
  Boolean: TRUE,    HEADSET is present.
           FALSE    HEADSET is not present.

SIDE EFFECTS
  None

===========================================================================*/
boolean keypad_is_headset_present(void)
{
#ifdef FEATURE_PMIC_HSED_PMIC4
  /* If the PMIC is available then use the PMIC to find out if the
   * Headset is inserted. */
  return pm_hsed_get_inserted();
#else
  return FALSE;
#endif
}


/*===========================================================================

FUNCTION KEYPAD_HEADSET_KEY_CLEARED

DESCRIPTION
  This function clears status of the hsed key after Keypad scan has read it.

DEPENDENCIES
  If FEATURE_PMIC and FEATURE_PMIC_HS_SEND_END_DETECT is defined thenhsed_init
  must have been called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void keypad_clear_headset_key(void)
{
  /* Use the PMIC to clear the HSED key status */
  pm_hsed_clear_key_pressed();
}
#endif /* FEATURE_PMIC_HS_SEND_END_DETECT */ 
#endif /* BUILD_BOOT_CHAIN*/



#ifdef CUST_EDITION

#ifdef FEATURE_ALL_KEY_PAD
#define DLOAD_KEY_ROW  0
#define DLOAD_KEY_COL  0
#else
#define DLOAD_KEY_ROW  3
#define DLOAD_KEY_COL  0
#endif

boolean keypad_is_dload_key_pressed(void)
{
    boolean bPressed = FALSE;
    keypad_drive_one_scan_col_low(DLOAD_KEY_COL);
    
    /* The following delays the polling of sense signals to make sure the
     * signals are stable. You may tweak the delay if you wish.
     */
    clk_busy_wait(KEYPAD_POLLING_DELAY_USEC);
 
    if (GPIO_LOW_VALUE == gpio_in(keytbl_row_to_keysense_gpio_map[DLOAD_KEY_ROW]))
    {
         /* A low keysense reading indicates a closed switch (pressed) */
        bPressed = TRUE;
    }

    keypad_drive_all_scan_cols_low();
    return bPressed;
}
#endif


