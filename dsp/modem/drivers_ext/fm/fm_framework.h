#ifndef __FM_FRAMEWORK_H__
#define __FM_FRAMEWORK_H__

#include "AEEStdDef.h"
#define WART_FM_SUCCESS     0
#define WART_FM_FAILED         1


typedef enum
{
	DIRECTION_UP,
	DIRECTION_DOWN
}WarT_Direction_e;

typedef enum
{
    FM_NOT_INIT,
    FM_POWER_DOWN,
    FM_POWER_UP,
    FM_IDLE_STATUS = FM_POWER_UP,
    FM_IN_PROGRESS,
    
    FM_MAX_STATUS = 0xFFFF
}WarT_Fm_Status_e;



typedef struct 
{	
	int (*fm_initail)(void);	
	int (*fm_powerup)(void);	
	int (*fm_powerdown)(void);

	int (*fm_set_channel)(uint16 freq);

	int (*fm_setvolume)(uint16 wVolume);
	void (*fm_mute)(boolean on);	
	
	WarT_Fm_Status_e (*fm_get_status)(void);
	uint16 (*fm_GetCurrentChannel)(void);
}WarT_Fm_t;

extern int WarT_Fm_Init(void);
extern int WarT_Fm_PowerUp(void);
extern int WarT_Fm_PowerDown(void);
extern int WarT_Fm_Set_Channel(uint16 freq);
extern int WarT_Fm_Set_Volume(uint16 wVolume);
extern WarT_Fm_Status_e WarT_Fm_Get_status(void);
extern void WarT_Fm_Mute(boolean on);
extern uint16 WarT_Fm_Get_Current_Channel(void);

#define WarT_Fm_Is_Inited() (WarT_Fm_Get_status()>FM_NOT_INIT)
#define WarT_Fm_Is_Powerup() (WarT_Fm_Get_status()>FM_POWER_UP)
#define WarT_Fm_Is_Powerdown() (WarT_Fm_Get_status()>FM_POWER_DOWN)
#define WarT_Fm_Is_Tuning() (WarT_Fm_Get_status == FM_IN_PROGRESS)

#endif





