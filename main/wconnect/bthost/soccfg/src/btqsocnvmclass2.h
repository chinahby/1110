#ifndef _BT_QSOC_NVM_BTS_CLASS2_SUPPORT_H
#define _BT_QSOC_NVM_BTS_CLASS2_SUPPORT_H 

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

       B L U E T O O T H    B T S    N V M   C L A S S 2  S U P P O R T H E A D E R  F I L E

GENERAL DESCRIPTION
  This module contains extern declaration of NVM strings. 
  
EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

     Copyright (c) 2009-2011 QUALCOMM Incorporated. 
All Rights Reserved. 
     Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order. Please
  use ISO format for dates.

  $Header: //source/qcom/qct/wconnect/bthost/soccfg/rel/00.00.26/src/btqsocnvmclass2.h#11 $
  $DateTime: 2011/01/17 23:23:57 $
  $Author: roystonr $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2010-10-25   tw Added support for 4025 B3
  2010-12-03   av  4025 B2 NVM update
  2010-07-26   rr  4025 B2 NVM update.
  2010-07-05   rr  4025 B2 NVM update.
  2010-04-27   rr  4025 B2 NVM update.
  2010-03-10   rr  Updated 4025 B2. ROM patch for CR#189653 - pabBT Scatternet: AG(master)+FTP(slave)+DUNP (master/slave) concurrent connections fails.
  2009-11-10  dgh  Updated NVM version for 4025 B2.
  2009-08-27   jn  Updated NVM version for 4025 B1.
  2009-08-07  dgh  Added support for 4025 B2.
  2009-07-07   jn  Updated NVM version for 4025 B1.
  2009-05-19   sa  Updated patch for 4025 B1 (19.2 & 32 MHz).
  2009-04-24   sa  Updated patch for 4025 B1 (19.2 & 32 MHz).
  2009-02-13   sa  Updated patch for 4025 B1 (32 MHz only).

*   #1         02 Feb  2009          SA
*   Created new file to support class 2 nv cmds for 4025 B1.

===========================================================================*/

/*===========================================================================

                         INCLUDE FILES FOR MODULE

===========================================================================*/

/*=========================================================================*/
/*                               TYPEDEFS                                  */
/*=========================================================================*/


/*===========================================================================
                                 Globals
===========================================================================*/


static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_32_19P2MHz_4025_b1_CLASS2[] = \
{
  0x05, 0x01, 0x20, 0x02, 
  0x0D, 0x17    // version 17.0D
};

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_32_32MHz_4025_b1_CLASS2[] = \
{
  0x05, 0x01, 0x20, 0x02, 
  0x0E, 0x1D    // version 1D.0E
};

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_32_19P2MHz_4025_b2_CLASS2[] = \
{
  0x05, 0x01, 0x20, 0x02, 
  0x07, 0x17    // version 17.07
};

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_32_32MHz_4025_b2_CLASS2[] = \
{
  0x05, 0x01, 0x20, 0x02, 
  0x07, 0x1D    // version 1D.07
};

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_32_19P2MHz_4025_b3_CLASS2[] = \
{
  0x05, 0x01, 0x20, 0x02, 
  0x00, 0x18    // version 18.00
};

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_32_32MHz_4025_b3_CLASS2[] = \
{
  0x05, 0x01, 0x20, 0x02, 
  0x00, 0x19    // version 19.00
};

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_36_4025_CLASS2[] = \
{
   0x26, 0x01, 0x24, 0x23,
   0x07, 0x07, 0x07, 0x02, 0x07, 0x00, 0x01, 0x02, 
   0x03, 0x04, 0x05, 0x06, 0x07, 0x07, 0x07, 0x00, 
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 
   0x02, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 
   0x07, 0x08, 0x09
};

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_41_19P2MHz_4025_CLASS2[] = \
{
  0x29, 0x01, 0x29, 0x26,
  0x11, 0x00, 0x00, 0x00, 0x42, 0x33, 0xA5, 0x31, 
  0xB1, 0x07, 0xC1, 0x70, 0x1C, 0xFA, 0x1D, 0x04, 
  0x1E, 0x1F, 0x20, 0x6C, 0x3D, 0x67, 0x3E, 0xC9, 
  0xA7, 0x64, 0xAA, 0x42, 0xAB, 0x94, 0xAC, 0x8F, 
  0xA6, 0x27, 0xA8, 0x00, 0xCF, 0x04
};

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_41_32MHz_4025_CLASS2[] = \
{
  0x2B, 0x01, 0x29, 0x28,
  0x12, 0x00, 0x00, 0x00, 0x42, 0x33, 0xA5, 0x31, 
  0xB1, 0x07, 0xC1, 0x70, 0x1C, 0xFA, 0x1D, 0x04, 
  0x1E, 0x1F, 0x20, 0x6C, 0x3D, 0x67, 0x3E, 0xC9, 
  0xA7, 0x64, 0xAA, 0x42, 0xAB, 0x94, 0xAC, 0x8F, 
  0xA6, 0x27, 0xA8, 0x00, 0xCF, 0x04, 0x5B, 0x06
};

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_56_4025_CLASS2[] = \
{
  0x04, 0x01, 0x38, 0x01,
  0x04
};

#endif /* _BT_QSOC_NVM_BTS_CLASS2_SUPPORT_H  */
