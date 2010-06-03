/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                 Q D S P   D O W N L O A D   D R I V E R
                         S O U R C E   F I L E           

GENERAL DESCRIPTION
   Performs the QDSP download.

   Downloads the RAMA, RAMB, RAMC and RAMI images.
   If RAMCA and RAMCB images are present, performs two-stage download. If a
   RAMC image is present, performs one-stage download with direct RAMC access.
 
EXTERNALIZED FUNCTIONS
  qdspdown_start_image
    Sets the QDSP run bit to start the currently loaded image.
  qdspdown_download_image
    Downloads an image to a QDSP.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   Clocks must be enabled before the download is started.

Copyright (c) 1999 - 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/core/qdspservices/6k/ver1/qdsputils/main/latest/src/qdspdown.c#4 $
$DateTime: 2008/09/08 00:36:21 $ $Author: vsudhir $

when       who     what, where, why
--------   ---     ---------------------------------------------
06/05/08   sjw     Change force_gsm_fragment_download() to a void return type
11/20/07    ah     Workaround to force GSM download waiting for the 
                   appropriate FW fix 
09/19/07    ah     Modified qdspdown_download_fragment() to decide whether
                   to download the fragments based on the "downloaded" flag,
                   no "force_download"
03/08/07    sg     Modified to not blindly re-download a fragment unless
                   this is explicitly requested.
02/05/07    bt     Use proper word size define.
02/02/07    sg     Added support for setting user-defined dsp memory
                   accessor functions.
03/15/05    sg     Included string.h for memcpy function.
11/29/04    sg     Added support for using memcpy for 32bit words since the ARM
                   compiler implements this using more efficient burst copies.
08/25/04    sg     Added support for downloading modules.
06/01/03    sg     Eliminated support for indirect access memory banks (RAMCA
                   and RAMCB).
03/12/02    sg     Updated with new memshare interface.
                   Added support for DME data.
02/05/02    sg     Added support for memory 'sharing'.
10/30/01    ro     Eliminate dependencies on assert, msm, err and clk.
                   Use fw_ctrl_func from qdsp data.
02/21/01    ro     Break if download verify fails in download_image_16 and 32.
01/25/01    ro     Use reg_offset in qdsp data.
11/29/00    st     Removed BT specific register writes.
11/17/00    ro     Changes to qdspdown_download_image to support
                   secondary image format changes.
11/06/00    ro     Supports new image parser:
                    - No hardcoded names (supports multiple QDSP's)
                    - Optional direct RAMC access
                   Supports simultaneous download of multiple QDSP's:
                    - Download routines are reentrant
10/04/00    sm     Code optimizations, uses qdsp_memcpy.
09/11/00    ro     Don't start the FW runnning after download.
08/28/00    ro     Use QDSP Services.
07/19/00   spf     Initial Bluetooth support.
07/14/00   spf     Improved two-stage download process to automatically
                   detect RAMC data.  Lint updates.
06/19/00   spf     Updated to use an enumeration for image selection.
06/11/00   spf     Updated references.
03/22/00   spf     Added two-stage download support.
02/11/00   spf     Added const qualifier to qdspdown_download_image()
                   image pointer parameter.
12/01/99   spf     Run-time image selection support.
11/11/99   spf     Undefined QDSPDOWN_VERIFY_DOWNLOAD.
08/26/99   spf     Pink Panther Rev2 support.
06/15/99   spf     Initial revision.

===========================================================================*/
/* <EJECT> */
/*===========================================================================

                   DOWNLOADER TEST CONFIGURATIONS

===========================================================================*/

/* If defined, each write to QDSP will be read back and verified.
**   Note: slows download by a factor of ten or more
**/
#undef QDSPDOWN_VERIFY_DOWNLOAD

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"               /* Common stuff                           */
#include <string.h>               /* Memory manipulation routines           */
#include "qdspdown.h"             /* QDSPDOWN download type definitions     */
#include "qdspimage.h"            /* QDSP firmware image definitions        */
#include "qdspmem.h"              /* QDSP Memory transfer routines          */

/* <EJECT> */
/*===========================================================================

                 DEFINITIONS AND DECLARATIONS FOR MODULE

   This section contains definitions for constants, macros, types, variables
   and other items needed by this module.

===========================================================================*/
boolean force_fragment_download = FALSE;

/* <EJECT> */
/*===========================================================================

                 F U N C T I O N   D E F I N I T I O N S

===========================================================================*/

/*===========================================================================

FUNCTION QDSPDOWN_DOWNLOAD_16BIT

DESCRIPTION
   Parameters:
   - image_ptr, pointer to the image data
   - address, starting address in memory to be written
   - length, the length of the image data

   Writes each 16-bit data value in the image pointer to by image_ptr.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   Image will be written to QDSP.

===========================================================================*/
LOCAL void qdspdown_download_16bit
(
   uint16                     *image_ptr,
   uint16                     *address,
   uint32                     length,
   qdspext_memcpy_usr_fn_type memcpy_usr_fn
)
{
  if (memcpy_usr_fn != NULL)
  {
    /* Use user defined accessor function if available */
    memcpy_usr_fn(address, image_ptr, length, QDSP_WORD_SIZE_16BIT, FALSE);
  }
  else
  {
    /* Use default memcpy otherwise */
    qdsp_memcpy(address, image_ptr, length, FALSE);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION QDSPDOWN_DOWNLOAD_32BIT

DESCRIPTION
   Parameters:
   - image_ptr, pointer to the image data
   - address, starting address in memory to be written
   - length, the length of the image data

   Writes each 32-bit data value in the image pointer to by image_ptr.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   Image will be written to QDSP.

===========================================================================*/
LOCAL void qdspdown_download_32bit
(
   uint32                     *image_ptr,
   uint32                     *dest,
   uint32                     length,
   qdspext_memcpy_usr_fn_type memcpy_usr_fn
)
{
  if (memcpy_usr_fn != NULL)
  {
    /* Use user defined accessor function if available */
    memcpy_usr_fn(dest, image_ptr, length, QDSP_WORD_SIZE_32BIT, FALSE);
  }
  else
  {
    /* Use default memcpy otherwise */
    #ifdef FEATURE_QDSP_USES_NATIVE_MEMCPY
    memcpy(dest, image_ptr, length*4);
    #else
    length++;
    while ( --length )
    {
      /* 32-bit write */
      *dest++ = *image_ptr++;
    }
    #endif
  }
}

/* <EJECT> */
#ifdef QDSPDOWN_VERIFY_DOWNLOAD
/*===========================================================================

FUNCTION QDSPDOWN_VERIFY_16BIT

DESCRIPTION
   Parameters:
   - image_ptr, pointer to the image data
   - address, starting address in memory to be read
   - length, the length of the image data

   Reads each 16-bit data value from memory and compares it against
   the image pointer to by image_ptr.

DEPENDENCIES
   None.

RETURN VALUE
   - QDSPDOWN_DOWNLOAD_COMPLETED will be returned if the verification is 
     successful. 
   - QDSPDOWN_DOWNLOAD_FAILED will be returned if the verification is not
     successful.

SIDE EFFECTS
   None.

===========================================================================*/
LOCAL qdspdown_status_type qdspdown_verify_16bit
(
   uint16                     *image_ptr,
   uint16                     *src,
   uint32                     length,
   qdspext_memvfy_usr_fn_type memvfy_usr_fn
)
{
  if (memvfy_usr_fn != NULL)
  {
    memvfy_usr_fn(image_ptr, src, length, QDSP_WORD_SIZE_16BIT, FALSE);
  }
  else
  {
    length++;
    while ( --length )
    {
      if(*image_ptr++ != *src++) return(QDSPDOWN_DOWNLOAD_FAILED);
    }
  }

  return QDSPDOWN_DOWNLOAD_COMPLETED;
}

/* <EJECT> */
/*===========================================================================

FUNCTION QDSPDOWN_VERIFY_32BIT

DESCRIPTION
   Parameters:
   - image_ptr, pointer to the image data
   - address, starting address in memory to be read
   - length, the length of the image data

   Reads each 32-bit data value from memory and compares it against
   the image pointer to by image_ptr.

DEPENDENCIES
   None.

RETURN VALUE
   - QDSPDOWN_DOWNLOAD_COMPLETED will be returned if the verification is 
     successful. 
   - QDSPDOWN_DOWNLOAD_FAILED will be returned if the verification is not
     successful.

SIDE EFFECTS
   None.

===========================================================================*/
LOCAL qdspdown_status_type qdspdown_verify_32bit
(
   uint32                     *image_ptr,
   uint32                     *src,
   uint32                     length,
   qdspext_memvfy_usr_fn_type memvfy_usr_fn
)
{

  if (memvfy_usr_fn != NULL)
  {
    memvfy_usr_fn(image_ptr, src, length, QDSP_WORD_SIZE_32BIT, FALSE);
  }
  else
  {
    /* Read image */
    while ( --length )
    {
      /* Verify */
      if(*image_ptr++ != *src++) return(QDSPDOWN_DOWNLOAD_FAILED);
    }
  }

  return QDSPDOWN_DOWNLOAD_COMPLETED;
}
#endif /* QDPSDOWN_VERIFY_DOWNLOAD */

/* <EJECT> */
/*===========================================================================

FUNCTION QDSPDOWN_DOWNLOAD_IMAGE_16BIT

DESCRIPTION
   Parameters:
   - image_desc_ptr, pointer to a 16-bit ram image descriptor.

   The image data, segment descriptor, and number of segments are extracted
   from image_desc_ptr.  Each segment is downloaded and then each segment is
   verified.

DEPENDENCIES
   None.

RETURN VALUE
   - QDSPDOWN_DOWNLOAD_COMPLETED will be returned if the download was 
     successful. 
   - QDSPDOWN_DOWNLOAD_FAILED will be returned if the download was not
     successful.

SIDE EFFECTS
   None.

===========================================================================*/
LOCAL qdspdown_status_type qdspdown_download_image_16bit
(
   const qdspext_ram_image_16bit_type* image_desc_ptr,
         qdspext_memcpy_usr_fn_type    memcpy_usr_fn,
         qdspext_memvfy_usr_fn_type    memvfy_usr_fb
)
{
   qdspdown_status_type status = QDSPDOWN_DOWNLOAD_COMPLETED;
   if (image_desc_ptr != NULL)
   {
     uint16 seg_num;
     uint32 length;
     const uint16* segment_ptr   = (uint16*)image_desc_ptr->image_ptr;
     const uint16  num_segments  = (uint16)image_desc_ptr->num_segments;
     const qdspext_segment_type* segment_list = image_desc_ptr->segment_list;

     /* Download RAM image */
     for( seg_num = 0; seg_num <= num_segments; seg_num++ )
     {
        length = segment_list[seg_num].length;
        qdspdown_download_16bit
        ( 
           (uint16*)segment_ptr, 
           (uint16*)segment_list[seg_num].start_addr,
           (uint32)length,
           memcpy_usr_fn
        );
        segment_ptr = segment_ptr + length;
     }

     #ifdef QDSPDOWN_VERIFY_DOWNLOAD
     /* Verify RAM download */
     segment_ptr = image_desc_ptr->image_ptr;

     for( seg_num = 0; seg_num <= num_segments; seg_num++ )
     {
        length = segment_list[seg_num].length;
        status = qdspdown_verify_16bit
                 ( 
                   (uint16*)segment_ptr, 
                   (uint16*)segment_list[seg_num].start_addr,
                   (uint32)length,
                   memvfy_usr_fb
                 );

        if( status != QDSPDOWN_DOWNLOAD_COMPLETED )
        {
          break;
        }

       segment_ptr = segment_ptr + length;
     }
     #endif /* QDSPDOWN_VERIFY_DOWNLOAD */
   }
   return status;
}

/* <EJECT> */
/*===========================================================================

FUNCTION QDSPDOWN_DOWNLOAD_IMAGE_32BIT

DESCRIPTION
   Parameters:
   - image_desc_ptr, pointer to a 32-bit ram image descriptor.

   The image data, segment descriptor, and number of segments are extracted
   from image_desc_ptr.  Each segment is downloaded and then each segment is
   verified.

DEPENDENCIES
   None.

RETURN VALUE
   - QDSPDOWN_DOWNLOAD_COMPLETED will be returned if the download was 
     successful. 
   - QDSPDOWN_DOWNLOAD_FAILED will be returned if the download was not
     successful.

SIDE EFFECTS
   None.

===========================================================================*/
LOCAL qdspdown_status_type qdspdown_download_image_32bit
(
   const qdspext_ram_image_32bit_type* image_desc_ptr,
         qdspext_memcpy_usr_fn_type    memcpy_usr_fn,
         qdspext_memvfy_usr_fn_type    memvfy_usr_fb
)
{
   qdspdown_status_type status = QDSPDOWN_DOWNLOAD_COMPLETED;
   if (image_desc_ptr != NULL)
   {
     uint16 seg_num;
     uint32 length;

     const uint32* segment_ptr   = image_desc_ptr->image_ptr;
     const uint16  num_segments  = (uint16)image_desc_ptr->num_segments;
     const qdspext_segment_type* segment_list = image_desc_ptr->segment_list;

     /* Download RAM image */
     for( seg_num = 0; seg_num <= num_segments; seg_num++ )
     {
        length = segment_list[seg_num].length;
        qdspdown_download_32bit
        ( 
           (uint32*)segment_ptr, 
           (uint32*)segment_list[seg_num].start_addr,
           (uint32)length,
           memcpy_usr_fn
        );
        segment_ptr = segment_ptr + length;
     }

     #ifdef QDSPDOWN_VERIFY_DOWNLOAD
     /* Verify RAM download */
     segment_ptr = image_desc_ptr->image_ptr;

     for( seg_num = 0; seg_num <= num_segments; seg_num++ )
     {
        length = segment_list[seg_num].length;
        status = qdspdown_verify_32bit
                 ( 
                   (uint32*)segment_ptr, 
                   (uint32*)segment_list[seg_num].start_addr,
                   (uint32)length,
                   memvfy_usr_fn
                 );

        if( status != QDSPDOWN_DOWNLOAD_COMPLETED )
        {
          break;
        }

        segment_ptr = segment_ptr + length;
     }
     #endif /* QDSPDOWN_VERIFY_DOWNLOAD */
   }

   return status;
}

/* <EJECT> */
/*===========================================================================

FUNCTION QDSPDOWN_DOWNLOAD_IMAGE

DESCRIPTION
  Parameters:
  - image, index to a download image descriptor
  - dsp_data, the QDSP definition data

  Performs the download of the QDSP.  The image descriptor contains
  pointers to the RAMA, RAMB, RAMC and RAMI images.

DEPENDENCIES
  - The correct QDSP clock regime must be enabled.
  - The DSP must be download (RESET) mode.

RETURN VALUE
  - QDSPDOWN_DOWNLOAD_COMPLETED will be returned if the download was 
    successful. 
  - QDSPDOWN_DOWNLOAD_FAILED will be returned if the download was not
    successful.

SIDE EFFECTS
  The QDSP will be downloaded, but the FW will not be running.

===========================================================================*/
qdspdown_status_type qdspdown_download_image (
  /* Index of the image to download */ 
  const uint32                  image,
  /* QDSP definition data */
  const qdspext_data_type       *dsp_data
)
{
   const qdspext_image_type *image_desc_ptr;
   
   if ( dsp_data != NULL &&
        dsp_data->image_list != NULL &&
        dsp_data->image_list[image] != NULL )
   {
     /* Get the image to be downloaded */
     image_desc_ptr = dsp_data->image_list[image];

     if ( /* Download RAMA */
          qdspdown_download_image_16bit
          (
            image_desc_ptr->rama_image_ptr,
            dsp_data->accessor_funcs->memcpy_usr_func,
            dsp_data->accessor_funcs->verify_and_dwnld_usr_func
          ) == QDSPDOWN_DOWNLOAD_COMPLETED
          &&
          /* Download RAMB */
          qdspdown_download_image_16bit
          (
            image_desc_ptr->ramb_image_ptr,
            dsp_data->accessor_funcs->memcpy_usr_func,
            dsp_data->accessor_funcs->verify_and_dwnld_usr_func
          ) == QDSPDOWN_DOWNLOAD_COMPLETED
          &&
          /* Download RAMC */
          qdspdown_download_image_32bit
          (
            image_desc_ptr->ramc_image_ptr,
            dsp_data->accessor_funcs->memcpy_usr_func,
            dsp_data->accessor_funcs->verify_and_dwnld_usr_func
          ) == QDSPDOWN_DOWNLOAD_COMPLETED
          &&
          /* Download RAMI */
          qdspdown_download_image_32bit
          (
            image_desc_ptr->rami_image_ptr,
            dsp_data->accessor_funcs->memcpy_usr_func,
            dsp_data->accessor_funcs->verify_and_dwnld_usr_func
          ) == QDSPDOWN_DOWNLOAD_COMPLETED
        )
     {
       return QDSPDOWN_DOWNLOAD_COMPLETED;
     }
   }
   return QDSPDOWN_DOWNLOAD_FAILED;
}

/*===========================================================================

FUNCTION SET_GSM_FRAGMENT_DOWNLOAD

DESCRIPTION
  Sets the flag when GSM is being loaded

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  Reloads GSM app everytime GSM is getting enabled

===========================================================================*/
void force_gsm_fragment_download(boolean gsm_download)
{
  if (gsm_download == TRUE)
  {
    force_fragment_download = TRUE;
  }
  else
  {
    force_fragment_download = FALSE;
  }
}

/*===========================================================================

FUNCTION QDSPDOWN_DOWNLOAD_FRAGMENT

DESCRIPTION
  Parameters:
  - fragment to download
  - Boolean that indicates whether fragment_ptr->downloaded should be ignored.
  - optional memory accessor function pointer

  Downloads a single DSP fragment.

DEPENDENCIES
  The correct QDSP clock regime must be enabled.

RETURN VALUE
  - QDSPDOWN_DOWNLOAD_COMPLETED will be returned if the download was 
    successful. 
  - QDSPDOWN_DOWNLOAD_INVALID will be returned if the download was not
    successful.

SIDE EFFECTS
  The QDSP will be downloaded, but the FW will not be running.

===========================================================================*/
qdspdown_status_type qdspdown_download_fragment (
  /* Pointer to the fragment to be loaded */
  qdsp_fragment_type         *fragment_ptr,
  qdspext_memcpy_usr_fn_type memcpy_usr_fb

)
{
  qdspdown_status_type download_status = QDSPDOWN_DOWNLOAD_COMPLETED;
  if (fragment_ptr->usage_count == 0)
  {
    if ( force_fragment_download == TRUE ||
         fragment_ptr->downloaded == FALSE )
    {
    switch (fragment_ptr->word_size)
    {
      case QDSP_WORD_SIZE_16BIT:
        qdspdown_download_16bit
        (
          (uint16*)fragment_ptr->src_addr,
          (uint16*)fragment_ptr->dest_addr,
          (uint32)fragment_ptr->size,
          memcpy_usr_fb
        );
        break;

      case QDSP_WORD_SIZE_32BIT:
        qdspdown_download_32bit
        (
          (uint32*)fragment_ptr->src_addr,
          (uint32*)fragment_ptr->dest_addr,
          (uint32)fragment_ptr->size,
          memcpy_usr_fb
        );
        break;

      default:
        download_status = QDSPDOWN_DOWNLOAD_INVALID;
        break;
    }
  }
    fragment_ptr->downloaded = TRUE;
  }
  fragment_ptr->usage_count++;
  return download_status;
}

/* End of File. */

