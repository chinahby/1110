#ifndef VIDEOENG_AMC_H
#define VIDEOENG_AMC_H
/*===========================================================================

                     V I D E O   E N C O D E   E N G I N E
                             A M C   S U P P O R T
                             H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary for other
  tasks to interface with the video encode engine AMC support module.

Copyright(c) 2003 by QUALCOMM, Incorporated. All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.
   
  $Header: //source/qcom/qct/multimedia/camcorder/services/recorder/6k/common/main/latest/inc/videoeng_amc.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/02/07   gopi    Notify Client if re-order atom operation is not successfull.
02/27/07   jdas    Fix up 'ftyp' for only 1 fragment in fragmented format.
03/07/05   jk      Moved the declaration for video engine file object to
                   videoeng_fio.h.
03/03/05   jk      Brought back old code for AMC.
03/01/05   jk      Branched from mainline.
                   Fixed for fragmented 3g2 brand at 384Kbps.
                   Implemented video engine file object.
                   Changed AMC fixup function:
                   (1) to use video engine file object.
                   (2) to write progressively without seeking.
01/26/04    gr     Added support for buffered file I/O.
10/27/03   rpw     Added support for file fragmentation.
08/01/03   rpw     Created file.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"             /* Target specific definitions             */
#include "comdef.h"             /* Definitions for byte, word, etc.        */

#if defined (FEATURE_EFS2) || defined (T_WINNT)
#include "fs_public.h"          /* File system typedefs and prototypes     */
#include "fs_stdio.h"           /* File system buffered I/O header         */
#include "videoeng_fio.h"       /* Video engine file I/O header            */
#endif /* defined (FEATURE_EFS2) || defined (T_WINNT) */

/* <EJECT> */
/*---------------------------------------------------------------------------
** VIDEOENG_AMC Public Function Prototypes
**---------------------------------------------------------------------------
*/

#if defined (FEATURE_EFS2) || defined (T_WINNT)

#define FIXUPERROR 2

/* <EJECT> */
/*===========================================================================

             V I D E O   E N G I N E   A M C   U T I L I T Y

===========================================================================*/

/*===========================================================================

FUNCTION video_eng_amc_fixup

DESCRIPTION
  This function takes an MP4/AMC file input and generates an MP4/AMC file
  output consisting of the input rearranged into the proper atom order for
  AMC, as well as combining all 'mdat' atoms together.

DEPENDENCIES
  FEATURE_EFS2 must be defined

PARAMETERS
  IN const char *inFileName
    This is the name of the input file to be converted.

  IN const char *outFileName
    This is the name of the output file in which to store the converted
    movie.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void video_eng_amc_fixup (
  const char *inFileName,
  const char *outFileName
);

/*===========================================================================

FUNCTION video_eng_amc_fixup_reorder

DESCRIPTION
  This function takes a region of an MP4/AMC file input and generates an
  MP4/AMC file output consisting of the input rearranged into the proper atom
  order for AMC, as well as combining all 'mdat' atoms together.

DEPENDENCIES
  FEATURE_EFS2 must be defined

PARAMETERS
  IN EFS_FILE * inFile
    This is the handle to the input file to be converted.

  IN unsigned long inOffset
    This is the number to add to any positions in the input file to get
    the corresponding position in the original input.

  IN unsigned long inSize
    This is the number of bytes of the input file to process.

  IN EFS_FILE * outFile
    This is the handle to the output file in which to store the converted
    movie.

  IN unsigned long outOffset
    This is the number to add to any positions in the output file to get
    the corresponding position in the final output.

RETURN VALUE
  The value TRUE is returned if the atoms are successfully reordered.
  Otherwise, the value FALSE is returned.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean video_eng_amc_fixup_reorder (
  EFS_FILE        *inFile,
  unsigned long   inOffset,
  unsigned long   inSize,
  EFS_FILE        *outFile,
  unsigned long   outOffset
);

/*===========================================================================

FUNCTION video_eng_fast_amc_fixup_reorder

DESCRIPTION
  This function takes a region of an MP4/AMC file input and generates an
  MP4/AMC file output consisting of the input rearranged into the proper atom
  order for AMC, as well as combining all 'mdat' atoms together.

  This function doesn't seek in the output file and is faster.  
  'inFile' must be in read and write mode.

DEPENDENCIES
  FEATURE_EFS2 must be defined

PARAMETERS
  IN VIDEO_ENG_FILE * inFile
    This is the handle to the input file to be converted.

  IN unsigned long inOffset
    This is the number to add to any positions in the input file to get
    the corresponding position in the original input.

  IN unsigned long inSize
    This is the number of bytes of the input file to process.

  IN VIDEO_ENG_FILE * outFile
    This is the handle to the output file in which to store the converted
    movie.

  IN unsigned long outOffset
    This is the number to add to any positions in the output file to get
    the corresponding position in the final output.

RETURN VALUE
  The value TRUE is returned if the atoms are successfully reordered.
  Otherwise, the value FALSE is returned.

SIDE EFFECTS
  None

===========================================================================*/
extern uint8 video_eng_fast_amc_fixup_reorder (
  VIDEO_ENG_FILE  *inFile,
  unsigned long   inOffset,
  unsigned long   inSize,
  VIDEO_ENG_FILE  *outFile,
  unsigned long   outOffset,
  unsigned char   *newFtyp,
  unsigned long   newFtypSize
);

#endif /* defined (FEATURE_EFS2) || defined (T_WINNT) */
#endif /* VIDEOENG_AMC_H */
