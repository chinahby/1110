/*=============================================================================

FILE: OEMNV.c

SERVICES: OEM Functions for Non Volatile memory access.

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:

   OEMNV_Get
   OEMNV_Put

INITIALIZATION AND SEQUENCING REQUIREMENTS:
No additional module-level initialization or sequencing requirements.

        Copyright c 2003-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
/*=============================================================================

$Header: //depot/asic/msmshared/apps/StaticExtensions/OEM/Src/OEMNV.c#22 $
$DateTime: 2008/11/24 23:27:55 $

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ---------------------------------------------------------
07/03/08     yb      Added CFGI_TOOLKIT_CS_PS_PARALLEL
09/28/07     clm     Fixed the default value for nonactive NV items 
                     NV_NITZ_NW_INFO_MCC_I and NV_NITZ_NW_INFO_MNC_I support.
04/04/07     cvs     Default backlight to always on
11/22/06     jas     Reverting prior change, expect another long-term solution
11/21/06     ak      Secured GPSOne NV items from access
10/19/06     jas     Updating comments to reflect header file changes
07/13/06     cvs     Fix TTY init
06/28/06     jas     Featurizing to work with 6275 on BREW 3.1.4
06/13/06     JD      Added support for CFGI_VOICEPRIVACY
06/12/06     jks     Ported IUsrSysClock to new BREW ISYSCLOCK interface
12/6/05      sj      Fixed the default value for nonactive NV item GSM_AMR_CALL_CONFIG
10/26/05     RI      Added NITZ PLMN info
11/11/04     SUN     Fixed default value of One touch Dial
10/12/04      as     Added code to prevent access to security related NV items.
06/02/03     PMD     Added Default Case for SMS_VM_NUMBER
05/14/03     kar     Removed sections of stripped code
03/11/03     kar     Changed new to newPage
02/01/03      lz     Fixed order of include files (dmss then
                     OEM then AEE Files).
01/18/03     kar     Initial revision
=============================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "ui.h"
#include "uixsnd.h"

#include "OEMNV.h"

#include "AEE.h"
#include "AEEStdLib.h"
#include "cm.h"

/*===========================================================================

                    LOCAL/STATIC DATA

===========================================================================*/

// GSM character set to 8-bit ASCII conversation table.
//
// Note that the gsm2ASCII[27] character is followed by a
// second byte, which is a continuation of that character
// and may be decoded using the gsmExtTable[] table below
//
static const byte gsm2ASCII[128] =
{
   64,  // COMMERCIAL AT @ 
   163, // POUND SIGN £
   36,  // DOLLAR SIGN $
   165, // YEN SIGN ¥
   232, // 0x04 4 LATIN SMALL LETTER E WITH GRAVE è
   233, // LATIN SMALL LETTER E WITH ACUTE é
   250, // LATIN SMALL LETTER U WITH GRAVE ú
   236, // LATIN SMALL LETTER I WITH GRAVE ì
   242, // LATIN SMALL LETTER O WITH GRAVE ò
   199, // LATIN CAPITAL LETTER C WITH CEDILLA Ç
   10,  // LINE FEED 
   216, // LATIN CAPITAL LETTER O WITH STROKE Ø
   248, // LATIN SMALL LETTER O WITH STROKE ø
   13,  // CARRIAGE RETURN 
   197, // LATIN CAPITAL LETTER A WITH RING ABOVE Å 
   229, // 0x0F 15 LATIN SMALL LETTER A WITH RING ABOVE å 229 
   '?', // GREEK CAPITAL LETTER DELTA
   95,  //  LOW LINE _
   '?', // GREEK CAPITAL LETTER PHI
   '?', // GREEK CAPITAL LETTER GAMMA
   '?', // GREEK CAPITAL LETTER LAMBDA
   '?', // GREEK CAPITAL LETTER OMEGA
   '?', // GREEK CAPITAL LETTER PI
   '?', // GREEK CAPITAL LETTER PSI
   '?', // GREEK CAPITAL LETTER SIGMA
   '?', // GREEK CAPITAL LETTER THETA
   '?', // GREEK CAPITAL LETTER XI

   27,   // ESCAPE TO EXTENSION TABLE:
         // 27  10   FORM FEED  12 
         // 27  20   CIRCUMFLEX ACCENT ^ 94 
         // 27  40   LEFT CURLY BRACKET { 123 
         // 27  41   RIGHT CURLY BRACKET } 125 
         // 27  47   REVERSE SOLIDUS (BACKSLASH) \ 92 
         // 27  60   LEFT SQUARE BRACKET [ 91 
         // 27  61   TILDE ~ 126 
         // 27  62   RIGHT SQUARE BRACKET ] 93 
         // 27  64   VERTICAL BAR | 124 
         // 27 101   EURO SIGN € 164 (ISO-8859-15) 

   192, // LATIN CAPITAL LETTER AE Æ
   230, // LATIN SMALL LETTER AE æ 230 
   223, // LATIN SMALL LETTER SHARP S (German) ß
   202, // LATIN CAPITAL LETTER E WITH ACUTE Ê

   32,  // SPACE
   33,  // EXCLAMATION MARK
   34,  // QUOTATION MARK
   35,  // NUMBER SIGN

   164, // CURRENCY SIGN ¤

   37, // PERCENT SIGN %
   38, // AMPERSAND &
   39, // APOSTROPHE '
   40, // LEFT PARENTHESIS (
   41, // RIGHT PARENTHESIS )
   42, // ASTERISK *
   43, // PLUS SIGN +
   44, // COMMA ,
   45, // HYPHEN-MINUS -
   46, // FULL STOP .
   47, // SOLIDUS (SLASH) /
   48, // DIGIT ZERO 0
   49, // DIGIT ONE 1
   50, // DIGIT TWO 2
   51, // DIGIT THREE 3
   52, // DIGIT FOUR 4
   53, // DIGIT FIVE 5
   54, // DIGIT SIX 6
   55, // DIGIT SEVEN 7
   56, // DIGIT EIGHT 8
   57, // DIGIT NINE 9
   58, // COLON :
   59, // SEMICOLON ;
   60, // LESS-THAN SIGN <
   61, // EQUALS SIGN =
   62, // GREATER-THAN SIGN >
   63, // QUESTION MARK ? 

   161, // INVERTED EXCLAMATION MARK ¡

   65, // LATIN CAPITAL LETTER A 
   66, // LATIN CAPITAL LETTER B 
   67, // LATIN CAPITAL LETTER C 
   68, // LATIN CAPITAL LETTER D 
   69, // LATIN CAPITAL LETTER E 
   70, // LATIN CAPITAL LETTER F 
   71, // LATIN CAPITAL LETTER G 
   72, // LATIN CAPITAL LETTER H 
   73, // LATIN CAPITAL LETTER I 
   74, // LATIN CAPITAL LETTER J 
   75, // LATIN CAPITAL LETTER K 
   76, // LATIN CAPITAL LETTER L 
   77, // LATIN CAPITAL LETTER M 
   78, // LATIN CAPITAL LETTER N 
   79, // LATIN CAPITAL LETTER O 
   80, // LATIN CAPITAL LETTER P 
   81, // LATIN CAPITAL LETTER Q 
   82, // LATIN CAPITAL LETTER R 
   83, // LATIN CAPITAL LETTER S 
   84, // LATIN CAPITAL LETTER T 
   85, // LATIN CAPITAL LETTER U 
   86, // LATIN CAPITAL LETTER V 
   87, // LATIN CAPITAL LETTER W 
   88, // LATIN CAPITAL LETTER X 
   89, // LATIN CAPITAL LETTER Y 
   90, // LATIN CAPITAL LETTER Z

   196, // LATIN CAPITAL LETTER A WITH DIAERESIS Ä
   214, // LATIN CAPITAL LETTER O WITH DIAERESIS Ö
   209, // LATIN CAPITAL LETTER N WITH TILDE Ñ
   220, // LATIN CAPITAL LETTER U WITH DIAERESIS Ü
   167, // SECTION SIGN §
   191, // INVERTED QUESTION MARK ¿

   97, // LATIN SMALL LETTER A 
   98, // LATIN SMALL LETTER B 
   99, // LATIN SMALL LETTER C 
   100, // LATIN SMALL LETTER D 
   101, // LATIN SMALL LETTER E 
   102, // LATIN SMALL LETTER F 
   103, // LATIN SMALL LETTER G 
   104, // LATIN SMALL LETTER H 
   105, // LATIN SMALL LETTER I 
   106, // LATIN SMALL LETTER J 
   107, // LATIN SMALL LETTER K 
   108, // LATIN SMALL LETTER L 
   109, // LATIN SMALL LETTER M 
   110, // LATIN SMALL LETTER N 
   111, // LATIN SMALL LETTER O 
   112, // LATIN SMALL LETTER P 
   113, // LATIN SMALL LETTER Q 
   114, // LATIN SMALL LETTER R 
   115, // LATIN SMALL LETTER S 
   116, // LATIN SMALL LETTER T 
   117, // LATIN SMALL LETTER U 
   118, // LATIN SMALL LETTER V 
   119, // LATIN SMALL LETTER W 
   120, // LATIN SMALL LETTER X 
   121, // LATIN SMALL LETTER Y 
   122, // LATIN SMALL LETTER Z 

   228, // LATIN SMALL LETTER A WITH DIAERESIS ä
   246, // LATIN SMALL LETTER O WITH DIAERESIS ö 
   241, // LATIN SMALL LETTER N WITH TILDE ñ 
   252, // LATIN SMALL LETTER U WITH DIAERESIS ü
   224, // LATIN SMALL LETTER A WITH GRAVE à 
};


// The second byte of the GSM Extension Table.  This table
// only contains the characters this mobile supports
static const struct {
   byte gsmChar;
   byte asciiChar;
} gsmExtTable[] = {
                     {20, '^'},
                     {40, '{'},
                     {41, '}'},
                     {47, '\\'},
                     {60, '['},
                     {61, '~'},
                     {62, ']'},
                     {64, '|'}
                  };


// 7-bit ASCII to GSM character set conversation table.
//
// Characters not supported by GSM are encoded as a space(32).
//
// If ascii2GSM[i] equals 27, the GSM character consists of two bytes.
// The first byte is 27 and the second byte is encoded using 
// the gsmExtTable[] table
static const byte ascii2GSM[128] =
{
   32,  // NUL
   32,  // SOH 
   32,  // STX
   32,  // ETX
   32,  // EOT
   32,  // ENQ
   32,  // ACK
   32,  // BELL
   32,  // BS
   32,  // HT
   10,  // LINE FEED 
   32,  // VT 
   32,  // LF 
   13,  // CARRIAGE RETURN 
   32,  // SO
   32,  // SI
   32,  // DLE
   32,  // DC1
   32,  // DC2 
   32,  // DC3
   32,  // DC4
   32,  // NAK
   32,  // SYN 
   32,  // ETB
   32,  // CAN
   32,  // EM
   32,  // SUB
   32,  // ESC
   32,  // FS
   32,  // GS
   32,  // RS
   32,  // US

   32,  // SPACE
   33,  // EXCLAMATION MARK
   34,  // QUOTATION MARK
   35,  // NUMBER SIGN
   2,   // DOLLAR SIGN 
   37, // PERCENT SIGN %
   38, // AMPERSAND &
   39, // APOSTROPHE '
   40, // LEFT PARENTHESIS (
   41, // RIGHT PARENTHESIS )
   42, // ASTERISK *
   43, // PLUS SIGN +
   44, // COMMA ,
   45, // HYPHEN-MINUS -
   46, // FULL STOP .
   47, // SOLIDUS (SLASH) /
   48, // DIGIT ZERO 0
   49, // DIGIT ONE 1
   50, // DIGIT TWO 2
   51, // DIGIT THREE 3
   52, // DIGIT FOUR 4
   53, // DIGIT FIVE 5
   54, // DIGIT SIX 6
   55, // DIGIT SEVEN 7
   56, // DIGIT EIGHT 8
   57, // DIGIT NINE 9
   58, // COLON :
   59, // SEMICOLON ;
   60, // LESS-THAN SIGN <
   61, // EQUALS SIGN =
   62, // GREATER-THAN SIGN >
   63, // QUESTION MARK ? 

   0,  // COMMERICAL AT @

   65, // LATIN CAPITAL LETTER A 
   66, // LATIN CAPITAL LETTER B 
   67, // LATIN CAPITAL LETTER C 
   68, // LATIN CAPITAL LETTER D 
   69, // LATIN CAPITAL LETTER E 
   70, // LATIN CAPITAL LETTER F 
   71, // LATIN CAPITAL LETTER G 
   72, // LATIN CAPITAL LETTER H 
   73, // LATIN CAPITAL LETTER I 
   74, // LATIN CAPITAL LETTER J 
   75, // LATIN CAPITAL LETTER K 
   76, // LATIN CAPITAL LETTER L 
   77, // LATIN CAPITAL LETTER M 
   78, // LATIN CAPITAL LETTER N 
   79, // LATIN CAPITAL LETTER O 
   80, // LATIN CAPITAL LETTER P 
   81, // LATIN CAPITAL LETTER Q 
   82, // LATIN CAPITAL LETTER R 
   83, // LATIN CAPITAL LETTER S 
   84, // LATIN CAPITAL LETTER T 
   85, // LATIN CAPITAL LETTER U 
   86, // LATIN CAPITAL LETTER V 
   87, // LATIN CAPITAL LETTER W 
   88, // LATIN CAPITAL LETTER X 
   89, // LATIN CAPITAL LETTER Y 
   90, // LATIN CAPITAL LETTER Z

   27, // [  EXTENSION TABLE ENCODING: 27,60
   27, // \  EXTENSION TABLE ENCODING: 27,47
   27, // ]  EXTENSION TABLE ENCODING: 27,62
   27, // ^  EXTENSION TABLE ENCODING: 27,20
   
   17, // LOW LINE _
   39, // APOSTROPHE `

   97, // LATIN SMALL LETTER A 
   98, // LATIN SMALL LETTER B 
   99, // LATIN SMALL LETTER C 
   100, // LATIN SMALL LETTER D 
   101, // LATIN SMALL LETTER E 
   102, // LATIN SMALL LETTER F 
   103, // LATIN SMALL LETTER G 
   104, // LATIN SMALL LETTER H 
   105, // LATIN SMALL LETTER I 
   106, // LATIN SMALL LETTER J 
   107, // LATIN SMALL LETTER K 
   108, // LATIN SMALL LETTER L 
   109, // LATIN SMALL LETTER M 
   110, // LATIN SMALL LETTER N 
   111, // LATIN SMALL LETTER O 
   112, // LATIN SMALL LETTER P 
   113, // LATIN SMALL LETTER Q 
   114, // LATIN SMALL LETTER R 
   115, // LATIN SMALL LETTER S 
   116, // LATIN SMALL LETTER T 
   117, // LATIN SMALL LETTER U 
   118, // LATIN SMALL LETTER V 
   119, // LATIN SMALL LETTER W 
   120, // LATIN SMALL LETTER X 
   121, // LATIN SMALL LETTER Y 
   122, // LATIN SMALL LETTER Z 

   27,  // {  EXTENSION TABLE ENCODING: 27,40
   27,  // |  EXTENSION TABLE ENCODING: 27,64
   27,  // }  EXTENSION TABLE ENCODING: 27,41
   27,  // ~  EXTENSION TABLE ENCODING: 27,61

   32,  // DEL
};



/*===========================================================================

FUNCTION OEMNV_UNREADABLE_NV_ITEM

DESCRIPTION
  This procedure checks the given nv item type to see if it's one of
  the few items which are never allowed to be read.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if it is unreadable.
  FALSE if it's not.

SIDE EFFECTS
  None.

============================================================================*/
static boolean oemnv_unreadable_nv_item (
  nv_items_enum_type item
)
{
  switch (item) {

    /* Keys, secrets, and passwords are unreadable */
    case NV_A_KEY_I:           /* Authentication Key */
    case NV_A_KEY_CHKSUM_I:    /* Authentication Key Checksum */
    case NV_SSD_A_I:           /* Shared secret data A */
    case NV_SSD_B_I:           /* Shared secret data B */
    case NV_UP_KEY_I:          /* Unwired Planet Key */ 
    case NV_HDR_AN_AUTH_PASSWORD_I:    /* HDR Access Network CHAP 
		                                  Authentication password */  
    case NV_HDR_AN_AUTH_PASSWD_LONG_I: /* HDR Access Network Stream CHAP 
		                                  Authentication password */
    case NV_HDR_AN_PPP_PASSWORD_I:     /* HDR PPP Password Access Network 
		                                  Authentication */
    case NV_PPP_PASSWORD_I:            /* IP PPP Password */
    case NV_PAP_PASSWORD_I:            /* Password Authentication Protocol 
		                                  Password */
    case NV_DS_MIP_SS_USER_PROF_I:     /* Data Services Mobile IP Shared 
		                                  Secret User Profile */
    case NV_DS_MIP_DMU_MN_AUTH_I:      /* Data Services Mobile IP DMU MN 
		                                  Authentication */
    case NV_SEC_DEVICE_KEY_I :         /* Secured Services Device Key */

    /* We can't read the roaming lists using the usual nv_read command */
    case NV_ROAMING_LIST_I:
    case NV_ROAMING_LIST_683_I:
      return (TRUE);

    default:
      return (FALSE);
  }
} /* oemnv_unreadable_nv_item */



/*===========================================================================

FUNCTION OEMNV_UNWRITABLE_NV_ITEM

DESCRIPTION
  This procedure checks the given nv item type to see if it's one of
  the few items which are never allowed to be written. 

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if it is unwritable.
  FALSE if it's not.

SIDE EFFECTS
  None.

============================================================================*/
static boolean oemnv_unwritable_nv_item (
  nv_items_enum_type item
)
{
  switch (item) {

    /* NV Items that are not unwritable */
    case NV_SEC_DEVICE_KEY_I :
       return (TRUE);

    default:
      return (FALSE);
  }
} /* oemnv_unwritable_nv_item */


/*===========================================================================

FUNCTION OEMNV_UNREPLACEABLE_NV_ITEM

DESCRIPTION
  This procedure checks the given nv item type to see if it's one of
  the few items which are never allowed to be replaced. 

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if it is unreplaceable.
  FALSE if it's not.

SIDE EFFECTS
  None.

============================================================================*/
static boolean oemnv_unreplaceable_nv_item (
  nv_items_enum_type item
)
{
  switch (item) {

    /* NV Items that are not replaceable */
    case NV_SEC_DEVICE_KEY_I :
       return (TRUE);

    default:
      return (FALSE);
  }
} /* oemnv_unreplaceable_nv_item */

/*===========================================================================
FUNCTION: OEMNV_Get

DESCRIPTION
  Get an item from the nonvolatile memory.  Handles nonactive items by
  providing a default value.

RETURN VALUE
  The NV return code, except for NV_NOTACTIVE_S, which is handled
  internally.

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from the UI task.
===========================================================================*/
nv_stat_enum_type OEMNV_Get
(
  nv_items_enum_type item,        /* which item */
  nv_item_type *data_ptr          /* pointer to space for item */
)
{
  int         i;

  nv_cmd_type nvi;           /* nv command buffer */

  /* --------------------------------------------------------------------- */

  nvi.tcb_ptr = rex_self();          /* notify this task when done */
  nvi.sigs = UI_NV_SIG;
  nvi.done_q_ptr = NULL;             /* command goes on no queue when done */

  nvi.item = item;                 /* item to read */
  nvi.cmd = NV_READ_F;

  /* Set up NV so that it will read the data into the correct location */
  nvi.data_ptr = data_ptr;

  /*-----------------------------------------------------------------------
    First check to see if this item is allowed to be read. If it is 
    unreadable, return NV_FAIL_S
  -----------------------------------------------------------------------*/
  if ( oemnv_unreadable_nv_item(item) ) {
    return NV_FAIL_S;
  }

  /* Call NV and wait for a response */
  (void)rex_clr_sigs(rex_self(), UI_NV_SIG);
  nv_cmd( &nvi );
  rex_wait( UI_NV_SIG );
  (void)rex_clr_sigs(rex_self(), UI_NV_SIG);


/* Handle a NV_NOTACTIVE_S or NV_FAIL_S status internally by replacing
** the random data returned with a default value of our own.  Items that
** share the same structure are lumped together in the switch.
*/

#ifdef FEATURE_RUIM_PHONEBOOK
  /* Special handling for RUIM phonebook items */
  if( nvi.status != NV_DONE_S && nvi.item == NV_STACK_I )
  {
    return nvi.status;
  }
#endif /* FEATURE_RUIM_PHONEBOOK */

  if( nvi.status == NV_NOTACTIVE_S || nvi.status == NV_FAIL_S ) {
    nvi.status = NV_DONE_S;

    switch( nvi.item ) {           /* provide our own values */
      case NV_ESN_I:
         // Return a zero ESN but still report Not Active to the caller..
         // (although the ESN should never return Not Active)
         data_ptr->esn.esn = 0;
         nvi.status = NV_NOTACTIVE_S;
         break;

      case NV_CALL_TIMER_G_I:       /* Last call time */
      case NV_LIFE_TIMER_G_I:       /* Life time (non-resettable cumm. call timer) */
        data_ptr->call_timer_g.time = 0 ;
        break;
      case NV_PWR_DWN_CNT_I :
        data_ptr->pwr_dwn_cnt = 0 ;
        break ;
      case NV_LIFE_CNT_I :
        data_ptr->life_cnt = 0 ;
        break ;
      case NV_AIR_CNT_I :
        data_ptr->air_cnt.cnt = 0 ;
        break ;
      case NV_ROAM_CNT_I :
        data_ptr->roam_cnt.cnt = 0 ;
        break ;
      case NV_AIR_TIMER_I:        /* Air time (resettable cummulative call timer) */
      case NV_ROAM_TIMER_I:       /* roam timer */
        data_ptr->air_timer.time = 0; /* no time */
        break;

      case NV_RUN_TIMER_I:        /* Run timer (time hardware has been running) */
        data_ptr->run_timer.time = 0; /* no time */
        break;

      case NV_DIAL_I:             /* speed dial number */
        data_ptr->dial.num_digits = 0; /* no number */
        data_ptr->dial.letters[0] = '\0';
        break;

      case NV_STACK_I:            /* call stack number */
        data_ptr->stack.num_digits = 0; /* no number */
        data_ptr->stack.letters[0] = '\0';
        break;

      case NV_STACK_IDX_I:        /* call stack last number index */
        for( i=0; i<NV_MAX_STACK_DIALS; i++ ) {
          data_ptr->stack_idx.index[i] = 255; /* no items on stack */
        }
        break;

      case NV_GSM_AMR_CALL_CONFIG_I:
        data_ptr->gsm_amr_call_config = FALSE;
        break;

      case NV_AUTO_VOLUME_ENABLED_I:
        data_ptr->auto_volume_enabled = FALSE;
        break;

      case NV_EAR_LVL_I:          /* Handset ear piece volume level   */
      case NV_SPEAKER_LVL_I:      /* ear volume level in car kit      */
      case NV_BEEP_LVL_I:         /* Key beep volume level            */
      case NV_BEEP_SPKR_LVL_I:    /* Key beep volume level in car kit */
      case NV_RINGER_LVL_I:       /* Ringer volume level              */
      case NV_RINGER_SPKR_LVL_I:  /* Ringer volume level in car kit   */
      case NV_ALERTS_LVL_I:       /* SMS message level  SMS Volume    */
      case NV_ALERTS_LVL_SHADOW_I:/* SMS message level in HFK         */
      case NV_EAR_LVL_SHADOW_I:   /* Headset volume level             */
      case NV_BEEP_LVL_SHADOW_I:  /* Headset key beep level           */
#ifdef FEATURE_MULTIMEDIA
      case NV_MM_LVL_I:
      case NV_MM_LVL_SHADOW_I:
      case NV_MM_SPEAKER_LVL_I:
#endif
#ifdef FEATURE_STEREO_DAC
      case NV_MM_SDAC_LVL_I:
      case NV_SDAC_LVL_I:
      case NV_BEEP_SDAC_LVL_I:
#endif
        data_ptr->ear_level = (byte) UISND_AVG_VOL; /* average volume */
        break;

      case NV_CONT_KEY_DTMF_I:    /* Continuous keypad DTMF tones select */
      case NV_AUTO_HYPHEN_I:      /* Auto hyphen setting */
        data_ptr->cont_key_dtmf = TRUE;
        break;

#if defined( FEATURE_UI_FORCE_CALL ) || defined( FEATURE_UI_FORCE_MODE_SEL )
      case NV_PREF_MODE_I:
        /* Default the mode to AUTOMATIC.
        */
        data_ptr->pref_mode.mode = (nv_mode_enum_type) CM_MODE_PREF_AUTOMATIC;
        (void) OEMNV_Put( NV_PREF_MODE_I, data_ptr );
        break;

      case NV_TIMED_PREF_MODE_I:
        /* Default so that mode is reset to CDMA_PREF.
        */
        data_ptr->timed_pref_mode = TRUE;
        break;
#endif /* FEATURE_UI_FORCE_CALL || FEATURE_UI_FORCE_MODE_SEL ) */

#ifdef FEATURE_MULTIPLE_RINGER_TYPES
      /* Ringer Type */
      case NV_RINGER_TYPE_I:
          data_ptr->ringer_type = 0; /* default ringer */
        break;
#endif /* FEATURE_MULTIPLE_RINGER_TYPES */



      case NV_ONETOUCH_DIAL_I:    /* One touch dial enabled */
        data_ptr->onetouch_dial = TRUE;
        break;

#ifdef FEATURE_UI_SEND_PIN
      case NV_SEND_PIN_I:        /* Send PIN code */
        data_ptr->send_pin = FALSE;
        break;
#endif

      case NV_VOICE_PRIV_I:      /* Voice Privacy enabled */
        data_ptr->voice_priv = 0; // 0 denotes VOICEPRIVACY_STD
        (void) OEMNV_Put ( NV_VOICE_PRIV_I, data_ptr );
        break;

      case NV_AUTO_NAM_I:      /* Auto NAM mode */
        data_ptr->auto_nam = TRUE;
        break;

      case NV_DIR_NUMBER_I:      /* Mobile Directory Number */
        for( i=0; i<NV_DIR_NUMB_SIZ; i++ ) {
          data_ptr->dir_number.dir_number[i] = '0'; /* no number */
        }
        break;

      case NV_DIR_NUMBER_PCS_I:
        for (i=0; 
             i < MIN(10,NV_DIR_NUMB_PCS_SIZ);  /*lint !e506*/
             i++) {
          data_ptr->mob_dir_number.digitn[i] = 0x0A; /* Note that 0x0A is the 
                                                        encoding for '0' */
        }
        data_ptr->mob_dir_number.n_digits = 
                                   MIN(10,NV_DIR_NUMB_PCS_SIZ);  /*lint !e506*/
        break;

#ifdef FEATURE_IS95B_TRUE_IMSI
      case NV_IMSI_T_11_12_I:      /* IMSI Mobile Network Code (IMSI_11_12) */
      case NV_IMSI_T_MCC_I:        /* IMSI Mobile Country Code */
      case NV_IMSI_T_S1_I:
      case NV_IMSI_T_S2_I:
      case NV_IMSI_T_ADDR_NUM_I:
        nvi.status = NV_NOTACTIVE_S ; /* will be initialized separately */
        break;
#endif /* FEATURE_IS95B_TRUE_IMSI */


#ifndef FEATURE_CDMA_1900
      case NV_IMSI_MCC_I:        /* IMSI Mobile Country Code */
        data_ptr->imsi_mcc.imsi_mcc = IMSI_MCC_0;
        (void) OEMNV_Put ( NV_IMSI_MCC_I, data_ptr );
        break;

      case NV_IMSI_11_12_I:      /* IMSI Mobile Network Code (IMSI_11_12) */
        data_ptr->imsi_11_12.imsi_11_12 = IMSI_11_12_0;
        (void) OEMNV_Put ( NV_IMSI_11_12_I, data_ptr );
        break;
#else /* def FEATURE_CDMA_1900 */
      case NV_IMSI_MCC_I:        /* IMSI Mobile Country Code */
      case NV_IMSI_11_12_I:      /* IMSI Mobile Network Code (IMSI_11_12) */
#endif /* def FEATURE_CDMA_1900 */
      case NV_MIN1_I:
      case NV_MIN2_I:
      case NV_IMSI_ADDR_NUM_I:
        nvi.status = NV_NOTACTIVE_S ; /* will be initialized separately */
        break;

      case NV_FSC_I:             /* Field Service Lock code */
        for( i=0; i<NV_FSC_SIZ; i++ ) {
          data_ptr->fsc.fsc[i] = '0'; /* Field Service code */
        }
        (void) OEMNV_Put ( NV_FSC_I, data_ptr );
        break;


      case NV_LANGUAGE_SELECTION_I:
        data_ptr->language_selection = NV_LANGUAGE_ENGLISH;
        break;

      case NV_LOCK_I:             /* Is the phone locked? */
      case NV_AUTO_LOCK_I:        /* Auto lock the phone? */
      case NV_CALL_BEEP_I:        /* One minute call beeper select */
      case NV_CONT_STR_DTMF_I:    /* Continuous string (memory) DTMF tones select */
      case NV_SVC_AREA_ALERT_I:   /* Service area enter/exit alert select */
      case NV_CALL_FADE_ALERT_I:  /* Call fade alert select */
      case NV_HORN_ALERT_I:       /* horn beep on call */
      case NV_AUTO_MUTE_I:        /* Auto radio mute setting */
        data_ptr->call_beep = FALSE;
        break;

      case NV_ROAM_RINGER_I:      /* Roam ringer */
        data_ptr->roam_ringer = FALSE;
        break;

      case NV_CURR_NAM_I:         /* Current NAM setting */
        data_ptr->curr_nam = 0;   /* write it to nv, if never set */
        (void) OEMNV_Put ( NV_CURR_NAM_I, data_ptr );
        break;

      case NV_BANNER_I:           /* Idle banner to display - QUALCOMM */
        (void) MEMSET( (void *) data_ptr->banner.letters, ' ', NV_MAX_LTRS );
        break;

      case NV_AUTO_REDIAL_I:      /* Auto redial setting */
      case NV_AUTO_ANSWER_I:      /* auto answer */
      case NV_PAGE_SET_I:         /* page set */
        data_ptr->auto_answer.enable = FALSE; /* Default off, one ring */
        data_ptr->auto_answer.rings = 0;
        break;

      case NV_SMS_VM_NUMBER_I:
        data_ptr->sms_vm_number.num_digits = 0;
        data_ptr->sms_vm_number.digits[data_ptr->sms_vm_number.num_digits] = 0;
        break;

      case NV_BACK_LIGHT_HFK_I: 
      case NV_BACK_LIGHT_I:       /* Backlighting manual/auto mode */
        data_ptr->back_light = 255;  /* 255: always on */
        break;

      case NV_CONFIG_CHKSUM_I:    /* Configuration checksum */
        data_ptr->config_chksum.chksum = 0; /* 0 = bad checksum default */
        break;

      case NV_PAGE_MSG_I:         /* Page messages */
        data_ptr->page_msg.newPage  = FALSE; /* Not a new page */
        data_ptr->page_msg.num_chars = 0;
        break;

      case NV_ERR_LOG_I:
        data_ptr->err_log.err_count=0;
        break;

      case NV_NAME_NAM_I:
        (void) MEMSET( (void *) data_ptr->name_nam.name, ' ', NV_MAX_LTRS );
        break;

      case NV_SPARE_2_I:
        data_ptr->spare_2 = 0;
        break;

      case NV_MAX_TX_ADJ_I:
        data_ptr->max_tx_adj = 0;
        break;
      case NV_INI_MUTE_I:
        data_ptr->init_mute = NV_MUTE_NORM;
        break;

      case NV_DM_ADDR_I:
        data_ptr->dm_addr = 1;
        break;

      case NV_FACTORY_INFO_I:
        (void) MEMCPY( (void *) data_ptr->fact_info, "No Entry", 9 );
        break;

      case NV_CALL_RSTRC_I:       /* call restrictions */
        data_ptr->call_rstrc = 0;
        break;

      case NV_WDC_I:              /* Warranty Date Code */
        (void) MEMSET ( (void *) data_ptr->wdc.wdc, ' ', NV_WDC_LTRS );
        break;

#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

#ifdef FEATURE_CDMA_800
      case NV_PCDMACH_I:
      case NV_SCDMACH_I:
        nvi.status = NV_NOTACTIVE_S ; /* will be initialized separately */
        break;
#endif /* FEATURE_CDMA_800 */

#ifdef FEATURE_SSPR_ENHANCEMENTS
      case NV_HOME_SID_NID_I:
        nvi.status = NV_NOTACTIVE_S ; /* will be initialized separately */
        break;
#else
      case NV_SID_NID_I:
        nvi.status = NV_NOTACTIVE_S ; /* will be initialized separately */
        break;
#endif

#if defined(FEATURE_SSPR_ENHANCEMENTS) || defined(FEATURE_PRL_FORCE_MODE) || \
    defined(FEATURE_E911) || defined(FEATURE_HOME_ONLY)

      case NV_SYSTEM_PREF_I:
        /* Default the preferred system to NV_SP_STANDARD.
        */
        data_ptr->system_pref.sys = NV_SP_STANDARD;
        (void) OEMNV_Put( NV_SYSTEM_PREF_I, data_ptr );
        break;
#endif

#ifdef FEATURE_SSPR_ENHANCEMENTS
      case NV_SID_NID_LOCK_I:
        nvi.status = NV_NOTACTIVE_S ; /* will be initialized separately */
        break;

#else
#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */
#endif /* FEATURE_SSPR_ENHANCEMENTS */

#ifdef FEATURE_SSPR_800
    case NV_PRL_ENABLED_I:
      nvi.status = NV_NOTACTIVE_S ; /* will be initialized separately */
      break;
#endif /* FEATURE_SSPR_800 */

#if defined(FEATURE_USRSYSCLOCK) || defined(FEATURE_ISYSCLOCK)
      case NV_NITZ_NW_INFO_I:
        (void) MEMSET( (void *) data_ptr->nitz_nw_info.plmn_name, 0, NV_MAX_NW_NAME_CHARS );
        break;
      case NV_NITZ_NW_INFO_MCC_I:
         data_ptr->nitz_nw_info_mcc = 0;
         break;
      case NV_NITZ_NW_INFO_MNC_I:
         data_ptr->nitz_nw_info_mnc = 0;
         break;
#endif 

#ifdef FEATURE_RUIM
      case NV_RUIM_CHV_1_I:
        nvi.status = NV_NOTACTIVE_S;
        break;

      case NV_RUIM_CHV_2_I:
        nvi.status = NV_NOTACTIVE_S;
        break;
#endif /* FEATURE_RUIM */

#ifdef FEATURE_GPSONE
#error code not present
#endif /* FEATURE_GPSONE */

#ifdef FEATURE_GSTK_CS_PS_PARALLEL
      case NV_TOOLKIT_CS_PS_PARALLEL_I:
        data_ptr->toolkit_cs_ps_parallel = FALSE;
      break;
#endif /* FEATURE_GSTK_CS_PS_PARALLEL */

      /* If it's an unknown item, print an info message and pass on the */
      /* NV_NOTACTIVE_S status. */
      default:
        MSG_HIGH( "Nonactive NV item 0x%x", nvi.item, 0, 0 );
        nvi.status = NV_NOTACTIVE_S;
        break;
    }
  } else {
    if( nvi.status != NV_DONE_S ) {
#ifdef FEATURE_RUIM_SMS
      //
      // The NV R-UIM layer always returns NV_BADPARM_S for
      // the special '0' SMS address.  Grep for where 
      // nvruimsms_buildlist() is used for more details.
      // 
      if ( (NV_SMS_I == nvi.item)       &&
           (NV_BADPARM_S == nvi.status) &&
           (0 == data_ptr->sms.address) ) {
         return nvi.status;
      }
#endif /* FEATURE_RUIM_SMS */

      ERR( "NV Read Failed Item %d Code %d", nvi.item, nvi.status, 0 );
    }
  }

  return nvi.status;
}


/* <EJECT> */
/*===========================================================================
FUNCTION: OEMNV_Put

DESCRIPTION
  Write an item to NV memory.  Wait till write is completed.

RETURN VALUE
  The NV Return Code

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from the UI task.
===========================================================================*/
nv_stat_enum_type OEMNV_Put
(
  nv_items_enum_type item,        /* which item */
  nv_item_type *data_ptr          /* pointer to data for item */
)
{
  nv_cmd_type nvi;                 /* nv command buffer */

  /* --------------------------------------------------------------------- */

  nvi.tcb_ptr = rex_self();        /* Notify this task when done */
  nvi.sigs = UI_NV_SIG;
  nvi.done_q_ptr = NULL;           /* command goes to no queue when done */

  nvi.item = item;                 /* item to write */
  nvi.cmd = NV_WRITE_F;

  nvi.data_ptr =  data_ptr;        /* the data to write */

  /*-----------------------------------------------------------------------
    First check to see if this item is allowed to be written. If it is 
    unwritable, return NV_FAIL_S
  -----------------------------------------------------------------------*/
  if ( oemnv_unwritable_nv_item(item) ) {
    return NV_FAIL_S;
  }


  /* Call NV and wait for a response */
  (void)rex_clr_sigs(rex_self(), UI_NV_SIG);
  nv_cmd( &nvi );
  rex_wait( UI_NV_SIG );
  (void)rex_clr_sigs(rex_self(), UI_NV_SIG);

  if( nvi.status != NV_DONE_S ) {
    ERR( "NV Write Failed Item %d Code %d", nvi.item, nvi.status, 0 );
  }

  return( nvi.status );

}

/*===========================================================================
FUNCTION: OEMNV_Replace

DESCRIPTION
  Replace an item to NV memory.  Wait till write is completed.

RETURN VALUE
  The NV Return Code

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from the UI task.
===========================================================================*/
nv_stat_enum_type OEMNV_Replace
(
  nv_items_enum_type item,        /* which item */
  nv_item_type *data_ptr          /* pointer to data for item */
)
{
  nv_cmd_type nvi;                 /* nv command buffer */

  /* --------------------------------------------------------------------- */

  nvi.tcb_ptr = rex_self();        /* Notify this task when done */
  nvi.sigs = UI_NV_SIG;
  nvi.done_q_ptr = NULL;           /* command goes to no queue when done */

  nvi.item = item;                 /* item to write */
  nvi.cmd = NV_REPLACE_F;

  nvi.data_ptr =  data_ptr;        /* the data to write */

  /*-----------------------------------------------------------------------
    First check to see if this item is allowed to be replaced. If it is 
    unreplaceable, return NV_FAIL_S
  -----------------------------------------------------------------------*/
  if ( oemnv_unreplaceable_nv_item(item) ) {
    return NV_FAIL_S;
  }

  /* Call NV and wait for a response */
  (void)rex_clr_sigs(rex_self(), UI_NV_SIG);
  nv_cmd( &nvi );
  rex_wait( UI_NV_SIG );
  (void)rex_clr_sigs(rex_self(), UI_NV_SIG);

  if( nvi.status != NV_DONE_S ) {
    ERR( "NV Replace Failed Item %d Code %d", nvi.item, nvi.status, 0 );
  }

  return( nvi.status );
}

/* <EJECT> */
/*===========================================================================
FUNCTION: OEMNV_Free

DESCRIPTION
  Free an item in NV memory.  Wait till free is completed.

RETURN VALUE
  The NV Return Code

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from the UI task.
===========================================================================*/
nv_stat_enum_type OEMNV_Free
(
  nv_items_enum_type item,        /* which item */
  nv_item_type *data_ptr          /* pointer to data for item */
)
{
  nv_cmd_type nvi;                 /* nv command buffer */

  /* --------------------------------------------------------------------- */

  nvi.tcb_ptr = rex_self();        /* Notify this task when done */
  nvi.sigs = UI_NV_SIG;
  nvi.done_q_ptr = NULL;           /* command goes to no queue when done */

  nvi.item = item;                 /* item to free */
  nvi.cmd = NV_FREE_F;

  nvi.data_ptr =  data_ptr;        /* the data to write */

  /* Call NV and wait for a response */
  (void)rex_clr_sigs(rex_self(), UI_NV_SIG);
  nv_cmd( &nvi );
  rex_wait( UI_NV_SIG );
  (void)rex_clr_sigs(rex_self(), UI_NV_SIG);

  if( nvi.status != NV_DONE_S ) {
    ERR( "NV Free Failed Item %d Code %d", nvi.item, nvi.status, 0 );
  }

  return( nvi.status );
}


/*=============================================================================
FUNCTION:  OEMNV_RUIMAlpha_TO_WSTR
 
DESCRIPTION: Decode a R-UIM alpha tag into an Unicode string
 
PARAMETERS:
   *alpha:  source string
   nAlphaSize: source string length 
   *wStr:   destination AECHAR string
   nWStrSize:  max size of wStr
 
RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void OEMNV_RUIMAlpha_TO_WSTR(byte const *alpha,
                             uint32      nAlphaSize,
                             AECHAR     *wStr,
                             uint32      nWStrSize)
{
   boolean isAscii;
   uint32  len,
           i, 
           j;

   if (nAlphaSize <= 1) {
      wStr[0] = (AECHAR)'\0';
      return;
   }

   if (alpha[0] < 0x80) {
      isAscii = TRUE;
   } else if (0x80 == alpha[0]) {
      isAscii = FALSE;
   } else {
      /* Encoding not supported */
      wStr[0] = (AECHAR)'\0';
      return;
   }

   /* determine the length of the input alpha string (in bytes) */
   len = 0;
   for (i = 0; i < nAlphaSize; i++) {
      if (0xFF == alpha[i]) { 
         break;
      } 
      len++;
   }

   MEMSET(wStr, 0, nWStrSize);

   if (isAscii) {
      //
      // Convert from GSM character set to ASCII, then to AECHAR
      //
      for (i = 0; i < MIN(len,nWStrSize); i++, wStr++) {

         *wStr = (AECHAR) gsm2ASCII[alpha[i]];

         if (27 == *wStr) {
            i++;

            // NOTE: If the character isn't found in the Extension Table,
            //       just keep the ESC character
            for (j = 0; j < sizeof(gsmExtTable)/sizeof(gsmExtTable[0]); j++ ) {
               if (gsmExtTable[j].gsmChar == (char)alpha[i]) {
                  *wStr = (AECHAR)(gsmExtTable[j].asciiChar);
                  break;
               }
            }
         }
      }

   } else {
      // encoding is in the first byte, don't count it towards
      // the length.
      len--;

      if (nWStrSize < len) {
         len = nWStrSize;
      }

      // Unicode takes up 2 bytes/character. If len is an odd
      // number then we only have half of the last character so
      // just ignore it.
      if (len % 2) {
         len--;
      }

      for (i = 0; i < len; i+=2, wStr++) {
         // Suppress "Suspicious pointer-to-pointer conversion(area too small)"
         /*lint -save -e826*/
         *wStr = NTOHS(*(AECHAR*)(&alpha[1+i]));
         /*lint -restore*/
      }
   }
} // OEMNV_RUIMAlpha_TO_WSTR


/*=============================================================================
FUNCTION:  OEMNV_WSTR_TO_RUIMAlpha
 
DESCRIPTION: Encode an Unicode string into an R-UIM alpha tag string
 
PARAMETERS:
   *wStr: source string
   *alpha: destination string
   nAlphaSize: size of the destination string.
 
RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void OEMNV_WSTR_TO_RUIMAlpha(AECHAR const *wStr,
                             byte         *alpha,
                             uint32        nAlphaSize)
{
   uint32  nWStrLen = (uint32) WSTRLEN(wStr);
   uint32  i,
           len;
   boolean isAscii;

   MEMSET(alpha, 0xFF, nAlphaSize);

   // Determine if the string only contains ASCII characters
   isAscii = TRUE;
   for (i = 0; i < nWStrLen; i++) {
      if ( (wStr[i] & 0xFF80) > 0 ) {
         isAscii = FALSE;
         break;
      }
   }

   if (isAscii) {
      //
      // Convert from ASCII to the GSM character set
      // (GSM 03.38 section 6.2.1)
      //
      for (len = 0; (len < nAlphaSize) && *wStr; len++, wStr++) {

         alpha[len] = ascii2GSM[*wStr];

         if (27 == alpha[len]) {
            if (len+1 >= nAlphaSize) {
               // Out of room in the buffer, remove the first byte of
               // the incomplete character and exit
               alpha[len] = 0xFF;
               break;
            }

            for (i = 0; ; i++) {
               if (i >= sizeof(gsmExtTable)/sizeof(gsmExtTable[0])) {
                  // Didn't find the character in the Extension Table,
                  // just use a space
                  alpha[len] = ascii2GSM[' '];
                  break;
               }

               if (gsmExtTable[i].asciiChar == (char)*wStr) {
                  len++;
                  alpha[len] = gsmExtTable[i].gsmChar;
                  break;
               }
            }
         }
      }

   } else {
      alpha[0] = 0x80; // 0x80 == Unicode encoding
      
      len = MIN(nAlphaSize-1,nWStrLen*2);

      // Unicode takes up 2 bytes/character. If len is an odd
      // number then we only have half of the last character so
      // just ignore it.
      if (len % 2) {
         len--;
      }
            
      for (i = 0; i < len; i+=2, wStr++) {
         // Suppress "Suspicious pointer-to-pointer conversion(area too small)"
         /*lint -save -e826*/
         *(AECHAR*)(&alpha[1+i]) = HTONS(*wStr);
         /*lint -restore*/
      }

      len++; // add one for the encoding byte 
   }

} // OEMNV_WSTR_TO_RUIMAlpha

