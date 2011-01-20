#ifndef AIT_SD_API_H
#define AIT_SD_API_H
#include "A8_common.h"
#include	"sys_IF_ait_api.h"

A8_ERROR_MSG sd_IF_ait_open_sd(void);
A8_ERROR_MSG sd_IF_ait_close_sd(void);
A8_ERROR_MSG sd_IF_ait_init_sd(void);
A8_ERROR_MSG sd_IF_ait_write_sectors(u_int startsect, u_int offset, u_char* buf, u_int write_size);
A8_ERROR_MSG sd_IF_ait_read_sectors(u_int startsect, u_int offset, u_char* buf, u_int read_size);
u_char* sd_IF_ait_get_CSD(void);
#endif
