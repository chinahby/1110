#ifndef QTV_TASK_IF_TASK_H
#define QTV_TASK_IF_TASK_H
/* =======================================================================

                          qtv_task_if_task.h

DESCRIPTION
  Definition of the task function used by qtv_task_if.  This is in
  a seperate header and not part of the class definition because it is
  used by services/task, which is not C++.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright 2006 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/task/main/latest/inc/qtv_task_if_task.h#7 $
$DateTime: 2008/05/08 14:17:40 $
$Change: 656443 $

========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "rex.h"
#include "queue.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
extern const rex_sigs_type QTV_TASK_INPUT_SIG;

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
typedef struct qtv_task_info_tag
{
  rex_timer_cnt_type  dog_rpt_interval;
  q_type              input_q;
  rex_crit_sect_type  input_cs;
}
qtv_task_info_struct;

/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */
/*
** Message queues for qtv task functions.  Each task needs its own message queue!
*/

extern qtv_task_info_struct qtv_task_video_renderer_info;
extern qtv_task_info_struct qtv_task_audio_info;
extern qtv_task_info_struct qtv_task10_info;

/* =======================================================================
**                        Macro Definitions
** ======================================================================= */

/* =======================================================================
**                        Class & Function Declarations
** ======================================================================= */
/* ========================================================================
FUNCTION:
  qtv_task_fn

DESCRIPTION:
  The task function for qtv tasks, to be used when starting static 
  tasks from services/task.

PARAMETERS:
  unsigned long
    unused, since there's no way to pass anything good in from the 
    task framework anyway.

RETURN VALUE:
  None.
===========================================================================*/
void qtv_task_fn( unsigned long );

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* QTV_TASK_IF_TASK_H */
