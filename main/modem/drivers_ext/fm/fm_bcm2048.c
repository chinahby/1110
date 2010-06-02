/*===========================================================================

        FM   Radio  Driver for BCM2048 Chipset

DESCRIPTION
  All the declarations and definitions necessary to support interaction
  with the FM Radio functions for the MSM60x0.

REFERENCES

===========================================================================*/
#include "customer.h"
#include "comdef.h"

int fm_set_volume(word wVolume)
{
    return 0;
}
int fm_tune_channel(word wChannel)
{
    return 0;
}

int fm_seek_start(boolean bIsUp, boolean bIsWrap)
{
    return 0;
}

int fm_get_seek_status(boolean* pIsFinish, boolean* pIsBandLimit, word* pChannel)
{
    return 0;
}

void fm_mute(boolean on)
{
}

int fm_radio_power_down(void)
{
    return 0;
}
boolean fm_radio_is_power_up(void)
{
    return TRUE;
}

boolean fm_radio_is_inited(void)
{
    return TRUE;
}
int fm_radio_init(void)
{
    return 0;
}

int fm_radio_power_up(void)
{
    return 0;
}

