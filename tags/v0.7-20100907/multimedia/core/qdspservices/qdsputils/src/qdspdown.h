#ifndef QDSPDOWN_H
#define QDSPDOWN_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               Q D S P   D O W N L O A D   D R I V E R
                        H E A D E R  F I L E

DESCRIPTION
   Performs the QDSP download.

   Downloads the RAMCA, RAMCB, RAMA, RAMB and RAMI images.
   RAMCA and RAMCB are required for two-stage download only.

Copyright (c) 1999 - 2007 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/core/qdspservices/6k/ver1/qdsputils/main/latest/src/qdspdown.h#4 $
$DateTime: 2008/09/08 00:36:21 $ $Author: vsudhir $

when       who     what, where, why
--------   ---     ---------------------------------------------  
06/05/08   sjw     Change force_gsm_fragment_download() to a void return type
11/20/07    ah     Workaround to force GSM download waiting for the 
                   appropriate FW fix 
09/19/07    ah     Modified qdspdown_download_fragment() to decide 
                   whether to download the fragments based on the
                   "downloaded" flag, no "force_download"
03/08/07    sg     Modified to not blindly re-download a fragment unless
                   this is explicitly requested.
11/09/04    ro     Fixed compiler warning.
02/02/07    sg     Added support for setting user-defined dsp memory
                   accessor functions.
08/25/04    sg     Added support for downloading modules.
10/30/01    ro     Removed qdspdown_start_image().
11/17/00    ro     Changed image type to uint32 in qdspdown_download_image.
11/06/00    ro     Supports new image parser:
                    - No hardcoded names (supports more than one QDSP)
                    - Optional direct RAMC access
                   Supports simultaneous download of multiple QDSP's:
                    - Download routines are reentrant
06/19/00   spf     Updated to use an enumeration for image selection.
06/11/00   spf     Updated references.
03/22/00   spf     Added two-stage download support.
02/11/00   spf     Added const qualifier to qdspdown_download_image()
                   image pointer parameter.
12/01/99   spf     Run-time image selection support.
08/26/99   spf     Pink Panther Rev2 support.
06/15/99   spf     Initial revision.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"    /* Definitions for byte, word, etc.      */
#include "qdspimage.h" /* QDSP firmware image definitions       */

/* <EJECT> */
/*===========================================================================

                 DEFINITIONS AND DECLARATIONS FOR MODULE

   This section contains definitions for constants, macros, types, variables
   and other items needed by this module.

===========================================================================*/

typedef enum 
{
   QDSPDOWN_NOTHING_DOWNLOADED = 0,       /* Power-up state only.        */
   QDSPDOWN_BUSY_DOWNLOADING,             /* DMA download active         */
   QDSPDOWN_DOWNLOAD_COMPLETED,           /* Download completed          */
   QDSPDOWN_DOWNLOAD_FAILED,              /* Download failed             */
   QDSPDOWN_DOWNLOAD_INVALID,             /* Invalid download parameters */
   QDSPDOWN_MAX_DOWNLOAD_STATUS
} qdspdown_status_type;         

/* <EJECT> */
/*===========================================================================

                 F U N C T I O N   D E F I N I T I O N S

===========================================================================*/

/*===========================================================================

FUNCTION QDSPDOWN_DOWNLOAD_IMAGE

DESCRIPTION
  Parameters:
  - image, index to an image descriptor

  Performs the download of the QDSP.  The image descriptor contains
  pointers to RAMCA, RAMCB, RAMA, RAMB, and RAMI images.

DEPENDENCIES
  The correct QDSP clock regime must be enabled.

RETURN VALUE
  - QDSPDOWN_DOWNLOAD_COMPLETED will be returned if the download was 
    successful. 
  - QDSPDOWN_DOWNLOAD_FAILED will be returned if the download was not
    successful.

SIDE EFFECTS
  The QDSP will be downloaded, but the FW will not be running.

===========================================================================*/
extern qdspdown_status_type qdspdown_download_image (
  /* Index of the image to download */ 
  const uint32                  image,
  /* QDSP definition data */
  const qdspext_data_type       *dsp_data
);

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
extern qdspdown_status_type qdspdown_download_fragment (
  /* Pointer to the fragment to be loaded */
  qdsp_fragment_type         *fragment_ptr,
  qdspext_memcpy_usr_fn_type memcpy_usr_fb
);

/*===========================================================================

FUNCTION FORCE_GSM_FRAGMENT_DOWNLOAD

DESCRIPTION
  Sets the flag when GSM is being loaded

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  Reloads GSM app everytime GSM is getting enabled

===========================================================================*/
void force_gsm_fragment_download(boolean gsm_download);

#endif /* QDSPDOWN_H */

/* End of File. */






