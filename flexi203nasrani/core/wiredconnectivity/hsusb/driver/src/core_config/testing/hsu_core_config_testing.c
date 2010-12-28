/*==============================================================================

                            HS-USB Core configuraion

GENERAL DESCRIPTION
    Contains the core configuration for HS-USB for RexNT unit testing.

EXTERNALIZED FUNCTIONS

INITALIZATION AND SEQUENCING REQUIREMENTS
    None

Copyright (c) 2007-2008 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: 
$DateTime: 

when     who  what, where, why
-------- ---  ---------------------------------------------------------
07/17/08 me  Initial version

==============================================================================*/

/*==============================================================================
INCLUDE FILES FOR MODULE
==============================================================================*/
#include "hsu_core_config.h"
#include "hsu_conf_sel_core_config.h"
#include "jtypes.h"
#include "uw_args.h"

/*==============================================================================
LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
==============================================================================*/
/*==============================================================================
Constants
==============================================================================*/
/*==============================================================================
Macros
==============================================================================*/
/*==============================================================================
Typedefs
==============================================================================*/
/*==============================================================================
Variables
==============================================================================*/
uint8 config_table_to_test = 0;
/*uint8 num_of_cores = 0;*/

extern hsu_core_config_type cores_details[HSU_NUM_OF_CORES_TYPES]; 

/**********************  host configuration ************************/

static hsu_core_config_type hsu_host_test_core_config = 
{ 
  HSU_CORE_CONFIG_INIT_MODE_HOST,
  NULL,
  NULL
};

/*******************  device only configuration *********************/
static struct hsu_config_params_type hsu_device_test_config_params[] = 
{ 
  /* Configuration 0 data */
  {
    {0,1,0,0,0,0,0,0,0},              /* 1 instance of OBEX */
    "HS-USB configuration 0",         /* config_name */
    NULL                              /* cur_dev_comp_details */
  }
};

static struct hsu_device_params_type hsu_device_test_device_params =
{
  HSU_CONF_SEL_HIGH_SPEED_CORE,
  0x900c,        /* product_id */                        
  1,             /* num_of_configs */
  &hsu_device_test_config_params[0]
};

static hsu_core_config_type hsu_device_test_core_config = 
{ 
  HSU_CORE_CONFIG_INIT_MODE_DEVICE,
  &hsu_device_test_device_params,
  NULL
};

/*******************  multi config configuration *********************/
static struct hsu_config_params_type hsu_multi_config_test_config_params[] = 
{ 
  /* Configuration 0 data */
  {
    {0,1,0,0,0,0,0,0,0},              /* 1 instance of OBEX */
    "HS-USB configuration 0",         /* config_name */
    NULL                              /* cur_dev_comp_details */
  }
  ,{
    {1,2,1,1,0,0,0,0,0},              /* DIAG & NMEA & MS & MDM & Net device */
    "HS-USB configuration 1",         /* config_name */
    NULL                              /* cur_dev_comp_details */
  },

};

static struct hsu_device_params_type hsu_multi_config_test_device_params =
{
  HSU_CONF_SEL_HIGH_SPEED_CORE,
  0x900B,        /* product_id */                        
  2,             /* num_of_configs */
  &hsu_multi_config_test_config_params[0]
};

hsu_core_config_type hsu_multi_config_test_core_config = 
{ 
  HSU_CORE_CONFIG_INIT_MODE_OTG,
  &hsu_multi_config_test_device_params,
  NULL
};


hsu_core_config_type* hsu_core_config_table_0[]=
{
  /* High-Speed USB core. */
  {
    &cores_details[HSU_HS_MAIN_CORE]
  }
  ,{
    NULL
  }
  ,{
    &hsu_device_test_core_config
  }
};

static hsu_core_config_type* hsu_core_config_table_1[]=
{
  /* High-Speed USB core. */
  {
    &cores_details[HSU_HS_MAIN_CORE]
  }
  ,{
    &hsu_host_test_core_config
  }
  ,{
    &hsu_device_test_core_config
  }
};

static hsu_core_config_type* hsu_core_config_table_2[]=
{
  /* High-Speed USB core. */
  {
    NULL
  }
  ,{
    &hsu_host_test_core_config
  }
};

static hsu_core_config_type* hsu_core_config_table_3[]=
{
  /* High-Speed USB core. */
  {
    NULL
  }
  ,{
    &cores_details[HSU_HS_MAIN_CORE]
  }
};

static hsu_core_config_type* hsu_core_config_table_4[]=
{
  /* High-Speed USB core. */
  {
    NULL
  }
};

static hsu_core_config_type* hsu_core_config_table_5[]=
{
  /* High-Speed USB core. */
  {
    &cores_details[HSU_HS_MAIN_CORE]
  }
};

static hsu_core_config_type* hsu_core_config_table_6[]=
{
  /* Multi config USB core. */
  {
    &hsu_multi_config_test_core_config
  }
};
/*==============================================================================
Forward Declarations
==============================================================================*/

/*==============================================================================
EXTERNAL FUNCTION DEFINITIONS FOR MODULE
==============================================================================*/
/*===========================================================================
FUNCTION ()
===========================================================================*/
hsu_core_config_type* hsu_core_config_get_core(uint8 core_idx)
{
  switch (config_table_to_test)
  {
    case 0:
      return hsu_core_config_table_0[core_idx];
    break;
    case 1:
      return hsu_core_config_table_1[core_idx];
    break;
    case 2:
      return hsu_core_config_table_2[core_idx];
    break;
    case 3:
      return hsu_core_config_table_3[core_idx];
    break;
    case 4:
      return hsu_core_config_table_4[core_idx];
    break;
    case 5:
      return hsu_core_config_table_5[core_idx];
    break;
    case 6:
      return hsu_core_config_table_6[core_idx];
    break;
    default:
      return NULL;
    break;
  }
}

uint8 hsu_core_config_get_cores_table_size(void)
{
  switch (config_table_to_test)
  {
  case 0:
    return sizeof(hsu_core_config_table_0)/sizeof(hsu_core_config_type*);
    break;
  case 1:
    return sizeof(hsu_core_config_table_1)/sizeof(hsu_core_config_type*);
    break;
  case 2:
    return sizeof(hsu_core_config_table_2)/sizeof(hsu_core_config_type*);
    break;
  case 3:
    return sizeof(hsu_core_config_table_3)/sizeof(hsu_core_config_type*);
    break;
  case 4:
    return sizeof(hsu_core_config_table_4)/sizeof(hsu_core_config_type*);
    break;
  case 5:
    return sizeof(hsu_core_config_table_5)/sizeof(hsu_core_config_type*);
    break;
  case 6:
    return sizeof(hsu_core_config_table_6)/sizeof(hsu_core_config_type*);
    break;
  default:
    return 0;
    break;
  }
}

static boolean hsu_core_config_test_verify_device_params(hsu_device_params_type* core_conf_sel_info, device_params_t *device_params)
{
  uint8 config_idx;
  uint8 fd_idx;
  /*uint8 stack_fd_idx = 0;*/
  uint8 hsu_num_of_fd_idx = 0;

  if (core_conf_sel_info->num_of_configs != device_params->num_of_configs)
  {
    return FALSE;
  }
  if (core_conf_sel_info->product_id != device_params->dev_info->product)
  {
    return FALSE;
  }

  if (((core_conf_sel_info->core_type == HSU_CONF_SEL_HIGH_SPEED_CORE) &&
      (device_params->full_speed_only == TRUE)) ||
      ((core_conf_sel_info->core_type == HSU_CONF_SEL_FULL_SPEED_CORE) &&
      (device_params->full_speed_only == FALSE)))

  {
    return FALSE;
  }

  for (config_idx = 0 ;  config_idx < core_conf_sel_info->num_of_configs; ++config_idx)
  {
    /* calculate num_of_fd_instances */
    hsu_num_of_fd_idx = 0;
    for (fd_idx = 0; fd_idx < HSU_CFG_SEL_NUM_OF_FD_TYPES; ++fd_idx)
    {
      if (core_conf_sel_info->configs_arr[config_idx].num_of_fd_instances[fd_idx] != 0)
      {
        /*for (stack_fd_idx = 0; stack_fd_idx < device_params->config_param[config_idx].num_of_fd_instances; ++stack_fd_idx)
        {
          if (device_params->config_param[config_idx].fd_instances[stack_fd_idx].type)
        }*/
        ++hsu_num_of_fd_idx;
      }
    }
    if (hsu_num_of_fd_idx != device_params->config_param[config_idx].num_of_fd_instances)
    {
      return FALSE;
    }
  }
  return TRUE;
}


static boolean hsu_core_config_test_verify_core(hsu_core_config_type* core_to_test, uw_hw_core_param_t* stack_core_info)
{
  /* TODO - verify core_index */
  switch (core_to_test->init_mode)
  {
  case HSU_CORE_CONFIG_INIT_MODE_HOST:
    if (stack_core_info->init_opt != UW_INIT_HOST)
    {
      return FALSE;
    }
    break;
  case HSU_CORE_CONFIG_INIT_MODE_DEVICE:
    if (stack_core_info->init_opt != UW_INIT_DEVICE)
    {
      return FALSE;
    }
    return hsu_core_config_test_verify_device_params((hsu_device_params_type*)core_to_test->hsu_conf_sel_core_info, stack_core_info->device_param);
    break;
  case HSU_CORE_CONFIG_INIT_MODE_OTG:
    if (stack_core_info->init_opt != UW_INIT_OTG)
    {
      return FALSE;
    }
    return hsu_core_config_test_verify_device_params((hsu_device_params_type*)core_to_test->hsu_conf_sel_core_info, stack_core_info->device_param);
    break;
  default:
    return FALSE;
    break;
  }
   
  return TRUE;
}
static boolean hsu_core_config_test_verify_cores_table(hsu_core_config_type** table_to_test, uw_args_t* stack_init_args)
{
  uint8 num_of_cores = 0;
  uint8 stack_core_idx = 0;
  uint8 core_table_index = 0;
  uint8 cores_table_size = hsu_core_config_get_cores_table_size();

  /* calculate num of cores*/
  for (core_table_index = 0; core_table_index < cores_table_size ; ++core_table_index)
  {
    if (table_to_test[core_table_index] != NULL)
      ++num_of_cores;
  }

  if (stack_init_args->num_of_cores != num_of_cores)
  {
    return FALSE;
  }
  /* Go over all the cores and compare them to Jungo's structure values */
  for (core_table_index = 0; core_table_index < cores_table_size ; ++core_table_index)
  {
    if (table_to_test[core_table_index] == NULL)
    {
      continue;
    }
    if (hsu_core_config_test_verify_core(table_to_test[core_table_index], &stack_init_args->hw_core_params[stack_core_idx]) == FALSE)
    {
      return FALSE;
    }
    ++stack_core_idx;
  }
  return TRUE;
}

boolean hsu_core_config_test_verify_stack_setup(uw_args_t* stack_init_args)
{
  hsu_core_config_type** table_to_test;
  switch (config_table_to_test)
  {
  case 0:
    table_to_test = &hsu_core_config_table_0[0];
    break;
  case 1:
    table_to_test = &hsu_core_config_table_1[0];
    break;
  case 2:
    table_to_test = &hsu_core_config_table_2[0];
    break;
  case 3:
    table_to_test = &hsu_core_config_table_3[0];
    break;
  case 4:
    table_to_test = &hsu_core_config_table_4[0];
    break;
  case 5:
    table_to_test = &hsu_core_config_table_5[0];
    break;
  case 6:
    table_to_test = &hsu_core_config_table_6[0];
    break;
  default:
    return FALSE;
    break;
  }

  return hsu_core_config_test_verify_cores_table(table_to_test, stack_init_args);

}

