/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED  2007                                                    
 * --------------------------------------------------------------------------------------------------------------
 * [File Name]            : MMD_I2C_Driver.c
 * [Description]          : I2C Driver  
 * [Author]                 : An Xinfang
 * [Date Of Creation]  : 2007-04-11 
 * [Platform]              : ARM
 * [Note]                   :  None   
 * Modification History : 
 * ------------------------------------------------------------------------------
 * Date                      	Author         		Modifications
 * ------------------------------------------------------------------------------
 * 2007-04-11        	An Xinfang      	Created.
 * 2007-10-10		Zhang Xuecheng	ported to Nu820 578b RDK.
 *******************************************************************************/
/*******************************************************************************
 * This source code has been made available to you by VIMICRO on an
 * AS-IS basis. Anyone receiving this source code is licensed under VIMICRO
 * copyrights to use it in any way he or she deems fit, including copying it,
 * modifying it, compiling it, and redistributing it either with or without
 * modifications. Any person who transfers this source code or any derivative
 * work must include the VIMICRO copyright notice and this paragraph in
 * the transferred software.
 *******************************************************************************/

/* INCLUDE FILE  */

//#include <stdio.h>
//#include <AASP_Shell.h>
//#include <aasp.h>

#include  "MMD_I2C_Define.h"
#include "MMP_rdk.h"

//#include  "hal_io.h"

//Module Name
#define MOD_NAME "I2CDriver"
//Module Version
#define MOD_VER "0.1"
//Module Description
#define MOD_DESC "I2CDriver"
//Module Owner
#define MOD_OWNER "Anxinfang"

/* MACRO DEFINITION */

/* I2C Read or Write Flag */
#define I2C_RWFLG_WRITE        0x00    /* Write Mode    */
#define I2C_RWFLG_READ          0x01    /* Read Mode    */

/* I2C Process Status */
#define I2C_ST_IDLE                  0        /* I2C Idle                               */
#define I2C_ST_SENDING           1        /* I2C Sending State                */
#define I2C_ST_AWAIT_ACK      2        /* I2C Awaiting ACK for Addr     */
#define I2C_ST_RECEIVING        3        /* I2C Receiving State              */
#define I2C_ST_RECV_LAST       4        /* I2C Receive Last                   */


#define STACON_OFF                 0         /* Start Condition None              */
#define STACON_ON                   1         /* Start Condition Occur             */
#define BYTE_CNT_DEFAULT     0         /* Start Condition Occur             */


/*I2C Max Time Count for Status Monitor */
#define I2C_MAX_TIME_COUNT 1000 /* Max Wait Time */
#define I2C_DELAY_TIME 1   /* Delay Time */

/*
  TYPE DEFINITION                                           
*/

/* I2C Transmission data management structure object */
typedef struct {
       MMD_U08 sladdr;      /* Device Slave Address*/
	MMD_U08 dummy0;  /* dummy0 */
       MMD_U16 dummy1;  /* dummy1 */
	MMD_U08 *buf;         /* Transmission Data */
	MMD_U32 bytes;       /* Transmission Data Size*/
}I2C_TransInfo;

/* I2C Processing management structure object */
static struct {
	MMD_U08        stacon_cnt;  /* Start condition occur*/
	MMD_U08        state;           /* I2C Processing Status*/
	MMD_U32        result;          /* I2C Processing Result*/
	MMD_U32        bytes_cnt;   /* */
	I2C_TransInfo *trans_info; /* */
} g_I2C_Status;

/*
  External Function declaration                             
*/
extern void delaycycle(MMD_U32);/*for delay */
extern MMD_U32 timer_getvalue(MMD_U32 timer_id);		//get current value of the counter


/*
  Internal Function declaration                             
*/
static void __I2C_StateManager(MMD_U08 sladdr, MMD_U08 rwflg, MMD_U08 *buf, MMD_U32 bytes);
static void __I2C_ByteTransfer(void);


/*
  FUNCTION (API)                                            
*/

/*
 * Initialization of I2C, 100kHz(default)                                                                             
 * Return:  
 *           None
 */
void MMD_I2C_Initialize(void)
{
      /**********************/
      MMD_U32 i2c_speed = 0x18; /*100kHz*/
      MMD_U32 i2c_register = 0 ;
	
      i2c_speed &= I2C_SCL5F;
      i2c_register |= i2c_speed;
      i2c_register |= I2C_EN;
	  
      Vm_SetReg( I2C_CTRL, i2c_register );
      Vm_SetReg( I2C_Command, 0x01 );
	
	/* State setting */
	g_I2C_Status.stacon_cnt = STACON_OFF;
	g_I2C_Status.state = I2C_ST_IDLE;
	g_I2C_Status.result = MMD_I2C_RET_NOERR;
	  
    
}

/*                                                                                                                                                                     
  * The I2C bus confirms the usage condition, and it is prepared to communicate I2C.                           
  * Return:                                                                                                                                               
  *           MMD_I2C_RET_NOERR             : OK                                                                                  
  *           MMD_I2C_RET_PROCESSING   : I2C is processing                                                                   
  */
MMD_U32 MMD_I2C_Start(void)
{
	MMD_U32 status;
	
	I2CPRINTF(("[I2C] I2C start! \n"));
	
    /* Usage condition confirmation of I2C bus */
        
      status = Vm_GetReg( I2C_Status );
                    	
         if((status & I2C_TIP) != 1)
	  {
		 	I2CPRINTF(("[I2C] I2C Bus is Idle \n"));
                      g_I2C_Status.stacon_cnt = STACON_ON;
	   }
	   else
          {
                      I2CPRINTF(("[I2C] I2C Bus busy Error!!\n"));
		        return MMD_I2C_RET_PROCESSING;
	    }
          
          return MMD_I2C_RET_NOERR;

}



/*                                                                                              
  * I2C bus status is returned to IDLE.                                                                                                                                       
  * Return:                                                                                                                                               
  *           MMD_I2C_RET_NOERR        : OK                                                                                                                                               
  */

MMD_U32 MMD_I2C_Stop(void)
{
	MMD_U32 ret;
  
	I2CPRINTF(("[I2C] I2C Stop!\n"));
	/* Stop condition generation */
	Vm_SetReg( I2C_Command, 0x10 );
	
	/* delay  to idle */
	MMD_USER_WaitMICRS(I2C_DELAY_TIME*10);
	
	/* I2C Bus Idle */
	I2CPRINTF(("[I2C] I2C Bus Idle \n"));
	ret = MMD_I2C_RET_NOERR;
       
	g_I2C_Status.stacon_cnt = STACON_OFF;
	g_I2C_Status.state = I2C_ST_IDLE;
	
	return(ret);

}


/*                                                                                              
  * From a slave address, it reads by bytes and it is stored in buf.                                                                                             
  * Return:                                                                                                                                               
  *           MMD_I2C_RET_NOERR                        : OK                                                                                                                                      
  *           MMD_I2C_RET_TIMEOUT                     : Communication time-out                                                          
  *           MMD_I2C_RET_NACK_ON_ADDR   : I2C No ACK for Addr                                                             
  *           MMD_I2C_RET_ARBIT_LOST         : Arbitration occurs
  *           MMD_I2C_RET_INVALIDPARAM            : I2C Invalid Parameter set
  */

MMD_U32 MMD_I2C_Read(
             MMD_U08 sladdr,      /* (IN)     Information in Slave Address */
             MMD_U08 *buf,         /* (OUT)  Information in Receive data storage buffer */
             MMD_U32 bytes        /* (IN)     Information in Number of bytes for receive data */
)

{
       MMD_U08 flag = I2C_RWFLG_READ;
	I2CPRINTF(("[I2C] I2C Read Start...\n"));

	/* NULL Pointer */
	if(buf == NULL){
		I2CPRINTF(("[I2C] NULL Pointer Error!!\n"));
		return (MMD_I2C_RET_INVALIDPARAM);
	}

	
	/* Receive state set */
	g_I2C_Status.state = I2C_ST_AWAIT_ACK;
	
	/* Internal function start */
	__I2C_StateManager(sladdr, flag, buf, bytes);
	
	return g_I2C_Status.result;	

}



/*                                                                                              
  * The data stored in buf is transmitted to bytes part I2C bus.                                                                                            
  * Return:                                                                                                                                               
  *           MMD_I2C_RET_NOERR                       : OK                                                                                                                                          
  *           MMD_I2C_RET_TIMEOUT                    : Communication time-out                                                          
  *           MMD_I2C_RET_NACK_ON_DATA  : I2C No ACK for Data                                                             
  *           MMD_I2C_RET_ARBIT_LOST        : Arbitration occurs 
  *           MMD_I2C_RET_INVALIDPARAM           : I2C Invalid Parameter set  
  */

MMD_U32 MMD_I2C_Write(
             MMD_U08 sladdr,      /*(IN)     Information in Slave Address */
             MMD_U08 *buf,         /*(IN)     Information in Transmission data storage buffer*/
             MMD_U32 bytes        /*(IN)     Information in Number of bytes for transmission data*/
)

{
	MMD_U08 flag = I2C_RWFLG_WRITE;
	I2CPRINTF(("[I2C] I2C Write Start...\n"));

	/* NULL Pointer */
	if(buf == NULL){
		I2CPRINTF(("[I2C] NULL Pointer Error!!\n"));
		return (MMD_I2C_RET_INVALIDPARAM);
	}

	/* Transmit state set */
	g_I2C_Status.state = I2C_ST_SENDING;
	
	/* Internal function start */
	__I2C_StateManager(sladdr, flag, buf, bytes);
	
	return g_I2C_Status.result;
}



/*                                                                                              
  * I2C Transfer mode set.                                                                                          
  * Return:                                                                                                                                               
  *           MMD_I2C_RET_NOERR              : OK                                                                                  
  *           MMD_I2C_RET_INVALIDPARAM  : Parameter Error of Argument                                                              
  */
MMD_U32 MMD_I2C_ModeSet(
             MMD_U08 mode   /* (IN)  transmission Mode  */
)

{

	
      MMD_U32 i2c_speed = 0;/*i2c_divider = mclk /(8*i2c_clk)-1, mclk=24MHz here*/
      MMD_U32 i2c_register = 0 ;
	
     I2CPRINTF(("[I2C] I2C ModeSet Start\n"));

	

	switch(mode)
	{
		case MMD_I2C_MODE_STANDARD:
			/* transfer mode : standard(100kbps) */
			I2CPRINTF(("[I2C] transfer mode : standard(100kbps)\n"));
			i2c_speed = 0x1d; /*100kHz*/
			i2c_speed &= I2C_SCL5F;
                     i2c_register |= i2c_speed;
                     i2c_register |= I2C_EN;
               	  
                     Vm_SetReg( I2C_CTRL, i2c_register );
            
					 
			return (MMD_I2C_RET_NOERR);
			
		case MMD_I2C_MODE_FAST:	
			/* transfer mode : fast(400kbps) */
			I2CPRINTF(("[I2C] transfer mode : fast(400kbps)\n"));
			
			i2c_speed = 0x06; /*400kHz*/
			//i2c_speed = 0x07; /*400kHz*/
			i2c_speed &= I2C_SCL5F;
                     i2c_register |= i2c_speed;
                     i2c_register |= I2C_EN;
               	  
                     Vm_SetReg( I2C_CTRL, i2c_register );
             
					 
			return (MMD_I2C_RET_NOERR);
		
		default:
			/* transfer mode : invalid */
			I2CPRINTF(("[I2C] transfer mode  invalid Error!!\n"));
			return( MMD_I2C_RET_INVALIDPARAM );
	}	
	
}


/*
  FUNCTION(INTERNAL)                                        
*/

/*                                                                                              
  * After the start condition is generated, the communication of every one byte is observed.                                                                                     
  * Return:                                                                                                                                               
  *          None                                             
  */
static void __I2C_StateManager(
                 MMD_U08 sladdr, /*(IN)     Information in Slave Address*/
                 MMD_U08 rwflg,   /*(IN)     Information in Read or Write Flag  */
                 MMD_U08 *buf,    /*(IN)     Information in Transmission(Receive) data storage buffer*/
                 MMD_U32 bytes   /* (IN)     Information in Number of bytes for transmission(receive) data */
)

{
	static I2C_TransInfo tmpTraInfo;
	MMD_U32  status;
       MMD_U32  current,end;
	
	I2CPRINTF(("[I2C] __I2C_StateManager Start...\n"));

	g_I2C_Status.trans_info = &tmpTraInfo;
	g_I2C_Status.trans_info->sladdr = (MMD_U08)( (sladdr & 0xFE) | rwflg );
	g_I2C_Status.trans_info->buf = buf;
	g_I2C_Status.trans_info->bytes = bytes;

	g_I2C_Status.bytes_cnt = BYTE_CNT_DEFAULT;
	g_I2C_Status.result = MMD_I2C_RET_ERR;

	Vm_SetReg( I2C_TXR, g_I2C_Status.trans_info->sladdr ); /*prepare the slave address*/

       Vm_SetReg( I2C_Command, 0x28 ); /*start i2c write,transfer the slave address first*/


	  /* Absolute acquisition of time */

	current = timer_getvalue(2);

	/* Absolute acquisition of time( after I2C_MAX_TIME_COUNT ) */

	end = current + I2C_MAX_TIME_COUNT;
 

	while(1)
	{
		if(g_I2C_Status.result != MMD_I2C_RET_ERR)
		{
			I2CPRINTF(("[I2C] I2C communication end \n"));
			break;
		}

		status= Vm_GetReg( I2C_Status); /*read i2c status*/
	  
		if( (status&I2C_TIP)!=1 ) /*check if in processing*/
		{
			__I2C_ByteTransfer();
		}
		else if(current > end)
		{
#ifndef I2C_DBG
			I2CPRINTF(("[I2C] Pollng TimeOut Error!!\n"));
			g_I2C_Status.result = MMD_I2C_RET_TIMEOUT;
			break;
#endif
			
		}
		else
		{
		  current =timer_getvalue(2);
		}

	}



	

}


/*                                                                                              
  * The data of one byte is transmitted, and the state changes.                                                                                     
  * Return:                                                                                                                                               
  *          None                                             
  */
static void __I2C_ByteTransfer(void)
{
	MMD_U32 status;

	
	I2CPRINTF(("[I2C] __I2C_ByteTransfer...\n"));

	status= Vm_GetReg( I2C_Status ); /*read i2c status*/
	if( (status&I2C_AL) !=0)
	{
		I2CPRINTF(("[I2C] Arbitration lost generation!!\n"));
		g_I2C_Status.result = MMD_I2C_RET_ARBIT_LOST;
		return;
	}

	switch (g_I2C_Status.state) 
	{
		/* State of transmission */
		case I2C_ST_SENDING:
			I2CPRINTF(("[I2C] State of transmission\n"));
			status= Vm_GetReg( I2C_Status); /*read i2c status*/
			 
                    	if(  (status&I2C_RxACK) !=0 ){
                                I2CPRINTF(("didn't receive slave ack!\n"));
                                g_I2C_Status.result = MMD_I2C_RET_NACK_DATA;
             		}
		
			else
			{
				I2CPRINTF(("[I2C] bytes_cnt = %X\n",g_I2C_Status.bytes_cnt));
				I2CPRINTF(("[I2C] bytes = %X\n",g_I2C_Status.trans_info->bytes));
				if (g_I2C_Status.bytes_cnt < g_I2C_Status.trans_info->bytes) 
				{
					/* The next byte forwarding */
					I2CPRINTF(("[I2C] The next byte forwarding \n"));
					I2CPRINTF(("[I2C] data = %X\n",g_I2C_Status.trans_info->buf[g_I2C_Status.bytes_cnt]));
                                    Vm_SetReg( I2C_TXR, g_I2C_Status.trans_info->buf[g_I2C_Status.bytes_cnt]);
					g_I2C_Status.bytes_cnt++;					
				       Vm_SetReg( I2C_Command, 0x08 ); /*start i2c write,transfer the data*/
       
				}
					
				else
				{
					I2CPRINTF(("[I2C] transmission end \n"));
					g_I2C_Status.result = MMD_I2C_RET_NOERR;
				}
			}
			break;

              /* State of reception (receive an address) */
		case I2C_ST_AWAIT_ACK:
			I2CPRINTF(("[I2C] State of reception (receive an address)\n"));
			status=Vm_GetReg( I2C_Status); /*read i2c status*/
			if((status&I2C_RxACK) !=0)
			{
				I2CPRINTF(("[I2C] Receive NACK of address!!\n"));
				g_I2C_Status.result = MMD_I2C_RET_NACK_ADDR;
			}
			else
			{
				I2CPRINTF(("[I2C] bytes_cnt = %X\n",g_I2C_Status.bytes_cnt));
				I2CPRINTF(("[I2C] bytes = %X\n",g_I2C_Status.trans_info->bytes));
				if (g_I2C_Status.trans_info->bytes == 1)
				{
					/* ACK no reply */
					I2CPRINTF(("[I2C] ACK no reply\n"));
					Vm_SetReg( I2C_Command, 0x06 ); /*start i2c read,no ack*/
					g_I2C_Status.state = I2C_ST_RECV_LAST;
				}
				else
				{
					/* ACK reply */
					I2CPRINTF(("[I2C] ACK reply\n"));
					Vm_SetReg( I2C_Command, 0x04 ); /*start i2c read,return ack */
					g_I2C_Status.state = I2C_ST_RECEIVING;
				}
			}
			break;
		
		/* State of reception (receive a data) */
		case I2C_ST_RECEIVING:
			I2CPRINTF(("[I2C] State of reception (receive a data)\n"));
			g_I2C_Status.trans_info->buf[g_I2C_Status.bytes_cnt]= (MMD_U08)Vm_GetReg( I2C_RXR); 
                   	I2CPRINTF(("[I2C] data =%X\n",g_I2C_Status.trans_info->buf[g_I2C_Status.bytes_cnt] ));
                     g_I2C_Status.bytes_cnt++;
			I2CPRINTF(("[I2C] bytes_cnt =%X\n",g_I2C_Status.bytes_cnt));
			I2CPRINTF(("[I2C] bytes = %X\n",g_I2C_Status.trans_info->bytes));			
			
			if ((g_I2C_Status.bytes_cnt + 1 ) == g_I2C_Status.trans_info->bytes) 
			{
				/* ACK no reply */
				I2CPRINTF(("[I2C] ACK no reply\n"));
				Vm_SetReg( I2C_Command, 0x06 );/*start i2c read,no ack*/
				g_I2C_Status.state = I2C_ST_RECV_LAST;
			}
			else
			{
				/* ACK reply */
				I2CPRINTF(("[I2C] ACK reply\n"));
				Vm_SetReg( I2C_Command, 0x04 ); /*start i2c read,return ack */
				g_I2C_Status.state = I2C_ST_RECEIVING;
			}
			break;
		
		/* State of reception (receive a last data) */
		case I2C_ST_RECV_LAST:
			I2CPRINTF(("[I2C] State of reception (receive a last data)\n"));
			g_I2C_Status.trans_info->buf[g_I2C_Status.bytes_cnt] =(MMD_U32)Vm_GetReg( I2C_RXR);
			I2CPRINTF(("[I2C] data =%X\n",g_I2C_Status.trans_info->buf[g_I2C_Status.bytes_cnt]));
                     g_I2C_Status.bytes_cnt++;
			I2CPRINTF(("[I2C] bytes_cnt =%X\n",g_I2C_Status.bytes_cnt));
			I2CPRINTF(("[I2C] bytes = %X\n",g_I2C_Status.trans_info->bytes));
			g_I2C_Status.result =MMD_I2C_RET_NOERR;
			break;
			
		default:
		
			break;
	}

	
}



