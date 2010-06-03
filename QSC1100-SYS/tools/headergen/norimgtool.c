/*=============================================================================

                        NOR Image Tool

GENERAL DESCRIPTION

    This file contains code to implement the NOR Image tool. This tool expects
    six arguments as input. The first three arguments are the input binary
    files and last argument is the name of the composite image file.

    The tool combines the three input binary files and forms a single composite
    image file.

    Each input binary file has a fixed start address, which is obtained from
    the macros defined in miheader.h. The start address and image size of the
    input binary files are compared to ensure that the images don't overlap
    each other. The tool also ensures that the total image size doesn't exceed
    the composite image size defined by NOR_IMG_SIZE.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
    None.

Copyright (c) 2004,2005 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  $
  $Header: //depot/asic/msmshared/tools/headergen/norimgtool.c#5 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/10/06   chs     Added support for Profiling.
05/10/05   chs     Increased NOR Image size to 32MB.
03/01/05   chs     Increased NOR Image size to 16MB.
01/15/05   chs     modified for new nor architecture.
10/19/04   chs     created.


=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/

#include "filetool.h"


/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

#define NUM_FILES       8UL
#define CONST_VAL       0xFFUL
#define NOR_IMG_SIZE    0x2000000UL  /* 32MB is our image size. */

#undef  DBUG_NORIMGTOOL

static char     ipfname[NUM_FILES][80]; /* input file names.    */
static uint32   fsize[NUM_FILES];       /* input file size.     */
static uint32   fstart_addr[NUM_FILES]; /* start address of the input binaries
                                        ** in our nor image.
                                        */
static char     opfname[80]; /* output file name. */
static uint32   write_done[NUM_FILES];


/*---------------------------------------------------------------------------
  
---------------------------------------------------------------------------*/



/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/


static uint32 check_file_errors (void);
static uint32 verify_comp_image (void);
static void   save_file_info (char **);
static uint32 create_comp_img (void);
static uint32 get_file_index (void);


int main(uint32 argc, char *argv[])
{
        /* argument checks. */
    if (argc != (NUM_FILES + 2))
    {
        printf ("USAGE: norimgtool AMSS_BOOT QCSBL_HEADER QCSBL OEMSBL OEMSBL_HEADER AMSS_HEADER AMSS_APP COMP_IMG_NAME\n");
        return ERROR;
    }
        /* save passed arguments. */
    save_file_info (argv);

        /* check for file errors. */
    if (check_file_errors() == ERROR)
    {
        return ERROR;
    }
        /* create the composite image. */
    printf("Generating composite image %s\n", opfname);
    if (create_comp_img () == ERROR)
    {
        return ERROR;
    }
        /* check the composite image and verify its contents. */
    printf("Verifying composite image %s\n", opfname);
    if (verify_comp_image () == ERROR)
    {
        return ERROR;
    }

    printf("Composite image %s was generated and verified successfully.\n",
            opfname);
    return OK;

} /* main () */


/*===========================================================================
FUNCTION  CREATE_COMP_IMG

DESCRIPTION
    This function will create our composite image by combining the input
    images passed as arguments.
    
DEPENDENCIES
    None.

RETURN VALUE
    ERROR    if file_cpy or file_append_const returns an ERROR else
    OK

SIDE EFFECTS
    Creates the composite image.

===========================================================================*/

uint32 create_comp_img (void)
{
    uint32 i; /* file index. */
    uint32 ipfsize = 0, opfsize = 0;/* input and output file size. */
    FILE *ipfile_ptr, *opfile_ptr;  /* File pointers. */
    uint32  done = 0;  /* while loop indicator. */ 

        /* open the output file in write mode. */
    if (file_open (opfname, "wb", &opfile_ptr) == ERROR)
    {
        return ERROR;
    }

        /* Read all the input binaries to create the composite image. */
    while (!done)
    {
            /* get the index of the file to write. */
	if ((i = get_file_index ()) == ERROR)
	{
	    done = 1;
	    continue;
	}
            /* get the file size. */
	file_size (ipfname[i], &ipfsize);

            /* open the input file in read mode. */
        if (file_open (ipfname[i], "rb", &ipfile_ptr) == ERROR)
        {
            write_done[i] = 1;
            continue;
        }
            /* Fill unused space in our nor image with CONST_VAL. */ 
        if (opfsize != fstart_addr[i])
        {
            #ifdef DBUG_NORIMGTOOL
                printf("Address 0x%x to 0x%x will be filled with 0xff.\n",
                        opfsize, (fstart_addr[i] - 1));
            #endif
			
            if (file_append_const (opfile_ptr, CONST_VAL,
                (fstart_addr[i] - opfsize)) == ERROR)
            {
                return ERROR;
            }
        }

        #ifdef DBUG_NORIMGTOOL
            printf ("Writing image %s to the composite image.\n", ipfname[i]);
        #endif
                /* copy the input binary to our nor image. */
	if (file_cpy (opfile_ptr, ipfile_ptr, ipfsize) == ERROR)
	{
            return ERROR;
        }

        write_done[i] = 1;

            /* calculate file size. */
	opfsize = ftell (opfile_ptr);

            /* close the input file. */
        fclose (ipfile_ptr);
    }
        /* close the output file. */
    fclose (opfile_ptr);

    return OK;

} /* create_comp_img () */


/*===========================================================================
FUNCTION    VERIFY_COMP_IMAGE

DESCRIPTION

    This function is used to check if the composite image file contains valid
    data.

    Each input binary file has a fixed start address, which is obtained from
    the macros defined in miheader.h. The start address and image size of the
    input binary files is used to ensure that the input files were written
    at the correct location in the composite image file. Also, the contents
    of the composite image is checked with the input files to ensure it contains
    valid data.

    The unused addresses in the composite image file was filled with one during
    it creation. So, we verify the unused locations in the composite image file
    to ensure it contains only 1s.

DEPENDENCIES
    None.

RETURN VALUE
    OK      if the composite image contains only 1's in the address range else
    ERROR

SIDE EFFECTS
    None.

===========================================================================*/

uint32 verify_comp_image (void)
{
    FILE *opfile_ptr; /* File pointer to our secure elf image. */
    FILE *ipfile_ptr[NUM_FILES]; /* file pointers to our input binaries. */
    int  i;  /* index. */
    uint32 addr = 0;  /* current file address. */
    int done = 0;  /* while loop indicator. */ 

        /* open the output file in read mode. */
    if (file_open (opfname, "rb", &opfile_ptr) == ERROR)
    {
        return ERROR;
    }

        /* reset file write indicator. */
    for (i = 0; i < NUM_FILES; i++)
    {
        write_done[i] = 0;
    }

    /*  Compare the contents of all the input files with the
    **  output file to ensure that the output file contains
    **  valid data.
    */

    while (!done)
    {
            /* get the index of the file to write. */
        if ((i = get_file_index ()) == ERROR)
        {
            done = 1;
            continue;
        }

            /* Is this an unused region in our nor image? */
        if (addr != fstart_addr[i])
        {
            #ifdef DBUG_NORIMGTOOL
                printf("Address 0x%x to 0x%x is being verified for 1's....",
                        addr, (fstart_addr[i] - 1));
            #endif
            if (file_cmp_const (opfile_ptr, CONST_VAL, (fstart_addr[i] - addr)) != 0)
            {
                printf("verification failed.\n");
                return ERROR;
            }
            #ifdef DBUG_NORIMGTOOL
                printf("verification done.\n");
            #endif
            addr = fstart_addr[i];
        }
            /* open the input file in read mode. */
        if (file_open (ipfname[i], "rb", &(ipfile_ptr[i])) == ERROR)
        {
            write_done[i] = 1;
            continue;
        }
            /* compare the input and the output file. */
        #ifdef DBUG_NORIMGTOOL
            printf ("Verifying %s and %s...", ipfname[i], opfname);
        #endif
        if (file_cmp (opfile_ptr, ipfile_ptr[i]) != 0)
        {
            printf("verification failed.\n");
            return ERROR;
        }
        #ifdef DBUG_NORIMGTOOL
            printf("verification done.\n");
        #endif

        addr += fsize[i];
        write_done[i] = 1;

            /* close the input  file. */
        fclose (ipfile_ptr[i]);
    }
       /* close the output file. */
    fclose (opfile_ptr);

     return OK;

} /* verify_comp_image () */

/*===========================================================================

FUNCTION  CHECK_FILE_ERRORS

DESCRIPTION

    This function will compare the file size and starting address of the
    input files and detects if there are any file overlaps. It also
    calculates the total image size to ensure it does'nt exceed the
    NOR_IMG_SIZE macro.

DEPENDENCIES
    None.

RETURN VALUE
    ERROR   if it detects any file overlaps or if the composite image size
            exceeds the NOR_IMG_SIZE macro else
    OK

SIDE EFFECTS
    None.

===========================================================================*/

uint32 check_file_errors (void)
{
    int sum = 0; /* sum of size of all input binaries. */
    int i, j; /* index. */

        /* calculate the total image size. */
    for (i = 0; i < NUM_FILES; i++)
    {
        sum += fsize[i];
    }
        /* check if the total image size exceeds NOR_IMG_SIZE. */
    if (sum > NOR_IMG_SIZE)
    {
        printf("ERROR: Sum of all the images exceeds 0x%x\n", NOR_IMG_SIZE);
        return ERROR;
    }
        /* check for file operlaps. */
    for (i = 0; i < (NUM_FILES - 1); i++)
    {
        for (j = i; j < (NUM_FILES - 1); j++)
        {
            if (((fstart_addr[i]+fsize[i]) > fstart_addr[j]) &&
                 (fstart_addr[i] < fstart_addr[j]))
            {
                printf("ERROR: %s and %s overlap.\n",ipfname[i], ipfname[j]);
                return ERROR;
            }
        }
    }

    if ((fstart_addr[NUM_FILES-1]+fsize[NUM_FILES-1]) > NOR_IMG_SIZE)
    {
        printf("ERROR: %s accesses invalid address.\n",ipfname[NUM_FILES-1]);
        return ERROR;
    }

    return OK;

} /* check_file_errors () */


/*===========================================================================

FUNCTION    SAVE_FILE_INFO

DESCRIPTION

    This function is used to save the file name information passed by the
    user. The starting address and the file size for the corresponding
    files is also saved.

DEPENDENCIES
    None.

RETURN VALUE
    None.

SIDE EFFECTS
    None.

===========================================================================*/

void save_file_info
(
    char *fnames[]  /* input file name. */
)
{
    int i; /* file index. */

        /* save the starting address. */
    fstart_addr[0] = MI_NOR_AMSS_IMG_BOOT_SRC_ADDR;
    fstart_addr[1] = MI_NOR_QCSBL_HEADER_CFG_DATA_SRC_ADDR;
    fstart_addr[2] = MI_NOR_QCSBL_IMG_SRC_ADDR;
    fstart_addr[3] = MI_NOR_OEM_SBL_IMG_SRC_ADDR;
    fstart_addr[4] = MI_NOR_OEM_SBL_HEADER_SRC_ADDR;
    fstart_addr[5] = MI_NOR_AMSS_HEADER_SRC_ADDR;
    fstart_addr[6] = MI_NOR_AMSS_IMG_APP_SRC_ADDR;

        /* save the input fine name and calculate the input file size. */
    for (i = 1; i <= NUM_FILES; i++)
    {
        fsize[i-1] = 0;
        strcpy (ipfname[i-1], fnames[i]);
        file_size (fnames[i], &(fsize[i-1]));
        write_done[i-1] = 0;
    }

    fstart_addr[7] = fstart_addr[6] + fsize[6];

    /* save the input fine name and calculate the input file size. */
    for (i = 0; i < NUM_FILES; i++)
    {
        #ifdef DBUG_NORIMGTOOL
            printf("File Name = %s\tFile Size = 0x%x\tStart Address = 0x%x\n",
                    ipfname[i], fsize[i], fstart_addr[i]);
        #endif
    }

        /* save the output file name. */
    strcpy (opfname, fnames[NUM_FILES+1]);

} /* save_file_info () */




/*===========================================================================

FUNCTION    GET_FILE_INDEX

DESCRIPTION

    This function is used to return the index if the next file to be written
    into the composite image. The file start address of all the files is
    compared to find out which file will be written next.

DEPENDENCIES
    None.

RETURN VALUE
    FILE Index of the next file to be written to the composite image else

SIDE EFFECTS
    None.

===========================================================================*/

uint32 get_file_index (void)
{
    uint32 rtnVal = ERROR; /* return value. */
    uint32 i;  /* file index. */
    uint32 faddr = 0xffffffff; /* default file address. */

        /* find which input binary will be written next. */
    for (i = 0; i < NUM_FILES; i++)
    {
        if ((fstart_addr[i] <= faddr) && (write_done[i] == 0))
        {
            faddr = fstart_addr[i];
            rtnVal = i;
        }
    }

    return rtnVal;

} /* get_file_index () */

