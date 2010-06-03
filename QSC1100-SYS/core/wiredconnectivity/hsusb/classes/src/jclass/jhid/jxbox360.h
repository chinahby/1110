/************************************************************************** 
*************************   Restricted access   *************************** 
*************************************************************************** 












This file must only be used for the development of the HSUSB 
driver for the AMSS / BREW SW baselines using the Jungo USB Stack.
This file must not be used in any way for the development of any
functionally equivalent USB driver not using the Jungo USB stack.

For any questions please contact: Sergio Kolor, Liron Manor,
Yoram Rimoni, Dedy Lansky.











************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */
#ifndef _HID_XBOX360_GAMEPAD_H
#define _HID_XBOX360_GAMEPAD_H

#define XBOX360_GAMEPAD_VID             0x045e
#define XBOX360_GAMEPAD_IF_CLASS        0xff 
#define XBOX360_GAMEPAD_IF_SUB_CLASS    0x5d
#define XBOX360_GAMEPAD_IF_PROT         1

static juint8_t xbox360report_d[] = {
    0x05, 0x01,        /* USAGE PAGE (Generic Desktop)          */
    0x09, 0x05,        /* USAGE (Gamepad)                       */
    0xa1, 0x01,        /* COLLECTION (Application)              */
    0x05, 0x01,        /*  USAGE PAGE (Generic Desktop)         */
    0x09, 0x3a,        /*  USAGE (Counted Buffer)               */
    0xa1, 0x02,        /*  COLLECTION (Logical)                 */
    0x85, 0x00,        /*   REPORT_ID (0)                       */
    /* Byte count */
    0x75, 0x08,        /*   REPORT SIZE (8)                     */
    0x95, 0x01,        /*   REPORT COUNT (1)                    */
    0x05, 0x01,        /*   USAGE PAGE (Generic Desktop)        */
    0x09, 0x3b,        /*   USAGE (Byte Count)                  */
    0x81, 0x01,        /*   INPUT (Constant)                    */
    /* D-Pad */
    0x05, 0x01,        /*   USAGE PAGE (Generic Desktop)        */
    0x09, 0x01,        /*   USAGE (Pointer)                     */
    0xa1, 0x00,        /*   COLLECTION (Physical)               */
    0x75, 0x01,        /*    REPORT SIZE (1)                    */
    0x15, 0x00,        /*    LOGICAL MINIMUM (0)                */
    0x25, 0x01,        /*    LOGICAL MAXIMUM (1)                */
    0x35, 0x00,        /*    PHYSICAL MINIMUM (0)               */
    0x45, 0x01,        /*    PHYSICAL MAXIMUM (1)               */
    0x95, 0x04,        /*    REPORT COUNT (4)                   */
    0x05, 0x01,        /*    USAGE PAGE (Generic Desktop)       */
    0x09, 0x90,        /*    USAGE (D-Pad Up)                   */
    0x09, 0x91,        /*    USAGE (D-Pad Down)                 */
    0x09, 0x93,        /*    USAGE (D-Pad Left)                 */
    0x09, 0x92,        /*    USAGE (D-Pad Right)                */
    0x81, 0x02,        /*    INPUT (Data, Variable, Absolute)   */
    0xc0,              /*   END COLLECTION                      */
    /* Buttons 5-11 */
    0x75, 0x01,        /*   REPORT SIZE (1)                     */
    0x15, 0x00,        /*   LOGICAL MINIMUM (0)                 */
    0x25, 0x01,        /*   LOGICAL MAXIMUM (1)                 */
    0x35, 0x00,        /*   PHYSICAL MINIMUM (0)                */
    0x45, 0x01,        /*   PHYSICAL MAXIMUM (1)                */
    0x95, 0x07,        /*   REPORT COUNT (7)                    */
    0x05, 0x09,        /*   USAGE PAGE (Button)                 */
    0x09, 0x08,        /*   USAGE (Button 8)                    */
    0x09, 0x07,        /*   USAGE (Button 7)                    */
    0x09, 0x09,        /*   USAGE (Button 9)                    */
    0x09, 0x0a,        /*   USAGE (Button 10)                   */
    0x09, 0x05,        /*   USAGE (Button 5)                    */
    0x09, 0x06,        /*   USAGE (Button 6)                    */
    0x09, 0x0b,        /*   USAGE (Button 11)                   */
    0x81, 0x02,        /*   INPUT (Data, Variable, Absolute)    */
    /* Unused */
    0x75, 0x01,        /*   REPORT SIZE (1)                     */
    0x95, 0x01,        /*   REPORT COUNT (1)                    */
    0x81, 0x01,        /*   INPUT (Constant)                    */
    /* Buttons 1-4 */
    0x75, 0x01,        /*   REPORT SIZE (1)                     */
    0x15, 0x00,        /*   LOGICAL MINIMUM (0)                 */
    0x25, 0x01,        /*   LOGICAL MAXIMUM (1)                 */
    0x35, 0x00,        /*   PHYSICAL MINIMUM (0)                */
    0x45, 0x01,        /*   PHYSICAL MAXIMUM (1)                */
    0x95, 0x04,        /*   REPORT COUNT (4)                    */
    0x05, 0x09,        /*   USAGE PAGE (Button)                 */
    0x19, 0x01,        /*   USAGE MINIMUM (Button 1)            */
    0x29, 0x04,        /*   USAGE MAXIMUM (Button 4)            */
    0x81, 0x02,        /*   INPUT (Data, Variable, Absolute)    */
    /* Triggers */
    0x75, 0x08,        /*   REPORT SIZE (8)                     */
    0x15, 0x00,        /*   LOGICAL MINIMUM (0)                 */
    0x26, 0xff, 0x00,  /*   LOGICAL MAXIMUM (255)               */
    0x35, 0x00,        /*   PHYSICAL MINIMUM (0)                */
    0x46, 0xff, 0x00,  /*   PHYSICAL MAXIMUM (255)              */
    0x95, 0x02,        /*   REPORT COUNT (2)                    */
    0x05, 0x01,        /*   USAGE PAGE (Generic Desktop)        */
    0x09, 0x32,        /*   USAGE (Z)                           */
    0x09, 0x35,        /*   USAGE (Rz)                          */
    0x81, 0x02,        /*   INPUT (Data, Variable, Absolute)    */
    /* Sticks */
    0x75, 0x10,        /*   REPORT SIZE (16)                    */
    0x16, 0x00, 0x80,  /*   LOGICAL MINIMUM (-32768)            */
    0x26, 0xff, 0x7f,  /*   LOGICAL MAXIMUM (32767)             */
    0x36, 0x00, 0x80,  /*   PHYSICAL MINIMUM (-32768)           */
    0x46, 0xff, 0x7f,  /*   PHYSICAL MAXIMUM (32767)            */
    0x95, 0x04,        /*   REPORT COUNT (4)                    */
    0x05, 0x01,        /*   USAGE PAGE (Generic Desktop)        */
    0x09, 0x30,        /*   USAGE (X)                           */
    0x09, 0x31,        /*   USAGE (Y)                           */
    0x09, 0x33,        /*   USAGE (Rx)                          */
    0x09, 0x34,        /*   USAGE (Ry)                          */
    0x81, 0x02,        /*   INPUT (Data, Variable, Absolute)    */
    /* Unused */
    0x75, 0x30,        /*   REPORT SIZE (48)                    */
    0x95, 0x01,        /*   REPORT COUNT (1)                    */
    0x81, 0x01,        /*   INPUT (Constant)                    */
    0xc0,              /*  END COLLECTION                       */
    /* Vibration report */
    0x05, 0x01,        /*  USAGE PAGE (Generic Desktop)         */
    0x09, 0x3a,        /*  USAGE (Counted Buffer)               */
    0xa1, 0x02,        /*  COLLECTION (Logical)                 */
    0x75, 0x08,        /*   REPORT_SIZE (8)                     */
    0x95, 0x01,        /*   REPORT_COUNT (1)                    */
    0x05, 0x01,        /*   USAGE_PAGE (Generic Desktop)        */
    0x09, 0x3b,        /*   USAGE (Byte Count)                  */
    0x91, 0x02,        /*   OUTPUT (Data,Var,Abs)               */
    0x75, 0x08,        /*   REPORT_SIZE (8)                     */
    0x95, 0x01,        /*   REPORT_COUNT (1)                    */
    0x91, 0x03,        /*   OUTPUT (Cnst,Var,Abs)               */
    0x75, 0x08,        /*   REPORT_SIZE (8)                     */
    0x15, 0x00,        /*   LOGICAL_MINIMUM (0)                 */
    0x26, 0xff, 0x00,  /*   LOGICAL_MAXIMUM (255)               */
    0x35, 0x00,        /*   PHYSICAL_MINIMUM (0)                */
    0x46, 0xff, 0x00,  /*   PHYSICAL_MAXIMUM (255)              */
    0x95, 0x02,        /*   REPORT_COUNT (2)                    */
    0x06, 0x00, 0x01,  /*   USAGE_PAGE (Motor)256               */
    0x09, 0x01,        /*   USAGE (Big motor)                   */
    0x09, 0x02,        /*   USAGE (Little motor)                */
    0x91, 0x02,        /*   OUTPUT (Data,Var,Abs)               */
    0x75, 0x18,        /*   REPORT_SIZE (24)                    */
    0x95, 0x01,        /*   REPORT_COUNT (1)                    */
    0x91, 0x01,        /*   OUTPUT (Cnst,Ary,Abs)               */
    0xc0,              /*  END COLLECTION                       */
    /* Leds */
    0x05, 0x01,        /*  USAGE PAGE (Generic Desktop)         */
    0x09, 0x3a,        /*  USAGE (Counted Buffer)               */
    0xa1, 0x02,        /*  COLLECTION (Logical)                 */
    0x75, 0x08,        /*   REPORT_SIZE (8)                     */
    0x95, 0x01,        /*   REPORT_COUNT (1)                    */
    0x85, 0x01,        /*   REPORT_ID (1)                       */
    0x05, 0x01,        /*   USAGE_PAGE (Generic Desktop)        */
    0x09, 0x3b,        /*   USAGE (Byte Count)                  */
    0x91, 0x02,        /*   OUTPUT (Data,Var,Abs)               */
    0x75, 0x08,        /*   REPORT_SIZE (8)                     */
    0x95, 0x01,        /*   REPORT_COUNT (1)                    */
    0x15, 0x00,        /*   LOGICAL_MINIMUM (0)                 */
    0x25, 0x0d,        /*   LOGICAL_MAXIMUM (13)                */
    0x35, 0x00,        /*   PHYSICAL_MINIMUM (0)                */
    0x45, 0x0d,        /*   PHYSICAL_MAXIMUM (13)               */
    0x05, 0x08,        /*   USAGE_PAGE (LEDs)                   */
    0x09, 0x4b,        /*   USAGE (Generic Indicator)           */
    0x91, 0x02,        /*   OUTPUT (Data,Var,Abs)               */
    0xc0,              /*  END COLLECTION                       */
    0xc0,              /* END COLLECTION                        */
};
#endif

