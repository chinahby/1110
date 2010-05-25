#ifndef TS_H
#define TS_H
/*===========================================================================

                 T I M E  S T A M P   H E A D E R    F I L E

DESCRIPTION
  This module contains defintions needed to interface with the time
  stamp module.

Copyright (c) 1990, 1991, 1992 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1993, 1994 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1998       by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2002, 2003 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright(c) 2003, 2004 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.


  $Header: //depot/asic/msm6800/drivers/clk/ts.h#1 $
  $DateTime: 2004/11/23 18:27:36 $ $Author: tkarra $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/25/04   ap      Merged support to use TIME_VU.
03/15/04   ajn     Return (void) from ts_get(), instead of time_get()'s value
02/09/04   ajn     Renamed time.h to time_svc.h to avoid OS conflict
08/27/03   ajn     Functionality moved to the Time Services Subsystem.
06/17/03   ajn     Merged ts_sync_time_of_day from TAZ archive
09/12/00   sr      Added function declarations for ts_set_frame_offset()
                   and ts_get_20ms_frame_offset_time()
09/05/98   scdb    Added _cdecl to ts_dmod_valid().
09/01/98   scdb    Added _cdecl to some functions to save ROM
07/16/98   scdb    Added prototype for ts_dmod_valid()
04/05/94   jah     Added prototype for ts_get_20ms_frame_time()
02/17/94   jah     Added macro TS_SYSTIME_TO_MS, added comdef/qw includes.
08/11/93   jah     Added prototype for ts_inc()
05/14/93   jah     Added prototype for ts_get_slot_time()
09/01/92   jah     Updated from code review.  Improved comments.
08/06/92   jah     Ported from brassboard

===========================================================================*/

#include "comdef.h"     /* Definitions for byte, word, etc.     */
#include "qw.h"         /* definition of qword                  */

#include "time_svc.h"   /* For general purpose time functions   */
#include "time_cdma.h"  /* For CDMA specific time functions     */


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
=============================================================================
         Forward old ts_( ) calls to time_( ) and/or time_cdma( )
=============================================================================
===========================================================================*/

#define ts_dmod_valid( dmod_valid ) time_cdma_set_valid( dmod_valid )
#define ts_set( set_val, phase80 )  time_cdma_set( set_val, phase80 )
#define ts_inc( increment )         time_cdma_inc( increment )
#define ts_tick( phase80 )          time_cdma_tick( phase80 )
#define ts_get( ts_val )            (void) time_get( ts_val )
#define ts_sync_time_of_day( )      time_sync_time_of_day( )
#define ts_get_slot_time( )         time_cdma_get_slot_time( )
#define ts_get_20ms_frame_time(ts)  time_cdma_get_20ms_frame_time( ts )
#define ts_set_frame_offset(offset) time_cdma_set_frame_offset( offset )

#define ts_get_20ms_frame_offset_time( ts_offset_val ) \
  time_cdma_get_20ms_frame_offset_time( ts_offset_val )

#define TS_SYSTIME_TO_MS(xxx)       time_to_ms(xxx)


#endif /* TS_H */
