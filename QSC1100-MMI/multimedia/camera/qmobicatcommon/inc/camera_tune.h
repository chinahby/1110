#ifndef CAMERA_TUNE_DOT_H
#define CAMERA_TUNE_DOT_H


/*////////////////////////////////////////////////////////////////////////////

  C A M E R A   T U N E

  FILE:        camera_tune.h

  COPYRIGHT:   Copyright (c) 2005, 2006, 2007 by Qualcomm, Incorporated.
               All Rights Reserved.

  (runtime tuning, that is)

  (1)  reads and writes configurations from and to EFS,

  (2)  reads and writes configurations in binary for
       embedding in EXIF headers (and possibly elsewhere),

  (3)  holds the keypress shortcut interpreter,

  (4)  holds the keypresses -> numbers interpreter, 

  and provides a place for whatever else doesn't belong in the BREW apps.

  
  HISTORY:

  $Header: //source/qcom/qct/multimedia/camera/qmobicatcommon/main/latest/inc/camera_tune.h#1 $
  $DateTime: 2008/05/09 16:40:03 $ 
  $Author: karenp $

  
  REVISION HISTORY:
  
  when        who       what, where, why
  ----------  -----     -------------------------------------------------------
  2007-10-18  jgarber   Added camera_tune_stop() for cleanup to occur.
  2007-05-29  jgarber   Added functions for new data exchange protocol (for PC)
  2007-02-14  jgarber   Made some parameters const.
  2006-10-06  jgarber   Initial rewrite of QMobiCaT engine to use
                        camera services tunable parameters API.


  DESCRIPTION:
  
  The Mobile Tuning tool for Camera Services relies on BREW
  apps Qcamera (and eventually Qcamcorder) for its
  ability to print and alter camera system variables
  on-screen.
  
  The Mobile Tuning tool also relies on EFS2 so that settings
  can be read from and written to a configuration file.
  
  Every assignment binds a value to a variable, so the 
  configuration file will be a sequence of such bindings.
   
  The config file syntaxes currently supported is based on
  Microsoft's .INI files.

  The lexical structure therefore consists of the following,
  where whitespace in the description corresponds to free
  "whitespace" in the configuration file.


  Config File   :   sequence of Assignments

  Assignment    :   Variable = Value [,Value]*
  
  Variable      :   matches the regular expression [a-zA-Z_0-9]+
  
  Number        :   matches the regular expression [+-]?[0-9]*(.[0-9]*)?
  
  Value         :   either an Integer
                    or a list of Integers separated by commas.


  Comments are delimited by ';' as in .INI.
  (These choices are easy to parse with a little parser.)

  This assumes ASCII, not Unicode.

  One concern with the choice of Microsoft .INI syntax 
  is that it does not allow expression of recursive structures.

  Here is a 5-line example as a Microsoft .INI configuration file.

                               ; A comment looks like this.
  [ sectionname ]              ; These are in the .INI Microsoft files.
  black_level  = 0             ; .. assignment to a scalar..
  little_table = 4, 16 , 256   ; blah
                               ; whitespace is not significant

  The Mobile Tuning tool has several screens, and each screen
  displays exactly one editable variable and its current value.

  These screens may be rotated among with the left & right
  direction keys.

  On any screen, the editable value may be altered with the
  up & down direction keys.

  This file defines two key-handling sequences.

    (*) initiatates spelling of a variable name with keys,
        which ends with another (*).

    (#) initiates entry of a number, which ends with another (#).

  The variable names can be used as a shortcut to a desired
  Mobile Tuning screen, while the number entry can be a shortcut
  to alter the value on such a screen.

  MobiCaT is essentially a UI to access a list of internal variables.
  The entries correspond to the things which can be 
  observed and altered at runtime.  The struct below codifies that.

  The nickname is what a config file knows
  a runtime observable/alterable entity as.
  That entity can optionally 
  (1)  respond to events from the camera software,
  (2)  know how to read its new value from a string,
  (3)  know how to print its value to a string, and
  (4)  know how to print its value to the jpeg encoder's
       proprietary app7 header encoding.


*/////////////////////////////////////////////////////////////////


//
//  Implemented in camera_tune.c ; called from camsensor.c .
//

extern void  camera_tune_init ( void );
extern void  camera_tune_stop ( void );


#ifdef  FEATURE_CAMERA_MOBICAT


// Error codes from QMobiCaT functions
#include "camera.h"
#define CAMERA_TUNE_ERROR_SUCCESS 0
#define CAMERA_TUNE_ERROR_INVALID_STATE (-CAMERA_INVALID_STATE)
#define CAMERA_TUNE_ERROR_INVALID_PARM (-CAMERA_INVALID_PARM)
#define CAMERA_TUNE_ERROR_NOT_SUPPORTED (-CAMERA_NOT_SUPPORTED)
#define CAMERA_TUNE_ERROR_FAILED (-CAMERA_FAILED)

  //
  //  Implemented in camera_tune.c ; called from camera_svcs_diag.c :
  //

  extern  int   camera_tune_write_dot_ini_to_filename ( const char * file   );
  extern  int   camera_tune_read_dot_ini_file         ( const char * file   ); // also for automation

  //
  //  Implemented in camera_tune.c ; called from qcamera.c which
  //  #includes camera_tune_ui_for_brew.c :
  //

  extern  void  camera_tune_lcd_ui_updater            ( void          );
  extern  char  camera_tune_lcd_ui_keypress_handler   ( uint16  key   );

  // called from diag, automation and elsewhere
  extern  int   camera_tune_write_dot_ini_files       ( void ); // uses default file name
  extern  int   camera_tune_get_list                  ( uint16 num_to_start, byte *data, uint32 data_size );
  extern  int   camera_tune_get_parms                 ( byte *request, uint32 request_size,
                                                        byte *response, uint32 response_size );
  extern  int   camera_tune_set_parms                 ( byte *request, uint32 request_size,
                                                        byte *status );
  extern  int   camera_tune_misc_command              ( byte *request, uint32 request_size,
                                                        byte *response, uint32 response_size );

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif // FEATURE_APP_TEST_AUTOMATION

  //
  //  To be implemented in camera_tune_ui_for_*.c :
  //
  extern  void  camera_tune_lcd_ui_toggle_visibility  ( void          );
  extern  void  camera_tune_lcd_ui_print              ( char * string );

#endif


#endif  //  CAMERA_TUNE_DOT_H

