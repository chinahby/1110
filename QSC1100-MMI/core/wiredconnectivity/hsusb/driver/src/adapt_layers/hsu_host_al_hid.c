/*==============================================================================

High Speed USB Host HID Adaptation Layer

GENERAL DESCRIPTION
Implementation of the Host HID Adaptation Layer.

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header:

when      who     what, where, why
--------  ---     --------------------------------------------------------------
02/15/08  tk      considered left/right key for modifier keys
07/08/06  sk      Initial version

==============================================================================*/

/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "comdef.h"
#include "clk.h"
#include "hsu_host_al_hid.h"
#include "hsu_host_api_i.h"
#include "hsu_common.h"
#include "usbhost_api.h"
#include "host_hid_app.h"
#include "uw_host_hid_common.h"
#include "jerrno.h"
#include "port.h"

#include "hsu_host_al_hid_test_app.h"

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */

#ifndef FEATURE_HSU_TEST
#include "event.h"
#include "hsu_event_defs.h"
#endif /*FEATURE_HSU_TEST*/

/* time to wait before we start to auto-repeat a key */
#define HSU_HOST_AL_HID_AUTO_REPEAT_DELAY 400

#define HSU_HOST_AL_HID_AUTO_REPEAT_INTERVAL 200

#ifdef CANDLEBOX_FEATURE_USBHID
uint16 hsu_host_al_hid_auto_repeat_delay = HSU_HOST_AL_HID_AUTO_REPEAT_DELAY;
uint16 hsu_host_al_hid_auto_repeat_interval = HSU_HOST_AL_HID_AUTO_REPEAT_INTERVAL;
#endif /* CANDLEBOX_FEATURE_USBHID */

#ifdef CANDLEBOX_FEATURE_USBHID
int32 g_Mouse_Coordinate_x = 0;
int32 g_Mouse_Coordinate_y = 0;
#endif /* CANDLEBOX_FEATURE_USBHID */
#define HSU_HOST_AL_HID_MAX_BUTTONS 30

typedef void (*hid_class_driver_disable_cb)(hid_app_dev_ctx_t app_dev_ctx);

/* HID Sampled Data Structure - used internally by the host stack to
** maintain the last occurrence of each specific data event.  Each data
** usage also maintains a incremental sequence number to be used by 
** a client as needed.  Sampled data allows for filtering out of micro
** measurements thereby reducing the amount of data transfers between
** the host stack and a client.
*/
typedef struct hsu_host_al_hid_sampled_data_s
{
  usbhost_hid_event_type  usage_x;
  usbhost_hid_event_type  usage_y;
  usbhost_hid_event_type  usage_z;
  usbhost_hid_event_type  usage_rx;
  usbhost_hid_event_type  usage_ry;
  usbhost_hid_event_type  usage_rz;
  usbhost_hid_event_type  usage_vx;
  usbhost_hid_event_type  usage_vy;
  usbhost_hid_event_type  usage_vz;
  usbhost_hid_event_type  usage_vbrx;
  usbhost_hid_event_type  usage_vbry;
  usbhost_hid_event_type  usage_vbrz;
  usbhost_hid_event_type  usage_vno;
  usbhost_hid_event_type  usage_slider;
} 
hsu_host_al_hid_sampled_data_type;

typedef struct hsu_host_al_hid_keyboard_modifiers_s
{
  boolean caps_lock_on;
  boolean num_lock_on;
  boolean scroll_lock_on;
  boolean left_shift_key_down;
  boolean left_alt_key_down;
  boolean left_ctrl_key_down;
  boolean left_gui_key_down;
  boolean right_shift_key_down;
  boolean right_alt_key_down;
  boolean right_ctrl_key_down;
  boolean right_gui_key_down;
}
hsu_host_al_hid_keyboard_modifiers_type;

/* Class-specific data for a hid device. A pointer to this type
** is stored in the device entry.
*/
typedef struct hsu_host_al_hid_device_data_s
{
  hsu_host_al_hid_sampled_data_type       sampled_data;
  usbhost_hid_attrib_type                 attributes;
  input_device_reg_t*                     input_dev_ptr;
  /* The following array saves the last known state (pressed or not) for each button */
  boolean                                 button_pressed[HSU_HOST_AL_HID_MAX_BUTTONS];
  boolean                                 got_hatswitch_value;
  jint32_t                                last_hatswitch_value;
  /* The following fields are needed only for keyboards */
  hsu_host_al_hid_keyboard_modifiers_type keyboard_modifiers;
  rex_crit_sect_type                      auto_repeat_crit_sect;
  rex_timer_type                          auto_repeat_timer;
  usbhost_hid_event_type                  auto_repeat_event;
}
hsu_host_al_hid_device_data_type;

/* Client immediate notification callback */
typedef void (* hid_notify_cb)
(
  usbhost_dev_handle_type     dev_hndl,
  usbhost_pdev_type           pdev_type,
  usbhost_hid_event_type *    event
);

/* Immediate notification callbacks, per client */
static hid_notify_cb hsu_host_al_hid_client_notify_cb[USBHOST_MAX_CLIENTS];

/* Synchronize access to hid device data */
static rex_crit_sect_type hsu_host_al_hid_device_data_crit_sect;

#define HID_RS_USAGE_IS_VALID(hid_usage) \
  ((hid_usage > USBHOST_HID_USAGE_DPADLEFT) && \
  (hid_usage < USBHOST_HID_USAGE_KEY_A))

/* Pool of hsu_host_al_hid_device_data_type structures,
** to be used by connected devices 
*/
static hsu_host_al_hid_device_data_type 
  hsu_host_al_hid_device_data_pool[USBHOST_MAX_DEVICES];
static boolean 
  hsu_host_al_hid_device_data_pool_entry_is_free[USBHOST_MAX_DEVICES];

#define HSU_HOST_AL_HID_MAX_ASCII_TBL_SIZ 256

/* Map usages to ASCII */
static uint8 hsu_host_al_hid_uid_to_ascii_low[HSU_HOST_AL_HID_MAX_ASCII_TBL_SIZ] =
{
  /* No event, ErrorRollOver, POSTFail, ErrorUndefined : Usages 0x00 - 0x03 */
  0,    0,    0,    0,

    /* Alphanumeric keys - Usages 0x04 - 0x27 */
    'a',  'b',  'c',  'd',  'e',  'f',  'g',  'h',  'i',  'j',  'k',  'l',  'm',
    'n',  'o',  'p',  'q',  'r',  's',  't',  'u',  'v',  'w',  'x',  'y',  'z',
    '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  '0',

    /* ENTER, ESCAPE, Backspace, Tab : Usages 0x28 - 0x2B */
    0xd,  0x1B,  0x8,  0x9,

    /* Punctuation keys - Usages  0x2C - 0x38 */
    ' ',  '-',  '=',  '[',  ']',  0x5c, 0,    ';',  0x27, 0x60, ',',  '.',  '/',

    /* Caps Lock, F1 - F12, Print Screen, Scroll Lock, Pause :
    Usages 0x39 - 0x48 */
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,

    /* Insert, Home, Pg Up, Delete, End, Pg Down, Arrow keys :
    Usages 0x49 - 0x52 */
    0,    0,    0,    0,    0,    0,    0x1A,    0x1B,    0x19,    0x18,

    /* Num Lock, Numpad operator keys, Numpad ENTER, Numpad numbers :
    Usages 0x53 - 0x63 */
    0,    '/',  '*',  '-',  '+',  0xd,
    '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  '0',  '.',

    /* Misc. keys and Numpad = : Usages 0x64 - 0x67 */
    0,    0,    0,    '=',

    /* Uncommon misc. keys : Usages 0x68 - 0xE7 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static uint8 hsu_host_al_hid_uid_to_ascii_hi[HSU_HOST_AL_HID_MAX_ASCII_TBL_SIZ] =
{
  /* No event, ErrorRollOver, POSTFail, ErrorUndefined : Usages 0x00 - 0x03 */
  0,    0,    0,    0,

    /* Alphanumeric keys - Usages 0x04 - 0x27 */
    'A',  'B',  'C',  'D',  'E',  'F',  'G',  'H',  'I',  'J',  'K',  'L',  'M',
    'N',  'O',  'P',  'Q',  'R',  'S',  'T',  'U',  'V',  'W',  'X',  'Y',  'Z',
    '!',  '@',  '#',  '$',  '%',  '^',  '&',  '*',  '(',  ')',

    /* ENTER, ESCAPE, Backspace, Tab : Usages 0x28 - 0x2B */
    0xd,  0x1B, 0x8,  0x9,

    /* Punctuation keys - Usages  0x2C - 0x38 */
    ' ',  '_',  '+',  '{',  '}',  '|',  0,    ':',  0x22, '~',  '<',  '>',  '?',

    /* Caps Lock, F1 - F12, Print Screen, Scroll Lock, Pause :
    Usages 0x39 - 0x48 */
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,

    /* Insert, Home, Pg Up, Delete, End, Pg Down, Arrow keys :
    Usages 0x49 - 0x52 */
    0,    0,    0,    0,    0,    0,    0x1A,    0x1B,    0x19,    0x18,

    /* Num Lock, Numpad operator keys, Numpad ENTER, Numpad numbers :
    Usages 0x53 - 0x63 */
    0,    '/',  '*',  '-',  '+',  0xd,
    '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  '0',  '.',

    /* Misc. keys and Numpad = : Usages 0x64 - 0x67 */
    0,    0,    0,    '=',

    /* Uncommon misc. keys : Usages 0x68 - 0xE7 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static uint8 hsu_host_al_hid_uid_to_ascii_low_CapsOn[HSU_HOST_AL_HID_MAX_ASCII_TBL_SIZ] =
{
  /* No event, ErrorRollOver, POSTFail, ErrorUndefined : Usages 0x00 - 0x03 */
  0,    0,    0,    0,

    /* Alphanumeric keys - Usages 0x04 - 0x27 */
    'A',  'B',  'C',  'D',  'E',  'F',  'G',  'H',  'I',  'J',  'K',  'L',  'M',
    'N',  'O',  'P',  'Q',  'R',  'S',  'T',  'U',  'V',  'W',  'X',  'Y',  'Z',
    '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  '0',

    /* ENTER, ESCAPE, Backspace, Tab : Usages 0x28 - 0x2B */
    0xd,  0x1B, 0x8,  0x9,

    /* Punctuation keys - Usages  0x2C - 0x38 */
    ' ',  '-',  '=',  '[',  ']',  0x5c, 0,    ';',  0x27, 0x60, ',',  '.',  '/',

    /* Caps Lock, F1 - F12, Print Screen, Scroll Lock, Pause :
    Usages 0x39 - 0x48 */
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,

    /* Insert, Home, Pg Up, Delete, End, Pg Down, Arrow keys :
    Usages 0x49 - 0x52 */
    0,    0,    0,    0,    0,    0,    0x1A,    0x1B,    0x19,    0x18,

    /* Num Lock, Numpad operator keys, Numpad ENTER, Numpad numbers :
    Usages 0x53 - 0x63 */
    0,    '/',  '*',  '-',  '+',  0xd,
    '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  '0',  '.',

    /* Misc. keys and Numpad = : Usages 0x64 - 0x67 */
    0,    0,    0,    '=',

    /* Uncommon misc. keys : Usages 0x68 - 0xE7 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static uint8 hsu_host_al_hid_uid_to_ascii_hi_CapsOn[HSU_HOST_AL_HID_MAX_ASCII_TBL_SIZ] =
{
  /* No event, ErrorRollOver, POSTFail, ErrorUndefined : Usages 0x00 - 0x03 */
  0,    0,    0,    0,

    /* Alphanumeric keys - Usages 0x04 - 0x27 */
    'a',  'b',  'c',  'd',  'e',  'f',  'g',  'h',  'i',  'j',  'k',  'l',  'm',
    'n',  'o',  'p',  'q',  'r',  's',  't',  'u',  'v',  'w',  'x',  'y',  'z',
    '!',  '@',  '#',  '$',  '%',  '^',  '&',  '*',  '(',  ')',

    /* ENTER, ESCAPE, Backspace, Tab : Usages 0x28 - 0x2B */
    0xd,  0x1B, 0x8,  0x9,

    /* Punctuation keys - Usages  0x2C - 0x38 */
    ' ',  '_',  '+',  '{',  '}',  '|',  0,    ':',  0x22, '~',  '<',  '>',  '?',

    /* Caps Lock, F1 - F12, Print Screen, Scroll Lock, Pause :
    Usages 0x39 - 0x48 */
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,

    /* Insert, Home, Pg Up, Delete, End, Pg Down, Arrow keys :
    Usages 0x49 - 0x52 */
    0,    0,    0,    0,    0,    0,    0x1A,    0x1B,    0x19,    0x18,

    /* Num Lock, Numpad operator keys, Numpad ENTER, Numpad numbers :
    Usages 0x53 - 0x63 */
    0,    '/',  '*',  '-',  '+',  0xd,
    '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  '0',  '.',

    /* Misc. keys and Numpad = : Usages 0x64 - 0x67 */
    0,    0,    0,    '=',

    /* Uncommon misc. keys : Usages 0x68 - 0xE7 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static uint8 hsu_host_al_hid_uid_to_ascii_keypad_numLockOn[HSU_HOST_AL_HID_MAX_ASCII_TBL_SIZ] =
{
  /* No event, ErrorRollOver, POSTFail, ErrorUndefined : Usages 0x00 - 0x03 */
  0,    0,    0,    0,

    /* Alphanumeric keys - Usages 0x04 - 0x27 */
    'a',  'b',  'c',  'd',  'e',  'f',  'g',  'h',  'i',  'j',  'k',  'l',  'm',
    'n',  'o',  'p',  'q',  'r',  's',  't',  'u',  'v',  'w',  'x',  'y',  'z',
    '!',  '@',  '#',  '$',  '%',  '^',  '&',  '*',  '(',  ')',

    /* ENTER, ESCAPE, Backspace, Tab : Usages 0x28 - 0x2B */
    0xd,  0x1B, 0x8,  0x9,

    /* Punctuation keys - Usages  0x2C - 0x38 */
    ' ',  '_',  '+',  '{',  '}',  '|',  0,    ':',  0x22, '~',  '<',  '>',  '?',

    /* Caps Lock, F1 - F12, Print Screen, Scroll Lock, Pause :
    Usages 0x39 - 0x48 */
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,

    /* Insert, Home, Pg Up, Delete, End, Pg Down, Arrow keys :
    Usages 0x49 - 0x52 */
    0,    0,    0,    0,    0,    0,    0x1A,    0x1B,    0x19,    0x18,

    /* Num Lock, Numpad operator keys, Numpad ENTER, Numpad numbers :
    Usages 0x53 - 0x63 */
    0,    '/',  '*',  '-',  '+',  0xd,
    '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  '0',  '.',

    /* Misc. keys and Numpad = : Usages 0x64 - 0x67 */
    0,    0,    0,    '=',

    /* Uncommon misc. keys : Usages 0x68 - 0xE7 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static uint8 hsu_host_al_hid_uid_to_ascii_keypad_numLockOff[HSU_HOST_AL_HID_MAX_ASCII_TBL_SIZ] =
{
  /* No event, ErrorRollOver, POSTFail, ErrorUndefined : Usages 0x00 - 0x03 */
  0,    0,    0,    0,

    /* Alphanumeric keys - Usages 0x04 - 0x27 */
    'a',  'b',  'c',  'd',  'e',  'f',  'g',  'h',  'i',  'j',  'k',  'l',  'm',
    'n',  'o',  'p',  'q',  'r',  's',  't',  'u',  'v',  'w',  'x',  'y',  'z',
    '!',  '@',  '#',  '$',  '%',  '^',  '&',  '*',  '(',  ')',

    /* ENTER, ESCAPE, Backspace, Tab : Usages 0x28 - 0x2B */
    0xd,  0x1B, 0x8,  0x9,

    /* Punctuation keys - Usages  0x2C - 0x38 */
    ' ',  '_',  '+',  '{',  '}',  '|',  0,    ':',  0x22, '~',  '<',  '>',  '?',

    /* Caps Lock, F1 - F12, Print Screen, Scroll Lock, Pause :
    Usages 0x39 - 0x48 */
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,

    /* Insert, Home, Pg Up, Delete, End, Pg Down, Arrow keys :
    Usages 0x49 - 0x52 */
    0,    0,    0,    0,    0,    0,    0x1A,    0x1B,    0x19,    0x18,

    /* Num Lock, Numpad operator keys, Numpad ENTER, Numpad Arrow Keys:
    Usages 0x53 - 0x63 */
    0,    '/',  '*',  '-',  '+',  0x0D,
    0,    0x19, 0,    0x1B, 0,    0x1A, 0,  0x18,  0,  0,  0x7F,

    /* Misc. keys and Numpad = : Usages 0x64 - 0x67 */
    0,    0,    0,    '=',

    /* Uncommon misc. keys : Usages 0x68 - 0xE7 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static usbhost_hid_usage_type hsu_host_al_hid_uid_to_usbhost_api_usage[HSU_HOST_AL_HID_MAX_ASCII_TBL_SIZ] =
{
    /* No event, ErrorRollOver, POSTFail, ErrorUndefined : Usages 0x00 - 0x03 */
    USBHOST_HID_USAGE_NONE, 
    USBHOST_HID_USAGE_NONE, 
    USBHOST_HID_USAGE_NONE,
    USBHOST_HID_USAGE_NONE,
    
    /* Alphanumeric keys - Usages 0x04 - 0x27 */
    USBHOST_HID_USAGE_KEY_A,
    USBHOST_HID_USAGE_KEY_B,
    USBHOST_HID_USAGE_KEY_C,
    USBHOST_HID_USAGE_KEY_D,
    USBHOST_HID_USAGE_KEY_E,
    USBHOST_HID_USAGE_KEY_F,
    USBHOST_HID_USAGE_KEY_G,
    USBHOST_HID_USAGE_KEY_H,
    USBHOST_HID_USAGE_KEY_I,
    USBHOST_HID_USAGE_KEY_J,
    USBHOST_HID_USAGE_KEY_K,
    USBHOST_HID_USAGE_KEY_L,
    USBHOST_HID_USAGE_KEY_M,
    USBHOST_HID_USAGE_KEY_N,
    USBHOST_HID_USAGE_KEY_O,
    USBHOST_HID_USAGE_KEY_P,
    USBHOST_HID_USAGE_KEY_Q,
    USBHOST_HID_USAGE_KEY_R,
    USBHOST_HID_USAGE_KEY_S,
    USBHOST_HID_USAGE_KEY_T,
    USBHOST_HID_USAGE_KEY_U,
    USBHOST_HID_USAGE_KEY_V,
    USBHOST_HID_USAGE_KEY_W,
    USBHOST_HID_USAGE_KEY_X,
    USBHOST_HID_USAGE_KEY_Y,
    USBHOST_HID_USAGE_KEY_Z,
    USBHOST_HID_USAGE_KEY_1,
    USBHOST_HID_USAGE_KEY_2,
    USBHOST_HID_USAGE_KEY_3,
    USBHOST_HID_USAGE_KEY_4,
    USBHOST_HID_USAGE_KEY_5,
    USBHOST_HID_USAGE_KEY_6,
    USBHOST_HID_USAGE_KEY_7,
    USBHOST_HID_USAGE_KEY_8,
    USBHOST_HID_USAGE_KEY_9,
    USBHOST_HID_USAGE_KEY_0,
    
    /* ENTER, ESCAPE, Backspace, Tab : Usages 0x28 - 0x2B */
    USBHOST_HID_USAGE_KEY_ENTER,
    USBHOST_HID_USAGE_KEY_ESCAPE,
    USBHOST_HID_USAGE_KEY_BACKSPACE,
    USBHOST_HID_USAGE_KEY_TAB,

    /* Punctuation keys - Usages  0x2C - 0x38 */
    USBHOST_HID_USAGE_KEY_SPACE,
    USBHOST_HID_USAGE_KEY_MINUS,
    USBHOST_HID_USAGE_KEY_EQUAL,
    USBHOST_HID_USAGE_KEY_LEFT_BRACKET,
    USBHOST_HID_USAGE_KEY_RIGHT_BRACKET,
    USBHOST_HID_USAGE_KEY_BACKSLASH,
    USBHOST_HID_USAGE_NONE, /* Non-US # and ~ */
    USBHOST_HID_USAGE_KEY_SEMICOLON,
    USBHOST_HID_USAGE_KEY_QUOTE,
    USBHOST_HID_USAGE_KEY_GRAVE,
    USBHOST_HID_USAGE_KEY_COMMA,
    USBHOST_HID_USAGE_KEY_PERIOD,
    USBHOST_HID_USAGE_KEY_FORWARD_SLASH,

    /* Caps Lock, F1 - F12, Print Screen, Scroll Lock, Pause :
    Usages 0x39 - 0x48 */
    USBHOST_HID_USAGE_KEY_CAPS_LOCK,
    USBHOST_HID_USAGE_KEY_F1,
    USBHOST_HID_USAGE_KEY_F2,
    USBHOST_HID_USAGE_KEY_F3,
    USBHOST_HID_USAGE_KEY_F4,
    USBHOST_HID_USAGE_KEY_F5,
    USBHOST_HID_USAGE_KEY_F6,
    USBHOST_HID_USAGE_KEY_F7,
    USBHOST_HID_USAGE_KEY_F8,
    USBHOST_HID_USAGE_KEY_F9,
    USBHOST_HID_USAGE_KEY_F10,
    USBHOST_HID_USAGE_KEY_F11,
    USBHOST_HID_USAGE_KEY_F12,
    USBHOST_HID_USAGE_KEY_PRINT_SCREEN,
    USBHOST_HID_USAGE_KEY_SCROLL_LOCK,
    USBHOST_HID_USAGE_KEY_PAUSE,

    /* Insert, Home, Pg Up, Delete, End, Pg Down, Arrow keys :
    Usages 0x49 - 0x52 */
    USBHOST_HID_USAGE_KEY_INSERT,
    USBHOST_HID_USAGE_KEY_HOME,
    USBHOST_HID_USAGE_KEY_PAGE_UP,
    USBHOST_HID_USAGE_KEY_DELETE,
    USBHOST_HID_USAGE_KEY_END,
    USBHOST_HID_USAGE_KEY_PAGE_DOWN,
    USBHOST_HID_USAGE_KEY_RIGHT,
    USBHOST_HID_USAGE_KEY_LEFT,
    USBHOST_HID_USAGE_KEY_DOWN,
    USBHOST_HID_USAGE_KEY_UP,

    /* Num Lock, Numpad operator keys, Numpad ENTER, Numpad numbers :
    Usages 0x53 - 0x63 */
    USBHOST_HID_USAGE_KEY_NUM_LOCK,
    USBHOST_HID_USAGE_KEY_PAD_DIVIDE,
    USBHOST_HID_USAGE_KEY_PAD_MULTIPLY,
    USBHOST_HID_USAGE_KEY_PAD_SUBTRACT,
    USBHOST_HID_USAGE_KEY_PAD_ADD,
    USBHOST_HID_USAGE_KEY_PAD_ENTER,
    USBHOST_HID_USAGE_KEY_PAD_1,
    USBHOST_HID_USAGE_KEY_PAD_2,
    USBHOST_HID_USAGE_KEY_PAD_3,
    USBHOST_HID_USAGE_KEY_PAD_4,
    USBHOST_HID_USAGE_KEY_PAD_5,
    USBHOST_HID_USAGE_KEY_PAD_6,
    USBHOST_HID_USAGE_KEY_PAD_7,
    USBHOST_HID_USAGE_KEY_PAD_8,
    USBHOST_HID_USAGE_KEY_PAD_9,
    USBHOST_HID_USAGE_KEY_PAD_0,
    USBHOST_HID_USAGE_KEY_PAD_DECIMAL,
    
    /* Misc. keys and Numpad = : Usages 0x64 - 0x67 */
    USBHOST_HID_USAGE_KEY_NON_US_BACKSLASH,
    USBHOST_HID_USAGE_KEY_APPL,
    USBHOST_HID_USAGE_KEY_POWER,
    USBHOST_HID_USAGE_KEY_PAD_EQUAL,

    /* Uncommon misc. keys : Usages 0x68 - 0xDF */
    USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE,
    USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE,
    USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE,
    USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE,
    USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE,
    USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE, USBHOST_HID_USAGE_NONE,

    /* Modifiers: Usages 0xE0 - 0xE7 */
    USBHOST_HID_USAGE_KEY_LEFT_CONTROL,
    USBHOST_HID_USAGE_KEY_LEFT_SHIFT,
    USBHOST_HID_USAGE_KEY_LEFT_ALT,
    USBHOST_HID_USAGE_KEY_LEFT_GUI,
    USBHOST_HID_USAGE_KEY_RIGHT_CONTROL,
    USBHOST_HID_USAGE_KEY_RIGHT_SHIFT,
    USBHOST_HID_USAGE_KEY_RIGHT_ALT,
    USBHOST_HID_USAGE_KEY_RIGHT_GUI,

    /* Others (to complete array) */
    USBHOST_HID_USAGE_NONE,
    USBHOST_HID_USAGE_NONE,
    USBHOST_HID_USAGE_NONE,
    USBHOST_HID_USAGE_NONE,
    USBHOST_HID_USAGE_NONE,
    USBHOST_HID_USAGE_NONE,
    USBHOST_HID_USAGE_NONE,
    USBHOST_HID_USAGE_NONE,
    USBHOST_HID_USAGE_NONE,
    USBHOST_HID_USAGE_NONE,
    USBHOST_HID_USAGE_NONE,
    USBHOST_HID_USAGE_NONE,
    USBHOST_HID_USAGE_NONE,
    USBHOST_HID_USAGE_NONE,
    USBHOST_HID_USAGE_NONE,
    USBHOST_HID_USAGE_NONE,
    USBHOST_HID_USAGE_NONE,
    USBHOST_HID_USAGE_NONE,
    USBHOST_HID_USAGE_NONE,
    USBHOST_HID_USAGE_NONE,
    USBHOST_HID_USAGE_NONE,
    USBHOST_HID_USAGE_NONE,
    USBHOST_HID_USAGE_NONE,
    USBHOST_HID_USAGE_NONE
};

static usbhost_hid_usage_type hsu_host_al_hid_get_usbhost_api_usage
(
  /* Usage to be translated (as defined in the standard) */
  uint32 usage
)
{
  usbhost_hid_usage_type ret;
  uint16 usage_page = (uint16)(HID_USAGE_PAGE(usage));

  if (usage_page == HUP_BUTTON)
  {
    return USBHOST_HID_USAGE_BUTTON;
  }

  switch(usage) {
  case USAGE_DIAL:
    ret = USBHOST_HID_USAGE_DIAL;
  	break;
  case USAGE_WHEEL:
    ret = USBHOST_HID_USAGE_WHEEL;
    break;
  case USAGE_SLIDER:
    ret = USBHOST_HID_USAGE_SLIDER;
    break;
  case USAGE_HATSWITCH:
    ret = USBHOST_HID_USAGE_HATSWITCH;
    break;
  case USAGE_BYTECOUNTER:
    ret = USBHOST_HID_USAGE_BYTECOUNTER;
    break;
  case USAGE_MOTIONWAKEUP:
    ret = USBHOST_HID_USAGE_MOTIONWAKEUP;
    break;
  case USAGE_START:
    ret = USBHOST_HID_USAGE_START;
    break;
  case USAGE_SELECT:
    ret = USBHOST_HID_USAGE_SELECT;
    break;
  case USAGE_SYSCONTRL:
    ret = USBHOST_HID_USAGE_SYSCONTRL;
    break;
  case USAGE_SYSPOWER:
    ret = USBHOST_HID_USAGE_SYSPOWERDOWN;
    break;
  case USAGE_SYSSLEEP:
    ret = USBHOST_HID_USAGE_SYSSLEEP;
    break;
  case USAGE_SYSWAKE:
    ret = USBHOST_HID_USAGE_SYSWAKE;
    break;
  case USAGE_SYSCONMENU:
    ret = USBHOST_HID_USAGE_SYSCONMENU;
    break;
  case USAGE_SYSMAINMENU:
    ret = USBHOST_HID_USAGE_SYSMAINMENU;
    break;
  case USAGE_SYSAPPMENU:
    ret = USBHOST_HID_USAGE_SYSAPPMENU;
    break;
  case USAGE_SYSMENUHELP:
    ret = USBHOST_HID_USAGE_SYSMENUHELP;
    break;
  case USAGE_SYSMENUEXIT:
    ret = USBHOST_HID_USAGE_SYSMENUEXIT;
    break;
  case USAGE_SYSMENURIGHT:
    ret = USBHOST_HID_USAGE_SYSMENURIGHT;
    break;
  case USAGE_SYSMENULEFT:
    ret = USBHOST_HID_USAGE_SYSMENULEFT;
    break;
  case USAGE_SYSMENUUP:
    ret = USBHOST_HID_USAGE_SYSMENUUP;
    break;
  case USAGE_SYSMENUDOWN:
    ret = USBHOST_HID_USAGE_SYSMENUDOWN;
    break;
  case USAGE_DPADUP:
    ret = USBHOST_HID_USAGE_DPADUP;
    break;
  case USAGE_DPADDOWN:
    ret = USBHOST_HID_USAGE_DPADDOWN;
    break;
  case USAGE_DPADRIGHT:
    ret = USBHOST_HID_USAGE_DPADRIGHT;
    break;
  case USAGE_DPADLEFT:
    ret = USBHOST_HID_USAGE_DPADLEFT;
    break;
  case USAGE_X:
    ret = USBHOST_HID_USAGE_X;
    break;
  case USAGE_Y:
    ret = USBHOST_HID_USAGE_Y;
    break;
  case USAGE_Z:
    ret = USBHOST_HID_USAGE_Z;
    break;
  case USAGE_RX:
    ret = USBHOST_HID_USAGE_RX;
    break;
  case USAGE_RY:
    ret = USBHOST_HID_USAGE_RY;
    break;
  case USAGE_RZ:
    ret = USBHOST_HID_USAGE_RZ;
    break;
  case USAGE_VX:
    ret = USBHOST_HID_USAGE_VX;
    break;
  case USAGE_VY:
    ret = USBHOST_HID_USAGE_VY;
    break;
  case USAGE_VZ:
    ret = USBHOST_HID_USAGE_VZ;
    break;
  case USAGE_VBRX:
    ret = USBHOST_HID_USAGE_VBRX;
    break;
  case USAGE_VBRY:
    ret = USBHOST_HID_USAGE_VBRY;
    break;
  case USAGE_VBRZ:
    ret = USBHOST_HID_USAGE_VBRZ;
    break;
  case USAGE_VNO:
    ret = USBHOST_HID_USAGE_VNO;
    break;
  default:
    /* No match */
    ret = USBHOST_HID_USAGE_NONE;
  }

  return ret;
}

/*======================================================================

FUNCTION hsu_host_al_hid_set_min_max

DESCRIPTION
A helper function that sets the events data_min and data_max fields
based on the att parameter.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

=======================================================================*/
static void hsu_host_al_hid_set_min_max
(
  usbhost_hid_event_type* event_ptr,
  const usage_att_t *att
)
{
  if ((att->physical_min != 0) || (att->physical_max != 0))
  {
    event_ptr->data_min = att->physical_min;
    event_ptr->data_max = att->physical_max;
  }
  else
  {
    event_ptr->data_min = att->logical_min;
    event_ptr->data_max = att->logical_max;
  }
}

/*======================================================================

FUNCTION hsu_host_al_hid_get_sampled_data

DESCRIPTION
This function gets the sampled data item stored in the device's
sampled data structure, and copies it to the client-supplied
event structure.

DEPENDENCIES
None.

RETURN VALUE
USBHOST_ERROR if parameters are invalid. Otherwise, USBHOST_SUCCESS.

SIDE EFFECTS
None.

=======================================================================*/
static usbhost_status_type hsu_host_al_hid_get_sampled_data
(
  /* Device instance id */
  const usbhost_dev_entry_type*  dev_entry_ptr,
  /* Pointer to event instance to be populated  */
  usbhost_hid_event_type *   hid_event
)
{
  usbhost_status_type        ret = USBHOST_SUCCESS;
  usbhost_hid_usage_type     usage = USBHOST_HID_USAGE_NONE;
  hsu_host_al_hid_device_data_type* hid_data_ptr = 
    (hsu_host_al_hid_device_data_type*)dev_entry_ptr->class_specific_data;

  if ((dev_entry_ptr != NULL) &&
    (hid_data_ptr != NULL) &&
    (hid_event != NULL) &&
    (HID_RS_USAGE_IS_VALID(hid_event->usage)))
  { 
    /* Prevent sampled data from being updated */
    rex_enter_crit_sect(&hsu_host_al_hid_device_data_crit_sect);

    /* Save off the usage, we need to restore it after the memcpy */
    usage = hid_event->usage;

    switch (usage)
    {
    case USBHOST_HID_USAGE_X:
      memcpy(hid_event, &(hid_data_ptr->sampled_data.usage_x),
        sizeof(usbhost_hid_event_type));
#ifdef CANDLEBOX_FEATURE_USBHID
	  hid_event->data_val= g_Mouse_Coordinate_x;
	  g_Mouse_Coordinate_x=0;
#endif /* CANDLEBOX_FEATURE_USBHID */
      break;

    case USBHOST_HID_USAGE_Y:
      memcpy(hid_event, &(hid_data_ptr->sampled_data.usage_y),
        sizeof(usbhost_hid_event_type));
#ifdef CANDLEBOX_FEATURE_USBHID
  	  hid_event->data_val=g_Mouse_Coordinate_y;
	  g_Mouse_Coordinate_y=0;
#endif /* CANDLEBOX_FEATURE_USBHID */
      break;

    case USBHOST_HID_USAGE_Z:
      memcpy(hid_event, &(hid_data_ptr->sampled_data.usage_z),
        sizeof(usbhost_hid_event_type));
      break;

    case USBHOST_HID_USAGE_RX:
      memcpy(hid_event, &(hid_data_ptr->sampled_data.usage_rx),
        sizeof(usbhost_hid_event_type));
      break;

    case USBHOST_HID_USAGE_RY:
      memcpy(hid_event, &(hid_data_ptr->sampled_data.usage_ry),
        sizeof(usbhost_hid_event_type));
      break;

    case USBHOST_HID_USAGE_RZ:
      memcpy(hid_event, &(hid_data_ptr->sampled_data.usage_rz),
        sizeof(usbhost_hid_event_type));
      break;

    case USBHOST_HID_USAGE_VX:
      memcpy(hid_event, &(hid_data_ptr->sampled_data.usage_vx),
        sizeof(usbhost_hid_event_type));
      break;

    case USBHOST_HID_USAGE_VY:
      memcpy(hid_event, &(hid_data_ptr->sampled_data.usage_vy),
        sizeof(usbhost_hid_event_type));
      break;

    case USBHOST_HID_USAGE_VZ:
      memcpy(hid_event, &(hid_data_ptr->sampled_data.usage_vz),
        sizeof(usbhost_hid_event_type));
      break;

    case USBHOST_HID_USAGE_VBRX:
      memcpy(hid_event, &(hid_data_ptr->sampled_data.usage_vbrx),
        sizeof(usbhost_hid_event_type));
      break;

    case USBHOST_HID_USAGE_VBRY:
      memcpy(hid_event, &(hid_data_ptr->sampled_data.usage_vbry),
        sizeof(usbhost_hid_event_type));
      break;

    case USBHOST_HID_USAGE_VBRZ:
      memcpy(hid_event, &(hid_data_ptr->sampled_data.usage_vbrz),
        sizeof(usbhost_hid_event_type));
      break;

    case USBHOST_HID_USAGE_VNO:
      memcpy(hid_event, &(hid_data_ptr->sampled_data.usage_vno),
        sizeof(usbhost_hid_event_type));
      break;
    case USBHOST_HID_USAGE_SLIDER:
      memcpy(hid_event, &(hid_data_ptr->sampled_data.usage_slider),
        sizeof(usbhost_hid_event_type));
      break;

    default:
      HSU_MSG_ERROR("Unknown usage=%d", usage, 0, 0);
      ret = USBHOST_ERROR;
    }
  }
  else
  {
    ret = USBHOST_ERROR;
  }

  rex_leave_crit_sect(&hsu_host_al_hid_device_data_crit_sect);

  /* Restore the usage identifier if no errors occurred */
  if (ret == USBHOST_SUCCESS)
  {
    hid_event->usage = usage;
  }

  return ret;
} /* hsu_host_al_hid_get_sampled_data */

/*===========================================================================

FUNCTION hsu_host_al_hid_ioctl_func

DESCRIPTION
This function is invoked by usbhost_ioctl() in order to handle
hid specific IOCTLs.

DEPENDENCIES
None.

RETURN VALUE
USBHOST_SUCCESS on success. Otherwise, an appropriate
usbhost_status_type value.

SIDE EFFECTS
None.
===========================================================================*/
static usbhost_status_type hsu_host_al_hid_ioctl_func
(
  /* Information regarding the device */
  const usbhost_dev_entry_type* dev_entry_ptr,
  /* Device specific command */
  usbhost_ioctl_cmd_type command,
  /* IOCTL specific parameter
  ** passed from the application
  */
  void * param
)
{
  usbhost_status_type ret = USBHOST_SUCCESS;
  hsu_host_al_hid_device_data_type* dev_data_ptr = 
    (hsu_host_al_hid_device_data_type*)dev_entry_ptr->class_specific_data;
  jresult_t jresult;

  if (dev_data_ptr == NULL)
  {
    HSU_MSG_ERROR("hsu_host_al_hid_ioctl_func: NULL device data.",
                  0,0,0);
    HSU_ASSERT(dev_data_ptr!=NULL);
    return USBHOST_ERROR;
  }
  
  switch(command)
  {
  case USBHOST_IOCTL_HID_GET_CONFIGURATION:
    {
    usbhost_ioctl_hid_cfg_type* cfg_parm;
    usbhost_client_id_type  client_id = 
      dev_entry_ptr->curr_client->data->client_id;

    HSU_ASSERT((uint32)client_id < USBHOST_MAX_CLIENTS);

    if (param==NULL)
    {
      HSU_MSG_ERROR("USBHOST_IOCTL_HID_GET_CONFIGURATION: NULL parameter.",0,0,0);
      ret = USBHOST_ERROR;
      HSU_ASSERT(param!=NULL);
      break;
    }

    cfg_parm = (usbhost_ioctl_hid_cfg_type *) param;

    cfg_parm->client_notify_cb = hsu_host_al_hid_client_notify_cb[client_id];
    }
    break;
  case USBHOST_IOCTL_HID_SET_CONFIGURATION:
    {
    usbhost_ioctl_hid_cfg_type * cfg_parm;
    usbhost_client_id_type  client_id = 
      dev_entry_ptr->curr_client->data->client_id;

    HSU_ASSERT((uint32)client_id < USBHOST_MAX_CLIENTS);

    if (param==NULL)
    {
      HSU_MSG_ERROR("USBHOST_IOCTL_HID_SET_CONFIGURATION: NULL parameter.",0,0,0);
      ret = USBHOST_ERROR;
      HSU_ASSERT(param!=NULL);
      break;
    }

    cfg_parm = (usbhost_ioctl_hid_cfg_type *) param;

    hsu_host_al_hid_client_notify_cb[client_id] = cfg_parm->client_notify_cb;
    }
    break;
  case USBHOST_IOCTL_HID_READ_SAMPLED:
    {
      ret = hsu_host_al_hid_get_sampled_data(dev_entry_ptr, 
                                            (usbhost_hid_event_type*)param);
    }
    break;
  case USBHOST_IOCTL_HID_GET_ATTRIB:
    {
      usbhost_hid_attrib_type* hid_param = (usbhost_hid_attrib_type *)param;
      hsu_host_al_hid_device_data_type* hid_data_ptr = 
        (hsu_host_al_hid_device_data_type*)dev_entry_ptr->class_specific_data;

      if ((hid_param != NULL) && (hid_data_ptr != NULL))
      {
        hid_param->max_buttons = hid_data_ptr->attributes.max_buttons;;
        hid_param->min_x = hid_data_ptr->attributes.min_x;
        hid_param->max_x = hid_data_ptr->attributes.max_x;
        hid_param->min_y = hid_data_ptr->attributes.min_y;
        hid_param->max_y = hid_data_ptr->attributes.max_y;
        hid_param->min_z = hid_data_ptr->attributes.min_z;
        hid_param->max_z = hid_data_ptr->attributes.max_z;
      }
      else
      {
        HSU_MSG_ERROR("USBHOST_IOCTL_HID_GET_ATTRIB: NULL hid_param or hid_data_ptr.",
                      0,0,0);
        ret = USBHOST_ERROR;
      }
    }
    break;
  case USBHOST_IOCTL_HID_SET_IDLE_RATE:
    {
      uint8* idle_rate_ptr = (uint8*)param;

      if (idle_rate_ptr != NULL)
      {
        jsafe_enter();
        jresult = dev_data_ptr->input_dev_ptr->set_idle_cb(
                                      dev_data_ptr->input_dev_ptr->app_dev_ctx,
                                      *idle_rate_ptr);
        jsafe_leave();
        if (jresult != 0)
        {
          HSU_MSG_ERROR("hsu_host_al_hid_ioctl_func(): set_idle_cb() returned error %d.",
                        jresult, 0, 0);
          ret = USBHOST_ERROR;
        }
      }
      else
      {
        HSU_MSG_ERROR("USBHOST_IOCTL_HID_SET_IDLE_RATE: NULL parameter.",
          0,0,0);
        ret = USBHOST_ERROR;
      }
    }
    break;

  case USBHOST_IOCTL_HID_GET_IDLE_RATE:
    {
      uint8* idle_rate_ptr = (uint8*)param;

      if (idle_rate_ptr != NULL)
      {
        jsafe_enter();
        jresult = dev_data_ptr->input_dev_ptr->get_idle_cb(
                                      dev_data_ptr->input_dev_ptr->app_dev_ctx,
                                      idle_rate_ptr);
        jsafe_leave();
        if (jresult != 0)
        {
          HSU_MSG_ERROR("hsu_host_al_hid_ioctl_func(): get_idle_cb() returned error %d.",
                        jresult, 0, 0);
          ret = USBHOST_ERROR;
        }
      }
      else
      {
        HSU_MSG_ERROR("USBHOST_IOCTL_HID_GET_IDLE_RATE: NULL parameter.",
          0,0,0);
        ret = USBHOST_ERROR;
      }
    }
    break;

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */

  default:
    HSU_MSG_ERROR("hsu_host_al_hid_ioctl_func: unknown command code %d.",
      command,0,0);
    HSU_ASSERT((uint32)"hsu_host_al_hid_ioctl_func: unknown command" == NULL);
    ret = USBHOST_ERROR;
    break;
  }

  return ret;
}

/*===========================================================================

FUNCTION hsu_host_al_hid_get_usbd_handle

DESCRIPTION
This function retrieves the usbd handle associated with a given device.

DEPENDENCIES
None.

RETURN VALUE
A valid usbd handle on success. NULL otherwise.

SIDE EFFECTS
None.
===========================================================================*/
static void* hsu_host_al_hid_get_usbd_handle
(
  const usbhost_dev_entry_type* dev_entry_ptr
)
{
  void* ret;

  hsu_host_al_hid_device_data_type* dev_data_ptr = 
    (hsu_host_al_hid_device_data_type*)dev_entry_ptr->class_specific_data;

  if (dev_data_ptr == NULL)
  {
    HSU_MSG_ERROR("hsu_host_al_hid_get_usbd_handle: NULL device data.",
      0,0,0);
    HSU_ASSERT(dev_data_ptr!=NULL);
    return NULL;
  }

  jsafe_enter();
  ret = dev_data_ptr->input_dev_ptr->get_usbd_handle_cb(
                                      dev_data_ptr->input_dev_ptr->app_dev_ctx);
  jsafe_leave();
  return ret;
}

/*===========================================================================

FUNCTION hsu_host_al_hid_init

DESCRIPTION
This function initializes the adaptation layer.

DEPENDENCIES
None.

RETURN VALUE
TRUE for success, FALSE for failure.

SIDE EFFECTS
None.
===========================================================================*/
boolean hsu_host_al_hid_init(void)
{
  uint32 i;

  HSU_MSG_HIGH("Starting hsu_host_al_hid_init().",0,0,0);

  rex_init_crit_sect(&hsu_host_al_hid_device_data_crit_sect);
  for (i=0; i<USBHOST_MAX_CLIENTS; i++)
  {
    hsu_host_al_hid_client_notify_cb[i] = NULL;
  }

  for (i=0; i<USBHOST_MAX_DEVICES; i++)
  {
    hsu_host_al_hid_device_data_pool_entry_is_free[i] = TRUE;
  }

  /* USBHOST_PDEV_TYPE_HID_GENERIC is not supported.
  ** USBHOST_PDEV_TYPE_HID_GAME_PAD is not supported
  ** (only game pads that enumerate as joysticks).
  */
  usbhost_set_ioctl_func(USBHOST_PDEV_TYPE_HID_JOYSTICK,
                         hsu_host_al_hid_ioctl_func);
  usbhost_set_ioctl_func(USBHOST_PDEV_TYPE_HID_KEYBOARD,
                         hsu_host_al_hid_ioctl_func);
  usbhost_set_ioctl_func(USBHOST_PDEV_TYPE_HID_MOUSE,
                         hsu_host_al_hid_ioctl_func);
  
  usbhost_set_usbd_handle_func(USBHOST_PDEV_TYPE_HID_JOYSTICK,
                               hsu_host_al_hid_get_usbd_handle);
  usbhost_set_usbd_handle_func(USBHOST_PDEV_TYPE_HID_KEYBOARD,
                               hsu_host_al_hid_get_usbd_handle);
  usbhost_set_usbd_handle_func(USBHOST_PDEV_TYPE_HID_MOUSE,
                               hsu_host_al_hid_get_usbd_handle);

#if !defined(T_REXNT) && !defined(FEATURE_BT_EXTPF_HID_HOST) 
  hsu_host_al_hid_test_app_init();
#endif

  HSU_MSG_HIGH("Exiting hsu_host_al_hid_init().",0,0,0);
  return TRUE;
}

/*===========================================================================

FUNCTION hsu_host_al_hid_uninit

DESCRIPTION
This function un-initializes the adaptation layer.

DEPENDENCIES
None.

RETURN VALUE
TRUE for success, FALSE for failure.

SIDE EFFECTS
None.
===========================================================================*/
boolean hsu_host_al_hid_uninit(void)
{
  HSU_MSG_HIGH("Starting hsu_host_al_hid_uninit().",0,0,0);

  usbhost_set_ioctl_func(USBHOST_PDEV_TYPE_HID_JOYSTICK, NULL);
  usbhost_set_ioctl_func(USBHOST_PDEV_TYPE_HID_KEYBOARD, NULL);
  usbhost_set_ioctl_func(USBHOST_PDEV_TYPE_HID_MOUSE, NULL);

  usbhost_set_usbd_handle_func(USBHOST_PDEV_TYPE_HID_JOYSTICK, NULL);
  usbhost_set_usbd_handle_func(USBHOST_PDEV_TYPE_HID_KEYBOARD, NULL);
  usbhost_set_usbd_handle_func(USBHOST_PDEV_TYPE_HID_MOUSE, NULL);

  HSU_MSG_HIGH("Exiting hsu_host_al_hid_uninit().",0,0,0);
  return TRUE;
}

/*===========================================================================

FUNCTION hsu_host_al_hid_unregister_input_device

DESCRIPTION
Callback, called by the class driver when a HID device is disconnected.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_host_al_hid_unregister_input_device
(
  os_input_ctx_t ctx
)
{
  uint32 i;
  usbhost_dev_entry_type* dev_entry_ptr = (usbhost_dev_entry_type*)ctx;
  hsu_host_al_hid_device_data_type* dev_data_ptr = NULL; 
  hid_class_driver_disable_cb disable_cb = NULL;
  hid_app_dev_ctx_t dev_ctx;

  HSU_MSG_HIGH("Starting hsu_host_al_hid_unregister_input_device().",0,0,0);
  
  jsafe_leave();

  dev_data_ptr = (hsu_host_al_hid_device_data_type*)dev_entry_ptr->class_specific_data;
  disable_cb = dev_data_ptr->input_dev_ptr->disable_cb;
  dev_ctx = dev_data_ptr->input_dev_ptr->app_dev_ctx;

  /* If this is a keyboard, we need to clear the auto-repeat timer */
  if (dev_data_ptr->input_dev_ptr->device_type == HID_KEYBOARD
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif
     )
  {
    (void)rex_clr_timer(&dev_data_ptr->auto_repeat_timer);
  }

  usbhost_remove_device(dev_entry_ptr);

  /* Free up the class-specific data */
  for (i=0; i<USBHOST_MAX_DEVICES; i++)
  {
    if (dev_data_ptr == &hsu_host_al_hid_device_data_pool[i])
    {
      hsu_host_al_hid_device_data_pool_entry_is_free[i] = TRUE;
    }
  }

  jsafe_enter();
  disable_cb(dev_ctx);
  
#ifndef FEATURE_HSU_TEST
  /* Send debug event on success */
  event_report(EVENT_HS_USB_HID_DISCONECT) ;
#endif /*FEATURE_HSU_TEST*/
  HSU_MSG_HIGH("Exiting hsu_host_al_hid_unregister_input_device().",0,0,0);
}

/*===========================================================================

FUNCTION hsu_host_al_hid_update_modifiers

DESCRIPTION
Helper function.
Updates the fields of the modifiers_ptr parameter 
according to the usage parameter (which indicates a modifier key).

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
static void hsu_host_al_hid_update_modifiers
(
  hsu_host_al_hid_keyboard_modifiers_type* modifiers_ptr,
  usbhost_hid_usage_type usage,
  boolean event
)
{
  /* handle left key */
  if (usage == USBHOST_HID_USAGE_KEY_LEFT_CONTROL) 
  {
    modifiers_ptr->left_ctrl_key_down = event;
  }

  if (usage == USBHOST_HID_USAGE_KEY_LEFT_ALT) 
  {
    modifiers_ptr->left_alt_key_down = event;
  }

  if (usage == USBHOST_HID_USAGE_KEY_LEFT_SHIFT) 
  {
    modifiers_ptr->left_shift_key_down = event;
  }

  if (usage == USBHOST_HID_USAGE_KEY_LEFT_GUI) 
  {
    modifiers_ptr->left_gui_key_down = event;
  }

  /* handle right key */
  if (usage == USBHOST_HID_USAGE_KEY_RIGHT_CONTROL)
  {
    modifiers_ptr->right_ctrl_key_down = event;
  }

  if (usage == USBHOST_HID_USAGE_KEY_RIGHT_ALT)
  {
    modifiers_ptr->right_alt_key_down = event;
  }

  if (usage == USBHOST_HID_USAGE_KEY_RIGHT_SHIFT)
  {
    modifiers_ptr->right_shift_key_down = event;
  }

  if (usage == USBHOST_HID_USAGE_KEY_RIGHT_GUI)
  {
    modifiers_ptr->right_gui_key_down = event;
  }

  if ((usage == USBHOST_HID_USAGE_KEY_CAPS_LOCK) && event)
  {
    /* Caps-Lock state is reversed each time the caps-lock key is pressed down */
    modifiers_ptr->caps_lock_on = !modifiers_ptr->caps_lock_on;
  }

  if ((usage == USBHOST_HID_USAGE_KEY_NUM_LOCK) && event)
  {
    modifiers_ptr->num_lock_on = !modifiers_ptr->num_lock_on;
  }

  if ((usage == USBHOST_HID_USAGE_KEY_SCROLL_LOCK) && event)
  {
    modifiers_ptr->scroll_lock_on = !modifiers_ptr->scroll_lock_on;
  }
}

/*===========================================================================

FUNCTION hsu_host_al_hid_get_ascii

DESCRIPTION
Helper function.
Receives a usage relative to the keyboard page (indicating a keyboard key)
and modifiers info (i.e., which modifiers are on at the moment). 
Returns the corresponding acsii value.

DEPENDENCIES
None.

RETURN VALUE
The appropriate ascii value.

SIDE EFFECTS
None.
===========================================================================*/
static uint8 hsu_host_al_hid_get_ascii
(
  const hsu_host_al_hid_keyboard_modifiers_type* modifiers_ptr,
  uint8 usage /* Usage value, relative to the keyboard page */
)
{
  uint8 ret = 0;
  
  if ((usage >= HUKP_NUMLOCK) && (usage <= HUKP_EQUAL))
  {
    /* Numpad keypress */
    if (modifiers_ptr->num_lock_on)
    {
      ret = hsu_host_al_hid_uid_to_ascii_keypad_numLockOn[usage];
    }
    else
    {
      ret = hsu_host_al_hid_uid_to_ascii_keypad_numLockOff[usage];
    }
  }
  else if ((modifiers_ptr->left_shift_key_down)||(modifiers_ptr->right_shift_key_down))
  {
    if (modifiers_ptr->caps_lock_on)
    {
      ret = hsu_host_al_hid_uid_to_ascii_hi_CapsOn[usage];
    }
    else
    {
      ret = hsu_host_al_hid_uid_to_ascii_hi[usage];
    }
  }
  else
  {
    if (modifiers_ptr->caps_lock_on == TRUE)
    {
      ret = hsu_host_al_hid_uid_to_ascii_low_CapsOn[usage];
    }
    else
    {
      ret = hsu_host_al_hid_uid_to_ascii_low[usage];
    }
  }

  return ret;
}

/*===========================================================================

FUNCTION hsu_host_al_hid_get_data_modifier

DESCRIPTION
Helper function.
Computes a uint32 in the format expected by client applications,
based on the state of modifier keys.

DEPENDENCIES
None.

RETURN VALUE
The computed "data modifier".

SIDE EFFECTS
None.
===========================================================================*/
static uint32 hsu_host_al_hid_get_data_modifier
(
  const hsu_host_al_hid_keyboard_modifiers_type* modifiers_ptr
)
{
  uint32 ret = 0;

  if (modifiers_ptr->caps_lock_on)
  {
    ret |= USBHOST_HID_CAPS_LOCK_ON;
  }
  if (modifiers_ptr->scroll_lock_on)
  {
    ret |= USBHOST_HID_SCROLL_LOCK_ON;
  }
  if (modifiers_ptr->num_lock_on)
  {
    ret |= USBHOST_HID_NUM_LOCK_ON;
  }
  if ((modifiers_ptr->left_shift_key_down)||(modifiers_ptr->right_shift_key_down))
  {
    ret |= USBHOST_HID_SHIFT_KEY_ACTIVE;
  }
  if ((modifiers_ptr->left_ctrl_key_down)||(modifiers_ptr->right_ctrl_key_down))
  {
    ret |= USBHOST_HID_CTRL_KEY_ACTIVE;
  }
  if ((modifiers_ptr->left_alt_key_down)||(modifiers_ptr->right_alt_key_down))
  {
    ret |= USBHOST_HID_ALT_KEY_ACTIVE;
  }
  if ((modifiers_ptr->left_gui_key_down)||(modifiers_ptr->right_gui_key_down))
  {
    ret |= USBHOST_HID_GUI_KEY_ACTIVE;
  }

  return ret;
}

/*===========================================================================

FUNCTION hsu_host_al_hid_keyboard_leds_need_updating

DESCRIPTION
Helper function. Checks if the key that was pressed or released
affects the keyboard leds.

DEPENDENCIES
None.

RETURN VALUE
TRUE if the leds need to be updated, FALSE otherwise.

SIDE EFFECTS
None.
===========================================================================*/
static boolean hsu_host_al_hid_keyboard_leds_need_updating
(
  usbhost_hid_usage_type usage
)
{
  return ((usage==USBHOST_HID_USAGE_KEY_CAPS_LOCK) ||
         (usage==USBHOST_HID_USAGE_KEY_SCROLL_LOCK) ||
         (usage==USBHOST_HID_USAGE_KEY_NUM_LOCK));
}

/*===========================================================================

FUNCTION hsu_host_al_hid_update_keyboard_leds

DESCRIPTION
Helper function. Uses class driver functions in order
to update the state of the keyboard leds.

DEPENDENCIES
None.

RETURN VALUE
TRUE if the leds need to be updated, FALSE otherwise.

SIDE EFFECTS
None.
===========================================================================*/
static void hsu_host_al_hid_update_keyboard_leds
(
  const hsu_host_al_hid_device_data_type* hid_data 
)
{
  input_device_reg_t* input_device;
  uint8 leds;
  jresult_t jresult;

  input_device = hid_data->input_dev_ptr;
  if (!input_device->send_out_cb)
  {
    HSU_MSG_ERROR("hsu_host_al_hid_update_keyboard_leds(): No send_out_cb!",
                  0,0,0);
    return;
  }

  /* No need for os_safe_enter(), since this function is called in stack context */
  jresult = input_device->set_report_field_cb(input_device->app_dev_ctx,
                                              HID_USAGE2(HUP_LEDS,HUL_SCROLL_LOCK),
                                              hid_data->keyboard_modifiers.scroll_lock_on,
                                              &leds, 0);
  if (jresult != 0)
  {
    HSU_MSG_ERROR("hsu_host_al_hid_update_keyboard_leds(): set_report_field_cb failed with error %d.",
                  jresult,0,0);
    return;
  }
  
  jresult = input_device->set_report_field_cb(input_device->app_dev_ctx,
                                              HID_USAGE2(HUP_LEDS,HUL_NUM_LOCK),
                                              hid_data->keyboard_modifiers.num_lock_on,
                                              &leds, 0);
  if (jresult != 0)
  {
    HSU_MSG_ERROR("hsu_host_al_hid_update_keyboard_leds(): set_report_field_cb failed with error %d.",
                  jresult,0,0);
    return;
  }

  jresult = input_device->set_report_field_cb(input_device->app_dev_ctx,
                                              HID_USAGE2(HUP_LEDS,HUL_CAPS_LOCK),
                                              hid_data->keyboard_modifiers.caps_lock_on,
                                              &leds, 0);
  if (jresult != 0)
  {
    HSU_MSG_ERROR("hsu_host_al_hid_update_keyboard_leds(): set_report_field_cb failed with error %d.",
                  jresult,0,0);
    return;
  }

  jresult = input_device->send_out_cb(input_device->app_dev_ctx, &leds, 1, 0);
  if (jresult != 0)
  {
    HSU_MSG_ERROR("hsu_host_al_hid_update_keyboard_leds(): send_out_cb failed with error %d.",
                  jresult,0,0);
    return;
  }
}

/*===========================================================================

FUNCTION hsu_host_al_hid_report_event

DESCRIPTION
Callback function. Called by the class driver when a keyboard key is pressed
or a mouse/joystick button is clicked. If needed, it notified registered 
client applications of the appropriate event.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
static void hsu_host_al_hid_report_event
(
 os_input_ctx_t ctx, 
 /* For keyboards, keycode is the ANSI key code. For mouse/joystick buttons,
 ** keycode is the button number.
 */
 juint8_t keycode,
 /* 1 means ON, 0 means OFF */
 juint8_t event, 
 usage_att_t *att
)
{
  usbhost_dev_entry_type* dev_entry_ptr;
  usbhost_client_id_type client_id = USBHOST_INVALID_CLIENT_ID;
  usbhost_hid_event_type hid_event;
  hsu_host_al_hid_device_data_type* hid_data;
  input_device_type_t device_type;
  uint16 usage; /* without the usage page */
#ifdef CANDLEBOX_FEATURE_USBHID
  static boolean pendownreceived = 0;
#endif /* CANDLEBOX_FEATURE_USBHID */
  HSU_PARAM_COULD_BE_CONST(att);

  dev_entry_ptr = (usbhost_dev_entry_type*)ctx;

  #ifdef CANDLEBOX_FEATURE_USBHID
 	 if(dev_entry_ptr == NULL)
	  {
	    return;
	  }

	  if(dev_entry_ptr->curr_client == NULL)
	  {
	    return;
	  }

	  if(dev_entry_ptr->curr_client->data == NULL)
	  {
	    return;
	  }
  #endif /* CANDLEBOX_FEATURE_USBHID */
  if (dev_entry_ptr->curr_client != NULL) 
  {
  client_id = dev_entry_ptr->curr_client->data->client_id;
  }

  hid_data = (hsu_host_al_hid_device_data_type*)dev_entry_ptr->class_specific_data;
  device_type = hid_data->input_dev_ptr->device_type;

  usage = (uint16)(HID_USAGE_USAGE(att->usage));

  memset(&hid_event, 0, sizeof(hid_event));

  if (device_type == HID_KEYBOARD 
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif
     )
  {
    HSU_ASSERT((uint32)usage<HSU_HOST_AL_HID_MAX_ASCII_TBL_SIZ);

    hid_event.usage = hsu_host_al_hid_uid_to_usbhost_api_usage[usage];

    if (hid_event.usage == USBHOST_HID_USAGE_NONE)
    {
      /* No usage defined for this key */
      HSU_MSG_HIGH(
        "hsu_host_al_hid_report_event: no usbhost usage defined for key %d.",
        usage,0,0);
      return;
    }

    /* Update modifiers */
    hsu_host_al_hid_update_modifiers(&hid_data->keyboard_modifiers, 
                                     hid_event.usage,
                                     event);

    if (hsu_host_al_hid_keyboard_leds_need_updating(hid_event.usage) && event)
    {
      hsu_host_al_hid_update_keyboard_leds(hid_data);
    }

    if (usage >= HSU_HOST_AL_HID_MAX_ASCII_TBL_SIZ)
    {
      hid_event.data_val = 0;
    }
    else
    {
      hid_event.data_val = hsu_host_al_hid_get_ascii(
                              &hid_data->keyboard_modifiers,
                              (uint8)usage);
    }

    hid_event.data_modifier = hsu_host_al_hid_get_data_modifier(
                                                &hid_data->keyboard_modifiers);

    if (((hid_data->auto_repeat_event.usage != hid_event.usage) && event) ||
        ((hid_data->auto_repeat_event.usage == hid_event.usage) && !event))
    {
      /* We stop auto-repeating if the currently auto-repeated
      ** key is released or if a different key is pressed.
      */
      (void)rex_clr_timer(&hid_data->auto_repeat_timer);
      rex_enter_crit_sect(&hid_data->auto_repeat_crit_sect);
      memset(&hid_data->auto_repeat_event, 0, sizeof(usbhost_hid_event_type));
      hid_data->auto_repeat_event.usage = USBHOST_HID_USAGE_NONE;
      rex_leave_crit_sect(&hid_data->auto_repeat_crit_sect);
    }

    /* See if we need to auto-repeat */
    if (event && (hid_event.data_val != 0) &&
        ((hid_data->auto_repeat_event.usage != hid_event.usage)))
    {
      /* A key was pressed && the key has an ascii value &&
      ** the key is not already being auto-repeated.
      ** --> start auto-repeating.
      */
      rex_enter_crit_sect(&hid_data->auto_repeat_crit_sect);
      hid_data->auto_repeat_event.data_val      = hid_event.data_val;
      hid_data->auto_repeat_event.usage         = hid_event.usage;
      hid_data->auto_repeat_event.data_flag     = att->flags;
#ifdef  CANDLEBOX_FEATURE_USBHID
      hid_data->auto_repeat_event.data_mkbrk    = event;
      hid_data->auto_repeat_event.data_modifier = hid_event.data_modifier;
#endif /* CANDLEBOX_FEATURE_USBHID */

      hsu_host_al_hid_set_min_max(&hid_data->auto_repeat_event, att);
      
      hid_data->auto_repeat_event.data_unit     = (uint32)att->unit;

      /* TODO: Set data_unitExpo and data_report_id */

      rex_leave_crit_sect(&hid_data->auto_repeat_crit_sect);

#ifdef CANDLEBOX_FEATURE_USBHID
      (void)rex_set_timer(&hid_data->auto_repeat_timer, 
                          hsu_host_al_hid_auto_repeat_delay);
#else /* CANDLEBOX_FEATURE_USBHID */
      (void)rex_set_timer(&hid_data->auto_repeat_timer, 
                          HSU_HOST_AL_HID_AUTO_REPEAT_DELAY);
#endif /* !CANDLEBOX_FEATURE_USBHID */
    }

#ifdef CANDLEBOX_FEATURE_USBHID
    /* app needs to know whether the key is down or up */
    hid_event.data_mkbrk = event;
#else /* CANDLEBOX_FEATURE_USBHID */
    /* Client is notified only when a key is pressed down, 
    ** not when it is released.
    */
    if (!event)
    {
      return;
    }
#endif /* !CANDLEBOX_FEATURE_USBHID */
  }
  else
  { /* Not a keyboard ... */
    hid_event.usage = hsu_host_al_hid_get_usbhost_api_usage(att->usage);
    if (hid_event.usage == USBHOST_HID_USAGE_NONE)
    {
      /* No usage defined in the host api for the reported usage */
      HSU_MSG_HIGH(
        "hsu_host_al_hid_report_event: no usbhost usage defined for usage %d.",
        att->usage,0,0);
      return;
    }

    if ((keycode == 0) && (hid_event.usage == USBHOST_HID_USAGE_WHEEL))
    {
      /* No need to notify the client in this case */
      return;
    }

    hid_event.data_mkbrk    = event;
#ifdef CANDLEBOX_FEATURE_USBHID
	if(1 == hid_event.data_mkbrk)
	{
	   pendownreceived = 1;
	}
#endif /* CANDLEBOX_FEATURE_USBHID */
    hid_event.data_val = keycode;
  }

  if (client_id == USBHOST_INVALID_CLIENT_ID || 
      hsu_host_al_hid_client_notify_cb[client_id] == NULL)
  {
    /* No client to report to */
    return;
  }
  
  if (hid_event.usage == USBHOST_HID_USAGE_BUTTON)
  {
    HSU_ASSERT(keycode < HSU_HOST_AL_HID_MAX_BUTTONS);
    if (hid_data->button_pressed[keycode] == (boolean)event)
    {
      /* No change in the button's state --> no need to notify the app */
      return;
    }
    hid_data->button_pressed[keycode] = (boolean)event;
  }

  hid_event.data_flag     = att->flags;

  hsu_host_al_hid_set_min_max(&hid_event, att);

  hid_event.data_unit     = (uint32)att->unit;
  
  /* TODO: Set hid_event.data_unitExpo and hid_event.data_report_id */

  clk_uptime_ms(hid_event.timestamp);
  
  /* Notify the client application.
  ** But before we do, we release the giant.
  */
  jsafe_leave();
#ifdef CANDLEBOX_FEATURE_USBHID
  if(device_type == HID_MOUSE)
  {
    //If pen is down or up send callback function
  	if((1 == hid_event.data_mkbrk) || (0 == hid_event.data_mkbrk && 1 == pendownreceived))
    {
       hsu_host_al_hid_client_notify_cb[client_id](dev_entry_ptr->dev_inst_id, 
                                              dev_entry_ptr->pdev_type, 
                                              &hid_event);
	   if(0 == hid_event.data_mkbrk)
	   {
	     pendownreceived = 0;
	   }
    }
    else
	{
	  //Do not escalate the callback function
	}
  }
  else
#endif /* CANDLEBOX_FEATURE_USBHID */
  {

     hsu_host_al_hid_client_notify_cb[client_id](dev_entry_ptr->dev_inst_id, 
                                              dev_entry_ptr->pdev_type, 
                                              &hid_event);
  }
  jsafe_enter();
}

/*===========================================================================

FUNCTION hsu_host_al_hid_report_data

DESCRIPTION
This function is called by the hsu_host_al_hid_report_relative_data and 
hsu_host_al_hid_report_absolute_data callback functions.
Depending on the usage reported (in the att parameter), it either updates
the sampled data or notified the client applications.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
static void hsu_host_al_hid_report_data
(
  os_input_ctx_t ctx, 
  juint8_t data_type, 
  jint32_t data, 
  const usage_att_t *att
)
{
  usbhost_dev_entry_type* dev_entry_ptr;
  usbhost_hid_event_type* sampled_data_event_ptr;
  hsu_host_al_hid_device_data_type* hid_data;
  usbhost_hid_usage_type usbhost_usage;
  usbhost_hid_event_type hid_event;
  hid_notify_cb notify_cb;          

  HSU_USE_PARAM(data_type);

  dev_entry_ptr = (usbhost_dev_entry_type*)ctx;
  hid_data = (hsu_host_al_hid_device_data_type*)dev_entry_ptr->class_specific_data;
  
  usbhost_usage = hsu_host_al_hid_get_usbhost_api_usage(att->usage);
  if (usbhost_usage == USBHOST_HID_USAGE_NONE)
  {
    /* No usage defined in the host api for the reported usage */
    HSU_MSG_HIGH(
      "hsu_host_al_hid_report_data: no usbhost usage defined for usage %d.",
      att->usage,0,0);
    return;
  }

  if ((data == 0) && (usbhost_usage == USBHOST_HID_USAGE_WHEEL))
  {
    /* No need to notify the client in this case */
    return;
  }

#ifdef CANDLEBOX_FEATURE_USBHID
  if(usbhost_usage == USBHOST_HID_USAGE_X)
  {
       g_Mouse_Coordinate_x+= data;  	
  }
  if(usbhost_usage == USBHOST_HID_USAGE_Y)
  {
       g_Mouse_Coordinate_y+= data;
  }
#endif /* CANDLEBOX_FEATURE_USBHID */

  if (usbhost_usage == USBHOST_HID_USAGE_HATSWITCH)
  {
    if (hid_data->got_hatswitch_value == TRUE)
    {
      if (hid_data->last_hatswitch_value == data)
      {
        /* No change in the hatswitch state --> no need to notify the app */
        return;
      }
    }
    else
    {
      hid_data->got_hatswitch_value = TRUE;
    }
    hid_data->last_hatswitch_value = data;
  }

  /* Update sampled data */
  sampled_data_event_ptr = NULL;
  switch(usbhost_usage) {
  case USBHOST_HID_USAGE_X:
    sampled_data_event_ptr = &hid_data->sampled_data.usage_x;
  	break;
  case USBHOST_HID_USAGE_Y:
    sampled_data_event_ptr = &hid_data->sampled_data.usage_y;
    break;
  case USBHOST_HID_USAGE_Z:
    sampled_data_event_ptr = &hid_data->sampled_data.usage_z;
    break;
  case USBHOST_HID_USAGE_RX:
    sampled_data_event_ptr = &hid_data->sampled_data.usage_rx;
    break;
  case USBHOST_HID_USAGE_RY:
    sampled_data_event_ptr = &hid_data->sampled_data.usage_ry;
    break;
  case USBHOST_HID_USAGE_RZ:
    sampled_data_event_ptr = &hid_data->sampled_data.usage_rz;
    break;
  case USBHOST_HID_USAGE_VX:
    sampled_data_event_ptr = &hid_data->sampled_data.usage_vx;
    break;
  case USBHOST_HID_USAGE_VY:
    sampled_data_event_ptr = &hid_data->sampled_data.usage_vy;
    break;
  case USBHOST_HID_USAGE_VZ:
    sampled_data_event_ptr = &hid_data->sampled_data.usage_vz;
    break;
  case USBHOST_HID_USAGE_VBRX:
    sampled_data_event_ptr = &hid_data->sampled_data.usage_vbrx;
    break;
  case USBHOST_HID_USAGE_VBRY:
    sampled_data_event_ptr = &hid_data->sampled_data.usage_vbry;
    break;
  case USBHOST_HID_USAGE_VBRZ:
    sampled_data_event_ptr = &hid_data->sampled_data.usage_vbrz;
    break;
  /* The following usages require immediate client notification. */
  case USBHOST_HID_USAGE_DIAL:
  case USBHOST_HID_USAGE_WHEEL:
  case USBHOST_HID_USAGE_HATSWITCH:
  case USBHOST_HID_USAGE_BYTECOUNTER:
  case USBHOST_HID_USAGE_MOTIONWAKEUP:
  case USBHOST_HID_USAGE_START:
  case USBHOST_HID_USAGE_SELECT:
  case USBHOST_HID_USAGE_SYSCONTRL:
  case USBHOST_HID_USAGE_SYSPOWERDOWN:
  case USBHOST_HID_USAGE_SYSSLEEP:
  case USBHOST_HID_USAGE_SYSWAKE:
  case USBHOST_HID_USAGE_SYSCONMENU:
  case USBHOST_HID_USAGE_SYSMAINMENU:
  case USBHOST_HID_USAGE_SYSAPPMENU:
  case USBHOST_HID_USAGE_SYSMENUHELP:
  case USBHOST_HID_USAGE_SYSMENUEXIT:
  case USBHOST_HID_USAGE_SYSMENURIGHT:
  case USBHOST_HID_USAGE_SYSMENULEFT:
  case USBHOST_HID_USAGE_SYSMENUUP:
  case USBHOST_HID_USAGE_SYSMENUDOWN:
  case USBHOST_HID_USAGE_DPADUP:
  case USBHOST_HID_USAGE_DPADDOWN:
  case USBHOST_HID_USAGE_DPADRIGHT:
  case USBHOST_HID_USAGE_DPADLEFT:
    {
      if (dev_entry_ptr->curr_client != NULL)
      {
      memset(&hid_event, 0, sizeof(hid_event));

      hid_event.seq_num       = 0;
      hid_event.usage         = usbhost_usage;
      hid_event.data_val      = data;
      hid_event.data_flag     = att->flags;

      hsu_host_al_hid_set_min_max(&hid_event, att);

      hid_event.data_unit     = (uint32)att->unit;
      hid_event.data_modifier = 0;

      /* TODO: Set data_unitExpo and data_report_id */

      clk_uptime_ms(hid_event.timestamp);

      /* Notify the client if one is registered */
      notify_cb = 
        hsu_host_al_hid_client_notify_cb[dev_entry_ptr->curr_client->data->client_id];
      if (notify_cb != NULL)
      {
        jsafe_leave();
        notify_cb(dev_entry_ptr->dev_inst_id, 
                  dev_entry_ptr->pdev_type,
                  &hid_event);
        jsafe_enter();
      }
    }
    }
    break;
  default:
    break;
  }

  if (sampled_data_event_ptr != NULL)
  {
    sampled_data_event_ptr->seq_num++;
    sampled_data_event_ptr->usage = usbhost_usage;
    sampled_data_event_ptr->data_val  = data;
    sampled_data_event_ptr->data_flag = att->flags;

    hsu_host_al_hid_set_min_max(sampled_data_event_ptr, att);

    sampled_data_event_ptr->data_unit = (uint32)att->unit;
    sampled_data_event_ptr->data_modifier = 0;

    /* TODO: Set data_unitExpo and data_report_id */

    clk_uptime_ms(sampled_data_event_ptr->timestamp);
  }
}

/*===========================================================================

FUNCTION hsu_host_al_hid_report_relative_data

DESCRIPTION
Callback function. Called by the class driver when a HID device sends relative
data to the host.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
static void hsu_host_al_hid_report_relative_data
(
  os_input_ctx_t ctx, 
  juint8_t data_type, 
  jint32_t data, 
  usage_att_t *att
)
{
  HSU_PARAM_COULD_BE_CONST(att);
  hsu_host_al_hid_report_data(ctx, data_type, data, att);
}

/*===========================================================================

FUNCTION hsu_host_al_hid_report_absolute_data

DESCRIPTION
Callback function. Called by the class driver when a HID device sends absolute
data to the host.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_host_al_hid_report_absolute_data
(
  os_input_ctx_t ctx, 
  juint8_t data_type,   
  juint32_t data, 
  usage_att_t *att
)
{
  HSU_PARAM_COULD_BE_CONST(att);
  hsu_host_al_hid_report_data(ctx, data_type, (jint32_t)data, att);
}

/*===========================================================================

FUNCTION hsu_host_al_hid_get_attributes

DESCRIPTION
Helper function. Gets the attributes of a device from the class driver.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
static void hsu_host_al_hid_get_attributes
(
  const input_device_reg_t* input_dev, 
  usbhost_hid_attrib_type* attrib_ptr
)
{
  usage_att_t       att;
  jresult_t         jresult;
  hid_app_dev_ctx_t jctx = input_dev->app_dev_ctx;

  jsafe_enter();

  attrib_ptr->max_buttons = (uint16)input_dev->get_max_buttons_cb(jctx);

  jresult = input_dev->query_usage_att_cb(jctx, USAGE_X, &att);
  if (jresult == 0)
  {
    if (att.physical_min!=0)
    {
      attrib_ptr->min_x = att.physical_min;
    }
    else
    {
      attrib_ptr->min_x = att.logical_min;
    }

    if (att.physical_max!=0)
    {
      attrib_ptr->max_x = att.physical_max;
    }
    else
    {
      attrib_ptr->max_x = att.logical_max;
    }
  }
  else
  {
    HSU_MSG_ERROR("query_usage_att_cb(jctx, USAGE_X, &att) failed with result %d.",
                  jresult,0,0);
    attrib_ptr->min_x = 0;
    attrib_ptr->max_x = 0;
  }

  jresult = input_dev->query_usage_att_cb(jctx, USAGE_Y, &att);
  if (jresult == 0)
  {
    if (att.physical_min!=0)
    {
      attrib_ptr->min_y = att.physical_min;
    }
    else
    {
      attrib_ptr->min_y = att.logical_min;
    }

    if (att.physical_max!=0)
    {
      attrib_ptr->max_y = att.physical_max;
    }
    else
    {
      attrib_ptr->max_y = att.logical_max;
    }
  }
  else
  {
    HSU_MSG_ERROR("query_usage_att_cb(jctx, USAGE_Y, &att) failed with result %d.",
                  jresult,0,0);
    attrib_ptr->min_y = 0;
    attrib_ptr->max_y = 0;
  }

  jresult = input_dev->query_usage_att_cb(jctx, USAGE_Z, &att);
  if (jresult == 0)
  {
    if (att.physical_min!=0)
    {
      attrib_ptr->min_z = att.physical_min;
    }
    else
    {
      attrib_ptr->min_z = att.logical_min;
    }

    if (att.physical_max!=0)
    {
      attrib_ptr->max_z = att.physical_max;
    }
    else
    {
      attrib_ptr->max_z = att.logical_max;
    }
  }
  else
  {
    HSU_MSG_ERROR("query_usage_att_cb(jctx, USAGE_Z, &att) failed with result %d.",
                  jresult,0,0);
    attrib_ptr->min_z = 0;
    attrib_ptr->max_z = 0;
  }

  jsafe_leave();
}

/*===========================================================================

FUNCTION hsu_host_al_hid_auto_repeat_timer_cb

DESCRIPTION
Timer callback.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
static void hsu_host_al_hid_auto_repeat_timer_cb(unsigned long arg)
{
  usbhost_dev_entry_type* dev_entry_ptr = (usbhost_dev_entry_type*)arg;
  hsu_host_al_hid_device_data_type* hid_data = 
    (hsu_host_al_hid_device_data_type*)dev_entry_ptr->class_specific_data;
  usbhost_client_id_type client_id;
  
  if (dev_entry_ptr->curr_client == NULL)
  {
    return;
  }

  client_id = dev_entry_ptr->curr_client->data->client_id;

  if (hsu_host_al_hid_client_notify_cb[client_id] == NULL)
  {
    return;
  }

  rex_enter_crit_sect(&hid_data->auto_repeat_crit_sect);
  if (hid_data->auto_repeat_event.usage == USBHOST_HID_USAGE_NONE)
  {
    rex_leave_crit_sect(&hid_data->auto_repeat_crit_sect);
    return;
  }

  /* Notify the applications of the key press event */
  clk_uptime_ms(hid_data->auto_repeat_event.timestamp);

#ifdef CANDLEBOX_FEATURE_USBHID
  /* there is no way to configure or disable the auto-repeat interval */
  /* there must be some way to detect that the event is an auto-repeat */
  hid_data->auto_repeat_event.seq_num ++;
#endif /* CANDLEBOX_FEATURE_USBHID */

  hsu_host_al_hid_client_notify_cb[client_id](dev_entry_ptr->dev_inst_id, 
                                              dev_entry_ptr->pdev_type, 
                                              &hid_data->auto_repeat_event);
  rex_leave_crit_sect(&hid_data->auto_repeat_crit_sect);

#ifdef CANDLEBOX_FEATURE_USBHID
  (void)rex_set_timer(&hid_data->auto_repeat_timer, 
                      hsu_host_al_hid_auto_repeat_interval);
#else /* CANDLEBOX_FEATURE_USBHID */
  (void)rex_set_timer(&hid_data->auto_repeat_timer, 
                      HSU_HOST_AL_HID_AUTO_REPEAT_INTERVAL);
#endif /* !CANDLEBOX_FEATURE_USBHID */
}

/*===========================================================================

FUNCTION register_input_device

DESCRIPTION
This function is called by the class driver when a HID device is connected.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
jresult_t register_input_device 
(
 input_device_reg_t *input_dev, 
 os_input_ctx_t *ctx,
 os_input_ops_t *ops
 )
{
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */
  usbhost_pdev_type                 pdev_type;
  usbhost_dev_entry_type*           dev_entry_ptr;
  hsu_host_al_hid_device_data_type* dev_data_ptr;
  uint32                            i, dev_data_index;
#ifndef FEATURE_HSU_TEST
  uint16                            dev_event_payload; /*help variable for event_report */    
#endif /*FEATURE_HSU_TEST*/
  jresult_t                         result;

  HSU_MSG_HIGH("Starting register_input_device().",0,0,0);

  switch(input_dev->device_type) {
  case HID_KEYBOARD:
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif 
    pdev_type = USBHOST_PDEV_TYPE_HID_KEYBOARD;
    break;
  case HID_MOUSE:
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif
    pdev_type = USBHOST_PDEV_TYPE_HID_MOUSE;
    break;
  case HID_JOYSTICK:
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif
    pdev_type = USBHOST_PDEV_TYPE_HID_JOYSTICK;
    break;
  case HID_GAMEPAD:
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif
    pdev_type = USBHOST_PDEV_TYPE_HID_GAME_PAD;
    break;
  default:
    HSU_MSG_ERROR("register_input_device(): Unrecognized type of HID device.",0,0,0);
#ifndef FEATURE_HSU_TEST
    dev_event_payload = (uint16)HS_USB_UNKNOWN_HID ;
    event_report_payload(EVENT_HS_USB_OPERATIONAL_ERROR, 
      sizeof(uint16),(void*)(&dev_event_payload));
#endif /*FEATURE_HSU_TEST*/
    return JENOTSUP;
  }

  /* Find a free entry for the class-specific data */

  rex_enter_crit_sect(&hsu_host_al_hid_device_data_crit_sect);

  dev_data_index = 0;
  dev_data_ptr = NULL;
  for (i=0; i<USBHOST_MAX_DEVICES; i++)
  {
    if (hsu_host_al_hid_device_data_pool_entry_is_free[i])
    {
      dev_data_index = i;
      hsu_host_al_hid_device_data_pool_entry_is_free[i] = FALSE;
      dev_data_ptr = &hsu_host_al_hid_device_data_pool[i];
      break;
    }
  }

  rex_leave_crit_sect(&hsu_host_al_hid_device_data_crit_sect);

  if (dev_data_ptr == NULL)
  {
    HSU_MSG_ERROR("register_input_device(): No free entry in hsu_host_al_hid_device_data_pool.",0,0,0);
#ifndef FEATURE_HSU_TEST
    dev_event_payload = (uint16)HS_USB_ADD_PERIF_NO_FREE_ENTRY;
    event_report_payload(EVENT_HS_USB_OPERATIONAL_ERROR, 
      sizeof(uint16), (void*)(&dev_event_payload));
#endif /*FEATURE_HSU_TEST*/
    return JENOMEM;
  }

  dev_data_ptr->input_dev_ptr = input_dev;

  for (i=0; i<HSU_HOST_AL_HID_MAX_BUTTONS; i++)
  {
    dev_data_ptr->button_pressed[i] = FALSE;
  }

  dev_data_ptr->got_hatswitch_value = FALSE;

  dev_data_ptr->keyboard_modifiers.left_alt_key_down = FALSE;
  dev_data_ptr->keyboard_modifiers.left_ctrl_key_down = FALSE;
  dev_data_ptr->keyboard_modifiers.left_shift_key_down = FALSE;
  dev_data_ptr->keyboard_modifiers.left_gui_key_down = FALSE;
  dev_data_ptr->keyboard_modifiers.right_alt_key_down = FALSE;
  dev_data_ptr->keyboard_modifiers.right_ctrl_key_down = FALSE;
  dev_data_ptr->keyboard_modifiers.right_shift_key_down = FALSE;
  dev_data_ptr->keyboard_modifiers.right_gui_key_down = FALSE;
  dev_data_ptr->keyboard_modifiers.num_lock_on = FALSE;
  dev_data_ptr->keyboard_modifiers.caps_lock_on = FALSE;
  dev_data_ptr->keyboard_modifiers.scroll_lock_on = FALSE;

  ops->unregister_input_device = hsu_host_al_hid_unregister_input_device;
  ops->report_event = hsu_host_al_hid_report_event;
  ops->report_absolute_data = hsu_host_al_hid_report_absolute_data;
  ops->report_relative_data = hsu_host_al_hid_report_relative_data;

  /* As far as we're concerned, the device should be enabled immediately.*/
  result = input_dev->enable_cb(input_dev->app_dev_ctx);
  if (result != 0)
  {
    HSU_MSG_ERROR("register_input_device(): enable_cb() returned error %d.",
                  result,0,0);
    /* Can't enable --> no point in trying to work with this device */
    /* Free up the data */
    hsu_host_al_hid_device_data_pool_entry_is_free[dev_data_index] = TRUE;
#ifndef FEATURE_HSU_TEST
    dev_event_payload = (uint16)HS_USB_CANT_ENABLE_HID;
    event_report_payload(EVENT_HS_USB_OPERATIONAL_ERROR, 
      sizeof(uint16),(void*)(&dev_event_payload));
#endif /*FEATURE_HSU_TEST*/
    return result;
  }

  /* Get the device attributes (irrelevant for keyboards) */
  if (pdev_type != USBHOST_PDEV_TYPE_HID_KEYBOARD)
  {
    hsu_host_al_hid_get_attributes(input_dev, 
                                   &dev_data_ptr->attributes);
  }

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */

  /* Create a new device entry and add it to the device
  ** table maintained by the host api module.
  */
  jsafe_leave();
  dev_entry_ptr = usbhost_add_device(USBHOST_DEV_TYPE_CHAR, 
                                     pdev_type,
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */
                                     dev_data_ptr);
  jsafe_enter();

  if (dev_entry_ptr == NULL)
  {
    HSU_MSG_ERROR("register_input_device(): usbhost_add_device() returned NULL.",
                  0,0,0);
    /* Free up the data */
    hsu_host_al_hid_device_data_pool_entry_is_free[dev_data_index] = TRUE;
    return JENOMEM;
  }

  if (pdev_type == USBHOST_PDEV_TYPE_HID_KEYBOARD)
  {
    memset(&dev_data_ptr->auto_repeat_event, 0, sizeof(usbhost_hid_event_type));
    dev_data_ptr->auto_repeat_event.usage = USBHOST_HID_USAGE_NONE;
    rex_init_crit_sect(&dev_data_ptr->auto_repeat_crit_sect);
    rex_def_timer_ex(&dev_data_ptr->auto_repeat_timer,
                     hsu_host_al_hid_auto_repeat_timer_cb,
                     (unsigned long)dev_entry_ptr);
  }

  *ctx = dev_entry_ptr;
#ifndef FEATURE_HSU_TEST
  /* Send debug event on sucsess */
  switch(input_dev->device_type) {
  case HID_KEYBOARD:
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif
    dev_event_payload = (uint16)HS_USB_ATTACHED_HID_KEYBOARD;
    break;
  case HID_MOUSE:
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif
    dev_event_payload = (uint16)HS_USB_ATTACHED_HID_MOUSE;
    break;
  case HID_JOYSTICK:
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif
    dev_event_payload = (uint16)HS_USB_ATTACHED_HID_JOYSTICK;
    break;
  case HID_GAMEPAD:
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif
    dev_event_payload = (uint16)HS_USB_ATTACHED_HID_GAMEPAD;
    break;
  default:
    HSU_ASSERT("register_input_device(): Unrecognized type of HID device." == NULL);
    break;
  }
  event_report_payload(EVENT_HS_USB_HID_CONNECT ,
    sizeof(uint16),(void*)(&dev_event_payload));

#endif /*FEATURE_HSU_TEST*/  
  HSU_MSG_HIGH("Exiting register_input_device().",0,0,0);
  return 0;
}

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */
