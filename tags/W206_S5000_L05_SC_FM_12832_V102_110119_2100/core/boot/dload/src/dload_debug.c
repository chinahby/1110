/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              M E M O R Y   D E B U G   I N F O R M A T I O N 

GENERAL DESCRIPTION
  This module defines the memory debug information to be sent to host

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2005-2008 by QUALCOMM Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/services/dload/1_0/dload_debug.c#5 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/10/06   tbg     Modified to compile with boot loader images.
10/20/05   tbg     Ported to msmshared. Moved some functions to target
                   specific file dload_debug_xxxx.c
08/01/05   ih      Created.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "bootmem.h"
#include "dload_debug.h"
#include "string.h"
#include <stdio.h>
#include "clkregim.h"
#include "mobile.h"

#ifdef FEATURE_PMIC_RTC
#include "clkrtc.h"
#endif

#ifdef FEATURE_DLOAD_MEM_DEBUG
#define NUM_REGIONS 20 
#define SCRIPT_BUF_SIZE 2048

#define POS(x) (x > 0 ? x : 0)

typedef struct 
{
  byte save_pref;
  uint32 mem_base;
  uint32 length;
  char *desc;
  char *filename;
} dload_debug_type;


/* Since memory dumps are done in 32-bit chunks, the script has to be aligned
 * as well to get dumped correctly. We also pad it with non-essential chars
 * so nothing gets truncated at the end
 */
static char __align(4) cmm_script[SCRIPT_BUF_SIZE];

static dload_debug_type dload_debug_info[NUM_REGIONS];
static uint32 real_num_regions = 0;

/*===========================================================================

                     EXTERNAL DEFINITIONS FOR MODULE

===========================================================================*/

/*===========================================================================

FUNCTION DLOAD_DEBUG_ADD_REGION

DESCRIPTION
  This function adds a region to dload_debug_info[]

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void dload_debug_add_region(dload_save_pref_type save_pref, uint32 mem_base, uint32 length, 
    char *desc, char *filename)
{
  uint32 i = real_num_regions;

  /* Make sure we don't overrun array and align memory regions */
  if(i == NUM_REGIONS) return;
  dload_debug_info[i].save_pref = (byte)save_pref; 
  dload_debug_info[i].mem_base  = mem_base & ~3; 
  dload_debug_info[i].length    = length  & ~3; 
  dload_debug_info[i].desc      = desc;
  dload_debug_info[i].filename  = filename;

  real_num_regions++; 
}

/*===========================================================================

FUNCTION DLOAD_MEM_DEBUG_INIT

DESCRIPTION
  This function call the function to do target specific initialization,
  and sets up the script.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void dload_mem_debug_init(void)
{
  int j, bufsize = SCRIPT_BUF_SIZE, n;
  char *s;
#ifndef BUILD_BOOT_CHAIN
#ifdef FEATURE_PMIC_RTC
  clk_julian_type error_time;
#endif
#endif

  /*
  ** Reset number of regions, to ensure only those added by the function
  ** dload_mem_debug_target_init() will be used.
  */
  real_num_regions = 0;
  
  /* Set up memory regions and clocks for specific target */
  dload_mem_debug_target_init();
  
  /* Initialize the script for the regions we just added
   */
  s = cmm_script;

  /* Add mobile ID */
  n = snprintf(s, POS(bufsize), "; Build ID: %s\n", mob_sw_rev);
  s += n;
  bufsize -= n;

#ifdef BUILD_BOOT_CHAIN
  /* Time stamp currently unavailable from boot loader */
  n = snprintf(s, POS(bufsize), 
               "; Time Stamp unavailable from boot loader.\n");
  s += n;
  bufsize -= n;  
#else
#ifdef FEATURE_PMIC_RTC
  /* Add time stamp */
  clk_rtc_get_time(&error_time);
  n = snprintf(s, POS(bufsize), 
        "; Time Stamp: %02d/%02d/%04d %02d:%02d:%02d (M/D/Y H:M:S)\n",
        error_time.month,error_time.day,error_time.year, error_time.hour, 
        error_time.minute,error_time.second
      );
  s += n;
  bufsize -= n;
#endif
#endif

  for(j=0; j < real_num_regions; j++)
  {
    /* Move s to the end of string after printing the string */
    n = snprintf(s, POS(bufsize), 
                 "if OS.FILE(%s)\n"
                 "(\n"
                 "  d.load.binary %s 0x%lx /noclear\n"
                 ")\n", 
                 dload_debug_info[j].filename, 
                 dload_debug_info[j].filename,                  
                 dload_debug_info[j].mem_base);
    s += n;
    bufsize -= n;
  }
  /* Pad the end of the script so it does not get truncated when the length of
   * the region gets masked to 32-bit boundaries
   */
  (void)snprintf(s, POS(bufsize), "\n\n\n");

  dload_debug_add_region(MANDATORY, (uint32)cmm_script, (uint32)strlen(cmm_script), 
      "CMM Script", "load.cmm");
}


/*===========================================================================

FUNCTION DLOAD_MEM_DEBUG_LEN

DESCRIPTION
  This function returns length of the debug info response

DEPENDENCIES
  None.

RETURN VALUE
  length of the debug info response

SIDE EFFECTS
  None.

===========================================================================*/
uint32 dload_mem_debug_len(void)
{
  dload_debug_type *p = dload_debug_info;
  uint32 resp = 0, i;
  for(i=0; i < real_num_regions; i++)
  {
    resp+=sizeof(p[i].save_pref) + sizeof(p[i].mem_base) + sizeof(p[i].length);
    resp+=strlen(p[i].desc) + 1;      // null-termination char
    resp+=strlen(p[i].filename) + 1;  // null-termination char
  }
  return resp;
}

/*===========================================================================

FUNCTION DLOAD_MEM_DEBUG_VERIFY_ADDR

DESCRIPTION
  This function checks to see if it belongs to a region of memory that can be
  dumped.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the address verifies, else FALSE.

SIDE EFFECTS
  None.

===========================================================================*/
boolean dload_mem_debug_verify_addr(uint32 base, uint32 len)
{
  dload_debug_type *p = dload_debug_info;
  uint32 i;
  for(i=0; i < real_num_regions; i++)
  {
    if(base >= p[i].mem_base && (base+len) <= (p[i].mem_base+p[i].length))
      return TRUE;
  }
  return FALSE;
}
/*===========================================================================

FUNCTION DLOAD_MEM_DEBUG_NUM_ENT

DESCRIPTION
  This function returns number of entries in the mem_debug_info table

DEPENDENCIES
  None.

RETURN VALUE
  number of entries in the mem_debug_info table

SIDE EFFECTS
  None.

===========================================================================*/
uint32 dload_mem_debug_num_ent(void)
{
  return real_num_regions;
}

/*===========================================================================

FUNCTION DLOAD_MEM_DEBUG_SAVE_PREF

DESCRIPTION
  This function returns the save preference of a given region

DEPENDENCIES
  None.

RETURN VALUE
  save preference of a given region
  0 if invalid region number
SIDE EFFECTS
  None.

===========================================================================*/
uint8 dload_mem_debug_save_pref(uint8 i)
{
  return (i < real_num_regions ? dload_debug_info[i].save_pref : 0);
}

/*===========================================================================

FUNCTION DLOAD_MEM_DEBUG_MEM_BASE

DESCRIPTION
  This function returns the memory base of a given region

DEPENDENCIES
  None.

RETURN VALUE
  memory base of a given region
  0 if invalid region number

SIDE EFFECTS
  None.

===========================================================================*/
uint32 dload_mem_debug_mem_base(uint8 i)
{
  return (i < real_num_regions ? dload_debug_info[i].mem_base : 0);
}

/*===========================================================================

FUNCTION DLOAD_MEM_DEBUG_MEM_LENGTH

DESCRIPTION
  This function returns the memory length of a given region

DEPENDENCIES
  None.

RETURN VALUE
  memory length of a given region
  0 if invalid region number

SIDE EFFECTS
  None.

===========================================================================*/
uint32 dload_mem_debug_mem_length(uint8 i)
{
  return (i < real_num_regions ? dload_debug_info[i].length : 0);
}

/*===========================================================================

FUNCTION DLOAD_MEM_DEBUG_DESC

DESCRIPTION
  This function returns the description of a given region

DEPENDENCIES
  None.

RETURN VALUE
  description of a given region
  NULL if invalid region number

SIDE EFFECTS
  None.

===========================================================================*/
char *dload_mem_debug_desc(uint8 i)
{
  return (i < real_num_regions ? dload_debug_info[i].desc : NULL);
}

/*===========================================================================

FUNCTION DLOAD_MEM_DEBUG_FILENAME

DESCRIPTION
  This function returns the filename of a given region

DEPENDENCIES
  None.

RETURN VALUE
  filename of a given region
  NULL if invalid region number

SIDE EFFECTS
  None.

===========================================================================*/
char *dload_mem_debug_filename(uint8 i)
{
  return (i < real_num_regions ? dload_debug_info[i].filename : NULL);
}
#endif /* FEATURE_DLOAD_MEM_DEBUG */
