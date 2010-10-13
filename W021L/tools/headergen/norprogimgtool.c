/*=============================================================================

                        NOR Progressive Image Tool

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

Copyright (c) 2004-2007 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/tools/headergen/norprogimgtool.c#4 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
08/15/06   anb     Made changes to handle the new ELF layout.
05/10/05   chs     Increased NOR Image size to 32MB.
03/01/05   chs     Increased NOR Image size to 16MB.
01/15/05   chs     modified for new nor architecture.
10/19/04   chs     created.


=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/

#include "pboot_melf.h"
#include "filetool.h"
#include "miheader.h"
#include "miimageheader.h"
#include "miaddress_generic.h"


/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

#define NUM_FILES       6UL
#define CONST_VAL       0xFFUL
#define MAX_SEGMENTS    0x20UL
#define NOR_IMG_SIZE    0x2000000UL  /* 32MB is our image size. */
#define GAP_FILL_RES    0x1000UL
/* Masks for segment flags. Writeable segments must go in RAM. */
#define ELF_FLAG_E_MASK 0x1
#define ELF_FLAG_W_MASK 0x2
#define ELF_FLAG_R_MASK 0x4

#undef DBUG_NORIMGTOOL_VERBOSE
#define DBUG_NORIMGTOOL

#ifdef DBUG_NORIMGTOOL_VERBOSE
//#define DEBUG(fmt,x,y,z) if(debug_print) fprintf(stderr, fmt, x,y,z);
#define DEBUG(fmt,x,y,z) printf(fmt, x,y,z);
#else
#define DEBUG(fmt,x,y,z)
#endif

static char     ipfname[NUM_FILES][80]; /* input file names.    */
static uint32   fsize[NUM_FILES];       /* input file size.     */
static uint32   fstart_addr[NUM_FILES]; /* start address of the input binaries
                                        ** in our nor image.
                                        */
static uint32   elf_seg_size[MAX_SEGMENTS];    /* size of every segment in our
                                               ** elf file.
                                               */
static uint32   num_segments;
elf_prog_header ephdr;  /* elf and program header. */
static uint32   elf_seg_offsets[MAX_SEGMENTS]; /* start address of every
                                               ** segment in our elf file.
                                               */
static char     opfname[80]; /* output file name. */
static uint32   write_done[NUM_FILES];
static uint32   total_nor_image_size;
boolean         change_shared_memory_addr = FALSE;
unsigned long   shared_memory_dest_addr = 0;
static uint32   elf_read = FALSE;
mi_boot_image_header_type hashheader;
char *hashdata_buff_ptr =NULL;
uint32 hashdata_total_size =0;

/*---------------------------------------------------------------------------
  
---------------------------------------------------------------------------*/



/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/


static uint32 check_file_errors (void);
static uint32 verify_comp_image (void);
static uint32 save_file_info (char **);
static uint32 create_comp_img (char **);
static uint32 get_file_info (uint32 *, uint32 *, uint32 *);
static uint32 update_elf_phdr (char *, elf_prog_header *);
static uint32 get_hash_header_information (char *);
static uint32 find_image_gap(uint32);

int main(uint32 argc, char *argv[])
{
  uint32 offset;
  uint32 start_addr;
  uint32 size;
  uint32 i, done=0;
  boolean done_with_elf_header=FALSE;

  /* argument checks. */
  if (argc < (NUM_FILES + 2))
  {
    printf ("USAGE: norprogimgtool AMSS DBL_HEADER DBL FSBL OSBL OSBL_HEADER COMP_IMG_NAME SHARED_MEMORY_ADDR\n");
    return ERROR;
  }
  if (argc == (NUM_FILES + 3))
  {
    change_shared_memory_addr = TRUE;
    sscanf(argv[7], "%x", &shared_memory_dest_addr);
    #ifdef DBUG_NORIMGTOOL_VERBOSE
      printf("SHARED MEMORY dest addr: 0x%x\n", shared_memory_dest_addr);
    #endif
  }

  DEBUG("MAIN - Updating Elf Header\n",0,0,0);
  /* update elf header. */
  if ( update_elf_phdr (argv[1], &ephdr) == ERROR)
  {
    printf ("update_elf_phdr returned an ERROR.\n");
    free(hashdata_buff_ptr);    
    return ERROR;
  }

  DEBUG("MAIN - Get Hash Header Information from %s\n",argv[1],0,0);
  /* update elf header. */
  if ( get_hash_header_information (argv[1]) == ERROR)
  {
    printf ("update_elf_phdr returned an ERROR.\n");
    free(hashdata_buff_ptr);
    return ERROR;
  }

  DEBUG("MAIN - Saving File Info\n",0,0,0);
  /* save passed arguments. */
  if (save_file_info (argv) == ERROR)
  {
    printf ("save_file_info returned an ERROR.\n");
    free(hashdata_buff_ptr);
    return ERROR;
  }

  DEBUG("MAIN - Check file for errors\n",0,0,0);
  /* check for file errors. */
  if (check_file_errors() == ERROR)
  {
    free(hashdata_buff_ptr);
    return ERROR;
  }

  DEBUG("MAIN - Generating Composite Image %s\n",opfname,0,0);
  /* create the composite image. */
  if (create_comp_img (&argv[1]) == ERROR)
  {
    free(hashdata_buff_ptr);
    return ERROR;
  }

  DEBUG("MAIN - Verifying Composite Image %s\n",opfname,0,0);
  /* check the composite image and verify its contents. */
  if (verify_comp_image () == ERROR)
  {
    free(hashdata_buff_ptr);  
    return ERROR;
  }

  DEBUG("Composite image %s was generated and verified successfully.\n",
          opfname,0,0);

  free(hashdata_buff_ptr);
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

uint32 create_comp_img
(
  char **ipfname  /* i/p file name. */
)
{
  uint32 i,j; /* file index. */
  uint32 ipfsize = 0, opfsize = 0;/* input and output file size. */
  FILE *ipfile_ptr, *opfile_ptr;  /* File pointers. */
  uint32  done = 0;  /* while loop indicator. */ 
  char *src_file_name;
  uint32 offset;
  uint32 start_addr;
  uint32 size;
  static boolean done_with_elf_header = FALSE;
  elf_prog_header ephdr_local;
  uint32 phdr_address = 0;
  uint32 phdr_size = 0;

  ephdr_local.elf_hdr.e_phnum = 0;

  /* open the output file in write mode. */
  if (file_open (opfname, "wb", &opfile_ptr) == ERROR)
  {
    return ERROR;
  }
  if (file_append_const (opfile_ptr, CONST_VAL, total_nor_image_size) == ERROR)
  {
    return ERROR;
  }

  /* Read all the input binaries to create the composite image. */
  while (!done)
  {
    /* get the index, start address and the size of the file/segment to write.
    */
    if ((i = get_file_info (&offset, &start_addr, &size)) == ERROR)
    {
      done = 1;
      continue;
    }

    #ifdef DBUG_NORIMGTOOL_VERBOSE
      printf ("offset = %d, 0x%x\tstart_addr = %d, 0x%x\tsize = %d, 0x%x\n",
               offset, offset, start_addr, start_addr, size, size);
    #endif
    DEBUG("\nstart_addr 0x%08x\n",start_addr,0,0);
    if  ( (elf_read == TRUE) && (done_with_elf_header == FALSE))
    {
      uint32 phdr_entry_size = 0;
      uint32 phdr_num_segment = 0;
      uint32 phdr_offset = 0;

      /* update elf header. */
      if ( update_elf_phdr (ipfname[i], &ephdr_local) == ERROR)
      {
        printf ("update_elf_phdr returned an ERROR.\n");
        return ERROR;
      }
      if (change_shared_memory_addr)
      {
        for (j = 0; j < ephdr_local.elf_hdr.e_phnum; j++)
        {
          if (MI_PBT_ACCESS_TYPE_VALUE(ephdr_local.phdr_table[j].p_flags) == MI_PBT_SHARED_SEGMENT)
          {
            ephdr_local.phdr_table[j].p_paddr = shared_memory_dest_addr;
            ephdr_local.phdr_table[j].p_vaddr = shared_memory_dest_addr;
            break;
          }
        }
      }
      /* Get program header location from ELF header in src_elf */
      phdr_offset = ephdr_local.elf_hdr.e_phoff;
      DEBUG("Found program header at offset [0x%x] in source file\n", phdr_offset, 0, 0);
      
      /* Calculate program header size */
      phdr_entry_size = ephdr_local.elf_hdr.e_phentsize;
      phdr_num_segment = ephdr_local.elf_hdr.e_phnum;
      phdr_size = phdr_entry_size * phdr_num_segment;

      if(phdr_size > UINT16_MAX)
      {
        printf("phdr_size is greater than the the limit");
        return ERROR;			
      }
	  
      DEBUG("Total program header size in bytes: [%d]\n", phdr_size, 0, 0);
      
      /* Copy ELF Header */
      if (fseek (opfile_ptr, start_addr, SEEK_SET) == ERROR)
      {
        fclose (opfile_ptr);
        return ERROR;
      }

      DEBUG("CCI - Copy ELF Header to 0x%08x\n",start_addr,0,0);
      if (file_bufcpy(opfile_ptr, (uint8 *)&ephdr_local.elf_hdr, ELF_HDR_SIZE) == ERROR)
      {
        fclose (opfile_ptr);
        return ERROR;
      }

      /* Copy Hash+Cert */
      if (fseek (opfile_ptr, start_addr+ELF_HDR_SIZE, SEEK_SET) == ERROR)
      {
        fclose (opfile_ptr);
        return ERROR;
      }

      DEBUG("CCI - Copy Hash and Cert to 0x%08x\n",start_addr+ELF_HDR_SIZE,0,0);
      if (file_bufcpy(opfile_ptr, hashdata_buff_ptr, hashdata_total_size) == ERROR)
      {
        fclose (opfile_ptr);
        return ERROR;
      }      

      /* Copy Program Header */      
      if (fseek (opfile_ptr, start_addr+phdr_offset, SEEK_SET) == ERROR)
      {
        fclose (opfile_ptr);
        return ERROR;
      }

      DEBUG("CCI - Copy Program Header to 0x%08x\n",start_addr+phdr_offset,0,0);
      if (file_bufcpy(opfile_ptr, (uint8 *)&ephdr_local.phdr_table, phdr_size) == ERROR)
      {
        fclose (opfile_ptr);
        return ERROR;
      }
      phdr_address = start_addr+phdr_offset;
      done_with_elf_header = TRUE;
    }
    else
    {
      if (start_addr < NOR_IMG_SIZE)
      {
        
        /* open the input file in read mode. */
        if (file_open (ipfname[i], "rb", &ipfile_ptr) == ERROR)
        {
          return ERROR;
        }
   
        /* seek to the correct location. */
        if (fseek (ipfile_ptr, offset, SEEK_SET) == ERROR)
        {
          fclose (ipfile_ptr);
          return ERROR;
        }
  
        if (fseek (opfile_ptr, start_addr, SEEK_SET) == ERROR)
        {
          fclose (opfile_ptr);
          return ERROR;
        }
  
        #ifdef DBUG_NORIMGTOOL_VERBOSE
          printf ("Writing image %s to the composite image at 0x%x from 0x%x.\n", ipfname[i],start_addr,offset);
        #endif
   
        /* copy the input binary to our nor image. */
        if (file_cpy (opfile_ptr, ipfile_ptr, size) == ERROR)
        {
          return ERROR;
        }
        /* close the input file. */
        fclose (ipfile_ptr);
      }
    }
    
    if (i != 0)
    {
      write_done[i-1] = 1;
    }

  }

  for (j = 0; j < ephdr_local.elf_hdr.e_phnum; j++)
  {
    if (ephdr_local.phdr_table[j].p_paddr > NOR_IMG_SIZE )//(ephdr_local.phdr_table[j].p_flags & ELF_FLAG_W_MASK) != 0)
    {
      uint32 w_seg_offset = find_image_gap(j);
      #ifdef DBUG_NORIMGTOOL_VERBOSE
        printf("Gap found at %x.\n",w_seg_offset);
      #endif
      if (w_seg_offset != ERROR)
      {
        /* open the amss input file in read mode. */
        if (file_open (ipfname[0], "rb", &ipfile_ptr) == ERROR)
        {
          return ERROR;
        }
   
        /* seek to the correct location. */
        if (fseek (ipfile_ptr, ephdr_local.phdr_table[j].p_offset, SEEK_SET) == ERROR)
        {
          fclose (ipfile_ptr);
          return ERROR;
        }
  
        if (fseek (opfile_ptr, w_seg_offset, SEEK_SET) == ERROR)
        {
          fclose (opfile_ptr);
          return ERROR;
        }
        
  
        #ifdef DBUG_NORIMGTOOL_VERBOSE
          printf ("Writing image %s to the composite image.\n", ipfname[0]);
        #endif
   
        /* copy the input binary to our nor image. */
        if (file_cpy (opfile_ptr, ipfile_ptr, ephdr_local.phdr_table[j].p_filesz) == ERROR)
        {
          return ERROR;
        }
        /* close the input file. */
        fclose (ipfile_ptr);
  
        ephdr_local.phdr_table[j].p_offset = w_seg_offset;
        ephdr.phdr_table[j].p_offset = w_seg_offset;
      }
      else
      {
        printf("Error could not find gap for segment %d.\n",j);
        return ERROR;
      }
      
    }
  }


  /* If image was created correctly, update the Program Header with correct offsets. */
  if ((elf_read == TRUE) && (done_with_elf_header == TRUE))
  {

      if(ephdr_local.elf_hdr.e_phnum > UINT16_MAX)
      {
        printf("ephdr_local.elf_hdr.e_phnum is grater than (practical) limit \n");
		return ERROR;
	  }
  
    /* Adjust offset to physical address if tools already did so. */
    for (j = 0; j < ephdr_local.elf_hdr.e_phnum; j++)
    {
      if (ephdr_local.phdr_table[j].p_paddr < NOR_IMG_SIZE)
      {
        ephdr_local.phdr_table[j].p_offset = ephdr_local.phdr_table[j].p_paddr;
      }
    }

    /* Copy Program Header */      
    if (fseek (opfile_ptr, phdr_address, SEEK_SET) == ERROR)
    {
      fclose (opfile_ptr);
      return ERROR;
    }

    DEBUG("CCI - Copy Updated Program Header to 0x%08x\n",phdr_address,0,0);
    if (file_bufcpy(opfile_ptr, (uint8 *)&ephdr_local.phdr_table, phdr_size) == ERROR)
    {
      fclose (opfile_ptr);
      return ERROR;
    }

  }

 #ifdef DBUG_NORIMGTOOL
  /* Print final headers */
  printf("\nFinished ELF Program Header\n\n");

  printf("%-10s %-10s %-10s %-10s %-10s %-4s %-10s %-6s\n",
         "offset","filesz","flags","vaddr",
         "paddr","type","memsz","align");

  /* display all the program headers. */
  for (i = 0; i < ephdr_local.elf_hdr.e_phnum; i++)
  {
    /* display the program header. */
    if (pboot_disp_phdr (&(ephdr_local.phdr_table[i])) == ERROR)
    {
      return ERROR;
    }
  }
#endif

  /* close the output file. */
  fclose (opfile_ptr);

  return OK;} /* create_comp_img () */


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
        if ((i = get_file_info (NULL, NULL, NULL)) == ERROR)
        {
            done = 1;
            continue;
        }

            /* Is this an unused region in our nor image? */
        if (addr != fstart_addr[i])
        {
            #ifdef DBUG_NORIMGTOOL_VERBOSE
                printf("Address 0x%x to 0x%x is being verified for 1's....",
                        addr, (fstart_addr[i] - 1));
            #endif
            if (file_cmp_const (opfile_ptr, CONST_VAL, (fstart_addr[i] - addr)) != 0)
            {
                printf("verification failed.\n");
                return ERROR;
            }
            #ifdef DBUG_NORIMGTOOL_VERBOSE
                printf("verification done.\n");
            #endif
            addr = fstart_addr[i];
        }
            /* open the input file in read mode. */
        if (file_open (ipfname[i], "rb", &(ipfile_ptr[i])) == ERROR)
        {
            return ERROR;
        }
            /* compare the input and the output file. */
        #ifdef DBUG_NORIMGTOOL_VERBOSE
            printf ("Verifying %s and %s...", ipfname[i], opfname);
        #endif
        if (file_cmp (opfile_ptr, ipfile_ptr[i]) != 0)
        {
            printf("verification failed.\n");
            return ERROR;
        }
        #ifdef DBUG_NORIMGTOOL_VERBOSE
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
  calculates the total image size to ensure it doesn't exceed the
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

  for (i = 0; i < ephdr.elf_hdr.e_phnum; i++)
  {
    sum += ephdr.phdr_table[i].p_filesz;
  }

  /* check if the total image size exceeds NOR_IMG_SIZE. */
  if (sum > NOR_IMG_SIZE)
  {
    printf("ERROR: Sum of all the images exceeds 0x%x\n", NOR_IMG_SIZE);
    return ERROR;
  }

  total_nor_image_size =sum;
#ifdef DBUG_NORIMGTOOL
  printf("Total NOR image size:%d 0x%x.\n", total_nor_image_size, total_nor_image_size);
#endif  

  /* check for file overlaps. */
  /**********************************************************************
  ** This will not check for overlaps of the input binaries with the amss
  ** segments. This needs to be implemented.
  ***********************************************************************/
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

  It will also save the segment address by reading the program headers in the
  elf file so that it can be used later while generating the nor image.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

uint32 save_file_info
(
  char *fnames[]  /* input file name. */
)
{
  int i; /* file index. */

  /* save the starting address. */
  fstart_addr[0] = MI_NOR_DBL_HEADER_SRC_ADDR;
  fstart_addr[1] = MI_NOR_DBL_IMG_SRC_ADDR;
  fstart_addr[2] = MI_NOR_FSBL_IMG_SRC_ADDR;
  fstart_addr[3] = MI_NOR_OSBL_HEADER_SRC_ADDR;
  fstart_addr[4] = MI_NOR_OSBL_IMG_SRC_ADDR;

  /* save the input fine name and calculate the input file size. */
  for (i = 2; i <= NUM_FILES; i++)
  {
    fsize[i-2] = 0;
    strcpy (ipfname[i-2], fnames[i]);
    file_size (fnames[i], &(fsize[i-2]));

    #ifdef DBUG_NORIMGTOOL_VERBOSE
      printf("\nFile Name = %s\nFile Size = 0x%08x\tStart Address = 0x%08x\n",
              ipfname[i-2], fsize[i-2], fstart_addr[i-2]);
    #endif
        
    write_done[i-2] = 0;
  }

  /* save the output file name. */
  strcpy (opfname, fnames[NUM_FILES+1]);

} /* save_file_info () */


/*===========================================================================

FUNCTION    UPDATE_ELF_HEADER

DESCRIPTION

  This function is used to save the file name information passed by the
  user. The starting address and the file size for the corresponding
  files is also saved.

  It will also save the segment address by reading the program headers in the
  elf file so that it can be used later while generating the nor image.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

uint32 update_elf_phdr
(
  char *fname,  /* input file name. */
  elf_prog_header *ephdr_write
)
{
  int i; /* file index. */

  /* check arguments. */
  if (fname == NULL)
  {
    return ERROR;
  }

  /* read the elf and program header from our input elf file. */
  if (pboot_read_ephdr (fname, ephdr_write) == ERROR)
  {
    return ERROR;
  }

  #ifdef DBUG_NORIMGTOOL_VERBOSE
    if (pboot_dispall_phdr (fname) == ERROR)
    {
      return ERROR;
    }
  #endif
  return OK;

} /* update_elf_header () */

/*===========================================================================

FUNCTION    IMAGE_OVERLAP

DESCRIPTION

  This function will compare two image start addresses and lengths to see
  if they overlap or not.

DEPENDENCIES
  None.

RETURN VALUE
  True if they overlap.
  False if they don't overlap.

SIDE EFFECTS
  None.

===========================================================================*/
uint32 image_overlap
(
  uint32 img1_start,
  uint32 img1_size,
  uint32 img2_start,
  uint32 img2_size
)
{

  if ( ( img1_start + img1_size ) < img2_start )
  {
    return FALSE;
  }
  if ( ( img2_start + img2_size ) < img1_start )
  {
    return FALSE;
  }
  return TRUE;

} /* image_overlap */


/*===========================================================================

FUNCTION    FIND_IMAGE_GAP

DESCRIPTION

  This function find an available gap in the output image for a given
  writable segment. It will return the first available gap on a 4k boundary.

DEPENDENCIES
  Segments to be moved must be passed to this function in order. It will
  assume the all writable segments with a smaller index have been placed
  properly in the output file and ensure the current one avoids overwriting.

RETURN VALUE
  The address to write the segment to in the output file.

SIDE EFFECTS
    None.

===========================================================================*/

uint32 find_image_gap
(
  uint32 segment_num       /* file/segment size. */
)
{
  uint32 rtnVal = 0;/*ERROR; *//* return value. */
  uint32 i;  /* index. */
  uint32 seg_index;
  uint32 w_seg_size = ephdr.phdr_table[segment_num].p_filesz;
  uint32 gap_found = FALSE;

  for (rtnVal = 0x0; ((rtnVal < NOR_IMG_SIZE) && (gap_found == FALSE)); rtnVal+=GAP_FILL_RES)
  {
    gap_found = TRUE;
    /* Test input binary files */
    for (i = 0; i < (NUM_FILES - 1); i++)
    {
      if ( image_overlap(fstart_addr[i], fsize[i], rtnVal, w_seg_size) )
      {
        gap_found = FALSE;
        break;
      }
    }

    if (!gap_found)
    {
      continue;
    }

    /* Test phdr entries */
    for (i = 0; i < ephdr.elf_hdr.e_phnum; i++)
    {
      /* Test writable segments before this index at their output file locations. */
      if ( ephdr.phdr_table[i].p_paddr > NOR_IMG_SIZE ) //(ephdr.phdr_table[i].p_flags & ELF_FLAG_W_MASK) != 0 )
      {
        if ((i < segment_num) && 
            (image_overlap(ephdr.phdr_table[i].p_offset, ephdr.phdr_table[i].p_filesz,
                           rtnVal, w_seg_size)) )
        {
          gap_found = FALSE;
          break;
        }
      }
      /* Test RO segments at their output file locations. */
      else
      {
        if (image_overlap(ephdr.phdr_table[i].p_paddr, ephdr.phdr_table[i].p_filesz,
                           rtnVal, w_seg_size))
        {
            gap_found = FALSE;
            break;
        }
      }
    }

    if (!gap_found)
    {
      continue;
    }

    /* Test ELF Header Address */
    if (image_overlap(MI_NOR_AMSS_BOOT_ELF_HEADER_SRC_ADDR, MI_NOR_AMSS_BOOT_ELF_HEADER_MAX_SIZE,
                      rtnVal, w_seg_size))
    {
      gap_found = FALSE;
    }

    if (!gap_found)
    {
      continue;
    }

 }

  /* Account for extra added address. */
  if (gap_found) {
    rtnVal -= GAP_FILL_RES;
  }
  

  if (rtnVal > NOR_IMG_SIZE)
  {
    return ERROR;
  }
  
  return rtnVal;
} /* get_file_info () */



/*===========================================================================

FUNCTION    GET_FILE_INDEX

DESCRIPTION

  This function determines the next file to be written into the composite
  image. The file start address of all the input files and elf segments is
  compared to find out which file/segment will be written next.

DEPENDENCIES
  None.

RETURN VALUE
  FILE Index of the next file to be written to the composite image else

SIDE EFFECTS
    None.

===========================================================================*/

uint32 get_file_info
(
  uint32 *offset,    /* file/segment offset. */
  uint32 *start_addr,/* file/segment address */
  uint32 *size       /* file/segment size. */
)
{
  uint32 rtnVal = ERROR; /* return value. */
  uint32 i;  /* index. */
  uint32 seg_index = 0;

  /* check for invalid arguments. */
  if ((offset == NULL) || (start_addr == NULL) || (size == NULL))
  {
    return ERROR;
  }

  *start_addr = (uint32) 0xFFFFFFFF;

  /* find which input binary will be written next. */
  for (i = 0; i < (NUM_FILES - 1); i++)
  {
    if ((fstart_addr[i] <= *start_addr) && (write_done[i] == 0))
    {
      *start_addr = fstart_addr[i];
      *offset = 0;
      *size = fsize[i];
      rtnVal = i + 1;
    }
  }

  /* read the program header and save the file offset in an array. It will be
  ** used later to generate our nor image.
  */
  for (i = 0; i < ephdr.elf_hdr.e_phnum; i++)
  {
    if (ephdr.phdr_table[i].p_offset < *start_addr)
    {

      *start_addr = ephdr.phdr_table[i].p_paddr;
      *offset = ephdr.phdr_table[i].p_offset;
      *size = ephdr.phdr_table[i].p_filesz;
      rtnVal = 0;
      seg_index = i;
    }
  }

  if ((*start_addr > MI_NOR_AMSS_BOOT_ELF_HEADER_SRC_ADDR) &&
     (elf_read == FALSE))
  {
    *start_addr = MI_NOR_AMSS_BOOT_ELF_HEADER_SRC_ADDR;
    *offset = 0;
    *size = MI_NOR_AMSS_BOOT_ELF_HEADER_MAX_SIZE;
    rtnVal = 0;
    elf_read = TRUE;
  }

  if ((rtnVal == 0) && (*offset != 0))
  {
    ephdr.phdr_table[seg_index].p_offset = 0xFFFFFFFF;
  }

  if (*start_addr == 0xFFFFFFFF)
  {
    return ERROR;
  }

  return rtnVal;
} /* get_file_info () */

/*===========================================================================

FUNCTION    GET_HASH_HEADER_INFORMATION

DESCRIPTION
  This function will determine the size needed to store the hash and certificate
  after the ELF header in the composite images.

DEPENDENCIES
  None.

RETURN VALUE
  ERROR if found any

SIDE EFFECTS
  None.

===========================================================================*/
uint32 get_hash_header_information 
(
  char *fname
 )
{
  int i; /* file index. */
  FILE *ipfile_ptr;  /* File pointers. */

  /* check arguments. */
  if (fname == NULL)
  {
    return ERROR;
  }

  if (file_open (fname, "rb", &ipfile_ptr) == ERROR)
  {
    return ERROR;
  }

  if (fseek (ipfile_ptr, ELF_HDR_SIZE, SEEK_SET) == ERROR)
  {
    fclose (ipfile_ptr);
    return ERROR;
  }

  if (file_read(ipfile_ptr, (char *)&hashheader, sizeof(mi_boot_image_header_type)) == ERROR)
  {
    fclose (ipfile_ptr);
    return ERROR;
  }

#ifdef DBUG_NORIMGTOOL_VERBOSE
  printf("\n HASH HEADER \n");
  printf("hashheader.image_id:%d 0x%x\n", hashheader.image_id, hashheader.image_id);
  printf("hashheader.header_vsn_num:%d 0x%x\n", hashheader.header_vsn_num, hashheader.header_vsn_num);
  printf("hashheader.image_src:%d 0x%x\n", hashheader.image_src, hashheader.image_src);    
  printf("hashheader.image_dest_ptr:%d 0x%x\n", hashheader.image_dest_ptr, hashheader.image_dest_ptr);  
  printf("hashheader.image_size:%d 0x%x\n", hashheader.image_size, hashheader.image_size);  
  printf("hashheader.code_size:%d 0x%x\n", hashheader.code_size, hashheader.code_size);
  printf("hashheader.signature_ptr:%d 0x%x\n", hashheader.signature_ptr, hashheader.signature_ptr);  
  printf("hashheader.signature_size:%d 0x%x\n", hashheader.signature_size, hashheader.signature_size);  
  printf("hashheader.cert_chain_ptr:%d 0x%x\n", hashheader.cert_chain_ptr, hashheader.cert_chain_ptr);  
  printf("hashheader.cert_chain_size:%d 0x%x\n", hashheader.cert_chain_size, hashheader.cert_chain_size);    
#endif

  hashdata_total_size = sizeof(mi_boot_image_header_type) +
                        hashheader.signature_size +
                        hashheader.cert_chain_size;
  
  if ((hashdata_buff_ptr = (char *) malloc (hashdata_total_size)) == NULL)
  {
    printf("fail to malloc memory: malloc failed.\n");
    return ERROR;
  }

  if (fseek (ipfile_ptr, ELF_HDR_SIZE, SEEK_SET) == ERROR)
  {
    fclose (ipfile_ptr);
    return ERROR;
  }
  
  if (file_read(ipfile_ptr, hashdata_buff_ptr, hashdata_total_size) == ERROR)
  {
    fclose (ipfile_ptr);
    return ERROR;
  }
  
  fclose (ipfile_ptr);
  return OK;
}
