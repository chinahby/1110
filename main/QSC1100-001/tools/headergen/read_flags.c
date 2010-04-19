/*=============================================================================
   PROGRESSIVE  BOOT ELF PROGRAM HEADER  FLAGS  MODIFICATION  TOOL

   This program Prints the flags of the Program Header's of an ELf file.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2005, 2006 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/tools/pboot/read_flags.c#5 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
08/31/06   rjain   Ported from 6800b for Simple elf support
08/01/06   tk      Created

=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include "pboot_elf_utils.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/

bool debug_print = 0;            /* global debug option */

/*=============================================================================

                              FUNCTION DEFINITIONS

=============================================================================*/

/*===========================================================================
FUNCTION  PRINT_USAGE

DESCRIPTION
    This function prints out the program usage

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

void print_usage(char *filename)
{
  printf("This program Prints the Program header flags of an ELF file \n "
         "Usage: %s [-d] elf_file \n"
         "      elf_file     - input ELF file \n");
}


/*===========================================================================

FUNCTION    print_flags

DESCRIPTION
    This program Prints the flags of the Program Header's of an ELf file.

DEPENDENCIES
    None.

RETURN VALUE
    !=0  Error
    0    Success

SIDE EFFECTS
    None.

===========================================================================*/
int print_flags
(
 FILE *elf_file,   /* input elf file.  */
 segments_t *segs /* segment information */
)
{
  struct elf_file_info src_file;

  int i, size;
  Elf32_Phdr *i_hdr;
  unsigned int ret;
  fpos_t curr_pos;

  src_file.fd = fileno(elf_file);

  /*
   ** Verify the ELF header
   */

  size = fread((char*) &(src_file.elf_hdr), ELF_HDR_SIZE,1,elf_file);
  if (size != 1)
  {
    printf("Couldn't read elf header\n");
    return errno;
  }


  if (src_file.elf_hdr.e_phentsize > UINT16_MAX)
  {
	printf("src_file.elf_hdr.e_phentsize is grater than limit\n");
    return ERROR;
  }

  ret = verify_elf_header(&src_file.elf_hdr);
  if (ret)
  {
    printf("ELF header fails verification\n");
    return ERROR;
  }

  /*----------------------------------------------------------------------
   ** Go to the start of the program header table in input file
   ----------------------------------------------------------------------*/
  if (fseek(elf_file, src_file.elf_hdr.e_phoff, SEEK_SET))
  {
    printf("Couldn't seek input file - errno(%d).\n", errno);
    return errno;
  }


  /*----------------------------------------------------------------------
   * What to do here?
   * 1) Change the p_type of the Program Header to the one Interpreted
        by the Scatter Loader file
   *----------------------------------------------------------------------*/
  for(i=1; i<=src_file.elf_hdr.e_phnum ; i++)
  {
    i_hdr = &src_file.phdr_table[i];


    if(fgetpos(elf_file,&curr_pos))
    {
      printf("fgetpos failed\n");
      return errno;
     }

    /* Get the segment description */
    if((fread((char*) i_hdr, src_file.elf_hdr.e_phentsize,1,elf_file)) != 1)
    {
      printf("Unable to read program header!\n");
      return errno;
    }


    DEBUG("Segment[%d]: offset[%x] vaddr[%x]", i, i_hdr->p_offset, i_hdr->p_vaddr);
	DEBUG(" type[%x] size[%x] memsz[%x]\n", i_hdr->p_flags, i_hdr->p_filesz, i_hdr->p_memsz);

  }

  return 0;
}


/*===========================================================================

FUNCTION    MAIN

DESCRIPTION
    Main program function

    Usage: print_flags in_elf

DEPENDENCIES
    None.

RETURN VALUE
    !=0  Error
    0    Success

SIDE EFFECTS
    None.

===========================================================================*/
int main(int argc, char **argv)
{
  /* argv[1] = input elf
   */
  segments_t segment_info = {0};          /* table containing segment info */

  char *in_elf_file;
  FILE *in_elf;


    in_elf_file = argv[1];


    OPENFILE(in_elf_file, in_elf, "rb");
    if(print_flags(in_elf, &segment_info)){
         perror("Error Print_flags");
         exit(-1);
    }
    fclose(in_elf);
    return 0;
}

