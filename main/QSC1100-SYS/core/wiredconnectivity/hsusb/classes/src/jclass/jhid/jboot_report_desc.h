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

#ifndef _HID_BOOT_PROT_H
#define _HID_BOOT_PROT_H

static juint8_t boot_kbd_report_d[]={
/* Usage Page (Generic Desktop)             */  0x05, 0x01,
/* Usage (Keyboard)                         */  0x09, 0x06,
/* Collection (Application)                 */  0xA1, 0x01,
/*      Usage page (Key Codes)              */  0x05, 0x07,
/*      Usage minimum (224)                 */  0x19, 0xE0,
/*      Usage maximum (231)                 */  0x29, 0xE7,
/*      Logical minimum (0)                 */  0x15, 0x00,
/*      Logical maximum (1)                 */  0x25, 0x01,
/*      Report size (1)                     */  0x75, 0x01,
/*      Report count (8)                    */  0x95, 0x08,
/*      Input (data, variable, absolute)    */  0x81, 0x02,
/*      Report count (1)                    */  0x95, 0x01,
/*      Report size (8)                     */  0x75, 0x08,
/*      Input (constant)                    */  0x81, 0x01,
/*      Report count (5)                    */  0x95, 0x05,
/*      Report size (1)                     */  0x75, 0x01,
/*      Usage page (page# for LEDs)         */  0x05, 0x08,
/*      Usage minimum (1)                   */  0x19, 0x01,
/*      Usage maxmimum (5)                  */  0x29, 0x05,
/*      Output (data, variable, absolute)   */  0x91, 0x02,
/*      Report count (1)                    */  0x95, 0x01,
/*      Report size (3)                     */  0x75, 0x03,
/*      Output (constant)                   */  0x91, 0x01,
/*      Report count (6)                    */  0x95, 0x06,
/*      Report size (8)                     */  0x75, 0x08,
/*      Logical minimum (0)                 */  0x15, 0x00,
/*      Logical maximum (101)               */  0x25, 0x65,
/*      Usage page (key codes)              */  0x05, 0x07,
/*      Usage minimum (0)                   */  0x19, 0x00,
/*      Usage maximum (101)                 */  0x29, 0x65,
/*      Input (data, array)                 */  0x81, 0x00,
/* End Collection                           */  0xC0
};


static juint8_t boot_mouse_report_d[]={
/* Usage Page (Generic Desktop)             */ 0x05, 0x01,
/* Usage (Mouse)                            */ 0x09, 0x02,
/* Collection (Application)                 */ 0xa1, 0x01,
/*     Usage (Pointer)                      */ 0x09, 0x01,
/*     Collection (Physical)                */ 0xa1, 0x00,
/*         Report Count (3)                 */ 0x95, 0x03,
/*         Report Size (1)                  */ 0x75, 0x01,
/*         Usage Page (Buttons)             */ 0x05, 0x09,
/*         Usage Minimum (1)                */ 0x19, 0x01,
/*         Usage Maximum (3)                */ 0x29, 0x03,
/*         Logical Minimum (0)              */ 0x15, 0x00,
/*         Logical Maximum (1)              */ 0x25, 0x01,
/*         Input (Data, Variable, Absolute) */ 0x81, 0x02,
/*         Report Count (1)                 */ 0x95, 0x01,
/*         Report Size (5)                  */ 0x75, 0x05,
/*         Input (Constant)                 */ 0x81, 0x01,
/*         Report Size (8)                  */ 0x75, 0x08,
/*         Report Count (2)                 */ 0x95, 0x02,
/*         Usage Page (Generic Desktop)     */ 0x05, 0x01,
/*         Usage (X)                        */ 0x09, 0x30,
/*         Usage (Y)                        */ 0x09, 0x31,
/*         Logical Minimum (-127)           */ 0x15, 0x81,
/*         Logical Maximum (127)            */ 0x25, 0x7f,
/*         Input (Data, Variable, Relative) */ 0x81, 0x06,
/*     End Collection                       */ 0xc0,
/* End Collection                           */ 0xc0
};
#endif
