#ifndef __SDCC_BOOT_INIT_H
#define __SDCC_BOOT_INIT_H
/**********************************************************************
 * sdcc_boot_init.h
 *
 * This file provides SDCC Boot Init API definitions.
 *
 * Copyright (c) 2009 Qualcomm Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 **********************************************************************/
/*======================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //source/qcom/qct/core/storage/sdcc/rel/02/src/sdcc_boot_init.h#1 $
  $DateTime: 2009/03/30 22:18:39 $ $Author: spuliven $

when           who    what, where, why
----------     ---    -----------------------------------------------
2009-03-19     sp     Initial Revision.
==================================================================*/

/*==================================================================

                              FUNCTION DEFINITIONS

==================================================================*/

/*******************************************************************
 * FUNCTION      sdcc_boot_init
 *
 * DESCRIPTION   This function performs the basic initialization of
 *               GPIO, PMIC, CLK & SDCC controller so as to drive
 *               the SDCC lines such that any un-wanted 
 *               glitches/assertions are eliminated at boot time
 *
 * DEPENDENCIES  None
 *
 * PARAMETERS    void
 *
 * RETURN VALUE  void
 *
 * SIDE EFFECTS  None
 *
 *******************************************************************/
void sdcc_boot_init (void);

#endif /* __SDCC_BOOT_INIT_H */
