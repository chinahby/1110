/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                 T I M E S T A M P    S E R V I C E S

GENERAL DESCRIPTION
  This module is responsible for creating 64 bit timestamps used
  during logging.

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None - See Time Service Subsystem initialization & sequencing requirements.

Copyright(c) 1990, 1991, 1992 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright(c) 1993, 1994, 1995 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright(c) 1996, 1997, 1998 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright(c) 1999, 2000, 2001 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright(c) 2002, 2003 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright(c) 2003, 2004 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msm6800/drivers/clk/ts.c#1 $
  $DateTime: 2004/11/23 18:27:36 $ $Author: tkarra $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/27/03   ajn     Deleting - functionality moved to Time Service Subsystem
06/17/03   ajn     Merged ts_sync_time_of_day from TAZ archive
01/09/03   ma      Featurisation for GSM PLT
10/30/02   ph      Removed RXC dependency in ts_get, not needed from Rel A onwards.
08/28/01   bkm     Merged in check for HDR system time if CDMA time is not valid.
09/12/00   sr      Added ts_set_frame_offset() and
                   ts_get_20ms_frame_offset_time()
09/12/00   ajn     Modify ts_tick to add 80msec if new phase < current phase.
07/27/00   ejv     Added MSM5000_IRAM_REV.
05/30/00   ejv     ts_get_slot_time now returns up to twice the max slot
                   cycle (in rolls).
11/02/99   kjn     Mainlined the FEATURE_VIRTUAL_COMBINER code.
04/27/99   bgc     Changed qw_div's to qw_div_by_power_of_2 for efficiency
03/17/99    ms     Changes for Virtual Boot Block.
                     Removed bb_ prefixes
                     Not including bbjmmpout.h, bbqw.h
                     Including qw.h, rxc.h, srch.h
02/05/99   ejv     Added scdb's and ajn's FEATURE_VIRTUAL_COMBINER.
10/04/98   scdb    Merged ARM code into mainline
09/05/98   scdb    Added _cdecl to bb_ts_dmod_valid().
09/01/98   scdb    Added _cdecl to some functions to save ROM
07/29/98   scdb    Added include of "customer.h"
07/16/98   scdb    AUXROM support is now controlled by a conditional compile
                   Added bb_ts_dmod_valid
                   Removed obsolete code from ts_inc
                   Added logic in ts_get to use the new bb_ts_dmod_valid_flag
                   which is set and cleared by bb_ts_dmod_valid
                   This is the mainlining of revision 1.11.1.1
05/08/98   scdb    Prepend "TS_" to TS_QWORD_INIT_ZERO.
05/08/98  kss/scdb Changed initialized data declaration to use the new
                   BOOT_DATA() macro. This is a merge into the mainline of
                   changes made in branch 1.9.2.
10/31/97   scdb    ts_inc no longer calls clk_set_ms.
04/08/96   dna     Added comment to each func. saying it is in the Boot Block
02/29/96   dna     Moved externs of qword routines into their own .h file.
02/28/96   dna     Prepended bb_ to function names and moved module into the
                   Boot Block.  Changed calls to qw_ functions to directly
                   call the ones in the Boot Block.  Changed declaration of
                   static variables ts_stamp and ts_phase80 to locate them
                   in the Boot Block data segment, and prepended bb_ to their
                   names.  Added functions ts_get_data() and ts_put_data()
                   and used them to access bb_phase_80.
12/07/95   jah     Changed ts_inc() to work in absence of system time.
04/05/94   jah     Added ts_get_20ms_frame_time()
11/18/93   jah     Changed comment in ts_get_slot_time() to "26.6ms units"
09/22/93   jah     Fixed '%' in ts_inc() which was an '&' instead.
08/11/93   jah     Added ts_inc()
05/14/93   jah     Added ts_get_slot_time()
09/01/92   jah     Updated from code review.  This includes improved comments,
                   correction for phase precision in ts_set() for the system
                   millisecond clock, and removal of the ts_set PN Roll hack.
08/13/92   jah     Fixed system clock set/read to be more accurate
08/06/92   jah     Ported from brassboard and added system clock set/call

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/


#include "comdef.h"     /* Common data types                               */
#include "customer.h"   /* Customization defines                           */



/***** Implementation moved to the Time Services Subsystem *****/

