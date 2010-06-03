/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                UI - HS INTERFACE FUNCTIONS

GENERAL DESCRIPTION
  These functions provide an interface between the handset and User Intrface.
  These functions are provided to facilitate testing the reference UI with the
  existing handset physical device  driver. These functions hide the actual
  hardware dependencies of the hs driver and allow the UI to use the User API
  to  communicate with the physical hs driver.

  The interface functions are divided into two classed:
   · Key pad interface functions
   · Display device interface functions


EXTERNALIZED FUNCTIONS

  kpd_reg_key_event
    This function is used by the client to register for key events

  kpd_sub_srvc
    This function is used by the client to subscribe to the keypad services

  kpd_ctrl_bklit_onoff
    This function is used by the client to modify the keypad backlighting

  lcd_sub_srvc
    This function is used by the client to subscribe to the display driver
    services

  lcd_cancel_srvc
    This function is used by the client to cancel the subscription to
    the display driver services

  lcd_ctrl_ann_onoff
    This function is used by the client to switch annunciators on or off

  lcd_blink_ann
    This function is used by the client to control blinking of annunciators

  lcd_ctrl_screen_onoff
    This function is used by the client to switch screen on or off

  lcd_ctrl_bklit_onoff
    This function is used by the client to modify the display backlighting

  lcd_display_chars
    This function is used by the client to display characters on the screen

  lcd_clr_chars
    This function is used by the client to clear characters on the screen

  lcd_clr_screen
    This function is  used by the client to clear the entire screen

  lcd_blink_chars
    This function is used by the client to blink characters on or off

  lcd_blink_screen_chars
    This function is used by the client to blink characters on or off

  lcd_snapshot
    This function is used by the client to capture the snapshot of the screen

  lcd_set_charmap
    This function is used by the client to modify the display's char mapping.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The handset driver shall be initialized before calling any of the above functions.
  The UI shall subscribe to the keypad and display services before using any of the
  operational functions.

Copyright (c) 1999 - 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$PVCSPath:  L:/src/asw/MSM6050/vcs/hsui.c_v   1.3   12 Mar 2002 19:53:28   dlee  $
$Header: //depot/asic/msmshared/services/hs/hsui.c#11 $ $DateTime: 2006/12/12 19:55:25 $ $Author: dpandian $

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.


when       who      what, where, why
--------   ---      ---------------------------------------------------------
12/05/03   djd      Add support for power key through PMIC.
10/10/02   pd       Added multiple key press support
09/09/02   NNG      Return command ptr to HS free queue if not used.
02/11/02   dl       Converted to keypad.h functions.
02/04/02   rmd      Replaced FEATURE_PM1000 with FEATURE_PMIC_LCDKBD_LED_DRIVER
07/27/01   kar      Sent buffered keypresses on client registration
04/30/01   kar      Added function lcd_set_blink_rate. Changed parameters
                    to lcd_display_chars to prevent redundant calls to HS
                    Copy blink character directly from buffer in function
                    lcd_blink_screen_chars
06/22/00   dsb      Merged with PP3.0c: Use logical BIO registers
06/08/00   dsb      Abstracted HS module by including hs.h, not hsig.h
05/08/00   dsb      Changed #include "biog.h" to "bio.h" to allow BIO
                    abstraction based on chips/targets.
11/10/99   jc       Added FEATURE_HEADSET.
10/19/99   jc       created the file.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"     /* Definitions for byte, word, etc.        */
#include "processor.h"  /* Processor architecture definitions      */
#include "queue.h"      /* Queue declaration package               */
#include "hs.h"         /* Header file for handset driver          */

#ifdef FEATURE_BSPUI_API


#include "hsui.h"       /* Header file for api functions           */
#include "hs.h"
#include "keypad.h"
#include "bio.h"


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/


/*  structure to store the display snapshot */
static lcd_snapshot_type snapshot;


/* lcd call back function pointer. Used for Cancel service */
static lcd_cb_f_handle_type *f_lcd_cb = NULL;

/* static variables used to remember the last requested blink rate
   for the annunciator and the display and the actual HS device */
static word hs_blink_rate   =0;


/* flags used to store the subscription status of the LCD device.
   Currently multiple registration(subscription) are not
   supported. */
static boolean lcd_subscribed = FALSE;
static boolean kpd_subscribed = FALSE;

/* actual text of screen and blink attributes of screen */
static byte   scrntxt [ HS_SCRN_HIGH ][ HS_SCRN_WIDE ];
static byte   scrnblnk[ HS_SCRN_HIGH ][ HS_SCRN_WIDE ];

/* Status of the back light */
static boolean lcd_backlight_on = TRUE;


/*===========================================================================

FUNCTION KPD_REG_KEY_EVENT

DESCRIPTION
  This function is used by the client to register for key events

DEPENDENCIES
  hs_init must have been called before using this function.
  kpd_sub_srvc must have been called before using this function.

RETURN VALUE
  The function returns TRUE if the command could be sent successfully to the hs
  command queue and FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean kpd_reg_key_event(kpd_handle_type          kpd_handle,
#ifdef FEATURE_KEYPAD_MULTI_KEY
                          keypad_key_event_cb_f_type *kpd_cb_func)
#else /* FEATURE_KEYPAD_MULTI_KEY */
#error code not present
#endif /* FEATURE_KEYPAD_MULTI_KEY */
{
#ifdef FEATURE_KEYPAD_MULTI_KEY
   keypad_key_event_type   key;
#else
#error code not present
#endif /* FEATURE_KEYPAD_MULTI_KEY */
 keypad_register(kpd_cb_func);

 /**********************************************************
 ** Now Simulate a series of key events to get the UI going.
 **********************************************************/


#ifdef FEATURE_HEADSET
 /* Always simulate the HS_EAR_UP_K when this the headset is used.
 */
 key.key_code = (byte) HS_EAR_UP_K;
#else
 /* Sample the state of the earpiece switch and pass it on.
 */
 key.key_code = (byte)  (HS_EAR_UP() ? HS_EAR_UP_K : HS_EAR_DOWN_K);
#endif

 kpd_cb_func(key );
 key.key_code = (byte) HS_RELEASE_K;
 kpd_cb_func(key);

 /* Sample the state of the ignition switch and pass it on.
 ** If we are not in the hands-free adapter, ignore the ignition sensor.
 */
 if ( !HS_EXT_PWR_ON() )
 {
    key.key_code = (byte) HS_EXT_PWR_OFF_K;
    kpd_cb_func(key );
    key.key_code = (byte) HS_RELEASE_K;
    kpd_cb_func(key);
 }

 /* Sample the state of the power key and pass it on.
 */
#if defined(FEATURE_PMIC) && !defined (T_CARMEN)
  /* If the PMIC is available then use the PMIC to find out if the
     Power Key is pressed or not */
  if ( pm_is_power_key_pressed() )
#else
 if ( BIO_INM( BIO_ON_SW_SENSE_REG, BIO_ON_SW_SENSE_M ) == BIO_ON_SW_ON_V ) 
#endif
 {
    key.key_code =  (byte) HS_PWR_K;
    kpd_cb_func(key);
 }

 /* See if there are any keys buffered up and send them to UI
 */
#ifdef FEATURE_KEYPAD_MULTI_KEY
 while ((key.key_code = keypad_get_key().key_code) != HS_NONE_K) {
#else /* FEATURE_KEYPAD_MULTI_KEY */
#error code not present
#endif /* FEATURE_KEYPAD_MULTI_KEY */
   kpd_cb_func(key);
 }

 return TRUE;
}


/*===========================================================================

FUNCTION KPD_SUB_SRVC

DESCRIPTION
  This function is used by the client to subscribe to the keypad services

DEPENDENCIES
  hs_init must have been called before using this function.

RETURN VALUE
  The function returns TRUE if the command could be sent successfully to the hs
  command queue and FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean kpd_sub_srvc( kpd_cb_f_handle_type  *kpd_cb_func)
{
kpd_handle_type  kpd_handle;

 /* If already subscribed then return. Only 1 subscription supported */
 if(kpd_subscribed)
    return FALSE;

 kpd_handle.id = HS_KPD_DEVICE;
 kpd_handle.status = HS_SUBSCRIBED;


 kpd_cb_func ( kpd_handle );


 kpd_subscribed = TRUE;    /* indicate that one subscription is active*/

 return TRUE;

}


/*===========================================================================

FUNCTION LCD_SUB_SRVC

DESCRIPTION
  This function is used by the client to subscribe to the display driver services

DEPENDENCIES
  hs_init must have been called before using this function.

RETURN VALUE
  The function returns TRUE if the command could be sent successfully to the hs
  command queue and FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean lcd_sub_srvc( lcd_cb_f_handle_type  *lcd_cb_func)
{
int i,j;
lcd_handle_type  lcd_handle;

/* If already subscribed then return. Only 1 subscription supported */
 if(lcd_subscribed)
    return FALSE;

  /* initialize the display screen and blink screen buffers  */
  for( i=0; i<HS_SCRN_HIGH; i++ )
  {
    for( j=0; j<HS_SCRN_WIDE; j++ )
    {
     scrntxt [i][j]  = ' ';
     scrnblnk[i][j]  = ' ';
    }
  }

 /**************************
 ** Normally all subscriptions should be handled by the device driver
 ** hsg.c.   This action is performed here due to idiosyncrocies of the
 ** UI initialization sequence ie UI waiting for PWR keys to come from
 ** the HS task but HS waiting for UI to subscribe to KPD services before
 ** sending keys.

 if( ( hs_cmd_ptr = (hs_packets_type *)  q_get( &hs_cmd_free_q )) == NULL )
   return FALSE;

 hs_cmd_ptr->lcd_srvc.hdr.cmd        = HS_LCD_SRVC;
 hs_cmd_ptr->lcd_srvc.hdr.task_ptr   = NULL;
 hs_cmd_ptr->lcd_srvc.hdr.done_q_ptr = &hs_cmd_free_q;
 hs_cmd_ptr->lcd_srvc.cmd            = HS_SUBSCRIBE;
 hs_cmd_ptr->lcd_srvc.cb_ptr         = lcd_cb_func;
 hs_cmd( hs_cmd_ptr );
 **
 **
 **
 *****************************/

 lcd_handle.id = HS_LCD_DEVICE;
 lcd_handle.status = HS_SUBSCRIBED;

 lcd_cb_func ( lcd_handle );



 f_lcd_cb = lcd_cb_func;   /* save the call back function pointer for */
                           /* later use when cancelling the service   */
 lcd_subscribed = TRUE;    /* indicate that one subscription is active*/

 return TRUE;

}


/*===========================================================================

FUNCTION LCD_CANCEL_SRVC

DESCRIPTION
  This function is used by the client to cancel the subscription to
  the display driver services

DEPENDENCIES
  hs_init must have been called before using this function.

RETURN VALUE
  The function returns TRUE if the command could be sent successfully to the hs
  command queue and FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean lcd_cancel_srvc(lcd_handle_type lcd_handle)

{

 /* Verify handle and subscription is valid */
 if ((lcd_handle.id != HS_LCD_DEVICE) || (!lcd_subscribed) )
   return FALSE;

 /**************************
 ** Normally all subscriptions should be handled by the device driver
 ** hsg.c.   This action is performed here due to idiosyncrocies of the
 ** UI initialization sequence ie UI waiting for PWR keys to come from
 ** the HS task but HS waiting for UI to subscribe to KPD services before
 ** sending keys.

 if( ( hs_cmd_ptr = (hs_packets_type *)  q_get( &hs_cmd_free_q )) == NULL )
   return FALSE;

 hs_cmd_ptr->lcd_srvc.hdr.cmd        = HS_LCD_SRVC;
 hs_cmd_ptr->lcd_srvc.hdr.task_ptr   = NULL;
 hs_cmd_ptr->lcd_srvc.hdr.done_q_ptr = &hs_cmd_free_q;
 hs_cmd_ptr->lcd_srvc.cmd            = HS_CANCEL;
 hs_cmd_ptr->lcd_srvc.handle.id      = lcd_handle.id;
 hs_cmd_ptr->lcd_srvc.cb_ptr         = f_lcd_cb;
 hs_cmd( hs_cmd_ptr );
 **
 **
 **
 *****************************/

 lcd_subscribed = FALSE;     /* indicate that NO subscriptions are active*/

 lcd_handle.status = HS_CANCELLED;

 f_lcd_cb ( lcd_handle );

 return TRUE;
}

/*===========================================================================

FUNCTION LCD_CTRL_ANN_ONOFF

DESCRIPTION
  This function is used by the client to switch annunciators on or off

DEPENDENCIES
  hs_init must have been called before using this function.
  lcd_sub_srvc must have been caleed before using this function.

RETURN VALUE
  The function returns TRUE if the command could be sent successfully to the hs
  command queue and FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean lcd_ctrl_ann_onoff(lcd_handle_type         lcd_handle,
                           lcd_ann_enum_type       lcd_ann,
                           boolean                 onoff,
                           lcd_ann_value_enum_type value)

{
 hs_packets_type *hs_cmd_ptr;

 /* Verify handle and subscription is valid */
 if ((lcd_handle.id != HS_LCD_DEVICE) || (!lcd_subscribed) )
   return FALSE;


 /* If no available command buffers return */
 if( ( hs_cmd_ptr = (hs_packets_type *)  q_get( &hs_cmd_free_q )) == NULL )
   return FALSE;

 switch (lcd_ann) {
  case HS_AN_CDMA:
  case HS_AN_INUSE:
  case HS_AN_NOSVC:
  case HS_AN_ROAM:
  case HS_AN_SNI:
  case HS_AN_VMAIL:
  case HS_AN_MSG:
  case HS_AN_ALL:
   hs_cmd_ptr->anna.hdr.cmd        = HS_ALL_ANNUN;
   hs_cmd_ptr->anna.hdr.task_ptr   = NULL;
   hs_cmd_ptr->anna.hdr.done_q_ptr = &hs_cmd_free_q;
   hs_cmd_ptr->anna.annun.mask     = lcd_ann;
   if (onoff)
      hs_cmd_ptr->anna.annun.value    = lcd_ann;
   else
      hs_cmd_ptr->anna.annun.value    = 0;
   /* Copy to blink areas to prevent blinking */
   hs_cmd_ptr->anna.blink_annun.mask  = lcd_ann;
   hs_cmd_ptr->anna.blink_annun.value = hs_cmd_ptr->anna.annun.value;
   break;

  case HS_AN_RSSI:
   hs_cmd_ptr->rssi.hdr.cmd        = HS_SCRN_RSSI;
   hs_cmd_ptr->rssi.hdr.task_ptr   = NULL;
   hs_cmd_ptr->rssi.hdr.done_q_ptr = &hs_cmd_free_q;
   hs_cmd_ptr->rssi.rssi           = value;
   hs_cmd_ptr->rssi.blink_rssi     = value; /* copy to prevent blinking */
   break;

  case HS_AN_BATTERY:
   hs_cmd_ptr->batt.hdr.cmd        = HS_SCRN_BATTERY;
   hs_cmd_ptr->batt.hdr.task_ptr   = NULL;
   hs_cmd_ptr->batt.hdr.done_q_ptr = &hs_cmd_free_q;
   hs_cmd_ptr->batt.batt           = value;
   hs_cmd_ptr->batt.blink_batt     = value; /* copy to prevent blinking */
   break;

  default:
   /* put the cmd back in the HS queue */
   q_put (&hs_cmd_free_q, &(hs_cmd_ptr->hdr.link) );
   return FALSE;
 }

 /* send command to hs_task */
 hs_cmd( hs_cmd_ptr );

 return TRUE;
}

 /*===========================================================================

FUNCTION LCD_BLINK_ANN

DESCRIPTION
  This function is used by the client to control blinking of annunciators

DEPENDENCIES
  hs_init must have been called before using this function.
  lcd_sub_srvc must have been caleed before using this function.

  The blink rate cannot be set to different values for different
  parts of the screen.

RETURN VALUE
  The function returns TRUE if the command could be sent successfully to the hs
  command queue and FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean lcd_blink_ann(lcd_handle_type         lcd_handle,
                      lcd_ann_enum_type       lcd_ann,
                      lcd_ann_value_enum_type value,
                      word                    blink_rate)

{
 hs_packets_type *hs_cmd_ptr;
 word blink_value;

 /* Verify handle and subscription is valid */
 if ((lcd_handle.id != HS_LCD_DEVICE) || (!lcd_subscribed) )
   return FALSE;


 /* If no available command buffers return */
 if( ( hs_cmd_ptr = (hs_packets_type *)  q_get( &hs_cmd_free_q )) == NULL )
   return FALSE;


 switch (lcd_ann) {
  case HS_AN_CDMA:
  case HS_AN_INUSE:
  case HS_AN_NOSVC:
  case HS_AN_ROAM:
  case HS_AN_SNI:
  case HS_AN_VMAIL:
  case HS_AN_MSG:
  case HS_AN_ALL:
   hs_cmd_ptr->anna.hdr.cmd          = HS_ALL_ANNUN;
   hs_cmd_ptr->anna.hdr.task_ptr     = NULL;
   hs_cmd_ptr->anna.hdr.done_q_ptr   = &hs_cmd_free_q;
   hs_cmd_ptr->anna.annun.mask       = lcd_ann;
   hs_cmd_ptr->anna.annun.value      = lcd_ann;
   hs_cmd_ptr->anna.blink_annun.mask = lcd_ann;
   if (blink_rate==0) /* no blinking - keep display the same */
      blink_value = lcd_ann;
   else               /* blinking - change the display value */
      blink_value = 0;
   hs_cmd_ptr->anna.blink_annun.value = blink_value;
   break;

  case HS_AN_RSSI:
   hs_cmd_ptr->rssi.hdr.cmd        = HS_SCRN_RSSI;
   hs_cmd_ptr->rssi.hdr.task_ptr   = NULL;
   hs_cmd_ptr->rssi.hdr.done_q_ptr = &hs_cmd_free_q;
   hs_cmd_ptr->rssi.rssi           = value;
   if (blink_rate==0) /* no blinking - keep display the same */
      blink_value = value;
   else               /* blinking - change the display value */
      blink_value = 0;
   hs_cmd_ptr->rssi.blink_rssi     = blink_value;
   break;

  case HS_AN_BATTERY:
   hs_cmd_ptr->batt.hdr.cmd        = HS_SCRN_BATTERY;
   hs_cmd_ptr->batt.hdr.task_ptr   = NULL;
   hs_cmd_ptr->batt.hdr.done_q_ptr = &hs_cmd_free_q;
   hs_cmd_ptr->batt.batt           = value;
   if (blink_rate==0) /* no blinking - keep display the same */
      blink_value = value;
   else               /* blinking - change the display value */
      blink_value = 0;
   hs_cmd_ptr->batt.blink_batt     = blink_value;
   break;

  default:
   /* put the cmd back in the HS queue */
   q_put (&hs_cmd_free_q, &(hs_cmd_ptr->hdr.link) );
   return FALSE;
 }

 /* send command to hs_task */
 hs_cmd( hs_cmd_ptr );


   /* The HS device only supports one blink rate so set it to the latest */
   /* requested rate. The application should only set the ann or display */
   /* rate but not both to different values                              */

 if (blink_rate != hs_blink_rate) {

  if( ( hs_cmd_ptr = (hs_packets_type *)  q_get( &hs_cmd_free_q )) == NULL )
   return FALSE;

  hs_cmd_ptr->blink.hdr.cmd        = HS_BLINK_RATE;
  hs_cmd_ptr->blink.hdr.task_ptr   = NULL;
  hs_cmd_ptr->blink.hdr.done_q_ptr = &hs_cmd_free_q;
  hs_cmd_ptr->blink.blink_rate     = blink_rate;

  hs_cmd( hs_cmd_ptr );
  hs_blink_rate = blink_rate;
 }

 return TRUE;
}

 /*===========================================================================

FUNCTION LCD_CTRL_SCREEN_ONOFF

DESCRIPTION
  This function is used by the client to switch screen on or off

DEPENDENCIES
  hs_init must have been called before using this function.
  lcd_sub_srvc must have been caleed before using this function.

RETURN VALUE
  The function returns TRUE if the command could be sent successfully to the hs
  command queue and FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean lcd_ctrl_screen_onoff( lcd_handle_type lcd_handle,
                               boolean         onoff )
{

  hs_packets_type *hs_cmd_ptr;      /* command to handset */

  /* Verify handle and subscription is valid */
  if ((lcd_handle.id != HS_LCD_DEVICE) || (!lcd_subscribed) )
    return FALSE;


  /* If no available command buffers return */
  if( ( hs_cmd_ptr = (hs_packets_type *)  q_get( &hs_cmd_free_q )) == NULL )
    return FALSE;


  hs_cmd_ptr->ctl.hdr.cmd        = HS_SCRN_CTL;
  hs_cmd_ptr->ctl.hdr.task_ptr   = NULL;
  hs_cmd_ptr->ctl.hdr.done_q_ptr = &hs_cmd_free_q;
  hs_cmd_ptr->ctl.on_off         = onoff;
  hs_cmd( hs_cmd_ptr );
  return TRUE;
}

/*===========================================================================

FUNCTION KPD_CTRL_BKLIT_ONOFF

DESCRIPTION
  This function is used by the client to switch the display backlight on or off

DEPENDENCIES
  hs_init must have been called before using this function.
  lcd_sub_srvc must have been caleed before using this function.

RETURN VALUE
  The function returns TRUE if the command could be sent successfully to the hs
  command queue and FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean kpd_ctrl_bklit_onoff( kpd_handle_type kpd_handle,
                              byte            intensity  )
{

#ifdef FEATURE_PMIC_LCDKBD_LED_DRIVER
  hs_packets_type *hs_cmd_ptr;      /* command to handset */

  /* Verify handle and subscription is valid */
  if ((kpd_handle.id != HS_KPD_DEVICE) || (!kpd_subscribed) )
    return FALSE;

  /* If no available command buffers return */
  if( ( hs_cmd_ptr = (hs_packets_type *)  q_get( &hs_cmd_free_q )) == NULL )
    return FALSE;

  hs_cmd_ptr->ctl2.hdr.cmd        = HS_SET_KPD_LEVEL;
  hs_cmd_ptr->ctl2.hdr.task_ptr   = NULL;
  hs_cmd_ptr->ctl2.hdr.done_q_ptr = &hs_cmd_free_q;
  hs_cmd_ptr->ctl2.level          = intensity;
  hs_cmd( hs_cmd_ptr );
#endif
  return TRUE;
}


/*===========================================================================

FUNCTION LCD_CTRL_BKLIT_ONOFF

DESCRIPTION
  This function is used by the client to switch the display backlight on or off

DEPENDENCIES
  hs_init must have been called before using this function.
  lcd_sub_srvc must have been caleed before using this function.

RETURN VALUE
  The function returns TRUE if the command could be sent successfully to the hs
  command queue and FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean lcd_ctrl_bklit_onoff( lcd_handle_type lcd_handle,
                              byte            intensity  )
{

  hs_packets_type *hs_cmd_ptr;      /* command to handset */

  /* Verify handle and subscription is valid */
  if ((lcd_handle.id != HS_LCD_DEVICE) || (!lcd_subscribed) )
    return FALSE;

  /* If no available command buffers return */
  if( ( hs_cmd_ptr = (hs_packets_type *)  q_get( &hs_cmd_free_q )) == NULL )
    return FALSE;

  if ( intensity == 0)
  {
     lcd_backlight_on = FALSE;
  }
  else
  {
     lcd_backlight_on = TRUE;
  }

#if defined FEATURE_PMIC_LCDKBD_LED_DRIVER && !(defined T_CARMEN)
  hs_cmd_ptr->ctl2.hdr.cmd        = HS_SET_LCD_LEVEL;
  hs_cmd_ptr->ctl2.hdr.task_ptr   = NULL;
  hs_cmd_ptr->ctl2.hdr.done_q_ptr = &hs_cmd_free_q;
  hs_cmd_ptr->ctl2.level          = intensity;
  hs_cmd( hs_cmd_ptr );
#else
  hs_cmd_ptr->ctl.hdr.cmd        = HS_BACKLIGHT;
  hs_cmd_ptr->ctl.hdr.task_ptr   = NULL;
  hs_cmd_ptr->ctl.hdr.done_q_ptr = &hs_cmd_free_q;
  hs_cmd_ptr->ctl.on_off         = intensity?TRUE:FALSE;
  hs_cmd( hs_cmd_ptr );
#endif
  return TRUE;
}

/*===========================================================================

FUNCTION LCD_SET_BLINK_RATE

DESCRIPTION
  This function sets the LCD blink rate

DEPENDENCIES
  hs_init must have been called before using this function.
  lcd_sub_srvc must have been called before using this function.

RETURN VALUE
  The function returns TRUE if the command could be sent successfully to the hs
  command queue and FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean lcd_set_blink_rate( word blink_rate )
{
  hs_packets_type *hs_cmd_ptr;      /* command to handset */
  /* The HS device only supports one blink rate so set it to the latest */
  /* requested rate. The application should only set the ann or display */
  /* rate but not both to different values                              */

  if (blink_rate != hs_blink_rate) {

    if( ( hs_cmd_ptr = (hs_packets_type *)  q_get( &hs_cmd_free_q )) == NULL )
      return FALSE;

    hs_cmd_ptr->blink.hdr.cmd        = HS_BLINK_RATE;
    hs_cmd_ptr->blink.hdr.task_ptr   = NULL;
    hs_cmd_ptr->blink.hdr.done_q_ptr = &hs_cmd_free_q;
    hs_cmd_ptr->blink.blink_rate     = blink_rate;
    hs_cmd( hs_cmd_ptr );
    hs_blink_rate = blink_rate;
 }

 return TRUE;
}


/*===========================================================================

FUNCTION LCD_DISPLAY_CHARS

DESCRIPTION
  This function is used by the client to display characters on the screen

DEPENDENCIES
  hs_init must have been called before using this function.
  lcd_sub_srvc must have been called before using this function.

  The characters to be displayed shall be terminated by a NULL character.

RETURN VALUE
  The function returns TRUE if the command could be sent successfully to the hs
  command queue and FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean lcd_display_chars( lcd_handle_type lcd_handle,
                           byte           *chars,
                           byte           *bchars,
                           byte            row,
                           byte            col,
                           byte            size )
{

  int i,j,k;
  hs_packets_type *hs_cmd_ptr;      /* command to handset */

  /* Verify handle and subscription is valid */
  if ((lcd_handle.id != HS_LCD_DEVICE) || (!lcd_subscribed) )
    return FALSE;

  /* Verify buffer pointer is valid */
  if ((chars == NULL) || (bchars == NULL))
    return FALSE;

  /* If no available command buffers return */
  if( ( hs_cmd_ptr = (hs_packets_type *)  q_get( &hs_cmd_free_q )) == NULL )
    return FALSE;

  /* write the charaters into the display and blink screen buffers */
  k=0;
  for( i=row; i<HS_SCRN_HIGH; i++ ) {
     for( j=col; j<HS_SCRN_WIDE; j++ ) {
        scrntxt [i][j]  = *chars++;
        scrnblnk[i][j]  = *bchars++;
        if (++k == size)     /* stop when all charaters have been copied */
            break;
     }
  }

  /* turn the screen on */
   hs_cmd_ptr->scrna.hdr.cmd        = HS_ALL_SCRN;
   hs_cmd_ptr->scrna.hdr.task_ptr   = NULL;
   hs_cmd_ptr->scrna.hdr.done_q_ptr = &hs_cmd_free_q;
   hs_cmd_ptr->scrna.scrn_ptr       = ( hs_scrn_type * ) scrntxt;
   hs_cmd_ptr->scrna.blink_scrn_ptr = ( hs_scrn_type * ) scrnblnk;
   hs_cmd( hs_cmd_ptr );
   return TRUE;
}

/*===========================================================================

FUNCTION LCD_CLR_CHARS

DESCRIPTION
  This function is used by the client to clear characters on the screen

DEPENDENCIES
  hs_init must have been called before using this function.
  lcd_sub_srvc must have been called before using this function.

RETURN VALUE
  The function returns TRUE if the command could be sent successfully to the hs
  command queue and FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean lcd_clr_chars( lcd_handle_type lcd_handle,
                       byte            row,
                       byte            col,
                       byte            size )
{

 int i,j,k;
 hs_packets_type *hs_cmd_ptr;      /* command to handset */

  /* Verify handle and subscription is valid */
  if ((lcd_handle.id != HS_LCD_DEVICE) || (!lcd_subscribed) )
    return FALSE;

  /* If no available command buffers return */
  if( ( hs_cmd_ptr = (hs_packets_type *)  q_get( &hs_cmd_free_q )) == NULL )
    return FALSE;

 /* clear the charaters in the display screen and blink screen buffers  */
  k=0;
  for( i=row; i<HS_SCRN_HIGH; i++ ){
     for( j=col; j<HS_SCRN_WIDE; j++ ){
         scrntxt [i][j]  = ' ';
         scrnblnk[i][j]  = ' ';
         if (++k == size)     /* stop when the "size" has been reached */
            break;
    }
  }

   /* turn the screen on */
   hs_cmd_ptr->scrna.hdr.cmd        = HS_ALL_SCRN;
   hs_cmd_ptr->scrna.hdr.task_ptr   = NULL;
   hs_cmd_ptr->scrna.hdr.done_q_ptr = &hs_cmd_free_q;
   hs_cmd_ptr->scrna.scrn_ptr       = ( hs_scrn_type * ) scrntxt;
   hs_cmd_ptr->scrna.blink_scrn_ptr = ( hs_scrn_type * ) scrnblnk;
   hs_cmd( hs_cmd_ptr );
   return TRUE;
}


/*===========================================================================

FUNCTION LCD_BLINK_CHARS

DESCRIPTION
  This function is used by the client to blink characters on or off

DEPENDENCIES
  hs_init must have been called before using this function.
  lcd_sub_srvc must have been called before using this function.

  The blink rate cannot be set to different values for different
  parts of the screen.

RETURN VALUE
  The function returns TRUE if the command could be sent successfully to the hs
  command queue and FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean lcd_blink_chars( lcd_handle_type lcd_handle,
                         boolean         onoff,
                         byte            row,
                         byte            col,
                         byte            size,
                         word            blink_rate )
{

  int i,j,k;
  hs_packets_type *hs_cmd_ptr;      /* command to handset */

  /* Verify handle and subscription is valid */
  if ((lcd_handle.id != HS_LCD_DEVICE) || (!lcd_subscribed) )
    return FALSE;

  /* If no available command buffers return */
  if( ( hs_cmd_ptr = (hs_packets_type *)  q_get( &hs_cmd_free_q )) == NULL )
    return FALSE;

  k =0;
  if( blink_rate != 0) {      /* we want the display to blink. */
                              /* Clear the blink sreen buffers */
    for( i=row; i<HS_SCRN_HIGH; i++ ) {
     for( j=col; j<HS_SCRN_WIDE; j++ ) {
         scrnblnk[i][j]  = ' ';
         if (++k == size) break;
     }
    }
  }

  else {                      /* we want the display to stop blinking. Make */
                              /* the blink sreen buffers same as txt buffers*/
    for( i=row; i<HS_SCRN_HIGH; i++ ) {
     for( j=col; j<HS_SCRN_WIDE; j++ ) {
         scrnblnk[i][j]  = scrntxt[i][j];
         if (++k == size) break;
     }
    }
  }

   hs_cmd_ptr->scrn.hdr.cmd        = HS_UPDATE_BLINK;
   hs_cmd_ptr->scrn.hdr.task_ptr   = NULL;
   hs_cmd_ptr->scrn.hdr.done_q_ptr = &hs_cmd_free_q;
   hs_cmd_ptr->scrn.scrn_ptr       = ( hs_scrn_type * ) scrnblnk;
   hs_cmd( hs_cmd_ptr );

   /* The HS device only supports one blink rate so set it to the latest */
   /* requested rate. The application should only set the ann or display */
   /* rate but not both to different values                              */

   if (blink_rate != hs_blink_rate) {

    if( ( hs_cmd_ptr = (hs_packets_type *)  q_get( &hs_cmd_free_q )) == NULL )
     return FALSE;

    hs_cmd_ptr->blink.hdr.cmd        = HS_BLINK_RATE;
    hs_cmd_ptr->blink.hdr.task_ptr   = NULL;
    hs_cmd_ptr->blink.hdr.done_q_ptr = &hs_cmd_free_q;
    hs_cmd_ptr->blink.blink_rate     = blink_rate;
    hs_cmd( hs_cmd_ptr );
    hs_blink_rate = blink_rate;
   }

   return TRUE;
}


/*===========================================================================

FUNCTION LCD_BLINK_SCREEN_CHARS

DESCRIPTION
  This function is used by the client to blink characters on or off
  A ' ' character as part of the chars[] input array denotes that
  the character should blink.  Other chars should NOT blink.

DEPENDENCIES
  hs_init must have been called before using this function.
  lcd_sub_srvc must have been called before using this function.

  The blink rate cannot be set to different values for different
  parts of the screen.

RETURN VALUE
  The function returns TRUE if the command could be sent successfully to the hs
  command queue and FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean lcd_blink_screen_chars( lcd_handle_type lcd_handle,
                         byte           *chars,
                         byte            row,
                         byte            col,
                         byte            size,
                         word            blink_rate )
{

  int i,j,k;
  hs_packets_type *hs_cmd_ptr;      /* command to handset */

  /* Verify handle and subscription is valid */
  if ((lcd_handle.id != HS_LCD_DEVICE) || (!lcd_subscribed) )
    return FALSE;

  /* If no available command buffers return */
  if( ( hs_cmd_ptr = (hs_packets_type *)  q_get( &hs_cmd_free_q )) == NULL )
    return FALSE;

  k =0;
  if( blink_rate != 0) {      /* we want the display to blink. */
                              /* Clear the blink sreen buffers */
    for( i=row; i<HS_SCRN_HIGH; i++ ) {
     for( j=col; j<HS_SCRN_WIDE; j++ ) {
       scrnblnk[i][j] = chars[k];
         if (++k == size) break;
     }
    }
  }

  else {                      /* we want the display to stop blinking. Make */
                              /* the blink sreen buffers same as txt buffers*/
    for( i=row; i<HS_SCRN_HIGH; i++ ) {
     for( j=col; j<HS_SCRN_WIDE; j++ ) {
         scrnblnk[i][j]  = scrntxt[i][j];
         if (++k == size) break;
     }
    }
  }

   hs_cmd_ptr->scrn.hdr.cmd        = HS_UPDATE_BLINK;
   hs_cmd_ptr->scrn.hdr.task_ptr   = NULL;
   hs_cmd_ptr->scrn.hdr.done_q_ptr = &hs_cmd_free_q;
   hs_cmd_ptr->scrn.scrn_ptr       = ( hs_scrn_type * ) scrnblnk;
   hs_cmd( hs_cmd_ptr );

   /* The HS device only supports one blink rate so set it to the latest */
   /* requested rate. The application should only set the ann or display */
   /* rate but not both to different values                              */

   if (blink_rate != hs_blink_rate) {

    if( ( hs_cmd_ptr = (hs_packets_type *)  q_get( &hs_cmd_free_q )) == NULL )
     return FALSE;

    hs_cmd_ptr->blink.hdr.cmd        = HS_BLINK_RATE;
    hs_cmd_ptr->blink.hdr.task_ptr   = NULL;
    hs_cmd_ptr->blink.hdr.done_q_ptr = &hs_cmd_free_q;
    hs_cmd_ptr->blink.blink_rate     = blink_rate;
    hs_cmd( hs_cmd_ptr );
    hs_blink_rate = blink_rate;
   }

   return TRUE;
}

/*===========================================================================

FUNCTION LCD_CLR_SCREEN

DESCRIPTION
  This function is used by the client to clear the entire screen


DEPENDENCIES
  hs_init must have been called before using this function.
  lcd_sub_srvc must have been called before using this function.

RETURN VALUE
  The function returns TRUE if the command could be sent successfully to the hs
  command queue and FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean lcd_clr_screen( lcd_handle_type lcd_handle)
{
   return ( lcd_clr_chars(  lcd_handle, sizeof (scrntxt),0,0 ) );
}


/*===========================================================================

FUNCTION LCD_SNAPSHOT

DESCRIPTION
  TThis function is used by the client to capture the snapshot of the entire screen


DEPENDENCIES
  hs_init must have been called before using this function.
  lcd_sub_srvc must have been called before using this function.

RETURN VALUE
  The function returns TRUE if the command could be sent successfully to the hs
  command queue and FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean lcd_snapshot( lcd_handle_type         lcd_handle,
                      lcd_cb_f_snapshot_type *lcd_cb_func)
{

  hs_packets_type *hs_cmd_ptr;      /* command to handset */

  /* Verify handle and subscription is valid */
  if ((lcd_handle.id != HS_LCD_DEVICE) || (!lcd_subscribed) )
    return FALSE;

  /* If no available command buffers return */
  if( ( hs_cmd_ptr = (hs_packets_type *) q_get( &hs_cmd_free_q )) == NULL )
    return FALSE;

  hs_cmd_ptr->snpsht.hdr.cmd           = HS_SNAPSHOT;
  hs_cmd_ptr->snpsht.hdr.task_ptr      = NULL;
  hs_cmd_ptr->snpsht.hdr.done_q_ptr    = &hs_cmd_free_q;
  hs_cmd_ptr->snpsht.lcd_snapshot_func = lcd_cb_func;
  hs_cmd_ptr->snpsht.snapshot          = &snapshot;
  hs_cmd( hs_cmd_ptr );
  return TRUE;
}


/*===========================================================================

FUNCTION LCD_SET_CHARMAP

DESCRIPTION
  TThis function is used by the client to program the individual char maps.


DEPENDENCIES
  hs_init must have been called before using this function.
  lcd_sub_srvc must have been called before using this function.

RETURN VALUE
  The function returns TRUE if the command could be sent successfully to the hs
  command queue and FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean lcd_set_charmap( lcd_handle_type         lcd_handle,
                         byte                    index,
                         byte                    size,
                         byte                    *data)
{
  int i;
  hs_packets_type *hs_cmd_ptr;      /* command to handset */

  /* Verify handle and subscription is valid */
  if ((lcd_handle.id != HS_LCD_DEVICE) || (!lcd_subscribed) )
    return FALSE;

  /* If no available command buffers return */
  if( ( hs_cmd_ptr = (hs_packets_type *) q_get( &hs_cmd_free_q )) == NULL )
    return FALSE;

  hs_cmd_ptr->glyph.hdr.cmd           = HS_SET_GLYPH;
  hs_cmd_ptr->glyph.hdr.task_ptr      = NULL;
  hs_cmd_ptr->glyph.hdr.done_q_ptr    = &hs_cmd_free_q;
  hs_cmd_ptr->glyph.index             = index;

  size = (size < HS_GLYPH_HEIGHT ) ? size : HS_GLYPH_HEIGHT;
  /* Now fill the bitmap */
  for (i=0; i<size; i++) {
    hs_cmd_ptr->glyph.bitmap[i] = data[i];
  }

  hs_cmd( hs_cmd_ptr );
  return TRUE;
}
/*===========================================================================

FUNCTION LCD_GET_BACKLIGHT_VALUE

DESCRIPTION
  This function returns the status of the backlight value

DEPENDENCIES
  hs_init must have been called before using this function.
  lcd_sub_srvc must have been caleed before using this function.

RETURN VALUE
  The function returns FALSE if the back light is OFF and TRUE
   otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean lcd_get_backlight_value( void)
{

  return lcd_backlight_on;
}

#ifdef FEATURE_UI_CORE_REMOVED
/*========================================================================

FUNCTION      LCD_MESSAGE

DESCRIPTION
              Display the specified message on the LCD, then put the
              phone into download mode.
              
              This function is only called after a fatal error.  We
              wish to inform the user of the error, then put the 
              phone in download mode so that new (and hopefully better)
              code can be downloaded to the phone.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

========================================================================*/

void lcd_message (char* msg)
{
  //lcd_debug_message(msg);
}

#endif //#ifdef FEATURE_UI_CORE_REMOVED
#endif /* FEATURE_BSPUI_API */
