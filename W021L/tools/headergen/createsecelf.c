/*=============================================================================

                        SECURE ELF Tool

GENERAL DESCRIPTION

    This file contains code to implement the creatsesecelf tool.
    Our createsecelf tool is used to create the ELF file for secure
    progressive boot. Since, we need to generate security certificates
    for different binaries the creation of this elf file is divided into
    two parts.
    
    In the first step,
    
        We creates a new elf file in a format that is required by our
        progressive boot architecture.
        Read the amss scl file to find
            which segments are pageable,non-pageable or not-used.
            the starting addresses of every segment in our amss scl file.
        Update the program header based on the information obatined from the
        amss scl file.
        Ensure that every segment is a multiple of PAGE_SIZE. 
        Create hash table and create our hash file.

    In the second step,
    
        We create our output elf file by writing all the input 
        input binaries and the input elf file to our output elf file.
        The address where the input binaries are written is obtained from
        miheader.h
    
        Once the elf file is generated we do file comparison between our
        output elf file and the input binaries to ensure that the elf file
        created contains valid data.
    

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
    None.

Copyright (c) 2005 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  $
  $Header: //depot/asic/msmshared/tools/headergen/createsecelf.c#4 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/09/05   chs     created.


=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/

#include "filetool.h"
#include "pboot_melf.h"
#include "pboot_genhash.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

#define NUM_FILES       2UL
#define CONST_VAL       0x0UL   /* constant appended to unused
                                ** sections of elf file.
                                */
#define MAX_SEGMENTS    100
#define HASH_TABLE_SIZE 0x40000 /* 256K */
#define PAGE_SIZE       0x1000  /* 4K   */


/* Strings used to detect if a segment is pageable,
** not-pageable or not-used.
*/
#define PAGEABLE        "PAGED"
#define NOTPAGEABLE     "NOTPAGED"
#define RESERVED        "RESERVED"
#define HASHTBL         "HASH"


/* Debug Flags.*/
#undef DBUG_CREATESECELF
#undef DBUG_PARSE_SCL_FILE
#undef DBUG_PART1
#undef DBUG_PART2
#undef MSM6275_COMPILE

/* enum describing segment type. */
enum flagtype
{
  PAGED = 0,
  NOTPAGED,
  NOTUSED,
  HASH
};


static char   ipfname[NUM_FILES][80];     /* input file names. */
static uint32 fsize[NUM_FILES];           /* input file size.  */
static uint32 fstart_addr[NUM_FILES];     /* start addresses of the input
                                          ** files in our output file.
                                          */
static char   opfname[2][80];             /* output file name. */

static enum flagtype paged[MAX_SEGMENTS]; /* segment flag indicator. */
static uint8  hash_table[HASH_TABLE_SIZE];  /* hash table buffer. */
static uint32 num_segments; /* number of segments in our scl file. */
static char flash_type[10]; /* type of flash being used. */


/*---------------------------------------------------------------------------
  
---------------------------------------------------------------------------*/



/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/


static uint32 parse_scl_file (char *);
static uint32 is_pageable (char *);
static uint32 str_cmp (char *, char *);
static uint32 create_hash_file (char *, uint8 *);
static uint32 generate_hash_certs (char *, char *);
static uint32 generate_hash_table (char *, uint8 *);
static uint32 update_elf_phdr (char *, char *);
static uint32 createsecelf_part1 (uint32, char **);
static uint32 createsecelf_part2 (uint32, char **);
static uint32  save_file_info (char **, char *);
static uint32 create_elf_img (void);
static uint32 verify_elf_image (void);
static uint32 check_file_errors (void);


/*===========================================================================

FUNCTION    MAIN

DESCRIPTION

    Our createsecelf tool is used to create the ELF file for secure
    progressive boot. Since, we need to generate security certificates
    for different binaries the creation of this elf file is divided into
    two parts.
    
    In the first step,
    
        We creates a new elf file in a format that is required by our
        progressive boot architecture.
        Read the amss scl file to find
            which segments are pageable,non-pageable or not-used.
            the starting addresses of every segment in our amss scl file.
        Update the program header based on the information obatined from the
        amss scl file.
        Ensure that every segment is a multiple of PAGE_SIZE. 
        Create hash table and create our hash file.

    In the second step,
    
        We create our output elf file by writing all the input 
        input binaries and the input elf file to our output elf file.
        The address where the input binaries are written is obtained from
        miheader.h
    
        Once the elf file is generated we do file comparison between our
        output elf file and the input binaries to ensure that the elf file
        created contains valid data.
    

DEPENDENCIES
    None.

RETURN VALUE
    ERROR   if invalid arguments or
            if createsecelf_part1 or createsecelf_part2 return and ERROR else
    OK

SIDE EFFECTS
    None.

===========================================================================*/

int main
(
  uint32 argc,    /* number of arguments passed. */
  char *argv[]    /* arguments passed. */
)
{
  uint32 rtnval = OK; /* return value. */

  /* argument checks. */
  if (argc == 6) 
  {
    rtnval = createsecelf_part1 (--argc, ++argv);
  }
  else if (argc == 7)
  {
    rtnval = createsecelf_part2 (--argc, ++argv);
  }
  else
  {
    printf ("USAGE: createsecelf elf_file elf_new amss_scl_file");
    printf (" hash_file\n");
    printf("OR\n");
    printf ("USAGE: createsecelf elf_file elf_new amss_hashhd ");
    printf (" amsshd amss_hash\n");
    rtnval = ERROR;
  }

  return rtnval;
} /* main () */


/*===========================================================================

FUNCTION    CREATESECELF_PART2

DESCRIPTION

    Our createsecelf tool is divided into two parts. This function is used
    to implement step two in our createsecelf tool. This function expects
    ten arguments as input. The first argument is our input elf file, second
    is our output elf file, the next two are the qcsbl header and config data
    binary and the qcsbl binary. The fifth and sixth arguments are the
    oemsblhd and oemsbl binary. The next three arguments are the amss hash
    file header, amss exception segment header and amss application header.
    The last argument is the hash file.
    
    
    This function will create our output elf file by writing all the
    input binaries and the input elf file to our output elf file.
    The address where the input binaries are written is obtained from
    miheader.h
    
    Once the elf file is generated we do file comparison between our output
    elf file and the input binaries to ensure that the elf file created 
    contains valid data.

DEPENDENCIES
    None.

RETURN VALUE
    ERROR   if invalid arguments or
            if check_file_errors, create_Elf_image or verify_elf_image
            returns ERROR else
    OK

SIDE EFFECTS
    None.

===========================================================================*/


uint32 createsecelf_part2
(
  uint32 argc, /* number of arguments passed. */
  char *argv[] /* arguments passed. */
)
{
  /* argument checks. */
  if ((argv == NULL) || (argc <= 0))
  {
    return ERROR;
  }

  /* save passed arguments. */
  if (save_file_info (argv, argv[argc-1]) == ERROR)
  {
    return ERROR;
  }

  /* check for file errors. */
  if (check_file_errors () == ERROR)
  {
    return ERROR;
  }

  /* last argument contains the flash type. */
  strncpy (flash_type, argv[argc-1], 5);

  /* create the elf image. */
  printf("Generating ELF image %s\n", opfname[1]);
  if (create_elf_img () == ERROR)
  {
    return ERROR;
  }

  /* check the elf image and verify its contents. */
  printf("Verifying ELF image %s\n", opfname[1]);
  if (verify_elf_image () == ERROR)
  {
    return ERROR;
  }

  printf("ELF image %s was generated and verified successfully.\n",
         opfname[1]);

  #ifdef DBUG_PART2
  printf("\nProgram Header contents:\n");
  if (pboot_dispall_phdr (opfname[1]) == ERROR)
  {
    return ERROR;
  }
  #endif
        
  return OK;

} /* createsecelf_part2 () */


/*===========================================================================
FUNCTION  CREATE_ELF_IMG

DESCRIPTION
    This function will create our ELF image by combining the input
    images passed as arguments.
    
DEPENDENCIES
    None.

RETURN VALUE
    ERROR    if file_cpy or file_append_const returns an ERROR else
    OK
    
SIDE EFFECTS
    Creates the composite image.

===========================================================================*/

uint32 create_elf_img (void)
{
  uint32 i; /* file index. */
  uint32 opfsize = 0;//, opfsize = 0;/* input and output file size. */
  FILE *ipfile_ptr, *opfile_ptr;  /* file pointers. */
  elf_prog_header ephdr;      /* elf header. */
  enum flagtype segtype = NOTUSED;   /* all the input binaries are set to NOTUSED */

  /* read the elf and program header from our input elf file. */
  if (pboot_read_ephdr (opfname[0], &ephdr) == ERROR)
  {
    return ERROR;
  }
  /* open the elf file in write mode. */
  if (file_open (opfname[1], WRITE_MODE, &opfile_ptr) == ERROR)
  {
    return ERROR;
  }
  /* open the input elf file in read mode. */
  if (file_open (opfname[0], READ_MODE, &ipfile_ptr) == ERROR)
  {
    fclose (opfile_ptr);
    return ERROR;
  }
  /* copy the elf header and program header to our elf file. */
  if (file_cpy (opfile_ptr, ipfile_ptr, PAGE_SIZE)== ERROR)
  {
    fclose (ipfile_ptr);
    fclose (opfile_ptr);
    return ERROR;
  }

  /* copy all the segments from our input elf file to our output
  ** elf file.
  */
  for (i = 0; i < ephdr.elf_hdr.e_phnum; i++)
  {
    /* seek to the correct location. */
    if (fseek (ipfile_ptr, ephdr.phdr_table[i].p_offset, SEEK_SET)
        == ERROR)
    {
      fclose (ipfile_ptr);
      fclose (opfile_ptr);
      return ERROR;
    }
     
    if (fseek (opfile_ptr, ephdr.phdr_table[i].p_offset, SEEK_SET)
        == ERROR)
    {
      fclose (ipfile_ptr);
      fclose (opfile_ptr);
      return ERROR;
    }
    /* copy the segment into our output elf file. */
    if (file_cpy (opfile_ptr, ipfile_ptr, ephdr.phdr_table[i].p_filesz)
        == ERROR)
    {
      fclose (ipfile_ptr);
      fclose (opfile_ptr);
      return ERROR;
    }
  }
  /* close the input elf file pointer. */
  fclose (ipfile_ptr);

  /* goto the correct location. */
  if (fseek (opfile_ptr, (ephdr.phdr_table[0].p_offset +
             ephdr.phdr_table[0].p_filesz), SEEK_SET) == ERROR)
  {
    fclose (opfile_ptr);
    return ERROR;
  }
  opfsize = ftell (opfile_ptr);

  /* write all the input binaries to our elf file. */
  for (i = 0; i < NUM_FILES; i++)
  {
    /* get the file size. */
//  file_size (ipfname[i], &ipfsize);

    /* open the input file in read mode. */
    if (file_open (ipfname[i], READ_MODE, &ipfile_ptr) == ERROR)
    {
      fclose (opfile_ptr);
      return ERROR;
    }
    /* fill unused space in our ELF image with CONST_VAL. */ 
    if (opfsize != fstart_addr[i])
    {
      #ifdef DBUG_CREATESECELF
      printf("Address 0x%x to 0x%x will be filled with 0xff.\n",
              opfsize, (fstart_addr[i] - 1));
      #endif
            
      if (file_append_const (opfile_ptr, CONST_VAL,
                             (fstart_addr[i] - opfsize)) == ERROR)
      {
        fclose (ipfile_ptr);
        fclose (opfile_ptr);
        return ERROR;
      }
    }

    #ifdef DBUG_CREATESECELF
    printf ("Writing image %s to our elf image.\n", ipfname[i]);
    #endif
    /* copy the input binary to our elf image. */
    if (file_cpy (opfile_ptr, ipfile_ptr, fsize[i]) == ERROR)
    {
      fclose (ipfile_ptr);
      fclose (opfile_ptr);
      return ERROR;
    }
    /* calculate file size. */
    opfsize = ftell (opfile_ptr);

    /* close the input file. */
    fclose (ipfile_ptr);

    if (i == 0)
    {
      /* updated the elf and program header. */
      ephdr.phdr_table[ephdr.elf_hdr.e_phnum].p_filesz =  fsize[0] + fsize[1];
      ephdr.phdr_table[ephdr.elf_hdr.e_phnum].p_flags = HASH;
      ephdr.phdr_table[ephdr.elf_hdr.e_phnum].p_offset = fstart_addr[i];
      ephdr.phdr_table[ephdr.elf_hdr.e_phnum].p_vaddr =
      fstart_addr[i] - PAGE_SIZE;
      ephdr.elf_hdr.e_phnum++;
    }
  }

  /* fill unused space in our ELF image with CONST_VAL. */ 
  if (opfsize != ephdr.phdr_table[1].p_offset)
  {
    #ifdef DBUG_CREATESECELF
    printf("Address 0x%x to 0x%x will be filled with 0xff.\n",
            opfsize, MI_NOR_AMSS_IMG_APP_SRC_ADDR);
    #endif
        
    if (file_append_const (opfile_ptr, CONST_VAL,
                           (ephdr.phdr_table[1].p_offset - opfsize))
        == ERROR)
    {
      fclose (opfile_ptr);
      return ERROR;
    }
  }
  /* close the output elf file pointer. */
  fclose (opfile_ptr);

  /* update the elf header and the program headers in our elf file */
  if (pboot_update_ephdr (opfname[1], &ephdr) == ERROR)
  {
    return ERROR;
  }

  return OK;

} /* create_elf_img () */


/*===========================================================================
FUNCTION    VERIFY_ELF_IMAGE

DESCRIPTION

    This function is used to check if the composite image file contains valid
    data.

    Each input binary file has a fixed start address, which is obtained from
    the macros defined in miheader.h. The start address and image size of the
    input binary files is used to ensure that the input files were written
    at the correct location in the composite image file. Also, the contents
    of the composite image is checked with the input files to ensure it
    contains valid data.

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

uint32 verify_elf_image (void)
{
  FILE *opfile_ptr; /* file pointer to our elf image. */
  FILE *ipfile_ptr; /* file pointers to our input binaries. */
  int  i;  /* index. */
  uint32 addr = 0; /* current file address. */
  elf_prog_header ephdr[2]; /* elf and program header. */
  uint32 ipfsize;  /* file size. */

  /* read the elf and program header from our input elf file. */
  if (pboot_read_ephdr (opfname[0], &ephdr[0]) == ERROR)
  {
    return ERROR;
  }
  /* read the elf and program header from our output elf file. */
  if (pboot_read_ephdr (opfname[1], &ephdr[1]) == ERROR)
  {
    return ERROR;
  }
  /* open the output elf file in read mode. */
  if (file_open (opfname[1], READ_MODE, &opfile_ptr) == ERROR)
  {
    return ERROR;
  }
  /* open the input elf file in read mode. */
  if (file_open (opfname[0], READ_MODE, &ipfile_ptr) == ERROR)
  {
    fclose (opfile_ptr);
    return ERROR;
  }

  /* verify all the segments common in our input and output elf file. */
  for (i = 0; i < ephdr[0].elf_hdr.e_phnum; i++)
  {
    /* seek to the correct location. */
    if (fseek (ipfile_ptr, ephdr[0].phdr_table[i].p_offset, SEEK_SET)
        == ERROR)
    {
      fclose (ipfile_ptr);
      fclose (opfile_ptr);
      return ERROR;
    }
        
    if (fseek (opfile_ptr, ephdr[1].phdr_table[i].p_offset, SEEK_SET)
        == ERROR)
    {
      fclose (ipfile_ptr);
      fclose (opfile_ptr);
      return ERROR;
    }
    /* compare the two segments. */
    if (filen_cmp (opfile_ptr, ipfile_ptr,
                   ephdr[1].phdr_table[i].p_filesz) != 0)
    {
      fclose (ipfile_ptr);
      fclose (opfile_ptr);
      return ERROR;
    }
  }
  /* close the input elf file pointer. */
  fclose (ipfile_ptr);

  /* verify if the input files were copied correctly. */
  for (i = ephdr[0].elf_hdr.e_phnum; i < ephdr[1].elf_hdr.e_phnum; i++)
  {
    /* seek to the correct location. */
    if (fseek (opfile_ptr, ephdr[1].phdr_table[i].p_offset, SEEK_SET)
        == ERROR)
    {
      fclose (opfile_ptr);
      return ERROR;
    }
    /* get the file size. */
    file_size (ipfname[i - ephdr[0].elf_hdr.e_phnum], &ipfsize);

    /* open the input file in read mode. */
    if (file_open (ipfname[i - ephdr[0].elf_hdr.e_phnum], READ_MODE,
                   &ipfile_ptr) == ERROR)
    {
      fclose (opfile_ptr);
      return ERROR;
    }
    /* compare the two segments. */
    if (filen_cmp (opfile_ptr, ipfile_ptr, ipfsize) != 0)
    {
      fclose (ipfile_ptr);
      fclose (opfile_ptr);
      return ERROR;
    }
    /* close the input file. */
    fclose (ipfile_ptr);
  }

  /* close the output file. */
  fclose (opfile_ptr);

  return OK;

} /* verify_elf_image () */



/*===========================================================================

FUNCTION    CREATESECELF_PART1

DESCRIPTION

    This function does the following:
    
        Creates a new elf file in a format that is required by our progressive
        boot architecture.
        Reads the amss scl file to find
            which segments are pageable,non-pageable or not-used.
            the starting addresses of every segment in our amss scl file.
        Update the program header based on the information obatined from the
        amss scl file.
        Ensure that every segment is a multiple of PAGE_SIZE. 
        Create hash table and create our hash file.

DEPENDENCIES
    None.

RETURN VALUE
    ERROR   if pboot_melf (), parse_scl_file (), update_elf_phdr (),
            generate_hash_table () or create_hash_file () returns ERROR else
    OK

SIDE EFFECTS
    None.

===========================================================================*/


uint32 createsecelf_part1
(
  uint32 argc, /* number of arguments passed. */
  char *argv[] /* arguments passed. */
)
{
  char temp_fname[] = "../temp.elf"; /* file generate by pboot_melf () */

  /* argument checks. */
  if ((argv == NULL) || (argc <= 0))
  {
    return ERROR;
  }
  /* generate our new elf file. */
  printf("Generating ELF image %s\n", argv[1]);
  if (pboot_melf (argv[0], temp_fname) == ERROR)
  {
    return ERROR;
  }
  /* read the amss scl file and find which segments are pageable,
  ** non-pageable or reserved. It will also determine the start address
  ** for every segment in our amss scl file.
  */
  if (parse_scl_file (argv[2]) == ERROR)
  {
    return ERROR;
  }

  /* last argument contains the flash type. */
  strncpy (flash_type, argv[argc-1], 5);

  /* update the program header in our elf file. */
  if (update_elf_phdr (temp_fname, argv[1]) == ERROR)
  {
    return ERROR;
  }
  /* generate hash table. */
  printf("Generating Hash Table\n");
  if (generate_hash_table (argv[1], hash_table) == ERROR)
  {
    return ERROR;
  }
  /* create hash file. */
  printf("Generating Hash File %s\n", argv[3]);
  if (create_hash_file (argv[3], hash_table) == ERROR)
  {
    return ERROR;
  }

  #ifdef DBUG_PART1
  printf("\nProgram Header contents:\n");
  if (pboot_dispall_phdr (argv[1]) == ERROR)
  {
    return ERROR;
  }
  #endif
        
  return OK;

} /* createsecelf_part1 () */

/*===========================================================================

FUNCTION    GENERATE_HASH_FILE

DESCRIPTION

    This function is used to generate our hash table.
    
    We generate hash for every segment. The number of hashes generated 
    depends on the program flag for every segment. If a segment is paged
    we'll generate hash for every 4K of the segment, if its not-paged
    we generate a single hash for the entire segment, if its not-used
    we'll ignore the segment.

DEPENDENCIES
    None.

RETURN VALUE
    ERROR   if invalid arguments or
            if pboot_read_ephdr (), file_open () or fseek () or file_read () or
            secprogboot_generate_hashes () returns ERROR
            or if malloc returns NULL else
    OK

SIDE EFFECTS
    None.

===========================================================================*/


uint32 generate_hash_table
(
    char  *filename, /* elf file name. */
    uint8 *hash_buff /* hash table. */
)
{
    elf_prog_header ephdr;  /* elf and program header. */
    uint32 i;          /* index. */
    uint32 num_hashes; /* number of hashes. */
    FILE   *file_ptr;  /* file pointer. */
    uint8  *file_buff; /* file buffer.  */
    uint32 seg_offset; /* calculates virtual address offset. */
    uint32 seg_size;   /* segment size. */

        /* argument checks. */
    if ((filename == NULL) && (hash_buff == NULL))
    {
        return ERROR;
    }
        /* read the elf and program header. */
    if (pboot_read_ephdr (filename, &ephdr) == ERROR)
    {
        return ERROR;
    }
        /*  open the elf file in read mode. */
    if (file_open (filename, READ_MODE, &file_ptr) == ERROR)
    {
        return ERROR;
    }

        /* generate hash for every segment. The number of hashes generated 
        ** depends on the program flasg for every segment. If a segment is
        ** paged we'll generate hash for every 4K of the segment, if its
        ** not-paged we generate a single hash for the entire segment else
        ** we'll ignore the segment. 
        */
    for (i = 0; i < ephdr.elf_hdr.e_phnum; i++)
    {
        num_hashes = 1;

            /* check the program flag. The number of hashes to generate
            ** depends on whether a segment is pageable or not-pageable.
            */
        if (ephdr.phdr_table[i].p_flags == NOTUSED)
        {
            continue;
        }
        else if (ephdr.phdr_table[i].p_flags == PAGED)
        {
                /* generate hash for every PAGE */
            num_hashes = ephdr.phdr_table[i].p_filesz/PAGE_SIZE;
        }

        /* Make sure that the hash is calculated such that every segments
        ** virtual address is 4K alligned.
        */
        seg_offset = ephdr.phdr_table[i].p_vaddr & (PAGE_SIZE - 1);
        seg_size   = ephdr.phdr_table[i].p_filesz;

            /* allocate memory based on segment size. */
        if ((file_buff = (uint8 *) malloc (ephdr.phdr_table[i].p_filesz))
            == NULL)
        {
            printf("generate_hash_table: malloc failed.\n");
            fclose (file_ptr);
            return ERROR;
        }
            /* seek to the correct position. */
        if (fseek (file_ptr, ephdr.phdr_table[i].p_offset, SEEK_SET) == ERROR)
        {
            printf("generate_hash_table: fseek failed.\n");
            free (file_buff);
            fclose (file_ptr);
            return ERROR;
        }
            /* copy the selected segment into the buffer. */
        if (file_read (file_ptr, file_buff, ephdr.phdr_table[i].p_filesz) == ERROR)
        {
            free (file_buff);
            fclose (file_ptr);
            return ERROR;
        }

        /* Make sure that the hash is calculated such that every segments
        ** virtual address is 4K alligned.
        */
        if (seg_offset != 0)
        {
            seg_size  -= (PAGE_SIZE - seg_offset);
            seg_offset = PAGE_SIZE - seg_offset;
        }
            /* generate the hash for the selected segment. */
        if (secprogboot_generate_hashes (file_buff + seg_offset,
                                         seg_size, num_hashes,
                                         hash_buff) == ERROR)
        {
            printf("generate_hash_table: secprogboot_generate_hashes failed.\n");
            free (file_buff);
            fclose (file_ptr);
            return ERROR;
        }
            /* increment the hash_buff address. */
        hash_buff += (PROG_BOOT_DIGEST_SIZE * num_hashes);

            /* free memory allocated. */
        free (file_buff);
    }
        /* close the elf file. */
    fclose (file_ptr);

    return OK;

} /* generate_hash_table () */


/*===========================================================================

FUNCTION    UPDATE_ELF_PHDR

DESCRIPTION

    This function is used to copy all the segments to our new elf file.
    It will check the segments size and ensure that every segment is a
    multiple of PAGE_SIZE. 
    
    The parse_scl_file reads the amss scl file to determine whether a segment
    is paged, non-paged or not-used. The parse_scl_file also determines
    the starting address of every segment in our amss scl file. But it is in
    this function where we update the flag field and the file offset field
    in the program header based on the information obtained from the amss
    scl file.
    
DEPENDENCIES
    None.

RETURN VALUE
    ERROR   if invalid arguments were passed or
            if pboot_read_ephdr, pboot_update_ephdr, file_open, file_cpy,
            fseek or file_append_const returns an ERROR else
    OK

SIDE EFFECTS
    None.

===========================================================================*/

uint32 update_elf_phdr
(
  char *srcfname, /* source elf file name */
  char *destfname /* dest elf file name   */
)
{
  FILE  *ipfile_ptr, *opfile_ptr; /* file pointers. */
  elf_prog_header ephdr;  /* elf and program header. */
  uint32 num; /* number of bytes to append to a file. */
  uint32 i;   /* index. */
  uint32 file_size;   /* file size. */
  uint32 start_addr;  /* start address. */
  uint32 page_boundary;

  /* argument checks. */
  if ((srcfname == NULL) || (destfname == NULL))
  {
    return ERROR;
  }
  /* read the elf and program header from our input elf file. */
  if (pboot_read_ephdr (srcfname, &ephdr) == ERROR)
  {
    return ERROR;
  }
  /*  open the output elf file in write mode. */
  if (file_open (destfname, WRITE_MODE, &opfile_ptr) == ERROR)
  {
    return ERROR;
  }
  /*  open the input elf file in read mode. */
  if (file_open (srcfname, READ_MODE, &ipfile_ptr) == ERROR)
  {
    fclose (opfile_ptr);
    return ERROR;
  }
  /* copy the first page in our input elf file to our output elf file.
  ** The first page contains the elf and program header.
  */
  if (file_cpy (opfile_ptr, ipfile_ptr, PAGE_SIZE) == ERROR)
  {
    fclose (ipfile_ptr);
    fclose (opfile_ptr);
    return ERROR;
  }

  #ifdef DBUG_CREATESECELF
  printf("\nProgram Header contents before updating Program Header:\n");
  if (pboot_dispall_phdr (srcfname) == ERROR)
  {
    fclose (ipfile_ptr);
    fclose (opfile_ptr);
    return ERROR;
  }
  #endif

  /* copy all the segments to our output elf file. Ensure every segment
  ** is a multiple of PAGE_SIZE and update the program headers if
  ** required. 
  */
  for (i = 0; i < ephdr.elf_hdr.e_phnum; i++)
  {
    /* Only the first segment can be at address 0x0. The scl naming
    ** convention is:
    **   segment name start address file size.
    ** The start address can indicate either the file offset or
    ** start immediately after the previous segment. Hence, we need
    ** to check for the condition below.
    */
    if ((fstart_addr[i] == PAGE_SIZE) && (i != 0))
    {
      fstart_addr[i] = ephdr.phdr_table[i - 1].p_offset +
      ephdr.phdr_table[i - 1].p_filesz;
    }
    /* seek to the correct location in our output elf file.
    ** The file offset where the segment will be written was determined
    ** by parse_scl_file () and saved in fstart_addr array.
    */
    if (fseek (opfile_ptr, fstart_addr[i], SEEK_SET) == ERROR)
    {
      fclose (ipfile_ptr);
      fclose (opfile_ptr);
      return ERROR;
    }
    /* seek to the correct location in our input elf file. */
    if (fseek (ipfile_ptr, ephdr.phdr_table[i].p_offset, SEEK_SET) == ERROR)
    {
      fclose (ipfile_ptr);
      fclose (opfile_ptr);
      return ERROR;
    }

    /* copy the selected segment to the destination file. */ 
    if (file_cpy (opfile_ptr, ipfile_ptr, ephdr.phdr_table[i].p_filesz)
        == ERROR)
    {
      fclose (ipfile_ptr);
      fclose (opfile_ptr);
      return ERROR;
    }

    file_size  = ephdr.phdr_table[i].p_filesz;
    start_addr = ephdr.phdr_table[i].p_vaddr;
    /* Check if every segment is a multiple of PAGE_SIZE. Append
    ** CONST_VAL at the end of every segment to make it a multiple
    ** of PAGE_SIZE. Since in Nor flash the load and the execution
    **  region are the same this is done for nand flash **only**. 
    */
    if (!strncmp (flash_type, "nand", 4))
    {
      page_boundary = (ephdr.phdr_table[i].p_vaddr & (PAGE_SIZE - 1));
      if (page_boundary != 0)
      {
        file_size  -= (PAGE_SIZE - page_boundary);
        start_addr += (PAGE_SIZE - page_boundary);
      }

 //     printf ("page_boundary = 0x%x, start_addr = 0x%x, file_size = 0x%x\n",
 //              page_boundary, start_addr, file_size);
      if (num = (file_size & (PAGE_SIZE - 1)))
      {
        num = PAGE_SIZE - num;
        if (file_append_const (opfile_ptr, CONST_VAL, num) == ERROR)
        {
          fclose (ipfile_ptr);
          fclose (opfile_ptr);
          return ERROR;
        }
        /* update the file size information in the program header. */
        ephdr.phdr_table[i].p_filesz += num;
      }
    }  /* if  */
      /* update the flag and the file offset field in the
      ** program header.
      */
      ephdr.phdr_table[i].p_flags = (int) paged[i];
      ephdr.phdr_table[i].p_offset = fstart_addr[i];
      if (paged[i] == HASH)
      {
        ephdr.elf_hdr.e_phnum--;
      }
      //ephdr.phdr_table[i].p_vaddr = fstart_addr[i] - PAGE_SIZE;
  }   /* for */

  /* close all open files. */
  fclose (ipfile_ptr);
  fclose (opfile_ptr);

  /* update the elf header and the program headers in our elf file */
  if (pboot_update_ephdr (destfname, &ephdr) == ERROR)
  {
    return ERROR;
  }

  #ifdef DBUG_CREATESECELF
  printf("\nProgram Header contents after updating Program Header:\n");
  if (pboot_dispall_phdr (destfname) == ERROR)
  {
    return ERROR;
  }
#endif
    
  return OK;

} /* update_elf_phdr () */


/*===========================================================================

FUNCTION    PARSE_SCL_FILE

DESCRIPTION

    This function is used to read our amss scl file. It will determine
    whether a segment is pageable, non-pageable or not-used. It will also
    determine the starting address for every segment in our amss scl file.

DEPENDENCIES
    None.

RETURN VALUE
    ERROR   if invalid arguments were passed or
            if file_open () returns an ERROR or
            fgets returns NULL and eof is not reached else
    OK

SIDE EFFECTS
    None.

===========================================================================*/

uint32 parse_scl_file
(
    char *filename  /* file name. */
)
{
    FILE *file_ptr; /* file pointer. */
    char buf[100], last_read[100];  /* file buffers. */
    int  counter = 0; /* used to find the segment buffer. */

        /* argument checks. */
    if (filename == NULL)
    {
        return ERROR;
    }
        /* open the amss scl file in read mode. */
    if (file_open (filename, READ_MODE, &file_ptr) == ERROR)
    {
        return ERROR;
    }
        /* read till eof is reached. */
    while(!feof(file_ptr))
    {
            /* save the last line read from the input file. */
        strcpy (last_read, buf);

            /* read a line from the amss scl file. */
        if ((fgets(buf, 100, file_ptr) == NULL) && (!feof (file_ptr)))
        {
            printf("parse_scl_file: fgets returned NULL.\n");
            fclose (file_ptr);
            return ERROR;
        }

        #ifdef DBUG_PARSE_SCL_FILE
            printf("buf = %s\n", buf);
        #endif
        
            /* We look for the symbol "{" to determine the line to read.
            ** Make sure nested { } are skipped.
            */
        if ((!str_cmp (buf, "{")) && (counter++ == 0))
        {
            #ifdef DBUG_PARSE_SCL_FILE
                printf("counter = %d\tbuf = %s\tlast_read = %s\n",
                        counter, buf, last_read);
            #endif
            paged[num_segments] = is_pageable (last_read);
            sscanf (last_read, "%s%x", buf, &fstart_addr[num_segments]);
            fstart_addr[num_segments] += PAGE_SIZE;
            #ifdef DBUG_PARSE_SCL_FILE
                printf("0x%x\n", fstart_addr[num_segments]);
            #endif
            num_segments++;
        }
        else if (!str_cmp (buf, "}"))
        {
            #ifdef DBUG_PARSE_SCL_FILE
                printf("counter = %d\tbuf = %s\n", counter, buf);
            #endif
            counter--;
        }

    }
        /* close the input file. */
    fclose (file_ptr);

    return OK;

} /* parse_scl_file () */

/*===========================================================================

FUNCTION    IS_PAGEABLE

DESCRIPTION

    This is a helper function to parse_scl_file. It is used to determine if
    if the segment represented by the string is pageable, not-pageable or
    not-used.

DEPENDENCIES
    None.

RETURN VALUE
    PAGED       if pageable else
    NOTPAGED    if not pageable else
    HASH        if its a hash segment else
    NOTUSED 

SIDE EFFECTS
    None.

===========================================================================*/

uint32 is_pageable
(
    char *seg_info  /* string containing segment information */
)
{
    enum flagtype rtnval = NOTUSED; /* return value. */

        /* argument checks. */
    if (seg_info == NULL)
    {
        return ERROR;
    }
        /* determine if the segment is pageable or not-pageable. */
    if (!str_cmp (seg_info, NOTPAGEABLE))
    {
        #ifdef DBUG_PARSE_SCL_FILE
            printf("%s is NOTPAGEABLE.\n", seg_info);
        #endif
        rtnval = NOTPAGED;
    }
    else if (!str_cmp (seg_info, PAGEABLE))
    {
        #ifdef DBUG_PARSE_SCL_FILE
            printf("%s is PAGEABLE.\n", seg_info);
        #endif
        rtnval = PAGED;
    }
    else if (!str_cmp (seg_info, HASHTBL))
    {
        #ifdef DBUG_PARSE_SCL_FILE
            printf("%s is HASHTBL.\n", seg_info);
        #endif
        rtnval = HASH;
    }

    return rtnval;

} /* is_pageable () */


/*===========================================================================

FUNCTION    STR_CMP

DESCRIPTION

    This function is used to determine if the string represented by the
    second argument is present in the string represented by the first argument.

DEPENDENCIES
    None.

RETURN VALUE
    None.

SIDE EFFECTS
    ERROR   if invalid arguments else
    0       if a match is found else
    1
===========================================================================*/

uint32 str_cmp 
(
    char *s1,   /* string to compare */
    char *s2    /* string to compare */
)
{
        /* argument checks. */
    if ((s1 == NULL) || (s2 == NULL))
    {
        return ERROR;
    }

        /* read till end of string is reached. */
    while (strlen (s1) > 0)
    {
        if (s1[0] == s2[0])
        {
            if (strncmp (s1, s2, strlen (s2)) == 0)
            {
                return 0;
            }
        }
        s1++;
    }

    return 1;

} /* str_cmp () */


/*===========================================================================

FUNCTION    CREATE_HASH_FILE

DESCRIPTION

    This function is used to create our hash file.

DEPENDENCIES
    None.

RETURN VALUE
    ERROR   if invalid arguments or
            if file_open or file_bufcpy returns an EROR else
    OK

SIDE EFFECTS
    None.

===========================================================================*/


uint32 create_hash_file
(
  char *hash_file,    /* hash file name */
  uint8 *hash_table   /* hash table buffer */
)
{
  FILE *file_ptr; /* file pointer. */

  /* argument checks. */
  if ((hash_file == NULL) || (hash_table == NULL))
  {
    return ERROR;
  }
  /* open the hash file in write mode. */
  if (file_open (hash_file, WRITE_MODE, &file_ptr) == ERROR)
  {
    return ERROR;
  }
  /* write the hash buffer to the hash file. */
  if (file_bufcpy (file_ptr, hash_table, HASH_TABLE_SIZE) == ERROR)
  {
    fclose (file_ptr);
    return ERROR;
  }
  /* close the hash file. */
  fclose (file_ptr);

  return OK;

} /* create_hash_file () */


/*===========================================================================

FUNCTION  CHECK_FILE_ERRORS

DESCRIPTION

    This function will compare the file size and starting address of the
    input files and detects if there are any file overlaps.

DEPENDENCIES
    None.

RETURN VALUE
    ERROR   if there are file overlaps else
    OK

SIDE EFFECTS
    None.

===========================================================================*/

uint32 check_file_errors (void)
{
  int sum = 0; /* sum of size of all input binaries. */
  int i, j; /* index. */

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
  /* check if the hash table and the amss_app image overlap. */
  if ((fstart_addr[NUM_FILES-1]+fsize[NUM_FILES-1]) >
      (MI_NOR_AMSS_IMG_APP_SRC_ADDR + PAGE_SIZE))
  {
    printf("ERROR: %s and amss_app_image overlap.\n",ipfname[NUM_FILES-1]);
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

uint32 save_file_info
(
  char *fnames[],   /* file name. */
  char *flash_type /* nand/nor boot */
)
{
  int i; /* index. */
  elf_prog_header ephdr;      /* elf header. */

  /* argument checks. */
  if ((fnames == NULL) || (flash_type == NULL))
  {
    return;
  }
  /* save the input fine name and calculate the input file size. */
  for (i = 2; i < NUM_FILES + 2; i++)
  {
    fsize[i-2] = 0;
    strcpy (ipfname[i-2], fnames[i]);
    file_size (fnames[i], &(fsize[i-2]));
  }

  /* Check if we are creating the elf file for nand/nor boot.
  ** In case of Nand, we'll put the hash table and the hash header
  ** 4K after the first segment of the amss. In case of Nor, the
  ** location of the hash table is obtained from the Hash Table.
  */
  if (!strncmp (flash_type, "nand", 4))
  {
    fstart_addr[0] = MI_NAND_AMSS_HASH_TBL_HEADER_DEST_ADDR + PAGE_SIZE;
    fstart_addr[1] = fstart_addr[0] + fsize[0];
  }
  else
  {
    /* save the starting address. The first PAGE contains the elf
    ** and program headers. Hence, all the addresses are shifted by 4K.
    */
    fstart_addr[0] = MI_NOR_AMSS_HASH_TBL_HEADER_SRC_ADDR + PAGE_SIZE;
    fstart_addr[1] = MI_NOR_AMSS_HASH_TBL_SRC_ADDR + PAGE_SIZE;
  }

  #ifdef DBUG_CREATESECELF
  for (i = 0; i < NUM_FILES; i++)
  {
    printf("File Name = %s\tFile Size = 0x%x,%d\tStart Address = 0x%x,%d\n",
            ipfname[i], fsize[i], fsize[i], fstart_addr[i], fstart_addr[i]);
  }
  #endif

  /* save the output file name. */
  strcpy (opfname[0], fnames[0]);
  strcpy (opfname[1], fnames[1]);

} /* save_file_info () */
