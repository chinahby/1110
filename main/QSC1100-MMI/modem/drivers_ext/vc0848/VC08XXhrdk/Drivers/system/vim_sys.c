#include "VIM_Common.h"
//#include "VIM_Drivers.h"

//static void pll_reconfig (unsigned int pll,unsigned int cpudiv);
//static void figure_pll_cpubus_div(VIM_CPU_FREQ cpu,VIM_BUS_FREQ bus,unsigned int*ppll_coeff,unsigned int*pcpubus_div);
//extern unsigned int do_clockswitch(unsigned int pll_coef,unsigned int cpubus_div);
//extern void lockICache(unsigned int addr,unsigned size);
//extern void unlockICache(void);
//#define SYS_MSG(str)	{VIM_UART_Printf str;}
/*************************************************
  Function:       
  	VIM_Sys
  	
  Description:    
  	Set system clock
  	
  Input:
	mode 
		0				216/72, full speed mode 
		1				TBD

  Output:        
	NULL
	
  Return:         
  	Error code 
*************************************************/
static const unsigned int  pll_tab_in_coef[] = {
#if(XCLKIN_6M)
		0x000A, //12M
#else
		0x0014, //12M
#endif
		0x000d, //13M
		0x000d, //26M
		0x000d, // 3.9M  
		0x0030, //19.2M
		0x0006, //6M
		0x000d, //6.5M
		0x0018, // 24M
		};
static const unsigned int   crytal_table[]=
		{
#if(XCLKIN_6M)
		60, //6M
#else
		120, //12M
#endif
		130, //13M
		260, //26M
		39, // 3.9M
		192, //19.2M
		60, //6M
		65, //6.5M
		240, //24M		
		};
int g_dummy; // to avoid warning
VIM_MODULE_FREQ_SET g_freq;

UINT32 VIM_Sys_Get_current_pll(void)
{
	UINT32 PLL_CLK ;//432M
	UINT32 M;
	UINT32 Addr,value,tmp;
	UINT32 crystal_index=0;
	crystal_index=VIM_HIF_GetReg32(V5_REG_STRAP_PIN);
	crystal_index = (crystal_index>>16)&0x07;
	
	Addr=V5_REG_SYS_PLL1CKD;// 0x6001011c;
	value=VIM_HIF_GetReg32(Addr);
	M=(value>>16)&0x3f;
	value=value&0x7ff;
	if(!M)
	 {
	 //MSG(("system clock setting error=========================\r\n"));
	 return 0;
		}	
	PLL_CLK=crytal_table[crystal_index]*2*value/(10*M);
	PLL_CLK/=2;
	Addr=V5_REG_SYS_CKDCBCON;// 0x6001011c;
	value=VIM_HIF_GetReg32(Addr);
       tmp=value&0x00100000;
       if(!tmp)
	   	PLL_CLK/=2;

    return PLL_CLK;
}

UINT32 timer_getvalue(UINT32 timer_id)//get current value of the counter
{
	UINT32 value;
	switch(timer_id)
	{
	case 1://TMR1
		value=VIM_HIF_GetReg32(V8_REG_TIMER_TMR1DOUT);
		break;
	case 2:	//TMR2
		value=VIM_HIF_GetReg32(V8_REG_TIMER_TMR2DOUT);
		break;
	default:
		break;
	}	
	return value;
}

UINT32 VIM_TIM2_GetValue(void)
{
	int val;
	val=timer_getvalue(2);
	return val;
}

void VIM_INT_Init( void)
{
	VIM_HIF_SetReg32(V5_REG_INT_INTCRTL, 0xf80f);	
}

void VIM_SYS_SetSensorClk(VIM_SYS_SENSORCLKTYPE Type)
{
	UINT32 sensor_target;
	UINT32 CDKLS;
	UINT8 CON_L,CON_H;
	UINT16 ML;//,MH;
	UINT32 PLL_CLK = 216;

	HAL_READ_UINT32(V5_REG_SYS_CKDLSCON,CDKLS);
	sensor_target=gVc0578b_Info.CaptureStatus.SensorClock/(Type+1);
	CON_L=PLL_CLK/sensor_target/2-1;
	CON_H=PLL_CLK/sensor_target-CON_L-2;
	if((CON_L+CON_H+2)*sensor_target<PLL_CLK)
		CON_H++;
	ML =(CON_H<<8)+CON_L;
	CDKLS&=0xffff0000;
	CDKLS|=ML;
	HAL_WRITE_UINT32(V5_REG_SYS_CKDLSCON,CDKLS);
}
/*************************************************
  Function:       
  	VIM_INT_Mask
  	
  Description:    
  	mask and lock interrupt
  	
  Input:
  	vector	
  		interrupt vector
           
  Output:        
	NULL
	
  Return:         
  	Error code 
  	
  History:        

      <author>  	<time>     	<desc>
      lizg    		07/05/04     	create this function  
*************************************************/
void VIM_INT_Mask(UINT32 vector)
{
	UINT32 lock;

	lock=VIM_HIF_GetReg32(V5_REG_INT_INTMASK);
	lock |= 1<<vector;
	VIM_HIF_SetReg32(V5_REG_INT_INTMASK, lock);
}
/*************************************************
  Function:       
  	VIM_INT_Unmask
  	
  Description:    
  	Un-mask and un-lock interrupt
  	
  Input:
  	vector	
  		interrupt vector
           
  Output:        
	NULL
	
  Return:         
  	Error code 
  	
  History:        

      <author>  	<time>     	<desc>
      lizg    		07/05/04     	create this function  
*************************************************/
void VIM_INT_Unmask(UINT32 vector)
{
	UINT32 unlock;

	unlock=VIM_HIF_GetReg32(V5_REG_INT_INTMASK);
	unlock &= ~(1<<vector);
	VIM_HIF_SetReg32(V5_REG_INT_INTMASK, unlock);
}
/*************************************************
  Function:       
  	VIM_INT_Acknowledge
  	
  Description:    
  	Clear 1st level interrupt status
  	
  Input:
  	vector	
  		interrupt vector
           
  Output:        
	NULL
	
  Return:         
  	Error code 
  	
  History:        

      <author>  	<time>     	<desc>
      lizg    		07/05/04     	create this function  
*************************************************/
void VIM_INT_Acknowledge(UINT32 vector)
{

	////////////////clear 2nd level status/////////////////////////	
	UINT32 Sts_2nd;
	UINT32 second_st = V5_REG_INT_MARB_ST+8*vector;

	Sts_2nd=VIM_HIF_GetReg32(second_st);
	VIM_HIF_SetReg32(second_st, Sts_2nd);
	////////////////clear 2nd level status end/////////////////////////	

	////////////////clear 1st level status/////////////////////////	
	VIM_HIF_SetReg32(V5_REG_INT_1LvINTClr, (1<<vector));
	////////////////clear 1st level status end/////////////////////////	
}

/*************************************************
  Function:       
  	VIM_INT_Disable
  	
  Description:    
  	mask but not lock interrupt
  	
  Input:
  	vector	
  		interrupt vector
           
  Output:        
	NULL
	
  Return:         
  	Error code 
  	
  History:        

      <author>  	<time>     	<desc>
      lizg    		07/05/04     	create this function  
*************************************************/
void VIM_INT_Disable(UINT32 vector)
{
    UINT32 mask;
    
    mask=VIM_HIF_GetReg32(V5_REG_INT_HINTEN);
    mask &= ~(1<<vector);
    VIM_HIF_SetReg32(V5_REG_INT_HINTEN, mask);
}

/*************************************************
  Function:       
  	VIM_INT_Enable
  	
  Description:    
  	Un-mask but without un-lock interrupt
  	
  Input:
  	vector	
  		interrupt vector
           
  Output:        
	NULL
	
  Return:         
  	Error code 
  	
  History:        

      <author>  	<time>     	<desc>
      lizg    		07/05/04     	create this function  
*************************************************/
void VIM_INT_Enable(UINT32 vector)
{
    UINT32 unmask;
    
    unmask=VIM_HIF_GetReg32(V5_REG_INT_HINTEN);
    unmask |= (1<<vector);
    VIM_HIF_SetReg32(V5_REG_INT_HINTEN, unmask);
}

/*************************************************
  Function:       
  	VIM_INT_Get2ndStatus
  	
  Description:    
  	get second level interrupt status
  	
  Input:
  	vector	
  		interrupt vector
           
  Output:        
	NULL
	
  Return:         
  	Error code 
  	
  History:        

      <author>  	<time>     	<desc>
      lizg    		07/05/04     	create this function  
*************************************************/
UINT32 VIM_INT_Get2ndStatus(UINT32 vector)
{
	UINT32 status;
	UINT32 second_st = V5_REG_INT_MARB_ST+8*vector;

	status=VIM_HIF_GetReg32(second_st);

	return status;
}

/*************************************************
  Function:       
  	VIM_INT_Get2ndEnable
  	
  Description:    
  	get second level interrupt enable register value
  	
  Input:
  	vector	
  		interrupt vector
           
  Output:        
	NULL
	
  Return:         
  	Error code 
  	
  History:        

      <author>  	<time>     	<desc>
      lizg    		07/05/04     	create this function  
*************************************************/
UINT32 VIM_INT_Get2ndEnable(UINT32 vector)
{
	UINT32 status;
	UINT32 second_en = V5_REG_INT_MARB_EN+8*vector;

	status=VIM_HIF_GetReg32(second_en);

	return status;
}


/*************************************************
  Function:       
  	VIM_INT_Set2ndEnable
  	
  Description:    
  	get second level interrupt enable register value
  	
  Input:
  	vector	
  		interrupt vector
           
  Output:        
	NULL
	
  Return:         
  	Error code 
  	
  History:        

      <author>  	<time>     	<desc>
      lizg    		07/05/04     	create this function  
*************************************************/
void VIM_INT_Set2ndEnable(UINT32 vector, UINT32 val)
{
	UINT32 second_en = V5_REG_INT_MARB_EN+8*vector;
	VIM_HIF_SetReg32(second_en, val);
}

/*************************************************
  Function:       
  	VIM_INT_Get1stStatus
  	
  Description:    
  	get 1st level interrupt status register value
  	
  Input:
	NULL
	
  Output:        
	NULL
	
  Return:         
  	Error code 
  	
  History:        

      <author>  	<time>     	<desc>
      lizg    		07/05/04     	create this function  
*************************************************/
UINT32 VIM_INT_Get1stStatus(void)
{
	UINT32 status;
	status=VIM_HIF_GetReg32(V5_REG_INT_HIRQST);
	return status;
}


/*************************************************
  Function:       
  	VIM_INT_Get1stEnable
  	
  Description:    
  	get 1st level interrupt enable register value
  	
  Input:
	NULL
	
  Output:        
	NULL
	
  Return:         
  	Error code 
  	
  History:        

      <author>  	<time>     	<desc>
      lizg    		07/05/04     	create this function  
*************************************************/
UINT32 VIM_INT_Get1stEnable(void)
{
	UINT32 status;
	status=VIM_HIF_GetReg32(V5_REG_INT_HINTEN);
	return status;
}

/*************************************************
  Function:       
  	VIM_INT_Set1stEnable
  	
  Description:    
  	set 1st level interrupt enable register value
  	
  Input:
	NULL
	
  Output:        
	NULL
	
  Return:         
  	Error code 
  	
  History:        

      <author>  	<time>     	<desc>
      lizg    		07/05/04     	create this function  
*************************************************/
void VIM_INT_Set1stEnable(UINT32 flg)
{
	VIM_HIF_SetReg32(V5_REG_INT_HINTEN, flg);
}



