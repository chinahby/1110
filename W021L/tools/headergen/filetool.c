
 
/*=============================================================================

                        File Tool Software

GENERAL DESCRIPTION

This file contains code to implement the file tool software. The following
functions are implemented:

file_cpy

    This function is used to append the contents of the source file to the
    destination file.
    
file_append_const

    This function is used to write a constant value to a file. The write is
    done at the eof and the number of bytes to write is passed as an argument.
    
file_cmp

    This function is used to compare two files.

filen_cmp
    This function is used to compare two files. The number of
    bytes to compare is passed as an argument.

file_cmp_const

    This function is used to compare the contents of a file with a constant.
    The number of byte to compare is passed as an argument.

file_open

    This function is used to open a file in the requested mode.
    
file_size

    This function is used to return the size of a file.
    
file_bufcpy

    This function is used to copy the contents of a buffer to the request file.

file_read

    This function is used to read the contents of the requested file into
    a buffer.


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
    None.

Copyright (c) 2004, 2005, 2006 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/tools/headergen/filetool.c#4 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/27/06   drh     Changed file_size to return error/success
09/09/06   amw     Added error messages for file IO errors.
01/23/05   chs     Added arguments checks.
01/09/05   chs     Added filen_cmp.
11/01/04   chs     Created.

=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/

#include "filetool.h"
#include <errno.h>
#include <string.h>

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.


=============================================================================*/



/*---------------------------------------------------------------------------
  
---------------------------------------------------------------------------*/

/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/




/*===========================================================================

FUNCTION  FILE_CMP

DESCRIPTION

    This function is used to compare the contents of two file.
    The first and the second argument to the function is the file pointers to
    be compared.

DEPENDENCIES
    None.

RETURN VALUE
    0   if the two files have the same contents else
    1

SIDE EFFECTS
    none.

===========================================================================*/

uint32 file_cmp
(
    FILE *f1_ptr, /* file pointer. */
    FILE *f2_ptr  /* file pointer. */
)
{
    char buf1[BUF_SIZE], buf2[BUF_SIZE]; /* file buffers. */
    int  numread1, numread2; /* number of bytes read. */

        /* argument checks */
    if ((f1_ptr == NULL) || (f2_ptr == NULL))
    {
        return ERROR;
    }

        /* read till eof is reached. */
    while (!feof(f2_ptr))
    {
        numread1 = 0;
        numread2 = fread(buf2, sizeof (char), BUF_SIZE, f2_ptr);
    
        while (numread1 < numread2)
        {
                /* exit if eof is reached. */
            if (feof (f1_ptr))
            {
                return 1;
            }
            numread1 += fread ((buf1+numread1), sizeof (char),
                               (numread2 - numread1), f1_ptr);
        }
            /* compare the contents of the two files. */
        if (memcmp (buf1, buf2, numread2) != 0)
        {
            return 1;
        }
    }

    return 0;

} /* file_cmp () */


/*===========================================================================

FUNCTION  FILEN_CMP

DESCRIPTION

    This function is used to compare the contents of two file.
    The first and the second argument to the function is the file pointers
    to be compared. The third argument to this function is the number
    of bytes to be compared.

DEPENDENCIES
    None.

RETURN VALUE
    0   if the two files have the same contents else
    1

SIDE EFFECTS
    none.

===========================================================================*/

uint32 filen_cmp
(
    FILE *f1_ptr, /* file pointer. */
    FILE *f2_ptr, /* file pointer. */
    uint32 numbytes /* number of bytes to compare. */
)
{
    char buf1[BUF_SIZE], buf2[BUF_SIZE]; /* file buffers. */
    int  numread1, numread2; /* number of bytes read. */
    uint32 read_size = BUF_SIZE; /* read size. */
    uint32 numleft = numbytes;   /* number of bytes left to be written. */

        /* argument checks. */
    if ((f1_ptr == NULL) || (f2_ptr == NULL))
    {
        return ERROR;
    }

        /* read till eof is reached. */
    while (numleft > 0)
    {
        if (numleft < BUF_SIZE)
        {
            read_size = numleft;
        }

            /* exit if eof is reached. */
        if (feof (f2_ptr))
        {
            return 0;
        }

        numread1 = 0;
        numread2 = fread(buf2, sizeof (char), read_size, f2_ptr);
        numleft -= numread2;

        while (numread1 < numread2)
        {
                /* exit if eof is reached. */
            if (feof (f1_ptr))
            {
                return 1;
            }
            numread1 += fread ((buf1+numread1), sizeof (char),
                               (numread2 - numread1), f1_ptr);
        }
            /* compare the contents of the two files. */
        if (memcmp (buf1, buf2, numread2) != 0)
        {
            return 1;
        }
    }

    return 0;

} /* filen_cmp () */



/*===========================================================================

FUNCTION  FILE_CMP_CONST

DESCRIPTION

    This function is used to compare the contents of a file with a constant.
    The first argument is the file pointer, the second is the constant value
    to which comparison is made and the third argument is the number of bytes
    to compare.

DEPENDENCIES
    None.

RETURN VALUE
    0   if the two files have the same contents else
    1

SIDE EFFECTS
    none.

===========================================================================*/

uint32 file_cmp_const
(
    FILE  *file_ptr, /* file pointer. */
    uint8 val,       /* constant value with which the file is compared.  */
    uint32 numbytes  /* number of bytes for which the comparison is done.*/
)
{
    char   buf1[BUF_SIZE], buf2[BUF_SIZE];  /* file buffer. */
    uint32 numread; /* number of bytes to read. */
    uint32 numleft   = numbytes; /* number of bytes left to be read. */
    uint32 read_size = BUF_SIZE; /* read size. */

        /* argument checks */
    if (file_ptr == NULL)
    {
        return ERROR;
    }

        /* set all the entries in the buffer to the constant value. */
    memset(buf1, val, sizeof(buf1));

        /* read till eof is reached. */
    while (numleft > 0)
    {
        if (numleft < BUF_SIZE)
        {
            read_size = numleft;
        }
            /* exit if eof is reached. */
        if (feof (file_ptr))
        {
            return 1;
        }
            /* read the input file. */
        numread = fread(buf2, sizeof (char), read_size, file_ptr);
        numleft -= numread;

            /* file comparison. */
        if (memcmp (buf1, buf2, numread) != 0)
        {
            return 1;
        }
    }

    return 0;

} /* file_cmp () */



/*===========================================================================

FUNCTION  FILE_APPEND_CONST

DESCRIPTION

    This function is used to write a constant value to a file. The first
    argument to the function is the file pointer, the second argument
    is the constant value to write and the third argument is the number
    of bytes to write.

DEPENDENCIES
    None.

RETURN VALUE
    OK

SIDE EFFECTS
    none.

===========================================================================*/

uint32 file_append_const
(
    FILE  *file_ptr,  /* file pointer. */
    uint8  const_val, /* constant that will be written to the file. */
    uint32 numbytes   /* number of bytes to write. */
)
{
    char    buf[BUF_SIZE];  /* file buffer. */
    uint32  numleft = numbytes; /* number of bytes left to be written. */
    uint32  write_size = BUF_SIZE;  /* write size */

        /* argument checks */
    if (file_ptr == NULL)
    {
        return ERROR;
    }

        /* set all the enteries in the buffer to the constant value. */
    memset (buf, const_val, sizeof (buf));

        /* write the requested number of bytes to the file. */
    while (numleft > 0)
    {
        if (numleft < BUF_SIZE)
        {
            write_size = numleft;
        }

        numleft -= fwrite(buf, sizeof (char), write_size, file_ptr);
    }

    return OK;

} /* file_append_const () */


/*===========================================================================

FUNCTION  FILE_CPY

DESCRIPTION

    This function is used to copy the contents of one file to the other.
    The first argument to the function is the destination file pointer,
    the second argument is source file pointer and the third argument
    is the number of bytes to write.

DEPENDENCIES
    None.

RETURN VALUE
    ERROR   if the source file reached eof else
    OK

SIDE EFFECTS
    none.

===========================================================================*/

uint32 file_cpy
(
    FILE *opfile_ptr, /* output file pointer. */
    FILE *ipfile_ptr, /* input file pointer.  */
    uint32 numbytes   /* number of bytes to copy. */
)
{
    char buf[BUF_SIZE]; /* file buffer. */
    uint32 numread, numwrote;  /* number of bytes read/wrote. */
    uint32 numleft = numbytes; /* number of bytes left to be written. */
    uint32 read_size = BUF_SIZE; /* read size */

        /* argument checks. */
    if ((ipfile_ptr == NULL) || (opfile_ptr == NULL))
    {
        return ERROR;
    }
        /* copy the requested number of bytes to the output file. */
    while (numleft > 0)
    {
        if (numleft < BUF_SIZE)
        {
            read_size = numleft;
        }
            /* exit if eof is reached. */
        if (feof (ipfile_ptr))
        {
            printf("ERROR: Source file reached end of file. Write failed.\n");
            return ERROR;
        }

        numwrote = 0;

            /* read the input file. */
        numread  = fread(buf, sizeof (char), read_size, ipfile_ptr);
        numleft -= numread;

            /* write to the output file. */
        while (numwrote < numread)
        {
            numwrote += fwrite((buf+numwrote), sizeof (char),
                               (numread - numwrote), opfile_ptr);
        }
    }

    return OK;

} /* file_cpy () */


/*===========================================================================

FUNCTION  FILE_BUFCPY

DESCRIPTION

    This function is used to copy the contents of a buffer to the request
    file. The first argument to the function is the destination file pointer,
    the second argument is the buffer to read from and the third argument
    is the buffer size.

DEPENDENCIES
    None.

RETURN VALUE
    OK

SIDE EFFECTS
    none.

===========================================================================*/

uint32 file_bufcpy
(
    FILE   *file_ptr, /* file pointer. */
    uint8  *buf,    /* buffer pinter.*/
    uint32 buf_size /* buffer size. */
)
{
    uint32  numwrote = 0;   /* number of bytes written. */
    uint32  numleft = buf_size; /* number of bytes left to be written. */
    uint32  write_size = BUF_SIZE;  /* write size. */

        /* argument checks */
    if ((file_ptr == NULL) || (buf == NULL))
    {
        return ERROR;
    }
        /* copy the requested number of bytes to the output file. */
    while (numwrote < buf_size)
    {
        if ((buf_size - numwrote) < BUF_SIZE)
        {
            write_size = numleft;
        }

        numwrote += fwrite((buf+numwrote),sizeof (uint8),write_size, file_ptr);
    }

    return OK;

} /* file_bufcpy () */


/*===========================================================================

FUNCTION  FILE_READ

DESCRIPTION

    This function is used to read the contents of the requested file.
    The first argument to the function is the source file pointer,
    the second argument is the buffer to read to and the third argument
    is the buffer size.

DEPENDENCIES
    None.

RETURN VALUE
    OK

SIDE EFFECTS
    none.

===========================================================================*/

uint32 file_read
(
    FILE   *fp,     /* file pointer. */
    uint8  *buf,    /* buffer pinter.*/
    uint32 buf_size /* buffer size.  */
)
{
    uint32 numread = 0; /* number of bytes read. */
    uint32 numleft = buf_size;  /* number of bytes left to be written. */
    uint32 read_size = BUF_SIZE; /* read size. */

        /* read the requested number of bytes to the output buffer. */
    while (numread < buf_size)
    {
        if (feof (fp))
        {
            printf("file_read: eof reached, exiting.\n");
            return ERROR;
        }

        if ((buf_size - numread) < BUF_SIZE)
        {
            read_size = (buf_size - numread);
        }

        numread += fread ((buf+numread), sizeof (uint8), read_size, fp);
    }

    return OK;

} /* file_read () */


/*===========================================================================

FUNCTION  FILE_OPEN

DESCRIPTION

    This function is used to open a file in the requested mode.

DEPENDENCIES
    None.

RETURN VALUE
    OK      if the fopen call is a success  else
    ERROR

SIDE EFFECTS
    none.

===========================================================================*/

uint32 file_open
(
    char *fname_ptr, /* file name to open. */
    char *fmode_ptr, /* mode in which file will be opened. */
    FILE **file_ptr  /* file pointer. */
)
{
        /* argument checks. */
    if ((fname_ptr == NULL) || (fmode_ptr == NULL) || (file_ptr == NULL))
    {
        return ERROR;
    }
        /* open file */
    if ((*file_ptr = fopen(fname_ptr, fmode_ptr)) == NULL)
    {
        printf("Cant open %s file in %s mode.\n", fname_ptr, fmode_ptr);
        printf("Error reason :: %s.\n", strerror(errno));
        return ERROR;
    } 
        /* write will be done in unbuffered mode. */
    if (strncmp (fmode_ptr, "w", 1) == 0)
    {
        setbuf (*file_ptr, NULL);
    }

    return OK;

} /* file_open () */



/*===========================================================================

FUNCTION  FILE_SIZE

DESCRIPTION

    This function is used to calculate the file size of the input file.

DEPENDENCIES
    None.

RETURN VALUE
    OK if success
    ERROR if failure

SIDE EFFECTS
    None.

===========================================================================*/

int file_size
(
    char   *fname_ptr, /* file name */
    uint32 *fsize_ptr  /* file size */
)
{
    struct stat fstats;
    int status;

        /* calculate file size for the requested file. */
    status = stat(fname_ptr, &fstats);

    if (status == 0) {
        /* Success, the file exists */
        *fsize_ptr = fstats.st_size;
        return (OK);
    }

    /* Failure of some kind */
    return (ERROR);
} /* file_size () */

