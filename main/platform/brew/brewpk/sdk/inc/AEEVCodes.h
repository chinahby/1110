#ifndef AEEVCODES_H
#define AEEVCODES_H
/*===================================================================
FILE: AEEVCodes.h

SERVICES:  AEE Virtual Codes and Type Definitions

DESCRIPTION:
  This header defines virtual codes and types for for Key, Screen,
  LED, etc.

PUBLIC CLASSES AND FUNCTIONS:

All Rights Reserved.
Qualcomm Confidential and Proprietary
===================================================================*/

#include "../../inc/AEEStdDef.h"

//-------------------------------------------------------------------
// Defines
//-------------------------------------------------------------------
#define AEE_INVALID_CODE 0xE010

// Base offset

#define AEE_AVK_BASE 0xE020 // For Keys
#define AEE_AVS_BASE 0      // For Screens
#define AEE_AVL_BASE 0      // For LEDs

#define AVK_MAX_OVERLOAD 5

//
// EVT_KEY dwParam bits.  These bits are sent as "dwParam" values to EVT_KEY, EVT_KEY_PRESS, etc.
//

#define KB_LSHIFT      0x00000001   // Left Shift Key
#define KB_RSHIFT      0x00000002   // Right Shift Key
#define KB_LCTRL       0x00000004   // Left Control Key
#define KB_RCTRL       0x00000008   // Right Control Key
#define KB_LALT        0x00000010   // Left Alt/Menu Key
#define KB_RALT        0x00000020   // Right Alt/Menu Key
#define KB_AUTOREPEAT  0x00000040   // Auto-repeat bit (1 if auto-repeat key event)
#define KB_EXTERNAL    0x00000080   // External Keyboard bit (1 if key evt from external keyboard)
#define KB_FUNCTION    0x00000100   // Function key
#define KB_SYMBOL      0x00000200   // Symbol key
                                

// Auto-Repeat
//
// Keys are auto-repeated starting KB_AUTOREPEAT_START msecs after the initial EVT_KEY 
// is received.  The app will then start receiving EVT_KEY with the KB_AUTOREPEAT bit
// set every KB_AUTOREPEAT_RATE msecs.
//

#define KB_AUTOREPEAT_START   (1500)
#define KB_AUTOREPEAT_RATE    (100)


// Bits for sticky modifier keys

#define KB_CAPSLOCK    0x00010000   // Caps Lock Key
#define KB_SCROLLLK    0x00020000   // Scroll Lock Key
#define KB_NUMLOCK     0x00040000   // Num Lock Key

// Macro to check Key state of a key

#define IS_KEY_DOWN(v,k) (boolean)((v)&(k)? TRUE : FALSE)

//
// EVT_KEY, EVT_KEY_PRESS, EVT_KEY_RELEASE virtual keys
//

enum _AVKType {
   AVK_UNDEFINED = AEE_INVALID_CODE,    // hex 0xE010; dec 57360
   AVK_FIRST = AEE_AVK_BASE,            // hex 0xE020; dec 57376

   AVK_0,                               // hex 0xE021; dec 57377
   AVK_1,                               // hex 0xE022; dec 57378
   AVK_2,                               // hex 0xE023; dec 57379
   AVK_3,                               // hex 0xE024; dec 57380
   AVK_4,                               // hex 0xE025; dec 57381
   AVK_5,                               // hex 0xE026; dec 57382
   AVK_6,                               // hex 0xE027; dec 57383
   AVK_7,                               // hex 0xE028; dec 57384
   AVK_8,                               // hex 0xE029; dec 57385
   AVK_9,                               // hex 0xE02A; dec 57386
   AVK_STAR,                            // hex 0xE02B; dec 57387
   AVK_POUND,                           // hex 0xE02C; dec 57388

   AVK_POWER,                           // hex 0xE02D; dec 57389
   AVK_END,                             // hex 0xE02E; dec 57390
   AVK_SEND,                            // hex 0xE02F; dec 57391
   AVK_CLR,                             // hex 0xE030; dec 57392

   AVK_UP,                              // hex 0xE031; dec 57393
   AVK_DOWN,                            // hex 0xE032; dec 57394
   AVK_LEFT,                            // hex 0xE033; dec 57395
   AVK_RIGHT,                           // hex 0xE034; dec 57396
   AVK_SELECT,                          // hex 0xE035; dec 57397

   AVK_SOFT1,                           // hex 0xE036; dec 57398
   AVK_SOFT2,                           // hex 0xE037; dec 57399
   AVK_SOFT3,                           // hex 0xE038; dec 57400
   AVK_SOFT4,                           // hex 0xE039; dec 57401

   AVK_FUNCTION1,                       // hex 0xE03A; dec 57402
   AVK_FUNCTION2,                       // hex 0xE03B; dec 57403
   AVK_FUNCTION3,                       // hex 0xE03C; dec 57404

   AVK_FUNCTION,                        // hex 0xE03D; dec 57405
   AVK_MENU,                            // hex 0xE03E; dec 57406
   AVK_INFO,                            // hex 0xE03F; dec 57407

   AVK_SHIFT,                           // hex 0xE040; dec 57408

   AVK_MESSAGE,                         // hex 0xE041; dec 57409
   AVK_MUTE,                            // hex 0xE042; dec 57410

   AVK_STORE,                           // hex 0xE043; dec 57411
   AVK_RECALL,                          // hex 0xE044; dec 57412

   AVK_PUNC1,                           // hex 0xE045; dec 57413
   AVK_PUNC2,                           // hex 0xE046; dec 57414

   AVK_VOLUME_UP,                       // hex 0xE047; dec 57415
   AVK_VOLUME_DOWN,                     // hex 0xE048; dec 57416

   AVK_WEB_ACCESS,                      // hex 0xE049; dec 57417
   AVK_VOICE_MEMO,                      // hex 0xE04A; dec 57418
   AVK_SPEAKER,                         // hex 0xE04B; dec 57419
   AVK_TAP,                             // hex 0xE04C; dec 57420

   AVK_PTT,                             // hex 0xE04D; dec 57421
   AVK_LSHIFT,                          // hex 0xE04E; dec 57422
   AVK_RSHIFT,                          // hex 0xE04F; dec 57423
   AVK_LCTRL,                           // hex 0xE050; dec 57424
   AVK_RCTRL,                           // hex 0xE051; dec 57425
   AVK_LALT,                            // hex 0xE052; dec 57426
   AVK_RALT,                            // hex 0xE053; dec 57427
   AVK_CAPLK,                           // hex 0xE054; dec 57428
   AVK_SCRLK,                           // hex 0xE055; dec 57429
   AVK_NUMLK,                           // hex 0xE056; dec 57430
   AVK_LNGHANGUL,                       // hex 0xE057; dec 57431
   AVK_LNGJUNJA,                        // hex 0xE058; dec 57432
   AVK_LNGFIN,                          // hex 0xE059; dec 57433
   AVK_LNGHANJA,                        // hex 0xE05A; dec 57434
   AVK_PRSCRN,                          // hex 0xE05B; dec 57435
   AVK_BREAK,                           // hex 0xE05C; dec 57436
   AVK_TXINSERT,                        // hex 0xE05D; dec 57437
   AVK_TXDELETE,                        // hex 0xE05E; dec 57438
   AVK_TXHOME,                          // hex 0xE05F; dec 57439
   AVK_TXEND,                           // hex 0xE060; dec 57440
   AVK_TXPGUP,                          // hex 0xE061; dec 57441
   AVK_TXPGDOWN,                        // hex 0xE062; dec 57442

   // Gamepad Keys
   AVK_GP_1,    // Gamepad Key 1                       hex 0xE063; dec 57443
   AVK_GP_2,    // Gamepad Key 2                       hex 0xE064; dec 57444
   AVK_GP_3,    // Gamepad Key 3                       hex 0xE065; dec 57445
   AVK_GP_4,    // Gamepad Key 4                       hex 0xE066; dec 57446
   AVK_GP_5,    // Gamepad Key 5                       hex 0xE067; dec 57447
   AVK_GP_6,    // Gamepad Key 6                       hex 0xE068; dec 57448
   AVK_GP_SL,   // Gamepad Key Shoulder Left           hex 0xE069; dec 57449
   AVK_GP_SR,   // Gamepad Key Shoulder Right          hex 0xE06A; dec 57450

   AVK_CAMERA,       // Access device camera (typically, for snapshot/camcorder functionality)    hex 0xE06B; dec 57451
   AVK_VIDEO_PHONE,  // Start video telephony session/app                                         hex 0xE06C; dec 57452

   AVK_APP_SWITCH,   // Switch among loaded apps to make one of them top visible                  hex 0xE06D; dec 57453
   
   // Multimedia keys.
   AVK_PLAY,      // can be overloaded with the pause key                    hex 0xE06E; dec 57454
   AVK_PAUSE,     // only if there is a separate pause key                   hex 0xE06F; dec 57455
   AVK_SKIP_FWD,  // skip forward                                            hex 0xE070; dec 57456
   AVK_SKIP_RWD,  // skip backward                                           hex 0xE071; dec 57457
   AVK_FFWD,      // fast-forward                                            hex 0xE072; dec 57458
   AVK_RWD,       // rewind                                                  hex 0xE073; dec 57459
   AVK_STOP,      // stop                                                    hex 0xE074; dec 57460

   AVK_HEADSET_FUNC, // headset key                                          hex 0xE075; dec 57461

   //Full set of function keys
   AVK_FUNCTION4,                                                         // hex 0xE076; dec 57462
   AVK_FUNCTION5,                                                         // hex 0xE077; dec 57463
   AVK_FUNCTION6,                                                         // hex 0xE078; dec 57464
   AVK_FUNCTION7,                                                         // hex 0xE079; dec 57465
   AVK_FUNCTION8,                                                         // hex 0xE07A; dec 57466
   AVK_FUNCTION9,                                                         // hex 0xE07B; dec 57467
   AVK_FUNCTION10,                                                        // hex 0xE07C; dec 57468
   AVK_FUNCTION11,                                                        // hex 0xE07D; dec 57469
   AVK_FUNCTION12,                                                        // hex 0xE07E; dec 57470

   // Keys for extended keypads
   AVK_SYMBOL, // Symbol (modifier key)                                   // hex 0xE07F; dec 57471
   AVK_ENTER,  // Enter key                                               // hex 0xE080; dec 57472
   AVK_SPACE,  // Space bar                                               // hex 0xE081; dec 57473
#ifdef CUST_EDITION	
   // 接上耳机
   AVK_HEADSET_CONNECT,
   
   // 断开耳机
   AVK_HEADSET_DISCONNECT,

   // 接上耳机
   AVK_HEADSET_SWITCH,
   
   // 断开耳机
   AVK_HEADSET_SWITCH_OFF,   
   
   // 挂机
   AVK_ENDCALL,     /*0xe079*/
   AVK_BGRUN,
   AVK_MORSE_BG,
   AVK_BGPLAY,

   AVK_WITHDRAW,
   // 翻盖、滑盖
   AVK_FLIP_OPEN,
   AVK_FLIP_CLOSE,
   AVK_USER_HEADSET,/*User press headset switch*/
   AVK_GSENSOR_FORWARD,
   AVK_GSENSOR_BACKWARD, 
   AVK_GSENSOR_STAND,
   AVK_GSENSOR_LEFT,
   AVK_GSENSOR_RIGHT,
   AVK_GSENSOR_UPEND,
   AVK_GSENSOR_STEP,
      // 接上耳机
   AVK_BT_HEADSET_CONNECT,
   
   // 断开耳机
   AVK_BT_HEADSET_DISCONNECT,
   AVK_CONTACTS_GIVEUP_EDIT,
#endif /*CUST_EDITION*/   
   AVK_LAST
};
#ifdef _WIN32
typedef unsigned AVKType;
#else
typedef uint16 AVKType;
#endif // _WIN32

// Alphabet keys
//
// Convenience defines for alphabet keys
// Each alphabet key is equal to the unicode
// value of the equivalent lower-case letter
//
#define AVK_A     (97)
#define AVK_B     (AVK_A + 1)   
#define AVK_C     (AVK_A + 2)
#define AVK_D     (AVK_A + 3)
#define AVK_E     (AVK_A + 4)
#define AVK_F     (AVK_A + 5) 
#define AVK_G     (AVK_A + 6)   
#define AVK_H     (AVK_A + 7) 
#define AVK_I     (AVK_A + 8) 
#define AVK_J     (AVK_A + 9) 
#define AVK_K     (AVK_A + 10) 
#define AVK_L     (AVK_A + 11) 
#define AVK_M     (AVK_A + 12) 
#define AVK_N     (AVK_A + 13) 
#define AVK_O     (AVK_A + 14) 
#define AVK_P     (AVK_A + 15) 
#define AVK_Q     (AVK_A + 16) 
#define AVK_R     (AVK_A + 17) 
#define AVK_S     (AVK_A + 18) 
#define AVK_T     (AVK_A + 19) 
#define AVK_U     (AVK_A + 20) 
#define AVK_V     (AVK_A + 21) 
#define AVK_W     (AVK_A + 22) 
#define AVK_X     (AVK_A + 23) 
#define AVK_Y     (AVK_A + 24) 
#define AVK_Z     (AVK_A + 25) 

// AEE Virtual Screen codes
enum _AVSType {
   AVS_UNDEFINED = AEE_INVALID_CODE,

   AVS_FIRST = AEE_AVS_BASE,

   AVS_SCREEN_0,
   AVS_SCREEN_1,
   AVS_SCREEN_2,
   AVS_SCREEN_3,

   AVS_LAST
};
#ifdef _WIN32
typedef unsigned AVSType;
#else
typedef uint16 AVSType;
#endif // _WIN32

// AEE Virtual LED codes

enum _AVLType {
   AVL_UNDEFINED = AEE_INVALID_CODE,

   AVL_FIRST = AEE_AVL_BASE,

   AVL_LED_0,
   AVL_LED_1,
   AVL_LED_2,
   AVL_LED_3,
   AVL_LED_4,
   AVL_LED_5,
   AVL_LED_6,
   AVL_LED_7,

   AVL_LAST
};
#ifdef _WIN32
typedef unsigned AVLType;
#else
typedef uint16 AVLType;
#endif // _WIN32

// OEM Vcodes range is reserved from the end of Unicode Private Area (0xF8FF).
// The range has 256 entries for OEM's to define and track their own Vcodes.
#define AVK_OEM_RANGE_START  0xF800
#define AVK_OEM_RANGE_END    0xF8FF

/*=====================================================================
DATA STRUCTURE DOCUMENTATION
=======================================================================
KB_LSHIFT

Description:
   This is a modifier flag for key events. The dwParam parameter of key
   events will have this bit mask flag set when the key event is 
   generated with the left shift key also pressed.

Definition:
   #define KB_LSHIFT      0x00000001

Members:
   None

Comments:
   None

See Also:
   Other bit modifier flags for key events.
=======================================================================
KB_RSHIFT

Description:
   This is a modifier flag for key events. The dwParam parameter of key
   events will have this bit mask flag set when the key event is 
   generated with the right shift key also pressed.

Definition:
   #define KB_RSHIFT      0x00000002

Members:
   None

Comments:
   None

See Also:
   Other bit modifier flags for key events.
=======================================================================
KB_LCTRL

Description:
   This is a modifier flag for key events. The dwParam parameter of key
   events will have this bit mask flag set when the key event is 
   generated with the left control key also pressed.

Definition:
   #define KB_LCTRL       0x00000004

Members:
   None

Comments:
   None

See Also:
   Other bit modifier flags for key events.
=======================================================================
KB_RCTRL

Description:
   This is a modifier flag for key events. The dwParam parameter of key
   events will have this bit mask flag set when the key event is 
   generated with the right control key also pressed.

Definition:
   #define KB_RCTRL       0x00000008

Members:
   None

Comments:
   None

See Also:
   Other bit modifier flags for key events.
=======================================================================
KB_LALT

Description:
   This is a modifier flag for key events. The dwParam parameter of key
   events will have this bit mask flag set when the key event is 
   generated with the left alt/menu key also pressed.

Definition:
   #define KB_LALT        0x00000010

Members:
   None

Comments:
   None

See Also:
   Other bit modifier flags for key events.
=======================================================================
KB_RALT

Description:
   This is a modifier flag for key events. The dwParam parameter of key
   events will have this bit mask flag set when the key event is 
   generated with the right alt/menu key also pressed.

Definition:
   #define KB_RALT        0x00000020

Members:
   None

Comments:
   None

See Also:
   Other bit modifier flags for key events.
=======================================================================
KB_AUTOREPEAT

Description:
   This is a modifier flag for key events. The dwParam parameter of key
   events will have this bit mask flag set when the key event is 
   generated due to a continuous key press. In this case as the key
   is continuously pressed BREW does not receive the key release event
   (from the OEM layer) and hence the EVT_KEY is generated repeatedly 
   from BREW layer based on a autorepeat timer.

   Key events are auto-repeated starting KB_AUTOREPEAT_START 
   milliseconds after the initial EVT_KEY is generated. The app will
   then start receiving EVT_KEY with the KB_AUTOREPEAT bit set every 
   KB_AUTOREPEAT_RATE milliseconds.

Definition:
   #define KB_AUTOREPEAT  0x00000040

Members:
   None

Comments:
   None

See Also:
   Other bit modifier flags for key events.
=======================================================================
KB_CAPSLOCK

Description:
   This is a modifier flag for key events. The dwParam parameter of key
   events will have this bit mask flag set when the key event is 
   generated with the caps lock on.

Definition:
   #define KB_CAPSLOCK    0x00010000

Members:
   None

Comments:
   None

See Also:
   Other bit modifier flags for key events.
=======================================================================
KB_SCROLLLK

Description:
   This is a modifier flag for key events. The dwParam parameter of key
   events will have this bit mask flag set when the key event is 
   generated with the scroll lock on.

Definition:
   #define KB_SCROLLLK    0x00020000

Members:
   None

Comments:
   None

See Also:
   Other bit modifier flags for key events.
=======================================================================
KB_NUMLOCK

Description:
   This is a modifier flag for key events. The dwParam parameter of key
   events will have this bit mask flag set when the key event is
   generated with the number lock on.

Definition:
   #define KB_NUMLOCK  0x00040000

Members:
   None

Comments:
   None

See Also:
   Other bit modifier flags for key events.
=======================================================================
KB_EXTERNAL

Description:
   This is a modifier flag for key events. The dwParam parameter of key
   events will have this bit mask flag set when the key event is generated
   from an external keyboard to the device.

Definition:
   #define KB_EXTERNAL    0x00000080

Members:
   None

Comments:
   None

See Also:
   Other bit modifier flags for key events.

=============================================================================
KB_FUNCTION

Description:
   This is a modifier flag for key events. The dwParam paramater of key
   events will have this bit mask flag set when the Function key is pressed 
   in combination with another key. The Function key is often used on extended
   keypads such as QWERTY. This key changes or overloads the meaning of the 
   key with which it is pressed in combination. 
 
Definition:
   #define KB_FUNCTION    0x00000100   

Members:
   None

Comments:
   This modifier is only applicable if the device keypad supports a Function 
   key that can overload other keys.

See Also:
   Other bit modifier flags for key events.
=============================================================================
KB_SYMBOL

Description:
   This is a modifier flag for key events. The dwParam paramater of key
   events will have this bit mask flag set when the Symbol key is pressed 
   in combination with another key. The Symbol key is often used on extended
   keypads such as QWERTY. This key changes or overloads the meaning of the 
   key with which it is pressed in combination. 

Definition:
   #define KB_SYMBOL    0x00000200 

Members:
   None

Comments:
   This modifier is only applicable if the device keypad supports a Symbol 
   key that can overload other keys.

See Also:
   Other bit modifier flags for key events.
=============================================================================
AVKType

Description:

Definition:
   enum _AVKType {
   AVK_UNDEFINED = AEE_INVALID_CODE,    // hex 0xE010; dec 57360
   AVK_FIRST = AEE_AVK_BASE,            // hex 0xE020; dec 57376

   AVK_0,                               // hex 0xE021; dec 57377
   AVK_1,                               // hex 0xE022; dec 57378
   AVK_2,                               // hex 0xE023; dec 57379
   AVK_3,                               // hex 0xE024; dec 57380
   AVK_4,                               // hex 0xE025; dec 57381
   AVK_5,                               // hex 0xE026; dec 57382
   AVK_6,                               // hex 0xE027; dec 57383
   AVK_7,                               // hex 0xE028; dec 57384
   AVK_8,                               // hex 0xE029; dec 57385
   AVK_9,                               // hex 0xE02A; dec 57386
   AVK_STAR,                            // hex 0xE02B; dec 57387
   AVK_POUND,                           // hex 0xE02C; dec 57388

   AVK_POWER,                           // hex 0xE02D; dec 57389
   AVK_END,                             // hex 0xE02E; dec 57390
   AVK_SEND,                            // hex 0xE02F; dec 57391
   AVK_CLR,                             // hex 0xE030; dec 57392

   AVK_UP,                              // hex 0xE031; dec 57393
   AVK_DOWN,                            // hex 0xE032; dec 57394
   AVK_LEFT,                            // hex 0xE033; dec 57395
   AVK_RIGHT,                           // hex 0xE034; dec 57396
   AVK_SELECT,                          // hex 0xE035; dec 57397

   AVK_SOFT1,                           // hex 0xE036; dec 57398
   AVK_SOFT2,                           // hex 0xE037; dec 57399
   AVK_SOFT3,                           // hex 0xE038; dec 57400
   AVK_SOFT4,                           // hex 0xE039; dec 57401

   AVK_FUNCTION1,                       // hex 0xE03A; dec 57402
   AVK_FUNCTION2,                       // hex 0xE03B; dec 57403
   AVK_FUNCTION3,                       // hex 0xE03C; dec 57404

   AVK_FUNCTION,                        // hex 0xE03D; dec 57405
   AVK_MENU,                            // hex 0xE03E; dec 57406
   AVK_INFO,                            // hex 0xE03F; dec 57407

   AVK_SHIFT,                           // hex 0xE040; dec 57408

   AVK_MESSAGE,                         // hex 0xE041; dec 57409
   AVK_MUTE,                            // hex 0xE042; dec 57410

   AVK_STORE,                           // hex 0xE043; dec 57411
   AVK_RECALL,                          // hex 0xE044; dec 57412

   AVK_PUNC1,                           // hex 0xE045; dec 57413
   AVK_PUNC2,                           // hex 0xE046; dec 57414

   AVK_VOLUME_UP,                       // hex 0xE047; dec 57415
   AVK_VOLUME_DOWN,                     // hex 0xE048; dec 57416

   AVK_WEB_ACCESS,                      // hex 0xE049; dec 57417
   AVK_VOICE_MEMO,                      // hex 0xE04A; dec 57418
   AVK_SPEAKER,                         // hex 0xE04B; dec 57419
   AVK_TAP,                             // hex 0xE04C; dec 57420

   AVK_PTT,                             // hex 0xE04D; dec 57421
   AVK_LSHIFT,                          // hex 0xE04E; dec 57422
   AVK_RSHIFT,                          // hex 0xE04F; dec 57423
   AVK_LCTRL,                           // hex 0xE050; dec 57424
   AVK_RCTRL,                           // hex 0xE051; dec 57425
   AVK_LALT,                            // hex 0xE052; dec 57426
   AVK_RALT,                            // hex 0xE053; dec 57427
   AVK_CAPLK,                           // hex 0xE054; dec 57428
   AVK_SCRLK,                           // hex 0xE055; dec 57429
   AVK_NUMLK,                           // hex 0xE056; dec 57430
   AVK_LNGHANGUL,                       // hex 0xE057; dec 57431
   AVK_LNGJUNJA,                        // hex 0xE058; dec 57432
   AVK_LNGFIN,                          // hex 0xE059; dec 57433
   AVK_LNGHANJA,                        // hex 0xE05A; dec 57434
   AVK_PRSCRN,                          // hex 0xE05B; dec 57435
   AVK_BREAK,                           // hex 0xE05C; dec 57436
   AVK_TXINSERT,                        // hex 0xE05D; dec 57437
   AVK_TXDELETE,                        // hex 0xE05E; dec 57438
   AVK_TXHOME,                          // hex 0xE05F; dec 57439
   AVK_TXEND,                           // hex 0xE060; dec 57440
   AVK_TXPGUP,                          // hex 0xE061; dec 57441
   AVK_TXPGDOWN,                        // hex 0xE062; dec 57442

   // Gamepad Keys
   AVK_GP_1,    // Gamepad Key 1                       hex 0xE063; dec 57443
   AVK_GP_2,    // Gamepad Key 2                       hex 0xE064; dec 57444
   AVK_GP_3,    // Gamepad Key 3                       hex 0xE065; dec 57445
   AVK_GP_4,    // Gamepad Key 4                       hex 0xE066; dec 57446
   AVK_GP_5,    // Gamepad Key 5                       hex 0xE067; dec 57447
   AVK_GP_6,    // Gamepad Key 6                       hex 0xE068; dec 57448
   AVK_GP_SL,   // Gamepad Key Shoulder Left           hex 0xE069; dec 57449
   AVK_GP_SR,   // Gamepad Key Shoulder Right          hex 0xE06A; dec 57450

   AVK_CAMERA,       // Access device camera (typically, for snapshot/camcorder functionality)    hex 0xE06B; dec 57451
   AVK_VIDEO_PHONE,  // Start video telephony session/app                                         hex 0xE06C; dec 57452

   AVK_APP_SWITCH,   // Switch among loaded apps to make one of them top visible                  hex 0xE06D; dec 57453
   
   // Multimedia keys.
   AVK_PLAY,      // can be overloaded with the pause key                    hex 0xE06E; dec 57454
   AVK_PAUSE,     // only if there is a separate pause key                   hex 0xE06F; dec 57455
   AVK_SKIP_FWD,  // skip forward                                            hex 0xE070; dec 57456
   AVK_SKIP_RWD,  // skip backward                                           hex 0xE071; dec 57457
   AVK_FFWD,      // fast-forward                                            hex 0xE072; dec 57458
   AVK_RWD,       // rewind                                                  hex 0xE073; dec 57459
   AVK_STOP,      // stop                                                    hex 0xE074; dec 57460

   AVK_HEADSET_FUNC, // headset key                                          hex 0xE075; dec 57461

   //Full set of function keys
   AVK_FUNCTION4,                                                         // hex 0xE076; dec 57462
   AVK_FUNCTION5,                                                         // hex 0xE077; dec 57463
   AVK_FUNCTION6,                                                         // hex 0xE078; dec 57464
   AVK_FUNCTION7,                                                         // hex 0xE079; dec 57465
   AVK_FUNCTION8,                                                         // hex 0xE07A; dec 57466
   AVK_FUNCTION9,                                                         // hex 0xE07B; dec 57467
   AVK_FUNCTION10,                                                        // hex 0xE07C; dec 57468
   AVK_FUNCTION11,                                                        // hex 0xE07D; dec 57469
   AVK_FUNCTION12,                                                        // hex 0xE07E; dec 57470

   // Keys for extended keypads
   AVK_SYMBOL, // Symbol (modifier key)                                   // hex 0xE07F; dec 57471
   AVK_ENTER,  // Enter key                                               // hex 0xE080; dec 57472
   AVK_SPACE,  // Space bar                                               // hex 0xE081; dec 57473


   AVK_LAST                                                               // hex 0xE082; dec 57474
};

Members:
   AVK_PLAY      - Multimedia key. Used to start a media play. Once a media
                   play is started the key can become the AVK_PAUSE key to
                   let pause a playing media. Hence this key can be overloaded
                   with the pause key.
   AVK_PAUSE     - Multimedia key. Used to pause a playing media.
   AVK_SKIP_FWD  - Skip Forward in media.
   AVK_SKIP_RWD  - Skip backward in media.
   AVK_FFWD      - Fast Forward Media.
   AVK_RWD       - Rewind Media.
   AVK_STOP      - Stop a playing media.

   AVK_HEADSET_FUNC - the generic send/end headset key event, 
                      which the UI can interpret appropriately.
   
   AVK_FUNCTION1 to -  Defines a full set of function keys.
   AVK_FUNCTION12      Used on full keyboards to represent function keys from 
                       F1 to F12. 

   AVK_SYMBOL       -  Used to represent the 'Symbol' key on extended keypads.
                       Symbol key is used in combination with other keys to 
                       overload their meaning. 

   AVK_ENTER        -  Used to represent the 'Enter' or 'Return' key, usually 
                       present on extended keypads

   AVK_SPACE        -  Used to represent the Spacebar, usually 
                       present on extended keypads
   
Comments:
   None

See Also:
   None

=============================================================================
AVK_OEM_RANGE_START
AVK_OEM_RANGE_END

Description:
   Range of OEM defined Vcodes.
   OEM Vcodes range is reserved from the end of Unicode Private Area (0xF8FF).
   The range has 256 entries for OEM's to define and track their own Vcodes.
   Range starts at AVK_OEM_RANGE_START (0xF800) and ends at AVK_OEM_RANGE_END
   (0xF8FF).

Definition:
   #define AVK_OEM_RANGE_START  0xF800
   #define AVK_OEM_RANGE_END    0xF8FF

Members:
   None

Comments:
   The key pressed state of the keys in the OEM defined range are to 
   be maintained by the OEM layer.

See Also:
   None

=============================================================================
AVK_A to AVK_Z

Description:
    Represents the range of keys from A to Z on an extended keypad or full keyboard, 
    such as a QWERTY keyboard. Each Vcode represents the Unicode value of the 
    corresponding lower-case letter. 

Definition:
   #define AVK_A     (97)
   #define AVK_B     (AVK_A + 1)   
   #define AVK_C     (AVK_A + 2)
   #define AVK_D     (AVK_A + 3)
   #define AVK_E     (AVK_A + 4)
   #define AVK_F     (AVK_A + 5) 
   #define AVK_G     (AVK_A + 6)   
   #define AVK_H     (AVK_A + 7) 
   #define AVK_I     (AVK_A + 8) 
   #define AVK_J     (AVK_A + 9) 
   #define AVK_K     (AVK_A + 10) 
   #define AVK_L     (AVK_A + 11) 
   #define AVK_M     (AVK_A + 12) 
   #define AVK_N     (AVK_A + 13) 
   #define AVK_O     (AVK_A + 14) 
   #define AVK_P     (AVK_A + 15) 
   #define AVK_Q     (AVK_A + 16) 
   #define AVK_R     (AVK_A + 17) 
   #define AVK_S     (AVK_A + 18) 
   #define AVK_T     (AVK_A + 19) 
   #define AVK_U     (AVK_A + 20) 
   #define AVK_V     (AVK_A + 21) 
   #define AVK_W     (AVK_A + 22) 
   #define AVK_X     (AVK_A + 23) 
   #define AVK_Y     (AVK_A + 24) 
   #define AVK_Z     (AVK_A + 25) 

Members:
   None

Comments:
   None

See Also:
   None
======================================================================= */

#endif  /* end ifndef AEEVCODES_H */
