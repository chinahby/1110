#ifndef QDSPIMAGE_H
#define QDSPIMAGE_H
/*===========================================================================

                   Q D S P   D O W N L O A D   D R I V E R
           I M A G E   D E F I N I T I O N   H E A D E R   F I L E

DESCRIPTION
  This header file contains the data types required to:
  1) Define the firmware images for the QDSP.
  2) Access the variables defined in each image.

REFERENCES
  None

Copyright (c) 2000 - 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
   
$Header: //source/qcom/qct/multimedia/core/qdspservices/6k/ver1/qdsputils/main/latest/src/qdspimage.h#3 $
$DateTime: 2008/05/20 10:10:08 $ $Author: vparasar $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/30/07   act     Added DME table size to qdspext_image_type.
02/02/07    sg     Added support for setting user-defined dsp memory
                   accessor functions.
08/22/06    sg     Made some data structures non-const to allow run-time
                   configuration of the DSP base address.
08/25/04    sg     Added support for downloading modules.
06/01/03    sg     Eliminated support for indirect access memory banks (RAMCA
                   and RAMCB). Removed unused declarations.
02/18/03    sg     Increased mem_share_bank size to uint32.
03/12/02    sg     Updated for new memshare interface.
                   Added support for DME data.
02/05/02    sg     Added support for memory 'sharing'.
01/25/02    sm     Added ramc and rami in qdspext_image_type.
10/30/01    ro     New types for, and added fw_ctrl_func to qdspext_data_type.
01/25/01    ro     Added reg_offset to qdspext_data_type.
11/17/00    ro     Modifed qdspext_data_type and added qdspext_image_set_type
                   to support addition of a secondary image set.
11/06/00    ro     First version.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"     /* Definitions for byte, word, etc.                */

/* <EJECT> */
/*===========================================================================

                         DATA DECLARATIONS

===========================================================================*/

/* Word sizes used by DSP memory banks */
typedef enum
{
  QDSP_WORD_SIZE_16BIT,
  QDSP_WORD_SIZE_32BIT,
  QDSP_WORD_SIZE_MAX
} qdsp_word_size_type;

/* Memory descriptor type definitions */
typedef struct
{
  /* Starting ARM address of segment */
  uint32 start_addr;
  /* Number of entries in segment */
  const uint32 length;
} qdspext_segment_type;

/* This type contains the data for one 16-bit memory bank */
typedef struct
{
  /* 16-bit RAM image data */
  const uint16* image_ptr;
  /* List of segments */
  qdspext_segment_type* segment_list;
  /* Number of segments in RAM image */
  const uint16 num_segments;
} qdspext_ram_image_16bit_type;

/* This type contains the data for one 32-bit memory bank */
typedef struct
{
  /* 32-bit RAM image data */
  const uint32* image_ptr;
  /* List of segments */
  qdspext_segment_type* segment_list;
  /* Number of segments in RAM image */
  const uint16 num_segments;
} qdspext_ram_image_32bit_type;

typedef uint32 qdsp_dsp_memconfig_type;

/* This type contains the data for one fragment */
typedef struct
{
  /* DSP bank word size */
  const qdsp_word_size_type word_size;
  /* Number of apps currently using this fragment */
        byte                usage_count;
  /* Size of the fragment (in DSP words) */
  const uint32              size;
  /* (ARM based) DSP address to which the fragment should be downloaded */
  const uint32              dest_addr;
  /* Address of the table where the data is stored */
  const void*               src_addr;
  /* TRUE if the fragment is currently downloaded */
  boolean downloaded;
  /* Don't re-download if TRUE */
  boolean no_redownload;
} qdsp_fragment_type;

/* This type contains all of the data in a single QDSP firmware image */
typedef struct
{
        void*                            dma_table;
  const uint32                           num_symbols;
  const qdsp_fragment_type***            overlay_table;
  qdspext_ram_image_16bit_type*          rama_image_ptr;
  qdspext_ram_image_16bit_type*          ramb_image_ptr;
  qdspext_ram_image_32bit_type*          ramc_image_ptr;
  qdspext_ram_image_32bit_type*          rami_image_ptr;
  const void*                            dme_rom_ptr;
  const uint32                           dme_rom_size;
  const void*                            dme_ram_ptr;
  const uint32                           dme_ram_size;
  const qdsp_dsp_memconfig_type          mem_share_bank; /* DSP memory mode */
} qdspext_image_type;

/* Definition of the function type and control parameter used
** to put the DSP into run or download mode
*/
typedef enum {
  QDSPEXT_FW_RUN,
  QDSPEXT_FW_DOWNLOAD
} qdspext_fw_ctrl_type;

/* Function type used to control the DSP memory modes */
typedef void (* qdspext_memshare_ctrl_func_type) (
  const qdsp_dsp_memconfig_type mem_share_mask
);

/* Definition of the function type and memory type
** to allow the DSP to perform DME operations
*/
typedef enum {
  QDSPEXT_DME_ROM,
  QDSPEXT_DME_RAM
} qdspext_dme_ctrl_type;

/* Function pointer used to configure the base addresses of extram sections 
 * in firmware.
 */
typedef void (* qdspext_dme_ctrl_func_type)
(
  const qdspext_dme_ctrl_type ext_type,
  const void* dme_data_ptr
);

/* Function pointer type for allowing clients to register a user-implemented
 * version of the qdsp_memcpy() - useful if there are HW differences in the
 * DSP memory architecture.
 */
typedef void (* qdspext_memcpy_usr_fn_type)
(
  /* Destination address */
  void *dest,
  /* Source address */
  void *src,
  /* Length of data to copy, in word_size words */
  uint32 len,
  /* whether a word is 16 or 32 bits long */
  qdsp_word_size_type word_size,
  /* Boolean to swap when copying to destination */
  boolean swap
);

/* Function pointer type for allowing clients to register a user-implemented
 * version of the qdsp_memvfy() - useful if there are HW differences in the
 * DSP memory architecture.
 */
typedef boolean (* qdspext_memvfy_usr_fn_type)
(
  /* Destination address */
  void *dest,
  /* Source address */
  void *src,
  /* Length of data to copy, in words */
  uint32 len,
  /* whether a word is 16 or 32 bits long */
  qdsp_word_size_type word_size,
  /* Boolean to swap when copying to destination */
  boolean swap
);

/* Function pointer used to control the RUN/RESET states of the DSP */
typedef void (* qdspext_ctrl_reg_usr_fn_type)
(
 qdspext_fw_ctrl_type fw_state
);

/* Contains User defined function pointers to address variations in DSP
 * memory architectures.
 */
typedef struct
{
  qdspext_memcpy_usr_fn_type    memcpy_usr_func;
  qdspext_memvfy_usr_fn_type    verify_and_dwnld_usr_func;
  qdspext_ctrl_reg_usr_fn_type  ctrl_reg_usr_func;
} qdspext_accessor_funcs_struct_type;

/*  This type contains data required to support one QDSP 
*/
typedef struct
{
  qdspext_image_type*       loader_image;
  uint32                    base_address; /* Start of DSP memory ARM address  */
  uint32                    rama_offset;
  uint32                    ramb_offset;
  uint32                    ramc_offset;
  uint32                    rami_offset;
  uint32                    reg_offset;   /* Offset of control registers      */
  uint32                    end_address;  /* End of DSP memory - ARM address  */
  qdspext_image_type**      image_list;

  /* Optional User defined function pointers to address variations in DSP
   * memory architectures.
   */
  qdspext_accessor_funcs_struct_type *accessor_funcs;
} qdspext_data_type;

#endif /* QDSPIMAGE_H */

