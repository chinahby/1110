#ifndef WIN32

#include "TlgInclude.h"

#include "Platform.h"


static void ATV_InitHostI2C()
{

}

static void ATV_DeInitHostI2C()
{

}

static void ATV_ChipPowerOn()
{

}

static void ATV_ChipPowerOff()
{

}

static void ATV_ChipReset()
{

}

static void ATV_VerifyI2C()
{
	AIT_Message_P0("ATV_VerifyI2C Test+");

	
}

void PowerOnFM(void)
{

	
}

void PowerOffFM(void)
{


}

/*************************************************************************
* FUNCTION
* init_ATV
*
* DESCRIPTION
* This function initialize the ATV and ISP control register.
*
* PARAMETERS
* None
*
* RETURNS
* None
*
* GLOBALS AFFECTED
*
*************************************************************************/


void init_ATV(void)
{
}

void get_ATV_id(unsigned char *sensor_write_id, unsigned char *sensor_read_id)
{
	*sensor_write_id=0x48;
	*sensor_read_id=0x49;
}





unsigned short ATV_Get_ScanLines(void)
{
//	if(gTvScanLines==TLG_TV_525_LINES)
//		return 525;
//	else
		return 625;
}

/*************************************************************************
* FUNCTION
* power_off_ATV
*
* DESCRIPTION
* This function is to turn off ATV power.
*
* PARAMETERS
* None
*
* RETURNS
* None
*
* GLOBALS AFFECTED
*
*************************************************************************/

void power_off_ATV(void)
{
}



#endif

