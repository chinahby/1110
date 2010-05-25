#ifndef ERR_RLE_H
#define ERR_RLE_H

/*===========================================================================

                    Error Run Length Encoding Header File

Description
  

Copyright (c) 1992-2004 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                           Edit History

$Header: //depot/asic/msmshared/services/err/err_rle.h#1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/07/04   tbg     Initial check in of file.  Part of FEATURE_ERR_SAVE_CORE.
===========================================================================*/

#include "comdef.h"

#ifdef CUST_H
#include "customer.h"
#endif

#ifdef FEATURE_PMIC_RTC
#include "clkrtc.h"
#endif

#include "rexlog.h"

/* File names used to store RLE info to EFS */
#define ARM_DATA_LOAD_FILENAME "err/arm_data_load.cmm"

#define MDSP_RAMA_FILENAME "err/mdsp_rama_dump.lst"
#define MDSP_RAMB_FILENAME "err/mdsp_ramb_dump.lst"
#define MDSP_RAMC_FILENAME "err/mdsp_ramc_dump.lst"
#define ARM_APP_RAM_FILENAME   "err/arm_app_ram_dump.rle"
#define ARM_BB_RAM_FILENAME    "err/arm_bb_ram_dump.rle"
#define ARM_ZI_REGION_FILENAME "err/arm_zi_region_ram_dump.rle"
#define ARM_OEM_HEAP_REGION_FILENAME "err/arm_oem_heap_ram_dump.rle"

/* RLE escape characters */
#define RLE_ESCAPE  0x16BEEF29
#define RLE_ESCAPED 0x00000000

/* Magic number to indicate whether there's a useful core that should be saved to EFS */
#define ERR_CORE_STORE_TO_FLASH_MAGIC (uint64)0x823DABD542410A0CULL

/* This is the maximum size of the compressed core in 32-bit words */
#define ERR_MAX_CORE_WORDS 0x80000 /* (2MB Buffer) */

typedef struct {
  /* Magic word to know whether to store the core dump to flash */
	err_data_flag_type store_to_flash;

  /* Local copy of rex_core for creating .cmm file */
  LOG_COREDUMP_C_type rex_core;
  
#ifdef FEATURE_PMIC_RTC
  clk_julian_type error_time;
#endif
  
  /* Statistical information */
  uint32 total_words;
  uint32 total_input_words;
  uint32 total_output_words;
  
  /* Pointers and lengths to RLE core data */
  uint32 *app_ram_core_ptr;
	uint32  app_ram_core_len;
	uint32 *bb_ram_core_ptr;
	uint32  bb_ram_core_len;
	uint32 *zi_region_core_ptr;
	uint32  zi_region_core_len;
#ifdef FEATURE_ERR_SAVE_CORE_OEM_HEAP
	uint32 *oem_heap_core_ptr;
#endif
	uint32  oem_heap_core_len;  

	/* run-length-encoded core */
	uint32 rle_core[ERR_MAX_CORE_WORDS];  

} err_core_dump_type;
  
extern uint32* oem_dynamic_heap_pointer;
extern uint32 oem_dynamic_heap_size;

/* -------------------------------------------------------------------------
   Function Definitions
   ------------------------------------------------------------------------- */
void err_save_ram_core_rle(const char *filename, unsigned int line);  

#endif /* ERR_STORE_H */
