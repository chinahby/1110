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
#if 0  //add by yangdecai 09-24
#include "kal_release.h"
#include "stack_common.h"
#include "stack_msgs.h"
#include "app_ltlcom.h"       /* Task message communiction */
#include "syscomp_config.h"
#include "task_config.h"
#include "stacklib.h"
#include "stack_timer.h"      /*stack_timer_struct....definitions*/
#include "drv_comm.h"
#include "IntrCtrl.h"
#include "reg_base.h"
#include "gpio_sw.h"
#include "isp_if.h"
#include "sccb.h"
#else
//add by yangdecai 09-25
#define	GPIO_ModeSetup(a, b)   
#define	GPIO_InitIO(a, b)      
#define	GPIO_WriteIO(a, b)

#endif
/******************************************************************************************
*              Macros
*******************************************************************************************/
#include "tlgInclude.h"
#ifndef gpio_sccb_serial_data_pin
#define gpio_sccb_serial_data_pin 46
#endif
#ifndef gpio_sccb_serial_clk_pin
#define gpio_sccb_serial_clk_pin  45
#endif

#define	 I2C_DELAY_UNIT		50//100


/* define I2C interface	*/
#define	SDA					46			/* I2C serial interface	data I/O */
#define	SCL					45			/* I2C serial interface	clock input	*/
                    		
                    		
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
static void set_i2c_pin_dir(unsigned char Dir);
static void i2c_delay(unsigned int time);
static void set_i2c_pin(unsigned int pin);
static void clr_i2c_pin(unsigned int pin);
static unsigned char get_i2c_pin(unsigned int pin);

static void i2c_begin(void);
static void i2c_end(void);
static void	i2c_write_ack(unsigned char	flag);
static unsigned char i2c_read_ack(void);
static unsigned char  i2c_read_byte(void);
static unsigned char i2c_write_byte(unsigned char data);

#ifndef WIN32

/******************************************************************************************
*              Functions
*******************************************************************************************/
#if 0 //delete by macro lei 2008/08/14
void i2c_init(BOOL on)
{
	if(on)
	{
		GPIO_ModeSetup(SDA, 0);
		GPIO_InitIO(1, SDA);
		GPIO_ModeSetup(SCL, 0);
		GPIO_InitIO(1, SCL);
	
		GPIO_WriteIO(1, SDA);
		GPIO_WriteIO(1, SCL);
	}
	else
	{		
		GPIO_ModeSetup(SDA, 0);
		GPIO_InitIO(1, SDA);
		GPIO_ModeSetup(SCL, 0);
		GPIO_InitIO(1, SCL);
	
		GPIO_WriteIO(0, SDA);
		GPIO_WriteIO(0, SCL);
	}
}
#endif

static void set_i2c_pin_dir(unsigned char Dir)
{
	//*(volatile unsigned int *)(BASE_GPIO_TIMER+GPIO_DIR) = Dir;

	
	if(Dir == 0)//INPUT
	{
	#if 0   //modi by yangdecai  09-24
		GPIO_InitIO(INPUT,SDA);//SET_SCCB_DATA_INPUT;//SET_SCCB_DATA_INPUT_EX;
	#else
		GPIO_InitIO(1,SDA);//SET_SCCB_DATA_INPUT;//SET_SCCB_DATA_INPUT_EX;
	#endif
	}
	else if (Dir == 1)//OUTPUT 
	{
		#if 0 //modi by yangdecai  09-24
		GPIO_InitIO_FAST(OUTPUT,SCL);//SET_SCCB_DATA_OUTPUT;//SET_SCCB_DATA_OUTPUT_EX;
		#else
		GPIO_InitIO(1,SCL);//SET_SCCB_DATA_INPUT;//SET_SCCB_DATA_INPUT_EX;
		#endif
	}
}




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

			if (i==7){
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
DECLSPEC TLGI2C_WriteReg(unsigned char dadd, unsigned short radd, unsigned short data)
{
	unsigned char tmpData = 0;

	ENTRY(TLGI2C_WriteReg);
	i2c_begin();
	if (!i2c_write_byte(dadd<<1))
	{
		i2c_end();
		return IIC_ERR;
	}
	/* write 16bits register */
      tmpData = ((radd & 0x7f00) >> 8);

	if (!i2c_write_byte(tmpData))
	{
		i2c_end();
		return IIC_ERR;
	}

	tmpData = (radd & 0x00ff);
	if (!i2c_write_byte(tmpData))
	{
		i2c_end();
		return IIC_ERR;
	}
    /* write 16bits data */
	tmpData	= (((data)	& 0xff00) >> 8);
	if (!i2c_write_byte(tmpData))
	{
		i2c_end();
		return IIC_ERR;
	}
	
	tmpData = ((data) & 0x00ff);
	if (!i2c_write_byte(tmpData))
	{
		i2c_end();
		return IIC_ERR;
	}


	i2c_end();


	EXIT(TLGI2C_WriteReg);

	return IIC_DONE;
}

/***************************************************************************
*
*
*    IIC Read data for Tlg1100
*                                    dab:device Id =0x2c
*                                    rab: register address
*                                    data: dump into register
*
****************************************************************************/
DECLSPEC TLGI2C_ReadReg(unsigned char dadd, unsigned short radd, unsigned short *data)
{
	unsigned short tmpData = 0;


	ENTRY(TLGI2C_ReadReg);


	i2c_begin();
	if (!i2c_write_byte(dadd<<1))
	{
		i2c_end();
		return IIC_ERR;
	}
	/* set MSB 1 */
	tmpData = (((radd | 0x8000) & 0xff00) >> 8); 
	if (!i2c_write_byte(tmpData))
	{
		i2c_end();
		return IIC_ERR;
	}
	tmpData = ((radd & 0x00ff)); 
	if (!i2c_write_byte(tmpData))
	{
		i2c_end();
		return IIC_ERR;
	}
///////////////////////////////////////////////////////////////////////////////////////////
//  ???????????????????????
//////////////////////////////////////////////////////////////////////////////////////////
       /* write any data into this register */
	if (!i2c_write_byte(0xff))
	{
		i2c_end();
		return IIC_ERR;
	}
	if (!i2c_write_byte(0xff))
	{
		i2c_end();
		return IIC_ERR;
	}
	i2c_end();  /* stop bit */
/////////////////////////////////////////////////////////////////////////////////////////
    
	/* start again */
	i2c_begin();

	if (!i2c_write_byte((unsigned char)((dadd<<1) |	1)))
	{
		i2c_end();
		return IIC_ERR;
	}

//////////////////////////////////////////////////////////
//			???????????????????
/////////////////////////////////////////////////////////
	*data = i2c_read_byte();
	i2c_write_ack(0);
	
	*data = i2c_read_byte();
	i2c_write_ack(0);
//////////////////////////////////////////////////////////
    
      tmpData = i2c_read_byte();
	  i2c_write_ack(0);
    
      *data = (tmpData << 8) & 0xff00;
    	
      tmpData = i2c_read_byte();
	  i2c_write_ack(1);					 /*	can	not	send ACK,must send NAck	*/
     
      *data |= tmpData;

	i2c_end();


	EXIT(TLGI2C_ReadReg);


	return IIC_DONE;
}

static void set_i2c_pin(unsigned int pin)
{
	if(pin == SDA)
	{
		#if 0   //modi by yangdecai 09-25
		GPIO_WriteIO_FAST(1,SDA);//SET_SCCB_DATA_HIGH// SET_SCCB_DATA_HIGH_EX;
		#endif
	}
	else if (pin == SCL)
	{
		#if 0   //modi by yangdecai 09-25
		GPIO_WriteIO_FAST(1,SCL);//SET_SCCB_DATA_HIGH// SET_SCCB_DATA_HIGH_EX;      SET_SCCB_CLK_HIGH //SET_SCCB_CLK_HIGH_EX;
		#endif
	}
	//*(volatile unsigned int *)(BASE_GPIO_TIMER+GPIO_OUT) |= pin;
}
/************************************************************************
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
static void clr_i2c_pin(unsigned int pin)
{
	//*(volatile unsigned int  *)(BASE_GPIO_TIMER+GPIO_OUT) &= (~pin);
	 ENTRY(clr_i2c_pin);
	#if 0 //dele by yangdecai  09-24
	    if(pin == SDA)
        {
   SET_SCCB_DATA_LOW//     SET_SCCB_DATA_LOW_EX;
        }
    else if (pin == SCL)
        {
     SET_SCCB_CLK_LOW//   SET_SCCB_CLK_LOW_EX;
        }
	#endif
	 EXIT(clr_i2c_pin);

} 

/************************************************************************
*
*			   READ	DATA FROM SDA PIN
*
*************************************************************************/
static unsigned char get_i2c_pin(unsigned int pin)
{
	unsigned char ret;
	unsigned int value;
	ENTRY(get_i2c_pin);
#if 0 //dele by yangdecai  09-24

	value = GET_SCCB_DATA_BIT;//	value =	GET_SCCB_DATA_BIT_EX;
#else
	value = 1;//	value =	GET_SCCB_DATA_BIT_EX;
#endif
	if (value  == 0)
		ret	= 0;
	else
		ret	= 1;
	EXIT(get_i2c_pin);

	return ret;
}

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

