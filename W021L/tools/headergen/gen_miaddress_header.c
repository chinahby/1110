/*=============================================================================
.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2006-2007 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/tools/headergen/gen_miaddress_header.c#2 $

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
#include "miparti.h"
#include "mibib.h"
#include "miaddress_generic.h"
#include "parse_system_partition.h"

#define  DEBUG_GENADDRESS

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/

char *sys_parti_name;
FILE *sys_parti_ptr;
char *headerfile_name;
#define MIADDRESSGENMAGIC  0xFFFFABCD

struct flash_partition_table sys_parti_input;
unsigned long qcsbl_dest_addr, oemsbl_dest_addr, amss_dest_addr;
unsigned long qcsbl_data_addr, oemsbl_data_addr, amss_data_addr;
unsigned long hostdl_dest_addr, ram_start_addr, oemsbl_stack_addr;
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

  printf( "This program parses input files to generate miaddress.h file (Make sure the sequence is correct)\n"
          "Usage %s \n"
          "         -flashtype       nand \n"
          "         -ramstartaddr    SDRAM Start Address: Chip select \n"
          "         -qcsbldestaddr   location in SDRAM for QCSBL, specific the offset from start address \n"
          "         -oemsbldestaddr  location in SDRAM for OEMSBL, specific the offset from start address\n"
          "         -oemsblstackaddr location in SDRAM for OEMSBL Stack, specific the offset from start address\n"
          "         -amssdestaddr    location in SDRAM for AMSS, specific the offset from start address  \n"
          "         -hostdldestaddr  location in SDRAM where to copy HOST Downloader, specific the offset from start address \n"
          "         -headerfilename  Header File name miaddress.h\n"
          "         -qcsblscriptname CMM script filename \n"
          "                          OR         \n\n"
          "Usage %s \n"
          "         -flashtype       nor \n"
          "         -syspartition    sys_partition.mbn \n"
          "         -ramstartaddr    SDRAM Start Address: Chip select \n"
          "         -qcsbldestaddr   if QCSBL's RO is to be copied into SDRAM, specific the offset from start address \n"
          "                          if qcsbldestaddr is provided then qcsbldataddr is ignored \n"
          "         -qcsbldataaddr   location in SDRAM for QCSBL's RW/NZI/ZI section, specific the offset from start address \n"
          "                          this will be ignored if qcsbldataaddr is provided \n"
          "         -oemsbldestaddr  if OEMSBL's RO is to be copied into SDRAM, specific the offset from start address \n"
          "                          if oemsbldestaddr is provided then oemsbldataddr is ignored \n"
          "         -oemsbldataaddr  location in SDRAM for OEMSBL's RW/NZI/ZI section, specific the offset from start address \n"
          "                          this will be ignored if oemsbldataaddr is provided \n"
          "         -oemsblstackaddr location in SDRAM for OEMSBL Stack, specific the offset from start address\n"
          "         -amssdataaddr    location in SDRAM for AMSS's RW/NZI/ZI section, specific the offset from start address \n"
          "         -hostdldestaddr  location in SDRAM where to copy HOST Downloader, specific the offset from start address \n"
          "         -headerfilename  Header File name miaddress.h\n"
          "         -qcsblscriptname CMM script filename \n",
          filename, filename);
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
  boolean foundqcsbldataaddr   = FALSE;
  boolean foundoemsbldestaddr  = FALSE;
  boolean foundoemsbldataaddr  = FALSE;
  boolean foundamssdataaddr    = FALSE;
  boolean foundamssdestaddr    = FALSE;
  boolean foundramstartaddr    = FALSE;
  boolean foundoemsblstackaddr = FALSE;
  boolean foundhostdldestaddr  = FALSE;

  qcsbl_dest_addr   = MIADDRESSGENMAGIC;
  qcsbl_data_addr   = MIADDRESSGENMAGIC;
  oemsbl_dest_addr  = MIADDRESSGENMAGIC;
  oemsbl_data_addr  = MIADDRESSGENMAGIC;
  oemsbl_stack_addr = MIADDRESSGENMAGIC;
  amss_dest_addr    = MIADDRESSGENMAGIC;
  amss_data_addr    = MIADDRESSGENMAGIC;
  hostdl_dest_addr  = MIADDRESSGENMAGIC;
  
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
    
    if (strcmp(fnames[i], "-syspartition") ==0)
    {
      sys_parti_name = fnames[i+1];
    }
    
    if ( (strcmp(fnames[i], "-ramstartaddr") ==0) && 
         (strncmp(fnames[i+1], "-", 1) !=0)
       )
    {
      sscanf(fnames[i+1], "%x", &ram_start_addr);      
      foundramstartaddr = TRUE;
    }
    
    
    if ( (strcmp(fnames[i], "-qcsbldestaddr") ==0) && 
         (strncmp(fnames[i+1], "-", 1) !=0)
       )
    {
      sscanf(fnames[i+1], "%x", &qcsbl_dest_addr);      
      foundqcsbldestaddr = TRUE;
    }
    
    if ( (strcmp(fnames[i], "-qcsbldataaddr") ==0) &&
         (strncmp(fnames[i+1], "-", 1) !=0)
       )
    {
      sscanf(fnames[i+1], "%x", &qcsbl_data_addr);            
      foundqcsbldataaddr = TRUE;
    }
    
    if ( (strcmp(fnames[i], "-oemsbldestaddr") ==0) &&
         (strncmp(fnames[i+1], "-", 1) !=0)
       )
    {
      sscanf(fnames[i+1], "%x", &oemsbl_dest_addr);            
      foundoemsbldestaddr = TRUE;
    }    

    if ( (strcmp(fnames[i], "-oemsbldataaddr") ==0) &&
         (strncmp(fnames[i+1], "-", 1) !=0)
       )
    {
      sscanf(fnames[i+1], "%x", &oemsbl_data_addr);            
      foundoemsbldataaddr = TRUE;
    }

    if ( (strcmp(fnames[i], "-oemsblstackaddr") ==0) &&
         (strncmp(fnames[i+1], "-", 1) !=0)
       )
    {
      sscanf(fnames[i+1], "%x", &oemsbl_stack_addr);            
      foundoemsblstackaddr = TRUE;
    }
    
    if ( (strcmp(fnames[i], "-amssdataaddr") ==0) &&
         (strncmp(fnames[i+1], "-", 1) !=0)
       )
    {
      sscanf(fnames[i+1], "%x", &amss_data_addr);            
      foundamssdataaddr = TRUE;
    }

    if ( (strcmp(fnames[i], "-amssdestaddr") ==0) &&
         (strncmp(fnames[i+1], "-", 1) !=0)
       )
    {
      sscanf(fnames[i+1], "%x", &amss_dest_addr);            
      foundamssdestaddr = TRUE;
    }

    if ( (strcmp(fnames[i], "-hostdldestaddr") ==0) &&
         (strncmp(fnames[i+1], "-", 1) !=0)
       )
    {
      sscanf(fnames[i+1], "%x", &hostdl_dest_addr);
      foundhostdldestaddr = TRUE;
    }
    
    if (strcmp(fnames[i], "-headerfilename") ==0)
    {
      headerfile_name = fnames[i+1];     
    }
    
  }

  if (foundramstartaddr == FALSE)
  {
    printf ("RAMSTART : SDRAM Start Address \n");
    return ERROR;
  }
  
  if ( (foundqcsbldestaddr == FALSE) && 
       (foundqcsbldataaddr == FALSE)
     )
  {
    printf ("QCSBL : Destination or Data Address needs to be provided \n");
    return ERROR;
  }

  if ( (foundqcsbldestaddr == TRUE) && 
       (foundqcsbldataaddr == TRUE)
     )
  {
    printf ("QCSBL : Cannot provide both Destination & Data Address\n");
    return ERROR;
  }
  
  if ( (foundoemsbldestaddr == FALSE) && 
       (foundoemsbldataaddr == FALSE)
     )
  {
    printf ("OEMSBL : Destination or Data Address needs to be provided \n");
    return ERROR;
  }

  if ( (foundoemsbldestaddr == TRUE) && 
       (foundoemsbldataaddr == TRUE)
     )
  {
    printf ("OEMSBL : Cannot provide both Destination & Data Address\n");
    return ERROR;
  }

  if (foundoemsblstackaddr == FALSE)
  {
    printf ("OEMSBL : Need Stack Address for OEMSBL Image \n");
    return ERROR;
  }
  
  if ( (foundamssdataaddr == FALSE) && (external_flash == 1) )
  {
    printf ("AMSS: Data Address needs to be provided for NOR flash\n");
    return ERROR;
  }

  if ( (foundamssdestaddr == FALSE) && (external_flash == 0) )
  {
    printf ("AMSS: Dest Address needs to be provided for NAND flash\n");
    return ERROR;
  }

  if (foundqcsbldestaddr == TRUE)
    qcsbl_dest_addr = ram_start_addr + qcsbl_dest_addr;

  if (foundqcsbldataaddr == TRUE)
    qcsbl_data_addr = ram_start_addr + qcsbl_data_addr;

  if (foundoemsbldestaddr == TRUE)
    oemsbl_dest_addr = ram_start_addr + oemsbl_dest_addr;

  if (foundoemsbldataaddr == TRUE)
    oemsbl_data_addr = ram_start_addr + oemsbl_data_addr;

  if (foundoemsblstackaddr == TRUE)
    oemsbl_stack_addr = ram_start_addr + oemsbl_stack_addr;

  if (foundamssdataaddr == TRUE)
    amss_data_addr = ram_start_addr + amss_data_addr;

  if (foundamssdestaddr == TRUE)
    amss_dest_addr = ram_start_addr + amss_dest_addr;
  
  if (foundhostdldestaddr == TRUE)
    hostdl_dest_addr = ram_start_addr + hostdl_dest_addr;
  
  return OK;
}


/*===========================================================================
FUNCTION  

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void write_file_heaader
(
  FILE   *opfile_ptr
)
{
  fprintf(opfile_ptr, "#ifndef _MIADDRESS_H_\n");
  fprintf(opfile_ptr, "#define _MIADDRESS_H_\n");
  fprintf(opfile_ptr, "#include \"miaddress_generic.h\"\n");
  
  fprintf(opfile_ptr, "/*===========================================================================\n\n");
  fprintf(opfile_ptr, "                         EDIT HISTORY FOR FILE\n\n");
  fprintf(opfile_ptr, "This section contains comments describing changes made to this file.\n");
  fprintf(opfile_ptr, "Notice that changes are listed in reverse chronological order.\n\n");
  fprintf(opfile_ptr, "   !!!!!!!!!This is an AUTO GENERATED FILE!!!!!!!!!!!!!! \n\n");
  fprintf(opfile_ptr, "============================================================================*/\n\n");
}

/*===========================================================================
FUNCTION  

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void write_file_body
(
  FILE   *opfile_ptr
)
{
  uint32 i;

  if (external_flash == 0) /* NAND Flash */
  {

    fprintf(opfile_ptr, "/*===========================================================================\n");
    fprintf(opfile_ptr, "  THIS MIADDRESS.H file was generated for NAND Flash so all the defines for NOR will be invalid.\n");
    fprintf(opfile_ptr, "===========================================================================*/\n");   

    fprintf(opfile_ptr, "/*===========================================================================\n");
    fprintf(opfile_ptr, "  This section is used by NAND/SDRAM.\n");
    fprintf(opfile_ptr, "  The source address is decided by partition table; We put some default value\n");
    fprintf(opfile_ptr, "  here. We expect the image_header.exe will change it\n");
    fprintf(opfile_ptr, "===========================================================================*/\n");
    
    fprintf(opfile_ptr, "#define MI_NAND_QCSBL_IMG_DEST_ADDR                  0x%x \n", qcsbl_dest_addr);
    fprintf(opfile_ptr, "#define MI_NAND_OEM_SBL_IMG_DEST_ADDR                0x%x \n", oemsbl_dest_addr);
    fprintf(opfile_ptr, "#define MI_NAND_OEM_SBL_IMG_STACK_ADDR               0x%x \n", oemsbl_stack_addr);
    fprintf(opfile_ptr, "#define MI_NAND_AMSS_IMG_DEST_ADDR                   0x%x \n", amss_dest_addr);
    fprintf(opfile_ptr, "#define MI_NAND_HOSTDL_IMG_DEST_ADDR                 0x%x \n", hostdl_dest_addr);

    fprintf(opfile_ptr, "/*===========================================================================\n");
    fprintf(opfile_ptr, "  The following section is used by NOR/(SDRAM/PSRAM)\n");
    fprintf(opfile_ptr, "===========================================================================*/\n");

    fprintf(opfile_ptr, "/* QCSBL in NOR, QCSBL is 0xFFFFFFFF bytes*/ \n");
    fprintf(opfile_ptr, "#define MI_NOR_QCSBL_HEADER_CFG_DATA_SRC_ADDR        0xFFFFFFFF \n");
    fprintf(opfile_ptr, "#define MI_NOR_QCSBL_IMG_SRC_ADDR                    0xFFFFFFFF /* NOR, CS0   */\n");
    fprintf(opfile_ptr, "#define MI_NOR_QCSBL_IMG_DEST_ADDR                   0xFFFFFFFF /* NOR, CS0   */\n");

    fprintf(opfile_ptr, "/* OEMSBL in NOR, OEMSBL is 256K bytes,each header is 40 bytes */\n");
    fprintf(opfile_ptr, "#define MI_NOR_OEM_SBL_HEADER_SRC_ADDR               0xFFFFFFFF /* NOR, CS0*/\n");
    fprintf(opfile_ptr, "#define MI_NOR_OEM_SBL_IMG_SRC_ADDR                  0xFFFFFFFF /* NOR, CS0 */\n");
    fprintf(opfile_ptr, "#define MI_NOR_OEM_SBL_IMG_DEST_ADDR                 0xFFFFFFFF /* NOR, CS0 */\n");
    fprintf(opfile_ptr, "#define MI_NOR_OEM_SBL_IMG_STACK_ADDR                0xFFFFFFFF /* NOR, CS0 */\n");
    
    fprintf(opfile_ptr, "#define MI_NOR_AMSS_IMG_BOOT_SRC_ADDR                0xFFFFFFFF\n");
    fprintf(opfile_ptr, "#define MI_NOR_AMSS_IMG_SRC_ADDR                     0xFFFFFFFF\n");
    fprintf(opfile_ptr, "#define MI_NOR_AMSS_IMG_DEST_ADDR                    0xFFFFFFFF\n");

    fprintf(opfile_ptr, "#define MI_NOR_AMSS_BOOT_ELF_HEADER_SRC_ADDR         0xFFFFFFFF /* NOR, CS0 */\n");
    fprintf(opfile_ptr, "#define MI_NOR_AMSS_BOOT_PROGRAM_HEADER_SRC_ADDR     0xFFFFFFFF /* NOR, CS0 */\n");
    fprintf(opfile_ptr, "#define MI_NOR_AMSS_HASH_START_ADDR                  0xFFFFFFFF/* NOR, CS0 */ \n");
    fprintf(opfile_ptr, "#define MI_NOR_AMSS_HEADER_SRC_ADDR                  0xFFFFFFFF /* header is 40 bytes */\n");    

    fprintf(opfile_ptr, "#define MI_NOR_AMSS_MIPIB_LOCATION_ADDR              0xFFFFFFFF\n");
    fprintf(opfile_ptr, "#define MI_NOR_HOSTDL_IMG_DEST_ADDR                  0xFFFFFFFF\n");
  }
  else 
  {

    fprintf(opfile_ptr, "/*===========================================================================\n");
    fprintf(opfile_ptr, "  THIS MIADDRESS.H file was generated for NOR Flash so all the defines for NAND will be invalid.\n");
    fprintf(opfile_ptr, "===========================================================================*/\n");   

    fprintf(opfile_ptr, "/*===========================================================================\n");
    fprintf(opfile_ptr, "  This section is used by NAND/SDRAM.\n");
    fprintf(opfile_ptr, "  The source address is decided by partition table; We put some default value\n");
    fprintf(opfile_ptr, "  here. We expect the image_header.exe will change it\n");
    fprintf(opfile_ptr, "===========================================================================*/\n");
    
    fprintf(opfile_ptr, "#define MI_NAND_QCSBL_IMG_DEST_ADDR                  0xFFFFFFFF \n");
    fprintf(opfile_ptr, "#define MI_NAND_OEM_SBL_IMG_DEST_ADDR                0xFFFFFFFF \n");
    fprintf(opfile_ptr, "#define MI_NAND_OEM_SBL_IMG_STACK_ADDR               0xFFFFFFFF \n");
    fprintf(opfile_ptr, "#define MI_NAND_AMSS_IMG_DEST_ADDR                   0xFFFFFFFF \n");
    fprintf(opfile_ptr, "#define MI_NAND_HOSTDL_IMG_DEST_ADDR                 0xFFFFFFFF \n");
    
    fprintf(opfile_ptr, "/*===========================================================================\n");
    fprintf(opfile_ptr, "  The following section is used by NOR/(SDRAM/PSRAM)\n");
    fprintf(opfile_ptr, "===========================================================================*/\n");

    for (i=0; i<sys_parti_input.numparts; i++)
    {
      if (strcmp(sys_parti_input.part_entry[i].name, MIBIB_QCSBL_PARTI_NAME) ==0)
      {
        fprintf(opfile_ptr, "/* QCSBL in NOR, QCSBL is 0x%x bytes*/ \n", sys_parti_input.part_entry[i].length);
        fprintf(opfile_ptr, "#define MI_NOR_QCSBL_HEADER_CFG_DATA_SRC_ADDR        0x%x\n", 
                             (sys_parti_input.part_entry[i].offset+MI_NOR_QCSBL_HEADER_OFFSET_FROM_START_ADDR)
             );
        fprintf(opfile_ptr, "#define MI_NOR_QCSBL_IMG_SRC_ADDR                    0x%x /* NOR, CS0   */\n", 
                 (sys_parti_input.part_entry[i].offset+MI_NOR_QCSBL_IMG_SRC_OFFSET_FROM_START_ADDR)
               );
        if (qcsbl_dest_addr == 0xFFFFABCD)
        {
          fprintf(opfile_ptr, "#define MI_NOR_QCSBL_IMG_DEST_ADDR                   0x%x /* NOR, CS0   */\n", 
                   (sys_parti_input.part_entry[i].offset+MI_NOR_QCSBL_IMG_SRC_OFFSET_FROM_START_ADDR)
                 );
          qcsbl_dest_addr = (sys_parti_input.part_entry[i].offset+MI_NOR_QCSBL_IMG_SRC_OFFSET_FROM_START_ADDR);
          fprintf(opfile_ptr, "#define MI_NOR_QCSBL_IMG_DATA_ADDR                   0x%x /* SDRAM/PSRAM   */\n", 
                  qcsbl_data_addr
                 );
        }
        else
        {
          fprintf(opfile_ptr, "#define MI_NOR_QCSBL_IMG_DEST_ADDR                   0x%x /* SDRAM/PSRAM   */\n", 
                  qcsbl_dest_addr
                 );
          fprintf(opfile_ptr, "#define MI_NOR_QCSBL_IMG_DATA_ADDR                   +0x0 /* SDRAM/PSRAM   */\n");
        }

      }
      else if (strcmp(sys_parti_input.part_entry[i].name, MIBIB_OEMSBL_PARTI_NAME) ==0)
      {
        fprintf(opfile_ptr, "/* OEMSBL in NOR, OEMSBL is 256K bytes,each header is 40 bytes */\n");
        fprintf(opfile_ptr, "#define MI_NOR_OEM_SBL_HEADER_SRC_ADDR               0x%x /* NOR, CS0*/\n", 
                            (sys_parti_input.part_entry[i].offset + MI_NOR_OEM_SBL_HEADER_OFFSET_FROM_START_ADDR)
               );
        fprintf(opfile_ptr, "#define MI_NOR_OEM_SBL_IMG_SRC_ADDR                  0x%x /* NOR, CS0 */\n",
                (sys_parti_input.part_entry[i].offset + MI_NOR_OEM_SBL_IMG_SRC_OFFSET_FROM_START_ADDR)
               );
        if (oemsbl_dest_addr == 0xFFFFABCD)
        {
          fprintf(opfile_ptr, "#define MI_NOR_OEM_SBL_IMG_DEST_ADDR                 0x%x /* NOR, CS0 */\n",
                  (sys_parti_input.part_entry[i].offset + MI_NOR_OEM_SBL_IMG_SRC_OFFSET_FROM_START_ADDR)
                 );
          fprintf(opfile_ptr, "#define MI_NOR_OEM_SBL_IMG_DATA_ADDR                 0x%x /* SDRAM/PSRAM   */\n", 
                  oemsbl_data_addr
                 );
        }
        else
        {
          fprintf(opfile_ptr, "#define MI_NOR_OEM_SBL_IMG_DEST_ADDR                 0x%x /* SDRAM/PSRAM   */\n",
                  oemsbl_dest_addr
                 );
          fprintf(opfile_ptr, "#define MI_NOR_OEM_SBL_IMG_DATA_ADDR                 +0x0 /* SDRAM/PSRAM   */\n");
        }

        fprintf(opfile_ptr, "#define MI_NOR_OEM_SBL_IMG_STACK_ADDR                0x%x \n", oemsbl_stack_addr);

      }
      else if (strcmp(sys_parti_input.part_entry[i].name, MIBIB_NOR_BOOT_PARTI_NAME) ==0)
      {
        fprintf(opfile_ptr, "#define MI_NOR_AMSS_IMG_BOOT_SRC_ADDR                0x%x\n",(sys_parti_input.part_entry[i].offset));
        fprintf(opfile_ptr, "#define MI_NOR_AMSS_IMG_SRC_ADDR                     0x%x\n",(sys_parti_input.part_entry[i].offset));
        fprintf(opfile_ptr, "#define MI_NOR_AMSS_IMG_DEST_ADDR                    0x%x\n",(sys_parti_input.part_entry[i].offset));
      }
      else if (strcmp(sys_parti_input.part_entry[i].name, MIBIB_AMSS_PARTI_NAME) ==0)
      {
        fprintf(opfile_ptr, "/* ELF-Program Header and AMSS Image */\n");
        fprintf(opfile_ptr, "#define MI_NOR_AMSS_BOOT_ELF_HEADER_SRC_ADDR         0x%x /* NOR, CS0 */\n",(sys_parti_input.part_entry[i].offset));
        fprintf(opfile_ptr, "#define MI_NOR_AMSS_BOOT_PROGRAM_HEADER_SRC_ADDR     0x%x /* NOR, CS0 */\n",
                (sys_parti_input.part_entry[i].offset+MI_NOR_AMSS_BOOT_ELF_HEADER_HASH_TBL_LENGTH)
               );
        fprintf(opfile_ptr, "#define MI_NOR_AMSS_HASH_START_ADDR                  0x%x /* NOR, CS0 */ \n",
                (sys_parti_input.part_entry[i].offset+MI_NOR_AMSS_BOOT_ELF_HEADER_HASH_TBL_LENGTH+MI_NOR_AMSS_BOOT_PROGRAM_HEADER_LENGTH)
               );
        fprintf(opfile_ptr, "#define MI_NOR_AMSS_IMG_DATA_ADDR                    0x%x /* SDRAM/PSRAM */\n",amss_data_addr);
        fprintf(opfile_ptr, "/* ARMPRG download location in SDRAM */\n");
        fprintf(opfile_ptr, "#define MI_NOR_HOSTDL_IMG_DEST_ADDR                  0x%x /* SDRAM/PSRAM */\n",hostdl_dest_addr);
        fprintf(opfile_ptr, "#define MI_NOR_AMSS_HEADER_SRC_ADDR                  0xFFFFFFFF /* header is 40 bytes */\n");
        
      }    
      else if (strcmp(sys_parti_input.part_entry[i].name, MIBIB_NOR_PARTI_INFO_PARTI_NAME) ==0)
      {
        fprintf(opfile_ptr, "/* Partition Table Location in NOR */\n");
        fprintf(opfile_ptr, "#define MI_NOR_AMSS_MIPIB_LOCATION_ADDR              0x%x\n",(sys_parti_input.part_entry[i].offset));
      }
    
    } 

  }

}

/*===========================================================================
FUNCTION  

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void write_file_end
(
  FILE   *opfile_ptr
)
{
  fprintf(opfile_ptr, "#endif /* _MIADDRESS_H_ */");
  fprintf(opfile_ptr, "\n\n");
}


/*===========================================================================
FUNCTION  GENERATE_HEADERFILE

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
uint32 generate_headerfile()
{
  FILE *opfile_ptr;
  
  if (file_open (headerfile_name, "wb", &opfile_ptr) == ERROR)
  {
    return ERROR;
  }

  write_file_heaader(opfile_ptr);
  write_file_body(opfile_ptr);
  write_file_end(opfile_ptr);
  /* close the output file. */
  fclose (opfile_ptr);
  
  return OK;
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
  
  if (generate_headerfile() == ERROR)
  {
    printf("Fail to generate header file: %s\n", headerfile_name);
    return ERROR;
  }
  
  return 0;  

}
