/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

          Modify ELF file for Progressive Boot

GENERAL DESCRIPTION

  This file contains functions necessary to modify the .elf file created
   by the build process to make it suitable for Progressive Boot.
  
EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  

  Copyright (c) 2004 by QUALCOMM Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/build/ms/pboot_melf.cc#1 $ $DateTime: 2007/10/25 15:24:48 $ $Author: milanp $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/04/04   rp     Created.
===========================================================================*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

typedef long           int32;

typedef unsigned long           word32;
typedef unsigned short          word16;
typedef unsigned char           word8;
typedef word32                  word;
typedef unsigned int            Size_t;
typedef int                     sSize_t;
typedef unsigned short          uint16;
typedef unsigned long           uint32;

/* The first 52 bytes of the file is the ELF header  */

#define ELFINFO_MAGIC_SIZE (16)

typedef struct
{
  unsigned char	e_ident[ELFINFO_MAGIC_SIZE];	
                             /* Magic number and other info         */
  uint16	e_type;			       /* Object file type                    */
  uint16	e_machine;	       /* Architecture                        */
  uint32	e_version;	       /* Object file version                 */
  uint32	e_entry;		       /* Entry point virtual address         */
  uint32	e_phoff;		       /* Program header table file offset    */
  uint32	e_shoff;		       /* Section header table file offset    */
  uint32	e_flags;		       /* Processor-specific flags            */
  uint16	e_ehsize;		       /* ELF header size in bytes            */
  uint16	e_phentsize;	     /* Program header table entry size     */
  uint16	e_phnum;		       /* Program header table entry count    */
  uint16	e_shentsize;	     /* Section header table entry size     */
  uint16	e_shnum;		       /* Section header table entry count    */
  uint16	e_shstrndx;		     /* Section header string table index   */
} Elf32_Ehdr;

/* Fields in the e_ident array.  The ELFINFO_*_INDEX macros are 
 * indices into the array.  The macros under each ELFINFO_* macro
 * is the values the byte may have.  
 */

#define ELFINFO_MAG0_INDEX		0     /* File identification byte 0 index */
#define ELFINFO_MAG0	        0x7f  /* Magic number byte 0              */

#define ELFINFO_MAG1_INDEX		1		  /* File identification byte 1 index */
#define ELFINFO_MAG1		'E'		      /* Magic number byte 1              */

#define ELFINFO_MAG2_INDEX		2		  /* File identification byte 2 index */
#define ELFINFO_MAG2		'L'		      /* Magic number byte 2              */

#define ELFINFO_MAG3_INDEX		3	    /* File identification byte 3 index */
#define ELFINFO_MAG3		'F'	        /* Magic number byte 3              */

#define ELFINFO_CLASS_INDEX	  4     /* File class byte index            */

/* ELF Object Type */
#define ELF_CLASS_32	1		          /* 32-bit objects                   */
 

/* Version information */
#define ELFINFO_VERSION_INDEX	6     /* File version byte index          */

#define ELF_VERSION_CURRENT	  1     /* Current version                  */

/* Flag values used to specify the program segment types */
#define P_FLAGS_NON_PAGED     0
#define P_FLAGS_PAGED         1
#define P_FLAGS_NOT_LOADED    2

/* Program segment header.  */

typedef struct
{
  uint32	p_type;			/* Segment type */
  uint32	p_offset;		/* Segment file offset */
  uint32	p_vaddr;		/* Segment virtual address */
  uint32	p_paddr;		/* Segment physical address */
  uint32	p_filesz;		/* Segment size in file */
  uint32	p_memsz;		/* Segment size in memory */
  uint32	p_flags;		/* Segment flags */
  uint32	p_align;		/* Segment alignment */
} Elf32_Phdr;


#define MAX(a,b) ((a) > (b) ? (a) : (b))

#ifdef WIN32
#define FILE_OPEN_RDONLY_FLAGS (O_RDONLY|O_BINARY)
#define FILE_OPEN_RDWR_FLAGS (O_RDWR|O_BINARY)
#else
#define FILE_OPEN_RDONLY_FLAGS O_RDONLY
#define FILE_OPEN_RDWR_FLAGS O_RDWR
#endif

#define ELF_HDR_SIZE 52
#define ELF_BLOCK_ALIGN 0x1000
#define BLOCK_SIZE (32*1024)
#define PHDRS_COUNT 32
#define DULPICATE_HDR_SIZE 0x800
#define QPST_DATA_OFFSET   0x20
#define QPST_DATA_SIZE     30
static char buffer[BLOCK_SIZE];

struct elf_file_info
{
   int        fd;
   Elf32_Ehdr elf_hdr;

   //index 0 is used for initial calculation only
   Elf32_Phdr phdr_table[PHDRS_COUNT+1];
};

struct elf_file_info src_file;
struct elf_file_info dest_file;

/*
** Resolution is always a power of 2
*/
inline word32 offset_align
(
   word32 position,
   word32 resolution
)
{   
   if ((position & (resolution - 1)) != 0)
   {
      return (position & ~(resolution - 1)) + resolution;      
   }

   return position;
}




int main(int argc, char** argv)
{
   // Open the file
   char* filename;
   int i, size;
   int block_count, block_remainder;
   word32 total_added_padding = 0;
   word32 initial_padding = 0;
   word32 phdrs_bcount;
   int ret;


   printf("\n");
   
  if (argc == 1)
  {
     printf("Provide a filename!\n");
     return -1;
  }

    filename = argv[1];
    src_file.fd = open(filename, FILE_OPEN_RDONLY_FLAGS);
   if (src_file.fd == -1)
   {
      printf("Couldn't open file '%s'.\n", filename);
      return -1;
   }
   
   /*
   ** Verify the ELF header
   */
   size = read(src_file.fd, (char*) &(src_file.elf_hdr), ELF_HDR_SIZE);
   if (size != ELF_HDR_SIZE)
   {
      printf("Couldn't read elf header\n");
      return -1;
   }
   
   if (src_file.elf_hdr.e_ident[ELFINFO_MAG0_INDEX] != ELFINFO_MAG0 ||
       src_file.elf_hdr.e_ident[ELFINFO_MAG1_INDEX] != ELFINFO_MAG1 ||
       src_file.elf_hdr.e_ident[ELFINFO_MAG2_INDEX] != ELFINFO_MAG2 ||
       src_file.elf_hdr.e_ident[ELFINFO_MAG3_INDEX] != ELFINFO_MAG3)
   {
      return 1;
   }
   

   if (src_file.elf_hdr.e_ident[ELFINFO_CLASS_INDEX] != ELF_CLASS_32)
   {
      return 1;
   }
    
   if(src_file.elf_hdr.e_ident[ELFINFO_VERSION_INDEX] != ELF_VERSION_CURRENT)
   {
      return 1;
   }

   /*----------------------------------------------------------------------
   ** Verify there is no linkview in the file
   ----------------------------------------------------------------------*/
   if (src_file.elf_hdr.e_shoff != 0)
   {
      printf("Debug info and section headers must be stripped first\n\n");
      printf("Run the following command:\n");
      printf("   ADS1.2\n");
      printf("      fromelf -nodebug -nolinkview -elf <inputfile> -output <outputfile>\n\n");
      printf("   RVCT2.1\n");
      printf("      fromelf --nodebug --nolinkview -elf <inputfile> -output <outputfile>\n\n");
      return 1;
   }
   
   /*
   ** Open the output file
   */
   char* filename_new = "out.elf";
   unlink(filename_new);
   dest_file.fd = open( filename_new, O_CREAT|FILE_OPEN_RDWR_FLAGS,
                        S_IREAD|S_IWRITE );
   if (dest_file.fd == -1)
   {
      printf("Couldn't open output file '%s'.\n", filename_new);
      return -1;
   }

   /*
   ** Copy the source elf header to the destination
   */
   dest_file.elf_hdr = src_file.elf_hdr;

   
   /*----------------------------------------------------------------------
   ** See if the program headers need to be moved
   ----------------------------------------------------------------------*/
   printf("Checking location of Program Headers:\n");

   if (src_file.elf_hdr.e_phoff != ELF_HDR_SIZE)
   {      
      phdrs_bcount = src_file.elf_hdr.e_phnum*src_file.elf_hdr.e_phentsize;
      
      initial_padding = 
      (
         offset_align(ELF_HDR_SIZE + phdrs_bcount, ELF_BLOCK_ALIGN)
         -
         (ELF_HDR_SIZE + phdrs_bcount)
      );

      printf("   Moving (%d) PHDRS from offset (%d) to (%d)\n",
             src_file.elf_hdr.e_phnum,
             src_file.elf_hdr.e_phoff,
             ELF_HDR_SIZE);
      
      printf("   (%d) bytes ELF Hdr followed by (%d) bytes for PHDRS\n",
             ELF_HDR_SIZE, phdrs_bcount);
      
      printf("   ---> Will add (%d) padding bytes up to the first segment\n\n",
             initial_padding);
      
      dest_file.elf_hdr.e_phoff = ELF_HDR_SIZE;
   }

   dest_file.elf_hdr.e_flags &= ~0x4;  //SYMARESORTED
   dest_file.elf_hdr.e_flags &= ~0x10; //MAPSYMFIRST
   
   /*----------------------------------------------------------------------
   ** Write the new ELF header
   ----------------------------------------------------------------------*/
   ret = write(dest_file.fd, (char*) &dest_file.elf_hdr, ELF_HDR_SIZE);
   if ((unsigned int)ret != dest_file.elf_hdr.e_phoff)
   {
      printf("ret(%d) - errno(%d)\n", ret, errno);
      return 1;
   }

   /*----------------------------------------------------------------------
   **The first segment will be put at this offset
   ----------------------------------------------------------------------*/
   memset(&dest_file.phdr_table[0], 0, sizeof(Elf32_Phdr));
   dest_file.phdr_table[0].p_offset = offset_align
   (
      ELF_HDR_SIZE + phdrs_bcount, ELF_BLOCK_ALIGN
   );

   
   /*----------------------------------------------------------------------
   ** Go to the start of the program header table in input file
   ----------------------------------------------------------------------*/
   ret = lseek(src_file.fd, src_file.elf_hdr.e_phoff, SEEK_SET);
   if ((unsigned int)ret != src_file.elf_hdr.e_phoff)
   {
      printf("Couldn't seek input file '%d'- errno(%d).\n", ret, errno);
      return 1;
   }

   /*----------------------------------------------------------------------
   ** Write the new program headers in the output file,
   ** updating the file offset of each segment to have them
   ** individually aligned on a ELF_BLOCK_ALIGN boundary.
   ----------------------------------------------------------------------*/
   word32 file_offset;
   Elf32_Phdr *i_hdr, *o_hdr;

   printf("Writing aligned Program Headers offsets:\n");
   
   for(i=1; i<=src_file.elf_hdr.e_phnum ; i++)
   {
      word32 seg_balign;

      i_hdr = &src_file.phdr_table[i];
      o_hdr = &dest_file.phdr_table[i];

      /* Get the segment description */
      read(src_file.fd, (char*) i_hdr, src_file.elf_hdr.e_phentsize);

      /* Align this segment in relation to the previous one */
      file_offset = offset_align
      (
         MAX
         (
            dest_file.phdr_table[i-1].p_offset
            +
            dest_file.phdr_table[i-1].p_filesz
            ,
            i_hdr->p_offset
         ),
         ELF_BLOCK_ALIGN
      );


      /* Amount of bytes to the next aligment boundary */
      seg_balign =
      (
         (i==1)
         ?
         initial_padding:
         offset_align( i_hdr->p_offset, ELF_BLOCK_ALIGN) - i_hdr->p_offset
      );

      
      printf
      (
         "   Segment [%d] aligned from [%#x] to [%#x] -- [add %#d bytes]\n",
         i,
         i_hdr->p_offset,
         file_offset,
         seg_balign
      );

      total_added_padding += seg_balign;

      /* Save the values for the output file */
      *o_hdr = *i_hdr;          
      o_hdr->p_offset = file_offset;
	  if (i==1) 
	  {
		  o_hdr->p_flags = P_FLAGS_NON_PAGED;
	  }
	  else
	  {
		  o_hdr->p_flags = P_FLAGS_PAGED;
	  }

      /* Write the header to the output file */
      ret = write(dest_file.fd, (char*) o_hdr, src_file.elf_hdr.e_phentsize);
      if (ret != src_file.elf_hdr.e_phentsize)
      {
         printf("\nret(%d) - errno(%d)\n", ret, errno);
         exit(1);
      }
   }

   printf("\n"); 

 
   /*
   ** Copy the program bits
   */
   printf("Copying Segments from old to new offsets:\n");

   for(i=1; i<=src_file.elf_hdr.e_phnum ; i++)
   {
      i_hdr = &src_file.phdr_table[i];
      o_hdr = &dest_file.phdr_table[i];

      printf("   Segment [%#d] moving from [%#x] to [%#x] - Size [%#x]\n",
             i, i_hdr->p_offset, o_hdr->p_offset, o_hdr->p_filesz);

      /* Move to the begining of the segment in input file */
      ret = lseek(src_file.fd, i_hdr->p_offset, SEEK_SET);
      if ((unsigned int)ret != i_hdr->p_offset)
      {
         printf("\nCouldn't seek input file '%d'.\n", ret);
         return -1;
      }

      /* Move to the begining of the segment in output file */
      ret = lseek(dest_file.fd, o_hdr->p_offset, SEEK_SET);
      if ((unsigned int)ret != o_hdr->p_offset)
      {
         printf("\nCouldn't seek output file '%d'.\n", ret);
         return -1;
      }

      /* Calculate how many copy blocks are in the segment */
      block_count = i_hdr->p_filesz / BLOCK_SIZE;
      block_remainder = i_hdr->p_filesz - (block_count * BLOCK_SIZE);
       
      printf("      ---> [%#x * %#x + %#x] bytes\n",
             block_count, BLOCK_SIZE, block_remainder);

      /* Copy the segment to the output file */
      int j;
      unsigned int ret;
      for(j=0; j<block_count; j++)
      {
	     ret = read(src_file.fd, (char*)buffer, BLOCK_SIZE);
		 if ((unsigned int)ret != BLOCK_SIZE)
         {
            printf("\nCouldn't read from input file '%d'.\n", ret);
            return -1;
         }
         
         ret = write(dest_file.fd, buffer, BLOCK_SIZE);
         if (ret != BLOCK_SIZE)
         {
            printf("\nCouldn't write to output file '%d'.\n", ret);
            return -1;
         }
      }
       
      if (block_remainder != 0)
      {
         read(src_file.fd, buffer, block_remainder);
         write(dest_file.fd, buffer, block_remainder);
      }

   }

   printf("Duplicating Elf header Program Headers :\n");

   /* Duplicate the ELF header and program header by copying the first 2Kbytes to
      second 2K bytes in destination file 
   */   

   /* Move to the begining of  output file */
   ret = lseek(dest_file.fd, 0, SEEK_SET);
   if (ret != 0)
   {
	  printf("\nCouldn't seek output file '%d'.\n", ret);
	  return -1;
   }

   /* Read 2k bytes from the beginning of file */
   ret = read(dest_file.fd, (char*)buffer, DULPICATE_HDR_SIZE);
   if ((unsigned int)ret != DULPICATE_HDR_SIZE)
   {
	  printf("\nCouldn't read from input file '%d'.\n", ret);
	  return -1;
   }
   /* Write 2k bytes read to the current position of file */
   ret = write(dest_file.fd, (char*)buffer, DULPICATE_HDR_SIZE);
   if ((unsigned int)ret != DULPICATE_HDR_SIZE)
   {
	  printf("\nCouldn't write to output file '%d'.\n", ret);
	  return -1;
   }
   if (close(dest_file.fd) != 0)
   {
      printf("close() - errno(%d)\n", errno);
   }
   printf("\nTotal of [%#x] bytes overhead added.\n", total_added_padding);   
   printf("\n");      

   return 0;
}

