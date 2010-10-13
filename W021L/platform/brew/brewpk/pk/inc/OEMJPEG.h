#ifndef  OEMJPEG_H
#define  OEMJPEG_H
/*=================================================================================
FILE:          OEMJPEG.h

SERVICES:      OEM JPEG Interface

DESCRIPTION:   Header file to expose OEM JPEG functions.

PUBLIC CLASSES:   Not Applicable

INITIALIAZTION AND SEQUENCEING REQUIREMENTS: Not Applicable

        Copyright © 2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=================================================================================*/
/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //depot/asic/qsc60x0/apps/brew_3.1.5/pk/inc/OEMJPEG.h#1 $
 $DateTime: 2006/01/20 14:40:56 $
 $Author: chonda $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 05/10/05  JN     Added OEMJPEG_SetExifAsciiValue().
 05/02/05  JN     Defined Exif tag type.
============================================================================*/

typedef enum
{
  GPS_LATITUDE_REF              = 0x0001,
  GPS_LATITUDE                  = 0x0002,
  GPS_LONGITUDE_REF             = 0x0003,
  GPS_LONGITUDE                 = 0x0004,
  GPS_ALTITUDE_REF              = 0x0005,
  GPS_ALTITUDE                  = 0x0006,
  GPS_PROCESS_METHOD            = 0x001B,
  IMAGE_DESCRIPTION             = 0x010e,
  EXIF_MAKE                     = 0x010f,
  EXIF_MODEL                    = 0x0110,
  SOFTWARE                      = 0x0131,
  EXIF_VERSION                  = 0x9000,
  EXIF_DATE_TIME_ORIGINAL       = 0x9003,
  EXIF_DATE_TIME_DIGITIZED      = 0x9004,
  FLASH                         = 0x9209,
  EXIF_USER_COMMENT             = 0x9286,
  WHITE_BALANCE                 = 0xA403,
} OEMJPEG_exif_tag_id_type;

// Function prototypes
void  * OEMJPEG_GetImageData(IImage * po);
boolean OEMJPEG_extractThumbnail(IImage * po, uint8 **outPtr, uint32 *out_buf_size);
void*   OEMJPEG_GetExifInfo(IImage *po);
boolean OEMJPEG_GetExifValue(IImage * po, uint16 tag_id, void** data, uint32* len);
void    OEMJPEG_SetExifAsciiValue(IImage * po, uint16 tag_id, void* data, uint32 len);

#endif /* OEMJPEG_H */

