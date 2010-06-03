/*====*====*===*====*====*====*====*====*====*====*====*====*====*====*====*
                             C M    I D M

                 I M P L E M E N T A T I O N    F I L E

GENERAL DESCRIPTION
  This file is the main implementation for the IDM module. This module will
  control the state transitions for Idle Digital Mode and control the
  handdown and handup mechanisms. This module will function in associaton
  with the CMSS module.

EXTERNALIZED FUNCTIONS
  Functional Interface:
    cmssidm_proc_rpt_srv_ind_ss_main
    cmssidm_proc_rpt_srv_ind_ss_hdr
    cmssidm_proc_hdr_reselect
    cmssidm_proc_cmd_call_connected
    cmssidm_proc_cmd_call_ended
    cmssidm_proc_timer
    cmssidm_get_curr_idm
    cmssidm_get_prev_idm
    cmssidm_init

REGIONAL FUNCTIONS
  None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The cmssidm_init() should be called before any APIs are called.


Copyright (c) 2003 - 2009 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmssidm.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/31/09   pm      Change EXTERN define to MMEXTN to resolve compile error 
                   when other modules define EXTERN
08/07/07   ic      Lint clean up
06/06/07   pk/cl   Added support for LN status notification from HDR
11/07/06   pk      Added support for HYBR_GW and UMTS -> 1X handover
09/08/06   pk      Removed more preferred acquisiton check from IDM timer 
                   processing
01/12/06   ic      For specific calls / walks Lint warnings, replaced global 
                   -fsp comment with -e{error#} comments where needed
01/06/06   ic      Lint cleanup  
12/09/05   ic      Changes to cmssidm_estimate_base_station_idm() 
                   declaration so cmcall.c lints clean
11/03/05   ic      Added CM_ASSERT() for ptr function parameters and lint 
                   comments for 613 Lint error 
10/20/05   sk      Added concept of base station IDM and functions to access
                   and update it.
05/23/05   ic      Do not change IDM while SMS is in progress 
08/04/04   ic      CM Lint clean up - cmssidm.c with Lint supression verified 
                   and free of Lint issues in all CUTE configurations. 
07/26/04   ic      Added cmssidm_dummy() to get rid of ADS 1.2 compiler
                   warning
07/07/04   sj      Fixed issue if HDR is acquired in STATE_MAIN and timer is
                   running.
06/28/04   sj      Lint cleanup.
05/28/04   ic      Modified cmssidm_proc_call_connected() to send IDM event
                   on voice call connect if SVD is supported
05/20/04   sj      Delay handdown to 1x, if HDR is lost and 1x is acquired.
02/25/04   ic      Changes to make the code pass unit test cases and at the
                   same time make calls in Saber build
02/12/04   ic      Merged in changes from MSMSHARED_CM.01.00.10.00.20 to
                   MSMSHARED_CM.01.00.10.00.23
02/06/04   ws      Initial jaguar/mainline merge.
12/11/03   ic      Replaced is_hybr_redialled with is_hybr_redialed
12/03/03   ic      Added CM SS IDM state machine diagram.
                   In cmssidm_proc_rpt_srv_ind_ss_hdr() fixed state machine
                   bug for transition #10 (as HDR became unusable, we were
                   using wrong field to update IDM state and we were not
                   starting the avoid hand up timer).
11/21/03   ic      Fixed FEATURE_HDR_HANDOFF featurization bug.
11/19/03   ic      Featurized SS IDM object's functionality under
                   FEATURE_HDR_HANDOFF.
                   Comment clean up.
                   State machine fixes.
11/06/03   SJ      Initial release.

===========================================================================*/
/*

                          CMSSIDM STATE MACHINE
                          =====================

    State transitions are numbered to identify them.
    Numbering does not imply order in which transitions take place.


                                      Transition #1
                                      Acq_main & usable,
                                      colloc HDR,
                                      phone not in use
                                      - start hand down timer
                                    -------------------------------
                                    |                             |
                                    |                             |
                                    |                             |
                                    V                             |
                        |----------------------------|            |
  [ Power up / init ]   |                            |            |
  --------------------->|     CMSSIDM_STATE_NONE     |-------------
                        |                            |-------------------------|
                        |----------------------------|-----                    |
                                            |             |                    |
                                            |             |                    |
          Transition #2      Transition #3  |           Transition #4          |
          PS data call       Hand down timer time out,  Acq 1X & usable        |
          connected on 1X,   1X available & usable,     no colloc HDR,         |
          no HDR attempt     phone not in use           phone not in use       |
          - IDM update       - IDM update   |           - IDM update           |
                |                           |             |                    |
Transition #5   |                           |             |   Transition #7    |
PS data call    |                           |             |   Acq HDR & usable,|
connected on 1X,|                           |             |   phone not in use |
HDR attempted   |                           |             |   - IDM update     |
- start avoid   |                           |             |                    |
  hand up timer |                           |             |                    |
- IDM update    |     Transition #6         |             |                    |
  |             |     Acq 1X & it's         |             |                    |
  |             |     usable,               |             |                    |
  |             |     no colloc HDR,        |             |                    |
  |             |     sys mode changed,     |             |                    |
  |             |     phone not in use      |             |                    |
  |             |     - IDM update          |             |                    |
  |             |        ------------       |             |                    |
  |             |        |          |       |             |                    |
  |             |        |          |       |             |                    |
  |             |        |          |       |             |                    |
  |             |        |          V       V             |                    |
  |             |       |----------------------------|    |                    |
  |             |------>|                            |<----                    |
  |-------------------->|     CMSSIDM_STATE_MAIN     |-----------              |
  |                ---->|                            |          |              |
  |                |    |----------------------------|          |              |
  |                |              ^          |                  |              |
  |                |              |          |                Transition #12   |
  |                |              |          |                Acq HDR & usable,|
Transition #8   Transition #9  Transition #10|                1X not usable,   |
Acq 1X          Hand down      HDR unusable, |                phone not in use |
& usable,       timer time     1X available, |                OR               |
hand down       out,           phone not     |                Lost 1X,         |
timer expired,  HDR not        in use        |                HDR acq & usable,|
phone not in    available      - start      Transition #11    phone not in use |
use, no colloc  1X available     avoid      Avoid hand up   - stop timers      |
- start avoid  - start avoid     handup     timer time out  - IDM update       |
hand up timer    hand up timer   timer      or timer not        |              |
- IDM update   - IDM update    - IDM update active, HDR         |              |
  |                       |       |         available &         |              |
  |                       |       |         usable, phone       |              |
  |                       |       |         not in use          |              |
  |                       |       |         - IDM update        |              |
  |                       |       |          |                  |              |
  |                       |       |          |                  |              |
  |                       |       |          |                  |              |
  |                       |       |          |                  |              |
  |                       |       |          |                  |              |
  |                       |       |          V                  |              |
  |                     |----------------------------|          |              |
  |---------------------|                            |<---------|              |
                        |   CMSSIDM_STATE_HYBR_HDR   |<------------------------|
  |-------------------->|                            |<--------|
  |                     |----------------------------|         |
  |                                      |                     |
 Transition #13                          |                     |
 PS call connected                       |---------------------|
 on HDR                                      Transition #14
                                             Lost HDR
                                             - start hand down timer
                                             or
                                             Acquired usable 1x & no timer active,
                                             having colloc,
                                             - start hand down timer

Notes:
------
1. End of every call, start 5 second timer or max
   if timer running, to possibly handup on HDR.
2. Any timeout processing
   if HDR avl && usable
     IDM = HDR
   else if main_avl && usable
     IDM = MAIN
3 If timer expires when the phone is in use, postpone processing
  the timer expiry until the phone is not in use

*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/*lint -esym(766,customer.h) */
#include "customer.h" /* Customer configuration file */

#ifdef FEATURE_HDR_HANDOFF
#error code not present
#else /* FEATURE_HDR_HANDOFF */

/*===========================================================================
FUNCTION cmssidm_dummy

DESCRIPTION

  Dummy function, defined to get rid of
  'Warning: C3040E: no external declaration in translation unit'
  ADS 1.2 compiler warning

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmssidm_dummy( void )
/*lint -esym(714,cmssidm_dummy) */
/*lint -esym(765,cmssidm_dummy)
** Can't be static as it results in more compiler warnings 
*/ 
{
}

#endif /* FEATURE_HDR_HANDOFF */
