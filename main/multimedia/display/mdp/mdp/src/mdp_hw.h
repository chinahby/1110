#ifndef MDP_HW_H
#define MDP_HW_H
/*============================================================================

                   MDP Device Driver Header File

  DESCRIPTION
  This file contains the definitions needed for the MDP device driver.

  Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/display/mdp1/mdp/main/latest/src/mdp_hw.h#3 $ $DateTime: 2009/03/17 08:26:09 $ $Author: gauravs $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 03/17/09  gs     Adding support for 24BPP output support
 06/09/08  rk     Included mdp_vsync_config.h
 09/28/07  dw     Remove compiler errors when TMD QVGA features are disabled.
 09/20/07  dw     Enable proper vsync parameters for SC2X QVGA support
 09/07/07  dw     Added support for TMD22 QVGA panel
 06/22/07  dw     Use combined feature name to identify all SC2X flavors
 05/16/07  dw     Include specific LCD info for SC2X Epson Optrex QcifP panel. 
 04/06/07  dw     Add support for Epson Optrex QcifP EBI2 display panel. 
 02/23/07  cl     Fixed lint warnings for unparenthesized parameters 
 04/06/06  dw     Enable dithering operation
 10/21/05  cjr    Added FEATURE_MDP_01_01 support
 04/12/05  cjr    fixed AHB bus clock speed
 03/31/05  cjr    L4 feature support 
 06/07/04  ap     Use defined port address defined in msm hw.
 05/28/04  cjr    Initial code
============================================================================*/

//============================================================================
//                        INCLUDE FILES
//============================================================================
#include "msm.h"
#include "mdp_vsync_config.h"

#define TCXO_FREQ   19.2e+006     // TCXO - 19.2 MHz
#define FREQ_73p6M    73.6e+006     // Micro_Clk/2 - 73.4 MHz

/////////////////////////////////////////////////////////////////////
//Vsync Config
/////////////////////////////////////////////////////////////////////
#define MDP_SW_VSYNC              // SW will reset MDP Vsync counter
//or #define MDP_HW_VSYNC if you have HW VSYNC line
//or #define MDP_SYS_VSYNC_OFF to disable VSYNC permanently 

#ifdef FEATURE_MDDI
#error code not present
#endif

/* SC2X QCIF+ LCD refresh rate and LCD lines differ from previous MDP 1.x 
   targets. It is better to retrieve this info dynamically but such support
   is not available. Assume this LCD info is static for now */



/////////////////////////////////////////////////////////////////////
// End of Vsync Config
/////////////////////////////////////////////////////////////////////

// Input Formats
#define RGB565      8         // aliased with LCD_16BPP
#define XRGB888     32
#define RGB888      33

// Output Formats/Modes
#define LCD_18BPP           0     // 18BPP 666 (parallel)
#define LCD_18BPP_99_I      1
#define LCD_18BPP_99_II     2
#define LCD_18BPP_666_I     3
#define LCD_18BPP_666_II    4
#define LCD_18BPP_666_III   5     // Supported by LCD on Toucan board
#define LCD_18BPP_666_IV    6
#define MDDI_18BPP          7
#define LCD_16BPP           8     // Supported by LCD on Toucan board - 16BPP 565 (parallel)
#define LCD_16BPP_88_I      9
#define LCD_16BPP_88_II     10
#define MDDI_16BPP          11
#define LCD_12BPP_444_I     12
#define LCD_12BPP_444_II    13
#define LCD_12BPP_444_III   14
#define LCD_12BPP_444_IV    15      // Supported by LCD on Toucan board
#define MEM_18BPP_XRGB      16      // Output to memory
#define MEM_16BPP_P565      17      // Output to memory
#define MISR_ONLY           18      // Output only to MISR

#define LCD_18BPP_ROUND_16BPP 25      // Not really a MDP output format - For internal use in VI code

/* TMD20 and TMD22 QVGA LCD panel port definitions */
#ifdef QVGA_CMD_WH
#define DISP_CMD_PORT2    QVGA_CMD_WH
#define DISP_DATA_PORT2   QVGA_DATA_WH
#else
#define DISP_CMD_PORT2    0
#define DISP_DATA_PORT2   0
#endif

/* QCIF Epson LCD panel definitions */
#ifdef QCIF_CMD_WH
#define DISP_CMD_PORT1    QCIF_CMD_WH
#define DISP_DATA_PORT1   QCIF_DATA_WH
#else
#define DISP_CMD_PORT1    0
#define DISP_DATA_PORT1   0
#endif

#ifdef FEATURE_L4
#error code not present
#endif /* FEATURE_L4 */

#ifdef FEATURE_MEMORY_VIRTUAL
#define DISP_CMD_PORT3    EBI2_LCD_BASE_PHYS
#define DISP_DATA_PORT3   (EBI2_LCD_BASE_PHYS+0x100000)
#else
#define DISP_CMD_PORT3    EBI2_LCD_BASE
#define DISP_DATA_PORT3   (EBI2_LCD_BASE+0x100000)
#endif /* FEATURE_MEMORY_VIRTUAL */ 

#define DISP_CMD_SD_PSET  0x75
#define DISP_CMD_SD_CSET  0x15
#define DISP_CMD_RAMWR    0x5c

/* TMD specific LCD register values - these definitions originate from 
** drivers/lcd and are repeated here so that this file does not include 
** LCD specific headers. The correct method is put this info into a 
** mdp_lcd.h that contains all LCD info. */

#if defined FEATURE_TMD_22_QVGA

#define DISP_QVGA_18BPP(x) ((((x) & 0xFF00) << 2) | (( (x) & 0x00FF) << 1))
#define DISP_QVGA_HORZ_RAM_ADDR_POS_1_ADDR 0x102 // Register to set col start
#define DISP_QVGA_HORZ_RAM_ADDR_POS_2_ADDR 0x106 // Register to set col end
#define DISP_QVGA_VERT_RAM_ADDR_POS_1_ADDR 0x104 // Register to set row start
#define DISP_QVGA_VERT_RAM_ADDR_POS_2_ADDR 0x108 // Register to set row end
#define DISP_QVGA_RAM_ADDR_SET_1_ADDR      0x118 // Start address - col
#define DISP_QVGA_RAM_ADDR_SET_2_ADDR      0x11A // Start address - row
#define DISP_QVGA_CMD_RAMWR                0x202 // RAM Data Write

#else /* defined FEATURE_TMD_20_QVGA */

#ifdef TMD20QVGA_LCD_18BPP
#define DISP_QVGA_18BPP(x) ((((x)<<2) & 0x3FC00) | (((x)<<1) & 0x1FE))
#else
#define DISP_QVGA_18BPP(x) ((uint16)(x))
#endif /* TMD20QVGA_LCD_18BPP */
#define DISP_QVGA_ROW_COL_TO_ADDR(row, col) (row * 0x100 + col)
#define DISP_QVGA_HORZ_RAM_ADDR_POS_1_ADDR 0x406 //Horizontal RAM Address Position (1) - col start
#define DISP_QVGA_HORZ_RAM_ADDR_POS_2_ADDR 0x407 //Horizontal RAM Address Position (2) - col end
#define DISP_QVGA_VERT_RAM_ADDR_POS_1_ADDR 0x408 //Vertical RAM Address Position (1) - row start
#define DISP_QVGA_VERT_RAM_ADDR_POS_2_ADDR 0x409 //Vertical RAM Address Position (2) - row end
#define DISP_QVGA_RAM_ADDR_SET_1_ADDR      0x200 //RAM Address Set (1)
#define DISP_QVGA_RAM_ADDR_SET_2_ADDR      0x201 //RAM Address Set (2)
#define DISP_QVGA_CMD_RAMWR                0x202 //RAM Data Write

#endif /* FEATURE_TMD_22_QVGA */

// MDDI config

#if defined(T_MSM6290) && defined(FEATURE_MDP_24BPP_OUTPUT)
#define MDDI_VDO_PACKET_DESC  0x5888
#else
#define MDDI_VDO_PACKET_DESC  0x5666
#endif

#define MDDI_VDO_PACKET_PRIM  0xC3
#define MDDI_VDO_PACKET_SECD  0xC0


//================================================================================
// MDP Instructions - Processed by Command Instruction processor (CIP)
//================================================================================

// Flow Control Instructions
#define SET_DEFAULT           (0x01 << 24)            // set the data path to initial state; does not affect cip, sst etc.
#define SET_INTR                (0x02 << 24)            // set cip interrupt
#define SUBROUTINE            (0x03 << 24)
#define RETURN                (0x04 << 24)
#define JUMP                  (0x05 << 24)
#define HALT                  (0x06 << 24)
#define NOP               (0x00 << 24)
#define WAIT_JOB_DONE           (0x07 << 24)
#define WAIT_LINE_GTE(p1)         ((0x08 << 24) | (0xfff&(p1)))   // p1 - line number(12 bits); if read_pointer <= line number, instruction exec. paused
#define WAIT_LINE_LT(p1)          ((0x09 << 24) | (0xfff&(p1)))   // p1 - line number(12 bits); if read_pointer >= line number, instruction exec. paused

// LCD Configuration Instructions
#define SET_LCD_CNTL_ADDR         (0x0A << 24)
#define SEND_LCD_CNTL(p1)         ((0x0B << 24) | (0x3ffff&(p1)))   // p1 - control value (18 bits)
#define SET_LCD_DATA_ADDR         (0x0C << 24)            // destination address increment control - bit(0) of address
//No 0x0D Instruction
#define SEND_LCD_DATA(p1)         ((0x0E << 24) | (0x3ffff&(p1)))   // p1 - data value (18 bits)
#define SET_LCD_FORMAT(p1, p2)        ((0x0F << 24) | ((p1) << 8) | (p2)) // p1 - verify enable (forces hsize to '10'), p2 - LCD data formats

// Fetch, Flip, Rotate Instructions
#define SET_GFX_ADDR            (0x10 << 24)
#define SET_GFX_LINE(p1)          ((0x11 << 24) | (0xffff&(p1)))    // p1 - jump value
#define SET_GFX_BLOCK(p1)         ((0x12 << 24) | (0xffff&(p1)))    // p2 - jump value
#define SET_GFX_STRIPE(p1)        ((0x13 << 24) | (0xffff&(p1)))    // p1 - jump value
#define SET_LUMA_ADDR           (0x14 << 24)
#define SET_LUMA_LINE(p1)         ((0x15 << 24) | (0xffff&(p1)))    // p1 - jump value
#define SET_LUMA_BLOCK(p1)        ((0x16 << 24) | (0xffff&(p1)))    // p1 - jump value
#define SET_LUMA_STRIPE(p1)       ((0x17 << 24) | (0xffff&(p1)))    // p1 - jump value
#define SET_CHROMA_ADDR         (0x18 << 24)
#define SET_CHROMA_LINE(p1)       ((0x19 << 24) | (0xffff&(p1)))    // p1 - jump value
#define SET_CHROMA_BLOCK(p1)        ((0x1A << 24) | (0xffff&(p1)))    // p1 - jump value
#define SET_CHROMA_STRIPE(p1)       ((0x1B << 24) | (0xffff&(p1)))    // p1 - jump value
#define SET_FFR_MODE(p1, p2, p3, p4)  ((0x1C << 24) | ((p1) << 16) | ((p2) << 8) | ((p3) << 4) | (p4))  // p1 - fringe override [top(msb), bottom, left, right(lsb)]; p2 - operation; p3 - video orientation; p4 - gfx operation
#define SEND_JOB_BEGIN          (0x1D << 24)

//Aliased Instructions
#define SET_FRGND_ADDR            SET_GFX_ADDR
#define SET_FRGND_LINE(p1)          SET_GFX_LINE(p1)
#define SET_FRGND_BLOCK(p1)       SET_GFX_BLOCK(p1)
#define SET_FRGND_STRIPE(p1)      SET_GFX_STRIPE(p1)
#define SET_BKGND_ADDR          SET_LUMA_ADDR
#define SET_BKGND_LINE(p1)        SET_LUMA_LINE(p1)
#define SET_BKGND_BLOCK(p1)       SET_LUMA_BLOCK(p1)
#define SET_BKGND_STRIPE(p1)      SET_LUMA_STRIPE(p1)


// Color Convert, Scale Instructions
#define SET_CCS_MODE(p1)          ((0x1E << 24) | (p1))         // p1 - color conversion type
#define SET_CCS_CCTF(p1, p2)        ((0x1F << 24) | (p1 << 16) | (p2))  // p1 - color conversion coeff. to program (0 to 12); p2 - coeff. value

// Mix, Blend, Crop Instructions
#define SET_MBC_TV(p1)            ((0x20 << 24) | (p1))         // p1 - transparency value (24 bits)
#define SET_MBC_ALPHA(p1, p2)     ((0x21 << 24) | ((p1) << 8) | (p2))   // p1 - alpha prime, p2 - alpha
#define SET_MBC_ACTION(p1)        ((0x22 << 24) | (p1))         // p1 - MBC modes
#define SET_MBC_ACTION_DITHER     (1<<3)     

// Virtual Device Buffer Instructions
#define SET_VDB_SIZE(p1, p2, p3)      ((0x24 << 24) | ((p1) << 16)  | ((p2) << 8) | (p3)) // p1 - simultaneous VDB fill/drain enable; p2 - vdb stripes per image; p3 - vdb blocks per stripe
#define SET_VDB_CROP_X(p1, p2)      ((0x3C << 24) | ((p1) << 16) | (p2))        // p1 - vdb left column; p2 - vdb right column
#define SET_VDB_CROP_Y(p1, p2)      ((0x3D << 24) | ((p1) << 16) | (p2))        // p1 - vdb top row; p2 - vdb bottom row

// RPB Instructions
#define SET_RPB_DATA_FORMAT(p1, p2)     ((0x29 << 24) | ((p1) << 8) | (p2))     // p1 - verify enable (forces hsize to '10'), p2 - LCD data formats
                                        // aliased internally with SET_LCD_FORMAT but, different opcodes

// Misc. Instructions
#define SET_MBC_GAMMA(p1, p2, p3)   ((0x3A << 24) | ((p1) << 16) | ((p2) << 8) | (p3))    // p1 - r/g/b enable, p2 - RAM addr(6 bits), p3 - gamma value(6 bits)
#define SET_MDP_CGC_EN          (0x3B << 24)
#define SET_POWER(p1)         ((0x3E << 24) | (p1))                 // p1 (11 bits) - enable (high) or disable(low) the 11 cgc's seperately
                                              // default value of p1 on reset = 0x0. so enable reqd. for cgc's in the scripts
#define SET_MEMORY_MAX          (0x3F << 24)                    // (32 bit address on the second word - 4 byte aligned) - set the max. memory location beyond which the FFR block shouldn't access. a fixed burst of 16 bytes (or four 32-bit words) is generated by MDP on EBI1

// MDDI Instructions

#define SET_MDDI_X_LEFT(p1)           ((0x31 << 24) | (p1))         // p1 - mddi x left
#define SET_MDDI_X_RIGHT(p1)          ((0x2A << 24) | (p1))         // p1 - mddi x right
#define SET_MDDI_Y_TOP(p1)            ((0x2B << 24) | (p1))         // p1 - mddi y top
#define SET_MDDI_Y_BOTTOM(p1)         ((0x2C << 24) | (p1))         // p1 - mddi y bottom
#define SET_MDDI_X_START(p1)          ((0x2D << 24) | (p1))         // p1 - mddi x start
#define SET_MDDI_Y_START(p1)          ((0x2E << 24) | (p1))         // p1 - mddi y start
#define SET_MDDI_PIXEL_COUNT(p1)      ((0x2F << 24) | (p1))         // p1 - mddi pixel count
#define SEND_MDDI_START               (0x30 << 24)
#define WAIT_MDDI_DONE                (0x32 << 24)

#define SET_MDDI_CLIENT_ID(p1)        ((0x33 << 24) | (p1))         // p1 - client id
#define SET_MDDI_VID_FMT_DESC(p1)     ((0x34 << 24) | (p1))         // p1 - video format desc
#define SET_MDDI_PIXEL_DATA_ATTR(p1)  ((0x35 << 24) | (p1))         // p1 - pixel data attrib

#endif /* MDP_HW_H */

