/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

          Modify ELF file for Progressive Boot

GENERAL DESCRIPTION

  This file contains functions necessary to modify the .elf file created
  by the build process to make it suitable for Progressive Boot.
  
EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  

  Copyright (c) 2005 by QUALCOMM Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/tools/headergen/pboot_melf.h#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/16/06   anb    Made changes to handle the new ELF layout.
05/25/05   chs    Always open files in binary mode.
01/11/05   chs    Modified to work with C compiler.
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

#include "filetool.h"
#include "miprogressive.h"


/* Flag values used to specify the program segment types */
//#define P_FLAGS_NON_PAGED     0
//#define P_FLAGS_PAGED         1
//#define P_FLAGS_NOT_LOADED    2

#define ELF_HDR_SIZE 52
#define MAX(a,b) ((a) > (b) ? (a) : (b))

//#define PHDRS_COUNT 32
struct elf_file_info
{
   int        fd;
   Elf32_Ehdr elf_hdr;
   //index 0 is used for initial calculation only
   Elf32_Phdr phdr_table[MI_PBT_MAX_SEGMENTS+1];
};

typedef struct
{
   Elf32_Ehdr elf_hdr;
   Elf32_Phdr phdr_table[MI_PBT_MAX_SEGMENTS+1];
} elf_prog_header;

/*
 * Section header 
 */
typedef struct {
  uint32        sh_name;
  uint32        sh_type;
  uint32        sh_flags;
  uint32        sh_addr;
  uint32        sh_offset;
  uint32        sh_size;
  uint32        sh_link;
  uint32        sh_info;
  uint32        sh_addralign;
  uint32        sh_entsize;
} Elf32_Shdr ;

typedef struct
{
   Elf32_Ehdr elf_hdr;
   Elf32_Shdr shdr_table[MI_PBT_MAX_SEGMENTS+1];
} elf_section_header;

#define FILE_OPEN_RDONLY_FLAGS (O_RDONLY|O_BINARY)
#define FILE_OPEN_RDWR_FLAGS (O_RDWR|O_BINARY)

/*===========================================================================

FUNCTION  PBOOT_UPDATE_PHDR

DESCRIPTION

    This function is used to update the program header in our elf file.
    The program header number to update is passed as an argument.
    A value of -1 for the program header number will result in a new
    program header being generated.
    To allow updating only specific program header information, a value
    of 0x0 in any of the program header fields is ignored.
    
    
 
DEPENDENCIES
    None.

RETURN VALUE
    ERROR   if the new file cant be created else
    OK
    
SIDE EFFECTS
    Creates the new amss elf file.

===========================================================================*/

uint32 pboot_update_phdr
(
    char *,       /* filename of the elf file. */
    uint32,       /* program header number to update. */
    Elf32_Phdr *  /* new program header. */
);



/*===========================================================================

FUNCTION  PBOOT_READ_PHDR

DESCRIPTION

    This function is used to read the program header in our elf file.
    The program header number to read is passed as an argument.
 
DEPENDENCIES
    None.

RETURN VALUE
    ERROR   if open, lseek or read returns as error else
    OK
    
SIDE EFFECTS

===========================================================================*/

uint32 pboot_read_phdr
(
    char *,      /* filename of the elf file. */
    uint32,      /* program header number to read. */
    Elf32_Phdr * /* program header buffer. */
);



/*===========================================================================

FUNCTION  PBOOT_UPDATE_EHDR

DESCRIPTION

    This function is used to update the elf header in our elf file.
 
DEPENDENCIES
    None.

RETURN VALUE
    ERROR   if file open or file write returns an error else
    OK
    
SIDE EFFECTS

===========================================================================*/

uint32 pboot_update_ehdr
(
    char *,         /* filename of the elf file. */
    Elf32_Ehdr *    /* elf header. */
);


/*===========================================================================

FUNCTION  PBOOT_READ_EHDR

DESCRIPTION

    This function is used to read the ELF header in our elf file.
 
DEPENDENCIES
    None.

RETURN VALUE
    ERROR   if file open or file read returns an error else
    OK
    
SIDE EFFECTS

===========================================================================*/

uint32 pboot_read_ehdr
(
    char *,         /* filename of the elf file. */
    Elf32_Ehdr *    /* elf header buffer. */
);


/*===========================================================================

FUNCTION  PBOOT_DISPALL_PHDR

DESCRIPTION

    This function is used to display all the program headers in our elf file.
    The elf file to read is passed as an argument.
    
DEPENDENCIES
    None.

RETURN VALUE
    ERROR   if the input argument is invalid else
    OK
    
SIDE EFFECTS

===========================================================================*/

uint32 pboot_dispall_phdr
(
    char *  /* elf filename. */
);


/*===========================================================================

FUNCTION  PBOOT_DISP_PHDR

DESCRIPTION

    This function is used to display the program header in our elf file.
    The program header to display is passed as an argument.
    
DEPENDENCIES
    None.

RETURN VALUE
    ERROR   if the input argument is invalid else
    OK
    
SIDE EFFECTS

===========================================================================*/

uint32 pboot_disp_phdr
(
    Elf32_Phdr *    /* program header buffer. */
);


/*===========================================================================

FUNCTION  PBOOT_UPDATE_EPHDR

DESCRIPTION

    This function is used to update the elf and program header in our elf file.
 
DEPENDENCIES
    None.

RETURN VALUE
    ERROR   if open, lseek or write returns as error else
    OK
    
SIDE EFFECTS

===========================================================================*/

uint32 pboot_update_ephdr
(
    char *, /* filename of the elf file. */
    elf_prog_header * /* elf and program header. */
);


/*===========================================================================

FUNCTION  PBOOT_READ_EPHDR

DESCRIPTION

    This function is used to read the elf and program header in our elf file.
 
DEPENDENCIES
    None.

RETURN VALUE
    ERROR   if open or read returns as error else
    OK
    
SIDE EFFECTS

===========================================================================*/

uint32 pboot_read_ephdr
(
    char *,  /* filename of the elf file. */
    elf_prog_header * /* elf and program header buffer. */
);

/*===========================================================================

FUNCTION  PBOOT_READ_ESHDR

DESCRIPTION

    This function is used to read the elf and section header in our elf file.
 
DEPENDENCIES
    None.

RETURN VALUE
    ERROR   if invalid arguments were passed or
            if file open or file read returns as error else
    OK
    
SIDE EFFECTS

===========================================================================*/
uint32 pboot_read_eshdr
(
    char *,  /* filename of the elf file. */
    elf_section_header * /* elf and program header buffer. */
);

/*===========================================================================

FUNCTION  PBOOT_READ_ESHDR

DESCRIPTION

    This function is used to read the elf and section header in our elf file.
 
DEPENDENCIES
    None.

RETURN VALUE
    ERROR   if invalid arguments were passed or
            if file open or file read returns as error else
    OK
    
SIDE EFFECTS

===========================================================================*/
uint32 pboot_read_stringname 
(
    char *,               /* filename of the elf file. */
    elf_section_header *, /* elf and program header buffer. */
    char *                /* pointer to the buffer to store the data */
);
