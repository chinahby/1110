/*===========================================================================
                                      
                           D S M I P _ P E R F . C

DESCRIPTION

  The Data Services Mobile IP call performance implementation.  Contains the
  API and internal functions to determine the various delays that are 
  involved in the Mobile IP call metrics. 

EXTERNALIZED FUNCTIONS

  mip_perf_init()
    Initializes Mobile IP performance timers.
     
  mip_perf_delay()
    Sets start and end times for specified delay.


Copyright (c) 2002-2008 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/dsmip_perf.c_v   1.2   16 Jul 2002 15:20:28   jeffd  $
  $Header: //source/qcom/qct/modem/data/1x/mip/main/lite/src/dsmip_perf.c#2 $ $DateTime: 2008/06/23 04:38:29 $ $Author: parmjeet $
                          
                          
when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/19/08    psng   Fixed compiler warnings
02/19/02    sjy    Now featurized under FEATURE_DS_MOBILE_IP_PERF
01/24/02    sjy    Created module
===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_DS_MOBILE_IP_PERF
#include "dsmip_perf.h"
#include "msg.h"
#include "ts.h"


/*===========================================================================

                             INTERNAL DATA 

===========================================================================*/

#ifndef T_ARM
/*---------------------------------------------------------------------------
  These names need to be kept in sync with the enums which they describe.
---------------------------------------------------------------------------*/
char *perf_delay_names[PERF_MAX_DELAY] =
{
  "Um PPP delay",
  "Rm PPP delay",
  "M.IP reg delay",
  "M.IP re-reg delay",
  "Total call setup delay",
  "Total handoff delay"
};
#endif

/*---------------------------------------------------------------------------
  Mobile IP performance info array.
---------------------------------------------------------------------------*/
mip_perf_ts_info_type mip_perf_info[PERF_MAX_DELAY];
   

/*---------------------------------------------------------------------------
  The null time stamp.  The start time stamp variable for each delay is 
  given this value initially.
---------------------------------------------------------------------------*/
qword ts_null;                                             


/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
  FUNCTION MIP_PERF_INIT

  DESCRIPTION
    This function initializes the Mobile IP performance info array.
      
  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void mip_perf_init
(
  void
)
{
  int i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Set null time stamp.
  -------------------------------------------------------------------------*/
  qw_set(ts_null, 0, 0);

  /*-------------------------------------------------------------------------
    Initialize start time stamps.
  -------------------------------------------------------------------------*/    
  for (i = 0; i < PERF_MAX_DELAY; i++) 
  {
    qw_equ(mip_perf_info[i].ts_start, ts_null);
  }
} /* mip_perf_init */


/*===========================================================================
  FUNCTION MIP_PERF_DELAY

  DESCRIPTION
    This function manages the start timestamp and end time stamps, as well
    as the calculation of the delta times for the delay types in the 
    performance info block.
    
  PARAMETERS
    ts_info: the time stamp info struct for the delay type
    delay_type: the delay type
    ts_type: type of timestamp to be set
  
  RETURN VALUE
    None
 
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void mip_perf_delay
(
  mip_perf_ts_info_type * ts_info,
  mip_perf_delay_type delay_type,
  mip_perf_ts_type ts_type
)
{
  /*-------------------------------------------------------------------------
    Intermediate delta time stamp variable.
  -------------------------------------------------------------------------*/
  qword ts_diff; 
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (ts_type)
  {
    case PERF_START_TS:
      /*---------------------------------------------------------------------
        Write system time stamp to start time stamp.
      ---------------------------------------------------------------------*/   
      ts_get(ts_info->ts_start);
    
#ifdef T_ARM
      MSG_LOW("Delay '%d': SET", delay_type, 0, 0);
#else
      MSG_LOW("%s: SET", perf_delay_names[delay_type], 0, 0); 
#endif

      break;

    case PERF_END_TS:
      /*---------------------------------------------------------------------
        Sanity check: write end time stamp and calculate delay only if the 
        start time stamp has been started.
      ---------------------------------------------------------------------*/
      if (qw_cmp(ts_info->ts_start, ts_null) != 0)
      {     
        /*-------------------------------------------------------------------
          Write system time stamp to end time stamp.
        -------------------------------------------------------------------*/
        ts_get(ts_info->ts_end);
    
        /*-------------------------------------------------------------------
          Calculate the difference between the start and end time stamps.
        -------------------------------------------------------------------*/
        qw_sub(ts_diff, 
               ts_info->ts_end, 
               ts_info->ts_start);
    
        /*-------------------------------------------------------------------
          Convert time stamp from System Time to millisecond units.
        -------------------------------------------------------------------*/
        TS_SYSTIME_TO_MS(ts_diff);
        ts_info->delay_in_ms = ts_diff[0];
    
        /*-------------------------------------------------------------------
          Calculate and display delta time in seconds and milliseconds.
        -------------------------------------------------------------------*/
           
#ifdef T_ARM
        MSG_HIGH("Delay '%d': %ds %dms", 
                 delay_type,
                 ts_info->delay_in_ms / 1000, 
                 ts_info->delay_in_ms % 1000);  
#else
        MSG_HIGH("%s: %ds %dms", 
                 perf_delay_names[delay_type],
                 ts_info->delay_in_ms / 1000, 
                 ts_info->delay_in_ms % 1000);
#endif
  
        /*-------------------------------------------------------------------
          Initialize start time stamp to null.
        -------------------------------------------------------------------*/
        qw_equ(ts_info->ts_start, ts_null);
      }
      else
      {
        MSG_ERROR("Delay '%d' not calculated, start TS not set", 
                   delay_type, 0, 0);
      }
      break;

    default:
      MSG_ERROR("Invalid M.IP perf delay '%d'", ts_type, 0, 0);
  }
} /* mip_perf_delay() */

#endif /* FEATURE_DS_MOBILE_IP_PERF */
