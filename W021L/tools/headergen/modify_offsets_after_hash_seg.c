/*=============================================================================
             MODIFY OFFSETS AFTER HASH SEGMENT

This tool modifies the file offsets of segments after the Hash segment 
to reserve space for Hash segment added by pboot_add_hash tool


Copyright (c) 2006 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/
/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/tools/pboot/modify_offsets_after_hash_seg.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
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
unsigned int hash_seg_memsz = 0;

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
   printf("This program gets the Hash Segment Size from SCL file and \n"
          "shifts all segments below hash segment with the hash segment size \n"
          "Usage: %s [-d] scl_file in_elf_file out_elf_file \n"
          "     -d               - debug mode\n"
		  "     scl_file         - input Scatter Loader File\n"
          "     in_elf_file      - the Input ELF file\n"
          "     out_elf_file     - the Output ELF file\n");
}



/*===========================================================================
FUNCTION  MODIFY_OFFSETS_AFTER_HASH_SEGMENT

DESCRIPTION
    This Function modifies the offsets of segments after Hash segment to reserve space for Hash Segment.

DEPENDENCIES
    None.

RETURN VALUE
    errno on error
    else 0

SIDE EFFECTS

===========================================================================*/
int modify_offsets_after_hash_segment(FILE *src_elf,FILE *dest_elf)
{
  struct stat fs;
  struct elf_file_info src_file;
  struct elf_file_info dest_file;
  Elf32_Phdr *i_hdr;
  

  fpos_t phdr_pos;
  int size, ret, i;
  uint32_t file_offset, phdr_size,src_offset,src_file_offset,dest_file_offset,src_elf_size,hash_next_seg_offset;
  uint32_t hash_seg_offset_adjust = 0;
  uint32_t hash_seg_offset, hash_seg_found;

 
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

  ret = verify_elf_header(&src_file.elf_hdr);
  if (ret)
  {
    printf("ELF header fails verification\n");
    return ERROR;
  }

  /* Get src_elf size */
  if(fstat(src_file.fd, &fs))
  {
	printf("Unable to stat src_elf file!\n");
	return errno;
  }
  src_elf_size = fs.st_size;
  DEBUG("\n\nInput file size in bytes: [%d]\n", src_elf_size, 0, 0);


  phdr_size = src_file.elf_hdr.e_phentsize;
  
  
  /*----------------------------------------------------------------------
   ** Copy all the Program Headers and Segments to the destination file
   /* Also change the file offset accordingly 
  ----------------------------------------------------------------------*/
  src_file_offset = src_file.elf_hdr.e_phoff;
  DEBUG("Source File Phdr offset is %x \n", src_file_offset,0,0); 

	  for(i=1; i<=src_file.elf_hdr.e_phnum ; i++)
	  {
		 i_hdr = &src_file.phdr_table[i];

		 ret = fseek(src_elf, src_file_offset, SEEK_SET);
		 if (ret)
		 {
		  printf("Cannot fseek. ret(%d) - errno(%d)\n", ret, errno);
		  return errno;
		 }

         /* Get the segment description */
		if(fread(i_hdr, phdr_size, 1, src_elf) != 1)
		{
		  printf("Unable to read program header!\n");
		  return errno;
		}

		DEBUG("Segment[%d]: offset[%x] vaddr[%x]", i, i_hdr->p_offset, i_hdr->p_vaddr);
		DEBUG(" type[%x] size[%x] memsz[%x]\n", i_hdr->p_flags, i_hdr->p_filesz, i_hdr->p_memsz);


		/* Check if the segment is a Hash Segment */
		if(MI_PBT_SEGMENT_TYPE_VALUE(i_hdr->p_flags) == MI_PBT_HASH_SEGMENT)
		{
		  hash_seg_offset = i_hdr->p_offset;
		  hash_seg_found = 1;
        }
		else if (hash_seg_found == 1) 
		{
		  hash_seg_offset_adjust = (hash_seg_memsz - (i_hdr->p_offset - hash_seg_offset));
		  hash_next_seg_offset = i_hdr->p_offset;
          hash_seg_found = 2;
		  break;
		}

        src_file_offset = src_file_offset + phdr_size;
		dest_file_offset = dest_file_offset + phdr_size;

	  }

	  src_file_offset = ELF_HDR_SIZE;
	  dest_file_offset = ELF_HDR_SIZE;

       DEBUG("\n fcpy Src_file_offset[%x]: dest_file_offset[%x] size[%x]", src_file_offset, dest_file_offset, hash_seg_offset);
      /* Copy From SRC to DEST till the Hash segment here */
	  ret = fcpy(src_elf,  src_file_offset, dest_elf, dest_file_offset, hash_seg_offset);
      if(ret)
      {
	    printf("Copy Segment[%d] failed! ret(%d) - errno(%d)\n",i, ret, errno);
	    return errno;
      }

	  src_file_offset = hash_next_seg_offset;
	  dest_file_offset = (hash_next_seg_offset + hash_seg_offset_adjust);

	  DEBUG("\n Src_file_offset[%x]: dest_file_offset[%x] size[%x]", src_file_offset, dest_file_offset, (src_elf_size - hash_next_seg_offset));
	  /* Copy From SRC to DEST from the end of Hash segment to the end of file here */
	  ret = fcpy(src_elf,  src_file_offset, dest_elf, dest_file_offset, (src_elf_size - hash_next_seg_offset));
      if(ret)
      {
	    printf("Copy Segment[%d] failed! ret(%d) - errno(%d)\n",i, ret, errno);
	    return errno;
      }


    /*----------------------------------------------------------------------
    ** COPY the ELF header to the Destination File
    ----------------------------------------------------------------------*/
    
    src_file.elf_hdr.e_phoff = (src_file.elf_hdr.e_phoff + hash_seg_offset_adjust);
	src_file.elf_hdr.e_shoff = (src_file.elf_hdr.e_shoff + hash_seg_offset_adjust);

	DEBUG("\n e_phoff[%x]: e_shoff[%x] hash_seg_offset_adjust[%x]", src_file.elf_hdr.e_phoff, src_file.elf_hdr.e_shoff, hash_seg_offset_adjust);

    ret = lseek(dest_file.fd, 0, SEEK_SET);
    if (ret)
    {
	  printf("Cannot lseek. ret(%d) - errno(%d)\n", ret, errno);
	  return errno;
    }

	/* Copy the ELF header here */
	 if(fwrite((char*) &src_file.elf_hdr,ELF_HDR_SIZE, 1, dest_elf) != 1)
	 {
	   printf("Write ELF header failed! ret(%d) - errno(%d)\n", ret, errno);
	   return errno;
	 }

    /*----------------------------------------------------------------------
    ** Adjust the program Headers of the Destination File
    ----------------------------------------------------------------------*/

	src_file_offset = (src_file.elf_hdr.e_phoff - hash_seg_offset_adjust);
	dest_file_offset = src_file.elf_hdr.e_phoff;


  for(i=1; i<=src_file.elf_hdr.e_phnum ; i++)
  {
     i_hdr = &src_file.phdr_table[i];

	 ret = fseek(src_elf, src_file_offset, SEEK_SET);
     if (ret)
     {
	  printf("Cannot fseek. ret(%d) - errno(%d)\n", ret, errno);
	  return errno;
     }

	 ret = fseek(dest_elf, dest_file_offset, SEEK_SET);
     if (ret)
     {
	  printf("Cannot fseek. ret(%d) - errno(%d)\n", ret, errno);
	  return errno;
     }

     /* Get the segment description */
    if(fread(i_hdr, phdr_size, 1, src_elf) != 1)
    {
	  printf("Unable to read program header!\n");
	  return errno;
	}

    DEBUG("\n Segment[%d]: offset[%x] vaddr[%x]", i, i_hdr->p_offset, i_hdr->p_vaddr);
    DEBUG(" type[%x] size[%x] memsz[%x]\n", i_hdr->p_flags, i_hdr->p_filesz, i_hdr->p_memsz);


    /* Check if the segment is a Hash Segment */
	if(MI_PBT_SEGMENT_TYPE_VALUE(i_hdr->p_flags) == MI_PBT_HASH_SEGMENT)
	{
      i_hdr->p_filesz = hash_seg_memsz;
      i_hdr->p_memsz = hash_seg_memsz;
	  hash_seg_found = 3;
    }
	else if (hash_seg_found == 3) 
	{
      hash_seg_found = 4;
	}
    
   
    if (hash_seg_found == 4) 
	{
     DEBUG("\n p_offset  i_hdr->p_offset Before Change [%#x] :\n",i_hdr->p_offset,0,0);
        i_hdr->p_offset = (i_hdr->p_offset + hash_seg_offset_adjust);
     DEBUG("\n p_offset  i_hdr->p_offset After Change [%#x] :\n",i_hdr->p_offset,0,0);
	}

     DEBUG("%d dest offset %d src offset %x ihdr p_offset \n", ftell(dest_elf), ftell(src_elf),i_hdr->p_offset);

	 /* Copy the Phdr here */
	 if(fwrite((char*) i_hdr,phdr_size, 1, dest_elf) != 1)
	 {
	   printf("Write Phdr header failed! ret(%d) - errno(%d)\n", ret, errno);
	   return errno;
	 }

    
     src_file_offset = src_file_offset + phdr_size;
	 dest_file_offset = dest_file_offset + phdr_size;

  }
  
  
 return 0;
}

/*===========================================================================
FUNCTION  PARSE_SCL_FILE_FOR_HASH_SEGMENT

DESCRIPTION
    Parses the input SCL file to get HASH Segment Size

DEPENDENCIES
    None.

RETURN VALUE
    !=0 on error
    else 0

SIDE EFFECTS

===========================================================================*/
int parse_scl_file_for_hash_segment(FILE *fp)
{
  char buf[100], hash_memsz[100];
  int counter = 0;
  char delims[] = " ";
  char *result = NULL;
  char *end;
  unsigned int i = 0;
  

  while(!feof(fp))
  {
    /* read a line from the amss scl file. */
    if ((fgets(buf, 100, fp) == NULL) && (!feof (fp)))
    {
      perror("parse_scl_file: fgets returned NULL.\n");
      return errno;
    }

    DEBUG("buf = %s\n", buf, 0,0);

    if ((strstr(buf, "HASH_SEGMENT_NOTPAGED")) && (!strstr(buf, "#")) && (!strstr(buf, "//")) && (!strstr(buf, "/*")))
    {
      /* DEBUG("counter = %d\tbuf = %s\tlast_read = %s\n",counter, buf, last_read); */
        result = strtok(buf, delims );
        for( i=0 ;result != NULL;result = strtok( NULL, delims ),i++) {
            DEBUG( "result is \"%s\"\n", result,0,0);
			if(i==2){
               strcpy(hash_memsz,result);
			}
           
        }

        hash_seg_memsz = strtol(hash_memsz,&end,0);

		DEBUG("The hash size is %x \n",hash_seg_memsz,0,0);
     
	}
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
   /* argv[1] = input scl file
    * argv[2] = input elf
	* argv[3] = output elf
    */

  segments_t segment_info = {0};          /* table containing segment info */

  char *in_elf_file, *in_scl_file, *out_elf_file;
  FILE *in_elf, *in_scl, *out_elf;
  int i=0, ret;

  struct stat fs;
  struct elf_file_info src_file;


  if((argc == 5) && strcmp(argv[1], "-d") == 0)
  {
	/* Enable Debug Mode */
    printf("Debug mode is enabled \n");
    debug_print = true;
    /* shift argv by 1 */
    i=1;
  }
  else if(argc != 4)
  {
    print_usage(argv[0]);
    exit(-1);
  }

    
    in_scl_file = argv[1+i];
	in_elf_file = argv[2+i];
	out_elf_file = argv[3+i];
    

  OPENFILE(in_scl_file, in_scl, "rb");
  printf("Parse scatter load file %s\n", in_scl_file);

  if(parse_scl_file_for_hash_segment(in_scl))
  {
    perror("Error parsing scl file");
    exit(-1);
  }

  fclose(in_scl);

     OPENFILE(in_elf_file, in_elf, "rb");
	 OPENFILE(out_elf_file, out_elf, "wb");

	
    if(modify_offsets_after_hash_segment(in_elf,out_elf)){
         perror("Error Changing ELF program Header p_flags");
         exit(-1);
    }
    fclose(in_elf);
	fclose(out_elf);
    return 0;

}

