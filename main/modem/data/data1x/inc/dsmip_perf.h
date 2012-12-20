#ifndef DSMIP_PERF_H
#define DSMIP_PERF_H

/*===========================================================================

                           D S M I P _ P E R F . H

DESCRIPTION

  The Data Services Mobile IP call performance header file.

  Triggers for the start and end times for the various delays are outlined
  below.
  
                           
  Um PPP delay:   
  +-------------------------+               
  | ppplcp.c                |              
  |                         |-->----+       
  | lcp_init(), Um          |       |       +-------------------------+
  +-------------------------+       |       | pppipcp.c               |
                                    +---->--|                         |
  +-------------------------+       |       | ipcp_opening(), Um      |
  | dssnet.c                |       |       +-------------------------+
  |                         |-->----+        
  | PPP_RESYNC_EV (Um only) |    handoff           
  +-------------------------+       
                             
  
  Rm PPP delay:
  +-------------------------+               +-------------------------+
  | ppplcp.c                |               | pppipcp.c               |
  |                         |-->--------->--|                         |
  | lcp_init(), Rm          |               | ipcp_opening(), Rm      |
  +-------------------------+               +-------------------------+

  
  Initial Mobile IP registration:  
  +-------------------------+               +-------------------------+
  | dsmip_metasm.c          |               | dsmip_regsm.c           |
  | metasm state:           |-->--------->--| regsm state:            |
  | closed -> open          |               | init_rrq -> registered  |
  +-------------------------+               +-------------------------+
  
              
  Mobile IP re-registration:
  +-------------------------+               +-------------------------+
  | dsmip_regsm.c           |               | dsmip_regsm.c           |
  | regsm state:            |-->--------->--| regsm state:            |
  | registered -> re_reg    |               | re_reg -> registered    |
  +-------------------------+               +-------------------------+

                 
  Total call setup:  
                                            +-------------------------+
                                non-netmdl  | dsmip_regsm.c           |
                                    +---->--| regsm state:            |
  +-------------------------+       |       | init_rrq -> registered  |
  | dsmgr.c                 |       |       +-------------------------+
  |                         |-->----+
  | origination cmd cb      |       |       +-------------------------+
  +-------------------------+       |       | dsmip_rmsm.c            | 
                                    +---->--| rmsm state: -> um_rm_up |
                                  netmdl    +-------------------------+
                                            | dsiface.c               |
                                            | dsiface_up, Rm          |
                                            +-------------------------+
  Total handoff delay:                                            
  +-------------------------+               
  | dssoctl.c               |   in-traffic            
  |                         |-->----+       
  | PZID active change      |       |       +-------------------------+
  +-------------------------+       |       | dsmip_regsm.c           |
                                    +---->--| regsm state:            |
  +-------------------------+       |       | re_rrq -> registered    |
  | dssoctl.c               |       |       +-------------------------+
  |                         |-->----+        
  | PZID idle change        |    dormant           
  +-------------------------+       
             
             
EXTERNALIZED FUNCTIONS

  mip_perf_init()
    Initializes Mobile IP performance timers.
     
  mip_perf_delay()
    Sets start and end times for specified delay.

Copyright (c) 2002 - 2011 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/dsmip_perf.h_v   1.4   16 Nov 2002 00:34:04   jeffd  $
  $Header: //source/qcom/qct/modem/data/1x/mip/main/lite/inc/dsmip_perf.h#2 $ $DateTime: 2011/02/24 23:31:53 $ $Author: msankar $


when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/25/11    ms     Ported MOBILE_IP_DEREG feature.
02/20/02    sjy    Updated delay descriptions to reflect changes made in the
                   performance metrics.
02/19/02    sjy    Updated delay descriptions to reflect changes made in the
                   performance metrics. 
01/24/02    sjy    Created module
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "qw.h"

                   
/*===========================================================================

                         REGIONAL DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Enumerated delay types.
---------------------------------------------------------------------------*/
typedef enum
{
  PERF_MIN_DELAY = -1,
  PERF_UM_PPP_DELAY,              /* Um PPP delay                          */
  PERF_RM_PPP_DELAY,              /* Rm PPP delay                          */
  PERF_MIP_INIT_REG_DELAY,        /* mobile IP registration delay          */
  PERF_MIP_REREG_DELAY,           /* mobile IP re-registration delay       */
  PERF_TOTAL_CALL_SETUP_DELAY,    /* total call setup time                 */
  PERF_TOTAL_HANDOFF_DELAY,       /* total inter-PDSN handoff setup time   */
  PERF_MIP_DEREG_DELAY,           /* mobile IP de-registration delay       */
  PERF_MAX_DELAY
} mip_perf_delay_type;

/*---------------------------------------------------------------------------
  Enumerated time stamp types.
---------------------------------------------------------------------------*/
typedef enum
{
  PERF_MIN_TS = -1,
  PERF_START_TS,                  /* start time stamp                      */
  PERF_END_TS,                    /* end time stamp                        */
  PERF_CANCEL_TS,
  PERF_MAX_TS
} mip_perf_ts_type;

/*---------------------------------------------------------------------------
  Delay info block.
---------------------------------------------------------------------------*/
typedef struct
{
  qword ts_start;                 /* start time stamp for delay            */
  qword ts_end;                   /* end time stamp for delay              */
  uint32 delay_in_ms;             /* difference time stamp for delay in ms */
} mip_perf_ts_info_type;

/*---------------------------------------------------------------------------
  Mobile IP call performance info array.
---------------------------------------------------------------------------*/
extern mip_perf_ts_info_type mip_perf_info[PERF_MAX_DELAY];

extern qword ts_null;


/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
  FUNCTION MIP_PERF_INIT

  DESCRIPTION
    This function initializes the Mobile IP performance info block.
      
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
);

/*===========================================================================
  FUNCTION MIP_PERF_DELAY

  DESCRIPTION
    This function manages the start timestamp and end time stamps, as well
    as the calculation of the elapsed time for the specified delay in the 
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
    The previously elapsed time in ts_info will be over-written.
===========================================================================*/
void mip_perf_delay
(
  mip_perf_ts_info_type * ts_info,
  mip_perf_delay_type delay_type,
  mip_perf_ts_type ts_type
);

#endif /* DSMIP_PERF_H */
