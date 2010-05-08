/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

          Modify ELF file for Progressive Boot

GENERAL DESCRIPTION

  This file contains functions necessary to modify the .elf file created
  by the build process to make it suitable for Progressive Boot.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2004-2007 by QUALCOMM Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc6055/tools/headergen/pboot_melf.c#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/15/06   anb    Made changes to handle the new ELF layout.
01/23/05   chs    Added argument checks.
01/16/05   chs    Added pboot_read_ehdr (), pboot_update_ehdr (),
                  pboot_read_ephdr () and pboot_update_ephdr ().
01/12/05   chs    Added pboot_read_phdr (), pboot_update_phdr (),
                  pboot_disp_phdr () and pboot_dispall_phdr ().
01/11/05   chs    Modified to work with C compiler.
11/04/04   rp     Created.
===========================================================================*/

#include "pboot_melf.h"


#undef DBUG_MELF

/* Fields in the e_ident array.  The ELFINFO_*_INDEX macros are
 * indices into the array.  The macros under each ELFINFO_* macro
 * is the values the byte may have.
 */

#define ELFINFO_MAG0_INDEX      0     /* File identification byte 0 index */
#define ELFINFO_MAG0            0x7f  /* Magic number byte 0              */

#define ELFINFO_MAG1_INDEX      1     /* File identification byte 1 index */
#define ELFINFO_MAG1        'E'       /* Magic number byte 1              */

#define ELFINFO_MAG2_INDEX      2     /* File identification byte 2 index */
#define ELFINFO_MAG2        'L'       /* Magic number byte 2              */

#define ELFINFO_MAG3_INDEX      3     /* File identification byte 3 index */
#define ELFINFO_MAG3        'F'       /* Magic number byte 3              */

#define ELFINFO_CLASS_INDEX   4        /* File class byte index            */

/* ELF Object Type */
#define ELF_CLASS_32    1              /* 32-bit objects                   */


/* Version information */
#define ELFINFO_VERSION_INDEX   6     /* File version byte index          */

#define ELF_VERSION_CURRENT   1     /* Current version                  */

#define ELF_BLOCK_ALIGN 0x1000
#define BLOCK_SIZE (32*1024)
#define PHDRS_COUNT 32
#define DULPICATE_HDR_SIZE 0x800
#define QPST_DATA_OFFSET   0x20
#define QPST_DATA_SIZE     30
static char buffer[BLOCK_SIZE];

struct elf_file_info src_file;
struct elf_file_info dest_file;

/*
** Resolution is always a power of 2
*/
inline uint32 offset_align
(
   uint32 position,
   uint32 resolution
)
{
   if ((position & (resolution - 1)) != 0)
   {
      return (position & ~(resolution - 1)) + resolution;
   }

   return position;
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

FUNCTION  PBOOT_UPDATE_PHDR

DESCRIPTION

    This function is used to update the program header in our elf file.
    The program header number to update is passed as an argument.
    A value of -1 for the program header number will result in a new
    program header being generated.

DEPENDENCIES
    None.

RETURN VALUE
    ERROR   if invalid arguments were passed or if pboot_update_ephdr () or
            pboot_read_ephdr () returns an error else
    OK

SIDE EFFECTS

===========================================================================*/

uint32 pboot_update_phdr
(
    char *filename,  /* filename of the elf file. */
    uint32 phnum,    /* program header number to update. */
    Elf32_Phdr *phdr /* program header. */
)
{
    elf_prog_header ephdr;  /* elf and program header. */

        /* argument checks. */
    if ((filename == NULL) || (phnum = 0) || (phdr == NULL))
    {
        return ERROR;
    }
        /* read the elf and program header. */
    if (pboot_read_ephdr (filename, &ephdr) == ERROR)
    {
        return ERROR;
    }

	if ((phnum - 1) > MI_PBT_MAX_SEGMENTS)
	{
	    return ERROR;
	}
        /* copy the program header. */
    memcpy (&(ephdr.phdr_table[phnum-1]), phdr, sizeof (Elf32_Phdr));

        /* update the program header. */
    if (pboot_update_ephdr (filename, &ephdr) == ERROR)
    {
        return ERROR;
    }

    return OK;

}   /* pboot_update_phdr () */


/*===========================================================================

FUNCTION  PBOOT_READ_PHDR

DESCRIPTION

    This function is used to read the program header in our elf file.
    The program header number to read is passed as an argument.

DEPENDENCIES
    None.

RETURN VALUE
    ERROR   if invalid arguments were passed or if pboot_read_ephdr ()
            returns an error else
    OK

SIDE EFFECTS

===========================================================================*/

uint32 pboot_read_phdr
(
    char *filename,  /* filename of the elf file. */
    uint32 phnum,    /* program header number to read. */
    Elf32_Phdr *phdr /* program header buffer. */
)
{
    elf_prog_header ephdr; /* elf and program header. */

        /* argument checks. */
    if ((filename == NULL) || (phdr == NULL))
    {
        return ERROR;
    }
        /* read the elf and program header. */
    if (pboot_read_ephdr (filename, &ephdr) == ERROR)
    {
        return ERROR;
    }

    if (ephdr.elf_hdr.e_phnum < (phnum + 1))
    {
        printf("pboot_read_phdr: Invalid Program Header number.\n");
        return ERROR;
    }
        /* copy the program header */
    memcpy (phdr, &(ephdr.phdr_table[phnum-1]), sizeof (Elf32_Phdr));

    return OK;

}   /* pboot_read_phdr () */


/*===========================================================================

FUNCTION  PBOOT_UPDATE_EPHDR

DESCRIPTION

    This function is used to update the elf and program header in our elf
    file.

DEPENDENCIES
    None.

RETURN VALUE
    ERROR   if invalid arguments were passed or if
            file open or file write returns as error else
    OK

SIDE EFFECTS

===========================================================================*/

uint32 pboot_update_ephdr
(
    char *filename,   /* filename of the elf file. */
    elf_prog_header *ephdr /* elf and program header. */
)
{
    int fd; /* file descriptor. */
    uint32 ret; /* number of bytes written. */

        /* argument checks. */
    if ((ephdr == NULL) || (filename == NULL))
    {
        return ERROR;
    }
        /* open the elf file in write mode. */
    if ((fd = open (filename, FILE_OPEN_RDWR_FLAGS)) == ERROR)
    {
       printf("pboot_update_phdr: Couldn't open file '%s'.\n", filename);
       return ERROR;
    }
        /* update the elf and program header. */
    ret = write (fd, (char *) ephdr, sizeof (elf_prog_header));
    if (ret != sizeof (elf_prog_header))
    {
       printf("ret(%d) - errno(%d)\n", ret, errno);
       close (fd);
       return ERROR;
    }
        /* close the elf file. */
    close (fd);

    return OK;

}   /* pboot_update_ephdr () */


/*===========================================================================

FUNCTION  PBOOT_READ_EPHDR

DESCRIPTION

    This function is used to read the elf and program header in our elf file.

DEPENDENCIES
    None.

RETURN VALUE
    ERROR   if invalid arguments were passed or
            if file open or file read returns as error else
    OK

SIDE EFFECTS

===========================================================================*/
uint32 pboot_read_ephdr
(
    char *filename,  /* filename of the elf file. */
    elf_prog_header *ephdr /* elf and program header buffer. */
)
{
    int fd; /* file descriptor. */
    uint32 ret; /* number of bytes written. */
    uint32 index = 0;
    uint32 phdr_entry_size = 0;
    uint32 phdr_num_segment = 0;
    uint32 phdr_size = 0;
    uint32 phdr_offset = 0;

    /* argument checks. */
    if ((filename == NULL) || (ephdr == NULL))
    {
      return ERROR;
    }
        /* open the file in read mode. */
    if ((fd = open (filename, FILE_OPEN_RDONLY_FLAGS)) == ERROR)
    {
       printf("pboot_read_phdr: Couldn't open file '%s'.\n", filename);
       return ERROR;
    }

    /* Read in ELF header from the source file */
    ret = read (fd, (char *) &ephdr->elf_hdr, ELF_HDR_SIZE);
    if (ret == ERROR)
    {
      printf("Couldn't read elf header ret_val:%d\n",ret);
      close (fd);
      return ERROR;
    }

    /* Verify the ELF header */
    ret = verify_elf_header(&ephdr->elf_hdr);
    if (ret == ERROR)
    {
      printf("ELF header fails verification\n");
      close (fd);
      return ERROR;
    }

    /* Get program header location from ELF header in src_elf */
    phdr_offset = ephdr->elf_hdr.e_phoff;
    #ifdef DBUG_MELF
    printf("Found program header at offset [0x%x] in source file\n", phdr_offset, 0, 0);
    #endif

    /* Calculate program header size */
    phdr_entry_size = ephdr->elf_hdr.e_phentsize;
    phdr_num_segment = ephdr->elf_hdr.e_phnum;
    phdr_size = phdr_entry_size * phdr_num_segment;

    if (phdr_size > UINT32_MAX)
    {
	  printf("phdr_size is grater than the limit\n");
      return ERROR;
	}
	
    #ifdef DBUG_MELF
    printf("Total program header size in bytes: [%d]\n", phdr_size, 0, 0);
    #endif

    /* Seek to program header location in src_elf */
    ret = lseek(fd, phdr_offset, SEEK_SET);
    if (ret == ERROR)
    {
      printf("Cannot lseek program header in SRC_ELF. ret(%d)", ret);
      close (fd);
      return ERROR;
    }

    /* Read in ELF header from the source file */
    ret = read (fd, (char *) ephdr->phdr_table, phdr_size);
    if (ret == ERROR)
    {
      printf("Couldn't read program header\n");
      close (fd);
      return ERROR;
    }

    /* close the elf file. */
    close (fd);
    return OK;

}   /* pboot_read_ephdr () */


/*===========================================================================

FUNCTION  PBOOT_UPDATE_EHDR

DESCRIPTION

    This function is used to update the elf header in our elf file.

DEPENDENCIES
    None.

RETURN VALUE
    ERROR   if invalid arguments were passed or
            if file open or file write returns an error else
    OK

SIDE EFFECTS

===========================================================================*/

uint32 pboot_update_ehdr
(
    char *filename,  /* filename of the elf file. */
    Elf32_Ehdr *ehdr /* elf header. */
)
{
    int fd; /* file descriptor. */
    uint32 ret; /* return value. */

        /* argument checks. */
    if ((filename == NULL) || (ehdr == NULL))
    {
        return ERROR;
    }
        /* open the file in write mode. */
    if ((fd = open (filename, FILE_OPEN_RDWR_FLAGS)) == ERROR)
    {
       printf("pboot_update_ehdr: Couldn't open file '%s'.\n", filename);
       return ERROR;
    }
        /* update the Elf Header. */
    ret = write (fd, (char *) ehdr, sizeof (Elf32_Ehdr));
    if (ret != sizeof (Elf32_Ehdr))
    {
       printf("ret(%d) - errno(%d)\n", ret, errno);
       return ERROR;
    }
        /* close the elf file. */
    close (fd);

    return OK;

} /* pboot_update_ehdr () */


/*===========================================================================

FUNCTION  PBOOT_READ_EHDR

DESCRIPTION

    This function is used to read the ELF header in our elf file.

DEPENDENCIES
    None.

RETURN VALUE
    ERROR   if invalid arguments were passed or
            if file open or file read returns an error else
    OK

SIDE EFFECTS

===========================================================================*/

uint32 pboot_read_ehdr
(
    char *filename,  /* filename of the elf file. */
    Elf32_Ehdr *ehdr /* elf header buffer. */
)
{
    int fd; /* file descriptor. */
    uint32 ret; /* return value. */

        /* argument checks. */
    if ((filename == NULL) || (ehdr == NULL))
    {
        return ERROR;
    }
        /* open the file in read mode. */
    if ((fd = open (filename, FILE_OPEN_RDONLY_FLAGS)) == ERROR)
    {
       printf("pboot_read_ehdr: Couldn't open file '%s'.\n", filename);
       return ERROR;
    }
        /* read the ELF header. */
    ret = read (fd, (char *) ehdr, sizeof (Elf32_Ehdr));
    if (ret != sizeof (Elf32_Ehdr))
    {
       printf("pboot_read_ehdr: ret(%d) - errno(%d)\n", ret, errno);
       return ERROR;
    }
        /* close the elf file. */
    close (fd);

    return OK;

} /* pboot_read_ehdr () */

/*===========================================================================

FUNCTION  PBOOT_DISPALL_PHDR

DESCRIPTION

    This function is used to display all the program headers in our elf file.
    The elf file to read is passed as an argument.

DEPENDENCIES
    None.

RETURN VALUE
    ERROR   if invalid arguments were passed or
            if pboot_read_ephdr () or pboot_disp_phdr () returns an ERROR else
    OK

SIDE EFFECTS

===========================================================================*/

uint32 pboot_dispall_phdr
(
  char *filename  /* elf filename. */
)
{
  elf_prog_header ephdr;  /* elf and program header. */
  uint32 i; /* index. */

  /* argument checks. */
  if (filename == NULL)
  {
    return ERROR;
  }

  /* read the elf and program header. */
  if (pboot_read_ephdr (filename, &ephdr) == ERROR)
  {
    return ERROR;
  }

  printf("\nELF Program Header\n\n");

  printf("%-10s %-10s %-10s %-10s %-10s %-4s %-10s %-6s\n",
         "offset","filesz","flags","vaddr",
         "paddr","type","memsz","align");

  if(ephdr.elf_hdr.e_phnum > UINT16_MAX)
  {
    printf("ephdr.elf_hdr.e_phnum is greater than the limit\n");
    return ERROR;
  }

  /* display all the program headers. */
  for (i = 0; i < ephdr.elf_hdr.e_phnum; i++)
  {
    /* display the program header. */
    if (pboot_disp_phdr (&(ephdr.phdr_table[i])) == ERROR)
    {
      return ERROR;
    }
  }

  return OK;

} /* pboot_dispall_phdr () */


/*===========================================================================
FUNCTION  PBOOT_DISP_PHDR

DESCRIPTION
  This function is used to display the program header in our elf file.
  The program header to display/print is passed as an argument.

DEPENDENCIES
    None

RETURN VALUE
  ERROR   if invalid arguments were passed or
  OK

SIDE EFFECTS

===========================================================================*/

uint32 pboot_disp_phdr
(
  Elf32_Phdr *phdr /* program header. */
)
{
  /* argument checks. */
  if (phdr == NULL)
  {
    return ERROR;
  }

  printf("0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%02x 0x%08x 0x%04x\n",
         phdr->p_offset,phdr->p_filesz,phdr->p_flags,phdr->p_vaddr,
         phdr->p_paddr,phdr->p_type,phdr->p_memsz,phdr->p_align);

  return OK;

} /* pboot_disp_phdr () */

/*===========================================================================
FUNCTION  PBOOT_DISP_SHDR

DESCRIPTION
    This function is used to display the program header in our elf file.
    The program header to display/print is passed as an argument.

DEPENDENCIES
    None

RETURN VALUE
  ERROR   if invalid arguments were passed or
  OK

SIDE EFFECTS

===========================================================================*/
uint32 pboot_disp_shdr
(
  Elf32_Shdr *shdr, /* program header. */
  char       *string_ptr
)
{
  int i;
  /* argument checks. */
  if (shdr == NULL)
  {
    return ERROR;
  }

  printf("The Section Header contains: \n");
  printf("s_name = 0x%x  ", shdr->sh_name);
  printf("s_type = 0x%x  ", shdr->sh_type);
  printf("s_addr = 0x%x  ", shdr->sh_addr);
  printf("s_offset = 0x%x  ", shdr->sh_offset);
  printf("s_size = 0x%x  ", shdr->sh_size);
  printf("s_flags = 0x%x  ", shdr->sh_flags);
  printf("s_name  = %s \n", &string_ptr[shdr->sh_name]);
  if (strcmp(&string_ptr[shdr->sh_name], "SHARED_RAM") ==0)
  {
    printf (">>>>>>>>>>>>>>Found SHARED_RAM<<<<<<<<<<<<<<<<<<<\n");
  }
  return OK;

} /* pboot_disp_phdr () */

/*===========================================================================
FUNCTION  PBOOT_READ_ESHDR

DESCRIPTION
  This function is used to read the elf and section header in our elf file.

DEPENDENCIES
  None

RETURN VALUE
  ERROR   if invalid arguments were passed or
          if file open or file read returns as error else
  OK

SIDE EFFECTS

===========================================================================*/
uint32 pboot_read_eshdr
(
    char *filename,  /* filename of the elf file. */
    elf_section_header *eshdr /* elf and section header buffer. */
)
{
  uint32 i;
  int fd; /* file descriptor. */
  uint32 ret; /* number of bytes written. */
  uint32 index = 0;
  uint32 shdr_entry_size = 0;
  uint32 shdr_num_segment = 0;
  uint32 shdr_size = 0;
  uint32 shdr_offset = 0;

  /* argument checks. */
  if ((filename == NULL) || (eshdr == NULL))
  {
    return ERROR;
  }
  /* open the file in read mode. */
  if ((fd = open (filename, FILE_OPEN_RDONLY_FLAGS)) == ERROR)
  {
    printf("pboot_read_eshdr: Couldn't open file '%s'.\n", filename);
    return ERROR;
  }

  /* Read in ELF header from the source file */
  ret = read (fd, (char *) &eshdr->elf_hdr, ELF_HDR_SIZE);
  if (ret == ERROR)
  {
    printf("Couldn't read elf header ret_val:%d\n",ret);
    close (fd);
    return ERROR;
  }

  /* Verify the ELF header */
  ret = verify_elf_header(&eshdr->elf_hdr);
  if (ret == ERROR)
  {
    printf("ELF header fails verification\n");
    close (fd);
    return ERROR;
  }

  /* Get program header location from ELF header in src_elf */
  shdr_offset = eshdr->elf_hdr.e_shoff;
  #ifdef DBUG_MELF
  printf("Found section header at offset [0x%x] in source file\n", shdr_offset, 0, 0);
  #endif

  /* Calculate section header size */
  shdr_entry_size  = eshdr->elf_hdr.e_shentsize;
  shdr_num_segment = eshdr->elf_hdr.e_shnum;
  shdr_size = shdr_entry_size * shdr_num_segment;

  if (shdr_size > UINT32_MAX)
  {
     printf("shdr_size is grater than the limit\n");
	 return ERROR;
  }
	  
  //#ifdef DBUG_MELF
  printf("Total section header size in bytes: [%d]\n", shdr_size, 0, 0);
  //#endif

  /* Seek to section header location in src_elf */
  ret = lseek(fd, shdr_offset, SEEK_SET);
  if (ret == ERROR)
  {
    printf("Cannot lseek section header in SRC_ELF. ret(%d)", ret);
    close (fd);
    return ERROR;
  }

  /* Read in Section header from the source file */
  ret = read (fd, (char *) eshdr->shdr_table, shdr_size);
  if (ret == ERROR)
  {
    printf("Couldn't read section header\n");
    close (fd);
    return ERROR;
  }

  /* close the elf file. */
  close (fd);

  return OK;
} /* pboot_read_eshdr */

/*===========================================================================
FUNCTION  PBOOT_READ_ESHDR

DESCRIPTION
  This function is used to read the elf and section header in our elf file.

DEPENDENCIES
  None

RETURN VALUE
  ERROR   if invalid arguments were passed or
          if file open or file read returns as error else
  OK

SIDE EFFECTS

===========================================================================*/
uint32 pboot_read_stringname
(
    char *filename,               /* filename of the elf file. */
    elf_section_header *eshdr,    /* elf and program header buffer. */
    char *buffer_ptr              /* pointer to the buffer to store the data */
)
{
  int fd; /* file descriptor. */
  uint32 ret; /* number of bytes written. */

  /* argument checks. */
  if ((filename == NULL) || (eshdr == NULL) || (buffer_ptr == NULL))
  {
    return ERROR;
  }

  /* open the file in read mode. */
  if ((fd = open (filename, FILE_OPEN_RDONLY_FLAGS)) == ERROR)
  {
    printf("pboot_read_eshdr: Couldn't open file '%s'.\n", filename);
    return ERROR;
  }
  /* Seek to section header location in src_elf */
  ret = lseek(fd, eshdr->shdr_table[eshdr->elf_hdr.e_shstrndx].sh_offset, SEEK_SET);
  if (ret == ERROR)
  {
    printf("Cannot lseek section header in SRC_ELF. ret(%d)", ret);
    close (fd);
    return ERROR;
  }

  /* Read in ELF header from the source file */
  ret = read (fd, buffer_ptr, eshdr->shdr_table[eshdr->elf_hdr.e_shstrndx].sh_size);
  if (ret == ERROR)
  {
    printf("Couldn't read section header\n");
    close (fd);
    return ERROR;
  }

  close (fd);
  return OK;
} /* pboot_read_stringname */

