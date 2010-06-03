#ifndef HSUI_H
#define HSUI_H
/*===========================================================================

              UI - HS INTERFACE   H E A D E R   F I L E

DESCRIPTION
  This file contains the data structures used to interface User Interface to the
  Handset Driver.

Copyright (c) 1999 - 2001 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath:  L:/src/asw/MSM6050/vcs/hsui.h_v   1.1   23 Oct 2001 18:34:12   donb  $
$Header: //depot/asic/msmshared/services/hs/hsui.h#5 $ $DateTime: 2004/05/24 17:31:35 $ $Author: jachan $

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/10/02   pd      Added multiple key press support
04/30/01   kar     Added function lcd_set_blink_rate. Changed parameters
                   to lcd_display_chars to prevent redundant calls to HS
10/15/99   jc      created the file
===========================================================================*/

#include "comdef.h"     /* Definitions for byte, word, etc.     */
#include "hs.h"         /* Definition of HS definitions         */

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/




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
extern boolean kpd_reg_key_event(kpd_handle_type          kpd_handle,
#ifdef FEATURE_KEYPAD_MULTI_KEY
                                  keypad_key_event_cb_f_type *kpd_cb_function);
#else /* FEATURE_KEYPAD_MULTI_KEY */
#error code not present
#endif /* FEATURE_KEYPAD_MULTI_KEY */


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
 extern boolean kpd_sub_srvc( kpd_cb_f_handle_type *kpd_cb_function);


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
extern boolean lcd_sub_srvc( lcd_cb_f_handle_type *lcd_cb_function);


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
 extern boolean lcd_cancel_srvc(lcd_handle_type lcd_handle);


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
 extern boolean lcd_ctrl_ann_onoff(lcd_handle_type         lcd_handle,
                                   lcd_ann_enum_type       lcd_ann,
                                   boolean                 onoff,
                                   lcd_ann_value_enum_type value);


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
 extern boolean lcd_blink_ann(lcd_handle_type         lcd_handle,
                              lcd_ann_enum_type       lcd_ann,
                              lcd_ann_value_enum_type value,
                              word                    blink_rate);


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
 extern boolean lcd_ctrl_screen_onoff( lcd_handle_type lcd_handle,
                                       boolean         onoff );


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
 extern boolean kpd_ctrl_bklit_onoff( kpd_handle_type kpd_handle,
                                      byte            intensity   );



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
 extern boolean lcd_ctrl_bklit_onoff( lcd_handle_type lcd_handle,
                                      byte            intensity   );

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
extern boolean lcd_set_blink_rate( word blink_rate );


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
 extern boolean lcd_display_chars( lcd_handle_type lcd_handle,
                                   byte           *chars,
                           byte           *bchars,
                                   byte            row,
                           byte            col,
                           byte            size );



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
 extern boolean lcd_clr_chars( lcd_handle_type lcd_handle,
                               byte            row,
                               byte            col,
                               byte            size );



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
 extern boolean lcd_blink_chars( lcd_handle_type lcd_handle,
                                 boolean         onoff,
                                 byte            row,
                                 byte            col,
                                 byte            size,
                                 word            blink_rate );


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
 extern boolean lcd_blink_screen_chars( lcd_handle_type lcd_handle,
                                 byte           *chars,
                                 byte            row,
                                 byte            col,
                                 byte            size,
                                 word            blink_rate );


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
 extern boolean lcd_clr_screen( lcd_handle_type lcd_handle);



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
 extern boolean lcd_snapshot( lcd_handle_type         lcd_handle,
                              lcd_cb_f_snapshot_type *lcd_cb_func);



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
 extern boolean lcd_set_charmap( lcd_handle_type         lcd_handle,
                                 byte                    index,
                                 byte                    size,
                                 byte                    *data);

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
extern boolean lcd_get_backlight_value( void);

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
extern void lcd_message (char* msg);
#endif //#ifdef FEATURE_UI_CORE_REMOVED

#endif /* HSUI_H */

