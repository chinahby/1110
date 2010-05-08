#ifndef CAMERA_DRVI_H
#define CAMERA_DRVI_H
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

$Header: //source/qcom/qct/multimedia/camera/6k/cameradrivetask/main/latest/src/camera_drvi.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/21/05   dle     Initial release.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#ifdef FEATURE_CAMERA_DRV_TASK
#define CAMERA_DRV_RPT_TIMER_SIG        0x0001
#define CAMERA_DRV_CMD_Q_SIG            0x0002

/*===========================================================================

                     MODULE FUNCTIONS

===========================================================================*/

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
#endif /* FEATURE_CAMERA_DRV_TASK */
#endif /* CAMERA_DRVI_H */
