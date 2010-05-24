/*=============================================================================
             P R O G R E S S I V E   B O O T  A D D  H A S H

  This program adds hash header and hash table to the ELF file at given
  addresses.

  Usage: pboot_add_hash [-d] elf_file hash_hdr hash_table output_elf
      -d            - debug mode
      elf_file      - the ELF file
      hash_hdr      - hash header generated for hash_table
      hash_table    - hash table (signed or not)
      output_elf    - final output

Copyright (c) 2005 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/
/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/tools/headergen/pboot_add_hash.c#3 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/16/07   tkarra  Removed a bug where uninit hash table size is compared hash table
                   space allocated.
09/22/06   rjain   Removed a bug in print_usage function where the argument to
                   be printed was not being passed to printf.
08/31/06   rjain   Modified for Simple elf support
08/08/05   ih      Created
=============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include "pboot_elf_utils.h"
#include "miimageheader.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/

bool debug_print = 0;            /* Global debug option */

uint32_t boot_img_hdr_size = 0;  /* Size of boot_img_hdr */

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
   printf("This program inserts hash header and hash table into the ELF file\n"
          "at the specified addresses. The addresses can be found in miheader.h\n\n"
          "Usage: %s [-d] elf_file hash_hdr hash_table output_elf\n\n"
          "       -d            - debug mode\n"
          "       elf_file      - the ELF file\n"
          "       hash_hdr      - hash header generated for hash_table\n"
          "       hash_table    - hash table (signed or unsigned)\n"
          "       output_elf    - final output\n", filename);
}


/*===========================================================================
FUNCTION  ADD_HASH_TO_ELF

DESCRIPTION
    This function modifies the Hash Header to point to Hash table,
    signature_ptr and cert_chain_ptr

DEPENDENCIES
    None.

RETURN VALUE
    errno on error
    else 0

SIDE EFFECTS

===========================================================================*/

int write_modified_hashhd
(
  mi_boot_image_header_type *hash_header,
  uint32_t addr_hash_tbl,
  FILE *out_elf,
  uint32_t phdr_offset
)
{
  int size, ret;
  int32_t hash_cert_chain_end = 0;

  hash_header->image_dest_ptr = (uint8 *) addr_hash_tbl;

  /* This will be true only for Secured Boot */
  if(hash_header->signature_size != 0)
  {
    /* The hash header, hash signature and cert_chain are to be placed
    ** right after the ELF header to support >126 of segments.
    **  -------------------------------------------------------
    ** | ELF_HDR | HASH_HDR | HASH_SIGNATURE | HASH_CERT_CHAIN |
    **  -------------------------------------------------------
    */
    hash_header->signature_ptr = (uint8*) (ELF_HDR_SIZE + boot_img_hdr_size);
    hash_header->cert_chain_ptr = (uint8*) (hash_header->signature_ptr +
                                            hash_header->signature_size);
  }

  /* Make sure that hash signature and certificate chain do not overwrite the program headers */
  hash_cert_chain_end = (uint32_t) hash_header->cert_chain_ptr + hash_header->cert_chain_size;
  if (hash_cert_chain_end >= phdr_offset)
  {
    printf("Cannot proceed - Hash cert_chain will overwrite the program headers!\n");
    DEBUG("Hash cert_chain end offset [%x], Program header start offset [%x]",
           hash_cert_chain_end, phdr_offset, 0);
    return errno;
  }
  ret = fseek(out_elf, ELF_HDR_SIZE, SEEK_SET);
  if (ret)
  {
    printf("Cannot fseek. ret(%d) - errno(%d)\n", ret, errno);
    return errno;
  }

  size = fwrite(hash_header, boot_img_hdr_size, 1, out_elf);
  if (size != 1)
  {
    DEBUG("Couldn't Write Hash Header %d ret(%d) - errno(%d) size\n", size, ret, errno);
    return errno;
  }

  return 0;
}


/*===========================================================================
FUNCTION  ADD_HASH_TO_ELF

DESCRIPTION
    This function adds hash_hdr and hash_tbl to the elf file

DEPENDENCIES
    None.

RETURN VALUE
    errno on error
    else 0

SIDE EFFECTS

===========================================================================*/
int add_hash_to_elf(FILE *src_elf,FILE *dest_elf, FILE *hash_hdr, FILE *hash_tbl )
{
  int size, ret;

  struct stat fs;
  struct elf_file_info src_file;
  struct elf_file_info dest_file;

  size_t src_elf_size = 0;
  size_t hash_tbl_size = 0;
  size_t hash_hdr_size = 0;

  uint32_t index = 0;
  uint32_t phdr_size = 0;
  uint32_t phdr_offset = 0;
  uint32_t phdr_entry_size = 0;
  uint32_t phdr_num_segment = 0;

  uint32_t hash_p_addr = 0;
  uint32_t hash_offset = 0;
  uint32_t src_hash_index = 0;
  uint32_t hash_phdr_offset = 0;
  uint32_t hash_alloc_size = 0;
  uint32_t hash_sig_cert_chain_size = 0;
  uint32_t src_hash_sig_cert_chain_offset = 0;
  uint32_t dest_hash_sig_cert_chain_offset = 0;


  mi_boot_image_header_type hash_header;

  src_file.fd = fileno(src_elf);
  dest_file.fd = fileno(dest_elf);

  ret = fseek(src_elf, 0, SEEK_SET);
  if (ret)
  {
    printf("Cannot fseek. ret(%d) - errno(%d)\n", ret, errno);
    return errno;
  }

  /* Read in ELF header from the source file */
  size = fread(&src_file.elf_hdr, ELF_HDR_SIZE, 1, src_elf);
  if (size != 1)
  {
    printf("Couldn't read elf header\n");
    return errno;
  }

  /* Verify the ELF header */
  ret = verify_elf_header(&src_file.elf_hdr);
  if (ret)
  {
    printf("ELF header fails verification\n");
    return ERROR;
  }

  /* Get src_elf size */
  if(fstat(fileno(src_elf), &fs))
  {
    printf("Unable to stat src_elf file!\n");
    return errno;
  }
  src_elf_size = fs.st_size;
  DEBUG("\n\nInput file size in bytes: [%d]\n", src_elf_size, 0, 0);

  /* Get hash header size */
  if(fstat(fileno(hash_hdr), &fs))
  {
    printf("Unable to stat hash header file!\n");
    return errno;
  }
  hash_hdr_size = fs.st_size;
  DEBUG("Hash header size in bytes: [%d]\n", hash_hdr_size, 0, 0);

  /* Get hash table size */
  if(fstat(fileno(hash_tbl), &fs))
  {
    printf("Unable to stat hash table file!\n");
    return errno;
  }
  hash_tbl_size = fs.st_size;
  DEBUG("Hash table size in bytes: [%d]\n", hash_tbl_size, 0, 0);

  /* Get program header location from ELF header in src_elf */
  phdr_offset = src_file.elf_hdr.e_phoff;
  DEBUG("Found program header at offset [0x%x] in source file\n", phdr_offset, 0, 0);

  /* Calculate program header size */
  phdr_entry_size = src_file.elf_hdr.e_phentsize;
  phdr_num_segment = src_file.elf_hdr.e_phnum;
  phdr_size = phdr_entry_size * phdr_num_segment;
  DEBUG("Total program header size in bytes: [%d]\n", phdr_size, 0, 0);

  /* Seek to program header location in src_elf */
  ret = fseek(src_elf, phdr_offset, SEEK_SET);
  if (ret)
  {
   printf("Cannot fseek program header in src_elf. ret(%d) - errno(%d)\n", ret, errno);
   return errno;
  }


  if (phdr_size > UINT32_MAX)
  {
    printf("phdr_size size is grater than the limit UINT32_MAX \n");
    return errno;    
  }
  /* Read in the program header table from src_elf */
  if(fread((char *)src_file.phdr_table, phdr_size, 1, src_elf) != 1)
  {
    printf("Unable to read the program header table from src_elf !\n");
    return errno;
  }

  DEBUG("Looking for HASH segment with flags [0x%x]\n", MI_PBT_ELF_HASH_SEGMENT, 0, 0);
  /* Scan the program header table for hash segment */
  while(index < phdr_num_segment)
  {
    /* Found hash segment? */
    if(MI_PBT_SEGMENT_TYPE_VALUE(src_file.phdr_table[index].p_flags) == MI_PBT_HASH_SEGMENT)
    {
      src_hash_index = index;
      /* These addresses are equal for src_elf and dest_elf */
      hash_p_addr = src_file.phdr_table[index].p_paddr;
      hash_offset = src_file.phdr_table[index].p_offset;
      hash_phdr_offset = phdr_offset + (phdr_entry_size * src_hash_index);
      hash_alloc_size = (src_file.phdr_table[index+1].p_offset - src_file.phdr_table[index].p_offset);
      DEBUG("Found hash segment at offset [0x%x] and program header index %d in the src_elf",
             hash_offset, src_hash_index, 0);

      break;
    }
    /* NO? check next segment */
    index++;

    /* Are we done scanning all the program headers */
    if (index == phdr_num_segment)
    {
      DEBUG("Scanned %d segments\n", index, 0, 0);
      printf("Unable to find the hash table in the input file !\n");
      return errno;
    }
  }

  /* Copy the entire src_elf to dest_elf first */
  if(fcpy(src_elf, 0, dest_elf, 0, src_elf_size))
  {
    printf("Uncable to copy src_elf to dest_elf! ret(%d) - errno(%d)\n", ret, errno);
    return errno;
  }

  /* Read in the hash header */
  ret = fseek(hash_hdr, 0, SEEK_SET);
  if (ret)
  {
    printf("Cannot fseek. ret(%d) - errno(%d)\n", ret, errno);
    return errno;
  }

  size = fread(&hash_header, boot_img_hdr_size, 1, hash_hdr);
  if (size != 1)
  {
    printf("Couldn't Read Hash Header\n");
    return errno;
  }
  DEBUG("Read in %d bytes of hash header\n", 0, 0, 0);

   /* Make sure that the hash table will not overwrite any other segment */
	if (hash_header.code_size > hash_alloc_size)
	{
	  printf("Hash table size [%d] exceeds the space allocated for hash segment [%d].\n",
			  hash_header.image_size, hash_alloc_size);
	  DEBUG("Hash segment offset: [%d] -- Next segment offset: [%d]\n", src_file.phdr_table[index].p_offset,
			 src_file.phdr_table[index+1].p_offset, 0);
	  return errno;
	}

  /* Check if the actual hash table size matches with the one in hash header */
  if (hash_tbl_size != hash_header.image_size)
  {
    printf("Hash table file size [%d] does not match that in hash header [%d] \n",
           hash_tbl_size, hash_header.image_size);
    return errno;
  }
  DEBUG("Hash table file size matches that in hash header\n", 0, 0, 0);

  
  if ( hash_header.signature_size > UINT32_MAX)
  {
	  printf("hash_header.image_size is greater than the limit \n");
	  return errno;
  }
  /* Write the hash header to destination elf */
  if(write_modified_hashhd(&hash_header, hash_p_addr, dest_elf, phdr_offset))
  {
    printf("Unable to copy hash header!\n");
    return ret;
  }

  /* Now write hash signature and cert chain to dest_elf */
  hash_sig_cert_chain_size = hash_header.signature_size + hash_header.cert_chain_size;
  src_hash_sig_cert_chain_offset = hash_header.code_size;
  dest_hash_sig_cert_chain_offset = (uint32_t)hash_header.signature_ptr;
  DEBUG("Hash signature offset in input hash table file [0x%x]\n",
        src_hash_sig_cert_chain_offset, 0, 0);
  DEBUG("Hash signature offset in output elf [0x%x]\n",
        dest_hash_sig_cert_chain_offset, 0, 0);

  if(fcpy(hash_tbl, src_hash_sig_cert_chain_offset, dest_elf,
          dest_hash_sig_cert_chain_offset, hash_sig_cert_chain_size))
  {
    printf("Unable to write hash signature and cert_chain to dest_elf! ret(%d) - errno(%d)\n",
            ret, errno);
    return errno;
  }
  DEBUG("Written %d bytes of hash signature and certificate to dest_elf\n",
        hash_sig_cert_chain_size, 0, 0);

  /* Now write actual hash table to dest_elf */

  if (hash_header.image_size > UINT32_MAX)
  {
	  printf("hash_header.image_size is greater than the limit \n");
	  return errno;
  }
  
  if(fcpy(hash_tbl, 0, dest_elf, hash_offset, hash_header.image_size))
  {
    printf("Unable to write hash table to dest_elf! ret(%d) - errno(%d)\n", ret, errno);
    return errno;
  }
  DEBUG("Written %d bytes of hash table to dest_elf\n",
        hash_header.image_size, 0, 0);

  /* Update hash program header with the actual file_sz of hash table */
  src_file.phdr_table[index].p_filesz = hash_header.code_size;

  /* Now update hash program header in dest_elf */
  DEBUG("Updating hash table size in program header to [0x%x]\n\n", hash_header.code_size, 0, 0);

  /* Seek to the hash header position in dest-elf */
  ret = fseek(dest_elf, hash_phdr_offset, SEEK_SET);
  if (ret)
  {
    printf("Cannot fseek to program header for hash segment in dest_elf. ret(%d) - errno(%d)\n",
           ret, errno);
    return errno;
  }

  if (phdr_entry_size > UINT32_MAX)
	{
	  printf("phdr_entry_size is greater than the UINT32_MAX \n");
	  return errno;
	}


  if(fwrite((char *) &src_file.phdr_table[index], phdr_entry_size, 1, dest_elf) != 1)
  {
    printf("Write ELF header failed! ret(%d) - errno(%d)\n", ret, errno);
    return errno;
  }

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
  /* argv[1] = input/output elf
  ** argv[2] = input hash header
  ** argv[3] = input hash table
  ** argv[4] = output elf
  */

  int i=0, ret;
  char *elf_file;
  char *hash_tbl_file;
  char *hash_hdr_file;
  char *dest_elf_file;

  FILE *elf;
  FILE *hash_tbl;
  FILE *hash_hdr;
  FILE *dest_elf;

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
  dest_elf_file = argv[4+i];

  /* Open files */
  OPENFILE(dest_elf_file, dest_elf, "wb+");
  OPENFILE(elf_file, elf, "rb");
  OPENFILE(hash_hdr_file, hash_hdr, "rb");
  OPENFILE(hash_tbl_file, hash_tbl, "rb");

  if(debug_print == 1)
  {
    DEBUG("\n  Input File \n",0,0,0);
    DEBUG("\n ============ \n\n",0,0,0);
    ret = read_elf_file(elf);
    if(ret == ERROR)
    {
     printf("Read Elf file Failed \n");
     exit(-1);
    }
  }

  /* Add header and hash table to ELF */
  boot_img_hdr_size = sizeof(mi_boot_image_header_type);

  ret = add_hash_to_elf(elf, dest_elf, hash_hdr, hash_tbl);
  if(ret)
  {
    if(ret != ERROR) perror("add_hash_to_elf failed");
    exit(-1);
  }

  if(debug_print == 1)
  {
    DEBUG("\n  Output File \n",0,0,0);
    DEBUG("\n ============= \n\n",0,0,0);
    read_elf_file(dest_elf);
    if(ret)
    {
     exit(-1);
    }
  }

  fclose(elf);
  fclose(dest_elf);
  fclose(hash_tbl);
  fclose(hash_hdr);
  exit(0);
}
