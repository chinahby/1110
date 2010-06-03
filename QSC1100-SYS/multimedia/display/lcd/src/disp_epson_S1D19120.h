#ifndef DISP_EPSON_S1D19120_H
#define DISP_EPSON_S1D19120_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   L C D    D I S P L A Y    D R I V E R
                           H E A D E R   F I L E

GENERAL DESCRIPTION
  This is the header file for the Epson S1D19120 QVGA LCD Display panel driver.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/display/mdp1/lcd_qsc1100/main/latest/src/disp_epson_S1D19120.h#1 $ $DateTime: 2008/05/06 14:29:11 $ $Author: karenp $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/07/08   dw      Initial version

============================================================================ */

/*============================================================================

              CONSTANT AND MACRO DEFINITIONS FOR TMD DISPLAY

============================================================================*/

/*=====================================================================
             DEFINITIONS AND DECLARATIONS FOR MODULE
======================================================================*/
/*=====================================================================
      Typedefs
======================================================================*/

/*=====================================================================
      Constants and Macros 
======================================================================*/
/*                                                              
 * Common Command Codes
 */
#define EPSON_S1D19XXX_NOP_C                           0x00
#define EPSON_S1D19XXX_DISPLAY_ON_C                    0xAF
#define EPSON_S1D19XXX_DISPLAY_OFF_C                   0xAE
#define EPSON_S1D19XXX_SET_DISPLAY_C                   0xCA
#define EPSON_S1D19XXX_SET_DISPLAY_TIMING_C            0xA1
#define EPSON_S1D19XXX_SET_DATA_C                      0xBC
#define EPSON_S1D19XXX_SET_START_ADDRESS_C             0x15
#define EPSON_S1D19XXX_SET_END_ADDRESS_C               0x75
#define EPSON_S1D19XXX_RAM_WRITE_C                     0x5C
#define EPSON_S1D19XXX_RAM_READ_C                      0x5D
#define EPSON_S1D19XXX_SET_AREA_SCROLLING_C            0xAA
#define EPSON_S1D19XXX_SET_DISPLAY_START_LINE_C        0xAB
#define EPSON_S1D19XXX_PARTIAL_DISPLAY_IN_C            0xA8
#define EPSON_S1D19XXX_PARTIAL_DISPLAY_OUT_C           0xA9
#define EPSON_S1D19XXX_SET_DISPLAY_DATA_INTERFACE_C    0x31
#define EPSON_S1D19XXX_SET_DISPLAY_COLOR_MODE_C        0x8B
#define EPSON_S1D19XXX_GATE_LINE_SCAN_MODE_C           0x6F
#define EPSON_S1D19XXX_SET_AC_OPERATION_DRIVE_C        0x8C
#define EPSON_S1D19XXX_SET_ELECTRONIC_CONTROL_C        0x20
#define EPSON_S1D19XXX_SET_CORRECTION_CHARS_C          0x22
#define EPSON_S1D19XXX_SET_POWER_CONTROL_C             0x21
#define EPSON_S1D19XXX_SET_PARTIAL_POWER_CONTROL_C     0x23
#define EPSON_S1D19XXX_SLEEP_IN_C                      0x95
#define EPSON_S1D19XXX_SLEEP_OUT_C                     0x94
#define EPSON_S1D19XXX_VOSC_OFF_C                      0x97
#define EPSON_S1D19XXX_VOSC_ON_C                       0x96
#define EPSON_S1D19XXX_STOP_OSCILLATION_C              0x93
#define EPSON_S1D19XXX_START_OSCILLATION_C             0x92
#define EPSON_S1D19XXX_SOFT_RESET_C                    0x99
#define EPSON_S1D19XXX_STATUS_READ_C                   0xE8
#define EPSON_S1D19XXX_REVISION_READ_C                 0xE9
#define EPSON_S1D19XXX_TEST_C                          0xFF

/*                                                              
 * S1D19105 LCD Controller Commands
 */
#define EPSON_S1D19105_NOP_C                           (EPSON_S1D19XXX_NOP_C)
#define EPSON_S1D19105_DISPLAY_ON_C                    (EPSON_S1D19XXX_DISPLAY_ON_C)
#define EPSON_S1D19105_DISPLAY_OFF_C                   (EPSON_S1D19XXX_DISPLAY_OFF_C)
#define EPSON_S1D19105_SET_DISPLAY_C                   (EPSON_S1D19XXX_SET_DISPLAY_C)
#define EPSON_S1D19105_SET_DISPLAY_TIMING_C            (EPSON_S1D19XXX_SET_DISPLAY_TIMING_C)
#define EPSON_S1D19105_SET_DATA_C                      (EPSON_S1D19XXX_SET_DATA_C)
#define EPSON_S1D19105_SET_START_ADDRESS_C             (EPSON_S1D19XXX_SET_START_ADDRESS_C)
#define EPSON_S1D19105_SET_END_ADDRESS_C               (EPSON_S1D19XXX_SET_END_ADDRESS_C)
#define EPSON_S1D19105_RAM_WRITE_C                     (EPSON_S1D19XXX_RAM_WRITE_C)
#define EPSON_S1D19105_RAM_READ_C                      (EPSON_S1D19XXX_RAM_READ_C)
#define EPSON_S1D19105_SET_AREA_SCROLLING_C            (EPSON_S1D19XXX_SET_AREA_SCROLLING_C)
#define EPSON_S1D19105_SET_DISPLAY_START_LINE_C        (EPSON_S1D19XXX_SET_DISPLAY_START_LINE_C)
#define EPSON_S1D19105_PARTIAL_DISPLAY_IN_C            (EPSON_S1D19XXX_PARTIAL_DISPLAY_IN_C)
#define EPSON_S1D19105_PARTIAL_DISPLAY_OUT_C           (EPSON_S1D19XXX_PARTIAL_DISPLAY_OUT_C)
#define EPSON_S1D19105_SET_DISPLAY_DATA_INTERFACE_C    (EPSON_S1D19XXX_SET_DISPLAY_DATA_INTERFACE_C)
#define EPSON_S1D19105_SET_DISPLAY_COLOR_MODE_C        (EPSON_S1D19XXX_SET_DISPLAY_COLOR_MODE_C)
#define EPSON_S1D19105_GATE_LINE_SCAN_MODE_C           (EPSON_S1D19XXX_GATE_LINE_SCAN_MODE_C)
#define EPSON_S1D19105_SET_AC_OPERATION_DRIVE_C        (EPSON_S1D19XXX_SET_AC_OPERATION_DRIVE_C)
#define EPSON_S1D19105_SET_ELECTRONIC_CONTROL_C        (EPSON_S1D19XXX_SET_ELECTRONIC_CONTROL_C)
#define EPSON_S1D19105_SET_CORRECTION_CHARS_C          (EPSON_S1D19XXX_SET_CORRECTION_CHARS_C)
#define EPSON_S1D19105_SET_POWER_CONTROL_C             (EPSON_S1D19XXX_SET_POWER_CONTROL_C)
#define EPSON_S1D19105_SET_PARTIAL_POWER_CONTROL_C     (EPSON_S1D19XXX_SET_PARTIAL_POWER_CONTROL_C)
#define EPSON_S1D19105_SLEEP_IN_C                      (EPSON_S1D19XXX_SLEEP_IN_C)
#define EPSON_S1D19105_SLEEP_OUT_C                     (EPSON_S1D19XXX_SLEEP_OUT_C)
#define EPSON_S1D19105_VOSC_OFF_C                      (EPSON_S1D19XXX_VOSC_OFF_C)
#define EPSON_S1D19105_VOSC_ON_C                       (EPSON_S1D19XXX_VOSC_ON_C)
#define EPSON_S1D19105_STOP_OSCILLATION_C              (EPSON_S1D19XXX_STOP_OSCILLATION_C)
#define EPSON_S1D19105_START_OSCILLATION_C             (EPSON_S1D19XXX_START_OSCILLATION_C)
#define EPSON_S1D19105_SOFT_RESET_C                    (EPSON_S1D19XXX_SOFT_RESET_C)
#define EPSON_S1D19105_STATUS_READ_C                   (EPSON_S1D19XXX_STATUS_READ_C)
#define EPSON_S1D19105_REVISION_READ_C                 (EPSON_S1D19XXX_REVISION_READ_C)
#define EPSON_S1D19105_TEST_C                          (EPSON_S1D19XXX_TEST_C)
#define EPSON_S1D19105_READ_MODIFY_WRITE_C             0xE0

/* Maximum number of parameters for any S1D19105 cmd (not including cmd byte) */
#define EPSON_S1D19105_MAX_CMD_PARAMS                  13

/*                                                              
 * S1D19105 LCD Controller Commands
 */
#define EPSON_S1D19120_DISPLAY_ON_C                    (EPSON_S1D19XXX_DISPLAY_ON_C)
#define EPSON_S1D19120_DISPLAY_OFF_C                   (EPSON_S1D19XXX_DISPLAY_OFF_C)
#define EPSON_S1D19120_SET_DISPLAY_C                   (EPSON_S1D19XXX_SET_DISPLAY_C)
#define EPSON_S1D19120_SET_DISPLAY_TIMING_C            (EPSON_S1D19XXX_SET_DISPLAY_TIMING_C)
#define EPSON_S1D19120_SET_DATA_C                      (EPSON_S1D19XXX_SET_DATA_C)
#define EPSON_S1D19120_SET_START_ADDRESS_C             (EPSON_S1D19XXX_SET_START_ADDRESS_C)
#define EPSON_S1D19120_SET_END_ADDRESS_C               (EPSON_S1D19XXX_SET_END_ADDRESS_C)
#define EPSON_S1D19120_RAM_WRITE_C                     (EPSON_S1D19XXX_RAM_WRITE_C)
#define EPSON_S1D19120_RAM_READ_C                      (EPSON_S1D19XXX_RAM_READ_C)
#define EPSON_S1D19120_SET_AREA_SCROLLING_C            (EPSON_S1D19XXX_SET_AREA_SCROLLING_C)
#define EPSON_S1D19120_SET_DISPLAY_START_LINE_C        (EPSON_S1D19XXX_SET_DISPLAY_START_LINE_C)
#define EPSON_S1D19120_PARTIAL_DISPLAY_IN_C            (EPSON_S1D19XXX_PARTIAL_DISPLAY_IN_C)
#define EPSON_S1D19120_PARTIAL_DISPLAY_OUT_C           (EPSON_S1D19XXX_PARTIAL_DISPLAY_OUT_C)
#define EPSON_S1D19120_SET_DISPLAY_DATA_INTERFACE_C    (EPSON_S1D19XXX_SET_DISPLAY_DATA_INTERFACE_C)
#define EPSON_S1D19120_SET_DISPLAY_COLOR_MODE_C        (EPSON_S1D19XXX_SET_DISPLAY_COLOR_MODE_C)
#define EPSON_S1D19120_SELECT_MTP_ROM_MODE_C           0x65
#define EPSON_S1D19120_MTP_ROM_MODE_IN                 0x67
#define EPSON_S1D19120_MTP_ROM_MODE_OUT                0x68
#define EPSON_S1D19120_MTP_ROM_OPERATION_IN            0x69
#define EPSON_S1D19120_MTP_ROM_OPERATION_OUT           0x70
#define EPSON_S1D19120_GATE_LINE_SCAN_MODE_C           (EPSON_S1D19XXX_GATE_LINE_SCAN_MODE_C)
#define EPSON_S1D19120_SET_AC_OPERATION_DRIVE_C        (EPSON_S1D19XXX_SET_AC_OPERATION_DRIVE_C)
#define EPSON_S1D19120_SET_ELECTRONIC_CONTROL_C        (EPSON_S1D19XXX_SET_ELECTRONIC_CONTROL_C)
#define EPSON_S1D19120_SET_POS_GAMMA_CORRECTN_CHARS_C  (EPSON_S1D19XXX_SET_CORRECTION_CHARS_C)
#define EPSON_S1D19120_SET_NEG_GAMMA_CORRECTN_CHARS_C  0x25
#define EPSON_S1D19120_SET_POWER_CONTROL_C             (EPSON_S1D19XXX_SET_POWER_CONTROL_C)
#define EPSON_S1D19120_SET_PARTIAL_POWER_CONTROL_C     (EPSON_S1D19XXX_SET_PARTIAL_POWER_CONTROL_C)
#define EPSON_S1D19120_SET_8COLOR_CONTROL_C            0x24
#define EPSON_S1D19120_SLEEP_IN_C                      (EPSON_S1D19XXX_SLEEP_IN_C)
#define EPSON_S1D19120_SLEEP_OUT_C                     (EPSON_S1D19XXX_SLEEP_OUT_C)
#define EPSON_S1D19120_VDD_OFF_C                       (EPSON_S1D19XXX_VOSC_OFF_C)
#define EPSON_S1D19120_VDD_ON_C                        (EPSON_S1D19XXX_VOSC_ON_C)
#define EPSON_S1D19120_STOP_OSCILLATION_C              (EPSON_S1D19XXX_STOP_OSCILLATION_C)
#define EPSON_S1D19120_START_OSCILLATION_C             (EPSON_S1D19XXX_START_OSCILLATION_C)
#define EPSON_S1D19120_TEST_SOURCE_C                   0xFD
#define EPSON_S1D19120_TEST_FUSE_C                     0xFE
#define EPSON_S1D19120_TEST_C                          (EPSON_S1D19XXX_TEST_C)
#define EPSON_S1D19120_NOP_C                           (EPSON_S1D19XXX_NOP_C)
#define EPSON_S1D19120_STATUS_READ_C                   (EPSON_S1D19XXX_STATUS_READ_C)
#define EPSON_S1D19120_REVISION_READ_C                 (EPSON_S1D19XXX_REVISION_READ_C)
#define EPSON_S1D19120_SOFT_RESET_C                    (EPSON_S1D19XXX_SOFT_RESET_C)

/* Maximum number of parameters for any S1D19120 cmd (not including cmd byte) */
#define EPSON_S1D19120_MAX_CMD_PARAMS                  14

/*                                                              
 * The following macro performs command reformatting appropriate for the 
 * Epson S1D19XXX LCD controller's 18-bit parallel bus mode.
 *
 * 8-bit raw command and parameter values must be mapped to 18 bits
 * as follows:
 *                                           ___________________________
 *                             RAW COMMAND  |     |         |     |     |
 *                                  or      |  7  |   ...   |  1  |  0  |
 *                              PARAMETER   |_____|_________|_____|_____|
 *                                           |_________________________|
 *                       ____________________________/ 
 *                     /       left-shift 10 bits
 *    ________________/________ 
 *   |                         |
 *   ___________________________________________________________________
 *  |     |     |         |     |/////|/////|/////|/////////|/////|/////|
 *  | 17  | 16  |   ...   | 10  |/ 9 /|/ 8 /|/ 7 /|// ... //|/ 1 /|/ 0 /|
 *  |_____|_____|_________|_____|/////|/////|/////|/////////|/////|/////|
 *
 *   |_________________________________________________________________|
 *                                    |
 *                                 18 bits
 */
#define DISP_EPSON_CMD18(cmdRaw)       ((uint32)(cmdRaw) << 10)

/* Parameter formatting is the same as for commands */
#define DISP_EPSON_PARAM18(paramRaw)   (DISP_EPSON_CMD18(paramRaw))

/*                                                              
 * The following macro performs command reformatting appropriate for the 
 * Epson S1D19XXX LCD controller's 16-bit parallel bus mode.
 *
 * 8-bit raw command and parameter values must be mapped to 18 bits
 * as follows:
 *                                           ___________________________
 *                             RAW COMMAND  |     |         |     |     |
 *                                  or      |  7  |   ...   |  1  |  0  |
 *                              PARAMETER   |_____|_________|_____|_____|
 *                                           |_________________________|
 *                       ____________________________/ 
 *                     /       left-shift 8 bits
 *    ________________/________ 
 *   |                         |
 *   _________________________________________________________disabled__
 *  |     |     |         |     |/////|/////////|/////|/////|     |     |
 *  | 17  | 16  |   ...   | 10  |/ 9 /|// ... //|/ 3 /|/ 2 /|  1  |  0  |
 *  |_____|_____|_________|_____|/////|/////////|/////|/////|_____|_____|
 *
 *   |_____________________________________________________|
 *                              |
 *                           16 bits
 */
#define DISP_EPSON_CMD16(cmdRaw)         ((uint32)(cmdRaw) << 8)

/* Parameter formatting is the same as for commands */
#define DISP_EPSON_PARAM16(paramRaw)     (DISP_EPSON_CMD16(paramRaw))


/*                                                              
 * The following macro performs command reformatting appropriate for the 
 * Epson S1D19XXX LCD controller's 9-bit parallel bus mode. Bus routing
 * from D8:0 to D17:9 is assumed to be handled transparently by hardware.
 *
 * 8-bit raw command and parameter values must be mapped to 9 bits
 * as follows:
 *                                           ___________________________
 *                             RAW COMMAND  |     |         |     |     |
 *                                  or      |  7  |   ...   |  1  |  0  |
 *                              PARAMETER   |_____|_________|_____|_____|
 *                                           |_________________________|
 *                       ____________________________/ 
 *                     /       left-shift 1 bit
 *    ________________/________ 
 *   |                         |
 *   ___________________________________disabled___disabled___disabled__
 *  |     |     |         |     |/////|         |     |     |     |     |
 *  | 17  | 16  |   ...   | 10  |/ 9 /|   ...   |  3  |  2  |  1  |  0  |
 *  |_____|_____|_________|_____|/////|_________|_____|_____|_____|_____|
 *
 *   |________________________________|
 *                  |
 *               9 bits
 */
#define DISP_EPSON_CMD9(cmdRaw)         ((uint32)(cmdRaw) << 1)

/* Parameter formatting is the same as for commands */
#define DISP_EPSON_PARAM9(paramRaw)     (DISP_EPSON_CMD9(paramRaw))

#define DISP_EPSON_PIXEL9_1ST_WRITE(pixelRaw)  (((pixelRaw) >> 9) & 0x000001FF)
#define DISP_EPSON_PIXEL9_2ND_WRITE(pixelRaw)  ((pixelRaw) & 0x000001FF)

/* Primary display  */
#define LCD_CMD_PRIMARY        0x30000000  // cmd register for primary LCD port
#define LCD_DATA_PRIMARY       0x30100000  // data register for primary LCD port
#define LCD_CMDPARAM_PRIMARY   (LCD_DATA_PRIMARY)  // Cmd param reg same as data

#define LCD_CFG0_PRIMARY       (LCD_CFG0)
#define LCD_CFG1_PRIMARY       (LCD_CFG1)

#define LCD_CFG0_DEFAULT_PRIMARY        (0x00330507)
#define LCD_CFG1_DEFAULT_PRIMARY        (0xF9110000)

// The supported EBI2 bus sizes
typedef enum {
   EBI2_WIDTH_8,                       // 8 bits
   EBI2_WIDTH_16,
   EBI2_WIDTH_18,
   EBI2_WIDTH_2_x_12,                  // 24 bits in 2 12 bit writes.
   EBI2_WIDTH_24,
   EBI2_WIDTH_2_PLUS_16,               // 18 bits in 2 writes (2 bits then 16)
   EBI2_WIDTH_16_PLUS_2,               // 18 bits in 2 writes (16 bits then 2)
   EBI2_WIDTH_2_x_9,                   // 18 bits in 2 writes (9 bits then 9)
   EBI2_WIDTH_INVALID
} lcd_ebi2_width_type;

/* Maximum number of panels mounted on display card */
#define PEGS_EPSON_MAX_DISPLAYS   2
#define LCD_PRIMARY 0

typedef enum
{
   pixel_format_rgb565,
   pixel_format_rgb666,
   pixel_format_rgb888,
   pixel_format_rgb666_unpacked32, // Least-sig 18 bits of uint32 stores one pixel
   pixel_format_count
} pixel_format_type;

/* What the Verde FPGA returns as a Verde-Epson-Optrex card config ID */
#define PEGS_EPSON_EBI2_ID  0xDC01

#define LCD_CMD_WH    EBI2_LCD_BASE           // command register for primary LCD port
#define LCD_DATA_WH   EBI2_LCD_BASE+0x100000  // data register for primary LCD port

#endif /* DISP_EPSON_S1D19120_H */

