/************************************************************************************************
*
*
*		  (C) Copyright	Telegent Systems, Inc. 2006	All	right reserved.
*				Confidential Information
*
*		  TLG1100  DEVICE ID: 0x2c
*
*		  Description: TLG1100 I2C timing sample code,code need	to OPTIMIZE
*
*		  Author:		 Zeng Hai
*
*		  Last modification	: 2006.	05.	22
*************************************************************************************************/
#include "tv_include.h"
/*
#include "tlg1100Hal.h"
#include "tlg1100Api.h"
#include "tlg1100App.h"
#if TLG_VERSION_MAJOR == 1 && TLG_VERSION_MINOR == 9 && TLG_VERSION_PATCH == 3
#include "tlg1100i2c.h"
#elif TLG_VERSION_MAJOR == 1 && TLG_VERSION_MINOR == 10 && TLG_VERSION_PATCH == 0
#include "tlgi2c.h"
#else
// Unkown version
#endif
#include "Platform.h"
*/
/******************************************************************************************
*			   Macros
*******************************************************************************************/
#define	 I2C_DELAY_UNIT		100 // 12

i2c_rw_cmd_type tv_i2c_command;


/* define I2C interface	*/
#define	SDA					ATV_I2C_SDA			/* I2C serial interface	data I/O */
#define	SCL					ATV_I2C_SCL			/* I2C serial interface	clock input	*/
                    		
                    		
#define	IIC_TRUE			1
#define	IIC_FALSE			0
#define	IIC_ERR				0
#define	IIC_DONE			1


#define	ENTRY(m)								/* These two defines cause warnings when */
#define	EXIT(m)									/* FUNCTION_CALL_TRACING is not defined */

/******************************************************************************************
*			   TYPES
*******************************************************************************************/
typedef	unsigned char	BOOL;


/******************************************************************************************
*			   Prototypes
*******************************************************************************************/

/******************************************************************************************
*
*				FUNCTION
*
*******************************************************************************************/


#ifdef TLG_I2C_USE_BUS
unsigned char Tlg_i2c_write_data(unsigned char dadd, unsigned short	radd, unsigned short *data)
{
    static byte buf[4];
    int i = 0;

    buf[0] = ((radd & 0x7f00)>>8);
    buf[1] =  (radd&0x00ff);
    buf[2] = (((*data)&0xff00)>>8);
    buf[3] = ((*data)&0x00ff);
    
    tv_i2c_command.addr.offset = 0;
    tv_i2c_command.buf_ptr  = buf;
    tv_i2c_command.len      = 4;

    for (i = 0; i < 3; ++i) 
    {
        if (i2c_write(&tv_i2c_command) == I2C_SUCCESS)
        {
            return TRUE;
        }
    }
    
    TLG_PRINT_0("Tlg_i2c_write_data failed\n");
    return FALSE;    
}


unsigned char Tlg_i2c_read_data(unsigned char dadd,	unsigned short radd, unsigned short	*data)
{
    static byte   buf[4];
    int i = 0;
   
    buf[0] = (((radd |0x8000)&0xff00)>>8);
    buf[1] = (radd&0x00ff);
    buf[2] = 0xff;
    buf[3] = 0xff;
    
    tv_i2c_command.addr.offset = 0;
    tv_i2c_command.buf_ptr  = buf;
    tv_i2c_command.len      = 4;
    for (i = 0; i < 3; ++i) 
    {
        if (i2c_write(&tv_i2c_command) == I2C_SUCCESS)
        {            
            break;
        }
    }
    if (i>=3)
    {
        TLG_PRINT_0("Tlg_i2c_read_data failed 1\n");
        return FALSE;
    }
    
    memset(buf,0x00,sizeof(buf));
    tv_i2c_command.addr.offset = 0;
    tv_i2c_command.buf_ptr  = buf;
    tv_i2c_command.len      = 4;
    
    for (i = 0; i < 3; ++i) 
    {
        if (i2c_read(&tv_i2c_command) == I2C_SUCCESS)
        {
            break;
        }
    }
    
    if (i>=3)
    {
        TLG_PRINT_0("Tlg_i2c_read_data failed 2");
        return FALSE;
    }
    
    *data = ((buf[2]<<8)|buf[3]);
    return TRUE;
   
}

#endif

unsigned char Tlg_i2c_read_data_burst(unsigned char dadd, unsigned short radd, unsigned short len ,unsigned short *data)
{
	while(len--)
	{
		if(!Tlg_i2c_read_data(dadd,  radd++, data++))
		{
			return IIC_ERR;
		}
	}
	return IIC_DONE;
}

/************************************************************************
*
*			   TLGI2C_Initcheck()
*
*************************************************************************/
int TLGI2C_Initcheck(unsigned i2c_addr, unsigned short on)
{
	unsigned short sda_data, scl_data; 
     ENTRY(TLGI2C_Initcheck);

	 sda_data = SDA;
	 scl_data = SCL;

	 if (sda_data>0x80)
	 	sda_data -= 0x80;
	 if (scl_data>0x80)
	 	scl_data -= 0x80;
	 

	 return ((sda_data << 3) + scl_data);		
	 //return ((SDA << 3) + SCL);

     EXIT(TLGI2C_Initcheck);
     
}

/* end of file */
