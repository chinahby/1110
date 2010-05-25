/*--------------------------------------------------------------------

			COPYRIGHT (C) 2008, Vimicro Corporation
                        ALL RIGHTS RESERVED

 This source code has been made available to you by VIMICRO on an
 AS-IS basis. Anyone receiving this source code is licensed under VIMICRO
 copyrights to use it in any way he or she deems fit, including copying it,
 modifying it, compiling it, and redistributing it either with or without
 modifications. Any person who transfers this source code or any derivative
 work must include the VIMICRO copyright notice and this paragraph in
 the transferred software.

 Module:		mmd_sd_driver.c.
 
 Description:	VC0848 SDPIO Driver functions.
 
 Update History:
	1.0			Initial version					2008.04.08		Chen Jing'en

-----------------------------------------------------------------------*/

#include "mmd_sd_driver.h"

/********************************************************************
 Description:
		Reset SD Card.

 Parameters:
		 	
	
 Returns:
		Return MMD_RET_NOERR if success, else return error value.

 Remarks:
 		The unit of card capacity is sector (512 bytes).
 		
 Example:

       
*********************************************************************/
MMD_U32 MMD_SD_Reset(void)
{
	MMD_S32 ret;

	//ret = Vsd_Reset(4, 0, 0x003f0000);
	ret = Vsd_Reset(4, 0, 2);
	if(ret < 0) 
	{
		return MMD_RET_ERR;
	}

	return MMD_RET_NOERR;
}

/********************************************************************
 Description:
 		Read data from specified sector.

 Parameters:
 		pbDataBuf: 	Pointer to the user-supplied buffer that is to receive the data read from card.
		dSecAddr:	The sector address of SD card to read.
				
 Returns:
		If success return MMD_RET_NOERR, else error value.
 		  
 Remarks:
 		
 Example:

		 		
*********************************************************************/
MMD_U32	MMD_SD_ReadSector(MMD_U08 *pbDataBuf, MMD_U32 dSecAddr)
{
	MMD_U32 sram_addr;
	MMD_S32 ret;

	if (NULL == pbDataBuf)
		return MMD_RET_INVALIDPARAM;

	sram_addr = MMD_SYS_Malloc(SD_BLOCK_SIZE);
	if(sram_addr == NULL)
		return MMD_RET_MALLOC_ERR;

	//ret = Vsd_ReadOneBlock(dSecAddr, sram_addr);
	ret = Vsd_RdMultiAtOnce(dSecAddr, sram_addr, 1);
	if(ret != VSD_SUCCESS)
	{
		ret = MMD_RET_ERR;
	}
	else
	{
		if((MMD_U32)pbDataBuf&0x3)
		{
			MMD_S08 *ptr = MMD_SD_Malloc(SD_BLOCK_SIZE);
			if(ptr != NULL)
			{
				Vm_RdSram(sram_addr, ptr, SD_BLOCK_SIZE);
				MMD_SD_MemCpy((MMD_S08 *)pbDataBuf, ptr, SD_BLOCK_SIZE);
				MMD_SD_Free(ptr);
				ret = MMD_RET_NOERR;
			}
			else
			{
				ret = MMD_RET_MALLOC_ERR;
			}
		}
		else
		{
			Vm_RdSram(sram_addr, (MMD_S08 *)pbDataBuf, SD_BLOCK_SIZE);
			ret = MMD_RET_NOERR;
		}		
	}	

	MMD_SYS_Free(sram_addr);

	return ret;
}

/********************************************************************
 Description:
 		Write data into specified sector.

 Parameters:
 		pbDataBuf: 	Pointer to the user-supplied buffer that contains the data to be written to the card.
		dSecAddr: 	The sector address of SD card to write.
				
 Returns:
		If success return MMD_RET_NOERR, else error value.
 		  
 Remarks: 		
 		
 Example:

		 		
*********************************************************************/
MMD_U32	MMD_SD_WriteSector(MMD_U08 *pbDataBuf, MMD_U32 dSecAddr)
{
	MMD_U32 sram_addr;
	MMD_S32 ret;
	
	if (NULL == pbDataBuf)
		return MMD_RET_INVALIDPARAM;

	sram_addr = MMD_SYS_Malloc(SD_BLOCK_SIZE);
	if(sram_addr == NULL)
		return MMD_RET_MALLOC_ERR;

	if((MMD_U32)pbDataBuf&0x3)
	{
		MMD_S08 *ptr = MMD_SD_Malloc(SD_BLOCK_SIZE);
		if(ptr != NULL)
		{
			MMD_SD_MemCpy(ptr, (MMD_S08 *)pbDataBuf, SD_BLOCK_SIZE);
			Vm_WrSram(sram_addr, ptr, SD_BLOCK_SIZE);
			MMD_SD_Free(ptr);
		}
		else
		{
			MMD_SYS_Free(sram_addr);			
			return MMD_RET_MALLOC_ERR;
		}
	}
	else
	{
		Vm_WrSram(sram_addr, (MMD_S08 *)pbDataBuf, SD_BLOCK_SIZE);
	}		
	
	//ret = Vsd_WriteOneBlock(dSecAddr, sram_addr);
	ret = Vsd_WrMultiAtOnce(dSecAddr, sram_addr, 1);
	if(ret != VSD_SUCCESS)
	{
		ret = MMD_RET_ERR;
	}
	else
	{
		ret = MMD_RET_NOERR;
	}
	
	MMD_SYS_Free(sram_addr);

	return ret;
}

/********************************************************************
 Description:
 		Read data from multi sectors.

 Parameters:
 		pbDataBuf: 		Pointer to the user-supplied buffer that is to receive the data read from card.
		dSecAddr:		The start sector address to read.
		dNum:			The number of sector to read
				
 Returns:
		If success return MMD_RET_NOERR, else error value.
 		  
 Remarks:
 		
 Example:

		 		
*********************************************************************/
MMD_U32 MMD_SD_ReadMultiSector(MMD_U08 *pbDataBuf, MMD_U32 dSecAddr, MMD_U32 dNum)
{
	MMD_U32 sram_addr, nitems, item, nsectors, i;
	MMD_S32 ret;

	if (NULL == pbDataBuf)
		return MMD_RET_INVALIDPARAM;

	item = dNum > SD_MULTI_MAX_ITEM_SIZE ? SD_MULTI_MAX_ITEM_SIZE : dNum;

	sram_addr = MMD_SYS_Malloc(item*SD_BLOCK_SIZE);
	if(sram_addr == NULL)
		return MMD_RET_MALLOC_ERR;

	nitems = (dNum + item -1)/item;

	for(i = 1; i <= nitems; i++)
	{
		if(i == nitems)
		{
			nsectors = dNum%item;
			nsectors = nsectors > 0? nsectors : item;
		}
		else
		{
			nsectors = item;
		}
		
		ret = Vsd_RdMultiAtOnce(dSecAddr, sram_addr, nsectors);
		if(ret != VSD_SUCCESS)
		{
			ret = MMD_RET_ERR;
			break;
		}
		else
		{
			if((MMD_U32)pbDataBuf&0x3)
			{
				MMD_S08 *ptr = MMD_SD_Malloc(nsectors*SD_BLOCK_SIZE);
				if(ptr != NULL)
				{
					Vm_RdSram(sram_addr, ptr, nsectors*SD_BLOCK_SIZE);
					MMD_SD_MemCpy((MMD_S08 *)pbDataBuf, ptr, nsectors*SD_BLOCK_SIZE);
					MMD_SD_Free(ptr);
				}
				else
				{
					ret = MMD_RET_MALLOC_ERR;
					break;
				}
			}
			else
			{
				Vm_RdSram(sram_addr, (MMD_S08 *)pbDataBuf, nsectors*SD_BLOCK_SIZE);
			}	
			pbDataBuf += nsectors*SD_BLOCK_SIZE;
			dSecAddr += nsectors;
			ret = MMD_RET_NOERR;
		}
	}
	
	MMD_SYS_Free(sram_addr);

	return ret;	
}

/********************************************************************
 Description:
 		Write data into multi sectors.

 Parameters:
 		pBuf: 	Pointer to the user-supplied buffer that contains the data to be written to the card.
 		dSec:	The start sector address to write
		dNum:	The number of sector to write.
				
 Returns:
		If success return VRES_SUCCESS, else error value.
 		  
 Remarks:
 		
 Example:
 		Example_SD
		 		
*********************************************************************/
MMD_U32	MMD_SD_WriteMultiSector(MMD_U08 *pbDataBuf, MMD_U32 dSecAddr, MMD_U32 dNum)
{
	MMD_U32 sram_addr, nitems, item, nsectors, i;
	MMD_S32 ret;

	if (NULL == pbDataBuf)
		return MMD_RET_INVALIDPARAM;

	item = dNum > SD_MULTI_MAX_ITEM_SIZE ? SD_MULTI_MAX_ITEM_SIZE : dNum;

	sram_addr = MMD_SYS_Malloc(item*SD_BLOCK_SIZE);
	if(sram_addr == NULL)
		return MMD_RET_MALLOC_ERR;

	nitems = (dNum + item - 1)/item;


	for(i=1; i<=nitems; i++)
	{
		if(i==nitems)
		{
			nsectors = dNum%item;
			nsectors = nsectors > 0? nsectors : item;
		}
		else
		{
			nsectors = item;
		}

		if((MMD_U32)pbDataBuf&0x3)
		{
			MMD_S08 *ptr = MMD_SD_Malloc(nsectors*SD_BLOCK_SIZE);
			if(ptr != NULL)
			{
				MMD_SD_MemCpy(ptr, (MMD_S08 *)pbDataBuf, nsectors*SD_BLOCK_SIZE);
				Vm_WrSram(sram_addr, ptr, nsectors*SD_BLOCK_SIZE);
				MMD_SD_Free(ptr);
			}
			else
			{
				MMD_SYS_Free(sram_addr);			
				return MMD_RET_MALLOC_ERR;
			}
		}
		else
		{
			Vm_WrSram(sram_addr, (MMD_S08 *)pbDataBuf, nsectors*SD_BLOCK_SIZE);
		}	
		
		
		ret = Vsd_WrMultiAtOnce(dSecAddr, sram_addr, nsectors);
		if(ret != VSD_SUCCESS)
		{
			ret = MMD_RET_ERR;
			break;
		}
		else
		{
			pbDataBuf += nsectors*SD_BLOCK_SIZE;
			dSecAddr += nsectors;
			ret = MMD_RET_NOERR;
		}
	}
	
	MMD_SYS_Free(sram_addr);

	return ret;
}

/********************************************************************
 Description:
 		Get SD card information.

 Parameters:
		pInfo:	Pointer to SDIO_CARD_INFORMATION structure to store SD card infomation
						
 Returns:
		If success return MMD_RET_NOERR, else error value.
 		  
 Remarks:
 		
 Example:

		 		
*********************************************************************/
MMD_U32	MMD_SD_GetInfo(SDIO_CARD_INFORMATION *psInfo)
{

	if (NULL == psInfo)
		return MMD_RET_INVALIDPARAM;

	Vsd_GetCardInfo(psInfo);

	return MMD_RET_NOERR;
}

/********************************************************************
 Description:
 		Get SD card capactity.

 Parameters:
		This function has no parameters. 
								
 Returns:
		Return the card capacity.
 		  
 Remarks:
 		The unit of card capacity is sector (512 bytes).
 		
 Example:
 				 		
*********************************************************************/
MMD_U32	MMD_SD_GetCapacity(void)
{
	return Vsd_GetCapacity();
}

/********************************************************************
 Description:
 		Get the minimum size of data can be erased.

 Parameters:
		This function has no parameters. 
								
 Returns:
		Return the minimum size of data can be erased.
 		  
 Remarks:
 		The unit of return value is sector (512 bytes).
 		If a card has the minimum erasable size that larger than 1, the erase operation
 		should be aligned with this size. i.e. if the size is 32, sectors form 0 to 63 is
 		valid erase area, but erase from 1 to 64 is not allowed.

 See Also: 
 		MMD_SD_Erase
 		
 Example:
 				 		
*********************************************************************/
MMD_U32	MMD_SD_GetMinErasableSize(void)
{
	return Vsd_GetMinErasableSize();
}

/********************************************************************
 Description:
 		Erase the specified area in SD card.

 Parameters:
		dSecAddr:		The start sector address to erase.
		dNum:			The number of sector to read
								
 Returns:
		Return the number of erased sectors 
 		  
 Remarks:
 		If a card has the minimum erasable size that larger than 1, dSecAddr & dNum should be
 		multiple of the size, otherwise the area erased actually might smaller than specified.

 See Also:
 		MMD_SD_GetMinErasableSize
 		
 Example:
 				 		
*********************************************************************/
MMD_U32	MMD_SD_Erase(MMD_U32 dSecAddr, MMD_U32 dNum)
{
	return Vsd_Erase(dSecAddr, (dSecAddr + dNum - 1));
}
