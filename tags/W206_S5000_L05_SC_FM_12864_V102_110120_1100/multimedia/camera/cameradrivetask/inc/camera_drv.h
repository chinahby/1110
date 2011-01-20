#ifndef CAMERA_DRV_H
#define CAMERA_DRV_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      C A M E R A     D R I V E R     T A S K

GENERAL DESCRIPTION
  This module contains camera driver software.

REFERENCES

EXTERNALIZED FUNCTIONS

  camera_drv_task
    This function is the entrance procedure for the camera driver
    Task. It contains the main processing loop for the camera driver task.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright(c) 2005 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/camera/6k/cameradrivetask/main/latest/inc/camera_drv.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/17/06   jn      Exported camera_drv_queue_focus().
12/21/05   dle     Initial release.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "target.h"             /* Target specific definitions             */
#include "customer.h"           /* Customer specific definitions           */
#include "comdef.h"             /* Definitions for byte, word, etc.        */
#ifdef FEATURE_CAMERA_DRV_TASK
/* <EJECT> */
/*===========================================================================

                DECLARATIONS FOR MODULE

===========================================================================*/

/* <EJECT> */
/*===========================================================================

FUNCTION camera_drv_task

DESCRIPTION
  This procedure is the entrance procedure for the CAMERA_DRV
  Task.  It contains the main processing loop for the CAMERA_DRV Task.

DEPENDENCIES
  FEATURE_CAMERA_DRV_TASK.

RETURN VALUE
  None

SIDE EFFECTS
  camera_drv_tcb.

===========================================================================*/
void camera_drv_task
(
  dword ignored
    /* Parameter received from Main Control task - ignored */
    /*lint -esym(715,ignored)
    ** Have lint not complain about the ignored parameter 'ignored' which is
    ** specified to make this routine match the template for rex_def_task().
    */
);

/*===========================================================================

FUNCTION camera_drv_queue_focus

DESCRIPTION
  This function queues camera focus request

DEPENDENCIES
  FEATURE_CAMERA_DRV_TASK

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void camera_drv_queue_focus
(
  void *callback,
  uint32 client_data,
  uint32 info1,
  uint32 info2,
  uint32 info3,
  uint32 info4
);

/*===========================================================================

FUNCTION camera_drv_queue_reset_lens

DESCRIPTION
  This function queues camera reset lens request

DEPENDENCIES
  FEATURE_CAMERA_DRV_TASK

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void camera_drv_queue_reset_lens
(
  void *callback,
  uint32 client_data,
  uint32 info1,
  uint32 info2,
  uint32 info3,
  uint32 info4
);

/* <EJECT> */
#endif /* FEATURE_CAMERA_DRV_TASK */
#endif /* CAMERA_DRV_H */
