/*=============================================================================
        P R O G R E S S I V E   B O O T   E L F   U T I L I T I E S

Copyright (c) 2005 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/
/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/tools/headergen/pboot_elf_utils.c#3 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/22/06   rjain   Increased the scl parse buffer size to 500 and added a
                   #define for it and some general cleanup.
08/31/06   rjain   Modified for Simple elf support
08/08/05   ih      Created
=============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "pboot_elf_utils.h"
#include "miimageheader.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/

#define PARSE_SCL_BUFF 500      /* buffer size for parsing scl */

extern bool debug_print;        /* global debug option */

static char fbuf[PAGE_SIZE];    /* buffer for file ops */

/*=============================================================================

                              FUNCTION DEFINITIONS

=============================================================================*/


/*===========================================================================
FUNCTION  FCPY

DESCRIPTION
    Copy len bytes from in_pos in file in_fp to out_pos in file out_fp

DEPENDENCIES
    None.

RETURN VALUE
    errno on error
    else 0

SIDE EFFECTS

===========================================================================*/
int fcpy(FILE *in_fp, uint32_t in_pos, FILE *out_fp, uint32_t out_pos, size_t len)
{
  int32_t in_pos_org, out_pos_org;
  size_t remaining = len, chunk;

  in_pos_org = ftell(in_fp);
  out_pos_org = ftell(out_fp);

  /* Save current positions of fp before moving them */
  if(in_pos_org < 0 || out_pos_org < 0)
  {
    return errno;
  }

  /* Set in and out fp to correct positions */
  if(fseek(in_fp, in_pos, SEEK_SET) || fseek(out_fp, out_pos, SEEK_SET))
  {
    return errno;
  }

  /* Copy file one chunk at a time */
  chunk = sizeof(fbuf);

  while(remaining)
  {
    if(chunk > remaining) chunk = remaining;
    if(fread(fbuf, chunk, 1, in_fp) != 1) return errno;
    if(fwrite(fbuf, chunk, 1, out_fp) !=1) return errno;
    remaining-=chunk;
  }

  /* Restore file positions before returning */
  /* Set in and out fp to correct positions */
  if(fseek(in_fp, in_pos_org, SEEK_SET) || fseek(out_fp, out_pos_org, SEEK_SET))
  {
    return errno;
  }

  return 0;
}

/*===========================================================================
FUNCTION  FREADN

DESCRIPTION
    Read len bytes from in_pos of in_fp 1 PAGE at a time

DEPENDENCIES
    None.

RETURN VALUE
    errno on error
    else 0

SIDE EFFECTS

===========================================================================*/
int freadn(char *buf, FILE *in_fp, uint32_t in_pos, size_t len)
{
  int32_t in_pos_org;
  size_t remaining = len, chunk;

  /* Save current positions of fp before moving them */
  if((in_pos_org = ftell(in_fp)) < 0)
  {
    return errno;
  }

  /* Set in_fp to correct positions */
  if(fseek(in_fp, in_pos, SEEK_SET))
  {
    return errno;
  }

  /* Copy file one chunk at a time */
  chunk = PAGE_SIZE;

  while(remaining)
  {
    if(chunk > remaining) chunk = remaining;
    if(fread(buf, chunk, 1, in_fp) != 1) return errno;
    buf+=chunk;
    remaining-=chunk;
  }

  /* Restore file position before returning */
  if(fseek(in_fp, in_pos_org, SEEK_SET))
  {
    return errno;
  }

  return 0;
}

/*===========================================================================
FUNCTION  FPAD

DESCRIPTION
    Pad len bytes starting from out_pos in out_fp

DEPENDENCIES
    None.

RETURN VALUE
    errno on error
    else 0

SIDE EFFECTS

===========================================================================*/
int fpad(FILE *out_fp, uint32_t out_pos, size_t len)
{
  int32_t out_pos_org;
  size_t remaining = len, chunk;

  /* Save current position of fp before moving it */
  if((out_pos_org = ftell(out_fp)) < 0)
  {
    return errno;
  }

  /* Set and out fp to correct positions */
  if(fseek(out_fp, out_pos, SEEK_SET))
  {
    return errno;
  }

  /* Pad file one chunk at a time */
  if(len < sizeof(fbuf))
  {
    chunk = len;
  }
  else
  {
    chunk = sizeof(fbuf);
  }

  memset(fbuf, 0, chunk);

  while(remaining)
  {
    if(chunk > remaining) chunk = remaining;
    if(fwrite(fbuf, chunk, 1, out_fp) !=1) return errno;
    remaining-=chunk;
  }

  /* Restore file position before returning */
  if(fseek(out_fp, out_pos_org, SEEK_SET))
  {
    return errno;
  }

  return 0;
}

/*===========================================================================
FUNCTION  VERIFY_ELF_HEADER

DESCRIPTION
    verify elf header

DEPENDENCIES
    None.

RETURN VALUE
    ERROR on error
    else 0

SIDE EFFECTS

===========================================================================*/
int verify_elf_header(Elf32_Ehdr *elf_hdr)
{
  if (elf_hdr->e_ident[ELFINFO_MAG0_INDEX] != ELFINFO_MAG0 ||
      elf_hdr->e_ident[ELFINFO_MAG1_INDEX] != ELFINFO_MAG1 ||
      elf_hdr->e_ident[ELFINFO_MAG2_INDEX] != ELFINFO_MAG2 ||
      elf_hdr->e_ident[ELFINFO_MAG3_INDEX] != ELFINFO_MAG3)
  {
    return ERROR;
  }


  if (elf_hdr->e_ident[ELFINFO_CLASS_INDEX] != ELF_CLASS_32)
  {
    return ERROR;
  }

  if(elf_hdr->e_ident[ELFINFO_VERSION_INDEX] != ELF_VERSION_CURRENT)
  {
    return ERROR;
  }

  return 0;
}


/*===========================================================================
  FUNCTION  PROCESS_ELF_HEADER

  DESCRIPTION
  Read and verify the source ELF header.

  DEPENDENCIES
  None

  RETURN VALUE
  !=0 on error
  else 0

  SIDE EFFECTS
  None
  ===========================================================================*/
int process_elf_header
(
  FILE *src_elf,
  struct elf_file_info *src_file
)
{
  int ret, size;

  /* Seek to start of file */
  ret = fseek(src_elf, 0, SEEK_SET);
  if (ret)
  {
    printf("Cannot fseek. ret(%d) - errno(%d)\n", ret, errno);
    return errno;
  }

  /* Read source file ELF header */
  size = fread(&src_file->elf_hdr, ELF_HDR_SIZE, 1, src_elf);
  if (size != 1)
  {
    printf("Could not read elf header\n");
    return errno;
  }

  /* Verify the ELF magic numbers, class and version number */
  ret = verify_elf_header(&src_file->elf_hdr);
  if (ret)
  {
    printf("ELF header fails verification\n");
    return errno;
  }
  return (0);
}


/*===========================================================================

FUNCTION    OFFSET_ALIGN

DESCRIPTION
    Helper to pboot_melf. Resolution is always a power of 2


DEPENDENCIES
    None.

RETURN VALUE
    errno or ERROR   On error
    0                Success

SIDE EFFECTS
    None.

===========================================================================*/
inline uint32_t offset_align
(
 uint32_t position,
 uint32_t resolution
)
{
  if ((position & (resolution - 1)) != 0)
  {
    return (position & ~(resolution - 1)) + resolution;
  }

  return position;
}

/*===========================================================================
FUNCTION  READ_ELF_FILE

DESCRIPTION
    PRINTS THE ELF HEADER AND PROGRAM HEADERS INFO OF A GIVEN FILE STREAM

DEPENDENCIES
    None.

RETURN VALUE
    errno on error
    else 0

SIDE EFFECTS

===========================================================================*/

int read_elf_file(FILE *src_elf)
{
  struct elf_file_info src_file;
  int size, ret, i;
  Elf32_Phdr *i_hdr;
  Elf32_Ehdr *elf_hdr;
  uint32_t phdr_size = 0, file_offset = 0;


  char *x;

  src_file.fd = fileno(src_elf);

  ret = fseek(src_elf, 0, SEEK_SET);
  if (ret)
  {
    printf("Cannot fseek. ret(%d) - errno(%d)\n", ret, errno);
    return errno;
  }

  /*
   ** Verify the ELF header
   */
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
    return errno;
  }
  else
  {
    //Print the Elf Header here
  }

  ret = fseek(src_elf, ELF_HDR_SIZE, SEEK_SET);
  if (ret)
  {
    printf("Cannot fseek. ret(%d) - errno(%d)\n", ret, errno);
	return errno;
  }

  phdr_size = src_file.elf_hdr.e_phentsize;
  file_offset = src_file.elf_hdr.e_phoff;

  if(src_file.elf_hdr.e_phnum > UINT16_MAX)
  {
	printf("src_file.elf_hdr.e_phnum is out of limit \n");
	return errno;
  }
  
  DEBUG("Found %d program headers\n",src_file.elf_hdr.e_phnum, 0, 0);
  DEBUG("Program header offset in bytes: %d\n", file_offset, 0, 0);
  DEBUG("Size of each program header in bytes: %d\n\n", phdr_size, 0, 0);

  // Print the Program Header info here
  for(i=1; i<=src_file.elf_hdr.e_phnum ; i++)
  {
    i_hdr = &src_file.phdr_table[i];

    ret = fseek(src_elf, file_offset, SEEK_SET);
    if (ret)
    {
      printf("Cannot fseek. ret(%d) - errno(%d)\n", ret, errno);
      return errno;
    }

	if(phdr_size > UINT32_MAX)
    {
	  printf("phdr_size is either zero or greater than the limit\n");
	  return errno;
    }
	
    /* Get the segment description */
    if(fread(i_hdr, phdr_size, 1, src_elf) != 1)
    {
      printf("Unable to read program header!\n");
      return errno;
    }

    DEBUG("Segment[%d]: offset[0x%x] vaddr[0x%x]", i, i_hdr->p_offset, i_hdr->p_vaddr);
    DEBUG(" type[0x%x] size[0x%x] memsz[0x%x]\n", i_hdr->p_flags, i_hdr->p_filesz, i_hdr->p_memsz);

    file_offset = file_offset + phdr_size;

  }

  return 0;


}

/*===========================================================================
FUNCTION  READ_HEADER_FILE

DESCRIPTION
    Reads the Image Header and Prints it.

DEPENDENCIES
    None.

RETURN VALUE
    errno on error
    else 0

SIDE EFFECTS

===========================================================================*/
int read_header_file(FILE *img_hd)
{
  mi_boot_image_header_type header;
  uint32_t boot_img_hdr_size = sizeof(mi_boot_image_header_type);
  int size, ret;

  ret = fseek(img_hd, 0, SEEK_SET);
  if (ret)
  {
    printf("Cannot fseek. ret(%d) - errno(%d)\n", ret, errno);
    return errno;
  }

  size = fread(&header,boot_img_hdr_size, 1,img_hd);
  if (size != 1)
  {
    printf("Couldn't Read Image Header\n");
    return errno;
  }

  DEBUG("\n Image Id[%d]: Version Number[0x%x] Image Src[0x%x]\n", header.image_id , header.header_vsn_num , header.image_src);
  DEBUG("Image Dest Ptr[0x%x] image_size[0x%x] memsz[0x%x]\n", header.image_dest_ptr, header.image_size, header.code_size);
  DEBUG("Signature_ptr[0x%x] signature_size[0x%x]\n", header.signature_ptr, header.signature_size, 0);
  DEBUG("cert_chain_ptr[0x%x] cert_chain_size[0x%x]\n", header.cert_chain_ptr, header.cert_chain_size, 0);

  return 0;

}

/*===========================================================================

FUNCTION    IS_PAGEABLE

DESCRIPTION

    This is a helper function to to determine if the segment represented by
    the string is pageable, not-pageable, not-used, RO, RW, ZI, AMSS or L4 segment.

DEPENDENCIES
    None.

RETURN VALUE


SIDE EFFECTS
    None.

===========================================================================*/
uint32 is_pageable
(
 char *seg_info  /* string containing segment information */
)
{
  segment_flag_t rtnval; /* return value. */
  char *s;

  /* argument checks. */
  if (seg_info == NULL)
  {
    return ERROR;
  }
  /* determine if the segment is pageable or not-pageable. */
  if (s = strstr (seg_info, NOTPAGEABLE))
  {
    /* Makesure we do not search the segment name for "RO" */
    if(strstr (seg_info, RO))
    {
      DEBUG("%s is NOTPAGEABLE/RO.\n", seg_info,0,0);
      rtnval = MI_PBT_ELF_AMSS_NON_PAGED_RO_SEGMENT;
    }
    else if (strstr (seg_info, CODE ))
    {
      DEBUG("%s is NOTPAGEABLE/RO.\n", seg_info,0,0);
      rtnval = MI_PBT_ELF_AMSS_NON_PAGED_RO_SEGMENT;
    }
    else if(strstr (seg_info, ZI))
    {
      if(strstr (seg_info, SWAP_POOL))
      {
	 if(strstr (seg_info, POOL_INDEX_0))
         {
            DEBUG("%s is NOTPAGEABLE/ZI/SWAP_POOL/INDEX0.\n", seg_info,0,0);
            rtnval = MI_PBT_ELF_SWAP_POOL_NON_PAGED_ZI_SEGMENT_INDEX0;
         }
         else
         {
            DEBUG("%s is NOTPAGEABLE/ZI/SWAP_POOL/INDEX1.\n", seg_info,0,0);
            rtnval = MI_PBT_ELF_SWAP_POOL_NON_PAGED_ZI_SEGMENT_INDEX1;
         }
      } 
      else
      {
      DEBUG("%s is NOTPAGEABLE/ZI.\n", seg_info,0,0);
      rtnval = MI_PBT_ELF_AMSS_NON_PAGED_ZI_SEGMENT;
      }
    }
    else if(strstr (seg_info,NOTUSED))
    {
      DEBUG("%s is NOTPAGEABLE/NOTUSED.\n", seg_info,0,0);
      rtnval = MI_PBT_ELF_AMSS_NON_PAGED_NOTUSED_SEGMENT;
    }
    else if(strstr (seg_info,SHARED))
    {
      DEBUG("%s is NOTPAGEABLE/SHARED.\n", seg_info,0,0);
      rtnval = MI_PBT_ELF_AMSS_NON_PAGED_SHARED_SEGMENT;
    }
    else if(strstr (seg_info,HASHTBL))
    {
      DEBUG("%s is NOTPAGEABLE/HASH.\n", seg_info,0,0);
      rtnval = MI_PBT_ELF_HASH_SEGMENT;
    }
    else if(strstr (seg_info,BOOT_SEGMENT))
    {
      DEBUG("%s is NOTPAGEABLE/BOOT SEGMENT.\n", seg_info,0,0);
      rtnval = MI_PBT_ELF_BOOT_SEGMENT;
    }
    else if(strstr (seg_info,L4BSP))
    {
      DEBUG("%s is NOTPAGEABLE/L4BSP.\n", seg_info,0,0);
      rtnval = MI_PBT_ELF_NON_PAGED_L4BSP_SEGMENT;
    }
    else
    {
      /* Assume the Default to be RW */
      DEBUG("%s is NOTPAGEABLE/RW.\n", seg_info,0,0);
      rtnval = MI_PBT_ELF_AMSS_NON_PAGED_RW_SEGMENT;
    }

  }

  else if (s = strstr (seg_info, PAGEABLE))
  {
    if(strstr (seg_info, RO) || strstr (seg_info, CODE))
    {
      if(strstr (seg_info, SWAPABLE))
      {
	 if(strstr (seg_info, POOL_INDEX_0))
         {
            DEBUG("%s is PAGEABLE/RO/SWAPABLE/INDEX0.\n", seg_info, 0,0);
            rtnval = MI_PBT_ELF_SWAPPED_PAGED_RO_SEGMENT_INDEX0;
         }
         else
         {
            DEBUG("%s is PAGEABLE/RO/SWAPABLE/INDEX1.\n", seg_info, 0,0);
            rtnval = MI_PBT_ELF_SWAPPED_PAGED_RO_SEGMENT_INDEX1;
         }
      } 
      else
      {
        DEBUG("%s is PAGEABLE/RO.\n", seg_info, 0,0);
        rtnval = MI_PBT_ELF_AMSS_PAGED_RO_SEGMENT;
      }
    }
    else if(strstr (seg_info, ZI))
    {
      DEBUG("%s is PAGEABLE/ZI.\n", seg_info,0,0);
      rtnval = MI_PBT_ELF_AMSS_PAGED_ZI_SEGMENT;
    }
    else if(strstr (seg_info,NOTUSED))
    {
      DEBUG("%s is PAGEABLE/NOTUSED.\n", seg_info,0,0);
      rtnval = MI_PBT_ELF_AMSS_PAGED_NOTUSED_SEGMENT;
    }
    else if(strstr (seg_info,SHARED))
    {
      DEBUG("%s is PAGEABLE/SHARED.\n", seg_info,0,0);
      rtnval = MI_PBT_ELF_AMSS_PAGED_SHARED_SEGMENT;
    }
    else if(strstr (seg_info,L4BSP))
    {
      DEBUG("%s is PAGEABLE/L4BSP.\n", seg_info,0,0);
      rtnval = MI_PBT_ELF_PAGED_L4BSP_SEGMENT;
    }
    else
    {
      /* Assume the Default to be RW */
      DEBUG("%s is PAGEABLE/RW.\n", seg_info, 0,0);
      rtnval = MI_PBT_ELF_AMSS_PAGED_RW_SEGMENT;
    }
  }

  else
  {
   printf("The segment name is Wrongly Defined in SCL file. Please follow the convention");
   exit(-1);
  }

  return rtnval;

} /* is_pageable () */


/*===========================================================================
FUNCTION  DEBUG_PRINT_SEGMENTS

DESCRIPTION
    Debug function to print out the segments parsed from the scl file

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void debug_print_segments(segments_t *seg)
{
  int i;
  for(i=0; i<seg->num_segments; i++)
  {
    DEBUG("Seg %d - flag: 0x%x \n", i, seg->segments[i].flag,0);
  }
}

/*===========================================================================

FUNCTION    PARSE_SCL_FILE

DESCRIPTION

    This function is used to read our amss scl file and determine
    whether a segment is pageable, non-pageable or not-used. It will also
    determine the starting address for every segment in our amss scl file.

DEPENDENCIES
    None.

RETURN VALUE
    errno or ERROR   On error
    0                Success

SIDE EFFECTS
    None.

===========================================================================*/
int parse_scl_file(FILE *fp, segments_t *segs)
{
  char buf[PARSE_SCL_BUFF], last_read[PARSE_SCL_BUFF];
  int counter = 0;

  while(!feof(fp))
  {
    /* save the last line read from the input file. */
    strcpy (last_read, buf);

    /* read a line from the amss scl file. */
    if ((fgets(buf, PARSE_SCL_BUFF, fp) == NULL) && (!feof (fp)))
    {
      perror("parse_scl_file: fgets returned NULL.\n");
      return errno;
    }

    //DEBUG("buf = %s\n", buf, 0,0);

    /* We look for the symbol "{" to determine the line to read.
    ** Make sure nested { } are skipped.
    */
    if ((strstr(buf, "{")) && (counter++ == 0))
    {
      //DEBUG("counter = %d\tbuf = %s\tlast_read = %s\n",
      //        counter, buf, last_read);

      if(segs->num_segments == MAX_SEGMENTS)
        return ERROR;

      segs->segments[segs->num_segments].flag = is_pageable (last_read);
      sscanf (last_read, "%s%x", buf, &(segs->segments[segs->num_segments].start_addr));

      /* Don't increment for now */
      DEBUG( "Segment:%d  Start addr: 0x%x flags:0x%x\n",
             segs->num_segments,
             segs->segments[segs->num_segments].start_addr,
             segs->segments[segs->num_segments].flag
           );

      segs->num_segments++;
    }
    else if (strstr(buf, "}"))
    {
      //DEBUG("counter = %d\tbuf = %s\n", counter, buf,0);
      counter--;
    }

  }

  debug_print_segments(segs);
  return 0;
}


