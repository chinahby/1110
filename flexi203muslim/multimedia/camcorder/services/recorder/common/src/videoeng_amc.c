/* linesize(132)
** pagesize(60)
** title("Dual Mode Subscriber Station")
** subtitle("Video Encode Engine AMC Support")
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     V I D E O   E N C O D E   E N G I N E
                             A M C   S U P P O R T

GENERAL DESCRIPTION
  This module contains AMC file format support for the video encode engine.
  This consists of functions which supplement the video encode engine in order
  to properly support the AMC file format.

REFERENCES
  CDMA Motion Picture Terminal Audio/Video File Format Specification for
  Downloading/Streaming Short Audio/Video Clips, version 1.1.2 [KDDI]

EXTERNALIZED FUNCTIONS
  video_eng_amc_fixup
    This function takes an MP4/AMC file input and generates an MP4/AMC file
    output consisting of the input rearranged into the proper atom order for
    AMC, as well as combining all 'mdat' atoms together.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright(c) 2003 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/camcorder/services/recorder/6k/common/main/latest/src/videoeng_amc.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/02/07   gopi    Notify Client if re-order atom operation is not successfull.
02/27/07   jdas    Fix up 'ftyp' for only 1 fragment in fragmented format.
12/21/06   dhaval  Removed lint errors.
13/10/05   Gopi    Fixed Lint error of MFRA atom.
03/10/05   Gopi    Added support for Mfra Atom.
10/17/05   gm      Automated script to modify the MSG macro's
03/07/05   jk      Moved the implementation of video engine file object to
                   videoeng_fio.c.
03/04/05   jk      Made video_eng_amc_fixup_reorder() faster by swapping
                   writing 'mdat' atom and fixing 'stco'/'trun' atoms.
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
#include <stdlib.h>             /* Dynamic memory allocation               */
#include "camcorder_msg.h" 
#include "target.h"             /* Target specific definitions             */
#include "customer.h"           /* Customer specific definitions           */
#include "comdef.h"             /* Definitions for byte, word, etc.        */

#include "err.h"                /* Error logging/reporting services        */
#include <memory.h>             /* Memory manipulation routines            */

#include "videoeng_fio.h"       /* Video engine file I/O header            */
#include "videoeng_amc.h"

/* <EJECT> */
/*===========================================================================

                          DECLARATIONS FOR MODULE

===========================================================================*/

/* This macro computes the number of elements in the given array. */
#define COUNT_OF(x)  (sizeof (x) / sizeof (*x))

/* This macro returns a non-zero value if the argument is a power of 2.
** Otherwise, it returns a zero value.
*/
#define IS_POWER_OF_2(x)  (((x) & ((x) - 1)) == 0)

/* This type of structure records useful information about an MP4 atom. */
struct AtomInfoType {
    long pos;
    unsigned long size;
};

/* This type of structure maps an atom types to the expected size of the
** atom's contents (before any contained child atoms).
*/
struct AtomContentSizeType {
    const char *tag;
    unsigned long contents;
};

/* <EJECT> */
/*===========================================================================

                LOCAL DATA DECLARATIONS

===========================================================================*/

/* This is the table of atoms which are defined to be "full" atoms by the
** spec, and so contain an extra header before the atom contents.
*/
static const char * const FULL_ATOMS [] = {
    "mvhd", "iods", "tkhd", "mdhd", "hdlr", "vmhd", "smhd", "hmhd", "nmhd",
    "url ", "urn ", "dref", "stts", "ctts", "esds", "stsd", "stsz", "stsc",
    "stco", "stss", "stsh", "stdp", "elst", "cprt"
};

/* This is the table of atoms which are defined to be "container" atoms by the
** spec, and so contain other atoms.
*/
static const char * const CONTAINER_ATOMS [] = {
    "moov", "trak", "edts", "mdia", "minf", "dinf", "dref", "stbl", "stsd",
    "mp4v", "mp4a", "mp4s",

    "moof", "traf"
};

/* This lookup table maps atom types to expected sizes of the atom's contents
** (before any contained child atoms).
*/
static const struct AtomContentSizeType ATOM_CONTENT_SIZES [] = {
    {"dref", 4},
    {"stsd", 4},
    {"mp4v", 78},
    {"mp4a", 28},
    {"mp4s", 8}
};

/* <EJECT> */
/*===========================================================================

                EXTERNAL FUNCTIONS

===========================================================================*/

/* <EJECT> */
/*===========================================================================

FUNCTION n2hl

DESCRIPTION
  This function converts the given 32-bit unsigned integer from network
  order to host order.

DEPENDENCIES
  None

PARAMETERS
  IN unsigned long arg
    This is the network-order integer to converted.

RETURN VALUE
  The converted host-order integer is returned.

SIDE EFFECTS
  None

===========================================================================*/
static unsigned long n2hl (
  unsigned long arg
)
{
    unsigned char *p = (unsigned char *) &arg;
    return (((unsigned long) p[0]) << 24)
        | (((unsigned long) p[1]) << 16)
        | (((unsigned long) p[2]) << 8)
        | ((unsigned long) p[3]);
}

/* <EJECT> */
/*===========================================================================

FUNCTION tagMatch

DESCRIPTION
  This function searches for the given atom tag in the given atom tag
  table.

DEPENDENCIES
  None

PARAMETERS
  IN const char *atomTag
    This is the tag to search for in the table.

  IN const char * const *table
    This is the tag table to search.

RETURN VALUE
  A non-zero value is returned if the tag is found in the table.
  Otherwise, a zero value is returned.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL int tagMatch (
  const char *atomTag,
  const char * const *table,
  unsigned tableSize
)
{
    unsigned i;
    for (i = 0; i < tableSize; ++i)
    {
        if (!memcmp (atomTag, table [i], 4))
        {
            return 1;
        }
    }
    return 0;
}

/* <EJECT> */
/*===========================================================================

FUNCTION atomContentSize

DESCRIPTION
  This function searches for the given atom tag in the given atom content
  size lookup table, and returns the maching content size.

DEPENDENCIES
  None

PARAMETERS
  IN const char *atomTag
    This is the tag to search for in the table.

  IN const struct AtomContentSizeType *table
    This is the lookup table to search.

RETURN VALUE
  A non-zero value is returned if the tag is found in the table.  This
  value corresponds to the size of the atom's contents before any
  contained child atoms.  If the tag is not found in the table, a zero
  value is returned.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL unsigned int atomContentSize (
  const char *atomTag,
  const struct AtomContentSizeType *table,
  unsigned tableSize
)
{
    unsigned i;
    for (i = 0; i < tableSize; ++i)
    {
        if (!memcmp (atomTag, table [i].tag, 4))
        {
            return table [i].contents;
        }
    }
    return 0;
}

/* <EJECT> */
/*===========================================================================

FUNCTION parseAtom

DESCRIPTION
   This function parses the next MP4 file atom from the given file, starting
   at the given position.  The atom's size, tag, and the file offset of the
   atom's contents is returned.

DEPENDENCIES
  None

PARAMETERS
  INOUT EFS_FILE * inFile
    This is the input file structure.

  IN unsigned long pos
    This is the offset of the beginning of the atom to parse.

  IN unsigned long size
    This is the total size of the input file, in bytes.

  OUT unsigned long *atomSize
    This points to where the atom's size should be stored.

  OUT char *atomTag
    This points to a 5-character string (including NULL character) where
    the atom's tag should be stored.

  OUT unsigned long *atomContents
    This points to where the file offset of the atom's contents should be
    stored.

RETURN VALUE
  A non-zero value is returned if the atom is parsed normally.
  Otherwise, a zero value indicates an error has occurred.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL int parseAtom (
  EFS_FILE * inFile,
  unsigned long pos, unsigned long size,
  unsigned long *atomSize,
  char *atomTag, unsigned long *atomContents
)
{
    unsigned long    temp32;

    /* Return an error if the file isn't big enough to contain the size and
    ** tag fields.
    */
    if (pos + 8 > size)
    {
        CAMCORDER_MSG_ERROR ("parseAtom: Unexpected end of "
                   "file, attempting to parse atom header!", 0, 0, 0);
        return 0;
    }

    /* Seek to the indicated position in the input file. */
    (void)efs_fseek (inFile, (int32)pos, SEEK_SET);

    /* Parse atom size. */
    (void)efs_fread (&temp32, 1, sizeof (temp32), inFile);
    *atomSize = n2hl (temp32);

    /* Parse atom tag. */
    (void)efs_fread (atomTag, 1, 4, inFile);
    atomTag [4] = '\0';

    /* Return an error if the file isn't big enough to contain the entire
    ** atom, as indicated by the atom size field.
    */
    if (pos + *atomSize > size)
    {
        CAMCORDER_MSG_ERROR ("parseAtom: Atom size invalid, as it would extend "
                   "beyond end of file!", 0, 0, 0);
        return 0;
    }

    /* Compute the position of the atom's contents.  If the atom type
    ** indicates an extended type, be sure to skip the UUID.  If the atom tag
    ** indicates one of the "full" atom types, be sure to skip the version
    ** number and flags.
    */
    *atomContents = pos + 8;
    if (!memcmp (atomTag, "uuid", 4))
    {
        *atomContents += 8;
    }
    if (tagMatch (atomTag, FULL_ATOMS, COUNT_OF (FULL_ATOMS)))
    {
        *atomContents += 4;
    }
    return 1;
}

/* <EJECT> */
/*===========================================================================

FUNCTION parseAtom_2

DESCRIPTION
   This function parses the next MP4 file atom from the given file, starting
   at the given position.  The atom's size, tag, and the file offset of the
   atom's contents is returned.

DEPENDENCIES
  None

PARAMETERS
  INOUT EFS_FILE * inFile
    This is the input file structure.

  IN unsigned long pos
    This is the offset of the beginning of the atom to parse.

  IN unsigned long size
    This is the total size of the input file, in bytes.

  OUT unsigned long *atomSize
    This points to where the atom's size should be stored.

  OUT char *atomTag
    This points to a 5-character string (including NULL character) where
    the atom's tag should be stored.

  OUT unsigned long *atomContents
    This points to where the file offset of the atom's contents should be
    stored.

RETURN VALUE
  A non-zero value is returned if the atom is parsed normally.
  Otherwise, a zero value indicates an error has occurred.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL int parseAtom_2 (
  VIDEO_ENG_FILE * inFile,
  unsigned long pos, unsigned long size,
  unsigned long *atomSize,
  char *atomTag, unsigned long *atomContents
)
{
    unsigned long    temp32;

    /* Return an error if the file isn't big enough to contain the size and
    ** tag fields.
    */
    if (pos + 8 > size)
    {
        CAMCORDER_MSG_ERROR ("parseAtom_2: Unexpected end of "
                   "file, attempting to parse atom header!", 0, 0, 0);
        return 0;
    }

    /* Seek to the indicated position in the input file. */
    (void)video_eng_file_fseek (inFile, (int32)pos, SEEK_SET);

    /* Parse atom size. */
    (void)video_eng_file_fread (&temp32, 1, sizeof (temp32), inFile);
    *atomSize = n2hl (temp32);

    /* Parse atom tag. */
    (void)video_eng_file_fread (atomTag, 1, 4, inFile);
    atomTag [4] = '\0';

    /* Return an error if the file isn't big enough to contain the entire
    ** atom, as indicated by the atom size field.
    */
    if (pos + *atomSize > size)
    {
        CAMCORDER_MSG_ERROR ("parseAtom_2: Atom size invalid, as it would extend "
                   "beyond end of file!", 0, 0, 0);
        return 0;
    }

    /* Compute the position of the atom's contents.  If the atom type
    ** indicates an extended type, be sure to skip the UUID.  If the atom tag
    ** indicates one of the "full" atom types, be sure to skip the version
    ** number and flags.
    */
    *atomContents = pos + 8;
    if (!memcmp (atomTag, "uuid", 4))
    {
        *atomContents += 8;
    }
    if (tagMatch (atomTag, FULL_ATOMS, COUNT_OF (FULL_ATOMS)))
    {
        *atomContents += 4;
    }
    return 1;
}

/* <EJECT> */
/*===========================================================================

FUNCTION expandBuffer

DESCRIPTION
  This function makes sure the given buffer is at least the given size,
  allocating more memory if necessary.

DEPENDENCIES
  None

PARAMETERS
  INOUT char **buffer
    This points to the pointer to the buffer in question.  The buffer does
    not have any memory allocated yet if the buffer size is zero.

  INOUT unsigned long *bufferSize
    This points to the variable tracking the size of the buffer.  The
    buffer does not have any memory allocated yet if this variable is
    zero.

  IN unsigned long minimumSize
    This is the minimum desired size for the buffer.  The memory allocated
    for the buffer is expanded to this minimum size, if it is smaller or
    not yet allocated.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void expandBuffer (
  char **buffer,
  unsigned long *bufferSize,
  unsigned long minimumSize
)
{
    if (minimumSize && (minimumSize > *bufferSize))
    {
        if (*bufferSize)
        {
            *buffer = videoeng_realloc (*buffer, minimumSize);
        }
        else
        {
            *buffer = videoeng_malloc (minimumSize);
        }
        *bufferSize = minimumSize;
    }
}

/* <EJECT> */
/*===========================================================================

FUNCTION expandAtomInfoTable

DESCRIPTION
  This function makes sure the given atom information table is big enough to
  store at least as many entries as twice the next power of 2 higher than
  the current size.

DEPENDENCIES
  None

PARAMETERS
  INOUT struct AtomInfoType **table
    This points to the pointer to the table in question.  The table does
    not have any memory allocated yet if the table size is zero.

  INOUT unsigned long tableSize
    This is the current size of the table.  The table does not have any
    memory allocated yet if this variable is zero.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void expandAtomInfoTable (
  struct AtomInfoType **table,
  unsigned long tableSize
)
{
    if (IS_POWER_OF_2 (tableSize + 1))
    {
        if (tableSize)
        {
            *table = videoeng_realloc
                (*table, 2 * (tableSize + 1) * sizeof (**table));
        }
        else
        {
            *table = videoeng_malloc (sizeof (**table));
        }
    }
}

/* <EJECT> */
/*===========================================================================

                EXTERNAL FUNCTIONS

===========================================================================*/

/* <EJECT> */
#if defined (FEATURE_EFS2) || defined (T_WINNT)
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
void video_eng_amc_fixup (
  const char *inFileName,
  const char *outFileName
)
{
    EFS_FILE             *inFile, *outFile;

    unsigned long        inSize;
    boolean              result;
    int                  errno  = 0 ;

    /* Attempt to open input and output files. */
    inFile = efs_fopen (inFileName, "r");
    if (inFile == NULL)
    {
        errno = efs_errno;
        CAMCORDER_MSG_ERROR ("amc_fixup: error '%d' from EFS2 "
                   "when opening input file",
                   errno, 0, 0);
        return;
    }
    outFile = efs_fopen (outFileName, "w+");
    if (outFile == NULL)
    {
        errno = efs_errno;
        CAMCORDER_MSG_ERROR ("amc_fixup: error '%d' from EFS2 "
                   "when opening output file",
                   errno, 0, 0);
        (void)efs_fclose (inFile);
        return;
    }

    /* Cache the size of the input file. */
    (void)efs_fseek (inFile, 0, SEEK_END);
    inSize = (unsigned long)efs_ftell (inFile);
    (void)efs_fseek (inFile, 0, SEEK_SET);

    /* Invoke the reorder routine for the entire file. */
    result = video_eng_amc_fixup_reorder (inFile, 0, inSize, outFile, 0);

    /* Close files and return. */
    (void)efs_fclose (inFile);
    (void)efs_fclose (outFile);
    if (!result)
    {
        (void)efs_unlink (outFileName);
    }
}

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
boolean video_eng_amc_fixup_reorder (
  EFS_FILE        *inFile,
  unsigned long   inOffset,
  unsigned long   inSize,
  EFS_FILE        *outFile,
  unsigned long   outOffset
)
{
    unsigned long        inStart, outStart;

    unsigned long        pos;
    char                 atomTag [5];
    unsigned long        atomSize, atomContents;
    unsigned long        i, j, k;
    unsigned long        stco_entries, chunk_offset, new_chunk_offset;
    unsigned long        mdat_offset;

    char                 *buffer = NULL;
    unsigned long        bufferSize = 0;

    struct AtomInfoType  *mdat = NULL;
    unsigned             num_mdat = 0;
    struct AtomInfoType  *stco = NULL;
    unsigned             num_stco = 0;

    struct AtomInfoType  ftyp = {0, 0};
    struct AtomInfoType  moov = {0, 0};
    struct AtomInfoType  new_mdat = {0, 0};
    struct AtomInfoType  new_moov = {0, 0};

    boolean              fragment = FALSE;

    /* Initialize structure values. */
    ftyp.pos = -1;
    moov.pos = -1;

    /* Cache the position of the beginnings of the input and output. */
    inStart  = (unsigned long) efs_ftell (inFile);
    outStart = (unsigned long) efs_ftell (outFile);

    /*************************************************************************
     ** PASS 1
     **
     ** First we need to walk through the atom structure, recording the
     ** current positions and sizes of any 'mdat', 'moov'/'moof', and
     ** 'stco'/'trun' chunks, and associating them together.  This is
     ** important because in the next pass we will need to modify all the
     ** 'stco' chunks to keep their pointers valid in the reorganized file.
     ************************************************************************/
    for (pos = inStart; pos < inStart + inSize; pos += atomSize)
    {
        /* Parse the common elements of the next atom. */
        if (!parseAtom (inFile, pos, inStart + inSize,
                        &atomSize, atomTag, &atomContents))
        {
            if (buffer) videoeng_free (buffer);
            if (mdat) videoeng_free (mdat);
            if (stco) videoeng_free (stco);
            return FALSE;
        }

        /* Save location of 'ftyp' atom. */
        if (!memcmp (atomTag, "ftyp", 4))
        {
            /* Generate error if there is more than one in the file. */
            if (ftyp.pos != -1)
            {
                CAMCORDER_MSG_ERROR ("amc_fixup: File type compatibility atom "
                           "('ftyp') found twice", 0, 0, 0);
                if (buffer) videoeng_free (buffer);
                if (mdat) videoeng_free (mdat);
                if (stco) videoeng_free (stco);
                return FALSE;
            }
            ftyp.pos = (long)pos;
            ftyp.size = atomSize;
        }

        /* Save location of 'moov' or 'moof' atom. */
        else if (!memcmp (atomTag, "moov", 4)
                 || !memcmp (atomTag, "moof", 4))
        {
            /* Generate error if there is more than one in the file. */
            if (moov.pos != -1)
            {
                CAMCORDER_MSG_ERROR ("amc_fixup: Movie atom ('moov/moof') found twice",
                           0, 0, 0);
                if (buffer) videoeng_free (buffer);
                if (mdat) videoeng_free (mdat);
                if (stco) videoeng_free (stco);
                return FALSE;
            }
            moov.pos = (long)pos;
            moov.size = atomSize;
            fragment = (!memcmp (atomTag, "moof", 4) ? TRUE : FALSE);
        }

        /* If 'mdat' atom is found, record its position and size. */
        else if (!memcmp (atomTag, "mdat", 4))
        {
            /* Add table entry. */
            expandAtomInfoTable (&mdat, num_mdat);
            mdat [num_mdat].pos = (long)pos;
            mdat [num_mdat].size = atomSize;
            ++num_mdat;
        }

        /* If 'stco' or 'trun' atom is found, record its position and size. */
        else if (!memcmp (atomTag, "stco", 4)
                 || !memcmp (atomTag, "trun", 4))
        {
            /* Add table entry. */
            expandAtomInfoTable (&stco, num_stco);
            stco [num_stco].pos = (long)pos;
            stco [num_stco].size = atomSize;
            ++num_stco;
        }

        /* Iterate child atoms if atom is on container list. */
        if (tagMatch (atomTag, CONTAINER_ATOMS, COUNT_OF (CONTAINER_ATOMS)))
        {
            atomContents += atomContentSize
                (atomTag, ATOM_CONTENT_SIZES, COUNT_OF (ATOM_CONTENT_SIZES));
            atomSize = atomContents - pos;
        }
    }

    /* There must be one and only one 'moov' atom. */
    if (moov.pos == -1)
    {
        CAMCORDER_MSG_ERROR ("amc_fixup: Movie atom ('moov') not found in file!",
                   0, 0, 0);
        if (buffer) videoeng_free (buffer);
        if (mdat) videoeng_free (mdat);
        if (stco) videoeng_free (stco);
        return FALSE;
    }

    /*************************************************************************
     ** PASS 2
     **
     ** Now we can write out atoms, beginning with any 'ftyp' atom, followed 
     ** by all atoms that are not 'moov'/'moof' and 'mdat', followed by the 
     ** 'moov'/'moof' atom, and followed by the one combined 'mdat' atom.
     ************************************************************************/

    /*************************************************************************
     ** PASS 2 - 1
     **
     ** Write atoms beginning with any 'ftyp' atom through to 'moov'/'moof'
     ** atom, except for 'mdat' atom.
     ************************************************************************/

    /* Write out 'ftyp' atom first, if it is present in the input. */
    new_moov.pos = (long)outStart;
    if (ftyp.pos != -1)
    {
        (void)efs_fseek (inFile, ftyp.pos, SEEK_SET);
        expandBuffer (&buffer, &bufferSize, ftyp.size);
        (void)efs_fread (buffer, 1, ftyp.size, inFile);
        (void)efs_fwrite (buffer, 1, ftyp.size, outFile);
        new_moov.pos += (long)ftyp.size;
    }

    /* Next, write out any other atoms besides 'ftyp', 'moov', and 'mdat'. */
    for (pos = inStart; pos < inStart + inSize; pos += atomSize)
    {
        /* Parse the common elements of the next atom. */
        if (!parseAtom (inFile, pos, inStart + inSize,
                        &atomSize, atomTag, &atomContents))
        {
            if (buffer) videoeng_free (buffer);
            if (mdat) videoeng_free (mdat);
            if (stco) videoeng_free (stco);
            return FALSE;
        }

        /* Skip atom if it is 'ftyp', 'moov'/'moof', or 'mdat'. */
        if (!memcmp (atomTag, "ftyp", 4) || !memcmp (atomTag, "moov", 4)
            || !memcmp (atomTag, "moof", 4) || !memcmp (atomTag, "mdat", 4))
        {
            continue;
        }

        /* Write atom out to the output file. */
        (void)efs_fseek (inFile, (long)pos, SEEK_SET);
        expandBuffer (&buffer, &bufferSize, atomSize);
        (void)efs_fread (buffer, 1, atomSize, inFile);
        (void)efs_fwrite (buffer, 1, atomSize, outFile);
        new_moov.pos += (long)atomSize;
    }

    /* Next, write out the 'moov'/'moof' atom. */
    (void)efs_fseek (inFile, moov.pos, SEEK_SET);
    expandBuffer (&buffer, &bufferSize, moov.size);
    (void)efs_fread (buffer, 1, moov.size, inFile);
    (void)efs_fwrite (buffer, 1, moov.size, outFile);

    if (num_mdat > 0)
    {
        /* Calculate size of newly combined 'mdat' atom. */
        for (i = 0, new_mdat.size = 8; i < num_mdat; ++i)
        {
            new_mdat.size += mdat [i].size - 8;
        }

        /* Remember the offset where the combined 'mdat' atom is
        ** stored in the newly rearranged atom structure.
        */
        new_mdat.pos = efs_ftell (outFile);
    }

    /*************************************************************************
     ** PASS 2 - 2
     **
     ** Now that the atoms have been rearranged, the pointers in the "sample
     ** table chunk offset" ('stco') atoms or "track run" ('trun') atoms are
     ** off and need to be readjusted according to the new arrangement.  We
     ** have all the information necessary to do this:
     **   * Original position of the 'moov'/'moof' atom
     **   * Original positions and sizes of the 'stco'/'trun' atoms
     **   * Original positions and sizes of the 'mdat' atoms
     **   * New position of the combined 'mdat' atom
     ** 
     ** We'll fix the 'stco'/'trun' atoms and then write them to the output
     ** file.
     ************************************************************************/
    for (i = 0; i < num_stco; ++i)
    {
        /* Read 'stco' atom contents into a buffer. */
        (void)efs_fseek (inFile, stco [i].pos + 8, SEEK_SET);
        expandBuffer (&buffer, &bufferSize, stco [i].size - 8);
        (void)efs_fread (buffer, 1, stco [i].size - 8, inFile);

        /* If atom is for a fragment, process as 'trun' atom. */
        if (fragment)
        {
            /* Parse data offset entry. */
            /*lint -e{826} suspicious pointer to pointer conversion */
            chunk_offset = n2hl (* (const unsigned long *) (buffer + 8));

            /* Walk through table of original 'mdat' atoms, until the 'mdat'
            ** containing the given chunk offset is located.  While walking
            ** the table, compute the offset of the original 'mdat' data in
            ** the new combined single 'mdat' data.
            */
            for (k = 0, mdat_offset = 0; k < num_mdat; ++k)
            {
                if ((chunk_offset >= (unsigned long)mdat [k].pos + 8 + inOffset)
                    && (chunk_offset < (unsigned long)mdat [k].pos + inOffset
                        + mdat [k].size))
                {
                    break;
                }
                mdat_offset += mdat [k].size - 8;
            }
            if (k >= num_mdat)
            {
                CAMCORDER_MSG_ERROR ("amc_fixup: Data offset in track run ('trun') "
                           "atom points into a region of the file outside "
                           "any movie data ('mdat') atom!", 0, 0, 0);
                if (buffer) videoeng_free (buffer);
                if (mdat) videoeng_free (mdat);
                if (stco) videoeng_free (stco);
                return FALSE;
            }

            /* Overwrite the table entry with the new offset, which should be:
            **   the original offset
            ** + the base offset where the new 'mdat' atom is located
            ** + the size of all the 'mdat' atoms before the one with the chunk
            ** + difference between output and input file offsets
            ** - the original base offset of that 'mdat' atom
            */
            new_chunk_offset = chunk_offset
                + (unsigned long)new_mdat.pos
                + mdat_offset
                + (outOffset - inOffset)
                - (unsigned long)mdat [k].pos;
            /*lint -e{826} suspicious pointer to pointer conversion */
            * (unsigned long *) (buffer + 8) = n2hl (new_chunk_offset);
        }

        /* Otherwise, parse as 'stco' atom. */
        else
        {
            /* Parse chunk offset table number of entries. */
            /*lint -e{826} suspicious pointer to pointer conversion */
            stco_entries = n2hl (* (const unsigned long *) (buffer + 4));
            if (8 + stco_entries * 4 > stco [i].size)
            {
                CAMCORDER_MSG_ERROR ("amc_fixup: Sample table chunk offset ('stco') "
                           "table indicates more entries than "
                           "is possible for the size of its atom!",
                           0, 0, 0);
                if (buffer) videoeng_free (buffer);
                if (mdat) videoeng_free (mdat);
                if (stco) videoeng_free (stco);
                return FALSE;
            }

            /* Iterate through the table of chunk offsets.  For each offset,
            ** determine which original 'mdat' atom the offset points into,
            ** compute the new location of this data, and add the difference
            ** to the chunk offset.
            */
            for (j = 0; j < stco_entries; ++j)
            {
                /* Parse chunk offset entry. */
                /*lint -e{826} suspicious pointer to pointer conversion */
                chunk_offset = n2hl (* (const unsigned long *)
                                     (buffer + 8 + j * 4));

                /* Walk through table of original 'mdat' atoms, until the
                ** 'mdat' containing the given chunk offset is located.  While
                ** walking the table, compute the offset of the original
                ** 'mdat' data in the new combined single 'mdat' data.
                */
                for (k = 0, mdat_offset = 0; k < num_mdat; ++k)
                {
                    if ((chunk_offset >= (unsigned long)mdat [k].pos + 8 + inOffset)
                        && (chunk_offset < (unsigned long)mdat [k].pos + inOffset
                            + mdat [k].size))
                    {
                        break;
                    }
                    mdat_offset += mdat [k].size - 8;
                }
                if (k >= num_mdat)
                {
                    CAMCORDER_MSG_ERROR ("amc_fixup: Chunk offset in sample table "
                               "chunk offset ('stco') atom points into a "
                               "region of the file outside any movie "
                               "data ('mdat') atom!", 0, 0, 0);
                    if (buffer) videoeng_free (buffer);
                    if (mdat) videoeng_free (mdat);
                    if (stco) videoeng_free (stco);
                    return FALSE;
                }

                /* Overwrite the table entry with the new offset, which
                ** should be:
                **   the original offset
                ** + the base offset where the new 'mdat' atom is located
                ** + the size of all the 'mdat' atoms before the one with the
                **   chunk
                ** + difference between output and input file offsets
                ** - the original base offset of that 'mdat' atom
                */
                new_chunk_offset = chunk_offset
                    + (unsigned long)new_mdat.pos
                    + mdat_offset
                    + (outOffset - inOffset)
                    - (unsigned long)mdat [k].pos;
                /*lint -e{826} suspicious pointer to pointer conversion */
                * (unsigned long *) (buffer + 8 + j * 4)
                    = n2hl (new_chunk_offset);
            }
        }

        /* Adjust 'stco'/'trun' atom offset to account for the 'moov' atom
        ** having been moved to a new location in the file.
        */
        j = (unsigned long)stco [i].pos;
        stco [i].pos = (long)
            (((signed long) stco [i].pos)
             + ((signed long) new_moov.pos)
             - ((signed long) moov.pos));

        /* Write new 'stco'/'trun' atom contents out to the file. */
        (void)efs_fseek (outFile, stco [i].pos + 8, SEEK_SET);
        (void)efs_fwrite (buffer, 1, stco [i].size - 8, outFile);
    }

    /* Set output file pointer to end. */
    (void)efs_fseek (outFile, 0, SEEK_END);

    /*************************************************************************
     ** PASS 2 - 3
     **
     ** Write the one combined 'mdat' atom.
     ************************************************************************/

    /* Finally, combine together and write out all the 'mdat' atoms as one
    ** combined 'mdat' atom, if there were any 'mdat' atoms at all.
    */
    if (num_mdat > 0)
    {
        /* Write out combined 'mdat' atom header. */
        atomSize = n2hl (new_mdat.size);
        (void)efs_fwrite (&atomSize, 1, sizeof (atomSize), outFile);
        memcpy (atomTag, "mdat", 4);
        (void)efs_fwrite (atomTag, 1, 4, outFile);

        /* Write out contents of all the original 'mdat' atoms. */
        for (i = 0; i < num_mdat; ++i)
        {
            (void)efs_fseek (inFile, mdat [i].pos + 8, SEEK_SET);
            expandBuffer (&buffer, &bufferSize, mdat [i].size - 8);
            (void)efs_fread (buffer, 1, mdat [i].size - 8, inFile);
            (void)efs_fwrite (buffer, 1, mdat [i].size - 8, outFile);
        }
    }

    /* Free memory buffers and return. */
    if (buffer) videoeng_free (buffer);
    if (mdat) videoeng_free (mdat);
    if (stco) videoeng_free (stco);
    return TRUE;
} /* video_eng_amc_fixup_reorder */

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
uint8 video_eng_fast_amc_fixup_reorder (
  VIDEO_ENG_FILE  *inFile,
  unsigned long   inOffset,
  unsigned long   inSize,
  VIDEO_ENG_FILE  *outFile,
  unsigned long   outOffset,
  unsigned char   *newFtyp,
  unsigned long   newFtypSize
)
{
    unsigned long        inStart;

    unsigned long        pos;
    char                 atomTag [5];
    unsigned long        atomSize, atomContents;
    unsigned long        i, j, k;
    unsigned long        stco_entries, chunk_offset, new_chunk_offset;
    unsigned long        mdat_offset;

    char                 *buffer = NULL;
    unsigned long        bufferSize = 0;

    struct AtomInfoType  *mdat = NULL;
    unsigned             num_mdat = 0;
    struct AtomInfoType  *stco = NULL;
    unsigned             num_stco = 0;

    struct AtomInfoType  ftyp = {0, 0};
    struct AtomInfoType  moov = {0, 0};
    struct AtomInfoType  new_mdat = {0, 0};
    struct AtomInfoType  mfra = {0,0};

    boolean              fragment = FALSE;
    boolean              mem_inFile = video_eng_mem_file(inFile);
    uint8                status = 1;
    /* Initialize structure values. */
    ftyp.pos = -1;
    moov.pos = -1;

    /* Cache the position of the beginnings of the input and output. */
    inStart  = (unsigned long)video_eng_file_ftell (inFile);

    /*************************************************************************
     ** PASS 1
     **
     ** First we need to walk through the atom structure, recording the
     ** current positions and sizes of any 'mdat', 'moov'/'moof', and
     ** 'stco'/'trun' chunks, and associating them together.  This is
     ** important because in the next pass we will need to modify all the
     ** 'stco' chunks to keep their pointers valid in the reorganized file.
     ************************************************************************/
    for (pos = inStart; pos < inStart + inSize; pos += atomSize)
    {
        /* Parse the common elements of the next atom. */
        if (!parseAtom_2 (inFile, pos, inStart + inSize,
                        &atomSize, atomTag, &atomContents))
        {
            if (buffer) videoeng_free (buffer);
            if (mdat) videoeng_free (mdat);
            if (stco) videoeng_free (stco);
            return 0;
        }

        /* Save location of 'ftyp' atom. */
        if (!memcmp (atomTag, "ftyp", 4))
        {
            /* Generate error if there is more than one in the file. */
            if (ftyp.pos != -1)
            {
                CAMCORDER_MSG_ERROR ("amc_fixup: File type compatibility atom "
                           "('ftyp') found twice", 0, 0, 0);
                if (buffer) videoeng_free (buffer);
                if (mdat) videoeng_free (mdat);
                if (stco) videoeng_free (stco);
                return 0;
            }
            ftyp.pos = (long)pos;
            ftyp.size = atomSize;
        }

        /* Save location of 'moov' or 'moof' atom. */
        else if (!memcmp (atomTag, "moov", 4)
                 || !memcmp (atomTag, "moof", 4))
        {
            /* Generate error if there is more than one in the file. */
            if (moov.pos != -1)
            {
                CAMCORDER_MSG_ERROR ("amc_fixup: Movie atom ('moov/moof') found twice",
                           0, 0, 0);
                if (buffer) videoeng_free (buffer);
                if (mdat) videoeng_free (mdat);
                if (stco) videoeng_free (stco);
                return 0;
            }
            moov.pos = (long)pos;
            moov.size = atomSize;
            fragment = (!memcmp (atomTag, "moof", 4) ? TRUE : FALSE);
        }

        /* If 'mdat' atom is found, record its position and size. */
        else if (!memcmp (atomTag, "mdat", 4))
        {
            /* Add table entry. */
            expandAtomInfoTable (&mdat, num_mdat);
            mdat [num_mdat].pos = (long)pos;
            mdat [num_mdat].size = atomSize;
            ++num_mdat;
        }
        /*Store the mfra related values*/
        else if(!memcmp (atomTag, "mfra", 4))
        {
            /*Set the position and size of MFRA atom*/
            mfra.pos = (long)pos;
            mfra.size = atomSize;  
        }
        /* If 'stco' or 'trun' atom is found, record its position and size. */
        else if (!memcmp (atomTag, "stco", 4)
                 || !memcmp (atomTag, "trun", 4))
        {
            /* Add table entry. */
            expandAtomInfoTable (&stco, num_stco);
            stco [num_stco].pos = (long)pos;
            stco [num_stco].size = atomSize;
            ++num_stco;
        }

        /* Iterate child atoms if atom is on container list. */
        if (tagMatch (atomTag, CONTAINER_ATOMS, COUNT_OF (CONTAINER_ATOMS)))
        {
            atomContents += atomContentSize
                (atomTag, ATOM_CONTENT_SIZES, COUNT_OF (ATOM_CONTENT_SIZES));
            atomSize = atomContents - pos;
        }
    }

    /* There must be one and only one 'moov' atom. */
    if (moov.pos == -1)
    {
        CAMCORDER_MSG_ERROR ("amc_fixup: Movie atom ('moov') not found in file!",
                   0, 0, 0);
        if (buffer) videoeng_free (buffer);
        if (mdat) videoeng_free (mdat);
        if (stco) videoeng_free (stco);
        return 0;
    }

    /*************************************************************************
     ** PASS 2
     **
     ** Now we can write out atoms, beginning with any 'ftyp' atom, followed 
     ** by all atoms that are not 'moov'/'moof' and 'mdat', followed by the 
     ** 'moov'/'moof' atom, and followed by the one combined 'mdat' atom.
     ************************************************************************/

    /*************************************************************************
     ** PASS 2 - 1
     **
     ** Write atoms that are not 'moov'/'moof' and 'mdat', beginning with any
     ** 'ftyp' atom.
     ************************************************************************/

    /* Write out 'ftyp' atom first, if it is present in the input. */
    if (ftyp.pos != -1)
    {
        if (newFtyp)
        {
            /* Replace the 'ftyp' with newFtyp. */
            (void)video_eng_file_fwrite (newFtyp, 1, newFtypSize, outFile);
        }
        else
        {
            (void)video_eng_file_fseek (inFile, (int32)ftyp.pos, SEEK_SET);
            if (!mem_inFile)
            {
                expandBuffer (&buffer, &bufferSize, ftyp.size);
            }
            (void)video_eng_file_fcopy(outFile, inFile, ftyp.size, buffer);
        }
    }

    /* Next, write out any other atoms besides 'ftyp', 'moov', and 'mdat'. */
    for (pos = inStart; pos < inStart + inSize; pos += atomSize)
    {
        /* Parse the common elements of the next atom. */
        if (!parseAtom_2 (inFile, pos, inStart + inSize,
                        &atomSize, atomTag, &atomContents))
        {
            if (buffer) videoeng_free (buffer);
            if (mdat) videoeng_free (mdat);
            if (stco) videoeng_free (stco);
            return 0;
        }

        /* Skip atom if it is 'ftyp', 'moov'/'moof',mfra, or 'mdat'. */
        if (!memcmp (atomTag, "ftyp", 4) || !memcmp (atomTag, "moov", 4)
            || !memcmp (atomTag, "moof", 4) || !memcmp (atomTag, "mdat", 4)
            || !memcmp (atomTag, "mfra", 4)
           )
        {
            continue;
        }

        /* Write atom out to the output file. */
        (void)video_eng_file_fseek (inFile, (int32)pos, SEEK_SET);
        if (!mem_inFile)
        {
            expandBuffer (&buffer, &bufferSize, atomSize);
        }
        (void)video_eng_file_fcopy(outFile, inFile, atomSize, buffer);
    }

    if (num_mdat > 0)
    {
        /* Calculate size of newly combined 'mdat' atom. */
        for (i = 0, new_mdat.size = 8; i < num_mdat; ++i)
        {
            new_mdat.size += mdat [i].size - 8;
        }

        /* Remember the offset where the combined 'mdat' atom is
        ** stored in the newly rearranged atom structure.
        */
        new_mdat.pos = video_eng_file_ftell (outFile) + (long)moov.size;
    }

    /*************************************************************************
     ** PASS 2 - 2
     **
     ** Now that the atoms have been rearranged, the pointers in the "sample
     ** table chunk offset" ('stco') atoms or "track run" ('trun') atoms are
     ** off and need to be readjusted according to the new arrangement.  We
     ** have all the information necessary to do this:
     **   * Original position of the 'moov'/'moof' atom
     **   * Original positions and sizes of the 'stco'/'trun' atoms
     **   * Original positions and sizes of the 'mdat' atoms
     **   * New position of the combined 'mdat' atom
     ** 
     ** We'll fix the 'stco'/'trun' atoms and then write them back to the
     ** input file.
     ************************************************************************/
    for (i = 0; i < num_stco; ++i)
    {
        /* Read 'stco' atom contents into a buffer. */
        (void)video_eng_file_fseek (inFile, (int32)stco [i].pos + 8, SEEK_SET);
        expandBuffer (&buffer, &bufferSize, stco [i].size - 8);
        (void)video_eng_file_fread (buffer, 1, stco [i].size - 8, inFile);

        /* If atom is for a fragment, process as 'trun' atom. */
        if (fragment)
        {
            /* Parse data offset entry. */
            /*lint -e{826} suspicious pointer to pointer conversion */
            chunk_offset = n2hl (* (const unsigned long *) (buffer + 8));

            /* Walk through table of original 'mdat' atoms, until the 'mdat'
            ** containing the given chunk offset is located.  While walking
            ** the table, compute the offset of the original 'mdat' data in
            ** the new combined single 'mdat' data.
            */
            for (k = 0, mdat_offset = 0; k < num_mdat; ++k)
            {
                if ((chunk_offset >= (unsigned long)mdat [k].pos + 8 + inOffset)
                    && (chunk_offset < (unsigned long)mdat [k].pos + inOffset
                        + mdat [k].size))
                {
                    break;
                }
                mdat_offset += mdat [k].size - 8;
            }
            if (k >= num_mdat)
            {
                CAMCORDER_MSG_ERROR ("amc_fixup: Data offset in track run ('trun') "
                           "atom points into a region of the file outside "
                           "any movie data ('mdat') atom!", 0, 0, 0);
                if (buffer) videoeng_free (buffer);
                if (mdat) videoeng_free (mdat);
                if (stco) videoeng_free (stco);
                return 0;
            }

            /* Overwrite the table entry with the new offset, which should be:
            **   the original offset
            ** + the base offset where the new 'mdat' atom is located
            ** + the size of all the 'mdat' atoms before the one with the chunk
            ** + difference between output and input file offsets
            ** - the original base offset of that 'mdat' atom
            */
            new_chunk_offset = chunk_offset
                + (unsigned long)new_mdat.pos
                + mdat_offset
                + (outOffset - inOffset)
                - (unsigned long)mdat [k].pos;
            /*lint -e{826} suspicious pointer to pointer conversion */
            * (unsigned long *) (buffer + 8) = n2hl (new_chunk_offset);
        }

        /* Otherwise, parse as 'stco' atom. */
        else
        {
            /* Parse chunk offset table number of entries. */
            /*lint -e{826} suspicious pointer to pointer conversion */
            stco_entries = n2hl (* (const unsigned long *) (buffer + 4));
            if (8 + stco_entries * 4 > stco [i].size)
            {
                CAMCORDER_MSG_ERROR ("amc_fixup: Sample table chunk offset ('stco') "
                           "table indicates more entries than "
                           "is possible for the size of its atom!",
                           0, 0, 0);
                if (buffer) videoeng_free (buffer);
                if (mdat) videoeng_free (mdat);
                if (stco) videoeng_free (stco);
                return 0;
            }

            /* Iterate through the table of chunk offsets.  For each offset,
            ** determine which original 'mdat' atom the offset points into,
            ** compute the new location of this data, and add the difference
            ** to the chunk offset.
            */
            for (j = 0; j < stco_entries; ++j)
            {
                /* Parse chunk offset entry. */
                /*lint -e{826} suspicious pointer to pointer conversion */
                chunk_offset = n2hl (* (const unsigned long *)
                                     (buffer + 8 + j * 4));

                /* Walk through table of original 'mdat' atoms, until the
                ** 'mdat' containing the given chunk offset is located.  While
                ** walking the table, compute the offset of the original
                ** 'mdat' data in the new combined single 'mdat' data.
                */
                for (k = 0, mdat_offset = 0; k < num_mdat; ++k)
                {
                    if ((chunk_offset >= (unsigned long)mdat [k].pos + 8 + inOffset)
                        && (chunk_offset < (unsigned long)mdat [k].pos + inOffset
                            + mdat [k].size))
                    {
                        break;
                    }
                    mdat_offset += mdat [k].size - 8;
                }
                if (k >= num_mdat)
                {
                    CAMCORDER_MSG_ERROR ("amc_fixup: Chunk offset in sample table "
                               "chunk offset ('stco') atom points into a "
                               "region of the file outside any movie "
                               "data ('mdat') atom!", 0, 0, 0);
                    if (buffer) videoeng_free (buffer);
                    if (mdat) videoeng_free (mdat);
                    if (stco) videoeng_free (stco);
                    return 0;
                }

                /* Overwrite the table entry with the new offset, which
                ** should be:
                **   the original offset
                ** + the base offset where the new 'mdat' atom is located
                ** + the size of all the 'mdat' atoms before the one with the
                **   chunk
                ** + difference between output and input file offsets
                ** - the original base offset of that 'mdat' atom
                */
                new_chunk_offset = chunk_offset
                    + (unsigned long)new_mdat.pos
                    + mdat_offset
                    + (outOffset - inOffset)
                    - (unsigned long)mdat [k].pos;
                /*lint -e{826} suspicious pointer to pointer conversion */
                * ((unsigned long *) (buffer + 8 + j * 4))
                    = n2hl (new_chunk_offset);
            }
        }

        /* Write new 'stco'/'trun' atom contents back to the input file. */
        (void)video_eng_file_fseek (inFile, (int32)stco [i].pos + 8, SEEK_SET);
        (void)video_eng_file_fwrite (buffer, 1, stco [i].size - 8, inFile);
    }


    /*************************************************************************
     ** PASS 2 - 3
     **
     ** Write the 'moov'/'moof' atom, followed by the one combined 'mdat' 
     ** atom.
     ************************************************************************/

    /* Next, write out the 'moov'/'moof' atom. */
    (void)video_eng_file_fseek (inFile, (int32)moov.pos, SEEK_SET);
    if (!mem_inFile)
    {
        expandBuffer (&buffer, &bufferSize, moov.size);
    }
    (void)video_eng_file_fcopy(outFile, inFile, moov.size, buffer);

    /* Finally, combine together and write out all the 'mdat' atoms as one
    ** combined 'mdat' atom, if there were any 'mdat' atoms at all.
        */
    if (num_mdat > 0)
    {
        /* Write out combined 'mdat' atom header. */
        atomSize = n2hl (new_mdat.size);
        (void)video_eng_file_fwrite (&atomSize, 1, sizeof (atomSize), outFile);
        memcpy (atomTag, "mdat", 4);
        (void)video_eng_file_fwrite (atomTag, 1, 4, outFile);

        /* Write out contents of all the original 'mdat' atoms. */
        for (i = 0; i < num_mdat; ++i)
        {
            (void)video_eng_file_fseek (inFile, (int32)mdat [i].pos + 8, SEEK_SET);
            if (!mem_inFile)
            {
                expandBuffer (&buffer, &bufferSize, mdat [i].size - 8);
            }
            //Check if re-order was sucessfull
            if (!video_eng_file_fcopy(outFile, inFile, mdat [i].size - 8, buffer))
            {
                CAMCORDER_MSG_ERROR ("Fix up Error In camcorder",0, 0, 0);
                //Indicates that re-ordering is not successful
                status = FIXUPERROR;          
            }
        }
    }
    /*Check if there is MFRA atom that needs to be written*/
    if(mfra.size)
    {
        (void)video_eng_file_fseek (inFile, (int32)mfra.pos, SEEK_SET);
        if (!mem_inFile)
        {
            expandBuffer (&buffer, &bufferSize, mfra.size);
        }
        /*Copy the Contents to the output file*/
        (void)video_eng_file_fcopy(outFile, inFile, mfra.size, buffer);
    }
    /* Free memory buffers and return. */
    if (buffer) videoeng_free (buffer);
    if (mdat) videoeng_free (mdat);
    if (stco) videoeng_free (stco);
    return status;
} /* video_eng_fast_amc_fixup_reorder */

#endif /* defined (FEATURE_EFS2) || defined (T_WINNT) */
