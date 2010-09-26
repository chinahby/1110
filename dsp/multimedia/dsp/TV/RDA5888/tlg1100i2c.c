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

#include "tlg1100i2c.h"
#include "analog_tv.h"

extern void GPIO_WriteIO(char data, char port);
extern void GPIO_InitIO(char direction, char port);
extern char GPIO_ReadIO(char port);
/******************************************************************************************
*              Macros
*******************************************************************************************/
#define I2C_DELAY_UNIT         80 // 25
//#define ATV_I2C_SDA_PIN    	42
//#define ATV_I2C_SCL_PIN		41

#define		TRUE		1
#define		FALSE		0

#define set_sda_output()  	GPIO_InitIO(1,ATV_I2C_SDA_PIN)
#define set_sda_input()  	GPIO_InitIO(0,ATV_I2C_SDA_PIN)
#define set_scl_output()	GPIO_InitIO(1,ATV_I2C_SCL_PIN)

#define set_i2c_scl_PIN     GPIO_WriteIO(1,ATV_I2C_SCL_PIN)	
#define	clr_i2c_scl_PIN     GPIO_WriteIO(0,ATV_I2C_SCL_PIN)
#define set_i2c_sda_PIN     GPIO_WriteIO(1,ATV_I2C_SDA_PIN)
#define clr_i2c_sda_PIN     GPIO_WriteIO(0,ATV_I2C_SDA_PIN)
#define get_i2c_sda_PIN     GPIO_ReadIO(ATV_I2C_SDA_PIN)

static void i2c_delay(unsigned int time);
static void i2c_begin(void);
static void i2c_end(void);
static void i2c_write_ask(unsigned char flag);
static unsigned char i2c_read_ack(void);
static unsigned char  i2c_read_byte(void);
static unsigned char i2c_write_byte(unsigned char data);

/******************************************************************************************
*              Functions
*******************************************************************************************/
static unsigned char i2c_read_byte(void)
{
	unsigned char i;
	unsigned char ret;

	ret	= 0;
	set_sda_input();
	for	(i=0; i<8; i++)
	{			
		i2c_delay(I2C_DELAY_UNIT << 0);
		set_i2c_scl_PIN;	
		i2c_delay(I2C_DELAY_UNIT << 0);
		ret	= ret<<1;
		if (get_i2c_sda_PIN)
			ret |= 1;
		i2c_delay(I2C_DELAY_UNIT << 0);
		clr_i2c_scl_PIN;
	        i2c_delay(I2C_DELAY_UNIT << 0);
	        if (i==7){
					set_sda_output();
	        	        }
	        i2c_delay(I2C_DELAY_UNIT << 0);
	}		

	return ret;
}

static unsigned char i2c_write_byte(unsigned char data)
{
	unsigned char i;

	 set_sda_output();


	for	(i=0; i<8; i++)
	{
		i2c_delay(I2C_DELAY_UNIT << 0);
		if (data & 0x80)
			set_i2c_sda_PIN;
		else
			clr_i2c_sda_PIN;
		data <<= 1;

		i2c_delay(I2C_DELAY_UNIT << 0);
		set_i2c_scl_PIN;
		i2c_delay(I2C_DELAY_UNIT << 0);
		clr_i2c_scl_PIN;		
	}	
	return i2c_read_ack();
}

/***************************************************************************
*
*
*    IIC Write data for Tlg1100
*
*
****************************************************************************/
int TLGI2C_WriteReg(unsigned char dab, unsigned short rab, unsigned short data)
{
	//unsigned char i;
	unsigned char tmpData = 0;

	i2c_begin();
	if (!i2c_write_byte(dab<<1))
	{
		i2c_end();
		kal_prompt_trace(0,"IIC_Addr_ERR!!!!!!!!!!!!");	
		return 0;
	}
	
	tmpData = (rab & 0x00ff);
	if (!i2c_write_byte(tmpData))
	{
		i2c_end();
		kal_prompt_trace(0,"IIC_RegLERR!!!!!!!!!!!!!!!!!");	
		return 0;
	}
    /* write 16bits data */
	tmpData = (((data) & 0xff00) >> 8);
	if (!i2c_write_byte(tmpData))
	{
		i2c_end();
		kal_prompt_trace(0,"IIC_DATAH_ERR!!!!!!!!!!!!!!!!");	
		return 0;
	}
	//data++;
	
	tmpData = ((data) & 0x00ff);
	if (!i2c_write_byte(tmpData))
	{
		i2c_end();
		kal_prompt_trace(0,"IIC_DATAL_ERR!!!!!!!!!!!!!!!!!!");	
		return 0;
	}


	i2c_end();
	return 1;
}

/***************************************************************************
*
*
*    IIC Read data for Tlg1100
*
*
****************************************************************************/
int TLGI2C_ReadReg(unsigned char dab, unsigned short rab, unsigned short *data)
{
	unsigned char i;
	unsigned char tmpData = 0;

	i2c_begin();
	if (!i2c_write_byte(dab<<1))
	{
		i2c_end();
		kal_prompt_trace(0,"IIC_dadd_ERR!!!!!!!!!!!!");
		return 0;
	}
#if 0	
	/* set MSB 1 */
	tmpData = (((rab | 0x8000) & 0xff00) >> 8); 
	if (!i2c_write_byte(tmpData))
	{
		i2c_end();
		return 0;
	}
#endif	
	tmpData = ((rab & 0x00ff)); 
	if (!i2c_write_byte(tmpData))
	{
		i2c_end();
		kal_prompt_trace(0,"IIC_radd_ERR!!!!!!!!!!!!");
		return 0;
	}
#if 0
       /* write any data into this register */
	if (!i2c_write_byte(0xff))
	{
		i2c_end();
		return 0;
	}
	if (!i2c_write_byte(0xff))
	{
		i2c_end();
		return 0;
	}
	i2c_end();  /* stop bit */
#endif
	/* start again */
	i2c_begin();

	if (!i2c_write_byte((unsigned char)((dab<<1) | 1)))
	{
		i2c_end();
		kal_prompt_trace(0,"IIC_dadd_ERR!!!!!!!!!!!!");
		return 0;
	}

#if 0	
	*data = i2c_read_byte();
	i2c_write_ask(0);
	
	*data = i2c_read_byte();
	i2c_write_ask(0);
#endif

      tmpData = i2c_read_byte();
      i2c_write_ask(0);
    
      *data = (tmpData << 8) & 0xff00;
    	
      tmpData = i2c_read_byte();
      i2c_write_ask(1);	                 /* can not send ACK,must send NAck */
     
      *data |= tmpData;

	i2c_end();
	return 1;
}

static void i2c_delay(unsigned int time)
{
	while(time--) 
	{	
        ;
	}
}

static void i2c_begin(void)
{
      set_sda_output();

	set_i2c_sda_PIN;
	i2c_delay(I2C_DELAY_UNIT << 0);
	set_i2c_scl_PIN;		//yah
	i2c_delay(I2C_DELAY_UNIT << 0);
	set_i2c_sda_PIN;		//yah
	i2c_delay(I2C_DELAY_UNIT << 0);
	clr_i2c_sda_PIN;	
	i2c_delay(I2C_DELAY_UNIT << 1);
	clr_i2c_scl_PIN;
	i2c_delay(I2C_DELAY_UNIT << 0);
}

static void i2c_end(void)
{

	set_sda_output();
	i2c_delay(I2C_DELAY_UNIT << 2);
	clr_i2c_sda_PIN;
	i2c_delay(I2C_DELAY_UNIT << 2);
	set_i2c_scl_PIN;
	i2c_delay(I2C_DELAY_UNIT << 3);
	set_i2c_sda_PIN;
	i2c_delay(I2C_DELAY_UNIT << 4);
}

static void i2c_write_ask(unsigned char flag)
{

   set_sda_output();


	if(flag)
		set_i2c_sda_PIN;
	else
		clr_i2c_sda_PIN;
	i2c_delay(I2C_DELAY_UNIT << 0);
	set_i2c_scl_PIN;
	i2c_delay(I2C_DELAY_UNIT << 0);
	clr_i2c_scl_PIN;
	i2c_delay(I2C_DELAY_UNIT << 0);
	set_i2c_sda_PIN;
	i2c_delay(I2C_DELAY_UNIT << 0);
}


static unsigned char i2c_read_ack(void)
{	
	unsigned char ret;
	set_sda_input();
	
	i2c_delay(I2C_DELAY_UNIT << 0);
	set_i2c_scl_PIN;
	i2c_delay(I2C_DELAY_UNIT << 0);
	if (!get_i2c_sda_PIN)
	{
		ret = 1;
	}
	else
	{
		ret = 0;
	}

	i2c_delay(I2C_DELAY_UNIT << 0);
	clr_i2c_scl_PIN;
       i2c_delay(I2C_DELAY_UNIT << 0);
	   
       set_sda_output();
	   
	i2c_delay(I2C_DELAY_UNIT << 0);	

	return ret;
}


/* end of file */
