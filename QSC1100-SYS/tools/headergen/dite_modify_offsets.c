/*=============================================================================
             PROGRESSIVE BOOT DITE MODIFY OFFSETS

  This program runs dite.exe second time to modify the program header offsets
  in Boot info segment. We need to adjust the offsets because we add the hash segment
  later in pboot_add_hash.exe. Tool reserves space in the Elf file for addtion of
  Hash Segment and corrects the file offsets (NAND address) both in Main Elf Header
  and in the Boot info segment.

Copyright (c) 2005 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/
/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/tools/pboot/dite_modify_offsets.c#1 $

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
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
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
    Prints usage to screen

DEPENDENCIES
    None.

RETURN VALUE
    errno on error
    else 0

SIDE EFFECTS

===========================================================================*/
void print_usage(char *filename)
{
   printf("This program runs dite.exe second time to modify the program header offsets\n"
          "in Boot info segment. We need to adjust the offsets because we add the hash segment\n"
          "later in pboot_add_hash.exe. Tool reserves space in the Elf file for addtion of\n"
          "Hash Segment and corrects the file offsets (NAND address) both in Main Elf Header\n"
          "and in the Boot info segment.\n"
          "Usage: %s [-d] elf_file hash_hdr hash_table build_id\n"
          "     -d            - debug mode\n"
          "     elf_file      - the Input ELF file\n"
          "     hash_hdr      - hash header generated for hash_table\n"
          "     hash_table    - hash table (signed or not)\n"
          "     build_id    - Build Id\n");
}



/*===========================================================================
FUNCTION  RUN_DITE_UPDATE_BOOT_SEGMENT

DESCRIPTION
    This function Runs dite.exe second time to update the Boot info segment with correct offsets.

DEPENDENCIES
    None.

RETURN VALUE
    errno on error
    else 0

SIDE EFFECTS

===========================================================================*/
int run_dite_update_boot_segment(FILE *in_elf,char *build_id, FILE *hash_hdr, FILE *hash_tbl )
{
  struct stat fs;
  struct elf_file_info in_out_file;
  int ret,size;

  char command[1000];

  size_t hash_tbl_size, hash_hdr_size;
  uint32_t file_offset, phdr_size,first_seg_offset;
  uint32_t elf_plus_prog_hdrs_end_addr = 0;
  uint32_t offset_from_hdr = 0;

  Elf32_Phdr *i_hdr;
  Elf32_Ehdr *elf_hdr;


  in_out_file.fd = fileno(in_elf);

  elf_hdr = &in_out_file.elf_hdr;

  ret = fseek(in_elf, 0, SEEK_SET);
  if (ret)
  {
	printf("Cannot fseek. ret(%d) - errno(%d)\n", ret, errno);
	return errno;
  }

  /*** Verify the ELF header */
  size = fread(&in_out_file.elf_hdr, ELF_HDR_SIZE, 1, in_elf);
  if (size != 1)
  {
	ret = ferror(in_elf);
    printf("Couldn't read elf header %d size %d ret\n",size,ret);
    return errno;
  }

  ret = verify_elf_header((Elf32_Ehdr *)elf_hdr);
  if (ret)
  {
    printf("ELF header fails verification\n");
    return ERROR;
  }

  phdr_size = (uint16)elf_hdr->e_phentsize;


  /*----------------------------------------------------------------------
	 Read the first Program Header from the Input File
  ----------------------------------------------------------------------*/

  i_hdr = &in_out_file.phdr_table[1];
  file_offset = ELF_HDR_SIZE;

  ret = fseek(in_elf, file_offset , SEEK_SET);
  if (ret)
  {
	printf("Cannot fseek. ret(%d) - errno(%d)\n", ret, errno);
	return errno;
  }

  /* Get the segment description */
  if(fread((char *)i_hdr, phdr_size, 1, in_elf) != 1)
  {
	  printf("Unable to read the First program header from Input File !\n");
	  return errno;
  }

 /* Get hash header and table file sizes */
  if(fstat(fileno(hash_hdr), &fs))
  {
    printf("Unable to stat hash header file!\n");
    return errno;
  }
  hash_hdr_size = fs.st_size;

  if(fstat(fileno(hash_tbl), &fs))
  {
    printf("Unable to stat hash table file!\n");
    return errno;
  }
  hash_tbl_size = fs.st_size;

  /* We have to take the addtional Hash Segment into account */

  offset_from_hdr = offset_align((sizeof(Elf32_Ehdr) + (phdr_size * (elf_hdr->e_phnum + 1))),ELF_BLOCK_ALIGN);

  first_seg_offset = offset_align((hash_tbl_size + hash_hdr_size + offset_from_hdr), ELF_BLOCK_ALIGN );

  DEBUG("First segment offset is %x",first_seg_offset,0,0);

  sprintf(command,"cd ../../platform;./dite.bat %s 0x%x ; cd ../build/ms", build_id, first_seg_offset);
  system(command);

 return 0;
}

/*===========================================================================
FUNCTION  MAIN

DESCRIPTION
    Main program function

DEPENDENCIES
    None.

RETURN VALUE
    !=0 on error
    else 0

SIDE EFFECTS

===========================================================================*/
int main(int argc, char **argv)
{
  /* argv[1] = input elf
   * argv[2] = input hash header
   * argv[3] = input hash table
   * argv[4] = build_id
   */
  char *elf_file, *hash_tbl_file, *hash_hdr_file, *build_id;
  //char build_id[100];
  FILE *elf1, *hash_tbl, *hash_hdr;
  int i=0, ret;
  struct stat fs;

  if(argc == 6 && strcmp(argv[1], "-d") == 0)
  {
    /* enable debug mode */
    debug_print = true;
    /* shift argv by 1 */
    i=1;
  }
  else if(argc != 5)
  {
    print_usage(argv[0]);
    exit(-1);
  }

  /* Save arguments */
  elf_file      = argv[1+i];
  hash_hdr_file = argv[2+i];
  hash_tbl_file = argv[3+i];
  build_id = argv[4+i];


  /* Open files */
  OPENFILE(elf_file, elf1, "rb");
  OPENFILE(hash_hdr_file, hash_hdr, "rb");
  OPENFILE(hash_tbl_file, hash_tbl, "rb");


  /* Run Dite to Update the Boot Info Segment */
  ret = run_dite_update_boot_segment(elf1, build_id , hash_hdr, hash_tbl);
  if(ret)
  {
    if(ret != ERROR) perror("Run Dite to Update Boot Info Segment Failed");
    exit(-1);
  }

  fclose(elf1);
  fclose(hash_tbl);
  fclose(hash_hdr);
  exit(0);
}
