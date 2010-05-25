#ifndef MEMORY_MAP_H
#define MEMORY_MAP_H
/*==========================================================================

                     ARM MMU Definitions Header File

DESCRIPTION
    Definitions specific to the ARM Memory Management Unit.


Copyright (c) 2007 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/boot/memory_map_full.h#7 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/29/08   aks     Port for QSC11x0
04/27/07   amw     Extend EBI1 CS0 to use 64MB.
03/12/07   amw     Extend EBI2 LCD region.
01/30/07   amw     Updated MMU for QSC6055.
01/18/05   ts      Changed memory map to use 16MB for code in first NOR
                   chip.
09/21/04   rp      Added Progressive boot support
05/13/04   ap      Updated MMU for 6550.
04/15/04   AP      Ported for MSM6550 from MSM6500.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "msmhwioreg.h"

/*===========================================================================

                        ADDRESS MAP REGIONS

===========================================================================*/

/* ALL SIZES ARE IN KBYTES */

#define QSC11x0_MEMORY_0_BASE          EBI1_CS0_BASE
#define QSC11x0_MEMORY_0_SIZE				(FLASH_NOR_EFS2_START_BYTE_OFFSET - QSC11x0_MEMORY_0_BASE) / 1024

#define	QSC11x0_MEMORY_EFS_BASE			(FLASH_NOR_EFS2_START_BYTE_OFFSET)
#define	QSC11x0_MEMORY_EFS_SIZE			(FLASH_NOR_EFS2_END_BYTE_OFFSET - FLASH_NOR_EFS2_START_BYTE_OFFSET + 1) / 1024

#define QSC11x0_GPIO2_BASE                   GPIO2_BASE
#define QSC11x0_GPIO2_SIZE                   (64*1024)

#define QSC11x0_MEMORY_1_BASE                EBI1_CS1_BASE
#define QSC11x0_MEMORY_1_SIZE                (T_RAM_SIZE/1024)

#define QSC11x0_PMEM_DATA_BASE               (QSC11x0_MEMORY_1_BASE + T_RAM_SIZE - QSC11x0_PMEM_DATA_SIZE*1024)
#define QSC11x0_PMEM_DATA_SIZE               128

#define QSC11x0_MEMORY_2_BASE                EBI1_CS2_BASE
#define QSC11x0_MEMORY_2_SIZE                (32*1024)

#define QSC11x0_MEMORY_3_BASE                EBI1_CS3_BASE
#define QSC11x0_MEMORY_3_SIZE                (0*1024)

#define QSC11x0_MEMORY_4_BASE                EBI2_CS0_BASE
#define QSC11x0_MEMORY_4_SIZE                (128*1024)

#define QSC11x0_MEMORY_5_BASE                EBI2_CS1_BASE
#define QSC11x0_MEMORY_5_SIZE                (0*64)

#define QSC11x0_MEMORY_6_BASE                EBI2_CS2_BASE
#define QSC11x0_MEMORY_6_SIZE                (0*64)

#define QSC11x0_MEMORY_7_BASE                EBI2_CS3_BASE
#define QSC11x0_MEMORY_7_SIZE                (0*64)

#define QSC11x0_LCD_BASE                     EBI2_LCD_BASE
#define QSC11x0_LCD_SIZE                     (8*1024)

#define QSC11x0_MOT_EST_BASE                 MOT_EST_BASE
#define QSC11x0_MOT_EST_SIZE                 (128*1024)

#define QSC11x0_ADSP_BASE                    ADSP_BASE
#define QSC11x0_ADSP_SIZE                    (128*1024)

#define QSC11x0_IMEM_BASE                    IMEM_BASE
#define QSC11x0_IMEM_SIZE                    (128)

#define QSC11x0_MEMORY_PT_BASE				(uint32)((void*)&PAGETABLE_L1[0])
#define QSC11x0_MEMORY_PT_SIZE				((MMU_PT_SIZE + MMU_NUM_COARSE_PT * MMU_COARSE_PT_SIZE) * sizeof(uint32)) / 1024

#define QSC11x0_CHIP_BUS_BASE                CHIP_BASE
#define QSC11x0_CHIP_BUS_SIZE                (64*1024)

#define QSC11x0_UARTDM_BASE                  TOP_UART_BASE
#define QSC11x0_UARTDM_SIZE                  (256)

#define QSC11x0_MODEM_BASE                   MODEM_BASE
#define QSC11x0_MODEM_SIZE                   (128*1024)

#define QSC11x0_GSBI0_BASE                   GSBI0_BASE
#define QSC11x0_GSBI0_SIZE                   (128*1024)

#define QSC11x0_GSBI0_CTRL                   GSBI0_CTRL_BASE
#define QSC11x0_GSBI0_UART                   GSBI0_UART_BASE  
#define QSC11x0_GSBI0_I2C                    GSBI0_I2C_BASE  
#define QSC11x0_GSBI0_SPI                    GSBI0_SPI_BASE  

#define QSC11x0_GSBI1_BASE                   GSBI1_BASE
#define QSC11x0_GSBI1_SIZE                   (128*1024) 

#define QSC11x0_GSBI1_CTRL                   GSBI1_CTRL_BASE
#define QSC11x0_GSBI1_UART                   GSBI1_UART_BASE 
#define QSC11x0_GSBI1_I2C                    GSBI1_I2C_BASE 
#define QSC11x0_GSBI1_SPI                    GSBI1_SPI_BASE

#define QSC11x0_GSBI2_BASE                   GSBI2_BASE
#define QSC11x0_GSBI2_SIZE                   (128*1024) 

#define QSC11x0_GSBI2_CTRL                   GSBI2_CTRL_BASE
#define QSC11x0_GSBI2_UART                   GSBI2_UART_BASE 
#define QSC11x0_GSBI2_I2C                    GSBI2_I2C_BASE 
#define QSC11x0_GSBI2_SPI                    GSBI2_SPI_BASE 

#define QSC11x0_RESERVED_0_BASE              RESERVED0_BASE
#define QSC11x0_RESERVED_0_SIZE              (128*1024)

#define QSC11x0_RESERVED_1_BASE              RESERVED1_BASE
#define QSC11x0_RESERVED_1_SIZE              (128*1024)

#define QSC11x0_RESERVED_2_BASE              RESERVED2_BASE
#define QSC11x0_RESERVED_2_SIZE              (128*1024)

#define QSC11x0_RESERVED_3_BASE              RESERVED3_BASE
#define QSC11x0_RESERVED_3_SIZE              (128*1024)

#define QSC11x0_RESERVED_4_BASE              RESERVED4_BASE
#define QSC11x0_RESERVED_4_SIZE              (128*1024)

#define QSC11x0_RESERVED_5_BASE              RESERVED5_BASE
#define QSC11x0_RESERVED_5_SIZE              (128*1024)

#define QSC11x0_RESERVED_6_BASE              RESERVED6_BASE
#define QSC11x0_RESERVED_6_SIZE              (128*1024)

#define QSC11x0_RESERVED_7_BASE              RESERVED7_BASE
#define QSC11x0_RESERVED_7_SIZE              (128*1024)

#define QSC11x0_RESERVED_8_BASE              RESERVED8_BASE
#define QSC11x0_RESERVED_8_SIZE              (128*1024)

#define QSC11x0_RESERVED_9_BASE             RESERVED9_BASE
#define QSC11x0_RESERVED_9_SIZE             (128*1024)

#define QSC11x0_RESERVED_10_BASE             RESERVED10_BASE
#define QSC11x0_RESERVED_10_SIZE             (128*1024)

#define QSC11x0_BOOT_ROM_BASE                BOOT_ROM_BASE
#define QSC11x0_BOOT_ROM_SIZE                (32)

#define EBI1_CHIPSLECT_1_BASE                QSC11x0_MEMORY_1_BASE

#endif /*MEMORY_MAP_H*/
