/* linesize(132)
** pagesize(60)
** title("Dual Mode Subscriber Station")
** subtitle("Video Formats Services MP4 Writer")
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               V I D E O   F O R M A T S   -   M P 4   W R I T E R

GENERAL DESCRIPTION
  This module contains functions which create and write files in the MP4 file
  format.

REFERENCES
  ISO/IEC 14496, Part 1: Systems (MPEG-4)
  ISO/IEC 14496, Part 2: Visual (MPEG-4)
  ISO/IEC 14496, Part 3: Audio (MPEG-4)

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright(c) 2002-2003 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/mmservices/mpeg4fileformat/writer/main/latest/src/videofmt_mp4w.c#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/28/07   rpw     Added EVRC-B and EVRC-WB support
07/08/08   shiven  Adding support for loci atom
06/09/08   shiven  Max bit rate not working when EFS done
04/28/08   sanal   Added length field, changes for cprt atom.
11/05/07   dhaval  Added copyright atom support
07/03/07   jk      Added 'output_unit_size' for EFS throughput control.
06/19/07   kadhir  Added FTL and USB as storage media
02/26/07   jdas    Merged changes from mainline:
                   Memset the entire context->streams to zero outside the loop.
                   Modified the code to seperate the meta data stream buffer
                   to prevent any unwanted media data being mixed with meta
                   data in non-fragmented recording case.
02/19/06   vs      Merged code for support to enable/disable audio/video truncation
                   from 6550/4.5 branch.
12/18/06   gopi    Added support to enable/disable audio/video truncation
10/31/06   jdas    Made movie duration as MAX of all stream durations
08/28/06   jdas    Added few error checking code,
                   Leave cs before context is being released in error condition
07/24/06   jdas    Passed client_data in callback_ptr to fix PostEdit problem
                   in video_fmt_mp4w_write_clipinfo function
07/14/06   jdas    Added support for clipinfo stored as skip atom in recorded clip
06/20/06   Gopi    Added support for dynamically updating titl,descp
                   auth atoms.
                   Code is featurized in videoeng,cameraservices under
                   FEATURE_VIDEO_SETATOMS_DYNAMIC.
                   In videofmt support has been added as an additional API.
                   Also videofmt is feature free file hence didn't featurize
                   code.
06/05/06   jdas    Removed lint warning of "division by 0" in 
                   video_fmt_mp4w_fixup_data function
05/15/06   jdas    Removed the hard-coded profile/level from avcC atom
03/10/06   Gopi    Added support for MFRA Atom
02/01/06   jk      Changed so that videofmt does not generate the meta data
                   if space limit is reached, until the client ends videofmt.
12/23/05   ali     Fix for 3g2-frag where first few chunks have only one 
                   sample
12/09/05   rpw     Added AMR-WB and AMR-WB+ support
11/14/05   Shahed New implementation of camcorder space status error/warning msgs.
                  The new messaging does dot change the API of the videofmt.
11/11/05   Shahed Undo SPACE error/warning changes.
10/08/05   Shahed  Added support for detailed SPACE warning/error message reporting.
09/29/05   ali     Set include_titl_auth_desc member in video_fmt_mp4w_create
08/30/05   dw      Removed critical section for video_fmt_mp4w_seek
08/26/05   gopi    Fix coverity errors.
08/18/05   alin    Fix timed text related problems
06/28/05   jk      Made every I-frame in fragments the first frame of a chunk.
05/27/05   rpw     Fixed logic in estimating meta data size for streams having
                   no 'stts' table and/or no 'stss' table.
05/24/05   jk      Added support for a very long clip.
05/12/05   rpw     Added estimated 'stts' table size to size
                   limit calculation.
05/12/05   rpw     Added movie duration limit feature.
04/26/05   rpw     No longer writing empty tracks
04/01/05   sj      Added rounding to video_fmt_mp4w_convert_time_units
02/17/05   jk      Removed 'mvex' in 'moov' when no 'moof' follows.
                   Fixed space check for fragmented format.
02/07/05   rpw     Added still image support.
                   Added audio only support.
                   Added NTT DoCoMo DRM atom support.
                   Added capability of disabling interlacing.
                   Added file size limit feature.
                   Fixed file limit reached logic.
                   Now collapsing all 'mdat' atoms together when recording.
                   Added mode selection to select bitrate for AMR recording.
                   Replaced all TASKLOCK/TASKFREE with REX critical sections.
01/28/05   jk      Freed memory for footer.
01/13/05   jk      Re-implemented one sample delay to fix fragmented 3g2.
12/08/04   rpw     Added ability to read/write language code
                   from 'mdhd' atom.
                   Added ability to read width/height and origin
                   information from 'tkhd' atom.
11/22/04   jk      Fixed a bug where video buffer was used to write audio
                   frames in the last chunk.
11/18/04   jk      Updated as per the code walk.
                   Fixed a bug where chunks were not written out for non
                   master-slave streams
11/17/04   jk      Added video_fmt_mp4w_stream_regulate.
                   Updated video_fmt_mp4w_stream_seek to detect a case where
                   tail is wrapped around to 0 and head is also 0.
11/10/04   jk      Added video_fmt_mp4w_stream_reset to reset stream buffer.
                   Fixed two bugs, (1) free buffer size should not be
                   checked inside video_fmt_mp4w_write_stream in some cases,
                   (2) buffer tail shouldn't be wraped around if head has
                   been changed right before.
11/04/04   jk      Fixed two bugs that were hit respectively when
                   1) a chunk was set and it turned out it was the last chunk.
                   2) some chunk has no sample from interlaced stream.
11/03/04   jk      Added video_fmt_mp4w_seek to expose pointer to stream
                   buffer with its size to the client,
                   video_fmt_mp4w_write_footer to write fixed stream footer.
                   Implemented one sample delay before writing chunk out
                   to append fixed footer to the last sample.
09/28/04   sa      Set fixed sample size to 0 when generating stsz table
09/08/04   rpw     Added option to force 'stsz' table generation, even for
                   streams with fixed-size samples (SKM requirement).
07/15/04   rpw(hd) Added support for writing H.264 streams.
07/14/04   rpw     Added timed text encoding support.
06/25/04   ra      Commented out a condition that returns FAILURE if a stream
                   in a file is empty (Qvideophone-rashok)
06/22/04   sa      Updated video_fmt_mp4w_write_stream to replace break with
                   return.
04/21/04   rpw     Added ability to write 'udat' atoms through the
                   video_fmt_mp4w_write_uuid funtion, used to
                   write 'uuid' atoms.
04/06/04   rpw     Fixed corner case in filling stream buffer, where amount
                   left calculation returned a value one byte too large, and
                   when filling a buffer up to completely full, the buffer
                   could switch to completely empty if the head pointer was
                   precisely at the beginning of the buffer.
04/06/04   rpw     Fixed bug where optimal chunk size setting had no effect
                   on chunk generation, due to bad logic.
12/02/03   rpw     Added support for 3GPP2 QCELP (QCELPSampleEntry).
11/26/03   rpw     Fixed bug where 'mvhd' duration would not match 'tkhd'
                   duration for longest track in fragmented files, because
                   fragment was output before its duration was updated.
                   Another instance of the bug fixed in the last revision was
                   also found and fixed.
11/24/03   rpw     Fixed bug where main fragment duration would be one frame
                   short for fragmented files.
09/16/03   rpw     Added support for writing MP3 audio streams.
09/05/03   enj     Removing FEATURE_VIDEOFMT (i.e. permanently enabling it)
06/25/03   rpw     Created file, based on functionality from original
                   videoeng.c file.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"             /* Target specific definitions             */
#include "customer.h"           /* Customer specific definitions           */
#include "comdef.h"             /* Common definitions                      */

#include "videofmt_mp4.h"       /* MP4 format typedefs and prototypes      */
#include "videofmt_mp4w.h"      /* Internal MP4 writer definitions         */
#include "task.h"               /* task management services                */
#include "clk.h"                /* clock services                          */
#include "msg.h"                /* Message logging/reporting services      */
#include "assert.h"             /* assertion definitions                   */
#include <string.h>             /* Memory Manipulation routines            */
#include <stdio.h>              /* Standard I/O routines                   */
#include "AEEstd.h"

/* <EJECT> */
/*===========================================================================

                LOCAL MACROS

===========================================================================*/

#define STORE_ATOM_UINT16(buffer, offset, value) \
    (buffer) [(offset)] = (uint8) (((value) >> 8) & 0xFF); \
    (buffer) [(offset) + 1] = (uint8) ((value) & 0xFF)

#define STORE_ATOM_UINT24(buffer, offset, value) \
    (buffer) [(offset)] = (uint8) (((value) >> 16) & 0xFF); \
    (buffer) [(offset) + 1] = (uint8) (((value) >> 8) & 0xFF); \
    (buffer) [(offset) + 2] = (uint8) ((value) & 0xFF)

#define STORE_ATOM_UINT32(buffer, offset, value) \
    (buffer) [(offset)] = (uint8) (((value) >> 24) & 0xFF); \
    (buffer) [(offset) + 1] = (uint8) (((value) >> 16) & 0xFF); \
    (buffer) [(offset) + 2] = (uint8) (((value) >> 8) & 0xFF); \
    (buffer) [(offset) + 3] = (uint8) ((value) & 0xFF)

#define STORE_ATOM_ID(buffer, offset, id) \
    (buffer) [(offset)] = (id) [0]; \
    (buffer) [(offset) + 1] = (id) [1]; \
    (buffer) [(offset) + 2] = (id) [2]; \
    (buffer) [(offset) + 3] = (id) [3]

#define MAKE_ATOM_HEADER(buffer, size, type) \
    STORE_ATOM_UINT32 (buffer, 0, size); \
    STORE_ATOM_ID (buffer, 4, type)

#define MAKE_FULL_ATOM_HEADER(buffer, size, type, version, flags) \
    MAKE_ATOM_HEADER (buffer, size, type); \
    (buffer) [8] = (uint8) (version); \
    (buffer) [9] = (uint8) (((flags) >> 16) & 0xFF); \
    (buffer) [10] = (uint8) (((flags) >> 8) & 0xFF); \
    (buffer) [11] = (uint8) ((flags) & 0xFF)

#define EXTRA_PAD_FOR_SPACE_LIMIT 3 /* To make sure when space is full data gets written */
/* <EJECT> */
/*===========================================================================

                LOCAL DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
** MP4 file atom fixed parts - These arrays contain fixed data that is always
** output as part of various MP4 file atoms when saving a recorded movie.
**---------------------------------------------------------------------------
*/
LOCAL const uint8 MVHD_TRAILER [] = {  /* movie header atom */
    0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
};
LOCAL const uint8 IODS_HEADER [] = {  /* initial object descriptor atom */
    0x00, 0x00, 0x00, 0x21, 0x69, 0x6F, 0x64, 0x73,
    0x00, 0x00, 0x00, 0x00, 0x10, 0x13, 0x00, 0x4F,
    0xFF, 0x01, 0xFF, 0x03, 0xFF
};
LOCAL const uint8 DRM_HEADER [] = {  /* 'drm ' and 'dcmd' atom fixed parts */
    0x00, 0x00, 0x00, 0x12, 0x64, 0x72, 0x6D, 0x20,
    0x00, 0x00, 0x00, 0x0A, 0x64, 0x63, 0x6D, 0x64
};
LOCAL const uint8 TKHD_TRAILER_AUDIO [] = {  /* track header atom */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
};
LOCAL const uint8 TKHD_TRAILER_MPEG [] = {  /* track header atom */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
};
LOCAL const uint8 MDHD_TRAILER [] = {  /* media header atom */
    0x00, 0x00
};
LOCAL const uint8 VMHD_ATOM [] = {  /* video media header atom */
    0x00, 0x00, 0x00, 0x14, 0x76, 0x6D, 0x68, 0x64,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
};
LOCAL const uint8 SMHD_ATOM [] = {  /* sound media header atom */
    0x00, 0x00, 0x00, 0x10, 0x73, 0x6D, 0x68, 0x64,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
LOCAL const uint8 NMHD_ATOM [] = {  /* MPEG-4 media header atom */
    0x00, 0x00, 0x00, 0x0C, 0x6E, 0x6D, 0x68, 0x64,
    0x00, 0x00, 0x00, 0x00
};
LOCAL const uint8 DINF_ATOM [] = {  /* data information atom */
    0x00, 0x00, 0x00, 0x24, 0x64, 0x69, 0x6E, 0x66,
    0x00, 0x00, 0x00, 0x1C, 0x64, 0x72, 0x65, 0x66,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x0C, 0x75, 0x72, 0x6C, 0x20,
    0x00, 0x00, 0x00, 0x01
};
LOCAL const uint8 MP4V_ATOM_PART_1 [] = {  /* visual sample entry atom */
    0x6D, 0x70, 0x34, 0x76,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
LOCAL const uint8 MP4V_ATOM_PART_2 [] = {  /* visual sample entry atom */
    0x00, 0x48, 0x00, 0x00,
    0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x18, 0xFF, 0xFF
};
LOCAL const uint8 MP4A_ATOM_PART_1 [] = {  /* audio sample entry atom */
    0x6D, 0x70, 0x34, 0x61,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x02, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00
};
LOCAL const uint8 MP4A_ATOM_PART_2 [] = {  /* audio sample entry atom */
    0x00, 0x00
};
LOCAL const uint8 MP4S_ATOM_BODY [] = {  /* MPEG sample entry atom */
    0x6D, 0x70, 0x34, 0x73,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01
};
LOCAL const uint8 ESDS_TRAILER [] = {  /* ES descriptor atom */
    0x06, 0x01, 0x02
};
LOCAL const uint8 S263_ATOM_PART_1 [] = {  /* H.263 sample entry atom */
    0x73, 0x32, 0x36, 0x33, /* "s263" atom */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, /* data-reference-index */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* reserved fields */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* reserved fields */
};
LOCAL const uint8 S263_ATOM_PART_2 [] = {  /* H.263 sample entry atom */
    0x00, 0x48, 0x00, 0x00, 0x00, 0x48, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18,
    0xFF, 0xFF,
    0x00, 0x00, 0x00, 0x0F, 0x64, 0x32, 0x36, 0x33, /* "d263" atom */
    0x51, 0x63, 0x6F, 0x6D,                         /* vendor code - "Qcom" */
    0x01,              /* decoder version required */
};
LOCAL const uint8 SAMR_ATOM_PART_1 [] = {  /* AMR sample entry atom */
    0x73, 0x61, 0x6D, 0x72, /* "samr" atom */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, /* data-reference-index */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* reserved fields */
    0x00, 0x02, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00  /* reserved w/values */
};
LOCAL const uint8 SAWB_ATOM_PART_1 [] = {  /* AMR-WB sample entry atom */
    0x73, 0x61, 0x77, 0x62, /* "sawb" atom */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, /* data-reference-index */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* reserved fields */
    0x00, 0x02, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00  /* reserved w/values */
};
LOCAL const uint8 SAMR_SAWB_ATOM_PART_2 [] = {  /* AMR/AMR-WB sample entry */
    0x00, 0x00,
    0x00, 0x00, 0x00, 0x11, 0x64, 0x61, 0x6D, 0x72, /* "damr" atom */
    0x51, 0x63, 0x6F, 0x6D,                         /* vendor code - "Qcom" */
    0x01,              /* decoder version required */
};
LOCAL const uint8 SAWP_ATOM_PART_1 [] = {  /* AMR-WB+ sample entry atom */
    0x73, 0x61, 0x77, 0x70, /* "sawp" atom */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, /* data-reference-index */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* reserved fields */
    0x00, 0x02, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00  /* reserved w/values */
};
LOCAL const uint8 SAWP_ATOM_PART_2 [] = {  /* AMR-WB+ sample entry atom */
    0x00, 0x00,
    0x00, 0x00, 0x00, 0x0D, 0x64, 0x61, 0x77, 0x70, /* "dawp" atom */
    0x51, 0x63, 0x6F, 0x6D,                         /* vendor code - "Qcom" */
    0x01,              /* decoder version required */
};
LOCAL const uint8 SEVC_ATOM_PART_1 [] = {  /* EVRC sample entry atom */
    0x73, 0x65, 0x76, 0x63, /* "sevc" atom */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, /* data-reference-index */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* reserved fields */
    0x00, 0x02, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00  /* reserved w/values */
};
LOCAL const uint8 SEVC_ATOM_PART_2 [] = {  /* EVRC sample entry atom */
    0x00, 0x00,
    0x00, 0x00, 0x00, 0x0E, 0x64, 0x65, 0x76, 0x63, /* "devc" atom */
    0x51, 0x63, 0x6F, 0x6D,                         /* vendor code - "Qcom" */
    0x01,              /* decoder version required */
};
LOCAL const uint8 SECB_ATOM_PART_1 [] = {  /* EVRC-B sample entry atom */
    0x73, 0x65, 0x63, 0x62, /* "secb" atom */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, /* data-reference-index */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* reserved fields */
    0x00, 0x02, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00  /* reserved w/values */
};
LOCAL const uint8 SECB_ATOM_PART_2 [] = {  /* EVRC-B sample entry atom */
    0x00, 0x00,
    0x00, 0x00, 0x00, 0x0E, 0x64, 0x65, 0x63, 0x62, /* "decb" atom */
    0x51, 0x63, 0x6F, 0x6D,                         /* vendor code - "Qcom" */
    0x01,              /* decoder version required */
};
LOCAL const uint8 SECW_ATOM_PART_1 [] = {  /* EVRC-WB sample entry atom */
    0x73, 0x65, 0x63, 0x77, /* "secw" atom */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, /* data-reference-index */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* reserved fields */
    0x00, 0x02, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00  /* reserved w/values */
};
LOCAL const uint8 SECW_ATOM_PART_2 [] = {  /* EVRC-WB sample entry atom */
    0x00, 0x00,
    0x00, 0x00, 0x00, 0x0E, 0x64, 0x65, 0x63, 0x77, /* "decw" atom */
    0x51, 0x63, 0x6F, 0x6D,                         /* vendor code - "Qcom" */
    0x01,              /* decoder version required */
};
LOCAL const uint8 SQCP_ATOM_PART_1 [] = {  /* QCELP sample entry atom */
    0x73, 0x71, 0x63, 0x70, /* "sqcp" atom */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, /* data-reference-index */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* reserved fields */
    0x00, 0x02, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00  /* reserved w/values */
};
LOCAL const uint8 SQCP_ATOM_PART_2 [] = {  /* QCELP sample entry atom */
    0x00, 0x00,
    0x00, 0x00, 0x00, 0x0E, 0x64, 0x71, 0x63, 0x70, /* "dqcp" atom */
    0x51, 0x63, 0x6F, 0x6D,                         /* vendor code - "Qcom" */
    0x01,              /* decoder version required */
};

LOCAL const uint8 SAVC1_ATOM_PART_1 [] = { /* "avc1" */
    0x61, 0x76, 0x63, 0x31,  /* "avc1" atom */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* "reserved"*/
    0x00, 0x01, /* "Data reference index" */
    0x00, 0x00, /* "Predefined" */
    0x00, 0x00, /* "Reserved" */
    /* Predefined*/
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

LOCAL const uint8 SAVC1_ATOM_PART_2 [] = { /* "avc1" */
    0x00, 0x48, 0x00, 0x00, /* "72 dpi" */
    0x00, 0x48, 0x00, 0x00, /* "72 dpi" */
    0x00, 0x00, 0x00, 0x00, /* "Reserved" */
    0x00, 0x01, /* "Frame_count" */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* "Compressor Name" */
    0x00, 0x18, /* Depth */
    0xFF, 0xFF /* Predefined = -1 */
};

LOCAL const uint8 TFHD_TRAILER [] = {  /* track fragment header atom */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* base_data_offset */
};

/*---------------------------------------------------------------------------
** AMR frame size map - each entry in this table corresponds to the AMR frame
** type having the same number as the index into the table.  The entries give
** the sizes of the frames of each mode.
**
** Note that these sizes are for AMR IF1 format.
**---------------------------------------------------------------------------
*/
LOCAL const uint32 AMR_IF1_FRAME_SIZES [16] = {
    13, 14, 16, 18, 20, 21, 27, 32, 6, 6, 5, 5, 1, 1, 1, 1
};

/*---------------------------------------------------------------------------
** AMR-WB frame size map - each entry in this table corresponds to the AMR-WB
** frame type having the same number as the index into the table.  The entries
** give the sizes of the frames of each mode.
**
** Note that these sizes are for AMR-WB IF1 format.
**---------------------------------------------------------------------------
*/
LOCAL const uint32 AMR_WB_IF1_FRAME_SIZES [16] = {
    18, 24, 33, 37, 41, 47, 51, 59, 61, 6, 1, 1, 1, 1, 1, 1
};

/* <EJECT> */
/*===========================================================================

                EXTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================

FUNCTION  video_fmt_mp4w_create

DESCRIPTION
  This function creates a new MP4 file and prepares it for writing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void video_fmt_mp4w_create
(
  const video_fmt_create_params_type  *params,
  video_fmt_status_cb_func_type       callback_ptr,
  void                                *client_data
)
{
    video_fmt_mp4w_context_type  *context;
    video_fmt_alloc_type         alloc;
    video_fmt_free_type          free;
    video_fmt_writer_ready_type  writer_ready;
    uint32                       test_word = 1;
    uint32                       i, j;
    uint8                        *temp_buffer;
    uint32                       atom_size;

    MSG_LOW ("create: Entered function", 0, 0, 0);

    /* A valid client callback function is required. */
    if (NULL == callback_ptr)
    {
        MSG_ERROR ("create: NULL callback!", 0, 0, 0);
        return;
    }

    /* Ask client to allocate memory for our state structure. */
    alloc.size = sizeof (video_fmt_mp4w_context_type);
    callback_ptr (VIDEO_FMT_ALLOC, client_data,
                  (video_fmt_status_cb_info_type *) &alloc,
                  NULL);
    if (!alloc.ptr)
    {
        MSG_ERROR ("create: alloc failure!", 0, 0, 0);
        callback_ptr (VIDEO_FMT_FAILURE, client_data, NULL, NULL);
        return;
    }
    context = (video_fmt_mp4w_context_type *) alloc.ptr;

    /* Initialize movie writer state. */
    (void) memset (context, 0,
                   sizeof (video_fmt_mp4w_context_type));

    /* Initialize writer critical section. */
    rex_init_crit_sect (&context->cs);

    /* Determine if we need to byte-swap words to store network byte order
    ** words in host order.
    */
    if (((uint8 *) &test_word) [0] == 1)
    {
        context->byte_swap_needed = TRUE;
    }
    else
    {
        context->byte_swap_needed = FALSE;
    }

    /* Store movie-scope parameters in movie writer state. */
    context->timescale = params->movie_timescale;
    context->fragment_size = params->fragment_size;
    context->callback_ptr = callback_ptr;
    context->client_data = client_data;
    context->movie_size_warning_near_threshold
        = params->movie_size_warning_near_threshold;
    context->movie_size_warning_imminent_threshold
        = params->movie_size_warning_imminent_threshold;
    context->movie_size_limit = params->file_size_limit;
    context->movie_duration_limit = params->file_duration_limit;
    context->force_stsz_table = params->force_stsz_table;
    context->include_drm = params->include_drm;
    context->drm_distribution_rights = params->drm_distribution_rights;
    context->include_user_data = params->include_user_data;
    context->MfraAllocSuccess = TRUE;
    context->enable_fixupdata = params->enable_fixupdata;
    context->output_unit_size = params->output_unit_size;

    if (params->include_user_data)
    {
        context->user_data.midiSize = params->user_data.midiSize;
        context->user_data.midiData = params->user_data.midiData;

        if (params->user_data.titl_auth_desc_valid)
        {
            if (!video_fmt_mp4w_copy_language_coded_string(
			       &context->user_data.title, &params->user_data.title,
                   callback_ptr, client_data) || 
				!video_fmt_mp4w_copy_language_coded_string(
				   &context->user_data.author, &params->user_data.author,
                   callback_ptr, client_data) || 
				!video_fmt_mp4w_copy_language_coded_string(
				   &context->user_data.description, &params->user_data.description,
                   callback_ptr, client_data) || 
				!video_fmt_mp4w_copy_language_coded_string(
				   &context->user_data.copyright, &params->user_data.copyright,
                   callback_ptr, client_data)
			)
            {
                MSG_ERROR ("create: alloc failure!", 0, 0, 0);
                video_fmt_mp4w_release (context);
                callback_ptr (VIDEO_FMT_FAILURE, client_data, NULL, NULL);
                return;
            }
            context->user_data.titl_auth_desc_valid = TRUE;
        }

        /* Check & update location info */
        if (params->user_data.loci.loci_valid)
        {
            if(!video_fmt_mp4w_copy_location 
                (&context->user_data.loci, &params->user_data.loci, 
                 callback_ptr, client_data))
            {
              MSG_ERROR ("create: alloc failure!", 0, 0, 0);
              video_fmt_mp4w_release (context);
              callback_ptr (VIDEO_FMT_FAILURE, client_data, NULL, NULL);
              return;
            }
          context->user_data.loci.loci_valid = TRUE;
        }
    }

    /* Allocate a separate buffer for meta data if fragmentation is enabled. */
    if (!video_fmt_mp4w_stream_alloc(context, &context->meta_data_buffer,
                                    VIDEO_FMT_MP4W_META_DATA_BUFFER_SIZE))
    {
        MSG_ERROR ("create: alloc failure!", 0, 0, 0);
        video_fmt_mp4w_release (context);
        callback_ptr (VIDEO_FMT_FAILURE, client_data, NULL, NULL);
        return;
    }

    /* Ask client to allocate memory for streams. */
    context->num_streams = params->num_streams;
    alloc.size = sizeof (video_fmt_mp4w_stream_context_type)
        * params->num_streams;
    callback_ptr (VIDEO_FMT_ALLOC, client_data,
                  (video_fmt_status_cb_info_type *) &alloc,
                  NULL);
    if (!alloc.ptr)
    {
        MSG_ERROR ("create: alloc failure!", 0, 0, 0);
        video_fmt_mp4w_release (context);
        callback_ptr (VIDEO_FMT_FAILURE, client_data, NULL, NULL);
        return;
    }
    context->streams = (video_fmt_mp4w_stream_context_type *) alloc.ptr;

    /* Initialize stream state. */
    (void) memset (context->streams, 0,
        context->num_streams * sizeof (video_fmt_mp4w_stream_context_type));

    /* Allocate memory required for each stream. */
    for (i = 0; i < context->num_streams; ++i)
    {
        /* Store modifiable stream-scope parameters in stream state. */
        context->streams [i].max_samples = params->streams [i].max_samples;
        context->streams [i].max_samples_rate
            = params->streams [i].max_samples_rate;
        context->streams [i].max_table_stores
            = params->streams [i].max_table_stores;

        /* Disable table stores feature if fragmentation is enabled. */
        if (context->fragment_size > 0)
        {
            context->streams [i].max_table_stores = 0;
        }

        /* If context->streams [i].max_table_stores > 0, make sure
        ** context->streams [i].max_samples_rate is also greater than 0.
        */
        if (context->streams [i].max_table_stores > 0)
        {
            context->streams [i].max_samples_rate = MAX
                (context->streams [i].max_samples_rate, 1);

            /* context->streams [i].max_samples should be no less than
            ** context->streams [i].max_samples_rate. Otherwise this code
            ** will break.
            */
            context->streams [i].max_samples = MAX
                (context->streams [i].max_samples,
                context->streams [i].max_samples_rate);
        }

        /* If context->streams [i].max_table_stores == 0, set
        ** context->streams [i].max_samples_rate to 0 to save memory.
        */
        else
        {
            context->streams [i].max_samples_rate = 0;
        }

        /* Timed text streams cannot be marked as fixed sample size or having
        ** inter frames.
        */
        if (params->streams [i].type == VIDEO_FMT_STREAM_TEXT)
        {
            if (params->streams [i].sample_size > 0)
            {
                MSG_ERROR ("create: fixed sample sizes not supported for "
                           "text streams!", 0, 0, 0);
                video_fmt_mp4w_release (context);
                callback_ptr (VIDEO_FMT_FAILURE, client_data, NULL, NULL);
                return;
            }
            if (params->streams [i].inter_frames)
            {
                MSG_ERROR ("create: inter frames not supported for "
                           "text streams!", 0, 0, 0);
                video_fmt_mp4w_release (context);
                callback_ptr (VIDEO_FMT_FAILURE, client_data, NULL, NULL);
                return;
            }
        }

        /* Allocate memory for chunk table.
        ** The allcation size is 1 entry larger than the requested size
        ** to allow for access to the last entry at all times.
        */
        alloc.size = sizeof (video_fmt_mp4w_chunk_table_entry_type)
            * (params->streams [i].max_chunks + 1);
        callback_ptr (VIDEO_FMT_ALLOC, client_data,
                      (video_fmt_status_cb_info_type *) &alloc,
                      NULL);
        if (!alloc.ptr)
        {
            MSG_ERROR ("create: alloc failure!", 0, 0, 0);
            video_fmt_mp4w_release (context);
            callback_ptr (VIDEO_FMT_FAILURE, client_data, NULL, NULL);
            return;
        }
        context->streams [i].chunks
            = (video_fmt_mp4w_chunk_table_entry_type *) alloc.ptr;
        context->streams [i].chunks++;

        /* Allocate memory for sample table, unless stream consists of fixed
        ** size and fixed time samples, with no inter frames (in which case we
        ** can optimize away the whole table.
        ** The allocation size is context->streams [i].max_samples_rate
        ** entries larger than the requested size to allow for access to the
        ** last context->streams [i].max_samples_rate entries at all times.
        */
        if ((0 == params->streams [i].sample_size)
            || (0 == params->streams [i].sample_delta)
            || params->streams [i].inter_frames)
        {
            alloc.size = sizeof (video_fmt_mp4w_sample_table_entry_type)
                * (context->streams [i].max_samples
                  + context->streams [i].max_samples_rate);
            callback_ptr (VIDEO_FMT_ALLOC, client_data,
                          (video_fmt_status_cb_info_type *) &alloc,
                          NULL);
            if (!alloc.ptr)
            {
                MSG_ERROR ("create: alloc failure!", 0, 0, 0);
                video_fmt_mp4w_release (context);
                callback_ptr (VIDEO_FMT_FAILURE, client_data, NULL, NULL);
                return;
            }
            context->streams [i].samples
                = (video_fmt_mp4w_sample_table_entry_type *) alloc.ptr;
            context->streams [i].samples
                += context->streams [i].max_samples_rate;
        }

        /* Allocate memory for table stores table.
        ** The allcation size is 2 entries larger than the requested size
        ** in order to store the last sample table and the last chunk
        ** table after 'max_table_stores' are all used up.
        */
        if (context->streams [i].max_table_stores > 0)
        {
            alloc.size = sizeof (video_fmt_mp4w_table_store_entry_type)
                * (context->streams [i].max_table_stores + 2);
            callback_ptr (VIDEO_FMT_ALLOC, client_data,
                          (video_fmt_status_cb_info_type *) &alloc,
                          NULL);
            context->streams [i].table_stores
                = (video_fmt_mp4w_table_store_entry_type *) alloc.ptr;
        }

        /* Allocate memory for bitstream header, if there is any. */
        if (params->streams [i].max_header > 0)
        {
            alloc.size = params->streams [i].max_header;
            callback_ptr (VIDEO_FMT_ALLOC, client_data,
                          (video_fmt_status_cb_info_type *) &alloc,
                          NULL);
            if (!alloc.ptr)
            {
                MSG_ERROR ("create: alloc failure!", 0, 0, 0);
                video_fmt_mp4w_release (context);
                callback_ptr (VIDEO_FMT_FAILURE, client_data, NULL, NULL);
                return;
            }
            context->streams [i].header = (uint8 *) alloc.ptr;
        }

        /* Allocate memory for bitstream footer, if there is any. */
        if (params->streams [i].max_footer > 0)
        {
            alloc.size = params->streams [i].max_footer;
            callback_ptr (VIDEO_FMT_ALLOC, client_data,
                          (video_fmt_status_cb_info_type *) &alloc,
                          NULL);
            if (!alloc.ptr)
            {
                MSG_ERROR ("create: alloc failure!", 0, 0, 0);
                video_fmt_mp4w_release (context);
                callback_ptr (VIDEO_FMT_FAILURE, client_data, NULL, NULL);
                return;
            }
            context->streams [i].footer = (uint8 *) alloc.ptr;
        }

        /* Allocate memory for stream buffer. */
        if (!video_fmt_mp4w_stream_alloc
            (context, &context->streams [i].buffer,
             params->streams [i].buffer_size))
        {
            MSG_ERROR ("create: alloc failure!", 0, 0, 0);
            video_fmt_mp4w_release (context);
            callback_ptr (VIDEO_FMT_FAILURE, client_data, NULL, NULL);
            return;
        }

        /* Store stream-scope parameters in stream state. */
        context->streams [i].type = params->streams [i].type;
        context->streams [i].subinfo = params->streams [i].subinfo;
        context->streams [i].timescale = params->streams [i].media_timescale;
        context->streams [i].ref_track = params->streams [i].ref_track;
        context->streams [i].priority = params->streams [i].priority;
        context->streams [i].handler = params->streams [i].handler;
        context->streams [i].mode_change_restriction
            = params->streams [i].mode_change_restriction;
        context->streams [i].frames_per_sample
            = params->streams [i].frames_per_sample;
        context->streams [i].optimal_chunk_size
            = params->streams [i].chunk_size;
        context->streams [i].fixed_sample_size
            = params->streams [i].sample_size;
        context->streams [i].fixed_sample_delta
            = params->streams [i].sample_delta;
        context->streams [i].has_inter_frames
            = params->streams [i].inter_frames;
        context->streams [i].stsc_reset_rate
            = params->streams [i].stsc_reset_rate;
        context->streams [i].max_header = params->streams [i].max_header;
        context->streams [i].max_footer = params->streams [i].max_footer;
        context->streams [i].max_chunks = params->streams [i].max_chunks;
        context->streams [i].samples_out_near_threshold
            = params->streams [i].samples_out_near_threshold;
        context->streams [i].chunks_out_near_threshold
            = params->streams [i].chunks_out_near_threshold;
        context->streams [i].samples_out_imminent_threshold
            = params->streams [i].samples_out_imminent_threshold;
        context->streams [i].chunks_out_imminent_threshold
            = params->streams [i].chunks_out_imminent_threshold;
        context->streams [i].tkhd_layer = params->streams [i].layer;
        context->streams [i].tkhd_width = params->streams [i].width;
        context->streams [i].tkhd_height = params->streams [i].height;
        context->streams [i].tkhd_origin_x = params->streams [i].origin_x;
        context->streams [i].tkhd_origin_y = params->streams [i].origin_y;
        context->streams [i].language = params->streams [i].language;
        context->streams [i].match_duration
            = params->streams [i].match_duration;
        context->streams [i].match_duration_with
            = params->streams [i].match_duration_with;

        /* Set up 'stsc' algorithm to start in correct state. */
        context->streams [i].reset_stsc_algo = TRUE;
    }

    /* Mark "interlaced" flag for each interlaced stream. */
    for (i = 0; i < context->num_streams; ++i)
    {
        if ((params->streams [i].interlace < context->num_streams)
            && (params->streams [i].interlace != i))
        {
            context->streams [i].interlace_other
                = params->streams [i].interlace;
            context->streams [i].interlace_rate
                = params->streams [i].interlace_rate;
            context->streams [params->streams [i].interlace]
                .interlaced = TRUE;
        }
        else
        {
            context->streams [i].interlace_other = i;
        }
    }

    /* Search for circular interlace paths, and if any are found, trigger an
    ** error.
    */
    for (i = 0; i < context->num_streams; ++i)
    {
        if (context->streams [i].interlace_other != i)
        {
            for (j = context->streams [i].interlace_other;
                 j != i; j = context->streams [j].interlace_other)
            {
                if (context->streams [j].interlace_other == j)
                {
                    break;
                }
            }
            if (j == i)
            {
                MSG_ERROR ("create: circular interlace reference!", 0, 0, 0);
                video_fmt_mp4w_release (context);
                callback_ptr (VIDEO_FMT_FAILURE, client_data, NULL, NULL);
                return;
            }
        }
    }

    /* Allocate memory temporarily for generating the 'ftyp' atom. */
    atom_size = 16 + 4 * params->num_compat_brands;
    alloc.size = atom_size;
    context->callback_ptr (VIDEO_FMT_ALLOC, context->client_data,
                           (video_fmt_status_cb_info_type *) &alloc,
                           NULL);
    if (!alloc.ptr)
    {
        MSG_ERROR ("create: alloc failure!", 0, 0, 0);
        video_fmt_mp4w_release (context);
        callback_ptr (VIDEO_FMT_FAILURE, client_data, NULL, NULL);
        return;
    }
    temp_buffer = (uint8 *) alloc.ptr;

    /* Generate 'ftyp' atom containing brand information. */
    MAKE_ATOM_HEADER (temp_buffer, atom_size, "ftyp");
    memcpy (temp_buffer + 8, params->major_brand, 4);
    STORE_ATOM_UINT16 (temp_buffer, 12, params->version_major);
    STORE_ATOM_UINT16 (temp_buffer, 14, params->version_minor);
    for (i = 0; i < params->num_compat_brands; ++i)
    {
        memcpy (temp_buffer + 16 + 4 * i, params->compat_brands [i], 4);
    }

    /* Write atom to output file. */
    if (atom_size != video_fmt_mp4w_write_movie
        (context, temp_buffer, atom_size))
    {
        MSG_ERROR ("create: error writing output", 0, 0, 0);
        video_fmt_mp4w_release (context);
        callback_ptr (VIDEO_FMT_FAILURE, client_data, NULL, NULL);
        return;
    }

    /* Free memory temporarily used for generating the 'ftyp' atom. */
    free.ptr = temp_buffer;
    callback_ptr (VIDEO_FMT_FREE, client_data,
                  (video_fmt_status_cb_info_type *) &free, NULL);

    MSG_LOW ("create: Success, exiting function", 0, 0, 0);

    /* Send a writer ready indication, to provide the client with the hooks
    ** used to write UUID atoms and stream data.
    */
    writer_ready.write_cb = video_fmt_mp4w_write_stream;
    writer_ready.write_text_cb = video_fmt_mp4w_write_text;
    writer_ready.write_header_cb = video_fmt_mp4w_write_header;
    writer_ready.write_header_text_cb = video_fmt_mp4w_write_header_text;
    writer_ready.write_footer_cb = video_fmt_mp4w_write_footer;
    writer_ready.write_uuid_cb = video_fmt_mp4w_write_uuid;
    writer_ready.seek_cb = video_fmt_mp4w_seek;
    writer_ready.alloc_text_cb = video_fmt_mp4w_alloc_text;
    writer_ready.free_text_cb = video_fmt_mp4w_free_text;
    writer_ready.modify_style_text_cb = video_fmt_mp4w_modify_style_text;
    writer_ready.modify_hl_text_cb = video_fmt_mp4w_modify_hl_text;
    writer_ready.modify_hl_color_text_cb = video_fmt_mp4w_modify_hl_color_text;
    writer_ready.modify_karaoke_text_cb = video_fmt_mp4w_modify_karaoke_text;
    writer_ready.modify_scroll_text_cb = video_fmt_mp4w_modify_scroll_text;
    writer_ready.modify_link_text_cb = video_fmt_mp4w_modify_link_text;
    writer_ready.modify_box_text_cb = video_fmt_mp4w_modify_box_text;
    writer_ready.modify_blink_text_cb = video_fmt_mp4w_modify_blink_text;
    writer_ready.modify_wrap_text_cb = video_fmt_mp4w_modify_wrap_text;
    writer_ready.modify_user_atoms_cb = video_fmt_mp4w_modify_user_atoms;
    writer_ready.server_data = context;
    context->callback_ptr (VIDEO_FMT_WRITER_READY, context->client_data,
                           (video_fmt_status_cb_info_type *) &writer_ready,
                           video_fmt_mp4w_end);
}

/* <EJECT> */
/*===========================================================================

                HOOK FUNCTIONS

===========================================================================*/

/*===========================================================================

FUNCTION video_fmt_mp4w_write_stream

DESCRIPTION
  This procedure is used to provide the movie file writer with more stream
  data for the movie being written.  Depending on various parameters and the
  current state of the stream, data may or may not be written out to the movie
  file from this stream and possibly other streams with which it may be
  interlaced.

DEPENDENCIES
  video_fmt_mp4w_create must be called first.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void video_fmt_mp4w_write_stream (
  uint32                            stream_number,
  uint32                            num_samples,
  const video_fmt_sample_info_type  *sample_info,
  const uint8                       *sample_data,
  void                              *server_data,
  video_fmt_status_cb_func_type     callback_ptr,
  void                              *client_data
)
{
    video_fmt_mp4w_context_type         *context;
    video_fmt_mp4w_stream_context_type  *stream;
    video_fmt_io_done_type              io_done;
    uint32                              sample, lByte, frame;
    uint32                              total_amount = 0;
    uint8                               amr_mode;
    uint32                              fragment_number;
    uint32                              duration, drop_samples;
    boolean                             limit_reached = FALSE;
    boolean                             write_chunk = FALSE;
    video_fmt_done_type                 done;
    uint32                              temp_var=0;

    /* Verify a non-NULL context is given. */
    context = (video_fmt_mp4w_context_type *) server_data;
    if (!context)
    {
        MSG_ERROR ("write: Invalid context!", 0, 0, 0);
        return;
    }

    /* Serialize any simultaneous writes using a critical section. */
    rex_enter_crit_sect (&context->cs);

    /* If context is about to be deleted return right away */    
    if(context->context_deleted)
    {
        rex_leave_crit_sect(&context->cs);
        MSG_ERROR ("write_stream: Invalid context!", 0, 0, 0);
        return;

    }
    MSG_LOW ("write: Entered (stream=%d)", stream_number, 0, 0);

    /* Store updated client callback pointer and data, in case the client
    ** wants it change it at this point.
    */
    context->callback_ptr = callback_ptr;
    context->client_data = client_data;

    /* Cache pointer to stream being written. */
    stream = &context->streams [stream_number];

    /* If the limit was reached in a previous call to write to the stream,
    ** regenerate a limit reached callback and return early, without writing
    ** to the stream.
    */
    if (context->space_out)
    {
        MSG_LOW ("write: dropped sample for stream %d due to limit reached",
                 stream_number, 0, 0);
        video_fmt_mp4w_do_limit_reached_callback (context);
        rex_leave_crit_sect (&context->cs);
        return;
    }

    /* Record starting fragment sequence number. */
    fragment_number = context->fragment_sequence_number;

    /* If the stream is AMR, scan the modes used in the data, updating the AMR
    ** modes used bitmask.
    */
    if ((stream->type == VIDEO_FMT_STREAM_AUDIO)
        && ((stream->subinfo.audio.format == VIDEO_FMT_STREAM_AUDIO_AMR)
            || (stream->subinfo.audio.format
                == VIDEO_FMT_STREAM_AUDIO_AMR_WB)))
    {
        lByte = sample = frame = 0;
        while (sample < num_samples)
        {
            /* Extract AMR mode from first byte in sample. */
            amr_mode = ((sample_data [lByte] >> 3) & 15);

            /* Set corresponding bit in modes used bitmask. */
            stream->amr_modes_used |= (1 << amr_mode);

            /* Advance to next frame.  Count sample if frame count reaches
            ** next sample.
            */
            if (stream->subinfo.audio.format == VIDEO_FMT_STREAM_AUDIO_AMR)
            {
                lByte += AMR_IF1_FRAME_SIZES [amr_mode];
            }
            else
            {
                lByte += AMR_WB_IF1_FRAME_SIZES [amr_mode];
            }
            ++frame;
            if (0 == (frame % stream->frames_per_sample))
            {
                ++sample;
            }
        }
    }

    /* If this stream consists of fixed size and fixed time samples, with no
    ** inter frames, write all the samples in one shot, and don't bother
    ** constructing sample table information.  This saves a lot of extra time
    ** and space.
    */
    if ((stream->fixed_sample_size > 0)
        && (stream->fixed_sample_delta > 0)
        && !stream->has_inter_frames)
    {
        /* If movie duration limit would be met or exceeded by writing these
        ** samples, set limit reached flag and truncate samples to keep limit
        ** from being exceeded.
        */
        duration = stream->fixed_sample_delta * num_samples;
        if (context->movie_duration_limit)
        {
            if (video_fmt_mp4w_convert_time_units
                (stream->total_duration + duration,
                 stream->timescale, context->timescale)
                >= context->movie_duration_limit)
            {
                drop_samples = num_samples
                    - (video_fmt_mp4w_convert_time_units
                       (context->movie_duration_limit, context->timescale,
                        stream->timescale) - stream->total_duration)
                    / stream->fixed_sample_delta;
                MSG_HIGH ("duration limit reached on stream %d - "
                          "dropping %d samples",
                          stream_number, drop_samples, 0);
                num_samples -= drop_samples;
                duration = stream->fixed_sample_delta * num_samples;
                limit_reached = TRUE;
                context->space_out_status = VIDEO_FMT_SPACE_MSG_MOVIE_DURATION;
            }
        }

        /* Perform chunk processing, unless stream is interlaced by
        ** another stream.
        */
        if (!stream->interlaced && num_samples)
        {
            switch (video_fmt_mp4w_chunk_processing
                    (context, stream_number, FALSE, FALSE))
            {
            case VIDEO_FMT_FAILURE:
                video_fmt_mp4w_failure (context, TRUE);
                //rex_leave_crit_sect (&context->cs); called inside 
                //video_fmt_mp4w_failure
                return;

            case VIDEO_FMT_LIMIT_NEAR:
                if (!context->space_out_near)
                {
                    context->space_out_near = TRUE;
                    done.space_out_status = context->space_out_status;
                    context->callback_ptr
                        (VIDEO_FMT_LIMIT_NEAR, context->client_data,
                         (video_fmt_status_cb_info_type *) &done, video_fmt_mp4w_end);
                }
                break;

            case VIDEO_FMT_LIMIT_IMMINENT:
                if (!context->space_out_imminent)
                {
                    context->space_out_imminent = TRUE;
                    done.space_out_status = context->space_out_status;
                    context->callback_ptr
                        (VIDEO_FMT_LIMIT_IMMINENT, context->client_data,
                         (video_fmt_status_cb_info_type *) &done, video_fmt_mp4w_end);
                }
                break;

            case VIDEO_FMT_LIMIT_REACHED:
                context->space_out = TRUE;
                video_fmt_mp4w_do_limit_reached_callback (context);

                /* Return early, because once limit is reached, we are
                ** completely done writing the file, and should not
                ** allow more data to enter the stream buffers.
                */
                rex_leave_crit_sect (&context->cs);
                return;
            }
        }

        total_amount = stream->fixed_sample_size * num_samples;
        if (sample_data)
        {
            /* Check for stream buffer overflow.  We add 8 bytes here for the
            ** 'mdat' atom header because it is pushed in front of the data
            ** when written out.
            */
            if (total_amount + 8
                > video_fmt_mp4w_stream_amt_left (&stream->buffer))
            {
                MSG_ERROR ("write: stream %d buffer overflow!",
                          stream_number, 0, 0);
                video_fmt_mp4w_failure (context, FALSE);
                //rex_leave_crit_sect (&context->cs);
                //called inside video_fmt_mp4w_failure
                return;
            }

            /* Copy data into stream.  No need to check return value, since we
            ** know that the stream buffer can handle the data, and the only
            ** way it could be FALSE is if we tried to push more than the
            ** stream buffer could handle and an attempt to flush the stream
            ** buffer failed.
            */
            (void)video_fmt_mp4w_stream_push
                (context, &stream->buffer, sample_data, total_amount);
        }

        /* Count samples and bytes added. */
        stream->samples_size += num_samples;
        stream->samples_index += (int32) num_samples;
        stream->chunk_bytes += total_amount;

        /* Update stream duration and bitrate variables. */
        stream->duration += duration;
        stream->total_duration += duration;
        stream->bitrate_total_bits += stream->fixed_sample_size
            * num_samples * 8;
    }

    /* Otherwise, input samples one at a time, adding entries to the sample
    ** table cache.
    */
    else
    {
        for (sample = 0; sample < num_samples; ++sample)
        {
            /* Detect whether or not movie duration limit would be exceeded by
            ** writing this sample.
            */
            if (context->movie_duration_limit)
            {
                if (video_fmt_mp4w_convert_time_units
                    (stream->total_duration + sample_info [sample].delta,
                     stream->timescale, context->timescale)
                    > context->movie_duration_limit)
                {
                    drop_samples = num_samples - sample - 1;
                    MSG_HIGH ("duration limit reached on stream %d - "
                              "dropping %d samples",
                              stream_number, drop_samples, 0);
                    limit_reached = TRUE;
                    context->space_out_status = VIDEO_FMT_SPACE_MSG_MOVIE_DURATION;
                }
            }

            /* Make every I-frame be the first frame of the 'trun' atom
            ** in fragments except for the first fragment.
            */
            if (context->fragment_sequence_number > 0 &&
                stream->type == VIDEO_FMT_STREAM_VIDEO)
            {
                write_chunk = sample_info [sample].sync ? TRUE : FALSE;
            }

            /* Do sample processing. */
            if (!video_fmt_mp4w_sample_processing
                (context, stream_number, write_chunk, limit_reached))
            {
              video_fmt_mp4w_failure (context, TRUE);
               // rex_leave_crit_sect (&context->cs);
               //called inside vide_fmt_mp4w_failure
                return;
            }

            /* Check for the case where we run out of space in processing
            ** the last sample.  In this case, we cannot write any more
            ** to the movie.  A limit reached callback has already been made,
            ** so we can return early without sending any callback.
            */
            if (context->space_out)
            {
                rex_leave_crit_sect (&context->cs);
                return;
            }

            /* Check for stream buffer overflow.  We add 8 bytes here for
            ** the 'mdat' atom header because it may be pushed in front
            ** of the data when written out.
            */
            if (sample_data)
            {
                if (sample_info [sample].size + 8
                    > video_fmt_mp4w_stream_amt_left
                    (&stream->buffer))
                {
                    MSG_ERROR ("write: stream %d buffer overflow!",
                              stream_number, 0, 0);
                    video_fmt_mp4w_failure (context, FALSE);
                    //rex_leave_crit_sect (&context->cs);
                    //called inside video_fmt_mp4w_failure
                    return;
                }
            }

            /* Copy data into stream.  No need to check return value, since we
            ** know that the stream buffer can handle the data, and the only
            ** way it could be FALSE is if we tried to push more than the
            ** stream buffer could handle and an attempt to flush the stream
            ** buffer failed.
            */
            if (sample_data)
            {
                (void)video_fmt_mp4w_stream_push
                    (context, &stream->buffer,
                     sample_data + total_amount,
                     sample_info [sample].size);
            }
            total_amount += sample_info [sample].size;

            /* Update track meta data. */
            if (!video_fmt_mp4w_update_meta_data
                  (context, stream_number, &sample_info [sample]))
            {
                video_fmt_mp4w_failure (context, TRUE);
                //rex_leave_crit_sect (&context->cs);
                //called inside video_fmt_mp4w_failure
                return;
            }
        }
    }

    /* Update movie duration if this stream pushes it out. */    
    temp_var         =video_fmt_mp4w_convert_time_units(stream->duration, stream->timescale, context->timescale);
    context->fragment_duration = MAX( context->fragment_duration, temp_var );

    /* Send IO_DONE indication back, reporting the number of bytes actually
    ** written.
    */
    io_done.bytes = total_amount;
    io_done.fragment_done
        = (fragment_number != context->fragment_sequence_number)
        ? TRUE : FALSE;
    context->callback_ptr (VIDEO_FMT_IO_DONE, context->client_data,
                           (video_fmt_status_cb_info_type *) &io_done,
                           video_fmt_mp4w_end);

    MSG_LOW ("write: Success, exiting function", 0, 0, 0);
    rex_leave_crit_sect (&context->cs);
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_fmt_mp4w_write_text

DESCRIPTION
  This procedure is used to provide the movie file writer with information for
  constructing and writing a text sample to write to a text stream.  Depending
  on various parameters and the current state of the stream, data may or may
  not be written out to the movie file from this stream and possibly other
  streams with which it may be interlaced.

  The given text object is released by calling this function and should not be
  referenced again.  To release the text object without writing it, call
  video_fmt_mp4w_free_text instead.

DEPENDENCIES
  video_fmt_mp4w_create must be called first.

RETURN VALUE
  None

SIDE EFFECTS
  The text sample structure and any linked modifier atoms are automatically
  deallocated after they are written to the stream.

===========================================================================*/
void video_fmt_mp4w_write_text (
  uint32                            stream_number,
  void                              *handle,    /* allocated sample handle  */
  uint32                            delta,
  void                              *server_data,
  video_fmt_status_cb_func_type     callback_ptr,
  void                              *client_data
)
{
    video_fmt_mp4w_context_type         *context;
    video_fmt_mp4w_stream_context_type  *stream;
    video_fmt_io_done_type              io_done;
    uint32                              fragment_number;
    video_fmt_mp4w_text_sample_type     *text = NULL;
    video_fmt_mp4w_atom_list_type       *modifier;
    uint8                               temp_buffer [2];
    video_fmt_free_type                 free;
    video_fmt_sample_info_type          sample_info;
    uint32                              size;
    boolean                             limit_reached = FALSE;
    uint32                              temp_var=0;
    /* Verify a non-NULL context is given. */
    context = (video_fmt_mp4w_context_type *) server_data;
    if (!context)
    {
        MSG_ERROR ("write_text: Invalid context!", 0, 0, 0);
        return;
    }

    /* Serialize any simultaneous writes using a critical section. */
    rex_enter_crit_sect (&context->cs);

    /* If context is about to be deleted return right away */
    if(context->context_deleted)
    {
        rex_leave_crit_sect(&context->cs);
        MSG_ERROR ("write_text: Invalid context!", 0, 0, 0);
        return;

    }
    MSG_LOW ("write_text: Entered (stream=%d)", stream_number, 0, 0);

    /* Store updated client callback pointer and data, in case the client
    ** wants it change it at this point.
    */
    context->callback_ptr = callback_ptr;
    context->client_data = client_data;

    /* Cache pointer to stream being written. */
    stream = &context->streams [stream_number];

    /* Recover the text sample structure. */
    text = (video_fmt_mp4w_text_sample_type *) handle;

    /* The stream must be a text stream. */
    if (stream->type != VIDEO_FMT_STREAM_TEXT)
    {
        MSG_ERROR ("write_text: not a text stream!", 0, 0, 0);
        video_fmt_mp4w_free_text (text, callback_ptr, client_data);
        video_fmt_mp4w_failure (context, FALSE);
        //rex_leave_crit_sect (&context->cs); 
        //called inside video_fmt_mp4w_failure
        return;
    }

    /* Detect whether or not movie duration limit would be reached or exceeded
    ** by writing this sample.  If so, truncate last sample duration to
    ** exactly match the movie duration limit.
    */
    if (context->movie_duration_limit)
    {
        if (video_fmt_mp4w_convert_time_units
            (stream->total_duration + delta,
             stream->timescale, context->timescale)
            >= context->movie_duration_limit)
        {
            delta = video_fmt_mp4w_convert_time_units
                (context->movie_duration_limit,
                 context->timescale, stream->timescale)
                - stream->total_duration;
            limit_reached = TRUE;
            context->space_out_status = VIDEO_FMT_SPACE_MSG_MOVIE_DURATION;
        }
    }

    /* If the limit was reached in a previous call to write to the stream,
    ** declare an error and bail out.
    */
    if (context->space_out)
    {
        MSG_ERROR ("write_text: out of sample/chunk table space!", 0, 0, 0);
        video_fmt_mp4w_free_text (text, callback_ptr, client_data);
        video_fmt_mp4w_failure (context, FALSE);
        //rex_leave_crit_sect (&context->cs);
        //called inside video_fmt_mp4w_failure
        return;
    }

    /* Record starting fragment sequence number. */
    fragment_number = context->fragment_sequence_number;

    /* Calculate sample size. */
    size = 2 + text->text_length;
    for (modifier = text->modifiers; modifier; modifier = modifier->next)
    {
        size += modifier->size;
    }

    /* Check for stream buffer overflow.  We add 8 bytes here for
    ** the 'mdat' atom header because it is pushed in front of the
    ** data when written out.
    */
    if (size + 8 > video_fmt_mp4w_stream_amt_left (&stream->buffer))
    {
        MSG_ERROR ("write_text: stream %d buffer overflow!",
                   stream_number, 0, 0);
        video_fmt_mp4w_free_text (text, callback_ptr, client_data);
        video_fmt_mp4w_failure (context, FALSE);
        //rex_leave_crit_sect (&context->cs);
        //called inside video_fmt_mp4w_failure
        return;
    }

    /**************************************************************************
     ** Copy data into stream.  No need to check return value, since we
     ** know that the stream buffer can handle the data, and the only
     ** way it could be FALSE is if we tried to push more than the
     ** stream buffer could handle and an attempt to flush the stream
     ** buffer failed.
     *************************************************************************/

    /* Write text length. */
    STORE_ATOM_UINT16 (temp_buffer, 0, text->text_length);
    (void)video_fmt_mp4w_stream_push
        (context, &stream->buffer, temp_buffer, 2);

    /* Write text string. */
    (void)video_fmt_mp4w_stream_push
        (context, &stream->buffer,
         (const uint8 *) text->text_string,
         text->text_length);

    /* Write text modifier atoms. */
    while (text->modifiers)
    {
        /* Write modifier atom contents. */
        (void)video_fmt_mp4w_stream_push
            (context, &stream->buffer, text->modifiers->atom,
             text->modifiers->size);

        /* Save link to next modifier. */
        modifier = text->modifiers->next;

        /* Free modifier atom contents. */
        free.ptr = text->modifiers->atom;
        if (free.ptr)
        {
            context->callback_ptr
                (VIDEO_FMT_FREE, context->client_data,
                 (video_fmt_status_cb_info_type *) &free,
                 video_fmt_mp4w_end);
        }

        /* Free modifier atom structure. */
        free.ptr = text->modifiers;
        context->callback_ptr
            (VIDEO_FMT_FREE, context->client_data,
             (video_fmt_status_cb_info_type *) &free,
             video_fmt_mp4w_end);

        /* Move on to next modifier in linked list. */
        text->modifiers = modifier;
        if (!text->modifiers)
        {
            text->modifiers_end = NULL;
        }
    }

    /* Free text sample entry. */
    video_fmt_mp4w_free_text
        (text, callback_ptr, client_data);

    /* Update stream meta data. */
    sample_info.size = size;
    sample_info.delta = delta;
    sample_info.sync = TRUE;

    /* Update track meta data. */
    if (!video_fmt_mp4w_update_meta_data
          (context, stream_number, &sample_info))
    {
        video_fmt_mp4w_failure (context, TRUE);
        //rex_leave_crit_sect (&context->cs);
        //called inside video_fmt_mp4w_failure
        return;
    }

    /* Do sample processing. */
    if (!video_fmt_mp4w_sample_processing
        (context, stream_number, FALSE, limit_reached))
    {
        video_fmt_mp4w_failure (context, TRUE);       
        //rex_leave_crit_sect (&context->cs);
        //called inside video_fmt_mp4w_failure
        return;
    }

    /* Update movie duration if this stream pushes it out. */
    temp_var = video_fmt_mp4w_convert_time_units(stream->duration, stream->timescale, context->timescale);
    context->fragment_duration = MAX( context->fragment_duration,temp_var );

    /* Send IO_DONE indication back, reporting the number of bytes actually
    ** written.
    */
    io_done.bytes = size;
    io_done.fragment_done
        = (fragment_number != context->fragment_sequence_number)
        ? TRUE : FALSE;
    context->callback_ptr (VIDEO_FMT_IO_DONE, context->client_data,
                           (video_fmt_status_cb_info_type *) &io_done,
                           video_fmt_mp4w_end);

    MSG_LOW ("write_text: Success, exiting function", 0, 0, 0);
    rex_leave_crit_sect (&context->cs);
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_fmt_mp4w_write_header

DESCRIPTION
  This procedure is used to provide the movie file writer with more stream
  header data for the movie being written.  The data is held in a
  heap-allocated buffer until the meta-data for the stream is written out.
  Note that only streams having the capability of storing decoder specific
  information in the meta-data (currently only streams having 'esds' atoms)
  may have header information.

  For text streams, the header data given should be the entire contents of a
  'tx3g' atom.  To generate a 'tx3g' atom from a video_fmt_text_type
  structure, do not use this function.  Use video_fmt_mp4w_write_header_text
  instead.

DEPENDENCIES
  video_fmt_mp4w_create must be called first.

RETURN VALUE
  None

SIDE EFFECTS
  If the header is for a timed text track, any samples written to
  the stream after this point are associated with the new sample entry.

===========================================================================*/
void video_fmt_mp4w_write_header (
  uint32                            stream_number,
  uint32                            num_bytes,
  const uint8                       *header_data,
  void                              *server_data,
  video_fmt_status_cb_func_type     callback_ptr,
  void                              *client_data
)
{
    video_fmt_mp4w_context_type         *context;
    video_fmt_mp4w_stream_context_type  *stream;
    video_fmt_io_done_type              io_done;

    /* Verify a non-NULL context is given. */
    context = (video_fmt_mp4w_context_type *) server_data;
    if (!context)
    {
        MSG_ERROR ("write_header: Invalid context!", 0, 0, 0);
        return;
    }

    /* Serialize any simultaneous writes using a critical section. */
    rex_enter_crit_sect (&context->cs);

    /* If context is about to be deleted return right away */    
    if(context->context_deleted)
    {
        rex_leave_crit_sect(&context->cs);
        MSG_ERROR ("write_header: Invalid context!", 0, 0, 0);
        return;

    }
    MSG_LOW ("write_header: Entered function", 0, 0, 0);

    /* Store updated client callback pointer and data, in case the client
    ** wants it change it at this point.
    */
    context->callback_ptr = callback_ptr;
    context->client_data = client_data;

    /* Cache pointer to stream being written. */
    stream = &context->streams [stream_number];
    if (!stream)
    {
        rex_leave_crit_sect(&context->cs);
        MSG_ERROR ("write_header: Invalid stream!", 0, 0, 0);
        return;
    }

    /* If stream is timed text, interpret input buffer as 'tx3g' atom
    ** contents, and append it to the list of text sample entry atoms.
    */
    if (stream->type == VIDEO_FMT_STREAM_TEXT)
    {
        video_fmt_alloc_type           alloc;
        video_fmt_mp4w_atom_list_type  *ptr;

        /* Attempt to allocate memory for atom list node. */
        alloc.size = sizeof (*ptr);
        context->callback_ptr (VIDEO_FMT_ALLOC, context->client_data,
                               (video_fmt_status_cb_info_type *) &alloc,
                               video_fmt_mp4w_end);
        if (!alloc.ptr)
        {
            MSG_ERROR ("write_header: alloc failure!", 0, 0, 0);
            video_fmt_mp4w_failure(context,TRUE);
            return;
        }
        ptr = (video_fmt_mp4w_atom_list_type *) alloc.ptr;

        /* Attempt to allocate memory for the atom itself. */
        alloc.size = ptr->size = num_bytes + 8;
        context->callback_ptr (VIDEO_FMT_ALLOC, context->client_data,
                               (video_fmt_status_cb_info_type *) &alloc,
                               video_fmt_mp4w_end);
        if (!alloc.ptr)
        {
            video_fmt_free_type  free;

            MSG_ERROR ("write_header: alloc failure!", 0, 0, 0);
            free.ptr = ptr;
            context->callback_ptr
                (VIDEO_FMT_FREE, context->client_data,
                 (video_fmt_status_cb_info_type *) &free,
                 video_fmt_mp4w_end);
            video_fmt_mp4w_failure(context,TRUE);
            return;
        }
        ptr->atom = (uint8 *) alloc.ptr;

        /* Copy atom contents from input. */
        MAKE_ATOM_HEADER (ptr->atom, ptr->size, "tx3g");
        memcpy (ptr->atom + 8, header_data, num_bytes);

        /* Add atom to end of text sample entry atom list. */
        ptr->next = NULL;
        if (stream->text_end)
        {
            stream->text_end->next = ptr;
        }
        else
        {
            stream->text = ptr;
        }
        stream->text_end = ptr;
        ++stream->num_text;
    }

    /* Otherwise, append data in buffer to stream's decoder specific
    ** information.
    */
    else
    {
        /* Limit header write to space available. */
        num_bytes = MIN (num_bytes, stream->max_header - stream->header_size);
        if (num_bytes > 0)
        {
            if (!stream->header)
            {
                MSG_ERROR ("write_header: stream header is null!", 0, 0, 0);
                video_fmt_mp4w_failure(context,TRUE);
                return;
            }
            /* Copy bytes from client's buffer into header buffer. */
            memcpy (stream->header + stream->header_size,
                    header_data, num_bytes);

            /* Update count of bytes in stream header. */
            stream->header_size += num_bytes;
        }
    }

    /* Send IO_DONE indication back, reporting the number of bytes actually
    ** written.
    */
    io_done.bytes = num_bytes;
    io_done.fragment_done = FALSE;
    context->callback_ptr (VIDEO_FMT_IO_DONE, context->client_data,
                           (video_fmt_status_cb_info_type *) &io_done,
                           video_fmt_mp4w_end);

    MSG_LOW ("write_header: Success, exiting function", 0, 0, 0);
    rex_leave_crit_sect (&context->cs);
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_fmt_mp4w_write_header_text

DESCRIPTION
  This procedure is used to generate a new text sample entry ('tx3g') atom for
  a text stream.  The 'tx3g' atom is constructed based on the given header
  data structure, and held in a heap-allocated buffer until the meta-data for
  the stream is written out.

DEPENDENCIES
  video_fmt_mp4w_create must be called first.

RETURN VALUE
  None

SIDE EFFECTS
  A 'tx3g' sample entry atom is constructed on the heap and linked to the
  stream.  Any samples written to the stream after this point are associated
  with the new sample entry.

===========================================================================*/
void video_fmt_mp4w_write_header_text (
  uint32                              stream_number,
  const struct video_fmt_text_type_t  *text,
  void                                *server_data,
  video_fmt_status_cb_func_type       callback_ptr,
  void                                *client_data
)
{
    video_fmt_mp4w_context_type         *context;
    video_fmt_mp4w_stream_context_type  *stream;
    video_fmt_io_done_type              io_done;
    video_fmt_alloc_type                alloc;
    video_fmt_mp4w_atom_list_type       *ptr;
    uint16                              i;
    uint32                              offset;
    uint32                              temp32;
    uint8                               temp8;
    size_t                              length;

    /* Verify a non-NULL context is given. */
    context = (video_fmt_mp4w_context_type *) server_data;
    if (!context)
    {
        MSG_ERROR ("write_header_text: Invalid context!", 0, 0, 0);
        return;
    }

    /* Serialize any simultaneous writes using a critical section. */
    rex_enter_crit_sect (&context->cs);

    /* If context is about to be deleted return right away */    
    if(context->context_deleted)
    {
        rex_leave_crit_sect(&context->cs);
        MSG_ERROR ("write_header_text: Invalid context!", 0, 0, 0);
        return;

    }

    MSG_LOW ("write_header_text: Entered function", 0, 0, 0);

    /* Store updated client callback pointer and data, in case the client
    ** wants it change it at this point.
    */
    context->callback_ptr = callback_ptr;
    context->client_data = client_data;

    /* Cache pointer to stream being written. */
    stream = &context->streams [stream_number];

    /* The stream must be a text stream. */
    if (stream->type != VIDEO_FMT_STREAM_TEXT)
    {
        MSG_ERROR ("write_header_text: not a text stream!", 0, 0, 0);
        video_fmt_mp4w_failure(context,FALSE);
        return;
    }

    /* Attempt to allocate memory for atom list node. */
    alloc.size = sizeof (*ptr);
    context->callback_ptr (VIDEO_FMT_ALLOC, context->client_data,
                           (video_fmt_status_cb_info_type *) &alloc,
                           video_fmt_mp4w_end);
    if (!alloc.ptr)
    {
        MSG_ERROR ("write_header: alloc failure!", 0, 0, 0);
        video_fmt_mp4w_failure(context,FALSE);
        return;
    }
    ptr = (video_fmt_mp4w_atom_list_type *) alloc.ptr;

    /* Calculate size of 'tx3g' atom -- first, start with fixed part. */
    ptr->size = 56; /* size of fixed part of 'tx3g' atom */

    /* For each entry in the font table, add to the atom size. */
    for (i = 0; i < text->num_fonts; ++i)
    {
        length = strlen (text->fonts [i].font_name);
        if (length > 255)
        {
            MSG_ERROR ("write_header: font name too long!", 0, 0, 0);
            video_fmt_mp4w_failure(context,TRUE);
            return;
        }
        ptr->size += 3 /* fixed part of font table entry */
            + (uint32) length; /* size of font name */
    }

    /* Attempt to allocate memory for the atom itself. */
    alloc.size = ptr->size;
    context->callback_ptr (VIDEO_FMT_ALLOC, context->client_data,
                           (video_fmt_status_cb_info_type *) &alloc,
                           video_fmt_mp4w_end);
    if (!alloc.ptr)
    {
        video_fmt_free_type  free;

        MSG_ERROR ("write_header: alloc failure!", 0, 0, 0);
        free.ptr = ptr;
        context->callback_ptr
            (VIDEO_FMT_FREE, context->client_data,
             (video_fmt_status_cb_info_type *) &free,
             video_fmt_mp4w_end);
        video_fmt_mp4w_failure(context,TRUE);
        return;
    }
    ptr->atom = (uint8 *) alloc.ptr;

    /* Initialize atom contents. */
    (void) memset (ptr->atom, 0, ptr->size);
    MAKE_ATOM_HEADER (ptr->atom, ptr->size, "tx3g");

    /* data-reference-index is always 1 since we only write one data
    ** reference atom for the track.
    */
    STORE_ATOM_UINT16 (ptr->atom, 14, 1);/*lint!e572 */

    /* Assemble and write displayFlags. */
    temp32 = 0;
    if (text->scroll_in) temp32 |= 0x20;
    if (text->scroll_out) temp32 |= 0x40;
    if (text->scroll_in || text->scroll_out)
    {
        switch (text->scroll_type)
        {
          case VIDEO_FMT_SCROLL_VERT_UP: break;
          case VIDEO_FMT_SCROLL_HORZ_LEFT: temp32 |= 0x80; break;
          case VIDEO_FMT_SCROLL_VERT_DOWN: temp32 |= 0x100; break;
          case VIDEO_FMT_SCROLL_HORZ_RIGHT: temp32 |= 0x180; break;
        }
    }
    if (text->continuous_karaoke) temp32 |= 0x800;
    if (text->write_vertically) temp32 |= 0x20000;
    STORE_ATOM_UINT32 (ptr->atom, 16, temp32);

    /* Write horizontal-justification. */
    switch (text->horz_justify)
    {
      case VIDEO_FMT_JUSTIFY_LEFT_OR_TOP: ptr->atom [20] = 0; break;
      case VIDEO_FMT_JUSTIFY_CENTER: ptr->atom [20] = 1; break;
      case VIDEO_FMT_JUSTIFY_RIGHT_OR_BOTTOM: ptr->atom [20] = 0xFF; break;
    }

    /* Write vertical-justification. */
    switch (text->vert_justify)
    {
      case VIDEO_FMT_JUSTIFY_LEFT_OR_TOP: ptr->atom [21] = 0; break;
      case VIDEO_FMT_JUSTIFY_CENTER: ptr->atom [21] = 1; break;
      case VIDEO_FMT_JUSTIFY_RIGHT_OR_BOTTOM: ptr->atom [21] = 0xFF; break;
    }

    /* Write background-color-rgba. */
    STORE_ATOM_UINT32 (ptr->atom, 22, text->back_color);

    /* Write default-text-box. */
    STORE_ATOM_UINT16 (ptr->atom, 26, text->default_box_top);
    STORE_ATOM_UINT16 (ptr->atom, 28, text->default_box_left);
    STORE_ATOM_UINT16 (ptr->atom, 30, text->default_box_bottom);
    STORE_ATOM_UINT16 (ptr->atom, 32, text->default_box_right);

    /* Write default-style, font-id. */
    STORE_ATOM_UINT16 (ptr->atom, 38, text->font_id);

    /* Assemble and write default-style, face-style-flags. */
    temp8 = 0;
    if (text->bold) temp8 |= 1;
    if (text->italic) temp8 |= 2;
    if (text->underline) temp8 |= 4;
    ptr->atom [40] = temp8;

    /* Write default-style, font-size. */
    ptr->atom [41] = text->font_size;

    /* Write default-style, text-color-rgba. */
    STORE_ATOM_UINT32 (ptr->atom, 42, text->text_color);

    /* Write font-table. */
    MAKE_ATOM_HEADER (ptr->atom + 46, ptr->size - 46, "ftab");
    STORE_ATOM_UINT16 (ptr->atom, 54, text->num_fonts);
    for (i = 0, offset = 56; i < text->num_fonts; ++i)
    {
        /* Store font-ID and font-name-length. */
        STORE_ATOM_UINT16 (ptr->atom, offset, text->fonts [i].font_id);
        ptr->atom [offset + 2]
            = (uint8) strlen (text->fonts [i].font_name);

        /* Copy font name. */
        memcpy (ptr->atom + offset + 3, text->fonts [i].font_name,
                ptr->atom [offset + 2]);

        /* Set offset to one byte past last character in
        ** copied font name.
        */
        offset += 3 + ptr->atom [offset + 2];
    }

    /* Add atom to end of text sample entry atom list. */
    ptr->next = NULL;
    if (stream->text_end)
    {
        stream->text_end->next = ptr;
    }
    else
    {
        stream->text = ptr;
    }
    stream->text_end = ptr;
    ++stream->num_text;

    /* Send IO_DONE indication back, reporting the number of bytes actually
    ** written.
    */
    io_done.bytes = sizeof (*text);
    io_done.fragment_done = FALSE;
    context->callback_ptr (VIDEO_FMT_IO_DONE, context->client_data,
                           (video_fmt_status_cb_info_type *) &io_done,
                           video_fmt_mp4w_end);

    MSG_LOW ("write_header_text: Success, exiting function", 0, 0, 0);
    rex_leave_crit_sect (&context->cs);
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_fmt_mp4w_write_footer

DESCRIPTION
  This procedure is used to provide the movie file writer with more stream
  footer data for the movie being written.  The data is held in a
  heap-allocated buffer until the last sample for the stream is written out.

DEPENDENCIES
  video_fmt_mp4w_create must be called first.

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void video_fmt_mp4w_write_footer (
  uint32                            stream_number,
  uint32                            num_bytes,
  const uint8                       *footer_data,
  void                              *server_data,
  video_fmt_status_cb_func_type     callback_ptr,
  void                              *client_data
)
{
    video_fmt_mp4w_context_type         *context;
    video_fmt_mp4w_stream_context_type  *stream;
    video_fmt_io_done_type              io_done;

    /* Verify a non-NULL context is given. */
    context = (video_fmt_mp4w_context_type *) server_data;
    if (!context)
    {
        MSG_ERROR ("write_footer: Invalid context!", 0, 0, 0);
        return;
    }

    /* Serialize any simultaneous writes using a critical section. */
    rex_enter_crit_sect (&context->cs);
    
    /* If context is about to be deleted return right away */
    if(context->context_deleted)
    {
        rex_leave_crit_sect(&context->cs);
        MSG_ERROR ("write_footer: Invalid context!", 0, 0, 0);
        return;

    }
    MSG_LOW ("write_footer: Entered function", 0, 0, 0);

    /* Store updated client callback pointer and data, in case the client
    ** wants it change it at this point.
    */
    context->callback_ptr = callback_ptr;
    context->client_data = client_data;

    /* Cache pointer to stream being written. */
    stream = &context->streams [stream_number];

    if (!stream)
    {
        rex_leave_crit_sect(&context->cs);
        MSG_ERROR ("write_footer: Invalid stream!", 0, 0, 0);
        return;
    }

    /* Limit footer write to space available. */
    num_bytes = MIN (num_bytes, stream->max_footer - stream->footer_size);

    if (num_bytes > 0)
    {
        if (!stream->footer)
        {
            MSG_ERROR ("write_footer: stream footer is null!", 0, 0, 0);
            video_fmt_mp4w_failure(context,TRUE);
            return;
        }

        /* Copy bytes from client's buffer into header buffer. */
        memcpy (stream->footer + stream->footer_size,
                 footer_data, num_bytes);

        /* Update count of bytes in stream header. */
        stream->footer_size += num_bytes;
    }

    /* Send IO_DONE indication back, reporting the number of bytes actually
    ** written.
    */
    io_done.bytes = num_bytes;
    io_done.fragment_done = FALSE;
    context->callback_ptr (VIDEO_FMT_IO_DONE, context->client_data,
                           (video_fmt_status_cb_info_type *) &io_done,
                           video_fmt_mp4w_end);

    MSG_LOW ("write_footer: Success, exiting function", 0, 0, 0);
    rex_leave_crit_sect (&context->cs);
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_fmt_mp4w_write_uuid

DESCRIPTION
  This procedure is used to pass along user data atom information to be
  written to the movie file.  The data is written in the form of a 'uuid' atom
  with the UUID given to this function, or a 'udat' atom if no UUID is given
  (uuid = NULL).

DEPENDENCIES
  video_fmt_mp4w_create must be called first.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void video_fmt_mp4w_write_uuid (
  const uint8                    *uuid,  /* for 'uuid', 16-byte array      */
                                         /* for 'udat', set to NULL        */
  const void                     *data,  /* data for atom contents         */
  uint32                         size,   /* size of atom contents          */
  void                           *server_data,
  video_fmt_status_cb_func_type  callback_ptr,
  void                           *client_data
)
{
    video_fmt_mp4w_context_type  *context;
    uint32                       atom_size;
    uint8                        temp_buffer [24];
    int                          i;
    video_fmt_io_done_type       io_done;

    /* Verify a non-NULL context is given. */
    context = (video_fmt_mp4w_context_type *) server_data;
    if (!context)
    {
        MSG_ERROR ("write_uuid: Invalid context!", 0, 0, 0);
        return;
    }

    /* Serialize any simultaneous writes using a critical section. */
    rex_enter_crit_sect (&context->cs);

    /* If context is about to be deleted return right away */    
    if(context->context_deleted)
    {
        rex_leave_crit_sect(&context->cs);
        MSG_ERROR ("write_uuid: Invalid context!", 0, 0, 0);
        return;

    }
    MSG_LOW ("write_uuid: Entered function", 0, 0, 0);

    /* Store updated client callback pointer and data, in case the client
    ** wants it change it at this point.
    */
    context->callback_ptr = callback_ptr;
    context->client_data = client_data;

    /* Write out atom header -- generate a 'udat' atom if no UUID is given,
    ** otherwise generate a UUID atom.
    */
    if (uuid)
    {
        atom_size = size + 24;
        temp_buffer [0] = (uint8) ((atom_size >> 24) & 0xFF);
        temp_buffer [1] = (uint8) ((atom_size >> 16) & 0xFF);
        temp_buffer [2] = (uint8) ((atom_size >> 8) & 0xFF);
        temp_buffer [3] = (uint8) (atom_size & 0xFF);
        temp_buffer [4] = 'u';
        temp_buffer [5] = 'u';
        temp_buffer [6] = 'i';
        temp_buffer [7] = 'd';
        for (i = 0; i < 16; ++i)
        {
            temp_buffer [8 + i] = uuid [i];
        }
        if (24 != video_fmt_mp4w_write_movie (context, temp_buffer, 24))
        {
            MSG_ERROR ("write_uuid: error writing output", 0, 0, 0);
            video_fmt_mp4w_failure (context, TRUE);
            //rex_leave_crit_sect (&context->cs);
            //called inside video_fmt_mp4w_failure
            return;
        }
    }
    else
    {
        atom_size = size + 8;
        temp_buffer [0] = (uint8) ((atom_size >> 24) & 0xFF);
        temp_buffer [1] = (uint8) ((atom_size >> 16) & 0xFF);
        temp_buffer [2] = (uint8) ((atom_size >> 8) & 0xFF);
        temp_buffer [3] = (uint8) (atom_size & 0xFF);
        temp_buffer [4] = 'u';
        temp_buffer [5] = 'd';
        temp_buffer [6] = 'a';
        temp_buffer [7] = 't';
        if (8 != video_fmt_mp4w_write_movie (context, temp_buffer, 8))
        {
            MSG_ERROR ("write_uuid: error writing output", 0, 0, 0);
            video_fmt_mp4w_failure (context, TRUE);
            //rex_leave_crit_sect (&context->cs);
            //called inside video_fmt_mp4w_failure
            return;
        }
    }

    /* Write out atom contents. */
    if (size != video_fmt_mp4w_write_movie (context, data, size))
    {
        MSG_ERROR ("write_uuid: error writing output", 0, 0, 0);
        video_fmt_mp4w_failure (context, TRUE);
        //rex_leave_crit_sect (&context->cs);
        //called inside video_fmt_mp4w_failure
        return;
    }

    /* Send IO_DONE indication back, reporting the number of bytes actually
    ** written.
    */
    io_done.bytes = size;
    io_done.fragment_done = FALSE;
    context->callback_ptr (VIDEO_FMT_IO_DONE, context->client_data,
                           (video_fmt_status_cb_info_type *) &io_done,
                           video_fmt_mp4w_end);

    MSG_LOW ("write_uuid: Success, exiting function", 0, 0, 0);
    rex_leave_crit_sect (&context->cs);
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_fmt_mp4w_seek

DESCRIPTION
  This procedure is used to provide the the client with the buffer where the
  client can directly puts data.

DEPENDENCIES
  video_fmt_mp4w_create must be called first.

RETURN VALUE
  Pointer where more data can be put.
  NULL in error condition

SIDE EFFECTS

===========================================================================*/
 uint8* video_fmt_mp4w_seek (
  uint32                            stream_number,
  uint32                            offset,
  boolean                           bLarge,
  uint32                            *size,
  void                              *server_data,
  video_fmt_status_cb_func_type     callback_ptr,
  void                              *client_data
)
{
    video_fmt_mp4w_context_type         *context;
    video_fmt_mp4w_stream_context_type  *stream;
    uint8                               *retval;

    /* Verify a non-NULL context is given. */
    context = (video_fmt_mp4w_context_type *) server_data;
    if (!context)
    {
        MSG_ERROR ("seek: Invalid context!", 0, 0, 0);
        return NULL;
    }

    MSG_LOW ("seek: Entered function", 0, 0, 0);

    /* Store updated client callback pointer and data, in case the client
    ** wants it change it at this point.
    */
    context->callback_ptr = callback_ptr;
    context->client_data = client_data;

    /* Cache pointer to stream being written. */
    stream = &context->streams [stream_number];

    retval = video_fmt_mp4w_stream_seek
        (&stream->buffer, offset, bLarge, size);
    MSG_LOW ("seek: Success, exiting function", 0, 0, 0);
    return retval;
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_fmt_mp4w_alloc_text

DESCRIPTION
  This procedure is used to construct a new sample for a timed
  text stream.  It assembles a sample containing just the given text string,
  with no modifiers.

DEPENDENCIES
  video_fmt_mp4w_create must be called first.

RETURN VALUE
  A handle to the newly created text sample is returned.  Pass this handle to
  the video_fmt_mp4w_modify_*_text functions to apply modifiers to the
  sample.  Pass the handle to video_fmt_mp4w_write_text to write the sample
  to the track and deallocate the sample.  Otherwise, pass the handle to
  video_fmt_mp4w_free_text to dispose of the sample without writing it.

SIDE EFFECTS
  None

===========================================================================*/
void *video_fmt_mp4w_alloc_text (
  const char                     *data,  /* string to put into sample    */
  uint16                         size,   /* size of data string in bytes */
  void                           *server_data,
  video_fmt_status_cb_func_type  callback_ptr,
  void                           *client_data
)
{
    video_fmt_mp4w_context_type      *context;
    video_fmt_alloc_type             alloc;
    video_fmt_free_type              free;
    video_fmt_mp4w_text_sample_type  *ptr;

    /* Verify a non-NULL context is given. */
    context = (video_fmt_mp4w_context_type *) server_data;
    if (!context)
    {
        MSG_ERROR ("alloc_text: Invalid context!", 0, 0, 0);
        return NULL;
    }

    /* Serialize any simultaneous allocations using a critical section. */
    rex_enter_crit_sect (&context->cs);

    /* If context is about to be deleted return right away */
    if(context->context_deleted)
    {
        rex_leave_crit_sect(&context->cs);
        MSG_ERROR ("alloc_text: Invalid context!", 0, 0, 0);
        return NULL;

    }
    /* Check for input string */
    if (size && !data)
    {
        MSG_ERROR("alloc_text: input string in null!",0,0,0);
        rex_leave_crit_sect(&context->cs);
        return NULL;
    }
    MSG_LOW ("alloc_text: Entered function", 0, 0, 0);

    /* Store updated client callback pointer and data, in case the client
    ** wants it change it at this point.
    */
    context->callback_ptr = callback_ptr;
    context->client_data = client_data;

    /* Attempt to allocate memory for text sample structure. */
    alloc.size = sizeof (*ptr);
    context->callback_ptr (VIDEO_FMT_ALLOC, context->client_data,
                           (video_fmt_status_cb_info_type *) &alloc,
                           video_fmt_mp4w_end);
    if (!alloc.ptr)
    {
        MSG_ERROR ("alloc_text: alloc failure!", 0, 0, 0);
        video_fmt_mp4w_failure(context,TRUE);
        MSG_LOW ("alloc_text: Failed, exiting function", 0, 0, 0);
        return NULL;
    }
    ptr = (video_fmt_mp4w_text_sample_type *) alloc.ptr;
    (void) memset (ptr, 0, sizeof (*ptr));

    /* Allocate and copy client text string, if given. */
    if (size)
    {
        /* Attempt to allocate memory for text string copy. */
        alloc.size = size;
        context->callback_ptr (VIDEO_FMT_ALLOC, context->client_data,
                               (video_fmt_status_cb_info_type *) &alloc,
                               video_fmt_mp4w_end);
        if (!alloc.ptr)
        {
            MSG_ERROR ("alloc_text: alloc failure!", 0, 0, 0);
            free.ptr = ptr;
            context->callback_ptr
                (VIDEO_FMT_FREE, context->client_data,
                 (video_fmt_status_cb_info_type *) &free,
                 video_fmt_mp4w_end);
            video_fmt_mp4w_failure(context,TRUE);
            MSG_LOW ("alloc_text: Failed, exiting function", 0, 0, 0);
            return NULL;
        }
        ptr->text_string = (char *) alloc.ptr;

        /* Copy text string from client. */
        memcpy (ptr->text_string, data, size);
    }

    /* Set up other fields of text sample structure. */
    ptr->context = context;
    ptr->text_length = size;
    ptr->next = context->text_samples;
    context->text_samples = ptr;

    MSG_LOW ("alloc_text: Success, exiting function", 0, 0, 0);
    rex_leave_crit_sect (&context->cs);
    return ptr;
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_fmt_mp4w_free_text

DESCRIPTION
  This procedure is used to release a timed text stream sample without writing
  it to the stream.  If the sample is written to the
  video_fmt_mp4w_write_text function, do not pass it to this function, as
  the video_fmt_mp4w_write_text function automatically releases it after it
  is written.

DEPENDENCIES
  video_fmt_mp4w_alloc_text must be called first.

RETURN VALUE
  None

SIDE EFFECTS
  Any text modifiers allocated and linked to the text sample are deallocated.

===========================================================================*/
void video_fmt_mp4w_free_text (
  void                           *handle,    /* allocated sample handle     */
  video_fmt_status_cb_func_type  callback_ptr,
  void                           *client_data
)
{
    video_fmt_mp4w_context_type      *context;
    video_fmt_free_type              free;
    video_fmt_mp4w_text_sample_type  *ptr, *next, *prev = NULL;

    /* Recover sample and writer context. */
    if (!handle)
    {
        MSG_ERROR ("free_text: Invalid context!", 0, 0, 0);
        return;
    }
    ptr = (video_fmt_mp4w_text_sample_type *) handle;
    context = ptr->context;

    /* Serialize any simultaneous frees using a critical section. */
    rex_enter_crit_sect (&context->cs);

    /* If context is about to be deleted return right away */
    if(context->context_deleted)
    {
        rex_leave_crit_sect(&context->cs);
        MSG_ERROR ("free_text: Invalid context!", 0, 0, 0);
        return;

    }
    
    MSG_LOW ("free_text: Entered function", 0, 0, 0);

    /* Store updated client callback pointer and data, in case the client
    ** wants it change it at this point.
    */
    context->callback_ptr = callback_ptr;
    context->client_data = client_data;

    /* Find and remove the sample from the list of outstanding samples. */
    for (next = context->text_samples; next != ptr;
         prev = next, next = next->next)
    {
        if (!next)
        {
            MSG_ERROR ("free_text: Invalid text sample!", 0, 0, 0);
            rex_leave_crit_sect (&context->cs);
            return;
        }
    }
    if (prev)
    {
        prev->next = ptr->next;
    }
    else
    {
        context->text_samples = ptr->next;
    }

    /* Release all the modifier atoms. */
    while (ptr->modifiers)
    {
        free.ptr = ptr->modifiers->atom;
        if (free.ptr)
        {
            context->callback_ptr
                (VIDEO_FMT_FREE, context->client_data,
                 (video_fmt_status_cb_info_type *) &free,
                 video_fmt_mp4w_end);
        }
        free.ptr = ptr->modifiers;
        ptr->modifiers = ptr->modifiers->next;
        context->callback_ptr
            (VIDEO_FMT_FREE, context->client_data,
             (video_fmt_status_cb_info_type *) &free,
             video_fmt_mp4w_end);
    }

    /* Release text string and the structure itself. */
    free.ptr = ptr->text_string;
    context->callback_ptr
        (VIDEO_FMT_FREE, context->client_data,
         (video_fmt_status_cb_info_type *) &free,
         video_fmt_mp4w_end);
    free.ptr = ptr;
    context->callback_ptr
        (VIDEO_FMT_FREE, context->client_data,
         (video_fmt_status_cb_info_type *) &free,
         video_fmt_mp4w_end);

    MSG_LOW ("free_text: Success, exiting function", 0, 0, 0);
    rex_leave_crit_sect (&context->cs);
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_fmt_mp4w_modify_style_text

DESCRIPTION
  This procedure is used to apply a style modifier to a text sample
  constructed with the video_fmt_mp4w_alloc_text function, before writing it
  to the track with the video_fmt_mp4w_write_text function.

DEPENDENCIES
  video_fmt_mp4w_alloc_text must be called first.

RETURN VALUE
  None

SIDE EFFECTS
  A modifier atom is allocated on the heap and linked to the text sample.

===========================================================================*/
void video_fmt_mp4w_modify_style_text (
  uint16                           num_entries,
  const video_fmt_text_style_type  *entries,
  void                             *handle,    /* allocated sample handle   */
  video_fmt_status_cb_func_type    callback_ptr,
  void                             *client_data
)
{
    video_fmt_mp4w_context_type      *context;
    video_fmt_mp4w_text_sample_type  *ptr;
    uint8                            *atom;
    uint16                           i;
    uint8                            temp8;

    MSG_LOW ("modify_style_text: Entered function", 0, 0, 0);

    /* Recover sample and writer context. */
    if (!handle)
    {
        MSG_ERROR ("modify_style_text: Invalid context!", 0, 0, 0);
        MSG_LOW ("modify_style_text: Failed, exiting function", 0, 0, 0);
        return;
    }
    ptr = (video_fmt_mp4w_text_sample_type *) handle;
    context = ptr->context;

    /* Store updated client callback pointer and data, in case the client
    ** wants it change it at this point.
    */
    context->callback_ptr = callback_ptr;
    context->client_data = client_data;

    /* Allocate and link new atom. */
    atom = video_fmt_mp4w_new_text_modifier_atom
        (context, ptr, "styl", 8 + 2 + num_entries * 12);
    if (!atom)
    {
        MSG_LOW ("modify_style_text: Failed, exiting function", 0, 0, 0);
        return;
    }

    /* Store number of style record entries. */
    STORE_ATOM_UINT16 (atom, 8, num_entries);

    /* Store style record entries one by one. */
    for (i = 0; i < num_entries; ++i)
    {
        /* Write startChar and endChar. */
        STORE_ATOM_UINT16 (atom, 10 + i * 12, entries [i].start_char);
        STORE_ATOM_UINT16 (atom, 10 + i * 12 + 2, entries [i].end_char);

        /* Write font-id. */
        STORE_ATOM_UINT16 (atom, 10 + i * 12 + 4, entries [i].font_id);

        /* Assemble and write face-style-flags. */
        temp8 = 0;
        if (entries [i].bold) temp8 |= 1;
        if (entries [i].italic) temp8 |= 2;
        if (entries [i].underline) temp8 |= 4;
        atom [10 + i * 12 + 6] = temp8;

        /* Write font-size. */
        atom [10 + i * 12 + 7] = entries [i].font_size;

        /* Write text-color-rgba. */
        STORE_ATOM_UINT32 (atom, 10 + i * 12 + 8, entries [i].text_color);
    }

    MSG_LOW ("modify_style_text: Success, exiting function", 0, 0, 0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_fmt_mp4w_modify_hl_text

DESCRIPTION
  This procedure is used to apply a highlight modifier to a text sample
  constructed with the video_fmt_mp4w_alloc_text function, before writing it
  to the track with the video_fmt_mp4w_write_text function.

DEPENDENCIES
  video_fmt_mp4w_alloc_text must be called first.

RETURN VALUE
  None

SIDE EFFECTS
  A modifier atom is allocated on the heap and linked to the text sample.

===========================================================================*/
void video_fmt_mp4w_modify_hl_text (
  uint16                         start_char, /* starting offset in string   */
  uint16                         end_char,   /* ending offset in string     */
  void                           *handle,    /* allocated sample handle     */
  video_fmt_status_cb_func_type  callback_ptr,
  void                           *client_data
)
{
    video_fmt_mp4w_context_type      *context;
    video_fmt_mp4w_text_sample_type  *ptr;
    uint8                            *atom;

    MSG_LOW ("modify_hl_text: Entered function", 0, 0, 0);

    /* Recover sample and writer context. */
    if (!handle)
    {
        MSG_ERROR ("modify_hl_text: Invalid context!", 0, 0, 0);
        MSG_LOW ("modify_hl_text: Failed, exiting function", 0, 0, 0);
        return;
    }
    ptr = (video_fmt_mp4w_text_sample_type *) handle;
    context = ptr->context;

    /* Store updated client callback pointer and data, in case the client
    ** wants it change it at this point.
    */
    context->callback_ptr = callback_ptr;
    context->client_data = client_data;

    /* Allocate and link new atom. */
    atom = video_fmt_mp4w_new_text_modifier_atom
        (context, ptr, "hlit", 8 + 2 * 2);
    if (!atom)
    {
        MSG_LOW ("modify_hl_text: Failed, exiting function", 0, 0, 0);
        return;
    }

    /* Write startcharoffset and endcharoffset. */
    STORE_ATOM_UINT16 (atom, 8, start_char);
    STORE_ATOM_UINT16 (atom, 8 + 2, end_char);

    MSG_LOW ("modify_hl_text: Success, exiting function", 0, 0, 0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_fmt_mp4w_modify_hl_color_text

DESCRIPTION
  This procedure is used to apply a highlight color modifier to a text sample
  constructed with the video_fmt_mp4w_alloc_text function, before writing it
  to the track with the video_fmt_mp4w_write_text function.

DEPENDENCIES
  video_fmt_mp4w_alloc_text must be called first.

RETURN VALUE
  None

SIDE EFFECTS
  A modifier atom is allocated on the heap and linked to the text sample.

===========================================================================*/
void video_fmt_mp4w_modify_hl_color_text (
  uint32                         color,      /* 0xrrggbbaa where rr=red,    */
                                             /* gg=green, bb=blue, aa=alpha */
  void                           *handle,    /* allocated sample handle     */
  video_fmt_status_cb_func_type  callback_ptr,
  void                           *client_data
)
{
    video_fmt_mp4w_context_type      *context;
    video_fmt_mp4w_text_sample_type  *ptr;
    uint8                            *atom;

    MSG_LOW ("modify_hl_color_text: Entered function", 0, 0, 0);

    /* Recover sample and writer context. */
    if (!handle)
    {
        MSG_ERROR ("modify_hl_color_text: Invalid context!", 0, 0, 0);
        MSG_LOW ("modify_hl_color_text: Failed, exiting function", 0, 0, 0);
        return;
    }
    ptr = (video_fmt_mp4w_text_sample_type *) handle;
    context = ptr->context;

    /* Store updated client callback pointer and data, in case the client
    ** wants it change it at this point.
    */
    context->callback_ptr = callback_ptr;
    context->client_data = client_data;

    /* Allocate and link new atom. */
    atom = video_fmt_mp4w_new_text_modifier_atom
        (context, ptr, "hclr", 8 + 4);
    if (!atom)
    {
        MSG_LOW ("modify_hl_color_text: Failed, exiting function", 0, 0, 0);
        return;
    }

    /* Write highlight_color_rgba. */
    STORE_ATOM_UINT32 (atom, 8, color);

    MSG_LOW ("modify_hl_color_text: Success, exiting function", 0, 0, 0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_fmt_mp4w_modify_karaoke_text

DESCRIPTION
  This procedure is used to apply a dynamic highlighting (karaoke) modifier to
  a text sample constructed with the video_fmt_mp4w_alloc_text function,
  before writing it to the track with the video_fmt_mp4w_write_text
  function.

DEPENDENCIES
  video_fmt_mp4w_alloc_text must be called first.

RETURN VALUE
  None

SIDE EFFECTS
  A modifier atom is allocated on the heap and linked to the text sample.

===========================================================================*/
void video_fmt_mp4w_modify_karaoke_text (
  uint32                            start_time,
  uint16                            num_entries,
  const video_fmt_karaoke_box_type  *entries,
  void                              *handle,    /* allocated sample handle  */
  video_fmt_status_cb_func_type     callback_ptr,
  void                              *client_data
)
{
    video_fmt_mp4w_context_type      *context;
    video_fmt_mp4w_text_sample_type  *ptr;
    uint8                            *atom;
    uint16                           i;

    MSG_LOW ("modify_karaoke_text: Entered function", 0, 0, 0);

    /* Recover sample and writer context. */
    if (!handle)
    {
        MSG_ERROR ("modify_karaoke_text: Invalid context!", 0, 0, 0);
        MSG_LOW ("modify_karaoke_text: Failed, exiting function", 0, 0, 0);
        return;
    }
    ptr = (video_fmt_mp4w_text_sample_type *) handle;
    context = ptr->context;

    /* Store updated client callback pointer and data, in case the client
    ** wants it change it at this point.
    */
    context->callback_ptr = callback_ptr;
    context->client_data = client_data;

    /* Allocate and link new atom. */
    atom = video_fmt_mp4w_new_text_modifier_atom
        (context, ptr, "krok", 8 + 6 + num_entries * 8);
    if (!atom)
    {
        MSG_LOW ("modify_karaoke_text: Failed, exiting function", 0, 0, 0);
        return;
    }

    /* Store highlight-start-time. */
    STORE_ATOM_UINT32 (atom, 8, start_time);

    /* Store number of style record entries. */
    STORE_ATOM_UINT16 (atom, 12, num_entries);

    /* Store karaoke entries one by one. */
    for (i = 0; i < num_entries; ++i)
    {
        /* Write highlight-end-time. */
        STORE_ATOM_UINT32 (atom, 14 + i * 8, entries [i].end_time);

        /* Write startcharoffset and endcharoffset. */
        STORE_ATOM_UINT16 (atom, 14 + i * 8 + 4, entries [i].start_char);
        STORE_ATOM_UINT16 (atom, 14 + i * 8 + 6, entries [i].end_char);
    }

    MSG_LOW ("modify_karaoke_text: Success, exiting function", 0, 0, 0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_fmt_mp4w_modify_scroll_text

DESCRIPTION
  This procedure is used to apply a scroll delay modifier to a text sample
  constructed with the video_fmt_mp4w_alloc_text function, before writing it
  to the track with the video_fmt_mp4w_write_text function.

DEPENDENCIES
  video_fmt_mp4w_alloc_text must be called first.

RETURN VALUE
  None

SIDE EFFECTS
  A modifier atom is allocated on the heap and linked to the text sample.

===========================================================================*/
void video_fmt_mp4w_modify_scroll_text (
  uint32                         scroll_delay,
  void                           *handle,    /* allocated sample handle     */
  video_fmt_status_cb_func_type  callback_ptr,
  void                           *client_data
)
{
    video_fmt_mp4w_context_type      *context;
    video_fmt_mp4w_text_sample_type  *ptr;
    uint8                            *atom;

    MSG_LOW ("modify_scroll_text: Entered function", 0, 0, 0);

    /* Recover sample and writer context. */
    if (!handle)
    {
        MSG_ERROR ("modify_scroll_text: Invalid context!", 0, 0, 0);
        MSG_LOW ("modify_scroll_text: Failed, exiting function", 0, 0, 0);
        return;
    }
    ptr = (video_fmt_mp4w_text_sample_type *) handle;
    context = ptr->context;

    /* Store updated client callback pointer and data, in case the client
    ** wants it change it at this point.
    */
    context->callback_ptr = callback_ptr;
    context->client_data = client_data;

    /* Allocate and link new atom. */
    atom = video_fmt_mp4w_new_text_modifier_atom
        (context, ptr, "dlay", 8 + 4);
    if (!atom)
    {
        MSG_LOW ("modify_scroll_text: Failed, exiting function", 0, 0, 0);
        return;
    }

    /* Write scroll-delay. */
    STORE_ATOM_UINT32 (atom, 8, scroll_delay);

    MSG_LOW ("modify_scroll_text: Success, exiting function", 0, 0, 0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_fmt_mp4w_modify_link_text

DESCRIPTION
  This procedure is used to apply a hyperlink modifier to a text sample
  constructed with the video_fmt_mp4w_alloc_text function, before writing it
  to the track with the video_fmt_mp4w_write_text function.

DEPENDENCIES
  video_fmt_mp4w_alloc_text must be called first.

RETURN VALUE
  None

SIDE EFFECTS
  A modifier atom is allocated on the heap and linked to the text sample.

===========================================================================*/
void video_fmt_mp4w_modify_link_text (
  uint16                         start_char, /* starting offset in string   */
  uint16                         end_char,   /* ending offset in string     */
  const char                     *url,       /* URL for hyperlink           */
  uint8                          url_length, /* length of URL in bytes      */
  const char                     *alt,       /* alternate text for link     */
  uint8                          alt_length, /* length of alt text in bytes */
  void                           *handle,    /* allocated sample handle     */
  video_fmt_status_cb_func_type  callback_ptr,
  void                           *client_data
)
{
    video_fmt_mp4w_context_type      *context;
    video_fmt_mp4w_text_sample_type  *ptr;
    uint8                            *atom;

    MSG_LOW ("modify_link_text: Entered function", 0, 0, 0);

    /* Recover sample and writer context. */
    if (!handle)
    {
        MSG_ERROR ("modify_link_text: Invalid context!", 0, 0, 0);
        MSG_LOW ("modify_link_text: Failed, exiting function", 0, 0, 0);
        return;
    }
    ptr = (video_fmt_mp4w_text_sample_type *) handle;
    context = ptr->context;

    /* Store updated client callback pointer and data, in case the client
    ** wants it change it at this point.
    */
    context->callback_ptr = callback_ptr;
    context->client_data = client_data;

    /* Allocate and link new atom. */
    atom = video_fmt_mp4w_new_text_modifier_atom
        (context, ptr, "href", 8 + 4 + 2 + url_length + alt_length);
    if (!atom)
    {
        MSG_LOW ("modify_link_text: Failed, exiting function", 0, 0, 0);
        return;
    }

    /* Write startcharoffset and endcharoffset. */
    STORE_ATOM_UINT16 (atom, 8, start_char);
    STORE_ATOM_UINT16 (atom, 8 + 2, end_char);

    /* Write URL length and URL. */
    atom [8 + 4] = url_length;
    memcpy (atom + 8 + 5, url, url_length);

    /* Write alternate text length and alternate text. */
    atom [8 + 5 + url_length] = alt_length;
    memcpy (atom + 8 + 6 + url_length, alt, alt_length);

    MSG_LOW ("modify_link_text: Success, exiting function", 0, 0, 0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_fmt_mp4w_modify_box_text

DESCRIPTION
  This procedure is used to apply a text box modifier to a text sample
  constructed with the video_fmt_mp4w_alloc_text function, before writing it
  to the track with the video_fmt_mp4w_write_text function.

DEPENDENCIES
  video_fmt_mp4w_alloc_text must be called first.

RETURN VALUE
  None

SIDE EFFECTS
  A modifier atom is allocated on the heap and linked to the text sample.

===========================================================================*/
void video_fmt_mp4w_modify_box_text (
  int16                          left,
  int16                          top,
  int16                          right,
  int16                          bottom,
  void                           *handle,    /* allocated sample handle     */
  video_fmt_status_cb_func_type  callback_ptr,
  void                           *client_data
)
{
    video_fmt_mp4w_context_type      *context;
    video_fmt_mp4w_text_sample_type  *ptr;
    uint8                            *atom;

    MSG_LOW ("modify_box_text: Entered function", 0, 0, 0);

    /* Recover sample and writer context. */
    if (!handle)
    {
        MSG_ERROR ("modify_box_text: Invalid context!", 0, 0, 0);
        MSG_LOW ("modify_box_text: Failed, exiting function", 0, 0, 0);
        return;
    }
    ptr = (video_fmt_mp4w_text_sample_type *) handle;
    context = ptr->context;

    /* Store updated client callback pointer and data, in case the client
    ** wants it change it at this point.
    */
    context->callback_ptr = callback_ptr;
    context->client_data = client_data;

    /* Allocate and link new atom. */
    atom = video_fmt_mp4w_new_text_modifier_atom
        (context, ptr, "tbox", 8 + 4 * 2);
    if (!atom)
    {
        MSG_LOW ("modify_box_text: Failed, exiting function", 0, 0, 0);
        return;
    }

    /* Write text-box. */
    STORE_ATOM_UINT16 (atom, 8, top);
    STORE_ATOM_UINT16 (atom, 8 + 2, left);
    STORE_ATOM_UINT16 (atom, 8 + 4, bottom);
    STORE_ATOM_UINT16 (atom, 8 + 6, right);

    MSG_LOW ("modify_box_text: Success, exiting function", 0, 0, 0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_fmt_mp4w_modify_blink_text

DESCRIPTION
  This procedure is used to apply a blink modifier to a text sample
  constructed with the video_fmt_mp4w_alloc_text function, before writing it
  to the track with the video_fmt_mp4w_write_text function.

DEPENDENCIES
  video_fmt_mp4w_alloc_text must be called first.

RETURN VALUE
  None

SIDE EFFECTS
  A modifier atom is allocated on the heap and linked to the text sample.

===========================================================================*/
void video_fmt_mp4w_modify_blink_text (
  uint16                         start_char, /* starting offset in string   */
  uint16                         end_char,   /* ending offset in string     */
  void                           *handle,    /* allocated sample handle     */
  video_fmt_status_cb_func_type  callback_ptr,
  void                           *client_data
)
{
    video_fmt_mp4w_context_type      *context;
    video_fmt_mp4w_text_sample_type  *ptr;
    uint8                            *atom;

    MSG_LOW ("modify_blink_text: Entered function", 0, 0, 0);

    /* Recover sample and writer context. */
    if (!handle)
    {
        MSG_ERROR ("modify_blink_text: Invalid context!", 0, 0, 0);
        MSG_LOW ("modify_blink_text: Failed, exiting function", 0, 0, 0);
        return;
    }
    ptr = (video_fmt_mp4w_text_sample_type *) handle;
    context = ptr->context;

    /* Store updated client callback pointer and data, in case the client
    ** wants it change it at this point.
    */
    context->callback_ptr = callback_ptr;
    context->client_data = client_data;

    /* Allocate and link new atom. */
    atom = video_fmt_mp4w_new_text_modifier_atom
        (context, ptr, "blnk", 8 + 4);
    if (!atom)
    {
        MSG_LOW ("modify_blink_text: Failed, exiting function", 0, 0, 0);
        return;
    }

    /* Write startcharoffset and endcharoffset. */
    STORE_ATOM_UINT16 (atom, 8, start_char);
    STORE_ATOM_UINT16 (atom, 8 + 2, end_char);

    MSG_LOW ("modify_blink_text: Success, exiting function", 0, 0, 0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_fmt_mp4w_modify_wrap_text

DESCRIPTION
  This procedure is used to apply a text wrap modifier to a text sample
  constructed with the video_fmt_mp4w_alloc_text function, before writing it
  to the track with the video_fmt_mp4w_write_text function.

DEPENDENCIES
  video_fmt_mp4w_alloc_text must be called first.

RETURN VALUE
  None

SIDE EFFECTS
  A modifier atom is allocated on the heap and linked to the text sample.

===========================================================================*/
void video_fmt_mp4w_modify_wrap_text (
  boolean                        automatic,
  void                           *handle,    /* allocated sample handle     */
  video_fmt_status_cb_func_type  callback_ptr,
  void                           *client_data
)
{
    video_fmt_mp4w_context_type      *context;
    video_fmt_mp4w_text_sample_type  *ptr;
    uint8                            *atom;

    MSG_LOW ("modify_wrap_text: Entered function", 0, 0, 0);

    /* Recover sample and writer context. */
    if (!handle)
    {
        MSG_ERROR ("modify_wrap_text: Invalid context!", 0, 0, 0);
        MSG_LOW ("modify_wrap_text: Failed, exiting function", 0, 0, 0);
        return;
    }
    ptr = (video_fmt_mp4w_text_sample_type *) handle;
    context = ptr->context;

    /* Store updated client callback pointer and data, in case the client
    ** wants it change it at this point.
    */
    context->callback_ptr = callback_ptr;
    context->client_data = client_data;

    /* Allocate and link new atom. */
    atom = video_fmt_mp4w_new_text_modifier_atom
        (context, ptr, "twrp", 8 + 1);
    if (!atom)
    {
        MSG_LOW ("modify_wrap_text: Failed, exiting function", 0, 0, 0);
        return;
    }

    /* Write wrapflag. */
    atom [8] = (automatic ? 1 : 0);

    MSG_LOW ("modify_wrap_text: Success, exiting function", 0, 0, 0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_fmt_mp4w_modify_user_atoms

DESCRIPTION
 Client can dynamically set auth,titl and decsp atoms during recording session. 
DEPENDENCIES
  None
RETURN VALUE
  None

SIDE EFFECTS
  updates the titl,auth,descp atoms

===========================================================================*/

void video_fmt_mp4w_modify_user_atoms(
  video_fmt_user_data_type             *puser_data,
  void                                 *handle,
  video_fmt_status_cb_func_type        callback_ptr,
  void                                 *client_data
)
{
    video_fmt_mp4w_context_type      *context;
    context = (video_fmt_mp4w_context_type*)handle;

    /*Set the flags for writing atoms*/
    context->include_user_data = TRUE;

	/* Initial flag settings */
    context->user_data.atom_is_valid = FALSE;
    context->user_data.titl_auth_desc_valid = FALSE;
    context->user_data.loci.loci_valid = FALSE;

    if(puser_data->atom_is_valid)
    {
    /*Check for atoms validity and update them*/
        if(puser_data->titl_auth_desc_valid)
        {
            if(!video_fmt_mp4w_copy_language_coded_string(
			       &context->user_data.title, &puser_data->title,
                    callback_ptr, client_data) || 
			   !video_fmt_mp4w_copy_language_coded_string(
			       &context->user_data.author, &puser_data->author,
                   callback_ptr, client_data) || 
			   !video_fmt_mp4w_copy_language_coded_string(
			       &context->user_data.description, &puser_data->description,
                   callback_ptr, client_data) || 
			   !video_fmt_mp4w_copy_language_coded_string(
			       &context->user_data.copyright, &puser_data->copyright,
                   callback_ptr, client_data)
            )
            {
                MSG_ERROR ("create: alloc failure!", 0, 0, 0);
            } 
            else
            {
                context->user_data.titl_auth_desc_valid = TRUE;
                context->user_data.atom_is_valid = TRUE;
            }
        }

        /* Check & modify location info */
        if(puser_data->loci.loci_valid)
        {
            if(!video_fmt_mp4w_copy_location 
               (&context->user_data.loci, 
                &puser_data->loci, callback_ptr, client_data)
              )
            {
               MSG_ERROR ("create: alloc failure!", 0, 0, 0);
            }
            else
            {
                context->user_data.loci.loci_valid = TRUE;
                context->user_data.atom_is_valid = TRUE;
            }
        }
    }
}
/* <EJECT> */

/*===========================================================================

FUNCTION video_fmt_mp4w_write_clipinfo

DESCRIPTION
  This function pushes the Qcom platform specific information into the 
  the stream as skip atom

DEPENDENCIES
  Should be called while generating metadata for the clip at the end of 
  recording session

RETURN VALUE
  The value TRUE is returned normally to indicate successful processing.  If
  the value FALSE is returned, it indicates a failure to push the data into the
  stream

SIDE EFFECTS
  None.

===========================================================================*/

boolean video_fmt_mp4w_write_clipinfo(video_fmt_mp4w_context_type *context,
                                   video_fmt_mp4w_stream_type *buffer)
{
    video_fmt_clip_data_type clipdata;
    video_fmt_clipinfo_type  *clipinfo;
    uint32 skipatom_size;
    uint8   temp_buffer[50];
    uint8   *framedata;
    uint32  size;

    /*reset all the variables*/
    memset(&clipdata,0,sizeof(video_fmt_clip_data_type));
    context->callback_ptr (VIDEO_FMT_CLIPINFO_REQ, context->client_data,
                           (video_fmt_status_cb_info_type *)&clipdata,NULL);
    clipinfo = (video_fmt_clipinfo_type *)clipdata.data;
   /* Checks whether clipinfo is available or not */
    if (!clipinfo)
    {
        MSG_LOW ("video_fmt_mp4w_write_clipinfo: video clipinfo feature"
                    " is not enabled", 0,0,0);
        return TRUE;
    }

    //Calculate the total atom size
    skipatom_size = 
                12 // atom head size    
                + 260
                + clipinfo->build_id_len
                + 3  // for AAC recording
                + ((clipinfo->frame_dropped + clipinfo->frame_encoded + 7)/8);
    if(clipinfo->text_size > 0 && clipinfo->text != NULL)
        skipatom_size += clipinfo->text_size;
    if (clipinfo->err_msg && clipinfo->err_msg_size)
    {
        skipatom_size += 16;
        skipatom_size += clipinfo->err_msg_size;
    }

    /* Contruct the atom header */
    MAKE_FULL_ATOM_HEADER (temp_buffer, skipatom_size, "skip",1,0); /*lint !e572 */

    /* Output atom header. */
    if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 12))
    {
       return FALSE;
    }

    if (!video_fmt_mp4w_stream_push (context, buffer, (uint8 *)clipinfo->build_id,
                                        clipinfo->build_id_len))
    {
       return FALSE;
    }
    STORE_ATOM_UINT16 (temp_buffer, 0, clipinfo->vfe_dsp_id);
    STORE_ATOM_UINT16 (temp_buffer, 2, clipinfo->backend_dsp_id);
    temp_buffer[4] = clipinfo->qdspmodule;
    if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer,5))
    {
       return FALSE;
    }

    if (!video_fmt_mp4w_stream_push (context, buffer,(uint8*)clipinfo->sersor_model,
                                            VIDEO_FMT_SERSOR_MODEL_SIZE))
    {
       return FALSE;
    }

    memcpy(temp_buffer,"ENGINE INIT INFO:",17);
    if (!video_fmt_mp4w_stream_push (context, buffer,temp_buffer,17))
    {
       return FALSE;
    }
    temp_buffer[0] = clipinfo->file_format;
    temp_buffer[1] = clipinfo->file_brand;
    temp_buffer[2] = clipinfo->video_format;
    temp_buffer[3] = clipinfo->audio_format;
    STORE_ATOM_UINT16 (temp_buffer, 4, clipinfo->frame_width);
    STORE_ATOM_UINT16 (temp_buffer, 6, clipinfo->frame_height);
    STORE_ATOM_UINT16 (temp_buffer, 8, clipinfo->video_time_increment);
    STORE_ATOM_UINT16 (temp_buffer, 10, clipinfo->video_time_resolution);
    temp_buffer[12] = clipinfo->profile;
    STORE_ATOM_UINT32 (temp_buffer, 13, clipinfo->user_bitrate);
    STORE_ATOM_UINT16 (temp_buffer, 17, clipinfo->rotation_flag);
    temp_buffer[19] = clipinfo->method;
    temp_buffer[20] = clipinfo->short_header;
    temp_buffer[21] = clipinfo->fixed_fps_mode;
    STORE_ATOM_UINT32 (temp_buffer, 22, clipinfo->movie_size_limit);
    STORE_ATOM_UINT32 (temp_buffer, 26, clipinfo->text_size);
    temp_buffer[30] = clipinfo->text_type;
    if (!video_fmt_mp4w_stream_push (context, buffer,temp_buffer,31))
    {
       return FALSE;
    }
    if(clipinfo->text_size > 0 && clipinfo->text != NULL)
    {
        if (!video_fmt_mp4w_stream_push (context, buffer,(uint8*)clipinfo->text,
                                           clipinfo->text_size))
        {
           return FALSE;
        }
    }
    temp_buffer[0] = clipinfo->audio_channel;
    temp_buffer[1] = clipinfo->audio_sample_rate;
    temp_buffer[2] = clipinfo->audio_profile;
    if (!video_fmt_mp4w_stream_push (context, buffer,temp_buffer,3))
    {
       return FALSE;
    }

    STORE_ATOM_UINT32 (temp_buffer, 0, clipinfo->bufferCount);
    temp_buffer[4] = clipinfo->reorder_atom;
    STORE_ATOM_UINT32 (temp_buffer, 5, clipinfo->enc_pkt_size);
    STORE_ATOM_UINT32 (temp_buffer, 9, clipinfo->ved_buf_size);
    STORE_ATOM_UINT32 (temp_buffer, 13, clipinfo->enc_init_inter_memory_size);
    temp_buffer[17] = clipinfo->still_image;
    STORE_ATOM_UINT16 (temp_buffer, 18, clipinfo->audio_rate_set);
    temp_buffer[20] = clipinfo->no_interlace;
    STORE_ATOM_UINT32 (temp_buffer, 21, clipinfo->movie_duration_limit);
    temp_buffer[25] = clipinfo->include_drm;
    STORE_ATOM_UINT16 (temp_buffer, 26, clipinfo->drm_distribution_rights);
    STORE_ATOM_UINT16 (temp_buffer, 28, clipinfo->title_lang);
    STORE_ATOM_UINT16 (temp_buffer, 30, clipinfo->author_lang);
    STORE_ATOM_UINT16 (temp_buffer, 32, clipinfo->description_lang);
    STORE_ATOM_UINT16 (temp_buffer, 34, clipinfo->copyright_lang);
    STORE_ATOM_UINT16 (temp_buffer, 36, clipinfo->loci_lang);
    if (!video_fmt_mp4w_stream_push (context, buffer,temp_buffer,38))
    {
       return FALSE;
    }

    memcpy(temp_buffer,"ENC CFG:",8);
    if (!video_fmt_mp4w_stream_push (context, buffer,temp_buffer,8))
    {
       return FALSE;
    }
    STORE_ATOM_UINT32 (temp_buffer, 0, clipinfo->target_bitrate);
    STORE_ATOM_UINT32 (temp_buffer, 4, clipinfo->intra_period);
    STORE_ATOM_UINT32 (temp_buffer, 8, clipinfo->first_frame_delta);
    temp_buffer[12] = clipinfo->time_unit;
    temp_buffer[13] = clipinfo->rc_ctl;
    if (!video_fmt_mp4w_stream_push (context, buffer,temp_buffer,14))
    {
       return FALSE;
    }

    memcpy(temp_buffer,"ADV CFG:",8);
    if (!video_fmt_mp4w_stream_push (context, buffer,temp_buffer,8))
    {
       return FALSE;
    }
    STORE_ATOM_UINT32 (temp_buffer, 0, clipinfo->adv_config_cookie);
    STORE_ATOM_UINT16 (temp_buffer, 4, clipinfo->quantizer_scale_intra);
    STORE_ATOM_UINT16 (temp_buffer, 6, clipinfo->quantizer_scale_inter);
    temp_buffer[8] = clipinfo->frac_pixel_enable;
    STORE_ATOM_UINT16 (temp_buffer, 9, clipinfo->rcw_overflow_factor);
    STORE_ATOM_UINT16 (temp_buffer, 11, clipinfo->pframe_overflow_factor);
    STORE_ATOM_UINT16 (temp_buffer, 13, clipinfo->max_qp_frame_range_down);
    STORE_ATOM_UINT16 (temp_buffer, 15, clipinfo->max_qp_frame_range_up);
    STORE_ATOM_UINT32 (temp_buffer, 17, clipinfo->log2_nc_square);
    STORE_ATOM_UINT32 (temp_buffer, 21, clipinfo->alpha);
    STORE_ATOM_UINT32 (temp_buffer, 25, clipinfo->beta);
    STORE_ATOM_UINT16 (temp_buffer, 29, clipinfo->quantizer_scale_intra_low);
    STORE_ATOM_UINT16 (temp_buffer, 31, clipinfo->quantizer_scale_intra_high);
    STORE_ATOM_UINT16 (temp_buffer, 33, clipinfo->quantizer_scale_inter_low);
    STORE_ATOM_UINT16 (temp_buffer, 35, clipinfo->quantizer_scale_inter_high);
    if (!video_fmt_mp4w_stream_push (context, buffer,temp_buffer,37))
    {
       return FALSE;
    }

    memcpy(temp_buffer,"DSP CFG:",8);
    if (!video_fmt_mp4w_stream_push (context, buffer,temp_buffer,8))
    {
       return FALSE;
    }
    STORE_ATOM_UINT32 (temp_buffer, 0, clipinfo->dsp_cfg_cookie);
    STORE_ATOM_UINT16 (temp_buffer, 4, clipinfo->intra_16x16_bias);
    STORE_ATOM_UINT16 (temp_buffer, 6, clipinfo->intra_4x4_bias);
    STORE_ATOM_UINT16 (temp_buffer, 8, clipinfo->inter_one_mv_bias);
    STORE_ATOM_UINT16 (temp_buffer, 10, clipinfo->inter_four_mv_bias);
    temp_buffer[12] = clipinfo->search_type;
    temp_buffer[13] = clipinfo->use_16x16_srch_min;
    temp_buffer[14] = clipinfo->use_16x16_anch_pnt;
    temp_buffer[15] = clipinfo->ac_pred_flag;
    temp_buffer[16] = clipinfo->rounding_ctrl;
    temp_buffer[17] = clipinfo->video_hw_me_profile;
    temp_buffer[18] = clipinfo->search_method;
    temp_buffer[19] = clipinfo->luma_filter;
    if (!video_fmt_mp4w_stream_push (context, buffer,temp_buffer,20))
    {
       return FALSE;
    }


    memcpy(temp_buffer,"SPACE INFO:",11);
    if (!video_fmt_mp4w_stream_push (context, buffer,temp_buffer,11))
    {
       return FALSE;
    }

    temp_buffer[0] = clipinfo->save_location;
    STORE_ATOM_UINT32 (temp_buffer,  1, clipinfo->free_space);
    STORE_ATOM_UINT32 (temp_buffer,  5, clipinfo->bytes_available);
    STORE_ATOM_UINT32 (temp_buffer,  9, clipinfo->space_left);
    if (!video_fmt_mp4w_stream_push (context, buffer,temp_buffer,13))
    {
       return FALSE;
    }

    memcpy(temp_buffer,"FRAME INFO:",11);
    if (!video_fmt_mp4w_stream_push (context, buffer,temp_buffer,11))
    {
       return FALSE;
    }

    STORE_ATOM_UINT32 (temp_buffer, 0, clipinfo->frame_dropped);
    STORE_ATOM_UINT32 (temp_buffer, 4, clipinfo->frame_encoded);
    if (!video_fmt_mp4w_stream_push (context, buffer,temp_buffer,8))
    {
       return FALSE;
    }

    if(clipinfo->framedrop_head != NULL)
    {
        clipinfo->framedrop_current = clipinfo->framedrop_head;
        while(clipinfo->framedrop_current != NULL)
        {
            framedata = clipinfo->framedrop_current->data;
            size = (clipinfo->framedrop_current->used_bits + 7) / 8;
            if (!video_fmt_mp4w_stream_push (context, buffer, framedata, size))
            {
               return FALSE;
            }
            clipinfo->framedrop_current = clipinfo->framedrop_current->next;
        }
    }

    if(clipinfo->err_msg && clipinfo->err_msg_size)
    {
        memcpy(temp_buffer,"ERROR/FATAL MSG:",16);
        if (!video_fmt_mp4w_stream_push (context, buffer,temp_buffer,16))
        {
            return FALSE;
        }

        if (!video_fmt_mp4w_stream_push (context, buffer,(uint8*)clipinfo->err_msg,
                                         clipinfo->err_msg_size))
        {
           return FALSE;
        }
    }
    
    return TRUE;
} 
/* <EJECT> */

/*===========================================================================

FUNCTION  video_fmt_mp4w_end

DESCRIPTION
  This function terminates the current MP4 file format session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void video_fmt_mp4w_end
(
  void *server_data
)
{
    video_fmt_mp4w_context_type     *context;
    video_fmt_status_cb_func_type   callback_ptr;
    void                            *client_data;
    video_fmt_status_type           status;
    video_fmt_done_type             done;

    /* Recover context, callback function, and client data, from
    ** server data.
    */
    context = (video_fmt_mp4w_context_type *) server_data;
    callback_ptr = context->callback_ptr;
    client_data = context->client_data;

    /* Close output file and record total movie duration. */
    status = video_fmt_mp4w_close (context);
    done.total_duration = context->duration;

    /* Release video format services context. */
    video_fmt_mp4w_release (context);

    /* Send final indication back to client to indicate that video format
    ** services has finished closing.  Make sure to use local copy of callback
    ** pointer and client data, since all our state has been freed at this
    ** point.
    */
    callback_ptr (status, client_data,
                  (video_fmt_status_cb_info_type *) &done,
                  NULL);
}

/* <EJECT> */
/*===========================================================================

                INTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================

FUNCTION video_fmt_mp4w_update_meta_data

DESCRIPTION
  This procedure is called after data is written to a stream, in order to
  update the stream's meta data structures in memory.

DEPENDENCIES
  video_fmt_mp4w_create must be called first.

RETURN VALUE
  The value TRUE is returned normally to indicate successful processing.  If
  the value FALSE is returned, it indicates a failure to store meta data table.

SIDE EFFECTS
  None

===========================================================================*/
boolean video_fmt_mp4w_update_meta_data (
  video_fmt_mp4w_context_type       *context,
  uint32                            stream_number,
  const video_fmt_sample_info_type  *sample_info
)
{
    video_fmt_mp4w_stream_context_type  *stream;
    uint32                              duration;
    int32                               index;
    int32                               start;
    uint32                              temp_var;

    /* Cache pointer to stream being written. */
    stream = &context->streams [stream_number];

    /* Keep track of largest frame and total bits. */
    stream->samples_largest
        = MAX (stream->samples_largest, sample_info->size);
    stream->bitrate_total_bits += sample_info->size * 8;

    /* Calculate the maximum bitrate only for the first fragment. */
    if (context->fragment_sequence_number == 0)
    {
        /* Convert 'stream->bitrate_window_start' to table index. */
        index = (int32) (stream->bitrate_window_start
                        - stream->stored_samples_size);

        /* If there are samples less than 1 second window in the memory,
        ** only consider them for the maximum bitrate calculation.
        */
        if (index < (-1) * ((int32) stream->max_samples_rate))
        {
            MSG_HIGH ("WARNING: update_meta_data: Samples rate greater than "
                      "max_samples_rate. maxBitrate may be wrong", 0,0,0);

            stream->bitrate_window_time =
                ((stream->fixed_sample_delta > 0)
                ? stream->fixed_sample_delta : sample_info->delta);
            stream->bitrate_window_bits =
                ((stream->fixed_sample_size > 0)
                ? stream->fixed_sample_size : sample_info->size) * 8;

            start = (-1) * ((int32) stream->max_samples_rate);
            index = stream->samples_index - 1;

            while ((index >= start)
                  && (stream->bitrate_window_time <= stream->timescale))
            {
                stream->bitrate_window_time +=
                    ((stream->fixed_sample_delta > 0)
                    ? stream->fixed_sample_delta
                    : stream->samples [index].delta);
                stream->bitrate_window_bits +=
                    ((stream->fixed_sample_size > 0)
                    ? stream->fixed_sample_size
                    : stream->samples [index].size) * 8;
                --index;
            }

            stream->bitrate_window_start
                = (uint32) ((int32) stream->stored_samples_size + index + 1);
        }
        else
        {
            /* Update maximum bitrate calculations. */
            stream->bitrate_window_time +=
                ((stream->fixed_sample_delta > 0)
                ? stream->fixed_sample_delta : sample_info->delta);
            stream->bitrate_window_bits +=
                ((stream->fixed_sample_size > 0)
                ? stream->fixed_sample_size : sample_info->size) * 8;

            /* Move window start until window time is under one second. */
            while ((stream->bitrate_window_time > stream->timescale)
                  && (stream->bitrate_window_start < stream->samples_size))
            {
                stream->bitrate_window_time -=
                    ((stream->fixed_sample_delta > 0)
                    ? stream->fixed_sample_delta
                    : stream->samples [index].delta);
                stream->bitrate_window_bits -= 8 *
                    ((stream->fixed_sample_size > 0)
                    ? stream->fixed_sample_size
                    : stream->samples [index].size);
                ++stream->bitrate_window_start;
                ++index;
            }
        }

        /* Update maximum bitrate calculation. */
        stream->bitrate_max_bits = MAX (stream->bitrate_max_bits,
                                        stream->bitrate_window_bits);
    }

    /* Count number of entries there will be in 'stts' table. */
    if ((stream->fixed_sample_delta == 0)
        && ((0 == stream->stts_size)
            || (sample_info->delta
                != stream->samples [stream->samples_index - 1].delta)))
    {
        ++stream->stts_size;
    }

    /* Add sample table entry. */
    stream->samples [stream->samples_index].size = sample_info->size;
    stream->samples [stream->samples_index].delta = sample_info->delta;
    stream->samples [stream->samples_index].iframe
        = (sample_info->sync ? TRUE : FALSE);
    if (sample_info->sync)
    {
        ++stream->samples_num_iframes;
    }

    /* Count samples and bytes added. */
    ++stream->samples_size;
    ++stream->samples_index;
    stream->chunk_bytes += sample_info->size;

    /* Don't store samples table if the table_stores table is full.
    ** Samples table should remain full to satisfy the table space limit
    ** reached condition.
    */
    if ((stream->table_stores_size < stream->max_table_stores)
        && ((uint32) stream->samples_index == stream->max_samples))
    {
        if (!video_fmt_mp4w_put_meta_table
                (context, stream_number, VIDEO_FMT_MP4W_SAMPLE))
        {
            return FALSE;
        }
    }

    /* Update stream duration. */
    if (stream->fixed_sample_delta == 0)
    {
        duration = sample_info->delta;
    }
    else
    {
        duration = stream->fixed_sample_delta;
    }

    stream->duration += duration;
    stream->total_duration += duration;

    /* Update movie duration if this stream pushes it out. */
    temp_var= video_fmt_mp4w_convert_time_units(stream->duration, stream->timescale, context->timescale);
    context->fragment_duration = MAX( context->fragment_duration,temp_var);

    /* Update chunk description for text stream */
    if (stream->type == VIDEO_FMT_STREAM_TEXT)
    {
        stream->chunks [stream->chunks_size].desc = stream->num_text;
    }

    return TRUE;
}

/*===========================================================================

FUNCTION video_fmt_mp4w_sample_processing

DESCRIPTION
  This procedure is called after data is written to a stream, to check for the
  filling up of the sample table, and to perform chunk processing.

DEPENDENCIES
  video_fmt_mp4w_create must be called first.

RETURN VALUE
  The value TRUE is returned normally to indicate successful processing.  If
  the value FALSE is returned, it indicates a failure to write data to the
  output destination.

SIDE EFFECTS
  None

===========================================================================*/
boolean video_fmt_mp4w_sample_processing (
  video_fmt_mp4w_context_type       *context,
  uint32                            stream_number,
  boolean                           write_chunk,
  boolean                           limit_reached
)
{
    video_fmt_mp4w_stream_context_type  *stream;
    video_fmt_status_type               status;
    video_fmt_done_type                 done;
    /* text sample description updated */
    boolean                             text_desc_updated = FALSE;

    /* Cache pointer to stream being written. */
    stream = &context->streams [stream_number];

    /* Check if sample table will be full. */
    if (((uint32) stream->samples_index == stream->max_samples)
        && (stream->table_stores_size == stream->max_table_stores))
    {
        MSG_HIGH ("sample table for stream %d limit reached",
                  stream_number, 0, 0);
        limit_reached = TRUE;
        context->space_out_status = VIDEO_FMT_SPACE_MSG_SAMPLE_TABLE;
    }

    /* Whether or not stream is interlaced, if num_text is different than the current
    ** chunk desc, write out current chunk for a text stream*/
    if (stream->type == VIDEO_FMT_STREAM_TEXT &&
        stream->samples_size > stream->samples_chunk_start &&
        stream->num_text != stream->chunks [stream->chunks_size].desc)
    {
        text_desc_updated = write_chunk = TRUE;
    }

    /* Perform chunk processing, unless stream is interlaced by
    ** another stream.
    */
    if (((!stream->interlaced) &&
        (stream->samples_size > stream->samples_chunk_start))||
        text_desc_updated)
    {
        status = video_fmt_mp4w_chunk_processing
            (context, stream_number, write_chunk, FALSE);
        switch (status)
        {
          case VIDEO_FMT_FAILURE:
            //  video_fmt_mp4w_failure (context, TRUE);
              return FALSE;

          case VIDEO_FMT_LIMIT_NEAR:
          case VIDEO_FMT_LIMIT_IMMINENT:
              if ((status == VIDEO_FMT_LIMIT_NEAR)
                  && !context->space_out_near)
              {
                  context->space_out_near = TRUE;
                  done.space_out_status = context->space_out_status;
                  context->callback_ptr
                      (VIDEO_FMT_LIMIT_NEAR, context->client_data,
                       (video_fmt_status_cb_info_type *) &done, video_fmt_mp4w_end);
              }
              else if ((status == VIDEO_FMT_LIMIT_IMMINENT)
                       && !context->space_out_imminent)
              {
                  context->space_out_imminent = TRUE;
                  done.space_out_status = context->space_out_status;
                  context->callback_ptr
                      (VIDEO_FMT_LIMIT_IMMINENT, context->client_data,
                       (video_fmt_status_cb_info_type *) &done, video_fmt_mp4w_end);
              }

              /* Fall through on purpose to handle limit reached
              ** possibility.
              */

          case VIDEO_FMT_DONE:
              /* If chunk processing successful, but we have reached the
              ** limit for the sample table, perform same processing as
              ** if limit was reached during chunk processing.  Do this
              ** by falling through to the next case statement.
              */
              if (!limit_reached)
              {
                  break;
              }

          case VIDEO_FMT_LIMIT_REACHED:/*lint!e616 */
              context->space_out = TRUE;
              video_fmt_mp4w_do_limit_reached_callback (context);
              break;
        }
    }
    return TRUE;
}

/*===========================================================================

FUNCTION video_fmt_mp4w_new_text_modifier_atom

DESCRIPTION
  This function allocates memory to hold a new timed text sample modifier
  atom, and links the new atom to the list of modifier atoms attached to the
  given text sample.

DEPENDENCIES
  None.

RETURN VALUE
  If NULL is returned, memory allocation was unsuccessful.  Otherwise, the
  return value points to the memory allocated for the atom's contents.

SIDE EFFECTS
  The newly allocated text sample modifier atom is added to the end of the
  linked list of modifier atoms for the given text sample.

===========================================================================*/
uint8 *video_fmt_mp4w_new_text_modifier_atom (
  video_fmt_mp4w_context_type      *context,
  video_fmt_mp4w_text_sample_type  *sample,
  const char                       *atom_name,
  uint32                           atom_size
)
{
    video_fmt_status_cb_func_type  callback_ptr;
    void                           *client_data;
    video_fmt_alloc_type           alloc;
    video_fmt_free_type            free;
    video_fmt_mp4w_atom_list_type  *ptr;

    /* Cache client's callback pointer and data. */
    callback_ptr = context->callback_ptr;
    client_data = context->client_data;

    /* Attempt to allocate memory for atom list node. */
    alloc.size = sizeof (*ptr);
    context->callback_ptr (VIDEO_FMT_ALLOC, context->client_data,
                           (video_fmt_status_cb_info_type *) &alloc,
                           video_fmt_mp4w_end);
    if (!alloc.ptr)
    {
        MSG_ERROR ("new_text_modifier_atom: alloc failure!", 0, 0, 0);
        video_fmt_mp4w_free_text (sample, callback_ptr, client_data);
        video_fmt_mp4w_release (context);
        callback_ptr (VIDEO_FMT_FAILURE, client_data, NULL, NULL);
        return NULL;
    }
    ptr = (video_fmt_mp4w_atom_list_type *) alloc.ptr;
    ptr->size = atom_size;

    /* Attempt to allocate memory for the atom itself. */
    alloc.size = ptr->size;
    context->callback_ptr (VIDEO_FMT_ALLOC, context->client_data,
                           (video_fmt_status_cb_info_type *) &alloc,
                           video_fmt_mp4w_end);
    if (!alloc.ptr)
    {
        MSG_ERROR ("new_text_modifier_atom: alloc failure!", 0, 0, 0);
        free.ptr = ptr;
        context->callback_ptr
            (VIDEO_FMT_FREE, context->client_data,
             (video_fmt_status_cb_info_type *) &free, NULL);
        video_fmt_mp4w_free_text (sample, callback_ptr, client_data);
        video_fmt_mp4w_release (context);
        callback_ptr (VIDEO_FMT_FAILURE, client_data, NULL, NULL);
        return NULL;
    }
    ptr->atom = (uint8 *) alloc.ptr;

    /* Link atom to other atoms in text sample, at the end of the list. */
    ptr->next = NULL;
    if (sample->modifiers)
    {
        sample->modifiers_end->next = ptr;
    }
    else
    {
        sample->modifiers = ptr;
    }
    sample->modifiers_end = ptr;

    /* Store atom name and size. */
    MAKE_ATOM_HEADER (ptr->atom, atom_size, atom_name);

    /* Return pointer to atom data. */
    return ptr->atom;
}
/*===========================================================================

FUNCTION video_fmt_mp4w_fixup_data

DESCRIPTION
  This function cleans up the meta data so that the resulting audio and video
  in file will be close in le.

DEPENDENCIES
  None.

RETURN VALUE
  If TRUE is returned, fixup was successful
  If FALSE was returned, there was an error, and function did not touch 
  anything

===========================================================================*/

boolean video_fmt_mp4w_fixup_data
(
  video_fmt_mp4w_context_type  *context
)
{ 
  uint32 video_stream_length = 0;
  uint32 audio_stream_length = 0;
  uint32 cur_video_length = 0;

  uint64 video_timescale=0;
  uint32 sample = 0, table =0, index=0, chunk=0;
  int count=0;
  int numframes_in_video = 0;
  int num_video_stream =0;
  int total_frames_in_chunk=0;
  uint64 cumulative_video_length = 0;
  int num_i_frames = 0;
  
  video_fmt_mp4w_table_store_entry_type   *stored;

  /* Go through the streams get video/audio length */
  for(count=0; count < context->num_streams; count++)
  {
      switch(context->streams[count].type)
      {  case VIDEO_FMT_STREAM_VIDEO:
          
          /* Get video length */
          num_video_stream = count;
          if(context->streams[count].timescale)
          {
            video_timescale = (uint64)context->streams[count].timescale;
            video_stream_length =  video_fmt_mp4w_convert_time_units
                                  (context->streams[count].duration,
                                   context->streams[count].timescale, 
                                   1000);
           MSG_HIGH("Video stream length is  %u", video_stream_length,0,0);
          }
          else return FALSE;
          break;
         case VIDEO_FMT_STREAM_AUDIO:
           
           /* Get audio length */
           if(context->streams[count].timescale)
           {
             audio_stream_length = video_fmt_mp4w_convert_time_units
                                   (context->streams[count].duration,
                                    context->streams[count].timescale, 
                                    1000);
             MSG_HIGH("Audio stream length is  %u", audio_stream_length,0,0);
           }
           else return FALSE;
          break;
      }
  }

  /* See if there is a need to fixup the file (if audio and video differ */
  /* by more than 3 seconds  tunable param                               */
  if(video_stream_length > audio_stream_length + 3000 && context->num_streams > 1)
  { /* calculate the number of video frames to keep in the file*/
    
      if(!video_timescale)
         return FALSE;
      MSG_HIGH("Fixing the file video > audio",0,0,0);
      
      /* initialize values */
      context->update_file_data = TRUE;
      context->streams[num_video_stream].stts_size = 1;
      /* We need to read sample table, it can be stored either in */
      /* flash or memory                                          */
      if(context->streams[num_video_stream].stored_samples_table <=1){
            MSG_HIGH("Getting sample table data from local table ",0,0,0);
            for (count = 0; count < context->streams[num_video_stream].samples_size; count++)
            {
                /* Read samples from sample table, add up delta */
                /* until video length >= audio length */
                /* Also keep a tab of how many i-frames we have encountered*/
                if(context->streams[num_video_stream].samples[count].iframe)
                {
                  num_i_frames++;
                }
                /* Calculate length of video in ms */
                cur_video_length = video_fmt_mp4w_convert_time_units
                                            (cumulative_video_length,
                                             context->streams[num_video_stream].timescale, 
                                             1000);

                if(audio_stream_length < cur_video_length )
                {   
                    MSG_HIGH("Number of video frames in file will be %u",numframes_in_video,0,0);
                    numframes_in_video = count;
                    MSG_HIGH("Number of I-frames in file will be %u",num_i_frames,0,0);
                    context->streams[num_video_stream].samples_num_iframes = num_i_frames;
                    MSG_HIGH("Length of video will be %u",cumulative_video_length,0,0);
                
                    break;
                }
                cumulative_video_length = (uint64) cumulative_video_length + 
                                          (uint64) context->streams[num_video_stream].samples[count].delta;
            }
        for(count = 1; count < numframes_in_video; count++)
        {
            /* Count number of entries there will be in 'stts' table. */
            if(context->streams[num_video_stream].samples[count].delta
                != context->streams[num_video_stream].samples[count-1].delta)
            {
                context->streams[num_video_stream].stts_size++;
            }
        }
        MSG_HIGH("Stts size is %u",context->streams[num_video_stream].stts_size,0,0);
      }
      else
      {
        /*read the table from file */
        sample = 0;
        MSG_HIGH("Reading data from temporary file",0,0,0);
        for (table = 0, stored = context->streams[num_video_stream].table_stores;
            table < context->streams[num_video_stream].table_stores_size;
            table++, stored++)
        {
            if (stored->type == VIDEO_FMT_MP4W_SAMPLE)
            { /* check to make sure we are reading sample table */
                if (!video_fmt_mp4w_get_meta_table
                    (context, num_video_stream, stored))
                {
                    return FALSE;
                }

                for (index = 0; index < (int32) stored->items;
                    ++index, ++sample)
                {   
                    /* Read samples from sample table, add up delta */
                    /* until video length >= audio length */
                    /* Also keep a tab of how many i-frames we have encountered*/
                    if(context->streams[num_video_stream].samples[index].iframe)
                    {
                      num_i_frames++;
                    }
                    cur_video_length = video_fmt_mp4w_convert_time_units
                                                (cumulative_video_length,
                                                 context->streams[num_video_stream].timescale, 
                                                 1000);
                    if(audio_stream_length < cur_video_length )
                    {   
                        numframes_in_video = sample;
                        MSG_HIGH("Number of video frames in file will be %u",numframes_in_video,0,0);
                        context->streams[num_video_stream].samples_num_iframes = num_i_frames;
                        MSG_HIGH("Number of I-frames in file will be %u",num_i_frames,0,0);
                        table = context->streams[num_video_stream].table_stores_size;
                        MSG_HIGH("Video length in ms would be %u",cur_video_length,0,0);
                        
                        /* See if last entry in stts should be updated */
                        if(context->streams[num_video_stream].samples[index-1].delta
                           != context->streams[num_video_stream].samples[index].delta && sample)
                        {
                          context->streams[num_video_stream].stts_size++;
                        }

                        MSG_HIGH("Stts size is %u",context->streams[num_video_stream].stts_size,0,0);
                        break;
                    }
                    cumulative_video_length = (uint64) cumulative_video_length + 
                                              (uint64) context->streams[num_video_stream].samples[index].delta;
                    if(context->streams[num_video_stream].samples[index-1].delta
                        != context->streams[num_video_stream].samples[index].delta && sample)
                    {
                        context->streams[num_video_stream].stts_size++;
                    }

                } /* for loop for sample table */
            } /*if type == SAMPLE_TABLE */
        }
      } /* read table from file */


   /* Set the duration of video stream */
   context->streams[num_video_stream].duration = cumulative_video_length;

   context->streams[num_video_stream].stsc_size = 1;

   /* We need to read chunk table, it can be stored either in */
   /* flash or memory                                          */

   if (context->streams[num_video_stream].stored_chunks_table <= 1)
   { /* chunk table stored in memory */
     
     for(count = 0; count <context->streams[num_video_stream].chunks_size; count++)
     { 
       /* calculate how many chunks are required for the number of video */
       /* samples in the new file                                        */
       if( (total_frames_in_chunk  +  context->streams[num_video_stream].chunks[count].samples)
             >= numframes_in_video)
       {   
           context->streams[num_video_stream].chunks[count].samples = 
           numframes_in_video - total_frames_in_chunk;
           MSG_HIGH("Number of samples in last chunk %u", 
                    context->streams[num_video_stream].chunks[count].samples,
                    0,0);
           context->streams[num_video_stream].chunks_size = count+1;
           MSG_HIGH("Total number of video chunks in file %u", count+1,0,0);
           break;
       }
       MSG_HIGH("Video samples in chunk %u is %u", count+1, 
                 context->streams[num_video_stream].chunks[count].samples,0);
       total_frames_in_chunk += context->streams[num_video_stream].chunks[count].samples;
     }
   
     
     context->streams[num_video_stream].stsc_size = 1;
     for(count = 1; count < context->streams[num_video_stream].chunks_size; count++)
     {
      /* Count number of entries there will be in 'stsc' table. */
      if(context->streams[num_video_stream].chunks[count].samples
          != context->streams[num_video_stream].chunks[count-1].samples)
      {
          context->streams[num_video_stream].stsc_size++;
      }         
     }
     MSG_HIGH("Number of entries in video stsc table is %u", context->streams[num_video_stream].stsc_size,0,0);
   }
   else
   { 
      /* read chunk table from file */
      chunk = 0;
      for (table = 0, stored = context->streams[num_video_stream].table_stores;
          table < context->streams[num_video_stream].table_stores_size;
          table++, stored++)
      {
          if (stored->type == VIDEO_FMT_MP4W_CHUNK)
          {   
              /* look for chunk table */
              
            if (!video_fmt_mp4w_get_meta_table
                (context, num_video_stream, stored))
            {
                return FALSE;
            }

            for (index = 0; index < (int32) stored->items;
                ++index, ++chunk)
            {
               /* calculate how many chunks are required for the number of video */
               /* samples in the new file                                        */

                if( (total_frames_in_chunk  +  context->streams[num_video_stream].chunks[index].samples)
                    >= numframes_in_video)
                {   
                    /* Number of samples in last chunk */
                    context->streams[num_video_stream].chunks[index].samples = 
                    numframes_in_video - total_frames_in_chunk;
                    MSG_HIGH("Number of video samples in last chunk is %u", 
                              context->streams[num_video_stream].chunks[index].samples,0,0);
                    context->last_chunk_size = numframes_in_video - total_frames_in_chunk;
                    
                    /* Total number of chunks in new file */
                    context->streams[num_video_stream].chunks_size = chunk+1;
                    MSG_HIGH("Total number of chunks in new file is %u",
                              context->streams[num_video_stream].chunks_size,
                              0,0);
  
                    /* Update stsc size if sample size don't match */
                    if(context->streams[num_video_stream].chunks[index-1].samples
                     != context->streams[num_video_stream].chunks[index].samples && chunk)
                    {
                        context->streams[num_video_stream].stsc_size++;
                    }
                    MSG_HIGH("Total stsc size is %u", 
                              context->streams[num_video_stream].stsc_size,
                              0,0);
                    table = context->streams[num_video_stream].table_stores_size;
                    break;
                }
                total_frames_in_chunk += context->streams[num_video_stream].chunks[index].samples;
                
                /* see if size of sample matches */
                if(context->streams[num_video_stream].chunks[index-1].samples
                   != context->streams[num_video_stream].chunks[index].samples && chunk)
                    {
                        context->streams[num_video_stream].stsc_size++;
                    }
                MSG_HIGH("Video samples in chunk %u is %u", chunk+1, 
                          context->streams[num_video_stream].chunks[index].samples,0);

            }
          }
      }

   }


   
   /* Get fragment duration */
   context->fragment_duration = video_fmt_mp4w_convert_time_units
                                                       (cumulative_video_length,
                                                         video_timescale, 
                                                         context->timescale); 
   /* Set number of video frames */
   context->streams[num_video_stream].samples_size = numframes_in_video;
   context->streams[num_video_stream].samples_chunk_start = numframes_in_video;
   context->streams[num_video_stream].samples_index = numframes_in_video;
   context->streams[num_video_stream].samples_out_near_threshold = numframes_in_video;

  }
 return TRUE;
}
/*===========================================================================

FUNCTION video_fmt_mp4w_push_table

DESCRIPTION
  This function writes the last entries in chunk/sample table to temp file.

DEPENDENCIES
  None.

RETURN VALUE
  If VIDEO_SUCCESS call was successful
  If VIDEO_FAILURE was returned, there was an error

===========================================================================*/

video_status_type video_fmt_mp4w_push_table (
  video_fmt_mp4w_context_type  *context)
{
  video_fmt_mp4w_stream_context_type  *stream;
  int i=0;
    /*-----------------------------------------------------------------------
    ** Store the last samples/chunks table if samples/chunks table has been
    ** stored before and there is any additional entry in the tables in order
    ** to reuse the samples/chunks table space on memory.
    ** Complete storing them now before start reading them in later.
    **-----------------------------------------------------------------------
    */

    for (i = 0; i < context->num_streams; ++i)
    {
        /* Cache pointer to stream being accessed. */
        stream = &context->streams [i];

        /* Store the last samples table. */
        if ((stream->stored_samples_table > 0) && (stream->samples_index > 0))
        {
            if (!video_fmt_mp4w_put_meta_table
                    (context, i, VIDEO_FMT_MP4W_SAMPLE))
            {
              return VIDEO_FAILURE;
            }
        }

        /* Store the last chunks table. */
        if ((stream->stored_chunks_table > 0) && (stream->chunks_index > 0))
        {
            if (!video_fmt_mp4w_put_meta_table
                    (context, i, VIDEO_FMT_MP4W_CHUNK))
            {
              return VIDEO_FAILURE;
            }
        }

        MSG_LOW ("samples | size= %d, stored= %d, table= %d",
                stream->samples_size, stream->stored_samples_size,
                stream->stored_samples_table);
        MSG_LOW ("chunks  | size= %d, stored= %d, table= %d",
                stream->chunks_size, stream->stored_chunks_size,
                stream->stored_chunks_table);
    }
    return VIDEO_SUCCESS;
}
/* <EJECT> */
/*===========================================================================

FUNCTION video_fmt_mp4w_chunk_processing

DESCRIPTION
  This function determines whether or not the engine should output the next
  chunk of data, and if so, outputs the next chunk.

DEPENDENCIES
  None.

RETURN VALUE
  The value returned indicates the result of the processing:
  VIDEO_FMT_DONE - chunk processed successfully
  VIDEO_FMT_FAILURE - indicates critical failure in writing the output file
  VIDEO_FMT_LIMIT_NEAR - chunk processed successfully, but resource limit near
  VIDEO_FMT_LIMIT_IMMINENT - chunk processed successfully, but resource limit
                             imminent
  VIDEO_FMT_LIMIT_REACHED - failed to process chunk, but file is salvageable

SIDE EFFECTS
  None.

===========================================================================*/
video_fmt_status_type video_fmt_mp4w_chunk_processing (
  video_fmt_mp4w_context_type  *context,
  uint32                       stream_number,
  boolean                      write_chunk,
  boolean                      last_chunk
)
{
    video_fmt_mp4w_stream_context_type  *stream;
    video_fmt_mp4w_stream_context_type  *interlaced;
    uint32                              interlaced_stream_number;
    uint32                              samples;
    boolean                             do_chunk = (write_chunk || last_chunk)
                                                 ? TRUE : FALSE;
    boolean                             do_fragment = FALSE;
    boolean                             more_fragments;
    video_fmt_status_type               status;
    uint32                              i;
    uint32                              temp32;
    uint64                              temp64;
    uint32                              space, space2, space3, avg_bitrate;
    video_fmt_space_check_type          space_check;

    MSG_LOW ("chunk_processing: Entered (stream=%d)", stream_number, 0, 0);

    /* Cache pointer to stream being written. */
    stream = &context->streams [stream_number];

    /* This function does not generate meta data if the space limit is
    ** reached but returns right away. This function should be called the
    ** second time with last_chunk = TRUE to generate the meta data.
    */
    if (context->space_out)
    {
        MSG_HIGH ("chunk_processing: Movie limit already reached", 0, 0, 0);
    }

    /* If space low flags are already set, prepare to return
    ** VIDEO_FMT_LIMIT_NEAR or VIDEO_FMT_LIMIT_IMMINENT on success.
    ** Otherwise, just return VIDEO_FMT_DONE.
    */
    if (context->space_out_imminent)
    {
        status = VIDEO_FMT_LIMIT_IMMINENT;
    }
    else if (context->space_out_near)
    {
        status = VIDEO_FMT_LIMIT_NEAR;
    }
    else
    {
        status = VIDEO_FMT_DONE;
    }

    /* If table_stores table is full, check table limit conditions. */
    if (stream->table_stores_size == stream->max_table_stores)
    {
        /* If we are about to run out of space in the stream 'stsz' table,
        ** notify the client.
        */
        if ((status == VIDEO_FMT_DONE)
            && ((stream->max_samples - (uint32) stream->samples_index)
                < stream->samples_out_near_threshold))
        {
            status = VIDEO_FMT_LIMIT_NEAR;
            context->space_out_status = VIDEO_FMT_SPACE_MSG_SAMPLE_TABLE;
        }
        if (((status == VIDEO_FMT_DONE)
            || (status == VIDEO_FMT_LIMIT_NEAR))
            && ((stream->max_samples - (uint32) stream->samples_index)
                < stream->samples_out_imminent_threshold))
        {
            status = VIDEO_FMT_LIMIT_IMMINENT;
            context->space_out_status = VIDEO_FMT_SPACE_MSG_SAMPLE_TABLE;
        }

        /* If we are about to run out of space in the chunk table,
        ** notify the client.
        */
        if ((status == VIDEO_FMT_DONE)
            && ((stream->max_chunks - (uint32) stream->chunks_index)
                < stream->chunks_out_near_threshold))
        {
            status = VIDEO_FMT_LIMIT_NEAR;
            context->space_out_status = VIDEO_FMT_SPACE_MSG_CHUNK_TABLE;
        }
        if (((status == VIDEO_FMT_DONE) || (status == VIDEO_FMT_LIMIT_NEAR))
            && ((stream->max_chunks - (uint32) stream->chunks_index)
                < stream->chunks_out_imminent_threshold))
        {
            status = VIDEO_FMT_LIMIT_IMMINENT;
            context->space_out_status = VIDEO_FMT_SPACE_MSG_CHUNK_TABLE;
        }
    }

    /* Check table spaces for the interlaced streams. */
    interlaced = stream;
    interlaced_stream_number = stream_number;
    while (interlaced->interlace_other != interlaced_stream_number)
    {
        /* Move on to next interlaced stream. */
        interlaced_stream_number = interlaced->interlace_other;
        interlaced = &context->streams [interlaced_stream_number];

        /* If table_stores table is full, check table limit conditions. */
        if (interlaced->table_stores_size == interlaced->max_table_stores)
        {
            /* If we are about to run out of space in the stream 'stsz' table,
            ** notify the client.
            */
            if ((status == VIDEO_FMT_DONE)
                && ((interlaced->max_samples
                    - (uint32) interlaced->samples_index)
                    < interlaced->samples_out_near_threshold))
            {
                status = VIDEO_FMT_LIMIT_NEAR;
                context->space_out_status = VIDEO_FMT_SPACE_MSG_SAMPLE_TABLE;
            }
            if (((status == VIDEO_FMT_DONE)
                || (status == VIDEO_FMT_LIMIT_NEAR))
                && ((interlaced->max_samples
                    - (uint32) interlaced->samples_index)
                    < interlaced->samples_out_imminent_threshold))
            {
                status = VIDEO_FMT_LIMIT_IMMINENT;
                context->space_out_status = VIDEO_FMT_SPACE_MSG_SAMPLE_TABLE;
            }

            /* If we are about to run out of space in the chunk table,
            ** notify the client.
            */
            if ((status == VIDEO_FMT_DONE)
                && ((interlaced->max_chunks
                    - (uint32) interlaced->chunks_index)
                    < interlaced->chunks_out_near_threshold))
            {
                status = VIDEO_FMT_LIMIT_NEAR;
                context->space_out_status = VIDEO_FMT_SPACE_MSG_CHUNK_TABLE;
            }
            if (((status == VIDEO_FMT_DONE)
                    || (status == VIDEO_FMT_LIMIT_NEAR))
                && ((interlaced->max_chunks
                    - (uint32) interlaced->chunks_index)
                    < interlaced->chunks_out_imminent_threshold))
            {
                status = VIDEO_FMT_LIMIT_IMMINENT;
                context->space_out_status = VIDEO_FMT_SPACE_MSG_CHUNK_TABLE;
            }
        }
    }

    /* If the number of samples read in reaches the beginning of the next
    ** 'stsc' run-length encoding reset period, set the reset flag.
    */
    if ((stream->samples_chunk_start / stream->stsc_reset_rate)
        != (stream->samples_size / stream->stsc_reset_rate))
    {
        stream->reset_stsc_algo = TRUE;
    }

    /* Unless the reset flag is set, try to make the next chunk of the file
    ** have the same number of samples as the previous chunk, to minimize the
    ** size of the 'stsc' table.
    */
    samples = stream->samples_size - stream->samples_chunk_start;
    if (!do_chunk
        && (context->fragment_sequence_number == 0)
        && !stream->reset_stsc_algo
        && (samples == stream->chunks [stream->chunks_index - 1].samples))
    {
        do_chunk = TRUE;
    }

    /* If the stream is interlaced, and has reached the interlace period since
    ** the chunk was started, output the chunk.
    */
    if (!do_chunk
        && (stream->interlace_other != stream_number)
        && (stream->duration - stream->chunk_start_time
            >= stream->interlace_rate))
    {
        do_chunk = TRUE;
    }

    /* If the stream has reached the optimal chunk size
    ** in bytes, output the chunk.
    */
    if (!do_chunk
        && (stream->chunk_bytes > stream->optimal_chunk_size))
    {
        do_chunk = TRUE;
    }

    /* If file fragmentation is enabled and the stream has reached the
    ** fragment size, output the chunk.
    */
    if (context->fragment_size > 0)
    {
        /* Convert current stream duration to movie timescale. */
        temp32 = video_fmt_mp4w_convert_time_units
            (stream->duration, stream->timescale, context->timescale);

        /* Output chunk and fragment if stream duration for this fragment has
        ** reached fragment size.
        */
        if (temp32 >= context->fragment_size)
        {
            do_chunk = do_fragment = TRUE;
        }
    }

    /* Check if chunk table will be full. */
    if (do_chunk)
    {
        if (((uint32) stream->chunks_index + 1 == stream->max_chunks)
            && (stream->table_stores_size == stream->max_table_stores))
        {
            status = VIDEO_FMT_LIMIT_REACHED;
            context->space_out_status = VIDEO_FMT_SPACE_MSG_CHUNK_TABLE;
        }

        /* Check the interlaced streams. */
        interlaced = stream;
        interlaced_stream_number = stream_number;
        while (interlaced->interlace_other != interlaced_stream_number)
        {
            /* Move on to next interlaced stream. */
            interlaced_stream_number = interlaced->interlace_other;
            interlaced = &context->streams [interlaced_stream_number];

            if  (((uint32) interlaced->chunks_index + 1
                    == interlaced->max_chunks)
                && (interlaced->table_stores_size
                    == interlaced->max_table_stores))
            {
                status = VIDEO_FMT_LIMIT_REACHED;
                context->space_out_status = VIDEO_FMT_SPACE_MSG_CHUNK_TABLE;
                break;
            }
        }
    }

    /* Estimate the amount of space that would be required to store the
    ** buffered stream data plus the meta-data in the file.
    **
    ** Note that the numbers below are all estimates
    ** based on the atoms actually generated by the engine, and may need to be
    ** adjusted if atoms are added, modified, or removed from the engine
    ** architecture in the future.
    **
    ** Also note that only the current size of the meta-data is accounted
    ** for.  Of course there will be slightly more meta-data if the space is
    ** completely filled up, but it will be at least an order of magnitude
    ** less than the space filled by the actual stream data in that time
    ** window, and so the additional processing required to estimate it is not
    ** worthwhile spending.
    */
    space = 1024 * context->num_streams;  /* rough guess of overhead */
    for (i = 0; i < context->num_streams; ++i)
    {
        /* additional space needed for buffered data, plus footer */
        space += video_fmt_mp4w_stream_amt_full (&context->streams [i].buffer)
            + context->streams [i].max_footer;

        /* (estimated) additional space needed for last sample */
        space += context->streams [i].samples_largest;

        /* additional space needed for video 'stts' table */
        space += 8 * context->streams [i].stts_size;

        /* additional space needed for video 'stco' table */
        space += 4 * context->streams [i].chunks_size;

        /* additional space needed for video 'stsc' table */
        space += 12 * context->streams [i].stsc_size;

        /* additional space needed for video 'stsz' table */
        if (!context->streams [i].fixed_sample_size
            || context->force_stsz_table)
        {
            space += 4 * context->streams [i].samples_size;
        }

        /* additional space needed for video 'stss' table */
        if (context->streams [i].has_inter_frames)
        {
            space += 4 * context->streams [i].samples_num_iframes;
        }
    }

    /* Add the estimated amount of audio/video stream data in the next
    ** threshold window time to space estimate.  If we don't yet have at least
    ** a second of data for a stream, don't include it in the estimate.
    */
	space = (space * EXTRA_PAD_FOR_SPACE_LIMIT) >> 1; /* Increase the reqd space*1.5*/

    space2 = space3 = space;
    for (i = 0; i < context->num_streams; ++i)
    {
        if (context->streams [i].total_duration
            >= context->streams [i].timescale)
        {
            temp64 = context->streams [i].bitrate_total_bits;
            temp64 *= context->streams [i].timescale;
            temp64 /= MAX (1, context->streams [i].total_duration);
            temp64 /= 8;
            avg_bitrate = (uint32) temp64;
            space += avg_bitrate
                * context->movie_size_warning_near_threshold;
            space2 += avg_bitrate
                * context->movie_size_warning_imminent_threshold;
        }
    }

    /* Get number of bytes before threshold is reached. */
    context->callback_ptr (VIDEO_FMT_SPACE_CHECK, context->client_data,
                           (video_fmt_status_cb_info_type *) &space_check,
                           video_fmt_mp4w_end);

    /* If file size limit set, and limit is smaller than actual storage space,
    ** use that instead.
    */
    if (context->movie_size_limit)
    {
        space_check.bytes_available = MIN
            (space_check.bytes_available,
             context->movie_size_limit - context->output_offset);
    }

    /* If we have already reached the limit, notify the client. */
    if (space_check.bytes_available <= space3)
    {
        status = VIDEO_FMT_LIMIT_REACHED;
        context->space_out_status = VIDEO_FMT_SPACE_MSG_STORAGE;
    }

    /* If we have less than the estimated limit near threshold space left,
    ** notify the client that the limit is near.
    */
    if ((status == VIDEO_FMT_DONE)
        && (space >= space_check.bytes_available))
    {
        status = VIDEO_FMT_LIMIT_NEAR;
        context->space_out_status = VIDEO_FMT_SPACE_MSG_STORAGE;
    }

    /* If we have less than the estimated limit imminent threshold space left,
    ** notify the client that the limit is imminent.
    */
    if (((status == VIDEO_FMT_DONE) || (status == VIDEO_FMT_LIMIT_NEAR))
        && (space2 >= space_check.bytes_available))
    {
        status = VIDEO_FMT_LIMIT_IMMINENT;
        context->space_out_status = VIDEO_FMT_SPACE_MSG_STORAGE;
    }

    /* Check for movie duration approaching duration limit. */
    if (context->movie_duration_limit)
    {
        if ((status == VIDEO_FMT_DONE)
            && (context->movie_duration_limit
                - (context->duration + context->fragment_duration)
                < context->movie_size_warning_near_threshold
                * context->timescale))
        {
            status = VIDEO_FMT_LIMIT_NEAR;
            context->space_out_status = VIDEO_FMT_SPACE_MSG_MOVIE_DURATION;
        }
        if (((status == VIDEO_FMT_DONE) || (status == VIDEO_FMT_LIMIT_NEAR))
            && (context->movie_duration_limit
                - (context->duration + context->fragment_duration)
                < context->movie_size_warning_imminent_threshold
                * context->timescale))
        {
            status = VIDEO_FMT_LIMIT_IMMINENT;
            context->space_out_status = VIDEO_FMT_SPACE_MSG_MOVIE_DURATION;
        }
    }

    /* Return right away if the space limit was reached and 'last_chunk' was
    ** not set. 
    */
    if (!last_chunk && status == VIDEO_FMT_LIMIT_REACHED)
    {
        return VIDEO_FMT_LIMIT_REACHED;
    }

    /* Return right away if we aren't ready to output a chunk. */
    if (!do_chunk)
    {
        return status;
    }

    /* Change the status if this is the last chunk. */
    if (last_chunk)
    {
        status = VIDEO_FMT_DONE;
    }

    /* If this is the last chunk, append footers. */
    if (last_chunk)
    {
        /* Append footer for each stream here before starting writing chunk
        ** because it will affect 'mdat' atom size.
        */
        for (i = 0; i < context->num_streams; ++i)
        {
            stream = &context->streams [i];
            if (stream->footer_size > 0)
            {
                video_fmt_mp4w_append_footer(context, i);
            }
        }
    }

    if (last_chunk)
    {
        do_fragment = TRUE;
    }

    /* If this is the last chunk, write out chunks for all streams. */
    if (do_fragment)
    {
        for (i = 0; i < context->num_streams; ++i)
        {
            if (!video_fmt_mp4w_write_chunk (context, i))
            {
                return VIDEO_FMT_FAILURE;
            }
        }
    }

    /* Otherwise, just write out the chunk for this stream. */
    else
    {
        if (!video_fmt_mp4w_write_chunk (context, stream_number))
        {
            return VIDEO_FMT_FAILURE;
        }
    }

    /* Return early if we're not ready to finish up this fragment, or if the
    ** fragment is empty.
    */
    if (!do_fragment || (do_fragment && (context->fragment_duration == 0)))
    {
        return status;
    }

    if ((context->fragment_size > 0) && (!last_chunk))
    {
        more_fragments = TRUE;
    }
    else
    {
        more_fragments = FALSE;
    }
    if(video_fmt_mp4w_push_table(context) != VIDEO_SUCCESS)
    {
      return VIDEO_FMT_FAILURE;
    }

    if(!context->fragment_size && context->enable_fixupdata){
     video_fmt_mp4w_fixup_data(context);     
     
    }
    /* For the first fragment, generate normal meta-data. */
    if (context->fragment_sequence_number == 0)
    {
        if (!video_fmt_mp4w_generate_meta_data (context, more_fragments))
        {
            return VIDEO_FMT_FAILURE;
        }
    }

    /* For all other fragments, generate fragment meta-data. */
    else
    {
        if (!video_fmt_mp4w_generate_fragment (context))
        {
            return VIDEO_FMT_FAILURE;
        }
    }

    /* Increment fragment sequence number, record next fragment start
    ** time, and clear sample/chunk tables.
    */
    ++context->fragment_sequence_number;
    context->duration = video_fmt_mp4w_convert_time_units
                          (context->streams [0].total_duration, 
                           context->streams [0].timescale, 
                           context->timescale);
    for (i = 1; i < context->num_streams; ++i)
    {
        uint64  temp64 =  video_fmt_mp4w_convert_time_units (context->streams [i].total_duration, 
                                                             context->streams [i].timescale, 
                                                             context->timescale);
        context->duration = MAX(context->duration, temp64);
    }

    context->fragment_duration = 0;
    for (i = 0; i < context->num_streams; ++i)
    {
        context->streams [i].duration = 0;
        context->streams [i].chunk_start_time = 0;
        context->streams [i].samples_size = 0;
        context->streams [i].samples_chunk_start = 0;
        context->streams [i].samples_num_iframes = 0;
        context->streams [i].samples_largest = 0;
        context->streams [i].chunks_size = 0;
        context->streams [i].chunk_bytes = 0;
        context->streams [i].stts_size = 0;
        context->streams [i].stsc_size = 0;
        context->streams [i].chunks_index = 0;
        context->streams [i].samples_index = 0;
    }

    /* Return indicator of whether we're near the recording limit, at the
    ** recording limit, or neither.
    */
    return status;
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_fmt_mp4w_write_chunk

DESCRIPTION
  This function outputs the current chunk of data for a stream, and sets up to
  accept more incoming data for the next chunk of the stream.

DEPENDENCIES
  None.

RETURN VALUE
  The value TRUE is returned normally to indicate successful processing.  If
  the value FALSE is returned, it indicates a failure to write data to the
  output destination.

SIDE EFFECTS
  None.

===========================================================================*/
boolean video_fmt_mp4w_write_chunk (
  video_fmt_mp4w_context_type  *context,
  uint32                       stream_number
)
{
    video_fmt_mp4w_stream_context_type  *stream;
    video_fmt_mp4w_stream_context_type  *interlaced;
    uint32                              interlaced_stream_number;
    uint32                              samples;
    /* uint32                           write_size; */
    uint8                               temp_buffer [8];

    MSG_LOW ("write_chunk: entered (stream=%d)", stream_number, 0, 0);

    /* Cache pointer to stream being written. */
    stream = &context->streams [stream_number];

    /* Return without doing anything if there are no samples ready to
    ** write.
    */
    samples = stream->samples_size - stream->samples_chunk_start;
    if (0 == samples)
    {
        return TRUE;
    }

    /* All data in stream buffer will be written in this chunk - reset 'stsc'
    ** algo reset flag, since we've fixed upon the size of this chunk.
    */
    stream->reset_stsc_algo = FALSE;

    /* Update chunk start time. */
    stream->chunk_start_time = stream->duration;

    /* Count number of entries there will be in 'stsc' table. */
    if ((0 == stream->stsc_size)
        || (samples != stream->chunks [stream->chunks_index - 1].samples)
        || (stream->type == VIDEO_FMT_STREAM_TEXT &&
        (stream->chunks [stream->chunks_index - 1].desc !=
        stream->chunks [stream->chunks_index].desc)))
    {
        ++stream->stsc_size;
    }

    /* Add entry to chunk table. */
    stream->chunks [stream->chunks_index].pos = context->output_offset;
    if (!context->mdat_written)
    {
        /* add 8 for 'mdat' header */
        stream->chunks [stream->chunks_index].pos += 8;
        stream->chunk_bytes += 8;
    }
    stream->chunks [stream->chunks_index].samples = samples;

    /* For text streams, description field should already be
    ** set when sample was added to stream buffer; for all
    ** other streams, set description to 1 */
    if (stream->type != VIDEO_FMT_STREAM_TEXT)
    {
        stream->chunks [stream->chunks_index].desc = 1;
    }
    stream->samples_chunk_start += samples;
    ++stream->chunks_size;
    ++stream->chunks_index;

    /* Don't store chunks table if the table_stores table is full.
    ** Chunks table should remain full to satisfy the table space limit
    ** reached condition.
    */
    if ((stream->table_stores_size < stream->max_table_stores)
        && ((uint32) stream->chunks_index == stream->max_chunks))
    {
        if (!video_fmt_mp4w_put_meta_table
                (context, stream_number, VIDEO_FMT_MP4W_CHUNK))
        {
            return FALSE;
        }
    }

    /* Calculate the size of data to write to the media data, which will
    ** contain the data for this chunk, as well as data for all
    ** interlaced chunks.
    */
    /* write_size = stream->chunk_bytes; */
    if (stream->interlace_other != stream_number)
    {
        interlaced = stream;
        while (&context->streams [interlaced->interlace_other]
               != interlaced)
        {
            interlaced = &context->streams [interlaced->interlace_other];
            /* write_size += interlaced->chunk_bytes; */
        }
    }

    /* If the 'mdat' atom header is not yet written, push size of atom in
    ** front of the stream head, along with the atom type.  Add 8 for the
    ** length word and 'mdat' atom identifier.
    */
    if (!context->mdat_written)
    {
        context->mdat_written = TRUE;
        context->mdat_offset = context->output_offset;
        MAKE_ATOM_HEADER (temp_buffer, 0xFFFFFFFF, "mdat");
        video_fmt_mp4w_stream_push_reverse (&stream->buffer, temp_buffer, 8);
    }

    /* Write data from the stream. */
    if (!video_fmt_mp4w_stream_output (context, &stream->buffer,
                                       stream->chunk_bytes))
    {
        return FALSE;
    }
    stream->chunk_bytes = 0;

    /* Write data from any interlaced streams. */
    interlaced = stream;
    interlaced_stream_number = stream_number;
    while (interlaced->interlace_other != interlaced_stream_number)
    {
        /* Move on to next interlaced stream. */
        interlaced_stream_number = interlaced->interlace_other;
        interlaced = &context->streams [interlaced_stream_number];

        /* Skip this stream if no samples are in the buffer. */
        samples = interlaced->samples_size
            - interlaced->samples_chunk_start;
        if (0 == samples)
        {
            continue;
        }

        /* Count number of entries there will be in 'stsc' table. */
        if ((0 == interlaced->stsc_size)
            || (samples != interlaced->chunks [interlaced->chunks_index - 1].samples)
            || (interlaced->type == VIDEO_FMT_STREAM_TEXT &&
            (interlaced->chunks[interlaced->chunks_index-1].desc !=
            interlaced->chunks[interlaced->chunks_index].desc) ) )
        {
            ++interlaced->stsc_size;
        }

        /* Add entry to chunk table. */
        interlaced->chunks [interlaced->chunks_index].pos
            = context->output_offset;
        interlaced->chunks [interlaced->chunks_index].samples = samples;

        /* For text streams, description field should already be
        ** set when sample was added to stream buffer; for all
        ** other streams, set description to 1 */
        if (interlaced->type != VIDEO_FMT_STREAM_TEXT)
        {
            interlaced->chunks [interlaced->chunks_index].desc = 1;
        }
        interlaced->samples_chunk_start += samples;
        ++interlaced->chunks_size;
        ++interlaced->chunks_index;

        /* Don't store chunks table if the table_stores table is full.
        ** Chunks table should remain full to satisfy the table space limit
        ** reached condition.
        */
        if ((interlaced->table_stores_size < interlaced->max_table_stores)
            &&
            ((uint32) interlaced->chunks_index == interlaced->max_chunks))
        {
            if (!video_fmt_mp4w_put_meta_table
                   (context, interlaced_stream_number, VIDEO_FMT_MP4W_CHUNK))
            {
                return FALSE;
            }
        }

        /* Write data from the stream. */
        if (!video_fmt_mp4w_stream_output (context, &interlaced->buffer,
                                            interlaced->chunk_bytes))
        {
            return FALSE;
        }
        interlaced->chunk_bytes = 0;
    }

    return TRUE;
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_fmt_mp4w_append_footer

DESCRIPTION
  This function appends footer to the last sample and fixes its sample size.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void video_fmt_mp4w_append_footer (
  video_fmt_mp4w_context_type  *context,
  uint32                       stream_number
)
{
    video_fmt_mp4w_stream_context_type  *stream;
    uint32                              amt;

    MSG_LOW ("append_footer: stream=%d", stream_number,0,0);

    /* Cache pointer to stream being written. */
    stream = &context->streams [stream_number];

    /* Return without appending footer if there are no samples ready to
    ** write. This case should not be hit. But even if it were hit, the
    ** output file would not be corrupted except that we lose footer.
    */
    if (stream->samples_size == stream->samples_chunk_start)
    {
        MSG_ERROR ("append_footer: No sample to append footer to", 0,0,0);
        return;
    }

    amt = video_fmt_mp4w_stream_amt_full(&stream->buffer);

    /* "amt" should never be less than "stream->chunk_bytes".
    ** This case is an Error and should never be hit!
    */
    if (amt < stream->chunk_bytes)
    {
        MSG_ERROR ("append_footer: less data than known!", 0,0,0);
    }

    /* If "amt" is not equal to "stream->chunk_bytes", force them to be equal
    ** so that footer can be appended in the right place.
    */
    if (amt != stream->chunk_bytes)
    {
        video_fmt_mp4w_stream_truncate(&stream->buffer, stream->chunk_bytes);
    }

    /* Append footer to the last sample and update the last sample info. */
    (void)video_fmt_mp4w_stream_push
        (context, &stream->buffer, stream->footer, stream->footer_size);

    /* If the sample table including the last entry has been stored,
    ** put 'footer_size' in 'last_sample_size_increase'.
    */
    if (stream->samples_index > 0)
    {
        stream->samples [stream->samples_index - 1].size += stream->footer_size;
    }
    else
    {
        stream->last_sample_size_increase = stream->footer_size;
    }
    stream->chunk_bytes += stream->footer_size;
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_fmt_mp4w_generate_meta_data

DESCRIPTION
  This function finishes writing the main fragment of the MP4 output file by
  generating and writing all the meta-data streams and atoms that go in the
  main fragment.

DEPENDENCIES
  None.

RETURN VALUE
  The value TRUE is returned normally to indicate successful processing.  If
  the value FALSE is returned, it indicates a failure to write data to the
  output destination.

SIDE EFFECTS
  None.

===========================================================================*/
boolean video_fmt_mp4w_generate_meta_data
(
  video_fmt_mp4w_context_type  *context,
  boolean                      more_fragments
)
{
    boolean                             include_iods = FALSE;
    uint8                               bifs_track = 0;
    uint8                               od_track = 0;
    video_fmt_mp4w_stream_type          *buffer;
    uint32                              i, j;
    uint32                              sample, chunk, entry, last;
    uint32                              duration;
    video_fmt_mp4w_stream_context_type  *stream;
    uint32                              max_bitrate, avg_bitrate;
    uint8                               temp_buffer [60];
    uint32                              current_time;
    uint64                              temp64;
    video_fmt_mp4w_atom_list_type       *atom;
    uint32                              num_sample_iframes_written;
    uint32                              num_sample_stsz_written;
    uint32                              num_chunk_stsc_written;
    uint32                              num_chunk_stco_written; 
    uint32                              num_sample_stts_written;

    uint32                                  table;
    int32                                   index;
    video_fmt_mp4w_table_store_entry_type   *stored;
    video_fmt_user_data_type                user_atomsdata; 
    /* Atom types and sizes - one for every possible atom and stream
    ** combination, listed here in order from deepest atom level upwards.
    **
    ** NOTE: atoms not listed here are the ones with a well-known fixed size:
    ** 'dinf' - DINF_ATOM = 36 bytes (includes 'dref' and 'url ' atoms)
    ** 'mdhd' - data + MDHD_TRAILER = 32 bytes
    ** 'tkhd' - data + TKHD_TRAILER_... = 92 bytes
    ** 'iods' - IODS_HEADER + data = 33 bytes
    ** 'mvhd' - data + MVHD_TRAILER = 108 bytes
    */
    sample_entry_type_t  sample_entry [VIDEO_FMT_MAX_MEDIA_STREAMS];
    uint8                oti [VIDEO_FMT_MAX_MEDIA_STREAMS];
    uint8                stream_type [VIDEO_FMT_MAX_MEDIA_STREAMS];
    uint32               esds_size [VIDEO_FMT_MAX_MEDIA_STREAMS];
    uint32               sample_entry_size [VIDEO_FMT_MAX_MEDIA_STREAMS];
    uint32               stsd_size [VIDEO_FMT_MAX_MEDIA_STREAMS];
    uint32               stts_size [VIDEO_FMT_MAX_MEDIA_STREAMS];
    uint32               stss_size [VIDEO_FMT_MAX_MEDIA_STREAMS];
    uint32               stco_size [VIDEO_FMT_MAX_MEDIA_STREAMS];
    uint32               stsc_size [VIDEO_FMT_MAX_MEDIA_STREAMS];
    uint32               stsz_size [VIDEO_FMT_MAX_MEDIA_STREAMS];
    uint32               stbl_size [VIDEO_FMT_MAX_MEDIA_STREAMS];
    uint32               media_header_size [VIDEO_FMT_MAX_MEDIA_STREAMS];
    uint32               minf_size [VIDEO_FMT_MAX_MEDIA_STREAMS];
    uint32               hdlr_size [VIDEO_FMT_MAX_MEDIA_STREAMS];
    uint32               mdia_size [VIDEO_FMT_MAX_MEDIA_STREAMS];
    uint32               tref_size [VIDEO_FMT_MAX_MEDIA_STREAMS];
    uint32               trak_size [VIDEO_FMT_MAX_MEDIA_STREAMS];
    uint32               moov_size;
    uint32               mvex_size;
    uint32               udta_size;
    
    for( i=0;i<VIDEO_FMT_MAX_MEDIA_STREAMS;i++)
    {
      sample_entry_size[i]=0;
      media_header_size[i] = 0;
      oti[i]=0;
      stream_type[i]=0;
    } 
    
    for(i=0;i<VIDEO_FMT_MAX_MEDIA_STREAMS;i++)
    {
      sample_entry[i]=(sample_entry_type_t )1;
    } 

    MSG_LOW ("generate_meta_data: entered", 0,0,0);

    /*-----------------------------------------------------------------------
    ** Go through streams looking for ones which have the match_duration flag
    ** set.  For these streams, either lengthen the last frame's duration or
    ** discard frames to make the stream match the duration of the stream to
    ** which it is linked.
    **-----------------------------------------------------------------------
    */
    if (!more_fragments)
    {
        for (i = 0; i < context->num_streams; ++i)
        {
            /* Skip empty tracks. */
            if (context->streams [i].samples_size == 0)
            {
                continue;
            }

            /* Process track if match duration flag is set. */
            j = context->streams [i].match_duration_with;
            if (context->streams [i].match_duration
                && (i != j) && (j < context->num_streams))
            {
                if ((uint64) (context->streams [i].duration)
                    * ((uint64) context->streams [j].timescale)
                    < ((uint64) context->streams [j].duration)
                    * ((uint64) context->streams [i].timescale))
                {
                    /* Calculate amount of time to extend last frame. */
                    const uint32 extend = (uint32)
                        (((uint64) context->streams [j].duration)
                         * ((uint64) context->streams [i].timescale)
                         / ((uint64) context->streams [j].timescale)
                         - ((uint64) context->streams [i].duration));

                    /* Extend last frame if difference was
                    ** significant (!= 0)
                    */
                    if (extend)
                    {
                        /* Calculate affected sample number and the entry in
                        ** the 'stts' atom corresponding to it.
                        */
                        const int32 sample_num
                            = context->streams [i].samples_index - 1;

                        /* Update stream duration. */
                        context->streams [i].duration += extend;

                        /* Add one to 'stts' atom size if the last sample was
                        ** the same size as the previous sample, since now it
                        ** no longer will be.
                        */
                        if ((context->streams [i].samples_size > 1)
                            && (context->streams [i].samples
                                [sample_num - 1].delta
                                == context->streams [i].samples
                                [sample_num].delta))
                        {
                            ++context->streams [i].stts_size;
                        }

                        /* Extend the sample's duration.  If the sample table
                        ** including the last entry has been stored, put the
                        ** extension in 'last_sample_delta_extend'.
                        */
                        if (sample_num >= 0)
                        {
                            context->streams [i].samples [sample_num].delta
                                += extend;
                        }
                        else
                        {
                            context->streams [i].last_sample_delta_extend
                                = extend;
                        }
                    }
                }
            }
        }
    }

    /*-----------------------------------------------------------------------
    ** Determine whether or not to include 'iods' atom - include only if the
    ** movie has both a BIFS and OD track.  At the same time, locate and
    ** remember which tracks are BIFS and OD.
    **-----------------------------------------------------------------------
    */
    for (i = 0; i < context->num_streams; ++i)
    {
        if (context->streams [i].type == VIDEO_FMT_STREAM_DATA)
        {
            if (context->streams [i].subinfo.data.format
                == VIDEO_FMT_STREAM_DATA_BIFS)
            {
                bifs_track = (uint8) (i + 1);
            }
            else if (context->streams [i].subinfo.data.format
                     == VIDEO_FMT_STREAM_DATA_OD)
            {
                od_track = (uint8) (i + 1);
            }
        }
        if (bifs_track && od_track)
        {
            include_iods = TRUE;
            break;
        }
    }

    /*-----------------------------------------------------------------------
    ** Calculate sizes of various atoms that we're about to generate.  We
    ** need to know the sizes in advance in order to output the atom size
    ** fields.  Build sizes by working from the innermost atoms outward.
    **-----------------------------------------------------------------------
    */
    moov_size = 0;
    for (i = 0; i < context->num_streams; ++i)
    {
        /* Skip empty tracks. */
        if (context->streams [i].samples_size == 0)
        {
            continue;
        }

        /* Cache pointer to stream being accessed. */
        stream = &context->streams [i];

        /* Sample entry atom - sometimes there is an inner 'esds' atom,
        ** in the case of MPEG-4 video, MPEG-4 audio, or MPEG-4 data.  Other
        ** times, it is a special atom, such as 'samr' for 3GPP/3GPP2 AMR.
        */
        esds_size [i] = 0;  /* Set 'esds' size to zero initially, so that
                            ** by default no 'esds' atom will be generated.
                            */
        if ((stream->type == VIDEO_FMT_STREAM_DATA)
            || ((stream->type == VIDEO_FMT_STREAM_VIDEO)
                && (stream->subinfo.video.format
                    == VIDEO_FMT_STREAM_VIDEO_MPEG4))
            || ((stream->type == VIDEO_FMT_STREAM_AUDIO)
                && ((stream->subinfo.audio.format
                     == VIDEO_FMT_STREAM_AUDIO_MPEG4_AAC)
                    || (stream->subinfo.audio.format
                        == VIDEO_FMT_STREAM_AUDIO_MPEG1_L3)
                    || (stream->subinfo.audio.format
                        == VIDEO_FMT_STREAM_AUDIO_MPEG2_L3)
                    || (stream->subinfo.audio.format
                        == VIDEO_FMT_STREAM_AUDIO_QCELP13K_FULL)
                    || (stream->subinfo.audio.format
                        == VIDEO_FMT_STREAM_AUDIO_QCELP13K_HALF))))
        {
            /* Calculate size of 'esds' atom. */
            esds_size [i] = 37;  /* fixed part */
            if (stream->header_size > 0)
            {
                esds_size [i] += 3 + stream->header_size;
            }

            /* Sample entry is one of the MPEG-4 types, based on format. */
            switch (stream->type)
            {
            case VIDEO_FMT_STREAM_VIDEO:
                /* Sample entry is 'mp4v'. */
                sample_entry [i] = SAMPLE_ENTRY_MP4V;
                sample_entry_size [i]
                    = 86  /* fixed part */
                    + esds_size [i];
                switch (stream->subinfo.video.format)
                {
                case VIDEO_FMT_STREAM_VIDEO_MPEG4:
                    oti [i] = 0x20; /* Visual ISO/IEC 14496-2 */
                    stream_type [i] = 0x04;  /* Visual Stream */
                    break;

                default:
                    /* If we ever reach this case, we forgot to add code here
                    ** to handle one or more possible cases.
                    */
                    ASSERT (0);
                }
                break;

            case VIDEO_FMT_STREAM_AUDIO:
                /* Sample entry is 'mp4a'. */
                sample_entry [i] = SAMPLE_ENTRY_MP4A;
                sample_entry_size [i]
                    = 36  /* fixed part */
                    + esds_size [i];
                switch (stream->subinfo.audio.format)
                {
                case VIDEO_FMT_STREAM_AUDIO_QCELP13K_FULL:
                case VIDEO_FMT_STREAM_AUDIO_QCELP13K_HALF:
                    oti [i] = 0xE1; /* KDDI's spec for QCELP-13K */
                    stream_type [i] = 0x05;  /* Audio Stream */
                    break;

                case VIDEO_FMT_STREAM_AUDIO_MPEG4_AAC:
                    oti [i] = 0x40; /* Audio ISO/IEC 14496-3 */
                    stream_type [i] = 0x05;  /* Audio Stream */
                    break;

                case VIDEO_FMT_STREAM_AUDIO_MPEG1_L3:
                    oti [i] = 0x6B; /* Audio ISO/IEC 11172-3 */
                    stream_type [i] = 0x05;  /* Audio Stream */
                    break;

                case VIDEO_FMT_STREAM_AUDIO_MPEG2_L3:
                    oti [i] = 0x69; /* Audio ISO/IEC 13818-3 */
                    stream_type [i] = 0x05;  /* Audio Stream */
                    break;

                default:
                    /* If we ever reach this case, we forgot to add code here
                    ** to handle one or more possible cases.
                    */
                    ASSERT (0);
                }
                break;

            case VIDEO_FMT_STREAM_DATA:
                /* Sample entry is 'mp4s'. */
                sample_entry [i] = SAMPLE_ENTRY_MP4S;
                sample_entry_size [i]
                    = 16  /* fixed part */
                    + esds_size [i];
                oti [i] = 0x01;  /* Systems ISO/IEC 14496-1 */
                switch (stream->subinfo.data.format)
                {
                case VIDEO_FMT_STREAM_DATA_BIFS:
                    stream_type [i] = 0x03;  /* SceneDescriptionStream */
                    break;

                case VIDEO_FMT_STREAM_DATA_OD:
                    stream_type [i] = 0x01;  /* ObjectDescriptionStream */
                    break;

                case VIDEO_FMT_STREAM_DATA_SKT_TEXT:
                    oti [i] = 0xD0;  /*object type identifier*/
                                     /* for SKT text track*/
                    stream_type [i] = 0x20; /*stream type for SKT text track*/
                    break;

                default:
                    /* If we ever reach this case, we forgot to add code here
                    ** to handle one or more possible cases.
                    */
                    ASSERT (0);
                }
                break;

            default:
                /* If we ever reach this case, we forgot to add code here to
                ** handle one or more possible cases.
                */
                ASSERT (0);
            }
        }
        else if ((stream->type == VIDEO_FMT_STREAM_VIDEO)
                 && (stream->subinfo.video.format
                     == VIDEO_FMT_STREAM_VIDEO_H263))
        {
            /* Sample entry is 's263'. */
            sample_entry [i] = SAMPLE_ENTRY_S263;
            sample_entry_size [i] = 101;  /* fixed size */
        }
        else if ((stream->type == VIDEO_FMT_STREAM_VIDEO)
                 && (stream->subinfo.video.format
                     == VIDEO_FMT_STREAM_VIDEO_H264))
        {
             sample_entry [i] = SAMPLE_ENTRY_AVC1;
             sample_entry_size [i] = 4+ /* Size field of sample entry box */
                                     sizeof(SAVC1_ATOM_PART_1)+
                                     4 + /* Height and Width take 4 bytes */
                                     sizeof(SAVC1_ATOM_PART_2)+
                                     8+ /* Size and Type of "avcC" atom */
                                     5+ /* Size of first part of "avcC" */
                                     stream->header_size+/* Size of paramSets*/
                                     20 /* Size of entire "btrt" atom */
                                     ;
              /* Size of stream header + size of "btrt" */
        }
        else if ((stream->type == VIDEO_FMT_STREAM_AUDIO)
                 && (stream->subinfo.audio.format
                     == VIDEO_FMT_STREAM_AUDIO_EVRC))
        {
            /* Sample entry is 'sevc'. */
            sample_entry [i] = SAMPLE_ENTRY_SEVC;
            sample_entry_size [i] = 50;  /* fixed size */
        }
        else if ((stream->type == VIDEO_FMT_STREAM_AUDIO)
                 && (stream->subinfo.audio.format
                     == VIDEO_FMT_STREAM_AUDIO_EVRC_B))
        {
            /* Sample entry is 'secb'. */
            sample_entry [i] = SAMPLE_ENTRY_SECB;
            sample_entry_size [i] = 50;  /* fixed size */
        }
        else if ((stream->type == VIDEO_FMT_STREAM_AUDIO)
                 && (stream->subinfo.audio.format
                     == VIDEO_FMT_STREAM_AUDIO_EVRC_WB))
        {
            /* Sample entry is 'secw'. */
            sample_entry [i] = SAMPLE_ENTRY_SECW;
            sample_entry_size [i] = 50;  /* fixed size */
        }
        else if ((stream->type == VIDEO_FMT_STREAM_AUDIO)
                 && (stream->subinfo.audio.format
                     == VIDEO_FMT_STREAM_AUDIO_AMR))
        {
            /* Sample entry is 'samr'. */
            sample_entry [i] = SAMPLE_ENTRY_SAMR;
            sample_entry_size [i] = 53;  /* fixed size */
        }
        else if ((stream->type == VIDEO_FMT_STREAM_AUDIO)
                 && (stream->subinfo.audio.format
                     == VIDEO_FMT_STREAM_AUDIO_AMR_WB))
        {
            /* Sample entry is 'samr'. */
            sample_entry [i] = SAMPLE_ENTRY_SAWB;
            sample_entry_size [i] = 53;  /* fixed size */
        }
        else if ((stream->type == VIDEO_FMT_STREAM_AUDIO)
                 && (stream->subinfo.audio.format
                     == VIDEO_FMT_STREAM_AUDIO_AMR_WB_PLUS))
        {
            /* Sample entry is 'samr'. */
            sample_entry [i] = SAMPLE_ENTRY_SAWP;
            sample_entry_size [i] = 49;  /* fixed size */
        }
        else if ((stream->type == VIDEO_FMT_STREAM_AUDIO)
                 && (stream->subinfo.audio.format
                     == VIDEO_FMT_STREAM_AUDIO_PUREVOICE))
        {
            /* Sample entry is 'sqcp'. */
            sample_entry [i] = SAMPLE_ENTRY_SQCP;
            sample_entry_size [i] = 50;  /* fixed size */
        }
        else if (stream->type == VIDEO_FMT_STREAM_TEXT)
        {
            /* Sample entry is one or more of 'tx3g'. */
            sample_entry [i] = SAMPLE_ENTRY_TX3G;
            sample_entry_size [i] = 0;
            for (atom = stream->text; atom; atom = atom->next)
            {
                sample_entry_size [i] += atom->size;
            }
        }
        else
        {
            MSG_ERROR ("generate_meta_data - unsupported track type!",
                       0, 0, 0);
            return FALSE;
        }

        /* Sample description atom - contains sample entry atom, plus fixed
        ** part of 16 bytes.
        */
        stsd_size [i] = sample_entry_size [i] + 16;

        /* Decoding time to sample atom - fixed part is 16 bytes, and there
        ** are 8 bytes per table entry.  If the table is empty, the stream is
        ** fixed rate, and we still need at least one table entry, which can
        ** cover the entire stream.
        */
        if (0 == stream->stts_size)
        {
            stream->stts_size = 1;
        }
        stts_size [i] = 16 + stream->stts_size * 8;

        /* Sample to chunk atom - fixed part is 16 bytes, and there are 12
        ** bytes per table entry.
        */
        stsc_size [i] = 16 + stream->stsc_size * 12;

        /* Sample size atom - if stream has fixed sample size, we can optimize
        ** out this table with a fixed size of 20 bytes (unless
        ** 'force_stsz_table' flag is set).  Otherwise, the table is 20 fixed
        ** bytes plus 4 bytes per sample in the stream.
        */
        if (stream->fixed_sample_size && !context->force_stsz_table)
        {
            stsz_size [i] = 20;
        }
        else
        {
            stsz_size [i] = 20 + stream->samples_size * 4;
        }

        /* Chunk offset atom - fixed part is 16 bytes, and there are 4 bytes
        ** per table entry (chunk).
        */
        stco_size [i] = 16 + stream->chunks_size * 4;

        /* Sync sample atom - include only if stream has inter frames - if it
        ** does, atom is fixed part of 16 bytes, and there are 4 bytes per
        ** intra frame.
        */
        if (stream->has_inter_frames)
        {
            stss_size [i] = 16 + stream->samples_num_iframes * 4;
        }
        else
        {
            stss_size [i] = 0;
        }

        /* Sample table atom - contains sample description, decode time to
        ** sample, sample to chunk, sample size, chunk offset, and sync sample
        ** atoms, plus an 8 byte header.
        */
        stbl_size [i] = 8 + stsd_size [i] + stts_size [i] + stsc_size [i]
            + stsz_size [i] + stco_size [i] + stss_size [i];

        /* Media header atom - depends on root type of stream. */
        switch (stream->type)
        {
        case VIDEO_FMT_STREAM_VIDEO:
            /* Media header is 'vmhd'. */
            media_header_size [i] = 20;
            break;

        case VIDEO_FMT_STREAM_AUDIO:
            /* Media header is 'smhd'. */
            media_header_size [i] = 16;
            break;

        case VIDEO_FMT_STREAM_DATA:
        case VIDEO_FMT_STREAM_TEXT:
            /* Media header is 'nmhd'. */
            media_header_size [i] = 12;
            break;

        default:
            /* If we ever reach this case, we forgot to add code here to
            ** handle one or more possible cases.
            */
            ASSERT (0);
        }

        /* Media information atom - contains media header, data information,
        ** and sample table atoms, plus an 8 byte header.
        */
        minf_size [i] = 8 + media_header_size [i]
            + sizeof (DINF_ATOM) + stbl_size [i];

        /* Handler reference atom - contains a fixed part of 32 bytes, plus
        ** the size of the handler "name" string for the stream, including the
        ** NULL terminator.
        */
        hdlr_size [i] = 32 + (uint32) strlen (stream->handler) + 1;

        /* Media atom - contains media header atom (fixed size of 32 bytes),
        ** plus handler reference and media information atoms, plus an 8 byte
        ** header.
        */
        mdia_size [i] = 8 + 32 + hdlr_size [i] + minf_size [i];

        /* Track reference atom - include only if we have at least one track
        ** reference.  If included, it contains a fixed part of 16 bytes, plus
        ** 4 bytes for each track reference.
        */
        if (stream->ref_track.track_count)
        {
            tref_size [i] = 16
                + stream->ref_track.track_count * 4;
        }
        else
        {
            tref_size [i] = 0;
        }

        /* Track atom - contains track header atom (fixed size of 92 bytes),
        ** plus track reference atom (if any), plus media atom, plus an 8 byte
        ** header.
        */
        trak_size [i] = 8 + 92 + tref_size [i] + mdia_size [i];

        /* Add size of track atom to size of movie atom. */
        moov_size += trak_size [i];
    }

    /* If this file will be fragmented, add a movie extension atom. */
    if (more_fragments)
    {
        mvex_size = 8                     /* 'mvex' atom header */
            + 16                          /* 'mehd' atom        */
            + context->num_streams * 32;  /* 'trex' atoms       */
    }
    else
    {
        mvex_size = 0;
    }

    /* Calculate size of "udta" atom, if any. */
    udta_size = 0;

    /*reset all the variables*/
    memset(&user_atomsdata,0,sizeof(video_fmt_user_data_type));

    /* recording session has ended.
    **Try calling client call back funciton to check if there are
    **titl,auth,descp atoms to be updated.
    */  
    context->callback_ptr (VIDEO_FMT_UDTA_ATOM_REQ, context->client_data,
                           (video_fmt_status_cb_info_type *)&user_atomsdata,NULL);

    /*Check if we have valid atoms*/
    if(user_atomsdata.atom_is_valid)
    {
        video_fmt_mp4w_modify_user_atoms(&user_atomsdata,context,context->callback_ptr,
                                         context->client_data);
    }

    if (context->include_user_data)
    {
        /* Include atom header. */
        udta_size += 8;

        /*if midi present, include midi atom header and data*/
        if (context->user_data.midiSize)
        {
            udta_size += 12 + context->user_data.midiSize;
        }

        if (context->user_data.titl_auth_desc_valid)
        {
            /* Count size of title atom. */
            if (context->user_data.title.isUTF16)
            {
                udta_size += 12 + 2
                    + (context->user_data.title.length * sizeof (short));
            }
            else
            {
                udta_size += 12 + 2
                    + (context->user_data.title.length * sizeof (char));
            }

            /* Count size of author atom. */
            if (context->user_data.author.isUTF16)
            {
                udta_size += 12 + 2
                    + (context->user_data.author.length * sizeof (short));
            }
            else
            {
                udta_size += 12 + 2
                    + (context->user_data.author.length * sizeof (char));
            }

            /* Count size of description atom. */
            if (context->user_data.description.isUTF16)
            {
                udta_size += 12 + 2
                    + (context->user_data.description.length * sizeof (short));
            }
            else
            {
                udta_size += 12 + 2
                    + (context->user_data.description.length * sizeof (char));
            }

            /* Count size of copyright atom. */
            if (context->user_data.copyright.isUTF16)
            {
                udta_size += 12 + 2
                    + (context->user_data.copyright.length * sizeof (short));
            }
            else
            {
                udta_size += 12 + 2
                    + (context->user_data.copyright.length * sizeof (char));
            }
        }

        if (context->user_data.loci.loci_valid)
		{
            /* Count size of loci atom. */
            if (context->user_data.loci.name.isUTF16)
            {
                udta_size += 12 + 2 + 1 + 12
                    + ((context->user_data.loci.name.length 
                    +  context->user_data.loci.astr_body.length
                    +  context->user_data.loci.add_notes.length ) * sizeof (short));
            }
            else
            {
                udta_size += 12 + 2 + 1 + 12
                    + (context->user_data.loci.name.length 
                    +  context->user_data.loci.astr_body.length
                    +  context->user_data.loci.add_notes.length
                      ) * sizeof (char);
            }
        }
    }

    /* We've been adding the sizes of each stream's track atom to the total
    ** size of the movie atom.  Now add in the fixed parts, consisting of a
    ** movie header atom (fixed size of 108 bytes), an initial object
    ** descriptor, if any (fixed size of 33 bytes), an 8 byte header,
    ** and a movie extends atom, if any, and a user data atom, if any.
    */
    moov_size += 108 + 8 + mvex_size + udta_size;
    if (include_iods)
    {
        moov_size += 33;
    }
    if (context->include_drm)
    {
        moov_size += 18;
    }

        buffer = &context->meta_data_buffer;

    /* First clean the buffer up, in case there is some garbage left. */
    video_fmt_mp4w_stream_reset(buffer);

    /* Go back and write the total size of the media data atom. */
    if (context->mdat_written)
    {
        STORE_ATOM_UINT32 (temp_buffer, 0,
                           context->output_offset - context->mdat_offset);
        i = context->output_offset;
        context->output_offset = context->mdat_offset;
        if (!video_fmt_mp4w_write_movie (context, temp_buffer, 4))
        {
            return FALSE;
        }
        context->output_offset = i;
        context->mdat_written = FALSE;
    }

    /*-----------------------------------------------------------------------
    ** Generate movie atom.
    **-----------------------------------------------------------------------
    */

    /* Output movie atom header. */
    MAKE_ATOM_HEADER (temp_buffer, moov_size, "moov");
    if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 8))
    {
        return FALSE;
    }

    /*-----------------------------------------------------------------------
    ** Generate movie header atom.
    **-----------------------------------------------------------------------
    */

    /* Output movie header atom header. */
    MAKE_FULL_ATOM_HEADER (temp_buffer, 108, "mvhd", 0, 0); /*lint !e572 */
    if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 12))
    {
        return FALSE;
    }

    /* Output creation timestamp - current date/time in seconds past Midnight,
    ** January 1, 1904.  Note that this will cease to work after 19:14:07
    ** January 18, 2038 UDT.
    */
    /* 2398809600 is the number of seconds between the GPS/IS95 timebase of
    ** Midnight, January 6, 1980, and the MPEG-4 timebase of Midnight, January
    ** 1, 1904.
    */
    current_time = clk_read_secs ();
    current_time += 2398809600UL;
    STORE_ATOM_UINT32 (temp_buffer, 0, current_time);
    if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 4))
    {
        return FALSE;
    }

    /* Output modification timestamp - set to be same as creation
    ** timestamp.
    */
    if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 4))
    {
        return FALSE;
    }

    /* Output movie timescale and duration. */
    STORE_ATOM_UINT32 (temp_buffer, 0, context->timescale);
    STORE_ATOM_UINT32 (temp_buffer, 4, context->fragment_duration);
    if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 8))
    {
        return FALSE;
    }

    /* Output tailer of movie header atom. */
    if (!video_fmt_mp4w_stream_push
        (context, buffer, MVHD_TRAILER, sizeof (MVHD_TRAILER)))
    {
        return FALSE;
    }

    /* Output next track ID (depends on number of tracks) */
    STORE_ATOM_UINT32 (temp_buffer, 0, context->num_streams + 1);
    if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 4))
    {
        return FALSE;
    }

    /*-----------------------------------------------------------------------
    ** Generate initial object descriptor atom, if present.
    **-----------------------------------------------------------------------
    */
    if (include_iods)
    {
        /* Output atom header. */
        if (!video_fmt_mp4w_stream_push
            (context, buffer, IODS_HEADER, sizeof (IODS_HEADER)))
        {
            return FALSE;
        }

        /* Assemble initial object descriptor contents. */
        temp_buffer [4] = 0xFF;  /* OD profile = no OD capability required */
        temp_buffer [5] = 0x01;  /* Scene profile = Simple2D/Level 1 */
        temp_buffer [6] = 0xFF;  /* audio profile = no audio capability req. */
        temp_buffer [7] = 0x03;  /* visual profile = Simple/Level 1 */
        temp_buffer [8] = 0xFF;  /* graph profile = no graph capability req. */

        /* Assemble ES ID for BIFS track. */
        temp_buffer [9] = 0x0E;  /* ES_IDIncTag */
        temp_buffer [10] = 0x04; /* Size = 4 (0x04) */
        STORE_ATOM_UINT32 (temp_buffer, 11, bifs_track); /*lint !e572 */

        /* Assemble ES ID for OD track. */
        temp_buffer [15] = 0x0E;  /* ES_IDIncTag */
        temp_buffer [16] = 0x04; /* Size = 4 (0x04) */
        STORE_ATOM_UINT32 (temp_buffer, 17, od_track);/*lint !e572 */

        /* Output assembled IODS atom remainder. */
        if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 21))
        {
            return FALSE;
        }
    }

    /*-----------------------------------------------------------------------
    ** Generate NTT DoCoMo DRM atoms, if present.
    **-----------------------------------------------------------------------
    */
    if (context->include_drm)
    {
        /* Output atom header. */
        if (!video_fmt_mp4w_stream_push
            (context, buffer, DRM_HEADER, sizeof (DRM_HEADER)))
        {
            return FALSE;
        }

        /* Output distribution rights word. */
        STORE_ATOM_UINT16 (temp_buffer, 0, context->drm_distribution_rights);
        if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 2))
        {
            return FALSE;
        }
    }

    /*-----------------------------------------------------------------------
    ** Generate track atom for each stream, one at a time.
    **-----------------------------------------------------------------------
    */
    for (i = 0; i < context->num_streams; ++i)
    {
        /* Skip empty tracks. */
        if (context->streams [i].samples_size == 0)
        {
            continue;
        }

        /* Cache pointer to stream being accessed. */
        stream = &context->streams [i];

        /* Output atom header. */
        MAKE_ATOM_HEADER (temp_buffer, trak_size [i], "trak");
        if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 8))
        {
            return FALSE;
        }

        /*-------------------------------------------------------------------
        ** Generate track header atom.
        **-------------------------------------------------------------------
        */

        /* Output atom header. */
        MAKE_FULL_ATOM_HEADER (temp_buffer, 92, "tkhd", 0, 1); /*lint !e572 */
        if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 12))
        {
            return FALSE;
        }

        /* Output same creation/modification time as in movie header. */
        STORE_ATOM_UINT32 (temp_buffer, 0, current_time);
        STORE_ATOM_UINT32 (temp_buffer, 4, current_time);
        if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 8))
        {
            return FALSE;
        }

        /* Output track ID, reserved field, and duration.  Note that duration
        ** is given in movie timescale, not track/stream timescale.
        */
        STORE_ATOM_UINT32 (temp_buffer, 0, i + 1);
        STORE_ATOM_UINT32 (temp_buffer, 4, 0);   /*lint !e572 */
        duration = video_fmt_mp4w_convert_time_units
            (stream->duration, stream->timescale, context->timescale);
        STORE_ATOM_UINT32 (temp_buffer, 8, duration);
        if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 12))
        {
            return FALSE;
        }

        /* Output track header atom trailer. */
        switch (stream->type)
        {
        case VIDEO_FMT_STREAM_VIDEO:
        case VIDEO_FMT_STREAM_TEXT:
            STORE_ATOM_UINT32 (temp_buffer, 0, 0);/*lint !e572 */ /* reserved */  /*lint !e572 */
            STORE_ATOM_UINT32 (temp_buffer, 4, 0); /*lint !e572 */ /* reserved */
            STORE_ATOM_UINT16 (temp_buffer, 8, stream->tkhd_layer);
            STORE_ATOM_UINT16 (temp_buffer, 10, 0);/*lint !e572 */ /* alternate group */
            STORE_ATOM_UINT16 (temp_buffer, 12, 0);/*lint !e572 */ /* volume */
            STORE_ATOM_UINT16 (temp_buffer, 14, 0); /*lint !e572 *//* reserved */
            STORE_ATOM_UINT32 (temp_buffer, 16, 0x00010000);/*lint !e572 */ /* matrix[0] */
            STORE_ATOM_UINT32 (temp_buffer, 20, 0); /*lint !e572 *//* matrix[1] */
            STORE_ATOM_UINT32 (temp_buffer, 24, 0);/*lint !e572 */ /* matrix[2] */
            STORE_ATOM_UINT32 (temp_buffer, 28, 0);/*lint !e572 */ /* matrix[3] */
            STORE_ATOM_UINT32 (temp_buffer, 32, 0x00010000);/*lint !e572 */ /* matrix[4] */
            STORE_ATOM_UINT32 (temp_buffer, 36, 0);/*lint !e572 */ /* matrix[5] */
            STORE_ATOM_UINT32 (temp_buffer, 40, stream->tkhd_origin_x);
            STORE_ATOM_UINT32 (temp_buffer, 44, stream->tkhd_origin_y);
            STORE_ATOM_UINT32 (temp_buffer, 48, 0x40000000); /* matrix[8] */
            STORE_ATOM_UINT32 (temp_buffer, 52, stream->tkhd_width);
            STORE_ATOM_UINT32 (temp_buffer, 56, stream->tkhd_height);
            if (!video_fmt_mp4w_stream_push
                (context, buffer, temp_buffer, 60))
            {
                return FALSE;
            }
            break;

        case VIDEO_FMT_STREAM_AUDIO:
            if (!video_fmt_mp4w_stream_push
                (context, buffer, TKHD_TRAILER_AUDIO,
                 sizeof (TKHD_TRAILER_AUDIO)))
            {
                return FALSE;
            }
            break;

        case VIDEO_FMT_STREAM_DATA:
            if (!video_fmt_mp4w_stream_push
                (context, buffer, TKHD_TRAILER_MPEG,
                 sizeof (TKHD_TRAILER_MPEG)))
            {
                return FALSE;
            }
            break;

        default:
            /* If we ever reach this case, we forgot to add code here to
            ** handle one or more possible cases.
            */
            ASSERT (0);
        }

        /*-------------------------------------------------------------------
        ** Generate track reference atom, if any.
        **-------------------------------------------------------------------
        */
        if (tref_size [i] > 0)
        {
            /* Output atom header. */
            MAKE_ATOM_HEADER (temp_buffer, tref_size [i], "tref");
            if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 8))
            {
                return FALSE;
            }

            /* Output track reference type atom. */
            STORE_ATOM_UINT32 (temp_buffer, 0,
                               stream->ref_track.track_count * 4);
            STORE_ATOM_UINT32 (temp_buffer, 4, stream->ref_track.ref_atom);
            if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 8))
            {
                return FALSE;
            }

            /* Output track IDs. */
            for (j = 0; j < stream->ref_track.track_count; ++j)
            {
                STORE_ATOM_UINT32 (temp_buffer, j * 4,
                                   stream->ref_track.track_id [j]);
            }
            if (!video_fmt_mp4w_stream_push
                (context, buffer, temp_buffer,
                 stream->ref_track.track_count * 4))
            {
                return FALSE;
            }
        }

        /*-------------------------------------------------------------------
        ** Generate media atom.
        **-------------------------------------------------------------------
        */

        /* Output atom header. */
        MAKE_ATOM_HEADER (temp_buffer, mdia_size [i], "mdia");
        if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 8))
        {
            return FALSE;
        }

        /*-------------------------------------------------------------------
        ** Generate media header atom.
        **-------------------------------------------------------------------
        */

        /* Output atom header. */
        MAKE_FULL_ATOM_HEADER (temp_buffer, 32, "mdhd", 0, 0); /*lint !e572 */
        if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 12))
        {
            return FALSE;
        }

        /* Output same creation/modification time as in movie header. */
        STORE_ATOM_UINT32 (temp_buffer, 0, current_time);
        STORE_ATOM_UINT32 (temp_buffer, 4, current_time);
        if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 8))
        {
            return FALSE;
        }

        /* Output timescale and duration. */
        STORE_ATOM_UINT32 (temp_buffer, 0, stream->timescale);
        STORE_ATOM_UINT32 (temp_buffer, 4, stream->duration);
        STORE_ATOM_UINT16 (temp_buffer, 8, (stream->language & 0x7FFF));
        if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 10))
        {
            return FALSE;
        }

        /* Output video track movie header trailer. */
        if (!video_fmt_mp4w_stream_push
            (context, buffer, MDHD_TRAILER, sizeof (MDHD_TRAILER)))
        {
            return FALSE;
        }

        /*-------------------------------------------------------------------
        ** Generate handler reference atom.
        **-------------------------------------------------------------------
        */

        /* Output atom header. */
        MAKE_FULL_ATOM_HEADER (temp_buffer, hdlr_size [i], "hdlr", 0, 0); /*lint !e572 */
        if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 12))
        {
            return FALSE;
        }

        /* Output 32-bit reserved field, handler type, and 12 8-bit reserved
        ** fields. */
        STORE_ATOM_UINT32 (temp_buffer, 0, 0); /*lint !e572 */
        switch (stream->type)
        {
        case VIDEO_FMT_STREAM_VIDEO:
            STORE_ATOM_ID (temp_buffer, 4, "vide");
            break;

        case VIDEO_FMT_STREAM_AUDIO:
            STORE_ATOM_ID (temp_buffer, 4, "soun");
            break;

        case VIDEO_FMT_STREAM_TEXT:
            STORE_ATOM_ID (temp_buffer, 4, "text");
            break;

        case VIDEO_FMT_STREAM_DATA:
            switch (stream->subinfo.data.format)
            {
            case VIDEO_FMT_STREAM_DATA_BIFS:
                STORE_ATOM_ID (temp_buffer, 4, "sdsm");
                break;

            case VIDEO_FMT_STREAM_DATA_OD:
                STORE_ATOM_ID (temp_buffer, 4, "odsm");
                break;

            case VIDEO_FMT_STREAM_DATA_SKT_TEXT:
                STORE_ATOM_ID (temp_buffer, 4, "sktt");
                break;

            default:
                /* If we ever reach this case, we forgot to add code here to
                ** handle one or more possible cases.
                */
                ASSERT (0);
            }
            break;

        default:
            /* If we ever reach this case, we forgot to add code here to
            ** handle one or more possible cases.
            */
            ASSERT (0);
        }
        for (j = 0; j < 12; ++j)
        {
            temp_buffer [8 + j] = 0x00;
        }
        if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 20))
        {
            return FALSE;
        }

        /* Output handler name. */
        if (!video_fmt_mp4w_stream_push
            (context, buffer, (const uint8 *) stream->handler,
             (uint32) strlen (stream->handler) + 1))
        {
            return FALSE;
        }

        /*-------------------------------------------------------------------
        ** Generate media information atom.
        **-------------------------------------------------------------------
        */

        /* Output atom header. */
        MAKE_ATOM_HEADER (temp_buffer, minf_size [i], "minf");
        if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 8))
        {
            return FALSE;
        }

        /*-------------------------------------------------------------------
        ** Generate media header atom.
        **-------------------------------------------------------------------
        */

        /* Output different atom based on stream type. */
        switch (stream->type)
        {
        case VIDEO_FMT_STREAM_VIDEO:
            if (!video_fmt_mp4w_stream_push
                (context, buffer, VMHD_ATOM, sizeof (VMHD_ATOM)))
            {
                return FALSE;
            }
            break;

        case VIDEO_FMT_STREAM_AUDIO:
            if (!video_fmt_mp4w_stream_push
                (context, buffer, SMHD_ATOM, sizeof (SMHD_ATOM)))
            {
                return FALSE;
            }
            break;

        case VIDEO_FMT_STREAM_TEXT:
        case VIDEO_FMT_STREAM_DATA:
            if (!video_fmt_mp4w_stream_push
                (context, buffer, NMHD_ATOM, sizeof (NMHD_ATOM)))
            {
                return FALSE;
            }
            break;

        default:
            /* If we ever reach this case, we forgot to add code here to
            ** handle one or more possible cases.
            */
            ASSERT (0);
        }

        /*-------------------------------------------------------------------
        ** Generate data information atom.
        **-------------------------------------------------------------------
        */

        /* Output complete atom - this one is completely static. */
        if (!video_fmt_mp4w_stream_push
            (context, buffer, DINF_ATOM, sizeof (DINF_ATOM)))
        {
            return FALSE;
        }

        /*-------------------------------------------------------------------
        ** Generate sample table atom.
        **-------------------------------------------------------------------
        */

        /* Output atom header. */
        MAKE_ATOM_HEADER (temp_buffer, stbl_size [i], "stbl");
        if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 8))
        {
            return FALSE;
        }

        /*-------------------------------------------------------------------
        ** Generate sample description atom.
        **-------------------------------------------------------------------
        */

        /* Output atom header and entry count. */
        MAKE_FULL_ATOM_HEADER (temp_buffer, stsd_size [i], "stsd", 0, 0); /*lint !e572 */
        if (stream->type == VIDEO_FMT_STREAM_TEXT)
        {
            STORE_ATOM_UINT32 (temp_buffer, 12, stream->num_text);
        }
        else
        {
            STORE_ATOM_UINT32 (temp_buffer, 12, 1);/*lint !e572 */
        }
        if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 16))
        {
            return FALSE;
        }

        /*-------------------------------------------------------------------
        ** Generate sample entry atom.
        **-------------------------------------------------------------------
        */

        /* Output atom header. */
        if (stream->type != VIDEO_FMT_STREAM_TEXT)
        {
            STORE_ATOM_UINT32 (temp_buffer, 0, sample_entry_size [i]);
            if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 4))
            {
                return FALSE;
            }
        }
        switch (sample_entry [i])
        {
        case SAMPLE_ENTRY_MP4V:
            /* First part is boilerplate. */
            if (!video_fmt_mp4w_stream_push
                (context, buffer, MP4V_ATOM_PART_1,
                 sizeof (MP4V_ATOM_PART_1)))
            {
                return FALSE;
            }

            /* 32-bit reserved field at this point is used as width/height in
            ** some file formats...
            */
            STORE_ATOM_UINT16 (temp_buffer, 0, stream->subinfo.video.width);
            STORE_ATOM_UINT16 (temp_buffer, 2, stream->subinfo.video.height);
            if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 4))
            {
                return FALSE;
            }

            /* Latter part is also boilerplate. */
            if (!video_fmt_mp4w_stream_push
                (context, buffer, MP4V_ATOM_PART_2,
                 sizeof (MP4V_ATOM_PART_2)))
            {
                return FALSE;
            }
            break;

        case SAMPLE_ENTRY_MP4A:
            /* First part is boilerplate. */
            if (!video_fmt_mp4w_stream_push
                (context, buffer, MP4A_ATOM_PART_1,
                 sizeof (MP4A_ATOM_PART_1)))
            {
                return FALSE;
            }

            /* Lower 16 bits of stream timescale is repeated here. */
            STORE_ATOM_UINT16 (temp_buffer, 0, (uint16) stream->timescale);
            if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 2))
            {
                return FALSE;
            }

            /* Latter part is also boilerplate. */
            if (!video_fmt_mp4w_stream_push
                (context, buffer, MP4A_ATOM_PART_2,
                 sizeof (MP4A_ATOM_PART_2)))
            {
                return FALSE;
            }
            break;

        case SAMPLE_ENTRY_MP4S:
            /* Entire atom is boilerplate. */
            if (!video_fmt_mp4w_stream_push
                (context, buffer, MP4S_ATOM_BODY, sizeof (MP4S_ATOM_BODY)))
            {
                return FALSE;
            }
            break;

        case SAMPLE_ENTRY_SAMR:
        case SAMPLE_ENTRY_SAWB:
            /* First part is boilerplate. */
            if (sample_entry [i] == SAMPLE_ENTRY_SAMR)
            {
                if (!video_fmt_mp4w_stream_push
                    (context, buffer, SAMR_ATOM_PART_1,
                     sizeof (SAMR_ATOM_PART_1)))
                {
                    return FALSE;
                }
            }
            else
            {
                if (!video_fmt_mp4w_stream_push
                    (context, buffer, SAWB_ATOM_PART_1,
                     sizeof (SAWB_ATOM_PART_1)))
                {
                    return FALSE;
                }
            }

            /* Lower 16 bits of stream timescale is repeated here. */
            STORE_ATOM_UINT16 (temp_buffer, 0, (uint16) stream->timescale);
            if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 2))
            {
                return FALSE;
            }

            /* Next part is also boilerplate. */
            if (!video_fmt_mp4w_stream_push
                (context, buffer, SAMR_SAWB_ATOM_PART_2,
                 sizeof (SAMR_SAWB_ATOM_PART_2)))
            {
                return FALSE;
            }

            /* Last part consists of AMR mode set, mode change period, and
            ** frames per sample.
            */
            STORE_ATOM_UINT16 (temp_buffer, 0, stream->amr_modes_used);
            temp_buffer [2] = (uint8) stream->mode_change_restriction;
            temp_buffer [3] = (uint8) stream->frames_per_sample;
            if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 4))
            {
                return FALSE;
            }
            break;

        case SAMPLE_ENTRY_SAWP:
            /* First part is boilerplate. */
            if (!video_fmt_mp4w_stream_push
                (context, buffer, SAWP_ATOM_PART_1,
                 sizeof (SAWP_ATOM_PART_1)))
            {
                return FALSE;
            }

            /* Lower 16 bits of stream timescale is repeated here. */
            STORE_ATOM_UINT16 (temp_buffer, 0, (uint16) stream->timescale);
            if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 2))
            {
                return FALSE;
            }

            /* Next part is also boilerplate. */
            if (!video_fmt_mp4w_stream_push
                (context, buffer, SAWP_ATOM_PART_2,
                 sizeof (SAWP_ATOM_PART_2)))
            {
                return FALSE;
            }
            break;

        case SAMPLE_ENTRY_SEVC:
            /* First part is boilerplate. */
            if (!video_fmt_mp4w_stream_push
                (context, buffer, SEVC_ATOM_PART_1,
                 sizeof (SEVC_ATOM_PART_1)))
            {
                return FALSE;
            }

            /* Lower 16 bits of stream timescale is repeated here. */
            STORE_ATOM_UINT16 (temp_buffer, 0, (uint16) stream->timescale);
            if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 2))
            {
                return FALSE;
            }

            /* Next part is also boilerplate. */
            if (!video_fmt_mp4w_stream_push
                (context, buffer, SEVC_ATOM_PART_2,
                 sizeof (SEVC_ATOM_PART_2)))
            {
                return FALSE;
            }

            /* Last part consists of frames per sample. */
            temp_buffer [0] = (uint8) stream->frames_per_sample;
            if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 1))
            {
                return FALSE;
            }
            break;

        case SAMPLE_ENTRY_SECB:
            /* First part is boilerplate. */
            if (!video_fmt_mp4w_stream_push
                (context, buffer, SECB_ATOM_PART_1,
                 sizeof (SECB_ATOM_PART_1)))
            {
                return FALSE;
            }

            /* Lower 16 bits of stream timescale is repeated here. */
            STORE_ATOM_UINT16 (temp_buffer, 0, (uint16) stream->timescale);
            if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 2))
            {
                return FALSE;
            }

            /* Next part is also boilerplate. */
            if (!video_fmt_mp4w_stream_push
                (context, buffer, SECB_ATOM_PART_2,
                 sizeof (SECB_ATOM_PART_2)))
            {
                return FALSE;
            }

            /* Last part consists of frames per sample. */
            temp_buffer [0] = (uint8) stream->frames_per_sample;
            if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 1))
            {
                return FALSE;
            }
            break;

        case SAMPLE_ENTRY_SECW:
            /* First part is boilerplate. */
            if (!video_fmt_mp4w_stream_push
                (context, buffer, SECW_ATOM_PART_1,
                 sizeof (SECW_ATOM_PART_1)))
            {
                return FALSE;
            }

            /* Lower 16 bits of stream timescale is repeated here. */
            STORE_ATOM_UINT16 (temp_buffer, 0, (uint16) stream->timescale);
            if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 2))
            {
                return FALSE;
            }

            /* Next part is also boilerplate. */
            if (!video_fmt_mp4w_stream_push
                (context, buffer, SECW_ATOM_PART_2,
                 sizeof (SECW_ATOM_PART_2)))
            {
                return FALSE;
            }

            /* Last part consists of frames per sample. */
            temp_buffer [0] = (uint8) stream->frames_per_sample;
            if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 1))
            {
                return FALSE;
            }
            break;

        case SAMPLE_ENTRY_SQCP:
            /* First part is boilerplate. */
            if (!video_fmt_mp4w_stream_push
                (context, buffer, SQCP_ATOM_PART_1,
                 sizeof (SQCP_ATOM_PART_1)))
            {
                return FALSE;
            }

            /* Lower 16 bits of stream timescale is repeated here. */
            STORE_ATOM_UINT16 (temp_buffer, 0, (uint16) stream->timescale);
            if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 2))
            {
                return FALSE;
            }

            /* Next part is also boilerplate. */
            if (!video_fmt_mp4w_stream_push
                (context, buffer, SQCP_ATOM_PART_2,
                 sizeof (SQCP_ATOM_PART_2)))
            {
                return FALSE;
            }

            /* Last part consists of frames per sample. */
            temp_buffer [0] = (uint8) stream->frames_per_sample;
            if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 1))
            {
                return FALSE;
            }
            break;

        case SAMPLE_ENTRY_S263:
            /* First part is boilerplate. */
            if (!video_fmt_mp4w_stream_push
                (context, buffer, S263_ATOM_PART_1,
                 sizeof (S263_ATOM_PART_1)))
            {
                return FALSE;
            }

            /* Next part consists of a copy of the video width/height. */
            STORE_ATOM_UINT16 (temp_buffer, 0, stream->subinfo.video.width);
            STORE_ATOM_UINT16 (temp_buffer, 2, stream->subinfo.video.height);
            if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 4))
            {
                return FALSE;
            }

            /* Next part is also boilerplate. */
            if (!video_fmt_mp4w_stream_push
                (context, buffer, S263_ATOM_PART_2,
                 sizeof (S263_ATOM_PART_2)))
            {
                return FALSE;
            }

            /* Last part consists of H.263 profile and level. */
            temp_buffer [0] = (uint8) stream->subinfo.video.level;
            temp_buffer [1] = (uint8) stream->subinfo.video.profile;
            if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 2))
            {
                return FALSE;
            }
            break;

        case SAMPLE_ENTRY_AVC1:

           /* First part of AVC1 sample entry */
            if (!video_fmt_mp4w_stream_push(context,
                                            buffer,
                                            SAVC1_ATOM_PART_1,
                                            sizeof (SAVC1_ATOM_PART_1)))
            {
                return FALSE;
            }

            /* Store width and height */
            STORE_ATOM_UINT16 (temp_buffer, 0, stream->subinfo.video.width);
            STORE_ATOM_UINT16 (temp_buffer, 2, stream->subinfo.video.height);
            if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 4))
            {
                return FALSE;
            }

             /* Later part of AVC1 sample entry */
            if (!video_fmt_mp4w_stream_push(context,
                                            buffer,
                                            SAVC1_ATOM_PART_2,
                                            sizeof (SAVC1_ATOM_PART_2)))
            {
                return FALSE;
            }

            /* Write size and type of "avcC" atom */
            MAKE_ATOM_HEADER (temp_buffer,
                              stream->header_size
                              + 8  /* size and type of avcC atom */ 
                              + 5, /* first part of avcC atom */ 
                              "avcC");

            if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 8))
            {
               return FALSE;
            }
            /* Construct the first part of "avcC" atom */ 
            temp_buffer[0] = 0x01; /* configurationVersion */
            temp_buffer[1] = stream->subinfo.video.profile; /* AVCProfileIndication */
            temp_buffer[2] = stream->subinfo.video.profile_comp; /* ProfileCommpatibility */
            temp_buffer[3] = stream->subinfo.video.level;/* AVCLevelIndication */
            temp_buffer[4] = 0xFF; /* First 6 bits are reserved. Last 2 bits: No of bytes of
                                      NALUnitLength field - 1 */
            /* Write first part of "avcC" atom */
            if (!video_fmt_mp4w_stream_push (context,
                                             buffer,
                                             temp_buffer,
                                             5))
            {
               return FALSE;
            }

             /* Write paramSets */
            if (!video_fmt_mp4w_stream_push (context,
                                             buffer,
                                             stream->header,
                                             stream->header_size))
            {
               return FALSE;
            }

            /* Calculate average and maximum bitrates for this stream. */
            temp64 = stream->bitrate_total_bits;
            temp64 *= stream->timescale;
            temp64 /= MAX (1, stream->duration);
            avg_bitrate = (uint32) temp64;
            if ((stream->fixed_sample_size > 0)
                && (stream->fixed_sample_delta > 0)
                && !stream->has_inter_frames)
            {
                temp64 = stream->fixed_sample_size;
                temp64 *= stream->timescale;
                temp64 /= stream->fixed_sample_delta;
                temp64 *= 8;
                max_bitrate = (uint32) temp64;
            }
            else
            {
                max_bitrate = stream->bitrate_max_bits;
            }

            /* Handle case where we never got even one window for the maximum
            ** bitrate over a one-second window - just scale the partial
            ** window we got.
            */
            if (0 == max_bitrate)
            {
                temp64 = stream->bitrate_total_bits;
                temp64 *= stream->bitrate_window_time;
                temp64 /= stream->timescale;
            }


            /* Write btrt atom */
            MAKE_ATOM_HEADER (temp_buffer,20,"btrt"); /*lint !e572 */ /* "btrt" */
            STORE_ATOM_UINT32(temp_buffer,8,10000); /*lint !e572 */ /* Max frame size */
            STORE_ATOM_UINT32(temp_buffer,12,max_bitrate);/* Max bit rate */
            STORE_ATOM_UINT32(temp_buffer,16,avg_bitrate); /* Avg bit rate */
            if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 20))
            {
                return FALSE;
            }

          break;

        case SAMPLE_ENTRY_TX3G:
            /* Write out text sample entry atoms. */
            STORE_ATOM_UINT32 (temp_buffer, 12, 1);/*lint !e572 */ 
            for (atom = stream->text; atom; atom = atom->next)
            {
                if (!video_fmt_mp4w_stream_push
                    (context, buffer, atom->atom, atom->size))
                {
                    return FALSE;
                }
            }
            break;

        default:
            /* If we ever reach this case, we forgot to add code here to
            ** handle one or more possible cases.
            */
            ASSERT (0);
        }

        /*-------------------------------------------------------------------
        ** Generate ES descriptor atom, if present.
        **-------------------------------------------------------------------
        */
        if (esds_size [i])
        {
            /* Output atom header. */
            MAKE_FULL_ATOM_HEADER (temp_buffer, esds_size [i], "esds", 0, 0); /*lint !e572 */ 
            if (!video_fmt_mp4w_stream_push
                (context, buffer, temp_buffer, 12))
            {
                return FALSE;
            }

            /* Calculate average and maximum bitrates for this stream. */
            temp64 = stream->bitrate_total_bits;
            temp64 *= stream->timescale;
            temp64 /= MAX (1, stream->duration);
            avg_bitrate = (uint32) temp64;
            if ((stream->fixed_sample_size > 0)
                && (stream->fixed_sample_delta > 0)
                && !stream->has_inter_frames)
            {
                temp64 = stream->fixed_sample_size;
                temp64 *= stream->timescale;
                temp64 /= stream->fixed_sample_delta;
                temp64 *= 8;
                max_bitrate = (uint32) temp64;
            }
            else
            {
                max_bitrate = stream->bitrate_max_bits;
            }

            /* Handle case where we never got even one window for the maximum
            ** bitrate over a one-second window - just scale the partial
            ** window we got.
            */
            if (0 == max_bitrate)
            {
                temp64 = stream->bitrate_total_bits;
                temp64 *= stream->bitrate_window_time;
                temp64 /= stream->timescale;
            }

            /* Output ES fields up to decoder-specific info. */
            temp_buffer [0] = 0x03;  /* ES_DescrTag */
            temp_buffer [1] = (uint8) (((esds_size [i] - 15) >> 7) | 0x80);
            temp_buffer [2] = (uint8) ((esds_size [i] - 15) & 0x7F);
            STORE_ATOM_UINT16 (temp_buffer, 3, i + 1);  /* ES_ID */
            /* URL_FLAG = FALSE (0), OCRstreamFlag = FALSE (0) */
            temp_buffer [5] = (stream->priority & 0x3F);
            temp_buffer [6] = 0x04;  /* DecoderConfigDescrTag */
            /* descriptor size - boilerplate takes 13 bytes, but add 2 bytes
            ** in case decoder specific info (header) present.
            */
            if (stream->header_size > 0)
            {
                temp_buffer [7]
                    = (uint8) (((stream->header_size + 16) >> 7) | 0x80);
                temp_buffer [8]
                    = (uint8) ((stream->header_size + 16) & 0x7F);
            }
            else
            {
                temp_buffer [7] = (uint8) 0x80;
                temp_buffer [8] = (uint8) 13;
            }
            temp_buffer [9] = oti [i];
            /* upstream = FALSE (0), reserved = 1 */
            temp_buffer [10] = (stream_type [i] << 2) | 0x01;
            if (stream->fixed_sample_size > 0)
            {
                STORE_ATOM_UINT24 (temp_buffer, 11, stream->fixed_sample_size);
            }
            else
            {
                STORE_ATOM_UINT24 (temp_buffer, 11, stream->samples_largest);
            }
            STORE_ATOM_UINT32 (temp_buffer, 14, max_bitrate);
            STORE_ATOM_UINT32 (temp_buffer, 18, avg_bitrate);
            if (!video_fmt_mp4w_stream_push
                (context, buffer, temp_buffer, 22))
            {
                return FALSE;
            }

            /* Output video decoder specific information (if present). */
            if (stream->header_size > 0)
            {
                temp_buffer [0] = 0x05;  /* DecSpecificInfoTag */
                temp_buffer [1] = (uint8) ((stream->header_size >> 7) | 0x80);
                temp_buffer [2] = (uint8) (stream->header_size & 0x7F);
                if (!video_fmt_mp4w_stream_push
                    (context, buffer, temp_buffer, 3))
                {
                    return FALSE;
                }
                if (!video_fmt_mp4w_stream_push
                    (context, buffer, stream->header, stream->header_size))
                {
                    return FALSE;
                }
            }

            /* Output static footer of ESDS atom. */
            if (!video_fmt_mp4w_stream_push
                (context, buffer, ESDS_TRAILER, sizeof (ESDS_TRAILER)))
            {
                return FALSE;
            }
        }

        /*-------------------------------------------------------------------
        ** Generate decoding time to sample atom.
        **-------------------------------------------------------------------
        */

        /* Output atom header and entry count. */
        MAKE_FULL_ATOM_HEADER (temp_buffer, stts_size [i], "stts", 0, 0); /*lint !e572 */ 
        STORE_ATOM_UINT32 (temp_buffer, 12, stream->stts_size);
        if (!video_fmt_mp4w_stream_push
            (context, buffer, temp_buffer, 16))
        {
            return FALSE;
        }

        /* Output table entries. */
        if (stream->fixed_sample_delta > 0)
        {
            STORE_ATOM_UINT32 (temp_buffer, 0, stream->samples_size);
            STORE_ATOM_UINT32 (temp_buffer, 4, stream->fixed_sample_delta);
            if (!video_fmt_mp4w_stream_push
                (context, buffer, temp_buffer, 8))
            {
                return FALSE;
            }
        }
        else
        {
            /* Unless more than one table of the same kind are stored,
            ** there's no need to read them in.
            */
            if (stream->stored_samples_table <= 1)
            {
                for (sample = entry = last = 0;
                    sample < stream->samples_size; ++sample)
                {
                    if ((sample == stream->samples_size - 1)
                        || (stream->samples [sample].delta
                            != stream->samples [sample + 1].delta))
                    {
                        STORE_ATOM_UINT32 (temp_buffer, 0, sample + 1 - last);
                        STORE_ATOM_UINT32
                            (temp_buffer, 4, stream->samples [sample].delta);
                        if (!video_fmt_mp4w_stream_push
                            (context, buffer, temp_buffer, 8))
                        {
                            return FALSE;
                        }
                        ++entry;
                        last = sample + 1;
                    }
                }
            }
            else
            {
                num_sample_stts_written = 0;
                sample = last = 0;
                for (table = 0, stored = stream->table_stores;
                    table < stream->table_stores_size;
                    table++, stored++)
                {
                    if (stored->type == VIDEO_FMT_MP4W_SAMPLE)
                    {
                        if (!video_fmt_mp4w_get_meta_table
                            (context, i, stored))
                        {
                            return FALSE;
                        }

                        for (index = 0; index < (int32) stored->items;
                            ++index, ++sample)
                        {
                            if (sample == stream->samples_size - 1)
                            {
                                stream->samples [index].delta
                                    += stream->last_sample_delta_extend;
                            }

                            /* Exclude the very first sample */
                            if ((sample != 0)
                                && (stream->samples [index].delta
                                    != stream->samples [index - 1].delta))
                            {
                                num_sample_stts_written++;
                                STORE_ATOM_UINT32
                                    (temp_buffer, 0, sample - last);
                                STORE_ATOM_UINT32 (temp_buffer, 4,
                                    stream->samples [index - 1].delta);
                                if (!video_fmt_mp4w_stream_push
                                    (context, buffer, temp_buffer, 8))
                                {
                                    return FALSE;
                                }
                                last = sample;
                            }

                         if(num_sample_stts_written >= stream->stts_size){
                           table = stream->table_stores_size;
                          break;
                         }

                        }

                        /* Include the very last sample */
                        if (sample == stream->samples_size)
                        {
                            STORE_ATOM_UINT32 (temp_buffer, 0, sample - last);
                            STORE_ATOM_UINT32 (temp_buffer, 4,
                                stream->samples [index - 1].delta);
                            if (!video_fmt_mp4w_stream_push
                                (context, buffer, temp_buffer, 8))
                            {
                                return FALSE;
                            }
                        }

                    }
                }
            }
        }

        /*-------------------------------------------------------------------
        ** Generate chunk offset table atom.
        **-------------------------------------------------------------------
        */

        /* Output atom header and entry count. */
        MAKE_FULL_ATOM_HEADER (temp_buffer, stco_size [i], "stco", 0, 0);/*lint !e572 */ 
        STORE_ATOM_UINT32 (temp_buffer, 12, stream->chunks_size);
        if (!video_fmt_mp4w_stream_push
            (context, buffer, temp_buffer, 16))
        {
            return FALSE;
        }

        /* Output chunk offset table. */
        if (stream->stored_chunks_table <= 1)
        {
            for (entry = 0; entry < stream->chunks_size; ++entry)
            {
                STORE_ATOM_UINT32 (temp_buffer, 0, stream->chunks [entry].pos);
                if (!video_fmt_mp4w_stream_push
                    (context, buffer, temp_buffer, 4))
                {
                    return FALSE;
                }
            }
        }
        else
        {
            num_chunk_stco_written = 0;
            for (table = 0, stored = stream->table_stores;
                table < stream->table_stores_size;
                table++, stored++)
            {
                if (stored->type == VIDEO_FMT_MP4W_CHUNK)
                {
                    if (!video_fmt_mp4w_get_meta_table
                        (context, i, stored))
                    {
                        return FALSE;
                    }

                    for (entry = 0; entry < stored->items; ++entry)
                    {
                        num_chunk_stco_written++;
                        STORE_ATOM_UINT32 (temp_buffer, 0,
                                            stream->chunks [entry].pos);
                        if (!video_fmt_mp4w_stream_push
                            (context, buffer, temp_buffer, 4))
                        {
                            return FALSE;
                        }
                        if(num_chunk_stco_written >= stream->chunks_size)
                        {
                            table = stream->table_stores_size;
                            break;
                        }
                    }
                }
            }
        }

        /*-------------------------------------------------------------------
        ** Generate sample to chunk table atom.
        **-------------------------------------------------------------------
        */

        /* Output atom header and entry count. */
        MAKE_FULL_ATOM_HEADER (temp_buffer, stsc_size [i], "stsc", 0, 0);/*lint !e572 */ 
        STORE_ATOM_UINT32 (temp_buffer, 12, stream->stsc_size);
        if (!video_fmt_mp4w_stream_push
            (context, buffer, temp_buffer, 16))
        {
            return FALSE;
        }

        /* Output sample to chunk table. */
        if (stream->stored_chunks_table <= 1)
        {
            for (chunk = entry = 0; chunk < stream->chunks_size; ++chunk)
            {
                if ((0 == chunk)
                    || (stream->chunks [chunk].samples
                        != stream->chunks [chunk - 1].samples)
                    || (stream->chunks [chunk].desc
                        != stream->chunks [chunk - 1].desc))
                {
                    /* first chunk */
                    STORE_ATOM_UINT32 (temp_buffer, 0, chunk + 1);

                    /* samples per chunk */
                    STORE_ATOM_UINT32
                        (temp_buffer, 4, stream->chunks [chunk].samples);

                    /* sample description index */
                    STORE_ATOM_UINT32
                        (temp_buffer, 8, stream->chunks [chunk].desc);

                    /* table entry done */
                    if (!video_fmt_mp4w_stream_push
                        (context, buffer, temp_buffer, 12))
                    {
                        return FALSE;
                    }
                    ++entry;
                }
            }
        }
        else
        {
            num_chunk_stsc_written = 0;
            chunk = entry = 0;
            for (table = 0, stored = stream->table_stores;
                table < stream->table_stores_size;
                table++, stored++)
            {
                if (stored->type == VIDEO_FMT_MP4W_CHUNK)
                {
                    if (!video_fmt_mp4w_get_meta_table
                        (context, i, stored))
                    {
                        return FALSE;
                    }

                    for (index = 0; index < (int32) stored->items;
                        ++index, ++chunk)
                    {
                        if ((0 == chunk)
                            || (stream->chunks [index].samples
                                != stream->chunks [index - 1].samples)
                            || (stream->chunks [index].desc
                                != stream->chunks [index - 1].desc))
                        {
                          num_chunk_stsc_written++;
                          if(context->update_file_data && 
                             num_chunk_stsc_written == stream->stsc_size)
                            {
                               stream->chunks [index].samples = 
                                context->last_chunk_size;
                            }
                            /* first chunk */
                            STORE_ATOM_UINT32 (temp_buffer, 0, chunk + 1);

                            /* samples per chunk */
                            STORE_ATOM_UINT32
                                (temp_buffer, 4, stream->chunks [index].samples);

                            /* sample description index */
                            STORE_ATOM_UINT32
                                (temp_buffer, 8, stream->chunks [index].desc);

                            /* table entry done */
                            if (!video_fmt_mp4w_stream_push
                                (context, buffer, temp_buffer, 12))
                            {
                                return FALSE;
                            }
                            if(num_chunk_stsc_written >= stream->stsc_size)
                            {
                              table = stream->table_stores_size;
                              break;
                            }
                            ++entry;
                        }
                    }
                }
            }
        }

        /*-------------------------------------------------------------------
        ** Generate sample size table atom.
        **-------------------------------------------------------------------
        */

        /* Output atom header, fixed sample size, and sample count. */
        MAKE_FULL_ATOM_HEADER (temp_buffer, stsz_size [i], "stsz", 0, 0);/*lint !e572 */ 
        if (context->force_stsz_table)
        {
            STORE_ATOM_UINT32 (temp_buffer, 12, 0);/*lint !e572 */ 
        }
        else
        {
            STORE_ATOM_UINT32 (temp_buffer, 12, stream->fixed_sample_size);
        }
        STORE_ATOM_UINT32 (temp_buffer, 16, stream->samples_size);
        if (!video_fmt_mp4w_stream_push
            (context, buffer, temp_buffer, 20))
        {
            return FALSE;
        }

        /* Output sample size table, if any.
        ** (Force one if force_stsz_flag is set.)
        */
        if (!stream->fixed_sample_size || context->force_stsz_table)
        {
            num_sample_stsz_written = 0;
            if (stream->stored_samples_table <= 1)
            {
                for (sample = 0; sample < stream->samples_size; ++sample)
                {
                    if (stream->fixed_sample_size)
                    {
                        entry = stream->fixed_sample_size;
                    }
                    else
                    {
                        entry = stream->samples [sample].size;
                    }
                    STORE_ATOM_UINT32 (temp_buffer, 0, entry);
                    if (!video_fmt_mp4w_stream_push
                        (context, buffer, temp_buffer, 4))
                    {
                        return FALSE;
                    }
                }
            }
            else
            {
                /* Samples table was used to store samples size.
                ** Samples size should not be fixed.
                */
                sample = 0;
                for (table = 0, stored = stream->table_stores;
                    table < stream->table_stores_size;
                    table++, stored++)
                {
                    if (stored->type == VIDEO_FMT_MP4W_SAMPLE)
                    {
                        if (!video_fmt_mp4w_get_meta_table
                            (context, i, stored))
                        {
                            return FALSE;
                        }

                        for (entry = 0; entry < stored->items;
                            ++entry, ++sample)
                        {
                            if (sample == stream->samples_size - 1)
                            {
                                stream->samples [entry].size
                                    += stream->last_sample_size_increase;
                            }
                            num_sample_stsz_written++;

                            STORE_ATOM_UINT32 (temp_buffer, 0,
                                                stream->samples [entry].size);
                            if (!video_fmt_mp4w_stream_push
                                (context, buffer, temp_buffer, 4))
                            {
                                return FALSE;
                            }
                           if(num_sample_stsz_written >= stream->samples_size)
                           {
                            table = stream->table_stores_size;
                            break;
                           }
                        }
                    }
                }
            }
        }

        /*-------------------------------------------------------------------
        ** Generate sync sample table atom, if stream has inter frames.
        **-------------------------------------------------------------------
        */
        if (stream->has_inter_frames)
        {
            /* Output atom header and entry count. */
            MAKE_FULL_ATOM_HEADER (temp_buffer, stss_size [i], "stss", 0, 0);/*lint !e572 */ 
            STORE_ATOM_UINT32 (temp_buffer, 12, stream->samples_num_iframes);
            if (!video_fmt_mp4w_stream_push
                (context, buffer, temp_buffer, 16))
            {
                return FALSE;
            }

            /* Output sync sample table. */
            if (stream->stored_samples_table <= 1)
            {
                for (sample = 0; sample < stream->samples_size; ++sample)
                {
                    if (stream->samples [sample].iframe)
                    {
                        STORE_ATOM_UINT32 (temp_buffer, 0, sample + 1);
                        if (!video_fmt_mp4w_stream_push
                            (context, buffer, temp_buffer, 4))
                        {
                            return FALSE;
                        }
                    }
                }
            }
            else
            {
                num_sample_iframes_written = 0;
                sample = 0;
                for (table = 0, stored = stream->table_stores;
                    table < stream->table_stores_size;
                    table++, stored++)
                {
                    if (stored->type == VIDEO_FMT_MP4W_SAMPLE)
                    {
                        if (!video_fmt_mp4w_get_meta_table
                            (context, i, stored))
                        {
                            return FALSE;
                        }

                        for (entry = 0; entry < stored->items;
                            ++entry, ++sample)
                        {
                            if (stream->samples [entry].iframe)
                            {
                                num_sample_iframes_written++;
                                STORE_ATOM_UINT32 (temp_buffer, 0, sample + 1);
                                if (!video_fmt_mp4w_stream_push
                                    (context, buffer, temp_buffer, 4))
                                {
                                    return FALSE;
                                }
                              if(num_sample_iframes_written >= 
                                stream->samples_num_iframes)
                              {    table = stream->table_stores_size;
                                   break;
                              }
                            }
                        }
                    }
                }
            }
        }
    }

    /*-----------------------------------------------------------------------
    ** Generate user data atom, if present.
    **-----------------------------------------------------------------------
    */
    if (context->include_user_data)
    {
        /* Output atom header. */
        MAKE_ATOM_HEADER (temp_buffer, udta_size, "udta");
        if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 8))
        {
            return FALSE;
        }

        /*if midi present, output full atom header of midi and midi data*/
        if (context->user_data.midiSize)
        {
            uint32 size = 12 + context->user_data.midiSize;

            /* Generate atom header. */
            MAKE_FULL_ATOM_HEADER (temp_buffer, size, "midi", 0, 0);/*lint !e572 */ 

            //output atom full header
            if (!video_fmt_mp4w_stream_push
                    (context, buffer, temp_buffer, 12))
            {
                return FALSE;
            }

            //output atom data
            if (!video_fmt_mp4w_stream_push
                    (context, buffer, context->user_data.midiData,
                     context->user_data.midiSize))
            {
                return FALSE;
            }
        }

        if (context->user_data.titl_auth_desc_valid)
        {
            /* Output author atom. */
            if (!video_fmt_mp4w_encode_language_coded_string_atom
                (context, buffer, &context->user_data.author, "auth"))
            {
                return FALSE;
            }

            /* Output title atom. */
            if (!video_fmt_mp4w_encode_language_coded_string_atom
                (context, buffer, &context->user_data.title, "titl"))
            {
                return FALSE;
            }

            /* Output description atom. */
            if (!video_fmt_mp4w_encode_language_coded_string_atom
                (context, buffer, &context->user_data.description, "dscp"))
            {
                return FALSE;
            }
            /* Output copyright atom. */
            if (!video_fmt_mp4w_encode_language_coded_string_atom
                (context, buffer, &context->user_data.copyright, "cprt"))
            {
                return FALSE;
            }
        }

		if	(context->user_data.loci.loci_valid)
		{
            /* Output location atom. */
            if (!video_fmt_mp4w_encode_loci_atom
                (context, buffer, &context->user_data.loci, "loci"))
            {
                return FALSE;
            }
        }
    }

    /* If this file will be fragmented, add a movie extension atom. */
    if (more_fragments)
    {
        /*-------------------------------------------------------------------
        ** Generate movie extends atom.
        **-------------------------------------------------------------------
        */

        /* Output atom header. */
        MAKE_ATOM_HEADER (temp_buffer, mvex_size, "mvex");
        if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 8))
        {
            return FALSE;
        }

        /*-------------------------------------------------------------------
        ** Generate movie extends header atom.
        **-------------------------------------------------------------------
        */

        /* Output atom header and empty fragment_duration field. */
        MAKE_FULL_ATOM_HEADER (temp_buffer, 16, "mehd", 0, 0); /*lint !e572 */ 
        STORE_ATOM_UINT32 (temp_buffer, 12, 0); /*lint !e572 */ 
        if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 16))
        {
            return FALSE;
        }

        /*-------------------------------------------------------------------
        ** Generate track extends atom for each stream, one at a time.
        **-------------------------------------------------------------------
        */
        for (i = 0; i < context->num_streams; ++i)
        {
            /* Cache pointer to stream being accessed. */
            stream = &context->streams [i];

            /* Output atom header, track ID, and default sample description
            ** index.
            */
            MAKE_FULL_ATOM_HEADER (temp_buffer, 32, "trex", 0, 0);/*lint !e572 */ 
            STORE_ATOM_UINT32 (temp_buffer, 12, i + 1);
            STORE_ATOM_UINT32 (temp_buffer, 16, 1);/*lint !e572 */ 
            if (!video_fmt_mp4w_stream_push
                (context, buffer, temp_buffer, 20))
            {
                return FALSE;
            }

            /* Set up default sample duration -- it should be the same as our
            ** "fixed_sample_delta" field in our stream state.
            */
            STORE_ATOM_UINT32 (temp_buffer, 0, stream->fixed_sample_delta);

            /* Default sample size is always 0 according to the spec, even
            ** if we are dealing with a fixed sample size stream.
            */
            STORE_ATOM_UINT32 (temp_buffer, 4, 0);/*lint !e572 */ 

            /* Set up default sample flags -- 0x10000 for video, 0 for audio,
            ** and presumably 0 for text as well (spec does not say).
            */
            STORE_ATOM_UINT32 (temp_buffer, 8,
                               ((stream->type == VIDEO_FMT_STREAM_VIDEO)
                                ? 0x10000 : 0));/*lint !e572 */ 

            /* Output atom contents. */
            if (!video_fmt_mp4w_stream_push
                (context, buffer, temp_buffer, 12))
            {
                return FALSE;
            }
        }
    }

    if(!video_fmt_mp4w_write_clipinfo(context,buffer))
    {
         return FALSE;
    }

    /*-----------------------------------------------------------------------
    ** Flush output buffer.
    **-----------------------------------------------------------------------
    */
    if (!video_fmt_mp4w_stream_output (context, buffer,
          VIDEO_FMT_MP4W_FLUSH_ALL))
    {
        return FALSE;
    }
    return TRUE;
}

/*===========================================================================

FUNCTION video_fmt_mp4w_addmfra_node

DESCRIPTION
  This function copies the new MFRA values into memory.
  Later these values are retrieved and written.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void video_fmt_mp4w_addmfra_node(video_fmt_mpw_mfrastruct_type *pData,video_fmt_mp4w_context_type  *context)
{
   video_fmt_alloc_type alloc;
   video_fmt_free_type  free;
   /*Check if this is first Node*/
   if(context->pHead == NULL)
   {
       alloc.size = sizeof(video_fmt_mpw_mfradatastruct_type);
       context->callback_ptr (VIDEO_FMT_ALLOC, context->client_data,
                              (video_fmt_status_cb_info_type *) &alloc,
                               NULL);

       /*Check if pointer Allocation was successfull*/
       if(alloc.ptr == NULL)
       {
          context->MfraAllocSuccess = FALSE;
          return;
       }
       /*Initialize all elements to Zero*/
       memset(alloc.ptr, 0, alloc.size);
       /*Allocate First Element*/
       context->pHead = (video_fmt_mpw_mfradatastruct_type *)alloc.ptr;
       /*Set the current pointer*/
       context->pCurrent = context->pHead;
       /*Make Next pointer point to NULL*/
       context->pCurrent->pNext = NULL;
       /*Set the amount of space available*/
       context->pCurrent->currentIndex = 0;
       /*Allocate the buffer where the Actual data will be stored*/
       alloc.size = MEMORYSIZE;
       context->callback_ptr (VIDEO_FMT_ALLOC, context->client_data,
                              (video_fmt_status_cb_info_type *) &alloc,
                               NULL);

       /*
       Check for allocation Failure Since this is the first node
       deleting first node clears all memory.
       */
       if(alloc.ptr == NULL)
       {
           context->MfraAllocSuccess = FALSE;
           free.ptr = context->pHead;
           context->callback_ptr (VIDEO_FMT_FREE, context->client_data,
                                  (video_fmt_status_cb_info_type *) &free, NULL);
           return;
       }

       context->pCurrent->TrakId = pData->traf_number;
       context->pCurrent->pMfraData = alloc.ptr;
       /*Update the MFRA node values*/
       STORE_ATOM_UINT32(context->pCurrent->pMfraData, context->pCurrent->currentIndex, pData->time); 
       STORE_ATOM_UINT32(context->pCurrent->pMfraData, (context->pCurrent->currentIndex+4), pData->moof_offset);
       context->pCurrent->pMfraData[context->pCurrent->currentIndex+8]=pData->traf_number;
       context->pCurrent->pMfraData[context->pCurrent->currentIndex+9]=pData->trun_number;
       context->pCurrent->pMfraData[context->pCurrent->currentIndex+10]=pData->SampleNumber;
       /*Increment the index*/
       context->pCurrent->currentIndex += sizeof(video_fmt_mpw_mfrastruct_type);
   }
   /*Check if current Node has enough space*/
   else if( (context->pCurrent != NULL) && 
            (context->pCurrent->currentIndex < MEMORYSIZE) 
          )
   {
       /*Update the MFRA node values*/
       STORE_ATOM_UINT32(context->pCurrent->pMfraData, context->pCurrent->currentIndex, pData->time); 
       STORE_ATOM_UINT32(context->pCurrent->pMfraData, (context->pCurrent->currentIndex+4), pData->moof_offset);
       context->pCurrent->pMfraData[context->pCurrent->currentIndex+8]=pData->traf_number;
       context->pCurrent->pMfraData[context->pCurrent->currentIndex+9]=pData->trun_number;
       context->pCurrent->pMfraData[context->pCurrent->currentIndex+10]=pData->SampleNumber;
    
       /*Increment the index*/
       context->pCurrent->currentIndex += sizeof(video_fmt_mpw_mfrastruct_type);
     
   }
   else
   {
     /*Check for Error condition*/
     ASSERT(context->pCurrent != NULL);
     /*Alocate the next Element*/
     alloc.size = sizeof(video_fmt_mpw_mfradatastruct_type);
     context->callback_ptr (VIDEO_FMT_ALLOC, context->client_data,
                            (video_fmt_status_cb_info_type *) &alloc,
                             NULL);
     /*Check if pointer Allocation was successfull*/
     if(alloc.ptr == NULL)
     {
        context->MfraAllocSuccess = FALSE;
        /*Free all the other elements of MFRA atom*/
        video_fmt_mp4w_freemfra_datastruct(context);
        return;
     }
     /*Initialize all elements to Zero*/
     memset(alloc.ptr, 0, alloc.size);
     context->pCurrent->pNext = alloc.ptr;
     /*Make new node as current node*/
     context->pCurrent = context->pCurrent->pNext;
     /*Update Track Id*/
     context->pCurrent->TrakId = pData->traf_number;
     /*Make Next pointer point to NULL*/
     context->pCurrent->pNext = NULL;
     /*Set the amount of space available*/
     context->pCurrent->currentIndex = 0;
     /*Allocate the buffer where the Actual data will be stored*/
     alloc.size = MEMORYSIZE;
     context->callback_ptr (VIDEO_FMT_ALLOC, context->client_data,
                            (video_fmt_status_cb_info_type *) &alloc,
                             NULL);
     /*Check if pointer Allocation was successfull*/
     if(alloc.ptr == NULL)
     {
        context->MfraAllocSuccess = FALSE;
        /*Free all the other elements of MFRA atom*/
        video_fmt_mp4w_freemfra_datastruct(context);
        return;
     }
     /*Initialize all elements to Zero*/
     memset(alloc.ptr, 0, alloc.size);

     context->pCurrent->pMfraData = alloc.ptr;
     /*Update the MFRA node values*/
     STORE_ATOM_UINT32(context->pCurrent->pMfraData, context->pCurrent->currentIndex, pData->time); 
     STORE_ATOM_UINT32(context->pCurrent->pMfraData, (context->pCurrent->currentIndex+4), pData->moof_offset);
     context->pCurrent->pMfraData[context->pCurrent->currentIndex+8]=pData->traf_number;
     context->pCurrent->pMfraData[context->pCurrent->currentIndex+9]=pData->trun_number;
     context->pCurrent->pMfraData[context->pCurrent->currentIndex+10]=pData->SampleNumber;
     
     /*Increment the index*/
     context->pCurrent->currentIndex += sizeof(video_fmt_mpw_mfrastruct_type);    
   }

}

/*===========================================================================

FUNCTION video_fmt_mp4w_writemfra_tofile

DESCRIPTION
  This function copies the MFRA values from Memory into the temparory file.

DEPENDENCIES
  None.

RETURN VALUE
  None
SIDE EFFECTS
  None.

===========================================================================*/

void video_fmt_mp4w_writemfra_tofile(video_fmt_mp4w_context_type *context)
{
  uint32 TfraSize,MfraSize,index = 0,i=0,TempInt;
  uint32 WrittenBytes;
  video_fmt_alloc_type alloc;
  video_fmt_free_type  free;
  video_fmt_mpw_mfradatastruct_type *pTempDStruct;
  uint8 *pData;

  /*Check if there are any elements to be written*/
  if(context->pHead == NULL)
      return;

  /*Fixed Header Size + Number of entires*/
  TfraSize = context->MfraEntryCount * sizeof(video_fmt_mpw_mfrastruct_type) + 24;
  
  /*TfraSize+MfraHeader+Mfro*/
  MfraSize = 8+16+TfraSize;
  
  /*Allocate Temp buffer*/
  alloc.size = 32;
  context->callback_ptr (VIDEO_FMT_ALLOC, context->client_data,
                         (video_fmt_status_cb_info_type *) &alloc,
                          NULL);
  if(alloc.ptr == NULL)
  {
      /*Free the data structure*/
      video_fmt_mp4w_freemfra_datastruct(context);
      return;
  }

  pData = (uint8 *) alloc.ptr;
  
  /*Write MFRA size*/
  STORE_ATOM_UINT32(pData,index,MfraSize);
  index +=4;
  
  /*Write MFRA Atom*/
  STORE_ATOM_ID(pData,index,"mfra");
  index +=4;
  
  /*Write TFRA size*/
  STORE_ATOM_UINT32(pData,index,TfraSize);
  index +=4;
  
  /*Write TFRA Atom*/
  STORE_ATOM_ID(pData,index,"tfra");
  index +=4;
  
  /*TFRA Version+Flags*/
  TempInt = 0;
  STORE_ATOM_UINT32(pData,index,TempInt);
  index +=4;
  
  /*Track Id*/
  STORE_ATOM_UINT32(pData,index,context->pHead->TrakId);
  index +=4;
  
  /*Reserved Zero values*/
  TempInt = 0;
  STORE_ATOM_UINT32(pData,index,TempInt);
  index +=4;
  
  /*Number Of entries*/
  STORE_ATOM_UINT32(pData,index,context->MfraEntryCount);
  index +=4;
  i=0;
  /*Write Header Part of MFRA*/
  do
  {
     /*Write Header Elements*/
     WrittenBytes = video_fmt_mp4w_write_movie(context,&pData[i],index);
     i += WrittenBytes;
     index -= WrittenBytes;
  }
  while(index);
  
  pTempDStruct = context->pHead;
  /*Write all the data*/
  while(pTempDStruct)
  {
     index = pTempDStruct->currentIndex;
     i=0;
     /*Write All the elements into File*/
     do
     {
        WrittenBytes = video_fmt_mp4w_write_movie(context,
                      &pTempDStruct->pMfraData[i],index);
        i += WrittenBytes;
        index -= WrittenBytes;
     }
     while(index);

     if(pTempDStruct->pNext)
        pTempDStruct = pTempDStruct->pNext;
     else
       pTempDStruct = NULL;
  }
  
  index = 0;
  /*MFRO size*/
  TempInt = 16;
  STORE_ATOM_UINT32(pData,index,TempInt);
  index +=4;
  
  /*Write MFRO Atom*/
  STORE_ATOM_ID(pData,index,"mfro");
  index +=4;
  
  /*TFRA Version+Flags*/
  TempInt = 0;
  STORE_ATOM_UINT32(pData,index,TempInt);
  index +=4;
    
  /*Write MFRA size*/
  STORE_ATOM_UINT32(pData,index,MfraSize);
  index +=4;

  i=0;
  /*Write MFRO header part*/
  do
  {
     /*Write MFRO Elements*/
     WrittenBytes = video_fmt_mp4w_write_movie(context,&pData[i],index);
     i += WrittenBytes;
     index -= WrittenBytes;
  }
  while(index);
  /*Free all allocations*/
  free.ptr = (void*)pData;
  context->callback_ptr (VIDEO_FMT_FREE, context->client_data,
                         (video_fmt_status_cb_info_type *) &free, NULL);

  /*Free the data structure*/
  video_fmt_mp4w_freemfra_datastruct(context);
}

/*===========================================================================

FUNCTION video_fmt_mp4w_freemfra_datastruct

DESCRIPTION
  This function frees the datastruct of MFRA.

DEPENDENCIES
  None.

RETURN VALUE
  None
SIDE EFFECTS
  None.

===========================================================================*/

void video_fmt_mp4w_freemfra_datastruct(video_fmt_mp4w_context_type  *context)
{

 video_fmt_mpw_mfradatastruct_type *pTempDStruct;
 video_fmt_free_type  free;

 /*Free Data Structure Allocated*/
 while(context->pHead) 
 {
    /*Free the MFRA Data buffer*/
    if(context->pHead->pMfraData)
    {
        free.ptr = context->pHead->pMfraData;
        context->callback_ptr (VIDEO_FMT_FREE, context->client_data,
                               (video_fmt_status_cb_info_type *) &free, NULL);
    }
    /*Free the MFRA Data Structure*/
    pTempDStruct = context->pHead->pNext;
    free.ptr = context->pHead;
    context->callback_ptr (VIDEO_FMT_FREE, context->client_data,
                           (video_fmt_status_cb_info_type *) &free, NULL);
    /*Check if there are any more elements in data structure*/
    if(pTempDStruct)
    {
     context->pHead = pTempDStruct;
    }
    else
      context->pHead = NULL;
 }

}

/* <EJECT> */
/*===========================================================================

FUNCTION video_fmt_mp4w_generate_fragment

DESCRIPTION
  This function finishes writing the next fragment (not the main fragment) of
  the MP4 output file by generating and writing all the meta-data streams and
  atoms that go in the fragment.

DEPENDENCIES
  None.

RETURN VALUE
  The value TRUE is returned normally to indicate successful processing.  If
  the value FALSE is returned, it indicates a failure to write data to the
  output destination.

SIDE EFFECTS
  None.

===========================================================================*/
boolean video_fmt_mp4w_generate_fragment
(
  video_fmt_mp4w_context_type  *context
)
{
    video_fmt_mp4w_stream_type          *buffer;
    uint32                              i, j;
    uint32                              sample, entry;
    video_fmt_mp4w_stream_context_type  *stream;
    uint8                               temp_buffer [25];

    /* Atom types and sizes - one for every possible atom and stream
    ** combination, listed here in order from deepest atom level upwards.
    **
    ** NOTE: atoms not listed here are the ones with a well-known fixed size:
    ** 'tfhd' - data + TFHD_TRAILER = 24 bytes
    ** 'mfhd' - data = 16 bytes
    */
    uint32               traf_size [VIDEO_FMT_MAX_MEDIA_STREAMS];
    uint32               moof_size;

    /* Track run variables - one set for each stream. */
    uint32               header_flags [VIDEO_FMT_MAX_MEDIA_STREAMS];
    uint32               run_flags [VIDEO_FMT_MAX_MEDIA_STREAMS];
    uint32               run_element_size [VIDEO_FMT_MAX_MEDIA_STREAMS];
    uint32               run_header_size [VIDEO_FMT_MAX_MEDIA_STREAMS];
    /*Variables to track MFRA element values*/
    uint32               TimeOfCurrentFrame=0;
    uint32               moofOffset=0;
    uint32               TrunNumber=1;
    video_fmt_mpw_mfrastruct_type MfraValues;

    for(i=0;i<VIDEO_FMT_MAX_MEDIA_STREAMS;i++)
    {
      run_header_size[i]=0;
      run_element_size[i]=0;
      header_flags[i]=0;
      run_flags[i]=0;
    }
    MSG_LOW ("generate_fragment: entered", 0,0,0);

    /*-----------------------------------------------------------------------
    ** Calculate sizes of various atoms that we're about to generate.  We
    ** need to know the sizes in advance in order to output the atom size
    ** fields.  Build sizes by working from the innermost atoms outward.
    **-----------------------------------------------------------------------
    */
    moof_size = 0;
    for (i = 0; i < context->num_streams; ++i)
    {
        /* Cache pointer to stream being accessed. */
        stream = &context->streams [i];

        /* Determine track fragment header flags, track run flags, track run
        ** header size, and track run table element size, based on track
        ** type.
        */
        switch (stream->type)
        {
        case VIDEO_FMT_STREAM_VIDEO:
            run_header_size [i] = 24;
            run_element_size [i] = 8;
            header_flags [i] = 0x000001;
            run_flags [i] = 0x000305;
            /* Required For MFRA Atom
            ** Calulate the time of the first frame in the fragment.
            */
            TimeOfCurrentFrame = (stream->total_duration - stream->duration);
            moofOffset = context->mdat_offset;
            break;

        case VIDEO_FMT_STREAM_AUDIO:
            run_header_size [i] = 20;
            run_element_size [i] = 4;
            header_flags [i] = 0x000001;
            run_flags [i] = 0x000201;
            break;

        case VIDEO_FMT_STREAM_TEXT:
            /* FIXME - TODO - need to look up info on fragmented text streams.
            ** For now, use same settings as DATA.
            */
            run_header_size [i] = 20;
            run_element_size [i] = 8;
            header_flags [i] = 0x000003;
            run_flags [i] = 0x000301;
            break;

        case VIDEO_FMT_STREAM_DATA:
            run_header_size [i] = 20;
            run_element_size [i] = 8;
            header_flags [i] = 0x000003;
            run_flags [i] = 0x000301;
            break;

        default:
            /* If we ever reach this case, we forgot to add code here to
            ** handle one or more possible cases.
            */
            ASSERT (0);
        }

        /* Track fragment atom - fixed part is 8 bytes, plus 'tfhd' atom of 24
        ** bytes, plus total of 'trun' atom sizes.
        */
        traf_size [i] = 8 + 24;
        for (j = 0; j < stream->chunks_size; ++j)
        {
            /* Track run atom - fixed part depends on stream type (24 for
            ** video, 20 for audio and text), plus a number of bytes per
            ** sample (8 for video and text, 4 for audio).
            */
            traf_size [i] += run_header_size [i]
                + stream->chunks [j].samples * run_element_size [i];
        }

        /* Add size of track fragment atom to size of movie fragment atom. */
        moof_size += traf_size [i];
    }

    /* We've been adding the sizes of each stream's track fragment atom to the
    ** total size of the movie fragment atom.  Now add in the fixed parts,
    ** consisting of a movie fragment header atom (fixed size of 16 bytes),
    ** and an 8 byte header.
    */
    moof_size += 16 + 8;

        buffer = &context->meta_data_buffer;

    /* First clean the buffer up, in case there is some garbage left. */
    video_fmt_mp4w_stream_reset(buffer);

    /* Go back and write the total size of the media data atom. */
    if (context->mdat_written)
    {
        STORE_ATOM_UINT32 (temp_buffer, 0,
                           context->output_offset - context->mdat_offset);
        i = context->output_offset;
        context->output_offset = context->mdat_offset;
        if (!video_fmt_mp4w_write_movie (context, temp_buffer, 4))
        {
            return FALSE;
        }
        context->output_offset = i;
        context->mdat_written = FALSE;
    }

    /*-----------------------------------------------------------------------
    ** Generate movie fragment atom.
    **-----------------------------------------------------------------------
    */

    /* Output movie atom fragment header. */
    MAKE_ATOM_HEADER (temp_buffer, moof_size, "moof");
    if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 8))
    {
        return FALSE;
    }

    /*-----------------------------------------------------------------------
    ** Generate movie fragment header atom.
    **-----------------------------------------------------------------------
    */

    /* Output atom header and fragment sequence number. */
    MAKE_FULL_ATOM_HEADER (temp_buffer, 16, "mfhd", 0, 0);/*lint !e572 */ 
    STORE_ATOM_UINT32 (temp_buffer, 12, context->fragment_sequence_number);
    if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 16))
    {
        return FALSE;
    }

    /*-----------------------------------------------------------------------
    ** Generate track fragment atom for each stream, one at a time.
    **-----------------------------------------------------------------------
    */
    for (i = 0; i < context->num_streams; ++i)
    {
        /* Cache pointer to stream being accessed. */
        stream = &context->streams [i];

        /* Output atom header. */
        MAKE_ATOM_HEADER (temp_buffer, traf_size [i], "traf");
        if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 8))
        {
            return FALSE;
        }

        /*-------------------------------------------------------------------
        ** Generate track fragment header atom.
        **-------------------------------------------------------------------
        */

        /* Output atom header and track ID. */
        MAKE_FULL_ATOM_HEADER (temp_buffer, 24, "tfhd", 0, header_flags [i]);/*lint !e572 */ 
        STORE_ATOM_UINT32 (temp_buffer, 12, i + 1);
        if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 16))
        {
            return FALSE;
        }
        /*Track the TRAF Number*/
        MfraValues.traf_number = i + 1;
        /* Output track fragment header trailer. */
        if (!video_fmt_mp4w_stream_push
            (context, buffer, TFHD_TRAILER, sizeof (TFHD_TRAILER)))
        {
            return FALSE;
        }

        /*-------------------------------------------------------------------
        ** Generate track run atom for each chunk, one at a time.
        **-------------------------------------------------------------------
        */
        sample = 0;
        for (j = 0; j < stream->chunks_size; ++j)
        {
            /* Keep track of the Trun Number
            ** Need for MFRA Atom
            */
            MfraValues.trun_number = TrunNumber;
            TrunNumber++;
            /* Output atom header. */
            MAKE_FULL_ATOM_HEADER
                (temp_buffer, run_header_size [i]
                 + stream->chunks [j].samples * run_element_size [i],
                 "trun", 0, run_flags [i]);
            if (!video_fmt_mp4w_stream_push
                (context, buffer, temp_buffer, 12))
            {
                return FALSE;
            }

            /* Output sample count, chunk offset, and whether first sample is
            ** an I- or P-frame (if video).
            */
            STORE_ATOM_UINT32 (temp_buffer, 0, stream->chunks [j].samples);
            STORE_ATOM_UINT32 (temp_buffer, 4, stream->chunks [j].pos);
            if (!video_fmt_mp4w_stream_push
                (context, buffer, temp_buffer, 8))
            {
                return FALSE;
            }
            if (stream->type == VIDEO_FMT_STREAM_VIDEO)
            {
                STORE_ATOM_UINT32 (temp_buffer, 0,
                                   (stream->samples [sample].iframe
                                    ? 0x00000000 : 0x00010000));/*lint !e572 */ 
                if (!video_fmt_mp4w_stream_push
                    (context, buffer, temp_buffer, 4))
                {
                    return FALSE;
                }
            }

            /* Output track run table. */
            for (entry = 0; entry < stream->chunks [j].samples; ++entry)
            {

                /* Need to record the following parameters if the current frame
                ** is an I-Frame.
                */
                if(stream->type == VIDEO_FMT_STREAM_VIDEO &&
                   stream->samples [sample + entry].iframe
                   && context->MfraAllocSuccess)
                {
                    /*Number of elments in MFRA Data Struct*/
                    context->MfraEntryCount++;
                    /*Moof offset to which the current I-Frame belongs to*/
                    MfraValues.moof_offset = moofOffset;
                    /*Time of the current I-Frame*/
                    MfraValues.time = TimeOfCurrentFrame;
                    /*Sample number of the current I-Frame in the trun atom*/
                    MfraValues.SampleNumber = (entry+1);
                    /*Add the values to MFRA data struct*/
                    video_fmt_mp4w_addmfra_node(&MfraValues,context);
                }
                /* Add the Delta need to keep track of each frames time
                ** for MFRA atom
                */
                if (stream->fixed_sample_delta > 0
                    && stream->type == VIDEO_FMT_STREAM_VIDEO)
                { 
                    TimeOfCurrentFrame += stream->fixed_sample_delta;
                }
                else if(stream->fixed_sample_delta == 0
                        && stream->type == VIDEO_FMT_STREAM_VIDEO)
                {
                    TimeOfCurrentFrame += stream->samples [sample + entry].delta;
                }

                /* Output sample duration, if video or data. */
                /* FIXME - TODO - for text track too? */
                if ((stream->type == VIDEO_FMT_STREAM_VIDEO)
                    || (stream->type == VIDEO_FMT_STREAM_TEXT)
                    || (stream->type == VIDEO_FMT_STREAM_DATA))
                {
                    if (stream->fixed_sample_delta > 0)
                    {
                        STORE_ATOM_UINT32
                            (temp_buffer, 0, stream->fixed_sample_delta);
                    }
                    else
                    {
                        STORE_ATOM_UINT32
                            (temp_buffer, 0,
                             stream->samples [sample + entry].delta);
                    }
                    if (!video_fmt_mp4w_stream_push
                        (context, buffer, temp_buffer, 4))
                    {
                        return FALSE;
                    }
                }

                /* Output sample size. */
                if (stream->fixed_sample_size > 0)
                {
                    STORE_ATOM_UINT32
                        (temp_buffer, 0, stream->fixed_sample_size);
                }
                else
                {
                    STORE_ATOM_UINT32
                        (temp_buffer, 0,
                         stream->samples [sample + entry].size);
                }
                if (!video_fmt_mp4w_stream_push
                    (context, buffer, temp_buffer, 4))
                {
                    return FALSE;
                }
            }

            /* Update sample counter. */
            sample += stream->chunks [j].samples;
        }
    }

    /*-----------------------------------------------------------------------
    ** Flush output buffer.
    **-----------------------------------------------------------------------
    */
    if (!video_fmt_mp4w_stream_output (context, buffer,
          VIDEO_FMT_MP4W_FLUSH_ALL))
    {
        return FALSE;
    }
    return TRUE;
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_fmt_mp4w_stream_alloc

DESCRIPTION
  This function allocates memory for a stream buffer and initializes the
  stream.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE is returned if the stream was allocated successfully.  Otherwise, FALSE
  is returned.

SIDE EFFECTS
  None.

===========================================================================*/
boolean video_fmt_mp4w_stream_alloc (
  video_fmt_mp4w_context_type  *context,
  video_fmt_mp4w_stream_type   *stream,
  uint32                       size
) {
    video_fmt_alloc_type  alloc;

    /* If the given buffer size is under a minimum threshold, automatically
    ** allocate at least the threshold size.  This is because we may reuse
    ** this stream buffer later for buffering other kinds of data that
    ** requires a minimum buffer size.
    */
    size = MAX (size, VIDEO_FMT_MP4W_MINIMUM_BUFFER_SIZE);

    /* Attempt to allocate memory for the stream buffer. */
    alloc.size = size;
    context->callback_ptr (VIDEO_FMT_ALLOC, context->client_data,
                           (video_fmt_status_cb_info_type *) &alloc,
                           video_fmt_mp4w_end);
    if (!alloc.ptr)
    {
        return FALSE;
    }
    stream->data = alloc.ptr;

    /* Initialize other fields of stream structure. */
    stream->size = size;
    stream->head = 0;
    stream->tail = 0;
    stream->end = size;
    stream->linear = size;
    return TRUE;
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_fmt_mp4w_stream_free

DESCRIPTION
  This function frees the memory for a stream buffer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void video_fmt_mp4w_stream_free (
  video_fmt_mp4w_context_type  *context,
  video_fmt_mp4w_stream_type   *stream
) {
    video_fmt_free_type  free;

    /* Deallocate memory used for stream buffer. */
    if (stream->data != NULL)
    {
        free.ptr = stream->data;
        context->callback_ptr
            (VIDEO_FMT_FREE, context->client_data,
             (video_fmt_status_cb_info_type *) &free,
             video_fmt_mp4w_end);
        stream->data = NULL;
    }
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_fmt_mp4w_stream_amt_full

DESCRIPTION
  This function calculates the number of unconsumed bytes in the stream.

DEPENDENCIES
  None.

RETURN VALUE
  The number of unconsumed bytes of data in the stream is returned.

SIDE EFFECTS
  None.

===========================================================================*/
uint32 video_fmt_mp4w_stream_amt_full (
  video_fmt_mp4w_stream_type  *stream
) {
    uint32  amt;

    /* The amount of data in the buffer is simply the difference between the
    ** tail and head offsets.
    */
    amt = stream->tail - stream->head;

    /* Count on side effect that if the tail is larger than the head, the
    ** result of the above subtraction will underflow, but adding the size of
    ** the buffer back in will account for the underflow and bring it back to
    ** the right amount.
    */
    if (stream->tail < stream->head)
    {
        amt += stream->end;
    }
    return amt;
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_fmt_mp4w_stream_amt_left

DESCRIPTION
  This function calculates the number of bytes more data can fit in the
  stream.

DEPENDENCIES
  None.

RETURN VALUE
  The number of bytes of usable free space in the stream is returned.

SIDE EFFECTS
  None.

===========================================================================*/
uint32 video_fmt_mp4w_stream_amt_left (
  video_fmt_mp4w_stream_type  *stream
) {
    uint32  amt;

    /* The amount of free space in the buffer is the difference between the
    ** head and tail offsets, minus one.
    */
    amt = stream->head - stream->tail - 1;

    /* Count on side effect that if the head is not larger than the tail, the
    ** result of the above subtraction will underflow, but adding the size of
    ** the buffer back in will account for this and bring it back to the right
    ** amount.
    */
    if (stream->head <= stream->tail)
    {
        amt += stream->end;
    }

    /* The free buffer space is disjoint if "end" is different from "size". */
    amt += (stream->size - stream->end);
    return amt;
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_fmt_mp4w_stream_seek

DESCRIPTION
  This function advances stream->tail by offset and returns stream->tail
  with the number of bytes more data can linearly fit in the stream without
  wrapping around.
  Input argument bWrap set to TRUE lets stream->tail wrap around.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer where more data can be put.
  NULL in error condition.

SIDE EFFECTS
  None.

===========================================================================*/
uint8* video_fmt_mp4w_stream_seek (
  video_fmt_mp4w_stream_type  *stream,
  uint32                      offset,
  boolean                     bWrap,
  uint32                      *size
) {
    uint8  *ptr;
    uint32  linear;

    MSG_LOW ("stream_seek: offset=%d", offset,0,0);

    /*
    ** If "offset" is larger than the size of the linearly free space from
    ** "tail", it's an error.
    */
    if (offset > stream->linear)
    {
        return NULL;
    }

    /*
    ** When "bWrap" is set, check if linearly free space from "tail" has
    ** increased after this function was called last time.
    ** If so, don't wrap "tail" around.
    */
    if (bWrap)
    {
        if (stream->head <= stream->tail)
        {
            linear = stream->size - stream->tail - 1;
        }
        else
        {
            linear = stream->head - stream->tail - 1;
        }

        if (linear > stream->linear)
        {
            bWrap = FALSE;
        }
        else if (stream->tail < stream->head)
        {
            return NULL;
        }
    }

    stream->tail += offset;
    ptr = &stream->data[stream->tail];

    if (bWrap)
    {
        if (stream->head == 0)
        {
            ptr = NULL;
            stream->linear = 0;
        }
        else
        {
            stream->end = stream->tail;
            stream->tail = 0;
            ptr = &stream->data[stream->tail];
        }
    }

    if (ptr)
    {
        if (stream->head <= stream->tail)
        {
            stream->linear = stream->size - stream->tail - 1;
        }
        else
        {
            stream->linear = stream->head - stream->tail - 1;
        }
    }
    if (size)
    {
        *size = stream->linear;
    }
    return ptr;
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_fmt_mp4w_stream_push

DESCRIPTION
  This function pushes data into a stream buffer.  If the stream buffer fills
  up during the push of data, video_fmt_mp4w_stream_output is called
  automatically to flush the buffer.

DEPENDENCIES
  None.

RETURN VALUE
  The value TRUE is returned if the data was pushed successfully.  Otherwise,
  the value FALSE is returned.

SIDE EFFECTS
  None.

===========================================================================*/
boolean video_fmt_mp4w_stream_push (
  video_fmt_mp4w_context_type  *context,
  video_fmt_mp4w_stream_type   *stream,
  const uint8                  *input,
  uint32                       len
) {
    uint32  chunk;

    MSG_LOW ("stream_push: len=%d", len,0,0);

    /* Push bytes while there is still data left to push. */
    while (len > 0)
    {
        /* Push either the rest of the data, or enough bytes to push the tail
        ** to the end of the buffer, whichever comes first.
        */
        chunk = MIN (len, stream->size - stream->tail);

        /* Make sure not to overflow the buffer. */
        if (stream->head > stream->tail)
        {
            chunk = MIN (chunk, stream->head - stream->tail - 1);
        }
        else if (stream->head == 0)
        {
            chunk = MIN (chunk, stream->size - stream->tail - 1);
        }

        /* If chunk has bytes, move the actual data. */
        if (0 < chunk)
        {
            memcpy (stream->data + stream->tail, input, chunk);
        }

        /* Update buffer state and input parameters. */
        stream->tail += chunk;
        if (stream->tail >= stream->size)
        {
            stream->tail -= stream->size;
        }
        input += chunk;
        len -= chunk;

        /* If we couldn't push any bytes, flush buffer. */
        if (0 == chunk)
        {
            if (!video_fmt_mp4w_stream_output (context, stream,
                  VIDEO_FMT_MP4W_FLUSH_ALL))
            {
                return FALSE;
            }
        }
    }
    return TRUE;
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_fmt_mp4w_stream_push_reverse

DESCRIPTION
  This function pushes data into a stream buffer, but in reverse order.  In
  other words, the data is put in front of any previous data in the stream.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void video_fmt_mp4w_stream_push_reverse (
  video_fmt_mp4w_stream_type  *stream,
  const uint8                 *input,
  uint32                      len
) {
    uint32  chunk;

    /* Push bytes while there is still data left to push. */
    while (len > 0)
    {
        /* Push either the rest of the data, or enough bytes to push the head
        ** back to the beginning of the buffer, whichever comes first.
        */
        if (0 == stream->head)
        {
            stream->head = stream->size;
        }
        chunk = MIN (len, stream->head);

        /* Move the actual data. */
        memcpy (stream->data + stream->head - chunk,
            input + len - chunk, chunk);

        /* Update buffer state and input parameters. */
        stream->head -= chunk;
        len -= chunk;
    }
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_fmt_mp4w_stream_truncate

DESCRIPTION
  This function changes "stream->tail" and forces the data amount in the
  stream buffer to be equal to "data_size"

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void video_fmt_mp4w_stream_truncate (
  video_fmt_mp4w_stream_type   *stream,
  uint32                       data_size
)
{
    uint32  prev_tail;

    prev_tail = stream->tail;
    stream->tail = stream->head + data_size;
    if (stream->tail >= stream->end)
    {
        stream->tail -= stream->end;
    }
    if (prev_tail < stream->tail)
    {
        stream->end = stream->size;
    }
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_fmt_mp4w_stream_reset

DESCRIPTION
  This function resets the given output buffer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void video_fmt_mp4w_stream_reset (
  video_fmt_mp4w_stream_type   *stream
)
{
    stream->head = 0;
    stream->tail = 0;
    stream->end = stream->size;
    stream->linear = stream->size;
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_fmt_mp4w_stream_output

DESCRIPTION
  This function outputs given number of bytes in the given output buffer.

  If 'len' is set to VIDEO_FMT_MP4W_FLUSH_ALL, this flushes the buffer.

DEPENDENCIES
  None.

RETURN VALUE
  The value TRUE is returned if the data was output successfully.  Otherwise,
  the value FALSE is returned.

SIDE EFFECTS
  None.

===========================================================================*/
boolean video_fmt_mp4w_stream_output (
  video_fmt_mp4w_context_type  *context,
  video_fmt_mp4w_stream_type   *stream,
  uint32                       len
)
{
    uint32  chunk;

    MSG_LOW ("stream_output: len=%d", len,0,0);

    if (len == VIDEO_FMT_MP4W_FLUSH_ALL)
    {
        len = video_fmt_mp4w_stream_amt_full(stream);
    }

    /* Pull bytes while there is still data left to be pulled. */
    while (len > 0)
    {
        /* Pull either the rest of the data, or the amount of data between the
        ** head and the end of the buffer, whichever comes first.
        */
        chunk = MIN (len, stream->end - stream->head);

        if (context->output_unit_size > 0)
        {
            chunk = MIN (chunk, context->output_unit_size);
        }

        /* Output chunk. */
        if (chunk != video_fmt_mp4w_write_movie
            (context, stream->data + stream->head, chunk))
        {
            MSG_ERROR ("stream_output: error writing output", 0, 0, 0);
            return FALSE;
        }

        /* Update buffer state and input parameters. */
        stream->head += chunk;
        if (stream->head >= stream->end)
        {
            stream->head -= stream->end;
            stream->end = stream->size;
        }

        len -= chunk;
    }
    return TRUE;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  video_fmt_mp4w_write_movie

DESCRIPTION
  This function is called to write a block of data out to the movie file.

DEPENDENCIES
  None

RETURN VALUE
  The number of bytes actually written out is returned.

SIDE EFFECTS
  None

===========================================================================*/
uint32 video_fmt_mp4w_write_movie
(
  video_fmt_mp4w_context_type  *context,
  const uint8                  *buffer,
  uint32                       size
)
{
    video_fmt_put_data_type  put_data;

    /* Set up client information structure to put data.  Use the current movie
    ** file offset, since we write to the file purely sequentially.
    */
    put_data.buffer = buffer;
    put_data.offset = context->output_offset;
    put_data.num_bytes = size;
    put_data.callback_ptr = NULL;
    put_data.server_data = context;

    /* Ask client to put data out to movie file. */
    context->callback_ptr (VIDEO_FMT_PUT_DATA, context->client_data,
                           (video_fmt_status_cb_info_type *) &put_data,
                           video_fmt_mp4w_end);

    /* Advance the current movie file offset, since we write to the file
    ** purely sequentially.  Also return the number of bytes written.
    */
    context->output_offset += put_data.num_bytes;
    return put_data.num_bytes;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  video_fmt_mp4w_do_limit_reached_callback

DESCRIPTION
  This function sends a VIDEO_FMT_LIMIT_REACHED callback to the client.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void video_fmt_mp4w_do_limit_reached_callback
(
  video_fmt_mp4w_context_type  *context
)
{
    video_fmt_done_type  done;

    done.total_duration = context->duration;
    done.space_out_status = context->space_out_status;
    context->callback_ptr
        (VIDEO_FMT_LIMIT_REACHED, context->client_data,
         (video_fmt_status_cb_info_type *) &done, video_fmt_mp4w_end);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  video_fmt_mp4w_failure

DESCRIPTION
  This function is called whenever there is a critical failure in the video
  format services, in order to free any resources that the services had
  allocated.

DEPENDENCIES
  rex_enter_crit_sect must be called before calling this function

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void video_fmt_mp4w_failure
(
  video_fmt_mp4w_context_type  *context,
  boolean                      discard_output
)
{
    video_fmt_status_cb_func_type  callback_ptr;
    void                           *client_data;

    /* Recover context, callback function, and client data, from
    ** server data.
    */
    callback_ptr = context->callback_ptr;
    client_data = context->client_data;

    /* Unless discarding output, close output file, ignoring status code. */
    if (!discard_output)
    {
        (void)video_fmt_mp4w_close (context);
    }
    context->context_deleted = TRUE;
    rex_leave_crit_sect(&context->cs);
    /* Release video format services context. */
    video_fmt_mp4w_release (context);

    /* Stop services by indicating failure code to client. */
    callback_ptr (VIDEO_FMT_FAILURE, client_data, NULL, NULL);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  video_fmt_mp4w_release

DESCRIPTION
  This function frees all memory used for the state structures and context
  information of the video format services movie writer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void video_fmt_mp4w_release
(
  video_fmt_mp4w_context_type  *context
)
{
    video_fmt_free_type  free;
    uint32               i;

    /* Return early if we don't have any context. */
    if (!context)
    {
        return;
    }

    MSG_LOW ("Release: Entered function", 0, 0, 0);

    /* Free any user data. */
    video_fmt_mp4w_free_language_coded_string
        (&context->user_data.title,
         context->callback_ptr, context->client_data);
    video_fmt_mp4w_free_language_coded_string
        (&context->user_data.author,
         context->callback_ptr, context->client_data);
    video_fmt_mp4w_free_language_coded_string
        (&context->user_data.description,
         context->callback_ptr, context->client_data);
    video_fmt_mp4w_free_language_coded_string
        (&context->user_data.copyright,
         context->callback_ptr, context->client_data);
    video_fmt_mp4w_free_loci
        (&context->user_data.loci,
         context->callback_ptr, context->client_data);

    /* Free any outstanding text samples. */
    while (context->text_samples)
    {
        video_fmt_mp4w_free_text
            (context->text_samples,
             context->callback_ptr, context->client_data);
    }

    /* For each stream, deallocate memory used. */
    if (context->streams)
    {
        for (i = 0; i < context->num_streams; ++i)
        {
            /* Free chunk table. */
            if (context->streams [i].chunks)
            {
                context->streams [i].chunks--;
                free.ptr = context->streams [i].chunks;
                context->callback_ptr
                    (VIDEO_FMT_FREE, context->client_data,
                     (video_fmt_status_cb_info_type *) &free, NULL);
            }

            /* Free sample table. */
            if (context->streams [i].samples)
            {
                context->streams [i].samples
                    -= context->streams [i].max_samples_rate;
                free.ptr = context->streams [i].samples;
                context->callback_ptr
                    (VIDEO_FMT_FREE, context->client_data,
                     (video_fmt_status_cb_info_type *) &free, NULL);
            }

            /* Free table stores table. */
            if (context->streams [i].table_stores)
            {
                free.ptr = context->streams [i].table_stores;
                context->callback_ptr
                    (VIDEO_FMT_FREE, context->client_data,
                     (video_fmt_status_cb_info_type *) &free, NULL);
            }

            /* Free bitstream header. */
            if (context->streams [i].header)
            {
                free.ptr = context->streams [i].header;
                context->callback_ptr
                    (VIDEO_FMT_FREE, context->client_data,
                     (video_fmt_status_cb_info_type *) &free, NULL);
            }

            /* Free bitstream footer. */
            if (context->streams [i].footer)
            {
                free.ptr = context->streams [i].footer;
                context->callback_ptr
                    (VIDEO_FMT_FREE, context->client_data,
                     (video_fmt_status_cb_info_type *) &free, NULL);
            }

            /* Free timed text linked list. */
            while (context->streams [i].text)
            {
                free.ptr = context->streams [i].text->atom;
                if (free.ptr)
                {
                    context->callback_ptr
                        (VIDEO_FMT_FREE, context->client_data,
                         (video_fmt_status_cb_info_type *) &free, NULL);
                }
                free.ptr = context->streams [i].text;
                context->streams [i].text = context->streams [i].text->next;
                context->callback_ptr
                    (VIDEO_FMT_FREE, context->client_data,
                     (video_fmt_status_cb_info_type *) &free, NULL);
            }

            /* Free stream buffer. */
            video_fmt_mp4w_stream_free
                (context, &context->streams [i].buffer);
        }

        /* Free memory used by stream state structures. */
        free.ptr = context->streams;
        context->callback_ptr
            (VIDEO_FMT_FREE, context->client_data,
             (video_fmt_status_cb_info_type *) &free, NULL);
    }

    /* Free stream buffer. */
    video_fmt_mp4w_stream_free(context, &context->meta_data_buffer);

    /* Free memory used by movie writer state structure. */
    free.ptr = context;
    context->callback_ptr (VIDEO_FMT_FREE, context->client_data,
                           (video_fmt_status_cb_info_type *) &free, NULL);
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_fmt_mp4w_close

DESCRIPTION
  This function finishes up and closes the movie file being written.

DEPENDENCIES
  None.

RETURN VALUE
  VIDEO_FMT_DONE is returned if the last bit of the file was written
  successfully.  Otherwise, VIDEO_FMT_FAILURE is returned, indicating that the
  file is probably corrupted.

SIDE EFFECTS
  The movie file meta data is written and the file is closed.

===========================================================================*/
video_fmt_status_type video_fmt_mp4w_close
(
  video_fmt_mp4w_context_type  *context
)
{
    video_fmt_status_type  status = VIDEO_FMT_DONE;

    /* Finish up output file -- write out last chunk in each stream, and write
    ** out fragment meta-data.
    */
    status = video_fmt_mp4w_chunk_processing (context, 0, TRUE, TRUE);
    if (status == VIDEO_FMT_FAILURE)
    {
        return status;
    }
    else
    {
        /*Check if the file is fragmented*/
        if(context->fragment_size)
           video_fmt_mp4w_writemfra_tofile(context); 

        return VIDEO_FMT_DONE;
    }
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_fmt_mp4w_convert_time_units

DESCRIPTION
  This function converts the given time units from one timescale to another.

DEPENDENCIES
  None.

RETURN VALUE
  The given time units are converted from one timescale to another, and
  returned.

SIDE EFFECTS
  None.

===========================================================================*/
uint32 video_fmt_mp4w_convert_time_units
(
  uint32 units,
  uint32 from,
  uint32 to
)
{
    uint64 intermediate;

    /* For now, just use 64-bit integer math.  It might be more efficient to
    ** use Q-factor arithmetic in 32-bit integers, but this is more complex
    ** and currently beyond the developer's skills.
    */
    intermediate = (uint64) units;
    intermediate *= to;
    intermediate += from>>1;
    intermediate /= from;
    return (uint32) intermediate;
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_fmt_mp4w_copy_language_coded_string

DESCRIPTION
  This function makes a copy of a language coded string.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE is returned if the string was copied successfully.
  Otherwise, FALSE is returned.

SIDE EFFECTS
  The target language coded string is constructed.

===========================================================================*/
boolean video_fmt_mp4w_copy_language_coded_string
(
  video_fmt_language_coded_string_type        *target,
  const video_fmt_language_coded_string_type  *source,
  video_fmt_status_cb_func_type               callback_ptr,
  void                                        *client_data
)
{
    video_fmt_alloc_type  alloc;

    target->language = source->language;
    target->isUTF16  = source->isUTF16;
	target->length   = source->length;
    if (!source->length)
    {
        target->string.utf8 = NULL;
        return TRUE;
    }

    if (source->isUTF16)
    {
        /* if string not NULL terminated include null character*/
        if(source->string.utf16[source->length-1] != 0 )
        {
          target->length += 1;
        }
        alloc.size = sizeof (short) * target->length;

        /*Check if we need to allocate memory*/
        if(target->string.utf16 == NULL)
        {
            callback_ptr (VIDEO_FMT_ALLOC, client_data,
                          (video_fmt_status_cb_info_type *) &alloc, NULL);
            if (!alloc.ptr)
            {
                return FALSE;
            }
            target->string.utf16 = (const short *) alloc.ptr;
        }
        else
        {
            alloc.ptr = (void *)target->string.utf16;
            callback_ptr (VIDEO_FMT_REALLOC, client_data,
                          (video_fmt_status_cb_info_type *) &alloc, NULL);
            if(!alloc.ptr)
            {
                return FALSE;
            }
            target->string.utf16 = (const short *) alloc.ptr;
        }
        
        video_fmt_mp4w_wcscpy
            ((short *) target->string.utf16, source->string.utf16, source->length);
    }
    else
    {
        /* if string not NULL terminated include null character */
        if(source->string.utf8[source->length-1] != 0 )
        {
            target->length += 1;
        }
        alloc.size = sizeof (char) * target->length;
        /*Check if memory allocation is neccessary*/
        if(target->string.utf8 == NULL)
        {
            callback_ptr (VIDEO_FMT_ALLOC, client_data,
                          (video_fmt_status_cb_info_type *) &alloc, NULL);
            if (!alloc.ptr)
            {
                return FALSE;
            }
            target->string.utf8 = (const char *) alloc.ptr;
        }
        else
        {
            alloc.ptr = (void *)target->string.utf8;
            callback_ptr (VIDEO_FMT_REALLOC, client_data,
                          (video_fmt_status_cb_info_type *) &alloc, NULL);
            if (!alloc.ptr)
            {
                return FALSE;
            }
            target->string.utf8 = (const char *) alloc.ptr;
        }
        (void)std_strlcpy((char *) target->string.utf8, source->string.utf8,alloc.size);
    }
    return TRUE;
}
/* <EJECT> */
/*===========================================================================

FUNCTION video_fmt_mp4w_copy_location

DESCRIPTION
  This function makes a copy of a location

DEPENDENCIES
  None.

RETURN VALUE
  TRUE is returned if the location was copied successfully.
  Otherwise, FALSE is returned.

SIDE EFFECTS
  The target location is constructed.

===========================================================================*/
boolean video_fmt_mp4w_copy_location
(
  video_fmt_location_type       *target,
  const video_fmt_location_type *source,
  video_fmt_status_cb_func_type callback_ptr,
  void                          *client_data
)
{
    target->role      = source->role;
    target->altitude  = source->altitude;
    target->longitude = source->longitude;
    target->latitude  = source->latitude;
    if (!video_fmt_mp4w_copy_language_coded_string
          (&target->name, &source->name, callback_ptr, client_data)
        ||!video_fmt_mp4w_copy_language_coded_string
          (&target->astr_body, &source->astr_body, callback_ptr, client_data)
        || !video_fmt_mp4w_copy_language_coded_string
          (&target->add_notes, &source->add_notes, callback_ptr, client_data)
       )
    {
        MSG_ERROR ("create: alloc failure!", 0, 0, 0);
        callback_ptr (VIDEO_FMT_FAILURE, client_data, NULL, NULL);
        return FALSE;
    }
    return TRUE;
}
/* <EJECT> */
/*===========================================================================

FUNCTION video_fmt_mp4w_free_language_coded_string

DESCRIPTION
  This function frees memory used in a language coded string.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The language coded string is freed.

===========================================================================*/
void video_fmt_mp4w_free_language_coded_string
(
  video_fmt_language_coded_string_type        *target,
  video_fmt_status_cb_func_type               callback_ptr,
  void                                        *client_data
)
{
    video_fmt_free_type  free;

    if (target->isUTF16)
    {
        if (target->string.utf16)
        {
            free.ptr = (void *) target->string.utf16;
            callback_ptr
                (VIDEO_FMT_FREE, client_data,
                 (video_fmt_status_cb_info_type *) &free, NULL);
            target->string.utf16 = NULL;
        }
    }
    else
    {
        if (target->string.utf8)
        {
            free.ptr = (void *) target->string.utf8;
            callback_ptr
                (VIDEO_FMT_FREE, client_data,
                 (video_fmt_status_cb_info_type *) &free, NULL);
            target->string.utf8 = NULL;
        }
    }
}
/* <EJECT> */
/*===========================================================================

FUNCTION video_fmt_mp4w_free_loci

DESCRIPTION
  This function frees memory used in a language coded string.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The language coded string is freed.

===========================================================================*/
void video_fmt_mp4w_free_loci
(
  video_fmt_location_type                     *target,
  video_fmt_status_cb_func_type               callback_ptr,
  void                                        *client_data
)
{
   video_fmt_mp4w_free_language_coded_string
        (&target->name, callback_ptr, client_data);
   video_fmt_mp4w_free_language_coded_string
        (&target->astr_body, callback_ptr, client_data);
   video_fmt_mp4w_free_language_coded_string
        (&target->add_notes, callback_ptr, client_data);

}

/* <EJECT> */
/*===========================================================================

FUNCTION video_fmt_mp4w_encode_language_coded_string_atom

DESCRIPTION
  This function encodes and writes out an atom to contain the given language
  coded string.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE is returned if the string was copied successfully.
  Otherwise, FALSE is returned.

SIDE EFFECTS
  The language coded string is written out to the given stream buffer.

===========================================================================*/
boolean video_fmt_mp4w_encode_language_coded_string_atom
(
  video_fmt_mp4w_context_type                 *context,
  video_fmt_mp4w_stream_type                  *buffer,
  const video_fmt_language_coded_string_type  *source,
  const char                                  *atom
)
{
    uint8   temp_buffer [60];
    uint32  size;

    /* Determine atom size. */
    if (source->isUTF16)
    {
        size = 12 + 2 + (source->length * sizeof (short));
    }
    else
    {
        size = 12 + 2 + (source->length * sizeof (char));
    }

    /* Generate atom header. */
    MAKE_FULL_ATOM_HEADER (temp_buffer, size, atom, 0, 0);/*lint!e572 */

    /* Add language code. */
    STORE_ATOM_UINT16 (temp_buffer, 12, (source->language & 0x7FFF));

    /* Output atom so far. */
    if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 12 + 2))
    {
        return FALSE;
    }

    /* Write out the actual string. */
    if (source->isUTF16)
    {
        if (!video_fmt_mp4w_stream_push
            (context, buffer, (const uint8 *) source->string.utf16,
             size - (12 + 2)))
        {
            return FALSE;
        }
    }
    else
    {
        if (!video_fmt_mp4w_stream_push
            (context, buffer, (const uint8 *) source->string.utf8, size - (12 + 2)))
        {
            return FALSE;
        }
    }
    return TRUE;
}
/* <EJECT> */
/*===========================================================================

FUNCTION video_fmt_mp4w_encode_loci_atom

DESCRIPTION
  This function encodes and writes out an atom to contain the given location.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE is returned if the location was copied successfully.
  Otherwise, FALSE is returned.

SIDE EFFECTS
  The location is written out to the given stream buffer.

===========================================================================*/
boolean video_fmt_mp4w_encode_loci_atom
(
  video_fmt_mp4w_context_type                 *context,
  video_fmt_mp4w_stream_type                  *buffer,
  const video_fmt_location_type               *source,
  const char                                  *atom
)
{
    uint8   temp_buffer [60];
    uint32  size;

    /* Determine atom size. */
    if (source->name.isUTF16)
    {
        size = 12 + 2 + (source->name.length * sizeof (short));
		size+= 12 + 1 + ((source->astr_body.length + source->add_notes.length ) * sizeof (short));
    }
    else
    {
        size = 12 + 2 + (source->name.length  * sizeof (char));
		size+= 12 + 1 + ((source->astr_body.length + source->add_notes.length ) * sizeof (char));
    }

    /* Generate atom header. */
    MAKE_FULL_ATOM_HEADER (temp_buffer, size, atom, 0, 0);/*lint!e572 */

    /* Add language code. */
    STORE_ATOM_UINT16(temp_buffer, 12, (source->name.language & 0x7FFF));

    /* Output atom so far. */
    if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 12 + 2 ))
    {
        return FALSE;
    }

    /* Write out the actual string. */
    if (source->name.isUTF16)
    {
        if (!video_fmt_mp4w_stream_push
            (context, buffer, (const uint8 *) source->name.string.utf16, source->name.length * sizeof (short)))
        {
            return FALSE;
        }
    }
    else
    {
        if (!video_fmt_mp4w_stream_push
            (context, buffer, (const uint8 *) source->name.string.utf8, source->name.length ))
        {
            return FALSE;
        }
    }

    temp_buffer[0] = source->role;
	STORE_ATOM_UINT32(temp_buffer, 1 , source->longitude);
	STORE_ATOM_UINT32(temp_buffer, 1 + 4 , source->latitude );
	STORE_ATOM_UINT32(temp_buffer, 1 + 4 + 4 , source->altitude );
	
    /* Output atom so far. 13 = 1+ 4+ 4+ 4*/
    if (!video_fmt_mp4w_stream_push (context, buffer, temp_buffer, 13 ))
    {
        return FALSE;
    }

    /* Write out the other strings. */
    if (source->name.isUTF16)
    {
        if (!video_fmt_mp4w_stream_push
            (context, buffer, (const uint8 *) source->astr_body.string.utf16,
             source->astr_body.length * sizeof (short)))
        {
            return FALSE;
        }
        if (!video_fmt_mp4w_stream_push
            (context, buffer, (const uint8 *) source->add_notes.string.utf16,
             source->add_notes.length * sizeof (short)))
        {
            return FALSE;
        }
    }
    else
    {
        if (!video_fmt_mp4w_stream_push
            (context, buffer, (const uint8 *) source->astr_body.string.utf8, source->astr_body.length))
		{
			return FALSE;
		}
		if (!video_fmt_mp4w_stream_push
            (context, buffer, (const uint8 *) source->add_notes.string.utf8, source->add_notes.length))
        {
            return FALSE;
        }
    }
    return TRUE;
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_fmt_mp4w_wcscpy

DESCRIPTION
  This function makes a copy of a NULL-terminated wide string.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The destination string receives a copy of the source string.

===========================================================================*/
void video_fmt_mp4w_wcscpy
(
  short *dst,
  const short *src,
  const uint32 length
)
{
    uint32 count;
    for (count = 0; count != length ; ++count)
    {
        dst [count] = src [count];
        if(! dst[count])
        {
          break;
        }
    }
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_fmt_mp4w_put_meta_table

DESCRIPTION
  This function stores meta data table (chunks table or samples table).

DEPENDENCIES
  The following condition must be met before this function is called.
      (stream->table_stores_size < stream->max_table_stores)
  This function doesn't check the condtion.
  It is assumed that this function will be called sequentially from the start
  of the samples or chunks table to the end of the table.

RETURN VALUE
  The value TRUE is returned normally to indicate successful processing.  If
  the value FALSE is returned, it indicates a failure to store meta data
  table.

SIDE EFFECTS
  None.

===========================================================================*/
boolean video_fmt_mp4w_put_meta_table
(
  video_fmt_mp4w_context_type  *context,
  uint32                       stream_number,
  video_fmt_mp4w_table_type    type
)
{
    video_fmt_mp4w_stream_context_type  *stream;
    video_fmt_put_meta_data_type        put_meta_data;
    uint8                               *buffer;
    uint32                              size;
    uint32                              num_items;

    stream = &context->streams [stream_number];

    switch (type)
    {
    case VIDEO_FMT_MP4W_SAMPLE:
        buffer = (uint8 *) stream->samples;
        size = sizeof (video_fmt_mp4w_sample_table_entry_type)
               * (uint32) stream->samples_index;
        break;

    case VIDEO_FMT_MP4W_CHUNK:
        buffer = (uint8 *) stream->chunks;
        size = sizeof (video_fmt_mp4w_chunk_table_entry_type)
               * (uint32) stream->chunks_index;
        break;

    default:
        MSG_ERROR ("put_meta_table: illegal table type!", 0, 0, 0);
        return FALSE;
    }

    MSG_LOW ("put_meta_table: stream= %d, size= %d, type= %d",
              stream_number, size, type);

    put_meta_data.buffer = buffer;
    put_meta_data.num_bytes = size;
    put_meta_data.callback_ptr = NULL;
    put_meta_data.server_data = context;

    /* Ask client to put data out to movie file. */
    context->callback_ptr (VIDEO_FMT_PUT_META_DATA, context->client_data,
                           (video_fmt_status_cb_info_type *) &put_meta_data,
                           video_fmt_mp4w_end);

    /* Return FALSE if storing a table failed. */
    if (size != put_meta_data.num_bytes)
    {
        MSG_ERROR ("put_meta_table: failure!", 0, 0, 0);
        return FALSE;
    }

    /* Make a copy of the last entry of the stored table. */
    switch (type)
    {
    case VIDEO_FMT_MP4W_SAMPLE:
        memcpy(&stream->samples [- (int32) stream->max_samples_rate],
               &stream->samples [stream->samples_index
                                 - (int32) stream->max_samples_rate],
               sizeof (stream->samples [0]) * stream->max_samples_rate);
        ++stream->stored_samples_table;
        num_items = (uint32) stream->samples_index;
        stream->stored_samples_size += num_items;
        stream->samples_index = (int32) (stream->samples_size
                                        - stream->stored_samples_size);
        break;

    case VIDEO_FMT_MP4W_CHUNK:
        stream->chunks [-1] = stream->chunks [stream->chunks_index - 1];
        ++stream->stored_chunks_table;
        num_items = (uint32) stream->chunks_index;
        stream->stored_chunks_size += num_items;
        stream->chunks_index = (int32) (stream->chunks_size
                                        - stream->stored_chunks_size);
        break;

    default:
        MSG_ERROR ("put_meta_table: illegal table type!", 0, 0, 0);
        return FALSE;
    }

    /* Update table_stores table */
    stream->table_stores [stream->table_stores_size].id    = put_meta_data.id;
    stream->table_stores [stream->table_stores_size].size  = size;
    stream->table_stores [stream->table_stores_size].items = num_items;
    stream->table_stores [stream->table_stores_size].type  = type;
    stream->table_stores_size++;

    return TRUE;
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_fmt_mp4w_get_meta_table

DESCRIPTION
  This function reads in meta data table (chunks table or samples table).

DEPENDENCIES
  It is assumed that this function will be called sequentially from the start
  of the samples or chunks table to the end of the table.

RETURN VALUE
  The value TRUE is returned normally to indicate successful processing.  If
  the value FALSE is returned, it indicates a failure to read in meta data
  table.

SIDE EFFECTS
  None.

===========================================================================*/
boolean video_fmt_mp4w_get_meta_table
(
  video_fmt_mp4w_context_type           *context,
  uint32                                stream_number,
  video_fmt_mp4w_table_store_entry_type *stored
)
{
    video_fmt_mp4w_stream_context_type  *stream;
    video_fmt_get_meta_data_type        get_meta_data;
    uint8                               *buffer;

    MSG_LOW ("get_meta_table: stream= %d, size= %d, type= %d",
              stream_number, stored->size, stored->type);

    stream = &context->streams [stream_number];

    /* Make a copy of the last entries of the previous table. */
    switch (stored->type)
    {
    case VIDEO_FMT_MP4W_SAMPLE:
        stream->samples [-1] = stream->samples [stream->samples_index - 1];
        stream->samples_index = stored->items;
        buffer = (uint8 *) stream->samples;
        break;

    case VIDEO_FMT_MP4W_CHUNK:
        stream->chunks [-1] = stream->chunks [stream->chunks_index - 1];
        stream->chunks_index = stored->items;
        buffer = (uint8 *) stream->chunks;
        break;

    default:
        MSG_ERROR ("get_meta_table: illegal table type!", 0, 0, 0);
        return FALSE;
    }

    get_meta_data.buffer        = buffer;
    get_meta_data.id            = stored->id;
    get_meta_data.num_bytes     = stored->size;
    get_meta_data.callback_ptr  = NULL;
    get_meta_data.server_data   = context;

    /* Ask client to put data out to movie file. */
    context->callback_ptr (VIDEO_FMT_GET_META_DATA, context->client_data,
                           (video_fmt_status_cb_info_type *) &get_meta_data,
                           video_fmt_mp4w_end);

    if (stored->size != get_meta_data.num_bytes)
    {
        MSG_ERROR ("get_meta_table: failure!", 0, 0, 0);
        return FALSE;
    }

    return TRUE;
}
