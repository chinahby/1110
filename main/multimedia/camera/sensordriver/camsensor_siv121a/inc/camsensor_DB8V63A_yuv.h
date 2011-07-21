
#ifndef   DB8V63A_YUV_H
#define   DB8V63A_YUV_H

#include "camsensor.h"
#include "camctrl.h"


boolean camsensor_DB8V63A_ycbcr_init
(
	camsensor_function_table_type *camsensor_function_table_ptr,
	camctrl_tbl_type			  *camctrl_tbl_ptr
);

//LOCAL boolean camsensor_DB8V63A_sensor_init(void);
//LOCAL void camsensor_DB8V63A_init_info(void);

 boolean camsensor_DB8V63A_ycbcr_start
( 
	camsensor_static_params_type *camsensor_params 
);

boolean camsensor_DB8V63A_ycbcr_video_config
( 
  camsensor_static_params_type *camsensor_params /* Other config params */ 
);

void camsensor_DB8V63A_ycbcr_power_up(void);


boolean camsensor_DB8V63A_ycbcr_snapshot_config
( 
	camsensor_static_params_type *camsensor_params /* Other config params */
);


camera_ret_code_type camsensor_DB8V63A_set_effect(int8 effect);

boolean camsensor_DB8V63A_ycbcr_raw_snapshot_config
(
	camsensor_static_params_type *camsensor_params /* Other config params */
);

camera_ret_code_type  camsensor_DB8V63A_set_nightmode(int8 nightmode);

camera_ret_code_type camsensor_DB8V63A_set_wb(int8 wb);

camera_ret_code_type camsensor_DB8V63A_set_contrast(int8 contrast);

camera_ret_code_type camsensor_DB8V63A_set_antibanding(int8 antibanding);

camera_ret_code_type camsensor_DB8V63A_set_brightness(int8 value);

#endif

