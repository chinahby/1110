
#ifndef _FILETOOL_H
#define _FILETOOL_H


/*=============================================================================

                        File Tool

GENERAL DESCRIPTION

    This header file contains declarations and definitons for the File tool
    software.


INITIALIZATION AND SEQUENCING REQUIREMENTS
    None.

Copyright (c) 2004, 2005 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:$
$Header: //depot/asic/msmshared/tools/headergen/filetool.h#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/15/06   anb     Added support for partition table for NOR builds.
09/27/06   drh     Changed file_size to return error code
01/09/05   chs     Added filen_cmp.
11/01/04   chs     created.


============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "boot_comdef.h"


/*===========================================================================
                      PUBLIC DATA DECLARATIONS
===========================================================================*/

/****************************************************************
 * Defines
 ***************************************************************/

#define BUF_SIZE	0x4000      /*  Read/Write 16K at a time.   */


#define ERROR 		-1
#define	OK			0

#define READ_MODE       "rb"
#define WRITE_MODE      "wb"

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/



/*===========================================================================

FUNCTION  FILE_CMP

DESCRIPTION

    This function is used to compare the contents of two file.
	The first and the second argument to the function is the file pointers to
	be compared.

DEPENDENCIES
    None.

RETURN VALUE
    0	if the two files have the same contents	else
	1

SIDE EFFECTS
    none.

===========================================================================*/

uint32 file_cmp
(
    FILE *,	/*	file pointer. 	*/
	FILE *	/*	file pointer.	*/
);

/*===========================================================================

FUNCTION  FILEN_CMP

DESCRIPTION

    This function is used to compare the contents of two file.
    The first and the second argument to the function is the file pointers to
    be compared. The third argument to this function is the number
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
    FILE *, /*  file pointer.   */
    FILE *, /*  file pointer.   */
    uint32  /*  number of bytes to compare. */
);


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
    0	if the two files have the same contents	else
	1

SIDE EFFECTS
    none.

===========================================================================*/


uint32 file_cmp_const
(
    FILE *,		/*	file pointer. 	*/
	uint8,		/*	constant value with which the file is compared.		*/
	uint32		/*	number of bytes for which the comparison is done.	*/
);


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
	ERROR	if the source file reached eof else
	OK

SIDE EFFECTS
    none.

===========================================================================*/

uint32 file_cpy
(
    FILE *,			/*	output file pointer.		*/
	FILE *,			/*	input file pointer.			*/
	uint32			/*	number of bytes to copy.	*/
);



/*===========================================================================

FUNCTION  FILE_BUFCPY

DESCRIPTION

    This function is used to copy the contents of a buffer to the request file.
    The first argument to the function is the destination file pointer,
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
    FILE   *,   /* output file pointer. */
    uint8  *,   /* buffer pinter.       */
    uint32      /* buffer size.         */
);


/*===========================================================================

FUNCTION  FILE_APPEND_CONST

DESCRIPTION

    This function is used to write a constant value to a file. The first
	argument to the function is the file pointer, the second argument is
	the constant value to write and the third argument is the number of
	bytes to write.

DEPENDENCIES
    None.

RETURN VALUE
	OK

SIDE EFFECTS
    none.

===========================================================================*/

uint32 file_append_const
(
    FILE *,		/*	file pointer to write to.		*/
    uint8,		/*	constant that will be written to the file.	*/
    uint32		/*	number of bytes to write.	*/
);


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
	char   *,	/*	file name	*/
	uint32 *	/*	file size	*/
);



/*===========================================================================

FUNCTION  FILE_OPEN

DESCRIPTION

    This function is used to open a file in the requested mode.

DEPENDENCIES
    None.

RETURN VALUE
    OK		if the fopen call is a success	else
	ERROR

SIDE EFFECTS
    none.

===========================================================================*/



uint32 file_open
(
	char *,		/*	file name to open.					*/
	char *,		/*	mode in which file will be opened.	*/
	FILE **		/*	file pointer.	*/
);


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
    FILE   *,   /* file pointer. */
    uint8  *,   /* buffer pinter.*/
    uint32      /* buffer size.  */
);


#endif /* _FILETOOL_H */
