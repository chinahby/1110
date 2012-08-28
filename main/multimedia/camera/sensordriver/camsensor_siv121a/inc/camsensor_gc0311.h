#ifndef   GC0311_YUV_H
#define   GC0311_YUV_H

#include "camsensor.h"
#include "camctrl.h"

boolean camsensor_gc0311_init
(
	camsensor_function_table_type *camsensor_function_table_ptr,
	camctrl_tbl_type			  *camctrl_tbl_ptr
);


uint8 camsensor_gc0311_ycbcr_active(void);
uint8 camsensor_gc0311_ycbcr_unactive(void);

#endif

