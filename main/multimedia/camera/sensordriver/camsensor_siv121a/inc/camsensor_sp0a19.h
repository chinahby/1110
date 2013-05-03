#ifndef   SP0A19_YUV_H
#define   SP0A19_YUV_H

#include "camsensor.h"
#include "camctrl.h"

boolean camsensor_sp0a19_init
(
	camsensor_function_table_type *camsensor_function_table_ptr,
	camctrl_tbl_type			  *camctrl_tbl_ptr
);

uint8 camsensor_sp0a19_ycbcr_active(void);
uint8 camsensor_sp0a19_ycbcr_unactive(void);

boolean camsensor_sp0a19_ycbcr_start
( 
	camsensor_static_params_type *camsensor_params 
);

boolean camsensor_sp0a19_ycbcr_video_config
( 
  camsensor_static_params_type *camsensor_params /* Other config params */ 
);

void camsensor_sp0a19_ycbcr_power_up(void);


boolean camsensor_sp0a19_ycbcr_snapshot_config
( 
	camsensor_static_params_type *camsensor_params /* Other config params */
);


camera_ret_code_type camsensor_sp0a19_set_effect(int8 effect);

boolean camsensor_sp0a19_ycbcr_raw_snapshot_config
(
	camsensor_static_params_type *camsensor_params /* Other config params */
);


camera_ret_code_type camsensor_sp0a19_set_wb(int8 wb);

camera_ret_code_type camsensor_sp0a19_set_contrast(int8 contrast);

camera_ret_code_type camsensor_sp0a19_set_antibanding(int8 antibanding);

camera_ret_code_type camsensor_sp0a19_set_brightness(int8 value);
#endif


