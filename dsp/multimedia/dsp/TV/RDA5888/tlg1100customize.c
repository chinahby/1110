#include "tlg1100Hal.h"
#include "tlg1100App.h"
#include "tlg1100i2c.h"
#include "kal_release.h"

TLG1100DLL_API int TLG_ReadReg(uint32 base_addr, uint32 reg_addr, uint16 *val)
{
    int res = TLG_ERR_SUCCESS;
	
    if (reg_addr<0x100)
    	{
    		TLGI2C_WriteReg(base_addr, 0x02, 0x00);//page 0
    	}else{
    		TLGI2C_WriteReg(base_addr, 0x02, 0x01);//page 1
    		reg_addr=reg_addr&0x0ff;
	}
    res = TLG_ReadRegNS(base_addr, reg_addr, val);

    return(res);
}

TLG1100DLL_API int TLG_ReadRegNS(uint32 base_addr, uint32 reg_addr, uint16 *val)
{
    int res;

    res = TLGI2C_ReadReg(base_addr, reg_addr, val);

    return(res);
}

TLG1100DLL_API int TLG_WriteReg(uint32 base_addr, uint32 reg_addr, uint16 val)
{
    int res,i;
    uint16 val1;
	uint32 reg_addr1;

	reg_addr1=reg_addr;
	if (reg_addr < 0x100)
	{
	    TLGI2C_WriteReg(base_addr, 0x02, 0x00);//page 0
	}
	else
	{
	    TLGI2C_WriteReg(base_addr, 0x02, 0x01);//page 1
	    reg_addr=reg_addr&0xff;
	}
    res = TLGI2C_WriteReg(base_addr, reg_addr, val);

	for (i = 0; i < 2; i++)
	{
	    TLG_ReadReg( base_addr, reg_addr1, &val1);
		if((val!=val1)&&(reg_addr1!=0x150)&&(reg_addr1!=0x151)&&(reg_addr1!=0x152)
			&&(reg_addr1!=0x153)&&(reg_addr1!=0x154)&&(reg_addr1!=0x001))
		{ 	
		    kal_prompt_trace(0,"[rdamtv]reg_add:%x var:%x  var_read:%x",reg_addr1,val,val1);
		    TLGI2C_WriteReg(base_addr, reg_addr1, val);	
		}
	    else
		{
		    break;
		}
	}
	
    return(res);
}

void TLG_Delay(uint16 milliseconds)
{
	 uint16 i,j; 
	for(j=0;j<milliseconds;j++)
	{
		 for(i=0;i<30000;i++)
		  i++;
	}
}

