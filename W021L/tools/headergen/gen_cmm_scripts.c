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

  $Header: //depot/asic/msmshared/tools/headergen/gen_cmm_scripts.c#7 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
12/08/06   anb     Created

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
#include "filetool.h"
#include "mibib.h"
#include "pboot_elf_utils.h"
#include "parse_system_partition.h"
#include "miaddress_generic.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/

bool debug_print = 0;            /* global debug option */
#undef DBUG_CMM

char *sys_parti_name;
FILE *sys_parti_ptr;

char *qcsbl_file_name;
char *in_elf_file_name;
char *amss_entry_point_file_name;

#define MIADDRESSGENMAGIC  0xFFFFABCD

struct flash_partition_table sys_parti_input;
unsigned long qcsbl_dest_addr;
unsigned long ram_start_addr;
/* 0 for NAND. 1 for NOR flash */
uint32 external_flash;

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
  printf( "This program parses the ELF file to determine the ENTRY POINT \n "
          "Usage %s \n"
          "         -flashtype                 nand/nor \n"
          "         -syspartition              sys_partition.mbn \n"
          "         -elffile                   input ELF file \n"
          "         -ramstartaddr              SDRAM Start Address: Chip select \n"
          "         -qcsbldestaddr             if QCSBL's RO is to be copied into SDRAM, specific the offset from start address \n"
          "         -qcsblscriptname           CMM script filename \n"
          "         -amssentrypointscriptname  AMSS enrty point script filename\n",  filename);
}

/*===========================================================================
FUNCTION  SAVE_INPUT_INFO

DESCRIPTION
  This function will parse to the input parameters to determine the information
    
DEPENDENCIES
  None.

RETURN VALUE
  ERROR    if file_cpy or file_append_const returns an ERROR else
  OK

SIDE EFFECTS
  None.
===========================================================================*/
uint32 save_input_info
(
  uint32 num_arg,
  char   *fnames[]  /* input file name. */
)
{
  uint32  i;
  boolean foundqcsbldestaddr   = FALSE;
  qcsbl_dest_addr   = MIADDRESSGENMAGIC;
  
  for ( i=0; i<num_arg; i++ )
  {  
    printf ("argv[%d]:%s \n", i, fnames[i]);
    
    if (strcmp(fnames[i], "-flashtype") ==0)
    {
      if (strcmp(fnames[i+1], "nand") ==0)
        external_flash    = 0;
      else if (strcmp(fnames[i+1], "nor") ==0)
        external_flash    = 1;
      else
        return ERROR;
    } 
    
    if (  (strcmp(fnames[i], "-syspartition") ==0) && 
          (strncmp(fnames[i+1], "-", 1) !=0)
       )
    {
      sys_parti_name = fnames[i+1];
    }
    
    if ( (strcmp(fnames[i], "-ramstartaddr") ==0) && 
         (strncmp(fnames[i+1], "-", 1) !=0)
       )
    {
      sscanf(fnames[i+1], "%x", &ram_start_addr);      
    }
    
    
    if ( (strcmp(fnames[i], "-qcsbldestaddr") ==0) && 
         (strncmp(fnames[i+1], "-", 1) !=0)
       )
    {
      sscanf(fnames[i+1], "%x", &qcsbl_dest_addr);      
      foundqcsbldestaddr = TRUE;
    }
    
    if (strcmp(fnames[i], "-elffile") ==0)
    {
      in_elf_file_name = fnames[i+1];     
    }
    
    if (strcmp(fnames[i], "-qcsblscriptname") ==0)
    {
      qcsbl_file_name = fnames[i+1];     
    }

    if (strcmp(fnames[i], "-amssentrypointscriptname") ==0)
    {
      amss_entry_point_file_name = fnames[i+1];     
    }
    
  }

  if (foundqcsbldestaddr == TRUE)
    qcsbl_dest_addr = ram_start_addr + qcsbl_dest_addr;
  
  return OK;
}

/*=========================================================================

FUNCTION  ELF_SEGMENT_FILTER_

DESCRIPTION
  This is the default filter.

DEPENDENCIES
  None
  
RETURN VALUE
  TRUE if the segment flags indicate the segment is non-paged, not the
  hash segment, not shared and not unused, otherwise FALSE

SIDE EFFECTS
  None

===========================================================================*/
boolean elf_segment_filter
(
  uint32 flags   /* Segment flags field in program header */
)
{
  return ( ( MI_PBT_PAGE_MODE_VALUE(flags) == MI_PBT_NON_PAGED_SEGMENT ) &&
           ( MI_PBT_SEGMENT_TYPE_VALUE(flags) != MI_PBT_HASH_SEGMENT ) &&
           ( MI_PBT_ACCESS_TYPE_VALUE(flags) != MI_PBT_NOTUSED_SEGMENT ) &&
           ( MI_PBT_ACCESS_TYPE_VALUE(flags) != MI_PBT_SHARED_SEGMENT ) 
		 );
} /* elf_segment_filter */

/*===========================================================================

FUNCTION   READ_ELF_INFO

DESCRIPTION
  This program parses the Program Header's of an ELf file to determine the
  entry point of the image.

DEPENDENCIES
    None.

RETURN VALUE
    !=0  Error
    0    Success

SIDE EFFECTS
    None.

===========================================================================*/
int read_elf_info
(
 FILE *elf_file,
 uint32 *entrypoint
)
{
  struct elf_file_info src_file;

  int i, size;
  Elf32_Phdr *i_hdr;
  unsigned int ret;
  fpos_t curr_pos;
  uint32 phdr_entry_size = 0;
  uint32 phdr_num_segment = 0;
  uint32 phdr_size = 0;
  uint32 phdr_offset = 0;
  boolean success = FALSE;

  src_file.fd = fileno(elf_file);

  /* Verify the ELF header */
  size = fread((char*) &(src_file.elf_hdr), ELF_HDR_SIZE,1,elf_file);
  if (size != 1)
  {
    printf("Couldn't read elf header\n");
    return errno;
  }

  ret = verify_elf_header(&src_file.elf_hdr);
  if (ret)
  {
    printf("ELF header fails verification\n");
    return ERROR;
  }

  #ifdef DBUG_CMM
  printf("Entry Point into AMSS :0x%x\n", src_file.elf_hdr.e_entry);
  #endif
  *entrypoint = src_file.elf_hdr.e_entry;


  phdr_offset = src_file.elf_hdr.e_phoff;
  #ifdef DBUG_CMM
  printf("Found program header at offset [0x%x] in source file\n", phdr_offset, 0, 0);
  #endif

  /* Calculate program header size */
  phdr_entry_size  = src_file.elf_hdr.e_phentsize;
  phdr_num_segment = src_file.elf_hdr.e_phnum;
  phdr_size        = phdr_entry_size * phdr_num_segment;

  if(phdr_size > UINT16_MAX)
  {
	printf("phdr_size is grater than UINT16_MAX (64K) \n");
	return ERROR;
  }
  
  #ifdef DBUG_CMM
  printf("Total program header size in bytes: [%d]\n", phdr_size, 0, 0);
  #endif

  /* Seek to program header location in src_elf */
  ret = fseek(elf_file, phdr_offset, SEEK_SET);
  if (ret)
  {
     printf("Cannot lseek program header in SRC_ELF. ret(%d)", ret);
     return ERROR;
  }

  /* Read in ELF header from the source file */
  size = fread((char*) &(src_file.phdr_table), phdr_size,1,elf_file);
  if (size != 1)
  {
    printf("Couldn't read program header\n");
    return ERROR;
  }

  for (i = 0; i < phdr_num_segment; i++)
  {
    if (elf_segment_filter(src_file.phdr_table[i].p_flags)) 
    {
      if ( *entrypoint == src_file.phdr_table[i].p_vaddr )
      {
        success = TRUE;
        *entrypoint = src_file.phdr_table[i].p_paddr;
        break;
      }
    }
  }

  #ifdef DBUG_CMM
  /* diaplay all the program headers. */
  for (i = 0; i < phdr_num_segment; i++)
  {
    printf ("\n Current Segment[%d] ", i);
    
    if (MI_PBT_PAGE_MODE_VALUE(src_file.phdr_table[i].p_flags)== MI_PBT_NON_PAGED_SEGMENT)
      printf (" NOT_PAGED ");
    else
      printf (" PAGED     ");

    if (MI_PBT_SEGMENT_TYPE_VALUE(src_file.phdr_table[i].p_flags) == MI_PBT_L4_SEGMENT)
      printf (" L4 Segment     ");    
    else if (MI_PBT_SEGMENT_TYPE_VALUE(src_file.phdr_table[i].p_flags) == MI_PBT_AMSS_SEGMENT)
      printf (" AMSS Segment   ");    
    else if (MI_PBT_SEGMENT_TYPE_VALUE(src_file.phdr_table[i].p_flags) == MI_PBT_HASH_SEGMENT)
      printf (" HASH Segment   ");    
    else if (MI_PBT_SEGMENT_TYPE_VALUE(src_file.phdr_table[i].p_flags) == MI_PBT_BOOT_SEGMENT)
      printf (" BOOT Segment   ");    

    if (MI_PBT_ACCESS_TYPE_VALUE(src_file.phdr_table[i].p_flags)  == MI_PBT_RW_SEGMENT)
      printf (" RW Segment   ");       
    else if (MI_PBT_ACCESS_TYPE_VALUE(src_file.phdr_table[i].p_flags)  == MI_PBT_RO_SEGMENT)
      printf (" RO Segment   ");       
    else if (MI_PBT_ACCESS_TYPE_VALUE(src_file.phdr_table[i].p_flags)  == MI_PBT_ZI_SEGMENT)
      printf (" ZI Segment   ");       
    else if (MI_PBT_ACCESS_TYPE_VALUE(src_file.phdr_table[i].p_flags)  == MI_PBT_NOTUSED_SEGMENT)
      printf (" NU Segment   ");       
    else if (MI_PBT_ACCESS_TYPE_VALUE(src_file.phdr_table[i].p_flags)  == MI_PBT_SHARED_SEGMENT)
      printf (" SH Segment   ");          
  }
  #endif
  
  if (success == TRUE)
  return 0;
  else
    return ERROR;
}

/*===========================================================================
FUNCTION  GENERATE_QCSBL_CMMFILE

DESCRIPTION
  This function will generate CMM Scripts

DEPENDENCIES
  None.

RETURN VALUE
  !=0  Error
  0    Success

SIDE EFFECTS
  None.

===========================================================================*/
uint32 generate_qcsbl_cmmfile()
{
  uint32 i;
  FILE *opfile_ptr;
  
  if (file_open (qcsbl_file_name, "wb", &opfile_ptr) == ERROR)
  {
    return ERROR;
  }

  fprintf(opfile_ptr, ";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n");
  fprintf(opfile_ptr, "; This is an auto generated file\n");
  fprintf(opfile_ptr, ";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n");

  if (external_flash == 0) /* NAND Flash */
  {
    fprintf(opfile_ptr, "global &QCSBL_NONSEC_START_ADDR\n");
    fprintf(opfile_ptr, "&QCSBL_NONSEC_START_ADDR=0x%x\n", qcsbl_dest_addr);
    fprintf(opfile_ptr, "global &QCSBL_SEC_START_ADDR\n");
    fprintf(opfile_ptr, "&QCSBL_SEC_START_ADDR=0x%x\n", (qcsbl_dest_addr+0x28));  
  }
  else
  {
    for (i=0; i<sys_parti_input.numparts; i++)
    {
      if (strcmp(sys_parti_input.part_entry[i].name, MIBIB_QCSBL_PARTI_NAME) ==0)
      {
        if (qcsbl_dest_addr == 0xFFFFABCD)
        {
          qcsbl_dest_addr = (sys_parti_input.part_entry[i].offset+MI_NOR_QCSBL_IMG_SRC_OFFSET_FROM_START_ADDR);
        }
      }  
    }  
    fprintf(opfile_ptr, "global &QCSBL_NONSEC_START_ADDR\n");
    fprintf(opfile_ptr, "&QCSBL_NONSEC_START_ADDR=0x%x\n", qcsbl_dest_addr);
    fprintf(opfile_ptr, "global &QCSBL_SEC_START_ADDR\n");
    fprintf(opfile_ptr, "&QCSBL_SEC_START_ADDR=0x%x\n", (qcsbl_dest_addr+0x28));  
  }

  fclose (opfile_ptr);  
  return OK;
}


/*===========================================================================
FUNCTION  GENERATE_AMSS_ENTRY_POINT_CMMFILE

DESCRIPTION
  This function will generate CMM Scripts

DEPENDENCIES
  None.

RETURN VALUE
  !=0  Error
  0    Success

SIDE EFFECTS
  None.

===========================================================================*/
uint32 generate_amss_entry_point_cmmfile()
{
  FILE *in_elf_file_ptr;
  FILE *opfile_ptr;  
  uint32 entry_point;

  if (file_open (in_elf_file_name, "rb", &in_elf_file_ptr) == ERROR)
  {
    return ERROR;
  }

  if (read_elf_info(in_elf_file_ptr, &entry_point ) == ERROR)
  {
    /* close the output file. */
    fclose (in_elf_file_ptr);
    return ERROR;
  }

  /* close the output file. */
  fclose (in_elf_file_ptr);
  
  if (file_open (amss_entry_point_file_name, "wb", &opfile_ptr) == ERROR)
  {
    return ERROR;
  }

  fprintf(opfile_ptr, ";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n");
  fprintf(opfile_ptr, "; This is an auto generated file\n");
  fprintf(opfile_ptr, ";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n");
  fprintf(opfile_ptr, "global &AMSS_ENTRY_POINT_ADDR\n");
  fprintf(opfile_ptr, "&AMSS_ENTRY_POINT_ADDR=0x%x\n", entry_point);
  fprintf(opfile_ptr, "global &RAM_START_ADDR\n");
  fprintf(opfile_ptr, "&RAM_START_ADDR=0x%x\n", ram_start_addr);
  fprintf(opfile_ptr, "global &RAM_END_ADDR\n");
  fprintf(opfile_ptr, "&RAM_END_ADDR=0x%x\n", (ram_start_addr+0x07FFFFFF));

  /* close the output file. */
  fclose (opfile_ptr);

  return OK;
}

/*===========================================================================

FUNCTION    MAIN

DESCRIPTION
    Main program function

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

  /* save passed arguments. */
  if (save_input_info (argc, argv) == ERROR)
  {
    print_usage(argv[0]);
    return ERROR;
  }

  if (external_flash == 1)
  {
    if ( parse_system_partition_layout ( sys_parti_name,
                                         sys_parti_ptr,
                                         &sys_parti_input
                                       ) == ERROR)
    {
      printf("Fail to read and parse Partition Input File: %s\n", sys_parti_name);
      return ERROR;
    }
  }

  if (generate_qcsbl_cmmfile()== ERROR)
  {
    printf("Fail to generate cmm script: %s\n", qcsbl_file_name);
    return ERROR;
  }

  if (generate_amss_entry_point_cmmfile()== ERROR)
  {
    printf("Fail to generate cmm script: %s\n", amss_entry_point_file_name);
    return ERROR;
  }
  
  return 0;  

}

