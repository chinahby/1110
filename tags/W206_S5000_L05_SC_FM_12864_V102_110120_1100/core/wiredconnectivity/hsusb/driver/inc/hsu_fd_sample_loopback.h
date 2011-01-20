#ifndef HSU_FD_SAMPLE_LOOPBACK_H
#define HSU_FD_SAMPLE_LOOPBACK_H

/*==============================================================================

                      High Speed USB Sample Loopback Function Driver Header File
  
GENERAL DESCRIPTION
 Header file of loopback function Driver.
 This file externalizes the hsu_fd_sample_loopback_init as an sample for function driver.
 The following explains how to add new function driver into HS USB platform with 
 hsu_fd_sample_loopback_init for an example.
 
 1. add new function driver enum value in hsu_conf_sel_types.h
 
 enum
 {
   HSU_CFG_SEL_MS_FD = 0,
   HSU_CFG_SEL_OBEX_FD,
   HSU_CFG_SEL_ACM_FD,
   HSU_CFG_SEL_ECM_FD,
   HSU_CFG_SEL_SICD_FD,
     .....
     .....
   HSU_CFG_SEL_LOOPBACK_FD,  
   HSU_CFG_SEL_NUM_OF_FD_TYPES 
};

2. add new function driver instance.  Refer to build_fd_struct in hsu_conf_ sel_stack_utils.c 
    to see how to add function driver. 
    
 if(num_of_fd_instances_in_comp[HSU_CFG_SEL_LOOPBACK_FD] > 0)
 {
   fds_to_init[fd_index].type = DEVICE_TYPE_VENDOR;
   fds_to_init[fd_index].count = 1;
   fds_to_init[fd_index].init_func = hsu_fd_sample_loopback_init; 
   ++fd_index;
 }

EXTERNALIZED FUNCTIONS
hsu_fd_sample_loopback_init

Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

  EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
$Header: 
$DateTime: 


when      who     what, where, why
--------  ---     ----------------------------------------------------------
06/01/08  tk      created file
    
==============================================================================*/

#include <jusb_common.h>

/*==============================================================================

FUNCTION      hsu_fd_sample_loopback_init

DESCRIPTION   
  This function initializes sample loopback function driver.

DEPENDENCIES  
  None
  
RETURN VALUE  
  HSU_JSUCCESS (0) for success, other values for failure.

SIDE EFFECTS  
  None.

==============================================================================*/
jresult_t hsu_fd_sample_loopback_init(void *ctx);

#endif /* !HSU_FD_SAMPLE_LOOPBACK_H */
