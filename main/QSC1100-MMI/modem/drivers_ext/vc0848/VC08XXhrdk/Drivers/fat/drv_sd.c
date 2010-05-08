#include "thin_usr.h"
#include "drv_sd.h"

#include "mmd_sd_driver.h"

unsigned char sd_initfunc(void) 
{
	unsigned char t;
	MMD_U32 ret;
	for( t =0; t <3; t++)
	{
		ret = MMD_SD_Reset();
		if (ret == MMD_RET_NOERR)
			return 0; //success
	}
	return 1; //fail
}

unsigned char sd_getphy(F_PHY *phy)
{
	unsigned long size = 0;
	size = MMD_SD_GetCapacity();
	phy->number_of_heads = 255;
	phy->sector_per_track = size/64;
	phy->number_of_cylinders = phy->sector_per_track/256;
	phy->number_of_sectors = size;
	if (size)
		return 0; // ok
	else
		return 1;//fail
}

unsigned char sd_readsector(void *data, unsigned long sector)
{
	unsigned long addr;
	MMD_U32 ret;

	addr = sector ;

	ret = MMD_SD_ReadSector((MMD_U08 *)data, addr);
	if (!ret)
		return 0;

	ret = MMD_SD_Reset();
	if (ret) 
		return 1;

	return 2;
}

unsigned char sd_readmultisector(void *data, unsigned long sector, unsigned long count)
{
	MMD_U32 ret;

	ret = MMD_SD_ReadMultiSector((MMD_U08 *)data, sector, count);
	if (!ret)
		return 0;

	ret = MMD_SD_Reset();
	if (ret)
		return 1;

	return 2;
}

unsigned char sd_writesector(void *data, unsigned long sector)
{
	unsigned long addr;
	MMD_U32 ret;

	addr = sector;
	ret = MMD_SD_WriteSector((MMD_U08 *)data, addr);
	if (!ret) 
		return 0;

	ret = MMD_SD_Reset();
	if (ret) 
		return 1;

	return 2;	
}

unsigned char sd_writemultisector(void *data, unsigned long sector, unsigned long count)
{
	MMD_U32 ret;

	ret = MMD_SD_WriteMultiSector((MMD_U08 *)data, sector, count);
	if (!ret) 
		return 0;
	
	ret = MMD_SD_Reset();
	if (ret)
		return 1;

	return 2;
}


unsigned char sd_getstatus(void)
{
	return 0;
}

