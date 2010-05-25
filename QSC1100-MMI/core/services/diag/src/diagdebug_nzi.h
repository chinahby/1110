#ifndef DIAGDEBUG_NZI_H
#define DIAGDEBUG_NZI_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          Boot Trap Handlers non-init data
                
GENERAL DESCRIPTION
  Contains all data used to log exceptions to EFS.  Since the information is
	stored to RAM first, the logged to NV after reset, the data needs to go 
	untouched by any of the RAM initialization functions.  The scatter load
	file is set up to put any *_nzi.o files in an uninitialized portion of RAM.
	
INITIALIZATION AND SEQUENCING REQUIREMENTS
  No requirements

Copyright (c) 2003-2008 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/24/08   tbg     Added trace_buffer_id field
10/30/04   src     Featurised for non-debug enhanced builds.
05/14/04   eav     Added support to save events.  Renamed variables to make
                   them trace-type specific.
03/17/04   eav     File created.

===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

 
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
#ifdef FEATURE_ERR_EXTENDED_STORE

  #ifdef FEATURE_SAVE_TRACE_EXTENDED
  extern uint8 diag_debug_max_files;
  #endif

  #ifdef FEATURE_OPTIMIZE_TRACE_RECORD_TIMESTAMPS
  extern diag_debug_tstamp_type ref_tstamp;
  extern diag_debug_tstamp_type prev_tstamp;
  #endif

  extern uint32 diag_debug_trace_record_count;
  extern diag_debug_id_type trace_buffer_id;
  extern uint32 trace_buffer_head;
  extern uint32 trace_buffer_tail;
  extern uint8 trace_buffer[];

#endif /* FEATURE_ERR_EXTENDED_STORE */

#endif
