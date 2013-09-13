#ifndef   BF3903_YUV_H
#define   BF3903_YUV_H

#include "camsensor.h"
#include "camctrl.h"

boolean camsensor_byd_bf3903_init
(
	camsensor_function_table_type *camsensor_function_table_ptr,
	camctrl_tbl_type			  *camctrl_tbl_ptr
);


uint8 camsensor_bf3903_ycbcr_active(void);
uint8 camsensor_bf3903_ycbcr_unactive(void);

#endif

