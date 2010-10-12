/************************************************************************************************
*
*
*         (C) Copyright Telegent Systems, Inc. 2006 All right reserved.
*               Confidential Information
* 
*         TLG1100  DEVICE ID: 0x2c
*
*         Description: TLG1100 I2C timing sample code,code need to OPTIMIZE 
*                          
*         Author:        Zeng Hai
*
*         Last modification : 2006. 05. 22
*************************************************************************************************/
/******************************************************************************************
*              Macros
*******************************************************************************************/
#include "tlgInclude.h"
#include "gpio_1100.h"
#include "i2c.h"

#define TLG_I2C_USE_BUS		//real I2C

#define	 I2C_DELAY_UNIT		50//100

#define TLG_PRINT_0(x)
/* define I2C interface	*/
#define	SDA					I2C_SDA			/* I2C serial interface	data I/O */
#define	SCL					I2C_SCL			/* I2C serial interface	clock input	*/
                    		
                    		
#define	IIC_TRUE			1
#define	IIC_FALSE			0
#define	IIC_ERR				0
#define	IIC_DONE			1


#define	ENTRY(m)								/* These two defines cause warnings when */
#define	EXIT(m)									/* FUNCTION_CALL_TRACING is not defined */

/******************************************************************************************
*              TYPES
*******************************************************************************************/


/******************************************************************************************
*              Prototypes
*******************************************************************************************/
//void i2c_init(BOOL on);

static void i2c_delay(unsigned int time);


#define set_i2c_pin(pin)             gpio_out(pin, GPIO_HIGH_VALUE)
#define clr_i2c_pin(pin)             gpio_out(pin, GPIO_LOW_VALUE)
#define set_i2c_pin_dir(dir)         //gpio_set_direction(pin, dir)           
#define get_i2c_pin(pin)            (unsigned char)gpio_in(pin)

static void i2c_begin(void);
static void i2c_end(void);
static void	i2c_write_ack(unsigned char	flag);
static unsigned char i2c_read_ack(void);
static unsigned char  i2c_read_byte(void);
static unsigned char i2c_write_byte(unsigned char data);

/* Used to send I2C command */
static i2c_rw_cmd_type dsp_i2c_command = {0};

#ifndef WIN32

/******************************************************************************************
*              Functions
*******************************************************************************************/
static unsigned char i2c_read_byte(void)
{
	unsigned char i;
	unsigned char ret;

	ENTRY(i2c_read_byte);
	ret	= 0;
	
	set_i2c_pin_dir(0);//set gpio for input
	
	
	for	(i=0; i<8; i++)
	{			
		i2c_delay(I2C_DELAY_UNIT << 0);
		set_i2c_pin(SCL);	
		i2c_delay(I2C_DELAY_UNIT << 0);
		ret	= ret<<1;
		if (get_i2c_pin(SDA))
			ret |= 1;
		i2c_delay(I2C_DELAY_UNIT << 0);
		clr_i2c_pin(SCL);
		i2c_delay(I2C_DELAY_UNIT <<	0);

		if (i==7)
		{
			set_i2c_pin_dir(1);//set gpio for output
		}
		i2c_delay(I2C_DELAY_UNIT <<	0);
	}		

	EXIT(i2c_read_byte);
	return ret;
}

static unsigned char i2c_write_byte(unsigned char data)
{
	unsigned char i;
	
	ENTRY(i2c_write_byte);
	set_i2c_pin_dir(1);//set gpio for output
	
	for	(i=0; i<8; i++)
	{
		i2c_delay(I2C_DELAY_UNIT << 0);
		if (data & 0x80)
			set_i2c_pin(SDA);
		else
			clr_i2c_pin(SDA);
		data <<= 1;

		i2c_delay(I2C_DELAY_UNIT << 0);
		set_i2c_pin(SCL);
		i2c_delay(I2C_DELAY_UNIT << 0);
		clr_i2c_pin(SCL);		
	}	
	EXIT(i2c_write_byte);
	return i2c_read_ack();
}

/***************************************************************************
*
*
*    IIC Write data for Tlg1100
*                                    dab:device Id =0x2c
*                                    rab: register address
*                                    data: dump into register
*
****************************************************************************/

#ifdef TLG_I2C_USE_BUS
int TLGI2C_WriteReg(unsigned char dadd, unsigned short	radd, unsigned short data)
{
    static byte buf[4];
    int i = 0;

    buf[0] = ((radd & 0x7f00)>>8);
    buf[1] =  (radd&0x00ff);
    buf[2] = (((data)&0xff00)>>8);
    buf[3] = ((data)&0x00ff);
    
    dsp_i2c_command.addr.offset = 0;
    dsp_i2c_command.buf_ptr  = buf;
    dsp_i2c_command.len      = 4;

    for (i = 0; i < 3; ++i) 
    {
        if (i2c_write(&dsp_i2c_command) == I2C_SUCCESS)
        {
            return TRUE;
        }
    }
    
    TLG_PRINT_0("Tlg_i2c_write_data failed\n");
    return FALSE;    
}


int TLGI2C_ReadReg(unsigned char dadd,	unsigned short radd, unsigned short	*data)
{
    static byte   buf[4];
    int i = 0;
   
    buf[0] = (((radd |0x8000)&0xff00)>>8);
    buf[1] = (radd&0x00ff);
    buf[2] = 0xff;
    buf[3] = 0xff;
    dsp_i2c_command.addr.offset = 0;
    dsp_i2c_command.buf_ptr  = buf;
    dsp_i2c_command.len      = 4;
    for (i = 0; i < 3; ++i) 
    {
        if (i2c_write(&dsp_i2c_command) == I2C_SUCCESS)
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
    dsp_i2c_command.addr.offset = 0;
    dsp_i2c_command.buf_ptr  = buf;
    dsp_i2c_command.len      = 4;
    for (i = 0; i < 3; ++i) 
    {
        if (i2c_read(&dsp_i2c_command) == I2C_SUCCESS)
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

#else

/***************************************************************************
*
*
*	 IIC Write data	for	TLG1100
*	 Parameter:
*			   dadd:device address (must be	0x58)
*			   radd:register address
*
****************************************************************************/
int TLGI2C_WriteReg(unsigned char dadd, unsigned short	radd, unsigned short data)
{
	unsigned char tmpData =	0;


	ENTRY(TLGI2C_WriteReg);



	i2c_begin();
	if (!i2c_write_byte(dadd<<1))
	{
		i2c_end();
        TLG_PRINT_0("Tlg_i2c_write_data error");
		return IIC_ERR;
	}
	/* write 16bits	register */
	  tmpData =	((radd & 0x7f00) >>	8);

	if (!i2c_write_byte(tmpData))
	{
		i2c_end();
        TLG_PRINT_0("Tlg_i2c_write_data error");
		return IIC_ERR;
	}

	tmpData	= (radd	& 0x00ff);
	if (!i2c_write_byte(tmpData))
	{
		i2c_end();
        TLG_PRINT_0("Tlg_i2c_write_data error");
		return IIC_ERR;
	}

	 /*	write 16bits data */
	tmpData	= (((*data)	& 0xff00) >> 8);
	if (!i2c_write_byte(tmpData))
	{
		i2c_end();
        TLG_PRINT_0("Tlg_i2c_write_data error");
		return IIC_ERR;
	}

	tmpData	= ((*data) & 0x00ff);
	if (!i2c_write_byte(tmpData))
	{
		i2c_end();
        TLG_PRINT_0("Tlg_i2c_write_data error");
		return IIC_ERR;
	}


	i2c_end();


	EXIT(TLGI2C_WriteReg);

	return IIC_DONE;
}

/***************************************************************************
*
*
*	 IIC Read data for TLG1100
*	 Parameter:
*			   dadd:device add (must be	0x58)
*			   radd:register add
*
****************************************************************************/
int TLGI2C_ReadReg(unsigned char dadd,	unsigned short radd, unsigned short	*data)
{
	unsigned char tmpData =	0;


	ENTRY(TLGI2C_ReadReg);


	i2c_begin();
	if (!i2c_write_byte(dadd<<1))
	{
		i2c_end();
        TLG_PRINT_0("Tlg_i2c_read_data error");
		return IIC_ERR;
	}
	/* set MSB 1 */
	tmpData	= (((radd |	0x8000)	& 0xff00) >> 8);
	if (!i2c_write_byte(tmpData))
	{
		i2c_end();
        TLG_PRINT_0("Tlg_i2c_read_data error");
		return IIC_ERR;
	}
	tmpData	= ((radd & 0x00ff));
	if (!i2c_write_byte(tmpData))
	{
		i2c_end();
        TLG_PRINT_0("Tlg_i2c_read_data error");
		return IIC_ERR;
	}
///////////////////////////////////////////////////////////////////////////////////////////
//  ???????????????????????
//////////////////////////////////////////////////////////////////////////////////////////
	   /* write	any	data into this register	*/
#if 1
	if (!i2c_write_byte(0xff))
	{
		i2c_end();
        TLG_PRINT_0("Tlg_i2c_read_data error");
		return IIC_ERR;
	}
	if (!i2c_write_byte(0xff))
	{
		i2c_end();
        TLG_PRINT_0("Tlg_i2c_read_data error");
		return IIC_ERR;
	}
#endif    
	i2c_end();	/* stop	bit	*/
/////////////////////////////////////////////////////////////////////////////////////////

	/* start again */
	i2c_begin();

	if (!i2c_write_byte((unsigned char)((dadd<<1) |	1)))
	{
		i2c_end();
        TLG_PRINT_0("Tlg_i2c_read_data error");
		return IIC_ERR;
	}

//////////////////////////////////////////////////////////
//			???????????????????
/////////////////////////////////////////////////////////
#if 1
	*data =	i2c_read_byte();
	i2c_write_ack(0);

	*data =	i2c_read_byte();
	i2c_write_ack(0);
#endif
//////////////////////////////////////////////////////////

	  tmpData =	i2c_read_byte();
	  i2c_write_ack(0);

	  *data	= (tmpData << 8) & 0xff00;

	  tmpData =	i2c_read_byte();
	  i2c_write_ack(1);					 /*	can	not	send ACK,must send NAck	*/

	  *data	|= tmpData;

	i2c_end();


	EXIT(TLGI2C_ReadReg);


	return IIC_DONE;
}

#endif

/*****************************************************
*
*			   TLGI2C_Initcheck()
*
*************************************************************************/
int TLGI2C_Initcheck(unsigned i2c_addr, unsigned short on)
{
     ENTRY(TLGI2C_Initcheck);
	 return ((SDA<<3)+SCL);
     EXIT(TLGI2C_Initcheck);
     
}


/************************************************************************
*
*			   PULL	LOW	SDA/SCK	PIN
*
*************************************************************************/
/************************************************************************
*
*			   READ	DATA FROM SDA PIN
*
*************************************************************************/
/**************************************************************************
*
*		   IIC START BIT
*
***************************************************************************/
static void i2c_begin(void)
{
		ENTRY(i2c_begin);
	set_i2c_pin_dir(1);//set gpio for output
	
	i2c_delay(I2C_DELAY_UNIT << 0);
	set_i2c_pin(SDA);	
	i2c_delay(I2C_DELAY_UNIT << 0);
	set_i2c_pin(SCL);
	i2c_delay(I2C_DELAY_UNIT << 0);
	clr_i2c_pin(SDA);	
	i2c_delay(I2C_DELAY_UNIT << 0);
	clr_i2c_pin(SCL);
	i2c_delay(I2C_DELAY_UNIT << 0);
		EXIT(i2c_begin);
}

/**************************************************************************
*
*		   IIC STOP	BIT
*
***************************************************************************/
static void i2c_end(void)
{
		ENTRY(i2c_end);
	set_i2c_pin_dir(1);//set gpio for output
	
	i2c_delay(I2C_DELAY_UNIT << 2);
	clr_i2c_pin(SDA);
	i2c_delay(I2C_DELAY_UNIT << 2);
	set_i2c_pin(SCL);
	i2c_delay(I2C_DELAY_UNIT << 3);
	set_i2c_pin(SDA);
	i2c_delay(I2C_DELAY_UNIT << 4);
		EXIT(i2c_end);
}

static void i2c_write_ack(unsigned char flag)
{
	  ENTRY(i2c_write_ack);
	set_i2c_pin_dir(1);//set gpio for output
	
	if(flag)
		set_i2c_pin(SDA);
	else
		clr_i2c_pin(SDA);
	i2c_delay(I2C_DELAY_UNIT << 0);
	set_i2c_pin(SCL);
	i2c_delay(I2C_DELAY_UNIT << 0);
	clr_i2c_pin(SCL);
	i2c_delay(I2C_DELAY_UNIT << 0);
	//set_i2c_pin(SDA);				  // need to verify	here
	i2c_delay(I2C_DELAY_UNIT <<	0);

	  EXIT(i2c_write_ack);
}


static unsigned char i2c_read_ack(void)
{	
	unsigned char ret;
	
	   ENTRY(i2c_read_ack);
	/* set SDA as input	status here	*/
	set_i2c_pin_dir(0);//set gpio for input
	
	i2c_delay(I2C_DELAY_UNIT << 0);
	set_i2c_pin(SCL);
	i2c_delay(I2C_DELAY_UNIT << 0);
	if (!get_i2c_pin(SDA))
	{
		ret	= IIC_TRUE;
	}
	else
	{
		ret	= IIC_FALSE;
	}

	i2c_delay(I2C_DELAY_UNIT << 0);
	clr_i2c_pin(SCL);
    i2c_delay(I2C_DELAY_UNIT << 0);	

	   /* set SDA as output	status here	*/
	set_i2c_pin_dir(1);//set gpio for input
	i2c_delay(I2C_DELAY_UNIT <<	0);
	EXIT(i2c_read_ack);
	return ret;
}
DECLSPEC TLGI2C_WriteBurst(unsigned char i2c_addr, unsigned short start_addr, int length, unsigned	short *	data)
{
	return 0;
}

DECLSPEC TLGI2C_ReadBurst(unsigned char i2c_addr, unsigned short start_addr, int length, unsigned short * data)
{
	return 0;
}

static void i2c_delay(unsigned int time)
{
	while(time--) 
	{	
        ;
	}
}

/* end of file */
#endif

