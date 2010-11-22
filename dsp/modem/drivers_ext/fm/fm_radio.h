#ifndef __FM_RADIO_H_
#define __FM_RADIO_H_
/*===========================================================================

        FM   Radio  Publice Interface Header

DESCRIPTION
  All the declarations and definitions necessary to support interaction
  with the FM Radio functions for the MSM60x0.

REFERENCES

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "gpio_1100.h"
#include "bio.h"
#include "i2c.h"
#include "clk.h"
#include "msg.h"

#define FM_RADIO_SUCCESSFUL     0
#define FM_RADIO_FAILED         1
#if 0

typedef enum
{
    FM_NOT_INIT,
    FM_POWER_DOWN,
    FM_POWER_UP,
    FM_IDLE_STATUS = FM_POWER_UP,
    FM_IN_PROGRESS,
    
    FM_MAX_STATUS = 0xFFFF
}   WarT_Fm_Status_e;
void fm_radio_pre_init(void);

int fm_radio_init(void);

int fm_radio_power_up(void);

int fm_radio_power_down(void);

int fm_tune_channel(word wChannel);

//Synchronization Seek Function
int fm_seek_up(word* pChannel);
int fm_seek_down(word* pChannel);

//Asynchronization Seek Function
int fm_seek_start(boolean bIsUp, boolean bIsWrap);
int fm_get_seek_status(boolean* pIsFinish, boolean* pIsBandLimit, word* pChannel);

int fm_set_volume(word wVolume);

boolean fm_radio_is_inited( void);
boolean fm_radio_is_power_up( void);
boolean fm_radio_is_power_down( void);
boolean fm_radio_is_tuning( void);
word    fm_radio_get_playing_channel( void);
void fm_mute(boolean on);
#endif
#endif //__FM_RADIO_H_
//#define VC0848_HEADSET_PA      //PA OPTION
