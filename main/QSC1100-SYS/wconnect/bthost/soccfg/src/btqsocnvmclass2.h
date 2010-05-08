#ifndef _BT_QSOC_NVM_BTS_CLASS2_SUPPORT_H
#define _BT_QSOC_NVM_BTS_CLASS2_SUPPORT_H 

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

       B L U E T O O T H    B T S    N V M   C L A S S 2  S U P P O R T H E A D E R  F I L E

GENERAL DESCRIPTION
  This module contains extern declaration of NVM strings. 
  
EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2009 QUALCOMM Incorporated. 
All Rights Reserved. 
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

 $Header: 
 $DateTime: 
  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
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
  0x0B, 0x17    // version 17.0B
};

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_32_32MHz_4025_b1_CLASS2[] = \
{
  0x05, 0x01, 0x20, 0x02, 
  0x0C, 0x1D    // version 1D.0C
};

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_36_4025_b1_CLASS2[] = \
{
   0x26, 0x01, 0x24, 0x23,
   0x07, 0x07, 0x07, 0x02, 0x07, 0x00, 0x01, 0x02, 
   0x03, 0x04, 0x05, 0x06, 0x07, 0x07, 0x07, 0x00, 
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 
   0x02, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 
   0x07, 0x08, 0x09
};

#endif /* _BT_QSOC_NVM_BTS_CLASS2_SUPPORT_H  */
