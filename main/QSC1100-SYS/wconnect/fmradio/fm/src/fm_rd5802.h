#ifndef _FM_RD5802_H
#define _FM_RD5802_H
/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "custfmrds.h"          // include global fm rds switches

#ifdef FEATURE_FM

#include "fm_api.h"          // for typedefs
#include "fm_generic.h"      // include generic fm definitions
#include "i2c.h"                // i2c/2-wire defines
#include "gpio_int.h"           // GPIO interrupt defines

#define FM_RD5802_I2C_ID	(0x20)

typedef enum
{
	FM_I2C_WRITE,
	FM_I2C_READ,
	FM_I2C_MAX
}FM_I2C_COMM_TYPE;


#endif //FEATURE_FM

#endif //_FM_RD5802_H