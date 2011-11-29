#ifndef   SIV121A_YUV_H
#define   SIV121A_YUV_H

#include "camsensor.h"
#include "camctrl.h"

boolean camsensor_siv121a_init
(
	camsensor_function_table_type *camsensor_function_table_ptr,
	camctrl_tbl_type			  *camctrl_tbl_ptr
);


boolean camsensor_SIV121A_ycbcr_start
( 
	camsensor_static_params_type *camsensor_params 
);

boolean camsensor_SIV121A_ycbcr_video_config
( 
  camsensor_static_params_type *camsensor_params /* Other config params */ 
);

void camsensor_SIV121A_ycbcr_power_up(void);


uint8 camsensor_SIV121A_ycbcr_active(void);
uint8 camsensor_SIV121A_ycbcr_unactive(void);


boolean camsensor_SIV121A_ycbcr_snapshot_config
( 
	camsensor_static_params_type *camsensor_params /* Other config params */
);


camera_ret_code_type camsensor_SIV121A_set_effect(int8 effect);

boolean camsensor_SIV121A_ycbcr_raw_snapshot_config
(
	camsensor_static_params_type *camsensor_params /* Other config params */
);


camera_ret_code_type camsensor_SIV121A_set_wb(int8 wb);

camera_ret_code_type camsensor_SIV121A_set_contrast(int8 contrast);

camera_ret_code_type camsensor_SIV121A_set_antibanding(int8 antibanding);

camera_ret_code_type camsensor_SIV121A_set_brightness(int8 value);
#endif
