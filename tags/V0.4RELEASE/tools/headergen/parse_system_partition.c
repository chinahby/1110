/*=============================================================================

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2005, 2006 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/tools/headergen/parse_system_partition.c#1 $

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
#include "parse_system_partition.h"

#undef   DEBUG_PARSE_PARTITION
/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/

/*=============================================================================

                              FUNCTION DEFINITIONS

=============================================================================*/
/*===========================================================================
FUNCTION  PARSE_SYSTEM_PARTITION_LAYOUT

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
uint32 parse_system_partition_layout
(
  char *fname,  /* input file name. */
  FILE *filetoread,
  flash_partable_t  sys_parti
)
{

  uint32 i;
  if (file_open (fname, "rb", &filetoread) == ERROR)
  {
    return ERROR;
  }

  if (file_read(filetoread, (uint8 *) sys_parti, 16) == ERROR)
  {
    fclose (filetoread);
    return ERROR;
  }

  if ( (sys_parti->magic1 != FLASH_PART_MAGIC1) ||
       (sys_parti->magic2 != FLASH_PART_MAGIC2)
     )
  {
    fclose (filetoread);
    return ERROR;
  }

  if (fseek (filetoread, 16, SEEK_SET) == ERROR)
  {
    fclose (filetoread);
    return ERROR;
  }

  if ( file_read( filetoread,
                  (uint8 *) &sys_parti->part_entry, 
                  (sys_parti->numparts * sizeof(sys_parti->part_entry[0]))) == ERROR)
  {
    fclose (filetoread);
    return ERROR;
  }

  #ifdef DEBUG_PARSE_PARTITION
  printf( "Magic1:0x%x Magic2:0x%x \n", sys_parti->magic1, sys_parti->magic2);


  for (i=0; i<sys_parti->numparts; i++)
  {
    printf ( "Name:%s Offset:0x%x Length:0x%x WhichFlash:%d \n",
             sys_parti->part_entry[i].name,
             sys_parti->part_entry[i].offset,
             sys_parti->part_entry[i].length,
             sys_parti->part_entry[i].which_flash
           );
  }
  #endif  
  fclose (filetoread);
  return OK;
}


