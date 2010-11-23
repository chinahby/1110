#include "fm_framework.h"

static WarT_Fm_t g_tHeXingFm;


int WarT_Fm_Init(void)
{
#ifdef USE_FM_TLG1120
	TLGFM_Register(&g_tHeXingFm);
#elif defined USE_FM_RD5802

#endif

	return g_tHeXingFm.fm_initail();
}

int WarT_Fm_PowerUp(void)
{
	return g_tHeXingFm.fm_powerup();
}


int WarT_Fm_PowerDown(void)
{
	return g_tHeXingFm.fm_powerdown();
}

int WarT_Fm_Set_Channel(uint16 freq)
{	
	return g_tHeXingFm.fm_set_channel(freq);
}

WarT_Fm_Status_e WarT_Fm_Get_status(void)
{
	return g_tHeXingFm.fm_get_status();
}

int WarT_Fm_Set_Volume(uint16 wVolume)
{
	return g_tHeXingFm.fm_setvolume(wVolume);
}

void WarT_Fm_Mute(boolean on)
{
	g_tHeXingFm.fm_mute(on);
}

uint16 WarT_Fm_Get_Current_Channel(void)
{
	return g_tHeXingFm.fm_GetCurrentChannel();
}





