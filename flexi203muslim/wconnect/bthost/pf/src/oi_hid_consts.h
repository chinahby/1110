#ifndef _OI_HID_CONSTS_H
#define _OI_HID_CONSTS_H

/**
 * @file  
 * This file contains definitions for values specified in the Human Interface Device (HID) Profile.
 */

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include "oi_stddefs.h"

/** \addtogroup HID HID APIs */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif


/** HID profile-specific SDP attribute IDs */
enum {
    OI_ATTRID_HIDFirstAttribute =      0x0200,
    
    OI_ATTRID_HIDDeviceReleaseNumber = 0x0200,
    OI_ATTRID_HIDProfileVersion  =     0x0201,
    OI_ATTRID_HIDDeviceSubclass =      0x0202,
    OI_ATTRID_HIDCountryCode =         0x0203,
    OI_ATTRID_HIDVirtualCable =        0x0204,
    OI_ATTRID_HIDReconnectInitiate =   0x0205,
    OI_ATTRID_HIDDescriptorList =      0x0206,
    OI_ATTRID_HIDLANGIDBaseList =      0x0207,
    OI_ATTRID_HIDSDPDisable =          0x0208,
    OI_ATTRID_HIDBatteryPower =        0x0209,
    OI_ATTRID_HIDRemoteWake =          0x020a,
    OI_ATTRID_HIDParserVersion =       0x020b,
    OI_ATTRID_HIDSupervisionTimeout =  0x020c,
    OI_ATTRID_HIDNormallyConnectable = 0x020d,
    OI_ATTRID_HIDBootDevice =          0x020e,

    OI_ATTRID_HIDLastAttribute =       0x020e
};

#define OI_HID_PARSER_VERSION 0x0111
#define OI_HID_PROFILE_VERSION 0x0100


/** Descriptor type codes (Bluetooth HID specification section 7.11.6) */
enum {
    OI_HIDClassDescriptor_Report =     0x0022,
    OI_HIDClassDescriptor_Physical =   0x0023
};


/** BT HID transaction types (Bluetooth HID specification section 7.3) */
enum {
    OI_HID_HANDSHAKE =                 0x0,
    OI_HID_CONTROL =                   0x1,
    OI_HID_GET_REPORT =                0x4,
    OI_HID_SET_REPORT =                0x5,
    OI_HID_GET_PROTOCOL =              0x6,
    OI_HID_SET_PROTOCOL =              0x7,
    OI_HID_GET_IDLE =                  0x8,
    OI_HID_SET_IDLE =                  0x9,
    OI_HID_DATA =                      0xa,
    OI_HID_DATC =                      0xb
};

#define OI_HID_NO_TRANSACTION 0

/** BT HID transaction parameter types (Bluetooth HID specification sections 7.4.1-7.4.10) */
enum {
    OI_HID_HANDSHAKE_SUCCESSFUL =      0x0,
    OI_HID_HANDSHAKE_NOT_READY =       0x1,
    OI_HID_HANDSHAKE_ERR_INVALID_REPORT_ID = 0x2,
    OI_HID_HANDSHAKE_ERR_UNSUPPORTED_REQUEST = 0x3,
    OI_HID_HANDSHAKE_ERR_INVALID_PARAMETER = 0x4,
    OI_HID_HANDSHAKE_ERR_UNKNOWN =     0xe,
    OI_HID_HANDSHAKE_ERR_FATAL =       0xf,

    OI_HID_CONTROL_NOP =               0x0,
    OI_HID_CONTROL_HARD_RESET =        0x1,
    OI_HID_CONTROL_SOFT_RESET =        0x2,
    OI_HID_CONTROL_SUSPEND =           0x3,
    OI_HID_CONTROL_EXIT_SUSPEND =      0x4,
    OI_HID_CONTROL_VIRTUAL_CABLE_UNPLUG = 0x5,


    /* Used by GET_REPORT, SET_REPORT, DATA, and DATC: */
    OI_HID_REPORT_OTHER =              0,
    OI_HID_REPORT_INPUT =              1,
    OI_HID_REPORT_OUTPUT =             2,
    OI_HID_REPORT_FEATURE =            3,
    OI_HID_REPORT_TYPE_MASK =          (BIT0 | BIT1),
    OI_HID_REPORT_BUFFERSIZE_PRESENT = BIT3,
        
    /* Used by GET_PROTOCOL and SET_PROTOCOL */
    OI_HID_PROTOCOL_BOOT =             0,
    OI_HID_PROTOCOL_REPORT =           1
    
    /* GET_IDLE does not use the parameter field */
    /* SET_IDLE does not use the parameter field */
};

#define OI_HID_REPORT_MAX_LENGTH OI_UINT16_MAX
#define OI_HID_REPORT_ID_DEFAULT (-1)
#define OI_HID_REPORT_ID_UNDEFINED (-2)

#define OI_HID_BYTE_ORDER OI_LITTLE_ENDIAN_BYTE_ORDER

/** Boot protocol */

#define OI_HIDBOOT_KEYBOARD_REPORT_ID 1

#define OI_HIDBOOT_KEYBOARD_LED_NUM_LOCK       BIT0
#define OI_HIDBOOT_KEYBOARD_LED_CAPS_LOCK      BIT1
#define OI_HIDBOOT_KEYBOARD_LED_SCROLL_LOCK    BIT2
#define OI_HIDBOOT_KEYBOARD_LED_COMPOSE        BIT3
#define OI_HIDBOOT_KEYBOARD_LED_KANA           BIT4

#define OI_HIDBOOT_KEYBOARD_PACKET_LEN 9
#define OI_HIDBOOT_KEYBOARD_KEYCODE_COUNT 6
#define OI_HIDBOOT_KEYBOARD_LED_PACKET_LEN 

/** A subset of the Keyboard/Keypad HID Usage Page */
enum {
    OI_HIDBOOT_KEYBOARD_NULL =         0x00,
    OI_HIDBOOT_KEYBOARD_ERR_ROLLOVER = 0x01,
    OI_HIDBOOT_KEYBOARD_POST_FAIL =    0x02,
    OI_HIDBOOT_KEYBOARD_ERR_UNDEFINED = 0x03,
    OI_HIDBOOT_KEYBOARD_1 =            0x1e,
    OI_HIDBOOT_KEYBOARD_2 =            0x1f,
    OI_HIDBOOT_KEYBOARD_3 =            0x20,
    OI_HIDBOOT_KEYBOARD_4 =            0x21,
    OI_HIDBOOT_KEYBOARD_5 =            0x22,
    OI_HIDBOOT_KEYBOARD_6 =            0x23,
    OI_HIDBOOT_KEYBOARD_7 =            0x24,
    OI_HIDBOOT_KEYBOARD_8 =            0x25,
    OI_HIDBOOT_KEYBOARD_9 =            0x26,
    OI_HIDBOOT_KEYBOARD_0 =            0x27,
    OI_HIDBOOT_KEYBOARD_ENTER =        0x28
};

#define OI_HIDBOOT_MOUSE_REPORT_ID  2
#define OI_HIDBOOT_MOUSE_NO_BUTTONS 0
#define OI_HIDBOOT_MOUSE_BUTTON1    BIT0
#define OI_HIDBOOT_MOUSE_BUTTON2    BIT1
#define OI_HIDBOOT_MOUSE_BUTTON3    BIT2

#define OI_HIDBOOT_MOUSE_NULL_VALUE (-128)

#define OI_HIDBOOT_MOUSE_PACKET_LEN 4



/** HID Descriptor country codes (USB HID v1.11 section 6.2.1) */
enum {
    OI_HIDCountry_Not_Supported =      0,
    OI_HIDCountry_Arabic =             1,
    OI_HIDCountry_Belgian =            2,
    OI_HIDCountry_Canadian_Bilingual = 3,
    OI_HIDCountry_CanadianFrench =     4,
    OI_HIDCountry_Czech_Republic =     5,
    OI_HIDCountry_Danish =             6,
    OI_HIDCountry_Finnish =            7,
    OI_HIDCountry_French =             8,
    OI_HIDCountry_German =             9,
    OI_HIDCountry_Greek =              10,
    OI_HIDCountry_Hebrew =             11,
    OI_HIDCountry_Hungary =            12,
    OI_HIDCountry_International =      13,
    OI_HIDCountry_Italian =            14,
    OI_HIDCountry_Japan =              15,
    OI_HIDCountry_Korean =             16,
    OI_HIDCountry_Latin_American =     17,
    OI_HIDCountry_Netherlands =        18,
    OI_HIDCountry_Norwegian =          19,
    OI_HIDCountry_Persian =            20,
    OI_HIDCountry_Poland =             21,
    OI_HIDCountry_Portuguese =         22,
    OI_HIDCountry_Russia =             23,
    OI_HIDCountry_Slovakia =           24,
    OI_HIDCountry_Spanish =            25,
    OI_HIDCountry_Swedish =            26,
    OI_HIDCountry_Swiss_French =       27,
    OI_HIDCountry_Swiss_German =       28,
    OI_HIDCountry_Switzerland =        29,
    OI_HIDCountry_Taiwan =             30,
    OI_HIDCountry_Turkish_Q =          31,
    OI_HIDCountry_UK =                 32,
    OI_HIDCountry_US =                 33,
    OI_HIDCountry_Yugoslavia =         34,
    OI_HIDCountry_Turkish_F =          35
};

/** USB localization LANGID (USB 1.1 section 9.6.5) */
enum {
    OI_LANGID_Afrikaans =              0x0436,
    OI_LANGID_Albanian =               0x041c,
    OI_LANGID_Arabic_Saudi_Arabia =    0x0401,
    OI_LANGID_Arabic_Iraq =            0x0801,
    OI_LANGID_Arabic_Egypt =           0x0c01,
    OI_LANGID_Arabic_Libya =           0x1001,
    OI_LANGID_Arabic_Algeria =         0x1401,
    OI_LANGID_Arabic_Morocco =         0x1801,
    OI_LANGID_Arabic_Tunisia =         0x1c01,
    OI_LANGID_Arabic_Oman =            0x2001,
    OI_LANGID_Arabic_Yemen =           0x2401,
    OI_LANGID_Arabic_Syria =           0x2801,
    OI_LANGID_Arabic_Jordan =          0x2c01,
    OI_LANGID_Arabic_Lebanon =         0x3001,
    OI_LANGID_Arabic_Kuwait =          0x3401,
    OI_LANGID_Arabic_UAE =             0x3801,
    OI_LANGID_Arabic_Bahrain =         0x3c01,
    OI_LANGID_Arabic_Qatar =           0x4001,
    OI_LANGID_Armenian =               0x042b,
    OI_LANGID_Azeri_Latin =            0x042c,
    OI_LANGID_Azeri_Cyrillic =         0x082c,
    OI_LANGID_Basque =                 0x042d,
    OI_LANGID_Belarusian =             0x0423,
    OI_LANGID_Bulgarian =              0x0402,
    OI_LANGID_Catalan =                0x0403,
    OI_LANGID_Chinese_Taiwan =         0x0404,
    OI_LANGID_Chinese_PRC =            0x0804,
    OI_LANGID_Chinese_Hong_Kong =      0x0c04,
    OI_LANGID_Chinese_Singapore =      0x1004,
    OI_LANGID_Chinese_Macau =          0x1404,
    OI_LANGID_Croatian =               0x041a,
    OI_LANGID_Czech =                  0x0405,
    OI_LANGID_Danish =                 0x0406,
    OI_LANGID_Dutch_Standard =         0x0413,
    OI_LANGID_Dutch_Belgian =          0x0813,
    OI_LANGID_English_United_States =  0x0409,
    OI_LANGID_English_United_Kingdom = 0x0809,
    OI_LANGID_English_Australian =     0x0c09,
    OI_LANGID_English_Canadian =       0x1009,
    OI_LANGID_English_New_Zealand =    0x1409,
    OI_LANGID_English_Irish =          0x1809,
    OI_LANGID_English_South_Africa =   0x1c09,
    OI_LANGID_English_Jamaica =        0x2009,
    OI_LANGID_English_Caribbean =      0x2409,
    OI_LANGID_English_Belize =         0x2809,
    OI_LANGID_English_Trinidad =       0x2c09,
    OI_LANGID_English_Zimbabwe =       0x3009,
    OI_LANGID_English_Philippines =    0x3409,
    OI_LANGID_Estonian =               0x0425,
    OI_LANGID_Faeroese =               0x0438,
    OI_LANGID_Farsi =                  0x0429,
    OI_LANGID_Finnish =                0x040b,
    OI_LANGID_French_Standard =        0x040c,
    OI_LANGID_French_Belgian =         0x080c,
    OI_LANGID_French_Canadian =        0x0c0c,
    OI_LANGID_French_Swiss =           0x100c,
    OI_LANGID_French_Luxembourg =      0x140c,
    OI_LANGID_French_Monaco =          0x180c,
    OI_LANGID_Georgian =               0x0437,
    OI_LANGID_German_Standard =        0x0407,
    OI_LANGID_German_Swiss =           0x0807,
    OI_LANGID_German_Austrian =        0x0c07,
    OI_LANGID_German_Luxembourg =      0x1007,
    OI_LANGID_German_Liechtenstein =   0x1407,
    OI_LANGID_Greek =                  0x0408,
    OI_LANGID_Hebrew =                 0x040d,
    OI_LANGID_Hindi =                  0x0439,
    OI_LANGID_Hungarian =              0x040e,
    OI_LANGID_Icelandic =              0x040f,
    OI_LANGID_Indonesian =             0x0421,
    OI_LANGID_Italian_Standard =       0x0410,
    OI_LANGID_Italian_Swiss =          0x0810,
    OI_LANGID_Japanese =               0x0411,
    OI_LANGID_Kazakh =                 0x043f,
    OI_LANGID_Konkani =                0x0457,
    OI_LANGID_Korean =                 0x0412,
    OI_LANGID_Latvian =                0x0426,
    OI_LANGID_Lithuanian =             0x0427,
    OI_LANGID_Macedonian =             0x042f,
    OI_LANGID_Malay_Malaysia =         0x043e,
    OI_LANGID_Malay_Brunei_Darussalam = 0x083e,
    OI_LANGID_Marathi =                0x044e,
    OI_LANGID_Norwegian_Bokmal =       0x0414,
    OI_LANGID_Norwegian_Nynorsk =      0x0814,
    OI_LANGID_Polish =                 0x0415,
    OI_LANGID_Portuguese_Brazilian =   0x0416,
    OI_LANGID_Portuguese_Standard =    0x0816,
    OI_LANGID_Romanian =               0x0418,
    OI_LANGID_Russian =                0x0419,
    OI_LANGID_Sanskrit =               0x044f,
    OI_LANGID_Serbian_Latin =          0x081a,
    OI_LANGID_Serbian_Cyrillic =       0x0c1a,
    OI_LANGID_Slovak =                 0x041b,
    OI_LANGID_Slovenian =              0x0424,
    OI_LANGID_Spanish_Traditional_Sort = 0x040a,
    OI_LANGID_Spanish_Mexican =        0x080a,
    OI_LANGID_Spanish_Modern_Sort =    0x0c0a,
    OI_LANGID_Spanish_Guatemala =      0x100a,
    OI_LANGID_Spanish_Costa_Rica =     0x140a,
    OI_LANGID_Spanish_Panama =         0x180a,
    OI_LANGID_Spanish_Dominican_Republic = 0x1c0a,
    OI_LANGID_Spanish_Venezuela =      0x200a,
    OI_LANGID_Spanish_Colombia =       0x240a,
    OI_LANGID_Spanish_Peru =           0x280a,
    OI_LANGID_Spanish_Argentina =      0x2c0a,
    OI_LANGID_Spanish_Ecuador =        0x300a,
    OI_LANGID_Spanish_Chile =          0x340a,
    OI_LANGID_Spanish_Uruguay =        0x380a,
    OI_LANGID_Spanish_Paraguay =       0x3c0a,
    OI_LANGID_Spanish_Bolivia =        0x400a,
    OI_LANGID_Spanish_El_Salvador =    0x440a,
    OI_LANGID_Spanish_Honduras =       0x480a,
    OI_LANGID_Spanish_Nicaragua =      0x4c0a,
    OI_LANGID_Spanish_Puerto_Rico =    0x500a,
    OI_LANGID_Swahili =                0x0441,
    OI_LANGID_Swedish =                0x041d,
    OI_LANGID_Swedish_Finland =        0x081d,
    OI_LANGID_Tamil =                  0x0449,
    OI_LANGID_Tatar =                  0x0444,
    OI_LANGID_Thai =                   0x041e,
    OI_LANGID_Turkish =                0x041f,
    OI_LANGID_Ukrainian =              0x0422,
    OI_LANGID_Urdu =                   0x0420,
    OI_LANGID_Uzbek_Latin =            0x0443,
    OI_LANGID_Uzbek_Cyrillic =         0x0843,
    OI_LANGID_Vietnamese =             0x042a
};

#ifdef __cplusplus
}
#endif

/**@}*/

#endif /* _OI_HID_CONSTS_H */
