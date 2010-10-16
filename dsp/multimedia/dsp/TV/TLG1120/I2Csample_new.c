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
static void	i2c_delay(unsigned int time);


static void	i2c_begin(void);
static void	i2c_end(void);
static void	i2c_write_ack(unsigned char	flag);
static unsigned	char i2c_read_ack(void);
static unsigned	char i2c_read_byte(void);
static unsigned	char i2c_write_byte(unsigned char data);


/******************************************************************************************
*
*				FUNCTION
*
*******************************************************************************************/
#if 0
static boolean camsemsor_siv100b_i2c_write_byte(uint16 offset, uint8 data)
{
    static uint8 swapbytes;
    uint8 i;

    camsensor_i2c_command.addr.offset = offset;
    camsensor_i2c_command.buf_ptr  = (byte *)(&swapbytes);
    camsensor_i2c_command.len      = 1;

    for (i = 0; i < 3; ++i) 
    {
        if (i2c_write(&camsensor_i2c_command) == I2C_SUCCESS)
        {
            return TRUE;
        }
    }

    return FALSE;
}




static boolean camsemsor_siv100b_i2c_read_byte(uint16 offset, uint8 *data) 
{ 
    static uint8 swapbytes; 
    uint8  i;

    if (data == NULL) 
    {
        return FALSE;
    }

    camsensor_i2c_command.addr.offset = offset;
    camsensor_i2c_command.buf_ptr  = (byte *)(&swapbytes);
    camsensor_i2c_command.len      = 1;

    for (i =0; i < 3; ++i)
    {
        if (i2c_read(&camsensor_i2c_command) == I2C_SUCCESS)
        {
            *data = swapbytes;
            return TRUE;
        }
    }
    return FALSE;
}

#endif


static unsigned	char i2c_read_byte(void)
{
	unsigned char i;
	unsigned char ret;

	ENTRY(i2c_read_byte);

	ret	= 0;

	/* set SDA as intput status	here */
	//GPIO_InitIO(0, SDA);
	#if 0   //modi by yangdecai  2010-10-15
	set_i2c_pin_dir(SDA,I2C_PIN_INPUT);
	#endif 

	for	(i=0; i<8; i++)
	{
		i2c_delay(I2C_DELAY_UNIT <<	0);
		set_i2c_pin(SCL);
		i2c_delay(I2C_DELAY_UNIT <<	0);
		ret	= ret<<1;
		if (get_i2c_pin(SDA))
			ret	|= 1;
		i2c_delay(I2C_DELAY_UNIT <<	0);
		clr_i2c_pin(SCL);
		i2c_delay(I2C_DELAY_UNIT <<	0);

			if (i==7)
            {
				   /* set SDA as output	status here	*/
			  // GPIO_InitIO(1, SDA);
			  #if 0   //modi by yangdecai  2010-10-15
				set_i2c_pin_dir(SDA,I2C_PIN_OUTPUT);
			  #endif 
			  
		    }

			i2c_delay(I2C_DELAY_UNIT <<	0);
	}


	EXIT(i2c_read_byte);

	return ret;
}

static unsigned	char i2c_write_byte(unsigned char data)
{
	unsigned char i;


	ENTRY(i2c_write_byte);

	for	(i=0; i<8; i++)
	{
		i2c_delay(I2C_DELAY_UNIT <<	0);
		if (data & 0x80)
			set_i2c_pin(SDA);
		else
			clr_i2c_pin(SDA);
		data <<= 1;

		i2c_delay(I2C_DELAY_UNIT <<	0);
		set_i2c_pin(SCL);
		i2c_delay(I2C_DELAY_UNIT <<	0);
		clr_i2c_pin(SCL);
	}


	EXIT(i2c_write_byte);

	return i2c_read_ack();
}


#ifdef TLG_I2C_USE_BUS
unsigned char Tlg_i2c_write_data(unsigned char dadd, unsigned short	radd, unsigned short *data)
{
    static byte buf[4];
    int i = 0;
#if 0
    if (dadd != camsensor_i2c_command.slave_addr)
    {
        TLG_PRINT_0("Tlg_i2c_write_data slave_addr error\n");
        return FALSE;
    }
#endif
    buf[0] = ((radd & 0x7f00)>>8);
    buf[1] =  (radd&0x00ff);
    buf[2] = (((*data)&0xff00)>>8);
    buf[3] = ((*data)&0x00ff);
    
    camsensor_i2c_command.addr.offset = 0;
    camsensor_i2c_command.buf_ptr  = buf;
    camsensor_i2c_command.len      = 4;

    for (i = 0; i < 3; ++i) 
    {
        if (i2c_write(&camsensor_i2c_command) == I2C_SUCCESS)
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
#if 0
    if (dadd != camsensor_i2c_command.slave_addr)
    {
        TLG_PRINT_0("Tlg_i2c_read_data slave_addr error\n");
        return FALSE;
    }
#endif    
    buf[0] = (((radd |0x8000)&0xff00)>>8);
    buf[1] = (radd&0x00ff);
    buf[2] = 0xff;
    buf[3] = 0xff;
    camsensor_i2c_command.addr.offset = 0;
    camsensor_i2c_command.buf_ptr  = buf;
    camsensor_i2c_command.len      = 4;
    for (i = 0; i < 3; ++i) 
    {
        if (i2c_write(&camsensor_i2c_command) == I2C_SUCCESS)
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
    camsensor_i2c_command.addr.offset = 0;
    camsensor_i2c_command.buf_ptr  = buf;
    camsensor_i2c_command.len      = 4;
    for (i = 0; i < 3; ++i) 
    {
        if (i2c_read(&camsensor_i2c_command) == I2C_SUCCESS)
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
unsigned char Tlg_i2c_write_data(unsigned char dadd, unsigned short	radd, unsigned short *data)
{
	unsigned char tmpData =	0;


	ENTRY(Tlg_i2c_write_data);



	i2c_begin();
	if (!i2c_write_byte(dadd<<1))
	{
		i2c_end();
        //TLG_PRINT_0("Tlg_i2c_write_data error");
		return IIC_ERR;
	}
	/* write 16bits	register */
	  tmpData =	((radd & 0x7f00) >>	8);

	if (!i2c_write_byte(tmpData))
	{
		i2c_end();
        //TLG_PRINT_0("Tlg_i2c_write_data error");
		return IIC_ERR;
	}

	tmpData	= (radd	& 0x00ff);
	if (!i2c_write_byte(tmpData))
	{
		i2c_end();
        //TLG_PRINT_0("Tlg_i2c_write_data error");
		return IIC_ERR;
	}

	 /*	write 16bits data */
	tmpData	= (((*data)	& 0xff00) >> 8);
	if (!i2c_write_byte(tmpData))
	{
		i2c_end();
        //TLG_PRINT_0("Tlg_i2c_write_data error");
		return IIC_ERR;
	}

	tmpData	= ((*data) & 0x00ff);
	if (!i2c_write_byte(tmpData))
	{
		i2c_end();
        //TLG_PRINT_0("Tlg_i2c_write_data error");
		return IIC_ERR;
	}


	i2c_end();


	EXIT(Tlg_i2c_write_data);

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
unsigned char Tlg_i2c_read_data(unsigned char dadd,	unsigned short radd, unsigned short	*data)
{
	unsigned char tmpData =	0;


	ENTRY(Tlg_i2c_read_data);


	i2c_begin();
	if (!i2c_write_byte(dadd<<1))
	{
		i2c_end();
        //TLG_PRINT_0("Tlg_i2c_read_data error");
		return IIC_ERR;
	}
	/* set MSB 1 */
	tmpData	= (((radd |	0x8000)	& 0xff00) >> 8);
	if (!i2c_write_byte(tmpData))
	{
		i2c_end();
        //TLG_PRINT_0("Tlg_i2c_read_data error");
		return IIC_ERR;
	}
	tmpData	= ((radd & 0x00ff));
	if (!i2c_write_byte(tmpData))
	{
		i2c_end();
        //TLG_PRINT_0("Tlg_i2c_read_data error");
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
        //TLG_PRINT_0("Tlg_i2c_read_data error");
		return IIC_ERR;
	}
	if (!i2c_write_byte(0xff))
	{
		i2c_end();
        //TLG_PRINT_0("Tlg_i2c_read_data error");
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
        //TLG_PRINT_0("Tlg_i2c_read_data error");
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


	EXIT(Tlg_i2c_read_data);


	return IIC_DONE;
}

#endif

/***************************************************************************
*
*
*	 IIC Burst Read data for TLG1120
*	 Parameter:
*			   dadd:device add (must be	0x58)
*			   radd:register add
*
****************************************************************************/

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



/**************************************************************************
*
*		   IIC START BIT
*
***************************************************************************/
static void	i2c_begin(void)
{

		ENTRY(i2c_begin);


	i2c_delay(I2C_DELAY_UNIT <<	0);
	set_i2c_pin(SDA);
	i2c_delay(I2C_DELAY_UNIT <<	0);
	set_i2c_pin(SCL);
	i2c_delay(I2C_DELAY_UNIT <<	0);
	clr_i2c_pin(SDA);
	i2c_delay(I2C_DELAY_UNIT <<	0);
	clr_i2c_pin(SCL);
	i2c_delay(I2C_DELAY_UNIT <<	0);


		EXIT(i2c_begin);
}

/**************************************************************************
*
*		   IIC STOP	BIT
*
***************************************************************************/
static void	i2c_end(void)
{

		ENTRY(i2c_end);

	i2c_delay(I2C_DELAY_UNIT <<	2);
	clr_i2c_pin(SDA);
	i2c_delay(I2C_DELAY_UNIT <<	2);
	set_i2c_pin(SCL);
	i2c_delay(I2C_DELAY_UNIT <<	3);
	set_i2c_pin(SDA);
	i2c_delay(I2C_DELAY_UNIT <<	4);


		EXIT(i2c_end);
}

/**************************************************************************
*
*		   IIC SEND	ACK	BIT
*
*					   flag	= 1: write NACK(SDA	HIGH)
*								0: write ACK(SDA LOW)
***************************************************************************/
static void	i2c_write_ack(unsigned char	flag)
{

	  ENTRY(i2c_write_ack);


	if(flag)
		set_i2c_pin(SDA);
	else
		clr_i2c_pin(SDA);
	i2c_delay(I2C_DELAY_UNIT <<	0);
	set_i2c_pin(SCL);
	i2c_delay(I2C_DELAY_UNIT <<	0);
	clr_i2c_pin(SCL);
	i2c_delay(I2C_DELAY_UNIT <<	0);
	//set_i2c_pin(SDA);				  // need to verify	here
	i2c_delay(I2C_DELAY_UNIT <<	0);


	  EXIT(i2c_write_ack);
}

/**************************************************************************
*
*		   IIC READ	ACK	BIT
*
***************************************************************************/
static unsigned	char i2c_read_ack(void)
{
	 unsigned	char ret;

	 ENTRY(i2c_read_ack);

	/* set SDA as input	status here	*/
//	GPIO_InitIO(0, SDA);				   /* only apply on	MTK	platform */
    //set_i2c_pin_dir(SDA,I2C_PIN_INPUT);
    #if 0   //modi by yangdecai  2010-10-15
		set_i2c_pin_dir(SDA,I2C_PIN_INPUT);
    #endif 

	i2c_delay(I2C_DELAY_UNIT <<	0);
	set_i2c_pin(SCL);
	i2c_delay(I2C_DELAY_UNIT <<	0);
	if (!get_i2c_pin(SDA))
	{
		ret	= IIC_TRUE;
	}
	else
	{
		ret	= IIC_FALSE;
	}

	i2c_delay(I2C_DELAY_UNIT <<	0);
	clr_i2c_pin(SCL);
	i2c_delay(I2C_DELAY_UNIT	<< 0);

	   /* set SDA as output	status here	*/
//	   GPIO_InitIO(1, SDA);					   /* only apply on	MTK	platform */
	 #if 0   //modi by yangdecai  2010-10-15
		 set_i2c_pin_dir(SDA,I2C_PIN_OUTPUT);
  	 #endif 


	i2c_delay(I2C_DELAY_UNIT <<	0);

	EXIT(i2c_read_ack);

	return ret;
}

/********************************************************************************
*
*			  INSERT DELAY
*
*********************************************************************************/
static void	i2c_delay(unsigned int time)
{
	while(time--)
	{
		;
	}
}

/* end of file */
