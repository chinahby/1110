/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          Boot Trap Handlers non-init data
                
GENERAL DESCRIPTION
  Contains all data used to log exceptions to EFS.  Since the information is
	stored to RAM first, the logged to NV after reset, the data needs to go 
	untouched by any of the RAM initialization functions.  The scatter load
	file is set up to put any *_nzi.o files in an uninitialized portion of RAM.
	
INITIALIZATION AND SEQUENCING REQUIREMENTS
  No requirements

Copyright (c) 2003 - 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/24/08   tbg     Added trace_bufer_id field
10/31/05   as      Fixed lint errors.
10/30/04   src     Featurised for non-debug enhanced builds.
08/25/04   eav     Added support to save max number of files.
05/14/04   eav     Added support to save events.
03/10/04   eav     File created.

===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "diagdebug.h"

 
#if defined FEATURE_ERR_EXTENDED_STORE

  #if defined FEATURE_L4
#error code not present
  #endif

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

#ifdef FEATURE_SAVE_TRACE_EXTENDED
/* Used for multiple file saving */
uint8        diag_debug_max_files;
#endif

#ifdef FEATURE_OPTIMIZE_TRACE_RECORD_TIMESTAMPS
/* Needed for timestamp-saving optimization */
diag_debug_tstamp_type ref_tstamp;
diag_debug_tstamp_type prev_tstamp;
#endif

uint32 diag_debug_trace_record_count;
uint32 trace_buffer_head;
uint32 trace_buffer_tail;

diag_debug_id_type trace_buffer_id;

uint8 trace_buffer[TRACE_BUFFER_SIZE];

  #if defined FEATURE_L4
#error code not present
  #endif

#else
/* To supress warning: no external declaration in translation unit */
void diagdebug_foo (void)
{
  return;
}
#endif /* FEATURE_ERR_EXTENDED_STORE */
