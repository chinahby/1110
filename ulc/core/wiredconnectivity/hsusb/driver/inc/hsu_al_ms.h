#ifndef _HSU_AL_MS_H_
#define _HSU_AL_MS_H_

/*==============================================================================

                High Speed USB Mass Storage Adaptation Layer
  
  GENERAL DESCRIPTION
    Defines externalized data and functions for the mass storage adaptation layer.
  
  EXTERNALIZED FUNCTIONS
    hsu_al_ms_get_num_devices
    hsu_al_ms_task
    
  Copyright (c) 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

  EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
    $Header: //source/qcom/qct/core/hsusb/driver/main/latest/inc/hsu_al_ms.h#2 $
    
    when     who  what, where, why
    -------- ---  ---------------------------------------------------------
    
==============================================================================*/

#include "rex.h"

/* Definitions related to the Mass Storage Adaptation Layer Task */
#define HSU_AL_MS_TASK_NAME "HSU AL MS TASK"
#define HSU_AL_MS_STACK_SIZE  (2048 / sizeof(rex_stack_word_type))
extern unsigned char          hsu_al_ms_stack[ ]; /* defined in hsu_al_ms.c */
extern rex_tcb_type           hsu_al_ms_tcb; /* defined in hsu_al_ms.c */

#ifdef HS_USB_SCSI_BUFFER_SIZE
#define HSU_SCSI_BUFFER_SIZE HS_USB_SCSI_BUFFER_SIZE
#else
#define HSU_SCSI_BUFFER_SIZE (32*1024)
#endif /* HS_USB_SCSI_BUFFER_SIZE */

/*==============================================================================
Typedefs
==============================================================================*/

/*==============================================================================
PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/

/*===========================================================================

FUNCTION hsu_al_ms_get_num_devices

DESCRIPTION
  This function returns the number of mass storage devices
  (that is, the number of mass storage USB interfaces)

DEPENDENCIES
  None.

RETURN VALUE
  The number of mass storage devices.

SIDE EFFECTS
  None.
===========================================================================*/
uint8 hsu_al_ms_get_num_devices(void);

/*===========================================================================

FUNCTION hsu_al_ms_task

DESCRIPTION
  The entry point for the mass storage adaptation layer task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_al_ms_task
(
  dword parm /* Not used */
);

/*===========================================================================
FUNCTION      hsu_al_ms_get_num_luns

DESCRIPTION
This function counts the total number of luns

DEPENDENCIES
None.

RETURN VALUE
Number of luns

SIDE EFFECTS
None.
===========================================================================*/
uint8 hsu_al_ms_get_num_luns(void);

void hsu_al_ms_disable_polling(void);

#endif /* _HSU_AL_MS_H_ */
