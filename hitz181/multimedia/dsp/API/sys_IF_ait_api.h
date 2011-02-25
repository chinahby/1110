#ifndef AIT_SYS_API_H
#define AIT_SYS_API_H

#include	"A8_common.h"
#include	"AIT700_EBIbus.h"

typedef enum
{
	AIT_STATUS_DUMMY = 0x00,
	
	AIT_STATUS_CAM_CLOSED,
	AIT_STATUS_CAM_READY,
	AIT_STATUS_CAM_SENSOR_READY,		// AIT Active and Sensor PowerOn
	AIT_STATUS_CAM_PREVIEW,
	AIT_STATUS_CAM_CAPTURE,
	AIT_STATUS_CAM_PLAY,
	
	AIT_STATUS_VDO_RECORD,
	AIT_STATUS_VDO_PLAY,
	AIT_STATUS_VDO_PLAY_PAUSE,
	
	AIT_STATUS_USB_CLOSED,		// AIT enter bypass mode after USB function is finished
	AIT_STATUS_USB_READY,	//Downloading usb fw is successful
	AIT_STATUS_USB_MSDC,
	AIT_STATUS_USB_PCCAM,
	AIT_STATUS_USB_VCOM
} AIT_STATUS;

/* ################################################################## */
extern void sys_IF_ait_delay1us(u_int time);
extern void sys_IF_ait_delay1ms(u_int time);
extern void sys_IF_ait_delay10ms(u_int time);

extern void sys_IF_ait_set_input_clock(u_char ait_mode, u_char on);
extern void sys_IF_ait_reset_chip (void);
extern void sys_IF_ait_set_bypass_mode (u_char on);
extern u_char	sys_IF_ait_get_bypass_status(void );

extern void sys_IF_ait_sensor_power(u_char on);
extern void sys_IF_ait_sd_power(u_char on);
extern void sys_IF_ait_usb_power(u_char on);

extern A8_ERROR_MSG sys_IF_ait_boot_init(void);
extern A8_ERROR_MSG sys_IF_ait_enter_sleep(void);
extern AIT_STATUS	sys_IF_ait_get_status(void);
extern void sys_IF_ait_set_status(u_char CurStatus);

#endif
