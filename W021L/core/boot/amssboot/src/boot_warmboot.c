/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

          B O O T   W A R M B O O T  

GENERAL DESCRIPTION
  This module performs WARM BOOT specific initialization such as backup and 
  restore.

EXTERNALIZED FUNCTIONS
  boot_warmboot_handler

Copyright (c) 2005,2006 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/drivers/boot/boot_warmboot.c#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/22/05   tkuo     Created.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "msm.h"
#include "boot_warmboot.h"
#include "err.h"
#include "assert.h"
#include "boot_feature_cfg_data.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/
/* Array to restore MPMC and EBI1 settings */
uint32 warmboot_ebi1_mpmc_rec_array[]=
{
  HWIO_ADDR(MPMC_DY_TRD_CFG),
  HWIO_ADDR(MPMC_DY_TRP),
  HWIO_ADDR(MPMC_DY_TRAS),
  HWIO_ADDR(MPMC_DY_TSREX),
  HWIO_ADDR(MPMC_DY_TAPR),
  HWIO_ADDR(MPMC_DY_TDAL),
  HWIO_ADDR(MPMC_DY_TWR),
  HWIO_ADDR(MPMC_DY_TRC),
  HWIO_ADDR(MPMC_DY_TRFC),
  HWIO_ADDR(MPMC_DY_TXSR),
  HWIO_ADDR(MPMC_DY_TRRD),
  HWIO_ADDR(MPMC_DY_TMRD),
  HWIO_ADDR(MPMC_DY_CONFIG0),
  HWIO_ADDR(MPMC_DY_RAS_CAS0),
  HWIO_ADDR(MPMC_DY_CONFIG1),
  HWIO_ADDR(MPMC_DY_RAS_CAS1),
  HWIO_ADDR(MPMC_DY_CONFIG2),
  HWIO_ADDR(MPMC_DY_RAS_CAS2),
  HWIO_ADDR(MPMC_DY_CONFIG3),
  HWIO_ADDR(MPMC_DY_RAS_CAS3),
  HWIO_ADDRI(EBI1_CSn_CFG0,0),
  HWIO_ADDRI(EBI1_CSn_CFG0,1),
  HWIO_ADDRI(EBI1_CSn_CFG0,2),
  HWIO_ADDRI(EBI1_CSn_CFG0,3),
  HWIO_ADDRI(EBI1_CSn_CFG1,0),
  HWIO_ADDRI(EBI1_CSn_CFG1,1),
  HWIO_ADDRI(EBI1_CSn_CFG1,2),
  HWIO_ADDRI(EBI1_CSn_CFG1,3),
  HWIO_ADDR(EBI1_AHB_BUS_PRIORITY),
  HWIO_ADDR(EBI1_AHB0_COAL_EN),
};


/*--------------------------------------------------------------------------
   Local variable definitions.
-------------------------------------------------------------------------- */
/***************************************************************************
**                   ALWAYS-ON-DOMAIN Layout                              **                    
**                   MPM_REGn_FILE(from 0 to 7)                           **
** We use total 256 bit in ALWAYS-ON-DOMAIN to store the memory settings  ** 
** and return address.                                                    **
** These are used by PBL to recover from warm boot                        **
***************************************************************************/
/*---------------------------------------------------------------------------
** The following sections define the mask to backup the content into PBL 
** MPM_REGn_FILE where is the always-on-domain. 
** This cannot be changed unless PBL is changed. The layout should be maintain
** to get PBL recover from warmboot.
---------------------------------------------------------------------------*/
/***************************************************************************
  The following sections define the mask for SDRAM;
****************************************************************************/
/*----------------------------------------------------------------------------
Bits    Register Name/Content   Description
0:1     SDRAM_ChipSelect        SDRAM can be CS0, CS1 ,CS2 and CS3
2:3     EBI1_MEM_CTLR_MODE  
4:22    PAD_HDRIVE_MSEL_1 
23:25   GPIO_PAD_HDRlVE_MSEL_3_BITS
26      EBI1_NOR_SDRAM_SETTING_BIT If this bit is set to 1, we assume NOR/SDRAM.
                                   and recover from SDRAM.
27:31                           Unused.
----------------------------------------------------------------------------*/
/* Definition for SDRAM chip select either CS2 ro CS3 or CS0 or CS1 */
#define  BOOT_WARMBOOT_SDRAM_CHIPSELECT_0  0x0UL
#define  BOOT_WARMBOOT_SDRAM_CHIPSELECT_1  0x1UL
#define  BOOT_WARMBOOT_SDRAM_CHIPSELECT_2  0x2UL
#define  BOOT_WARMBOOT_SDRAM_CHIPSELECT_3  0x3UL
/*------------------------------------------------------------*/
#define MPM_REG0_SDRAM_CHIPSELECT_MASK                     0x00000003UL
#define MPM_REG0_SDRAM_CHIPSELECT_SHFT                     0x00000000UL
#define MPM_REG0_SDRAM_EBI1_MEM_CTLR_MODE_MASK             0x0000000CUL
#define MPM_REG0_SDRAM_EBI1_MEM_CTLR_MODE_SHFT             0x00000002UL
#define MPM_REG0_SDRAM_PAD_HDRIVE_MSEL_1_MASK              0x007FFFF0UL 
#define MPM_REG0_SDRAM_PAD_HDRIVE_MSEL_1_SHFT              0x00000004UL 
#define MPM_REG0_SDRAM_GPIO_PAD_HDRIVE_MSEL_3BITS_MASK     0x03800000UL
#define MPM_REG0_SDRAM_GPIO_PAD_HDRIVE_MSEL_3BITS_SHFT     0x00000017UL
#define MPM_REG0_SDRAM_EBI1_NOR_SDRAM_SETTING_BIT_MASK     0x04000000UL
#define MPM_REG0_SDRAM_EBI1_NOR_SDRAM_SETTING_BIT_SHFT     0x0000001AUL
/*----------------------------------------------------------------------------
Register Name: MPM_REG1_FILE

Bits    Register Name/Content   Description
0:10    MPMC_DY_REF 
11:13   MPMC_CONTROL    
14:15   MPMC_CONFIG 
16:24   MPMC_DY_CNTL    
25:26   MPMC_DY_TRD_CFG 
27:30   EBI1_STDY_SEL          
----------------------------------------------------------------------------*/
#define MPM_REG1_SDRAM_MPMC_DY_REF_MASK            0x000007FFUL
#define MPM_REG1_SDRAM_MPMC_DY_REF_SHFT            0x00000000UL
#define MPM_REG1_SDRAM_MPMC_CONTROL_MASK           0x00003800UL
#define MPM_REG1_SDRAM_MPMC_CONTROL_SHFT           0x0000000BUL
#define MPM_REG1_SDRAM_MPMC_CONFIG_MASK            0x0000C000UL
#define MPM_REG1_SDRAM_MPMC_CONFIG_SHFT            0x0000000DUL
#define MPM_REG1_SDRAM_MPMC_DY_CNTL_MASK           0x01FF0000UL
#define MPM_REG1_SDRAM_MPMC_DY_CNTL_SHFT           0x00000010UL
#define MPM_REG1_SDRAM_MPMC_DY_TRD_CFG_MASK        0x06000000UL
#define MPM_REG1_SDRAM_MPMC_DY_TRD_CFG_SHFT        0x00000019UL
#define MPM_REG1_SDRAM_EBI1_STDY_SEL_MASK          0x78000000UL
#define MPM_REG1_SDRAM_EBI1_STDY_SEL_SHFT          0x0000001BUL

/*----------------------------------------------------------------------------
Register Name: MPM_REG2_FILE

Bits    Register Name/Content   Description
0:20    MPMC_DY_CONFIGn 
21:30   MPMC_DY_RAS_CASn
31  SDRAM_Present           Use SDRAM or PSRAM for warm boot
----------------------------------------------------------------------------*/
#define MPM_REG2_SDRAM_MPMC_DY_CONFIGn_MASK        0x001FFFFFUL
#define MPM_REG2_SDRAM_MPMC_DY_CONFIGn_SHFT        0x00000000UL
#define MPM_REG2_SDRAM_MPMC_DY_RAS_CASn_MASK       0x7FE00000UL
#define MPM_REG2_SDRAM_MPMC_DY_RAS_CASn_SHFT       0x00000015UL
#define MPM_REG2_SDRAM_PRESRENT_MASK               0x80000000UL
#define MPM_REG2_SDRAM_PRESRENT_SHFT               0x0000001FUL

/*----------------------------------------------------------------------------
Register Name: MPM_REG3_FILE

Bits    Register Name/Content Description
0:6 GPIO_PAGE             For example:GPIO 67(0x43)
7   GPIO_CFG              0: input( no operation), 1:output(put value as 0x4)
8:14    GPIO_PAGE             For example:GPIO 68(0x44)
15  GPIO_CFG              0: input( no operation), 1:output(put value as 0x4)
16:22   GPIO_PAGE             For example:GPIO 69(0x45)
23  GPIO_CFG              0: input( no operation), 1:output(put value as 0x4)
24:30   GPIO_PAGE             For example:GPIO 70(0x46)
31  GPIO_CFG              0: input( no operation), 1:output(put value as 0x4)
----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
Register Name: MPM_REG_4FILE

Bits    Register Name/Content   Description
0:6     GPIO_PAGE               For example: GPIO 71(0x47)
7       GPIO_CFG                0: input( no operation), 1:output(put value as 0x4)
8:14    GPIO_PAGE               For example: GPIO 72(0x48)
15      GPIO_CFG                0: input( no operation), 1:output(put value as 0x4)
16:22   GPIO_PAGE               For example: GPIO 73(0x49)
23      GPIO_CFG                0: input( no operation), 1:output(put value as 0x4)
24:30   GPIO_PAGE               For example: GPIO 74(0x4A)
31      GPIO_CFG                0: input( no operation), 1:output(put value as 0x4)
----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
Register Name: MPM_REG5_FILE

Bits    Register Name/Content   Description
0:6     GPIO_PAGE               For example: GPIO 75(0x4B)
7       GPIO_CFG                0: input( no operation), 1:output(put value as 0x4)
8:14    GPIO_PAGE               For example: GPIO 77(0x4D)
15      GPIO_CFG                0: input( no operation), 1:output(put value as 0x4)
16:22   GPIO_PAGE               For example: GPIO 78(0x4E)
23      GPIO_CFG                0: input( no operation), 1:output(put value as 0x4)
24:30   GPIO_PAGE               For example: GPIO 79(0x4F)
31      GPIO_CFG                0: input( no operation), 1:output(put value as 0x4)
----------------------------------------------------------------------------*/
#define MPM_REGn_SDRAM_GPIO_PAGE0_MASK   0x0000007FUL
#define MPM_REGn_SDRAM_GPIO_PAGE0_SHFT   0x00000000UL
#define MPM_REGn_SDRAM_GPIO_CFG0_MASK    0x00000080UL
#define MPM_REGn_SDRAM_GPIO_CFG0_SHFT    0x00000007UL
#define MPM_REGn_SDRAM_GPIO_PAGE1_MASK   0x00007F00UL
#define MPM_REGn_SDRAM_GPIO_PAGE1_SHFT   0x00000008UL
#define MPM_REGn_SDRAM_GPIO_CFG1_MASK    0x00008000UL
#define MPM_REGn_SDRAM_GPIO_CFG1_SHFT    0x0000000FUL
#define MPM_REGn_SDRAM_GPIO_PAGE2_MASK   0x007F0000UL
#define MPM_REGn_SDRAM_GPIO_PAGE2_SHFT   0x00000010UL
#define MPM_REGn_SDRAM_GPIO_CFG2_MASK    0x00800000UL
#define MPM_REGn_SDRAM_GPIO_CFG2_SHFT    0x00000017UL
#define MPM_REGn_SDRAM_GPIO_PAGE3_MASK   0x7F000000UL
#define MPM_REGn_SDRAM_GPIO_PAGE3_SHFT   0x00000018UL
#define MPM_REGn_SDRAM_GPIO_CFG3_MASK    0x80000000UL
#define MPM_REGn_SDRAM_GPIO_CFG3_SHFT    0x0000001FUL
/*----------------------------------------------------------------------------
Register Name: MPM_REG6_FILE

Bits    Register Name/Content   Description
0:31    EBI1_CFG    Configure EBI1 register
----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
Register Name: MPM_REG7_FILE

Bits    Register Name/Content   Description
0:31    Address to jump to  After warm boot, we will jump to this address.
----------------------------------------------------------------------------*/

/***************************************************************************
  The following sections define the mask for PSRAM
****************************************************************************/
/*--------------------------------------------------------------------------
Register Name: MPM_REG0_FILE

Bits    Register Name/Content   Description
0: 31   EBI1_CSn_CFG0          
---------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
Register Name: MPM_REG1_FILE

Bits    Register Name/Content   Description
0:31    EBI1_CSn_CFG1           
---------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
Register Name: MPM_REG2_FILE

Bits    Register Name/Content   Description
0:1     PSRAM_ChipSelect        0:CS0,1 CS1, 2:CS2, 3:CS3
2:5     EBI1_STDY_SEL
6:7     EBI1_MEM_CTLR_MODE
8:26    PAD_HDRIVE_MSEL_1
26:30                           Unused
31      SDRAM_Present           SDRAM present: 1, PSRAM present:0
                                This bit needs to be in sync with the profile
                                of SDRAM.
---------------------------------------------------------------------------*/

#define MPM_REG2_PSRAM_CHIPSELECT_MASK             0x00000003UL
#define MPM_REG2_PSRAM_CHIPSELECT_SHFT             0x00000000UL
#define MPM_REG2_PSRAM_EBI1_STDY_SEL_MASK          0x0000003CUL
#define MPM_REG2_PSRAM_EBI1_STDY_SEL_SHFT          0x00000002UL
#define MPM_REG2_PSRAM_EBI1_MEM_CTLR_MODE_MASK     0x000000C0UL
#define MPM_REG2_PSRAM_EBI1_MEM_CTLR_MODE_SHFT     0x00000006UL
#define MPM_REG2_PSRAM_PAD_HDRIVE_MSEL_1_MASK      0x07FFFF00UL
#define MPM_REG2_PSRAM_PAD_HDRIVE_MSEL_1_SHFT      0x00000008UL

/*--------------------------------------------------------------------------
Register Name: MPM_REG3_FILE

Bits    Register Name/Content   Description
0:31    EBI1_CFG
---------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
Register Name: MPM_REG4_FILE

Bits    Register Name/Content   Description
0:6 GPIO_PAGE               For example: GPIO 73(0x49)
7                               Unused.
8:14    GPIO_PAGE               For example: GPIO 74(0x4A)
15                              Unused. 
16:22   GPIO_PAGE               For example: GPIO 75(0x4B)
23                              Unused.
24:25   GPIO_CFG                GPIO output value.0: input( no operation), 
                                1:output(put value as 0x4),2:(put value as 0x8).
26:27   GPIO_CFG                GPIO output value.0: input( no operation), 
                                1:output(put value as 0x4),2:(put value as 0x8).
28:29   GPIO_CFG                GPIO output value.0: input( no operation), 
                                1:output(put value as 0x4),2:(put value as 0x8).
30:31                           Unused.
---------------------------------------------------------------------------*/
#define MPM_REG4_PSRAM_GPIO_PAGE0_MASK   0x0000007FUL
#define MPM_REG4_PSRAM_GPIO_PAGE0_SHFT   0x00000000UL
#define MPM_REG4_PSRAM_GPIO_CFG0_MASK    0x03000000UL
#define MPM_REG4_PSRAM_GPIO_CFG0_SHFT    0x00000018UL
#define MPM_REG4_PSRAM_GPIO_PAGE1_MASK   0x00007F00UL
#define MPM_REG4_PSRAM_GPIO_PAGE1_SHFT   0x00000008UL
#define MPM_REG4_PSRAM_GPIO_CFG1_MASK    0x0C000000UL
#define MPM_REG4_PSRAM_GPIO_CFG1_SHFT    0x0000001AUL
#define MPM_REG4_PSRAM_GPIO_PAGE2_MASK   0x007F0000UL
#define MPM_REG4_PSRAM_GPIO_PAGE2_SHFT   0x00000010UL
#define MPM_REG4_PSRAM_GPIO_CFG2_MASK    0x30000000UL
#define MPM_REG4_PSRAM_GPIO_CFG2_SHFT    0x0000001CUL
/*--------------------------------------------------------------------------
Register Name: MPM_REG5_FILE

Bits    Register Name/Content   Description
0:6     GPIO_PAGE               For example: GPIO 76(0x4C)
7       GPIO_CFG                GPIO 76 output value.0: input( no operation), 
                                1:output(put value as 0x4).
8:14    GPIO_PAGE               For example: GPIO 77(0x4D)
15      GPIO_CFG                GPIO 77 output value.0: input( no operation), 
                                1:output(put value as 0x4).
16:22   GPIO_PAGE               For example: GPIO 78(0x4E),  
23      GPIO_CFG                GPIO 78 output value:0:input( no operation),
                                1:output(put value as 0x4).
24:30   GPIO_PAGE               For example: GPIO 79(0x4F).0:input( no operation),   
31      GPIO_CFG                GPIO 79 output value: 0:input( no operation),
                                1:output(put value as 0x4).
---------------------------------------------------------------------------*/
#define MPM_REG5_PSRAM_GPIO_PAGE0_MASK   0x0000007FUL
#define MPM_REG5_PSRAM_GPIO_PAGE0_SHFT   0x00000000UL
#define MPM_REG5_PSRAM_GPIO_CFG0_MASK    0x00000080UL
#define MPM_REG5_PSRAM_GPIO_CFG0_SHFT    0x00000007UL
#define MPM_REG5_PSRAM_GPIO_PAGE1_MASK   0x00007F00UL
#define MPM_REG5_PSRAM_GPIO_PAGE1_SHFT   0x00000008UL
#define MPM_REG5_PSRAM_GPIO_CFG1_MASK    0x00008000UL
#define MPM_REG5_PSRAM_GPIO_CFG1_SHFT    0x0000000FUL
#define MPM_REG5_PSRAM_GPIO_PAGE2_MASK   0x007F0000UL
#define MPM_REG5_PSRAM_GPIO_PAGE2_SHFT   0x00000010UL
#define MPM_REG5_PSRAM_GPIO_CFG2_MASK    0x00800000UL
#define MPM_REG5_PSRAM_GPIO_CFG2_SHFT    0x00000017UL
#define MPM_REG5_PSRAM_GPIO_PAGE3_MASK   0x7F000000UL
#define MPM_REG5_PSRAM_GPIO_PAGE3_SHFT   0x00000018UL
#define MPM_REG5_PSRAM_GPIO_CFG3_MASK    0x80000000UL
#define MPM_REG5_PSRAM_GPIO_CFG3_SHFT    0x0000001FUL
/*--------------------------------------------------------------------------
Register Name: MPM_REG6_FILE

Bits    Register Name/Content   Description
0:31    GPIO_PAD_HDRIVE_MSEL_9  
---------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
Register Name: MPM_REG7_FILE

Bits    Register Name/Content   Description
0:31    Address to jump to  After warm boot, we will jump to this address.
---------------------------------------------------------------------------*/

/********************************************************************/
/*  This section has to be in special location                      */
/********************************************************************/
#pragma arm section code= "WARMBOOT_MEM_RESTORE" 
/*===========================================================================

FUNCTION  BOOT_WARMBOOT_NOR_RESTORE

DESCRIPTION
   This function continues to perform NOR settings restore from configuration  
   data file.
DEPENDENCIES
   This function should be done after we restore the minimal configurations 
   from PBL for NOR recovery. And this function has to be in RAM. We cannot 
   configure NOR while running at NOR. 
RETURN VALUE
  None.

SIDE EFFECTS
  This part has to be in either PSRAM or IMEM or Cache. It cannot be in NOR
  while we configure NOR.
===========================================================================*/
void boot_warmboot_nor_restore(void)
{
  uint32 nor_reg_addr;  /* MSM address */
  uint32 nor_reg_value; /* MSM register values */
  /* Configure the followings to make NOR into BURST mode */
  nor_reg_addr = boot_get_feature_cfg_item (CFG_ITEM_WARMBOOT_NOR_16_CFG1_ADDR);
  nor_reg_value= boot_get_feature_cfg_item (CFG_ITEM_WARMBOOT_NOR_16_CFG1_VALUE);
  out_word(nor_reg_addr,nor_reg_value);
  nor_reg_addr = boot_get_feature_cfg_item (CFG_ITEM_WARMBOOT_NOR_16_CFG2_ADDR);
  nor_reg_value= boot_get_feature_cfg_item (CFG_ITEM_WARMBOOT_NOR_16_CFG2_VALUE);
  out_word(nor_reg_addr,nor_reg_value);
  nor_reg_addr = boot_get_feature_cfg_item (CFG_ITEM_WARMBOOT_NOR_16_CFG3_ADDR);
  nor_reg_value= boot_get_feature_cfg_item (CFG_ITEM_WARMBOOT_NOR_16_CFG3_VALUE);
  out_word(nor_reg_addr,nor_reg_value);
  nor_reg_addr = boot_get_feature_cfg_item (CFG_ITEM_WARMBOOT_NOR_16_CFG4_ADDR);
  nor_reg_value= boot_get_feature_cfg_item (CFG_ITEM_WARMBOOT_NOR_16_CFG4_VALUE);
  out_word(nor_reg_addr,nor_reg_value);

 return;
}/* boot_warmboot_nor_restore */

/*===========================================================================

FUNCTION  BOOT_WARMBOOT_LOCAL_RESTORE

DESCRIPTION
   FOR NAND/SDRAM and NOR/SDRAM restore,
   This function continues to perform SDRAM or NOR settings restore from SDRAM local 
   buffer.
   FOR NOR/PSRAM restore,
   This function continues to perform NOR and PSRAM settings restore from PSRAM or 
   local buffer.

DEPENDENCIES
   This function should be done after we restore the minimal configurations 
   from PBL. 
RETURN VALUE
  None.

SIDE EFFECTS
  This part has to be in either IMEM or Cache. It cannot be in SDRAM or NOR 
  or PSRAM.
===========================================================================*/
void boot_warmboot_local_restore( 
	uint32 *warmboot_storage_ptr /* Local storage buffer to recover the settings
	                             ** not done by PBL 
								 */
)
{
  uint32 size_to_restore;/* number of elements stored in array */
  uint32 i =0; /* index for loop */
  uint32 msm_reg_addr;  /* MSM address */
  uint32 msm_reg_value; /* MSM register values */

  /* Check paramenters */
  /* If we see warmboot_storage_ptr == NULL, then we don't want to do local 
  ** restore.
  */
  if(warmboot_storage_ptr == NULL)
  {
    return;
  }
  /* Store these settings into storage buffer */
  size_to_restore = *warmboot_storage_ptr++;
  for (i=0;i<size_to_restore;i++) 
  {
    msm_reg_addr  = *warmboot_storage_ptr++;
	msm_reg_value = *warmboot_storage_ptr++;
    out_dword(msm_reg_addr,msm_reg_value);
  }/* end of for loop */

 return;
}/* boot_warmboot_local_restore */

#pragma arm section code
/********************************************************************/
/*  END of this section                                             */
/********************************************************************/
/*===========================================================================

FUNCTION  BOOT_WARMBOOT_LOCAL_BACKUP

DESCRIPTION
   For NAND/SDRAM,
   This function continues to perform SDRAM settings into SDRAM local buffer.
   For NOR/SDRAM,
   This function continues to perform SDRAM  and NOR settings into SDRAM local buffer.
   For NOR/PSRAM
   This function continues to perform NOR settings into PSRAM local buffer.
   
DEPENDENCIES
   This function should be done after we backup the minimal configurations 
   into always-on-domain for SDRAM recovery.
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void boot_warmboot_local_backup( 
     boot_flash_mem_cfg_type warmboot_mem,/* indicate which memory configuration */
	 uint32 *warmboot_storage_ptr  /* Local storage buffer to recover other 
	                                  settings not done by PBL */
)
{
  uint32 size_to_backup = ARR_SIZE(warmboot_ebi1_mpmc_rec_array);/*how many elements*/
  uint32 i =0; /* index for loop */

  /* Check paramenters */
  /* If we see warmboot_storage_ptr == NULL, then we don't want to do local 
  ** backup.
  */
  if (warmboot_storage_ptr == NULL)
  {
    return;
  }
  
  /* Store these settings into storage buffer */
   *warmboot_storage_ptr++ = size_to_backup;
  for (i=0;i<size_to_backup;i++) 
  {
    *warmboot_storage_ptr++ = warmboot_ebi1_mpmc_rec_array[i];
    *warmboot_storage_ptr++ = in_dword(warmboot_ebi1_mpmc_rec_array[i]);
  }/* end of for loop */

 return;
}/* boot_warmboot_local_backup */

/*===========================================================================

FUNCTION  BOOT_WARMBOOT_SDRAM_MAO_BACKUP

DESCRIPTION
   This function performs SDRAM settings backup into always-on-domain.
DEPENDENCIES
   It relies on always-on domain to recover SDRAM in PBL. The layout of
   always-on-domain is decided by PBL. Please refer to the comment of this file.
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void boot_warmboot_sdram_mao_backup( 
	boot_flash_mem_cfg_type warmboot_mem/* indicate which memory configuration */
)
{
 uint32 buffer; /* temporary storage */
 uint32 i; /* Index to loop through MPM_REGn_FILE */
 uint32 j; /* Index to loop through MPM_REGn_FILE */
 uint32 gpio_k;/* GPIO index  to loop through MPM_REGn_FILE*/

 /* GPIO mask used to store GPIO settings */
 uint32 gpio_mask_array[16]= {
 MPM_REGn_SDRAM_GPIO_PAGE0_MASK, 
 MPM_REGn_SDRAM_GPIO_PAGE0_SHFT, 
 MPM_REGn_SDRAM_GPIO_CFG0_MASK,
 MPM_REGn_SDRAM_GPIO_CFG0_SHFT,
 MPM_REGn_SDRAM_GPIO_PAGE1_MASK,
 MPM_REGn_SDRAM_GPIO_PAGE1_SHFT,
 MPM_REGn_SDRAM_GPIO_CFG1_MASK,
 MPM_REGn_SDRAM_GPIO_CFG1_SHFT,
 MPM_REGn_SDRAM_GPIO_PAGE2_MASK,
 MPM_REGn_SDRAM_GPIO_PAGE2_SHFT,
 MPM_REGn_SDRAM_GPIO_CFG2_MASK,
 MPM_REGn_SDRAM_GPIO_CFG2_SHFT,
 MPM_REGn_SDRAM_GPIO_PAGE3_MASK,
 MPM_REGn_SDRAM_GPIO_PAGE3_SHFT,
 MPM_REGn_SDRAM_GPIO_CFG3_MASK,
 MPM_REGn_SDRAM_GPIO_CFG3_SHFT,
 }; /*array to store GPIO masks */

 /* The followings are needed to allow PBL to recover from power collpase */

 /* Backup EBI1_CFG*/ 
 buffer = HWIO_IN(EBI1_CFG);
 HWIO_OUTI(MPM_REGn_FILE,6,buffer);
 
 /* Backup MEMORY TYPE */
 if ((warmboot_mem == BOOT_NAND_SDRAM_16_PRESENT) || 
     (warmboot_mem == BOOT_NAND_SDRAM_32_PRESENT)) 
 {
	 buffer = 0x0;
 }
 else if ((warmboot_mem == BOOT_NOR_16_SDRAM_16_PRESENT) || 
		  (warmboot_mem == BOOT_NOR_16_SDRAM_32_PRESENT)) 
 {
      buffer = 0x1;
 }
 else
 {
   ERR_FATAL("WARM BOOT MEMORY TYPE is UNKNOWN",0,0,0);
 }/* end of if */

 buffer <<=MPM_REG0_SDRAM_EBI1_NOR_SDRAM_SETTING_BIT_SHFT;
 HWIO_OUTMI(MPM_REGn_FILE,0,MPM_REG0_SDRAM_EBI1_NOR_SDRAM_SETTING_BIT_MASK,buffer);

 /* Backup PAD_HDRIVE_MSEL_1*/

 buffer = HWIO_IN(PAD_HDRIVE_MSEL_1);
 buffer <<=MPM_REG0_SDRAM_PAD_HDRIVE_MSEL_1_SHFT;
 HWIO_OUTMI(MPM_REGn_FILE,0,MPM_REG0_SDRAM_PAD_HDRIVE_MSEL_1_MASK,buffer);

 /* Backup GPIO_PAD_HDRIVE_MSEL_3BITS settings */
 /* We use one of the memory bus GPIO,GPIO79 to decide all of the remaining ones */

 buffer = HWIO_INM(GPIO_PAD_HDRIVE_MSEL_9,
				   HWIO_FMSK(GPIO_PAD_HDRIVE_MSEL_9,HDRIVE_GPIO_79));
 buffer = HWIO_SHFT(GPIO_PAD_HDRIVE_MSEL_9,HDRIVE_GPIO_79);
 buffer <<= MPM_REG0_SDRAM_GPIO_PAD_HDRIVE_MSEL_3BITS_SHFT; 
 HWIO_OUTMI(MPM_REGn_FILE,0,MPM_REG0_SDRAM_GPIO_PAD_HDRIVE_MSEL_3BITS_MASK, buffer);

 /* Back up EBI1_STDY_SEL */

 buffer = HWIO_IN(EBI1_STDY_SEL);
 buffer <<= MPM_REG1_SDRAM_EBI1_STDY_SEL_SHFT;
 HWIO_OUTMI(MPM_REGn_FILE,1,MPM_REG1_SDRAM_EBI1_STDY_SEL_MASK,buffer);

 /* Back up MPMC_CONTROL  */

 buffer = HWIO_IN(MPMC_CONTROL);
 buffer <<= MPM_REG1_SDRAM_MPMC_CONTROL_SHFT;
 HWIO_OUTMI(MPM_REGn_FILE,1,MPM_REG1_SDRAM_MPMC_CONTROL_MASK,buffer);

 /* Backup MPMC_CONFIG    */

 buffer = HWIO_IN(MPMC_CONFIG);
 buffer <<= MPM_REG1_SDRAM_MPMC_CONFIG_SHFT;
 HWIO_OUTMI(MPM_REGn_FILE,1,MPM_REG1_SDRAM_MPMC_CONFIG_MASK,buffer);

 /* Backup EBI1_MEM_CTLR_MODE*/

 buffer = HWIO_IN(EBI1_MEM_CTLR_MODE);
 buffer <<= MPM_REG0_SDRAM_EBI1_MEM_CTLR_MODE_SHFT;
 HWIO_OUTMI(MPM_REGn_FILE,0,MPM_REG0_SDRAM_EBI1_MEM_CTLR_MODE_MASK,buffer);


 if ((warmboot_mem == BOOT_NAND_SDRAM_32_PRESENT) || 
     (warmboot_mem == BOOT_NOR_16_SDRAM_32_PRESENT)) 
 {
   /* BACKUP GPIOs for 32 bit SDRAM; We put GPIO settings into 
   ** MSM_REG3_FILE, MSM_REG4_FILE and MSM_REG5_FILE, we use "for" loop.   
   ** MEMORY bus gpio starts from GPIO67 to GPIO79; GPIO76 for CS3 is skipped. 
   ** We ask customer to put the warm boot handler into CS2 always.
   **
   */
   for(gpio_k=0x43,i=3;((i<6) &&(gpio_k<0x50));i++) 
   {
     for(j=0;((j<16) && (gpio_k<0x50));j+=4,gpio_k++)
     {
       /* skip gpio76 */
       if(gpio_k==0x4C)
       {
        gpio_k++;
       }
       HWIO_OUTMI(MPM_REGn_FILE,i,gpio_mask_array[j+2],(0x1<<gpio_mask_array[j+3]));
       HWIO_OUTMI(MPM_REGn_FILE,i,gpio_mask_array[j],(gpio_k <<gpio_mask_array[j+1]));
     }/* for j */
   }/* for i */
 
 }/* end of if 32 bit SDRAM */
 else if ((warmboot_mem == BOOT_NAND_SDRAM_16_PRESENT) || 
		  (warmboot_mem == BOOT_NOR_16_SDRAM_16_PRESENT ))
 {
   /* BACKUP GPIOs for 16 bit SDRAM; We put GPIO settings into MSM_REG3_FILE,
   ** MSM_REG4_FILE and MSM_REG5_FILE, we use "for" loop.   
   ** MEMORY bus gpio starts from GPIO77 to GPIO79; GPIO76 for CS3 is skipped.
   ** Customer can put the warmboot recovery handler into CS3 here by enabling
   ** GPIO76 since there are space available for 16 bit SDRAM.
   */
   for(gpio_k=0x4D,i=3;((i<6) &&(gpio_k<0x50));i++) 
   {
     for(j=0;((j<16) && (gpio_k<0x50));j+=4,gpio_k++)
     {
       /* only GPIO77 and GPIO79 is needed for SDRAM 16 bits */
       if((gpio_k==0x4D) || (gpio_k==0x4F))
       {
         HWIO_OUTMI(MPM_REGn_FILE,i,gpio_mask_array[j+2],(0x1<<gpio_mask_array[j+3]));
         HWIO_OUTMI(MPM_REGn_FILE,i,gpio_mask_array[j],(gpio_k <<gpio_mask_array[j+1]));
         /* skip GPIO 78 */
         gpio_k++;
       }
       else
       {
         HWIO_OUTMI(MPM_REGn_FILE,i,gpio_mask_array[j+2],(0x0<<gpio_mask_array[j+3]));
         HWIO_OUTMI(MPM_REGn_FILE,i,gpio_mask_array[j],(gpio_k <<gpio_mask_array[j+1]));
        }/* end of if */
      }/* for j */
    }/* for i */
 }/* end of if 16 bit SDRAM */

 /* Continue to backup MPMC registers */

 /* Backup MPMC_DY_TRD_CFG */
 buffer = HWIO_IN(MPMC_DY_TRD_CFG);
 buffer <<= MPM_REG1_SDRAM_MPMC_DY_TRD_CFG_SHFT;
 HWIO_OUTMI(MPM_REGn_FILE,1,MPM_REG1_SDRAM_MPMC_DY_TRD_CFG_MASK,buffer);
 
 /* Backup MPMC_DY_REF     */ 
 buffer = HWIO_IN(MPMC_DY_REF);
 buffer <<= MPM_REG1_SDRAM_MPMC_DY_REF_SHFT;
 HWIO_OUTMI(MPM_REGn_FILE,1,MPM_REG1_SDRAM_MPMC_DY_REF_MASK,buffer);

 /* Bakup which chipselect SDRAM is at for warmboot recovery handler */
 /* We get this from external cfg_data item */
 buffer =boot_get_feature_cfg_item (CFG_ITEM_WARMBOOT_RAM_CHIP_SELECT);

 buffer <<= MPM_REG0_SDRAM_CHIPSELECT_SHFT;
 HWIO_OUTMI(MPM_REGn_FILE,0,MPM_REG0_SDRAM_CHIPSELECT_MASK,buffer);
 /* BACKUP MPMC_DY_RAS_CASn            */
 /* BACKUP MPMC_DY_CONFIGn             */

 switch(buffer)
 {
   case BOOT_WARMBOOT_SDRAM_CHIPSELECT_0: 

   buffer = HWIO_IN(MPMC_DY_CONFIG2);
   buffer <<= MPM_REG2_SDRAM_MPMC_DY_CONFIGn_SHFT;
   HWIO_OUTMI(MPM_REGn_FILE,2,MPM_REG2_SDRAM_MPMC_DY_CONFIGn_MASK,buffer);
  
   buffer = HWIO_IN(MPMC_DY_RAS_CAS2);
   buffer <<= MPM_REG2_SDRAM_MPMC_DY_RAS_CASn_SHFT;
   HWIO_OUTMI(MPM_REGn_FILE,2,MPM_REG2_SDRAM_MPMC_DY_RAS_CASn_MASK,buffer);
   break;

   case BOOT_WARMBOOT_SDRAM_CHIPSELECT_1: 

   buffer = HWIO_IN(MPMC_DY_CONFIG3);
   buffer <<= MPM_REG2_SDRAM_MPMC_DY_CONFIGn_SHFT;
   HWIO_OUTMI(MPM_REGn_FILE,2,MPM_REG2_SDRAM_MPMC_DY_CONFIGn_MASK,buffer);
  
   buffer = HWIO_IN(MPMC_DY_RAS_CAS3);
   buffer <<= MPM_REG2_SDRAM_MPMC_DY_RAS_CASn_SHFT;
   HWIO_OUTMI(MPM_REGn_FILE,2,MPM_REG2_SDRAM_MPMC_DY_RAS_CASn_MASK,buffer);
   break;

   case BOOT_WARMBOOT_SDRAM_CHIPSELECT_2: 

   buffer = HWIO_IN(MPMC_DY_CONFIG0);
   buffer <<= MPM_REG2_SDRAM_MPMC_DY_CONFIGn_SHFT;
   HWIO_OUTMI(MPM_REGn_FILE,2,MPM_REG2_SDRAM_MPMC_DY_CONFIGn_MASK,buffer);
  
   buffer = HWIO_IN(MPMC_DY_RAS_CAS0);
   buffer <<= MPM_REG2_SDRAM_MPMC_DY_RAS_CASn_SHFT;
   HWIO_OUTMI(MPM_REGn_FILE,2,MPM_REG2_SDRAM_MPMC_DY_RAS_CASn_MASK,buffer);
   break;

   case BOOT_WARMBOOT_SDRAM_CHIPSELECT_3: 

   buffer = HWIO_IN(MPMC_DY_CONFIG1);
   buffer <<= MPM_REG2_SDRAM_MPMC_DY_CONFIGn_SHFT;
   HWIO_OUTMI(MPM_REGn_FILE,2,MPM_REG2_SDRAM_MPMC_DY_CONFIGn_MASK,buffer);
   buffer = HWIO_IN(MPMC_DY_RAS_CAS1);
   buffer <<= MPM_REG2_SDRAM_MPMC_DY_RAS_CASn_SHFT;
   HWIO_OUTMI(MPM_REGn_FILE,2,MPM_REG2_SDRAM_MPMC_DY_RAS_CASn_MASK,buffer);

   break;

   default:
     ERR_FATAL("WARMBOOT SDRAM CHIPSELECT is UNKNOWN",0,0,0);
   break;
 }/* switch */

  /* Backup MPMC_DY_CNTL*/ 
 buffer = HWIO_IN(MPMC_DY_CNTL);                     
 buffer <<= MPM_REG1_SDRAM_MPMC_DY_CNTL_SHFT;
 HWIO_OUTMI(MPM_REGn_FILE,1,MPM_REG1_SDRAM_MPMC_DY_CNTL_MASK,buffer);
 return;

}/* boot_warmboot_sdram_mao_backup */

/*===========================================================================

FUNCTION  BOOT_WARMBOOT_PSRAM_MAO_BACKUP

DESCRIPTION
   This function performs PSRAM always-on-domain backup. 
DEPENDENCIES
   PBL relies on always-on domain to recover PSRAM later. The layout is done in PBL.
   Please refer to the comment of this file for layout of always-on domain. 
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void boot_warmboot_psram_mao_backup( 
	boot_flash_mem_cfg_type warmboot_mem/* indicate which memory configuration */ 
)
{

 uint32 buffer; /* temporary storage */
 uint32 i; /* Index to loop through MPM_REGn_FILE */
 uint32 j; /* Index to loop through MPM_REGn_FILE */
 uint32 gpio_k=0x49;/* GPIO index */
 uint32 which_cs;/* which chip select we are using */
 
 /* GPIO mask used store GPIO settings in always-on domain */
 uint32 mpm_reg4_gpio_mask_array[12]= {
 MPM_REG4_PSRAM_GPIO_PAGE0_MASK, 
 MPM_REG4_PSRAM_GPIO_PAGE0_SHFT, 
 MPM_REG4_PSRAM_GPIO_CFG0_MASK,
 MPM_REG4_PSRAM_GPIO_CFG0_SHFT,
 MPM_REG4_PSRAM_GPIO_PAGE1_MASK,
 MPM_REG4_PSRAM_GPIO_PAGE1_SHFT,
 MPM_REG4_PSRAM_GPIO_CFG1_MASK,
 MPM_REG4_PSRAM_GPIO_CFG1_SHFT,
 MPM_REG4_PSRAM_GPIO_PAGE2_MASK,
 MPM_REG4_PSRAM_GPIO_PAGE2_SHFT,
 MPM_REG4_PSRAM_GPIO_CFG2_MASK,
 MPM_REG4_PSRAM_GPIO_CFG2_SHFT
 }; 
 /* GPIO mask used store GPIO settings in always-on domain */
 uint32 mpm_reg5_gpio_mask_array[16]= {
 MPM_REG5_PSRAM_GPIO_PAGE0_MASK, 
 MPM_REG5_PSRAM_GPIO_PAGE0_SHFT, 
 MPM_REG5_PSRAM_GPIO_CFG0_MASK,
 MPM_REG5_PSRAM_GPIO_CFG0_SHFT,
 MPM_REG5_PSRAM_GPIO_PAGE1_MASK,
 MPM_REG5_PSRAM_GPIO_PAGE1_SHFT,
 MPM_REG5_PSRAM_GPIO_CFG1_MASK,
 MPM_REG5_PSRAM_GPIO_CFG1_SHFT,
 MPM_REG5_PSRAM_GPIO_PAGE2_MASK,
 MPM_REG5_PSRAM_GPIO_PAGE2_SHFT,
 MPM_REG5_PSRAM_GPIO_CFG2_MASK,
 MPM_REG5_PSRAM_GPIO_CFG2_SHFT,
 MPM_REG5_PSRAM_GPIO_PAGE3_MASK,
 MPM_REG5_PSRAM_GPIO_PAGE3_SHFT,
 MPM_REG5_PSRAM_GPIO_CFG3_MASK,
 MPM_REG5_PSRAM_GPIO_CFG3_SHFT,
 };

  /* Backup GPIOs; We put GPIO settings into MSM_REG4_FILE and MSM_REG5_FILE 
  */
  if(warmboot_mem == BOOT_NOR_16_PSRAM_32_PRESENT) 
  {
    /* BACKUP GPIO starts from GPIO73 */
    gpio_k= 0x49;
  }
  else if (warmboot_mem == BOOT_NOR_16_PSRAM_16_PRESENT) 
   {
    /* BACKUP GPIO starts from GPIO75 */
    gpio_k= 0x4B;
  }
  else
  {
    ERR_FATAL("WARM BOOT OPERATION TYPE is UNKNOWN",0,0,0);
  } /* end of if */
  /* Backup GPIO to MPM_REG4_FILE */
  for(j=0;((j<12) &&(gpio_k<=0x4B));j+=4,gpio_k++)
  {
    /* check to see if we need to write to this GPIO */
    HWIO_OUTMI(MPM_REGn_FILE,4,mpm_reg4_gpio_mask_array[j+2],(0x2<<mpm_reg4_gpio_mask_array[j+3]));
    HWIO_OUTMI(MPM_REGn_FILE,4,mpm_reg4_gpio_mask_array[j],(gpio_k <<mpm_reg4_gpio_mask_array[j+1]));
  }/* for j */

  /* Backup GPIO to MPM_REG5_FILE */
   for(i=3;((i<6) &&(gpio_k<0x50));i++) 
   {
     for(j=0;((j<16) && (gpio_k<0x50));j+=4,gpio_k++)
     {
       HWIO_OUTMI(MPM_REGn_FILE,5,mpm_reg5_gpio_mask_array[j+2],(0x1<<mpm_reg5_gpio_mask_array[j+3]));
       HWIO_OUTMI(MPM_REGn_FILE,5,mpm_reg5_gpio_mask_array[j],(gpio_k <<mpm_reg5_gpio_mask_array[j+1]));
     }/* for j */
   }/* for i */
  
   /* Backup EBI1_STDY_SEL */
   buffer = HWIO_IN(EBI1_STDY_SEL);
   buffer <<= MPM_REG2_PSRAM_EBI1_STDY_SEL_SHFT;
   HWIO_OUTMI(MPM_REGn_FILE,2,MPM_REG2_PSRAM_EBI1_STDY_SEL_MASK,buffer);

   /* Backup EBI1_MEM_CTLR_MODE */
   buffer = HWIO_IN(EBI1_MEM_CTLR_MODE);
   buffer <<= MPM_REG2_PSRAM_EBI1_MEM_CTLR_MODE_SHFT;
   HWIO_OUTMI(MPM_REGn_FILE,2,MPM_REG2_PSRAM_EBI1_MEM_CTLR_MODE_MASK,buffer);

  /* Backup PAD_HDRIVE_MSEL_1 */
   buffer = HWIO_IN(PAD_HDRIVE_MSEL_1);
   buffer <<= MPM_REG2_PSRAM_PAD_HDRIVE_MSEL_1_SHFT;
   HWIO_OUTMI(MPM_REGn_FILE,2,MPM_REG2_PSRAM_PAD_HDRIVE_MSEL_1_MASK,buffer);

   /* Backup GPIO_PAD_HDRIVE_MSEL_9 */
   buffer = HWIO_IN(GPIO_PAD_HDRIVE_MSEL_9);
   HWIO_OUTI(MPM_REGn_FILE,6,buffer);

  /* Backup EBI1_CFG */
   buffer = HWIO_IN(EBI1_CFG);
   HWIO_OUTI(MPM_REGn_FILE,3,buffer);

   /* Get this one from cfg_data item */
   which_cs =boot_get_feature_cfg_item (CFG_ITEM_WARMBOOT_RAM_CHIP_SELECT);
   which_cs <<= MPM_REG2_PSRAM_CHIPSELECT_SHFT;
  /* Check which chip select we are using */
  
   HWIO_OUTMI(MPM_REGn_FILE,2,MPM_REG2_PSRAM_CHIPSELECT_MASK,which_cs);

  /* Backup EBI1_CSn_CFG0 */ 
   buffer = HWIO_INI(EBI1_CSn_CFG0,which_cs);
   HWIO_OUTI(MPM_REGn_FILE,0,buffer);

  /* Backup EBI1_CSn_CFG1 */
   buffer = HWIO_INI(EBI1_CSn_CFG1,which_cs);
   HWIO_OUTI(MPM_REGn_FILE,1,buffer);
}/* boot_warmboot_psram_mao_backup */

/*===========================================================================

FUNCTION  BOOT_WARMBOOT_BACKUP

DESCRIPTION
 This function performs warm boot backup operation before power collapse 
 happens.  It performs memory settings backup. 
 For SDRAM,  we save MPMC and EBI settings into both always-on-domain and local
 buffer located in SDRAM.
 For PSRAM, we save EBI settings into always-on-domain. 
  

DEPENDENCIES
  For SDRAM,
  The warm boot backup function is always located in RAM or cache since we will.  
  put SDRAM into self-refresh mode.
  For PSRAM,
  The warm boot backup function is always located in PSRAM since it won't get 
  reset.
  The layout of always-on-domain cannot be changed since it's in PBL.Please
  refer to the comments of this file to understand the layout.
  
RETURN VALUE
  None

SIDE EFFECTS
  This function never returns.

===========================================================================*/
void boot_warmboot_backup(
    boot_flash_mem_cfg_type warmboot_mem,/* Indicate which memory configuation*/
    uint32 *warmboot_storage_ptr,/* where to store the local data for EBI1
	                             ** and MPMC.*/
    boot_warmboot_func_ptr_type warmboot_handler_ptr /* After PBL, we 
	                                               will jump to this address */
)
{
   uint32 sdram_present=TRUE; /* indicate whether it's SDRAM or not */
  /* We first check to see it's SDRAM or PSRAM */
  /* For SDRAM,We store the required information into always-on-domain first. 
  ** Then, PBL can use those information to recover memory settings as soon as 
  ** possible.
  ** Then, we backup the rest of memory settings into local SDRAM storage buffer. 
  ** For PSRAM, we backup the memory settings into local PSRAM storage buffer.
  */

  if((warmboot_mem == BOOT_NAND_SDRAM_16_PRESENT)    || 
     (warmboot_mem == BOOT_NAND_SDRAM_32_PRESENT)    || 
	 (warmboot_mem == BOOT_NOR_16_SDRAM_16_PRESENT)  ||
	 (warmboot_mem == BOOT_NOR_16_SDRAM_32_PRESENT))
  {
	  sdram_present = TRUE;
  }
  else if ((warmboot_mem == BOOT_NOR_16_PSRAM_16_PRESENT) ||
		   (warmboot_mem == BOOT_NOR_16_PSRAM_32_PRESENT))
  {

	  sdram_present = FALSE;
  }
  else
  {
	 ERR_FATAL("WARM BOOT OPERATION TYPE is UNKNOWN",0,0,0);
  }/* end of if */
  /* We backup the return 32 bit return address after power collapse */
  HWIO_OUTI( MPM_REGn_FILE, 7 ,(uint32)warmboot_handler_ptr);
  /* Backup the RAM type */
  HWIO_OUTMI(MPM_REGn_FILE,2,MPM_REG2_SDRAM_PRESRENT_MASK,
			 (sdram_present << MPM_REG2_SDRAM_PRESRENT_SHFT));
  /* Check to see whether it's SDRAM backup or PSRAM backup */
  if(sdram_present)
  {
    /* We first backup sdram settings into always-on-domain to keep the basic
	** SDRAM settings to recover later by PBL.
	*/
    boot_warmboot_sdram_mao_backup(warmboot_mem);
  }
  else 
  {
    /* We first backup EBI1 settings into always-on-domain to keep the basic
	** PSRAM settings to recover later by PBL.
	*/
    boot_warmboot_psram_mao_backup(warmboot_mem);
  }/* end of if */
  /* Then ,we back up the reset of settings into SDRAM or PSRAM local storage
  ** buffer to finish the SDRAMor NOR settings recovery later.
  */
  boot_warmboot_local_backup(warmboot_mem,warmboot_storage_ptr);
}/* boot_warmboot_backup */

/*===========================================================================

FUNCTION  BOOT_WARMBOOT_RESTORE

DESCRIPTION
 This function performs warm boot restore operation after power collapse 
 happens and PBL is executed.   
 PBL does partial recovery to ensure we can at least access SDRAM or PSRAM.
 this function will continue the memory recovery to ensure we can be back to 
 optimal stage.
 For SDRAM,  we retore MPMC and EBI settings from local buffer located in SDRAM.
 For PSRAM, we restore NOR back to Burst mode if needed. 
  

DEPENDENCIES
  Depends on how we recover in PBL.  Once we reach to this function, it means
  we can at least run something from SDRAM or PSRAM.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void boot_warmboot_restore(
	boot_flash_mem_cfg_type warmboot_mem, /* Indicate which memory configuation*/
    uint32 *warmboot_storage_ptr /* where to store the local data for EBI1
	                             ** and MPMC.*/
)
{
	/* Perform NOR flash restore */
	if ((warmboot_mem == BOOT_NOR_16_PSRAM_16_PRESENT) ||
	    (warmboot_mem == BOOT_NOR_16_PSRAM_32_PRESENT) || 
		(warmboot_mem == BOOT_NOR_16_SDRAM_16_PRESENT) ||
		(warmboot_mem == BOOT_NOR_16_SDRAM_32_PRESENT))
	{
		boot_warmboot_nor_restore();
	}
    /* Then restore memory settings from local buffer */
	boot_warmboot_local_restore(warmboot_storage_ptr);

}/* boot_warmboot_restore */

/*===========================================================================

FUNCTION  BOOT_WARMBOOT_HANDLER

DESCRIPTION
   This function performs memory settings backup and restore. 

 For SDRAM, we put it into self refresh mode to keep the content.
 For PSRAM, it won't get reset after MSM is reset.
 PBL will do minial configuration to allow customers warm boot handler to be
 continue.
  
 Parameter1: Get it from configuration data to indicate which memory configuration  
             we are using.
 Parameter2: Indicate which operation we are using. Either Backup or Restore. 
 Parameter3: Indicate the local storage buffer to backup and restore settings
             not done in PBL. If user doesn't want to have local buffer, they 
			 can put NULL here.
 Parameter4: Location of where the PBL will jump to once it finishes the minimal
             memory Configuration recovery.

DEPENDENCIES
  None. 
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void boot_warmboot_handler(
	boot_flash_mem_cfg_type warmboot_mem, /* Indicate which memory configuation*/
    boot_warmboot_opt_type warmboot_opt, /* Restore or Backup */
    uint32 *warmboot_storage_ptr,/* where to store the local data for EBI1 
	                             ** and MPMC.*/
    boot_warmboot_func_ptr_type  warmboot_handler_func_ptr /* After PBL, we 
	                                               will jump to this address */
	     
)

{
   
  if (warmboot_opt == WARMBOOT_OPT_BACKUP) 
  {
    boot_warmboot_backup(warmboot_mem,
						 warmboot_storage_ptr,
						 warmboot_handler_func_ptr);
  }
  else if (warmboot_opt == WARMBOOT_OPT_RESTORE) 
  {
	boot_warmboot_restore(warmboot_mem,warmboot_storage_ptr);
  }
  else
  {
	 ERR_FATAL("WARM BOOT OPERATION TYPE is UNKNOWN",0,0,0);
  }
}/* boot_warmboot_handler */

