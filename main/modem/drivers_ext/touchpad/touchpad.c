/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 Touchpad.c
 Copyright(c) Anylook
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                       INCLUDE FILES FOR MODULE

===========================================================================*/

#include "touchpad.h"
#include "charger.h"
#include "clk.h"
#include "err.h"
#include "msg.h"
#include "disp_options.h" 
#include "I2c.h" 
#include "rex.h" 
#include "hs_mb6550.h" 
#include "adc.h"
#include "hw.h"

#define TOUCHPAD_DEBUG
#ifdef TOUCHPAD_DEBUG
#define TOUCHPAD_LOG ERR 
#else
#define TOUCHPAD_LOG
#endif

//x,y值对调等值
#define TOUCHPAD_XCHANGE    1280
#define TOUCHPAD_YCHANGE    1140


#define TOUCHPAD_MIN_INTERVAL 		5		/* min  5 ms */
#define TOUCHPAD_DEFAULT_INTERVAL 	50		/* default to 50 ms */
#define TOUCHPAD_MAX_INTERVAL 		500		/* max 500 ms */

#define TOUCHPAD_PARAM_LIMITED	0
#define PEN_SAMPLE_NUM		3
#if 1
#define TOUCHPAD_GPIO_INT    	 	    GPIO_INT_30//GPIO_INT_32  //X+

#define TOUCHPAD_XL_IN                  GPIO_INPUT_31
#define TOUCHPAD_XL_OUT                 GPIO_OUTPUT_31

#define TOUCHPAD_XR_IN                  GPIO_INPUT_32
#define TOUCHPAD_XR_OUT                 GPIO_OUTPUT_32

#define TOUCHPAD_YU_IN                  GPIO_INPUT_30 
#define TOUCHPAD_YU_OUT                 GPIO_OUTPUT_30

#define TOUCHPAD_YD_IN                  GPIO_INPUT_36
#define TOUCHPAD_YD_OUT                 GPIO_OUTPUT_36

#else
#define TOUCHPAD_GPIO_INT    	 	    GPIO_INT_36  //X+

#define TOUCHPAD_XL_IN                  GPIO_INPUT_30 //GPIO_INPUT_31
#define TOUCHPAD_XL_OUT                 GPIO_OUTPUT_30//GPIO_OUTPUT_31

#define TOUCHPAD_XR_IN                  GPIO_INPUT_36//GPIO_INPUT_32
#define TOUCHPAD_XR_OUT                 GPIO_OUTPUT_36//GPIO_OUTPUT_32

#define TOUCHPAD_YU_IN                  GPIO_INPUT_31//GPIO_INPUT_30 
#define TOUCHPAD_YU_OUT                 GPIO_OUTPUT_31//GPIO_OUTPUT_30

#define TOUCHPAD_YD_IN                  GPIO_INPUT_32//GPIO_INPUT_36
#define TOUCHPAD_YD_OUT                 GPIO_OUTPUT_32//GPIO_OUTPUT_36
#endif
/*===========================================================================

                 DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
#define TOUCH_SCREEN_WIDTH	   (DISP_WIDTH)
#define TOUCH_SCREEN_HEIGHT	   (DISP_HEIGHT)
#define DELTA_MV			    40	//允许飘移的点距(该距离对应的电压值)
							        // 大约20 等于1  个点的距离
#define DELTA_MV2			    50

#define TOUCHPAD_SHIFT          13
#define TOUCHPAD_MV_MAX         0xFFF // ADC Max value
#if 1   //客户屏
#define TOUCHPAD_MVX_MAX        1008
#define TOUCHPAD_MVY_MAX        1020
#define INIT_MVX0               -96
#define INIT_MVY0              	-116
#else    //中广电屏
#define TOUCHPAD_MVX_MAX        992
#define TOUCHPAD_MVY_MAX       1020
#define INIT_MVX0              -116
#define INIT_MVY0              	-96
#endif

#define INIT_KMVX               (((TOUCH_SCREEN_WIDTH) <<TOUCHPAD_SHIFT)/(TOUCHPAD_MVX_MAX+INIT_MVX0))
#define INIT_KMVY               (((TOUCH_SCREEN_HEIGHT)<<TOUCHPAD_SHIFT)/(TOUCHPAD_MVY_MAX+INIT_MVY0))
#define MIN_KMVX                (((TOUCH_SCREEN_WIDTH) <<TOUCHPAD_SHIFT)/(TOUCHPAD_MV_MAX))
#define MIN_KMVY                (((TOUCH_SCREEN_HEIGHT)<<TOUCHPAD_SHIFT)/(TOUCHPAD_MV_MAX))
/*---------------------------------------------------------------------------
   polling macros, commands, definitions, and types
---------------------------------------------------------------------------*/
/* Interval after re-enabling polling before polling begins */
#define TOUCHPAD_POLL_TIME      20

/*---------------------------------------------------------------------------
                    touchpad State and touchpad Buffer Data
---------------------------------------------------------------------------*/
LOCAL pen_event_type            cur_pen_event;
LOCAL pen_value_type            cur_pen_value;

/* Ring buffer for holding pen data. */
LOCAL pen_data_array            pen_array = {0};
LOCAL pen_cal_type              pen_cal_data;
LOCAL int16                     touchpad_scan_interval = 0;
LOCAL PEN_MODE_TYPE             touchpad_scan_mode =PEN_NORMAL_MODE;


/* Clock call-back structure used to register the touchpad scanning routine
** to be called at regular intervals.
*/
LOCAL clk_cb_type               touchpad_clk_cb = {0}; 
LOCAL touchpad_event_pfn        touchpad_event_cb = NULL;
LOCAL boolean                   touchpad_polling_flag = FALSE;
LOCAL handwritting_cfg_type 	handwritting_cfg;

//用来记录笔按下的时间，超过5秒要踢狗
LOCAL int32                     down_tick = 0, 
                                current_tick = 0;
LOCAL boolean	touchpad_write_array(pen_event_type*pen_event);
LOCAL boolean   touchpad_press(void);
LOCAL boolean	touchpad_init_scanner(void);
LOCAL boolean   touchpad_dev_init = FALSE;

#if defined(T_SLEEP) && defined(FEATURE_NEW_SLEEP_API)
/* Variable to use the new sleep API */
char 							touchpadDeviceName[4] = {"TP"};
static sleep_okts_handle 		touchpad_sleep_handle = 0;
#endif /* T_SLEEP && FEATURE_NEW_SLEEP_API */

LOCAL void    					touchpad_pen_isr(void);
LOCAL void 						touchpad_enable_polling(void);
LOCAL void    					touchpad_disable_polling(void);
/*===========================================================================
MACRO TOUCHPAD_SLEEP_ALLOW
DESCRIPTION
    Tell the sleep task that we are in a state where we can sleep.
===========================================================================*/
#ifdef T_SLEEP
void TOUCHPAD_SLEEP_ALLOW(void)
{
#ifdef FEATURE_NEW_SLEEP_API
	sleep_assert_okts(touchpad_sleep_handle);
#else
	(void) rex_set_sigs( &sleep_tcb, SLEEP_TOUCHPAD_OKTS_SIG );
#endif
}
#else
#define TOUCHPAD_SLEEP_ALLOW()
#endif

/*===========================================================================
MACRO TOUCHPAD_SLEEP_FORBID
DESCRIPTION
    Tell the sleep task that we are NOT in a state where we can sleep.
===========================================================================*/
#ifdef T_SLEEP
void TOUCHPAD_SLEEP_FORBID()
{
#ifdef FEATURE_NEW_SLEEP_API
	sleep_negate_okts(touchpad_sleep_handle);
#else
	(void) rex_set_sigs( &sleep_tcb, SLEEP_TOUCHPAD_OKTS_SIG );
#endif
}
#else
#define TOUCHPAD_SLEEP_FORBID()
#endif

#ifndef ABS
#define ABS(VAL) (((VAL)>0)?(VAL):(-(VAL)))
#endif


/*===========================================================================

FUNCTION TOUCHPAD_INIT

DESCRIPTION
  This procedure initializes the touchpad operation.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void touchpad_init( void )
{
	INTLOCK();
	{
		cur_pen_event.pen_state = PEN_NONE;
		memset((char*)&pen_array,0x00,sizeof(pen_array));	
		memset((char*)&cur_pen_value,0x00,sizeof(cur_pen_value));	
		memset((char*)&pen_cal_data,0x00,sizeof(pen_cal_data));	
	    
		handwritting_cfg.color.fixed = FALSE;
		handwritting_cfg.color.begin = 0xf00f;
		handwritting_cfg.color.end = 0x0f00;
		handwritting_cfg.pixels = 2;
		handwritting_cfg.style = PEN_STYLE;
		pen_cal_data.mvx0 = INIT_MVX0;
		pen_cal_data.mvy0 = INIT_MVY0;
		pen_cal_data.kmvx = INIT_KMVX;
		pen_cal_data.kmvy = INIT_KMVY;	

		touchpad_polling_flag = FALSE;

		touchpad_set_scan_mode(PEN_NORMAL_MODE);
	    
	    /* Register the scanning routine with the Clock services,
	    ** so that it is called at regular intervals to poll the touchpad.
	    */
		clk_def( &touchpad_clk_cb );
		touchpad_event_cb = NULL;

#ifdef T_SLEEP
#ifdef FEATURE_NEW_SLEEP_API
		touchpad_sleep_handle = sleep_register(touchpadDeviceName, FALSE);
		if (0 == touchpad_sleep_handle)
		{
			ERR("[TP]: %s failed sleep_register", touchpadDeviceName, 0, 0);
		}
#endif
#endif
		touchpad_init_scanner();
	    touchpad_dev_init = TRUE;
	    touchpad_isr_open();
        MSG_FATAL("touchpad_isr_open............00",0,0,0);
    }

    INTFREE();
}

/*===========================================================================

FUNCTION TOUCHPAD_REGISTER

DESCRIPTION
  This procedure registers the touchpad callback function.
  Call this function with a function pointer to register.
  Call this function with NULL pointer to de-register.
  Only one client is supported.

DEPENDENCIES
  None

  if(cb==NULL) unregister and close isr
  else register and open isr

RETURN VALUE
  TRUE if register successfully, or use cb=NULL to unregister.
  FALSE if touchpad service was already registered and cb!=NULL.

SIDE EFFECTS
  None

===========================================================================*/

boolean touchpad_register( touchpad_event_pfn cb)
{
	MSG_FATAL("[TP]: touchpad_register", 0, 0, 0);
	
	if (touchpad_event_cb && cb) return(FALSE);

	touchpad_event_cb = cb;
	if(cb)
	{
		if(!touchpad_polling_flag)
		{
			touchpad_isr_open();
            MSG_FATAL("touchpad_isr_open............11",0,0,0);
		}
	}
	else
	{
		touchpad_disable_polling();
        touchpad_isr_close();
	}
	return(TRUE);
}

/*===========================================================================

FUNCTION touchpad_isr_open

DESCRIPTION
  打开笔中断

DEPENDENCIES
  None

RETURN VALUE

  return TRUE if succeed, return FALSE if failed.

SIDE EFFECTS
  None

===========================================================================*/
static uint16 touchpad_read_x(void);
boolean touchpad_isr_open()
{
    MSG_FATAL("[TP]: touchpad_isr_open", 0, 0, 0);
	gpio_tlmm_config(TOUCHPAD_XL_OUT);//gpio_tlmm_config(TOUCHPAD_YD_OUT);
	gpio_tlmm_config(TOUCHPAD_XR_IN);//gpio_tlmm_config(TOUCHPAD_YU_IN);
	//gpio_tlmm_config(TOUCHPAD_YU_OUT);
	//gpio_out(TOUCHPAD_YU_OUT,GPIO_LOW_VALUE);
	
	gpio_tlmm_config(TOUCHPAD_YD_IN);//gpio_tlmm_config(TOUCHPAD_XL_IN);
	//gpio_tlmm_config(TOUCHPAD_XL_OUT);
	//gpio_out(TOUCHPAD_XL_OUT,GPIO_HIGH_VALUE);
	
	gpio_tlmm_config(TOUCHPAD_YU_IN);//gpio_tlmm_config(TOUCHPAD_XR_IN);
	//gpio_tlmm_config(TOUCHPAD_XR_OUT);
	//gpio_out(TOUCHPAD_XR_OUT,GPIO_HIGH_VALUE);
	
	gpio_out(TOUCHPAD_XL_OUT,GPIO_HIGH_VALUE);//gpio_out(TOUCHPAD_YD_OUT,GPIO_LOW_VALUE);

	gpio_int_set_detect((gpio_int_type)TOUCHPAD_GPIO_INT,DETECT_LEVEL);
	
	/* Let the sleep task know it is now ok to sleep*/
	TOUCHPAD_SLEEP_ALLOW();
    MSG_FATAL("[TP]: touchpad_isr_open000", 0, 0, 0);
	return gpio_int_set_handler((gpio_int_type)TOUCHPAD_GPIO_INT, ACTIVE_HIGH, touchpad_pen_isr);
    MSG_FATAL("[TP]: touchpad_isr_open111", 0, 0, 0);
}

boolean touchpad_isr_close()
{
    MSG_FATAL("[TP]: touchpad_isr_close", 0, 0, 0);

	gpio_int_set_handler((gpio_int_type)TOUCHPAD_GPIO_INT, ACTIVE_HIGH, NULL);
	gpio_tlmm_config(TOUCHPAD_YD_IN);
	gpio_tlmm_config(TOUCHPAD_YU_IN);
	gpio_tlmm_config(TOUCHPAD_XL_IN);
	gpio_tlmm_config(TOUCHPAD_XR_IN);
	return TRUE;
}

/*===========================================================================

FUNCTION touchpad_pen_isr

DESCRIPTION
  Called when TRAMP detects a pen down

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  - turns on timer processing.
  - tells the sleep task that we are NOT in a state where we can sleep.
  - installs a touchpad polling timer.

===========================================================================*/

LOCAL void touchpad_pen_isr( void )
{
  	/* Ensure TCXO is enabled.
  	*/
  	MSG_FATAL("[TP]: touchpad_pen_isr", 0, 0, 0);

  	clk_unexpected_enable();
    
  	/* If a pen down is detected, start polling touchpad.
  	*/
	touchpad_isr_close();
	MSG_FATAL("[TP]: touchpad_enable_polling55555", 0, 0, 0);
  	touchpad_enable_polling();
}

/*===========================================================================
FUNCTION 	touchpad_lock
DESCRIPTION
    锁定触摸屏，不响应点笔
===========================================================================*/
boolean touchpad_lock(void)
{
    ERR("[TP]: touchpad_lock",0,0,0);
    
	if(touchpad_polling_flag)
	{
		touchpad_disable_polling();
        
		if(cur_pen_event.pen_state != PEN_UP && cur_pen_event.pen_state != PEN_NONE)
		{
			cur_pen_event.pen_state = PEN_UP;
		    touchpad_write_array(&cur_pen_event);
            cur_pen_event.pen_state = PEN_NONE;
		}
	}
	touchpad_isr_close();
	return TRUE;
}

/*===========================================================================
FUNCTION 	touchpad_unlock
DESCRIPTION
    触摸屏解 锁，恢复正常响应，与touchpad_lock 配对使用.
===========================================================================*/
boolean touchpad_unlock(void)
{
	if(touchpad_polling_flag)
	{
		return FALSE;
	}
    
	touchpad_isr_open();
	return TRUE;
}

/*===========================================================================
FUNCTION touchpad_set_scan_mode
DESCRIPTION
  设笔按下时，扫描笔位置的模式。
===========================================================================*/
boolean touchpad_set_scan_mode( PEN_MODE_TYPE mode )
{	
	if(mode == PEN_DISPTRACK_MODE)
    {
	    touchpad_scan_interval =  TOUCHPAD_MIN_INTERVAL; 	
	}
	else
    {
	    touchpad_scan_interval = TOUCHPAD_DEFAULT_INTERVAL;
	}
	
	if(touchpad_polling_flag)
	{
        touchpad_disable_polling();
        touchpad_scan_mode = mode;
		MSG_FATAL("[TP]: touchpad_enable_polling444", 0, 0, 0);
		touchpad_enable_polling();
	}
    else
    {
        touchpad_scan_mode = mode;
    }
	return TRUE;
}

PEN_MODE_TYPE touchpad_get_scan_mode(void)
{
	return touchpad_scan_mode;
}

void  touchpad_start_next_cycle(int4 ms_interval)
{
    extern rex_tcb_type hs_tcb; 
	rex_set_sigs(&hs_tcb, HS_TOUCHPAD_TIMER_SIG);
}

LOCAL void touchpad_enable_polling(void)
{
	MSG_FATAL("[TP]: touchpad_enable_polling", 0, 0, 0);
#ifdef T_SLEEP
	/* Let the sleep task know it is not ok to sleep
	*/
#ifdef FEATURE_NEW_SLEEP_API
	sleep_negate_okts(touchpad_sleep_handle);
#else
	(void) rex_clr_sigs( &sleep_tcb, SLEEP_TOUCHPAD_OKTS_SIG );
#endif

	/* 
	** If polling is not currently enabled, then uninstall the keypress
	**  detection ISR, and reenabled the keypad polling timer
	*/
	MSG_FATAL("[TP]: touchpad_enable_polling2222", 0, 0, 0);
	INTLOCK();
	if ( !touchpad_polling_flag )
	{
		touchpad_polling_flag = TRUE;

		/* 
		** Resume polling of the touchpad.
		*/
		clk_reg( &touchpad_clk_cb, touchpad_start_next_cycle, TOUCHPAD_POLL_TIME, 0, FALSE );
	}
	INTFREE();
	MSG_FATAL("[TP]: touchpad_enable_polling3333", 0, 0, 0);
#endif /* T_SLEEP */
} /* end of keypad_enable_polling */

LOCAL void touchpad_disable_polling()
{
#ifdef T_SLEEP

    MSG_FATAL("[TP]: touchpad_disable_polling", 0, 0, 0);

#ifdef FEATURE_NEW_SLEEP_API
	sleep_assert_okts(touchpad_sleep_handle);
#else
	(void) rex_set_sigs( &sleep_tcb, SLEEP_TOUCHPAD_OKTS_SIG );
#endif

  	/* If polling is currently enabled, disabled the touchpad polling timer,
	** and install the pen detection ISR.
	*/
  	INTLOCK();
	if (touchpad_polling_flag )
	{
    	clk_dereg( &touchpad_clk_cb );
		touchpad_polling_flag = FALSE;
  	}
  	INTFREE();
#endif
} 



/*===========================================================================
FUNCTION touchpad_check_pen_value
DESCRIPTION
  过滤突然漂移的点(在笔按下的过程中，与前面的点离开太远的点)
===========================================================================*/
boolean touchpad_check_pen_value(pen_event_type*pen_event,pen_value_type *pen_value)
{
	#define TRACE_NUM			4
	#define TRACE_MASK			3
	boolean ret = TRUE;
	int16 i,x,y,count;
	static int32 trace_num = 0,err_count = 0;
	int32  delta_mv = DELTA_MV2;
	static int16 old_x[TRACE_NUM],old_y[TRACE_NUM];

	x = pen_value->pen_x_mv;
	y = pen_value->pen_y_mv;
    MSG_FATAL("x===%d,y=====%d",x,y,0);
	if( touchpad_scan_mode == PEN_DISPTRACK_MODE)
	{
		delta_mv = (DELTA_MV2<<2); // *4  
	}
	else
	{
		delta_mv = (DELTA_MV<<3); //*16   
	}
    MSG_FATAL("old_x[0]==%d,,,old_y[0]===%d",old_x[0],old_y[0],0);
    MSG_FATAL("old_x[1]==%d,,,old_y[1]===%d",old_x[0],old_y[0],0);
    MSG_FATAL("old_x[2]==%d,,,old_y[2]===%d",old_x[0],old_y[0],0);
    MSG_FATAL("old_x[3]==%d,,,old_y[3]===%d",old_x[0],old_y[0],0);
	if(pen_event->pen_state == PEN_DOWN ||pen_event->pen_state == PEN_STAY || pen_event->pen_state == PEN_MOVE)
	{
		if(trace_num >=1)
		{
			ret = FALSE;
			for(i = 0,count =0; i < trace_num && i < TRACE_NUM; i++)
			{
				if(x - old_x[i] < delta_mv && x - old_x[i] > -delta_mv && y - old_y[i] < delta_mv && y - old_y[i] > -delta_mv)
				{

					count++;
					
					if(count >= 1)
					{
						ret = TRUE;
						break;
					}
				}
			}
				
		}
	}
	else
	{
		trace_num = 0;
	}
	if(ret)
	{
		old_x[trace_num & TRACE_MASK] = pen_value->pen_x_mv;
		old_y[trace_num & TRACE_MASK] = pen_value->pen_y_mv;
		trace_num++;
		err_count = 0;
	}
	else
	{
		err_count++;
		if(err_count > 2)
			trace_num = 0;
	}
	return ret;

}	

boolean 	touchpad_mv_2_xy(pen_event_type*pen_event,pen_value_type *pen_value)
{
	int32 x,y;

	/*去除抖动*/
    if(!touchpad_check_pen_value(pen_event,pen_value)) 
    {
        MSG_FATAL("[TP]: touchpad_check_pen_value_Fail",0,0,0);
        return FALSE;
    }
	MSG_FATAL("REMOVE DDtouchpad_read_value x = %d y = %d",pen_value->pen_x_mv,pen_value->pen_y_mv,0);
    x = (int32)((pen_value->pen_x_mv+pen_cal_data.mvx0)* pen_cal_data.kmvx)>>TOUCHPAD_SHIFT;
    y = (int32)((pen_value->pen_y_mv+pen_cal_data.mvy0)* pen_cal_data.kmvy)>>TOUCHPAD_SHIFT;
    
    pen_event->pen_x = x;
    pen_event->pen_y = y;
    MSG_FATAL("[TP]: touchpad_mv_2_xy x = %d y = %d",x,y,0);
    return TRUE;
}

void touchpad_xy_2_mv(int16 x, int16 y, int16 *x_mv, int16 *y_mv)
{
    int32 mv;
    MSG_FATAL("pen_cal_data.kmvx===%d,pen_cal_data.mvx0====%d",pen_cal_data.kmvx,pen_cal_data.mvx0,0);
    if(x_mv)
    {
        mv = ((x<<TOUCHPAD_SHIFT)/pen_cal_data.kmvx)-pen_cal_data.mvx0;
        *x_mv = (int16)mv;
    }
    
    if(y_mv)
    {
        mv = ((y<<TOUCHPAD_SHIFT)/pen_cal_data.kmvy)-pen_cal_data.mvy0;
        *y_mv = (int16)mv;
    }
    MSG_FATAL("[TP]: touchpad_xy_2_mv %d %d",*x_mv,*y_mv,0);
}

LOCAL boolean   touchpad_write_array(pen_event_type*pen_event)
{
	memmove((char *)&pen_array.pen_data[pen_array.write_p],(char *)pen_event,sizeof(pen_event_type));
	pen_array.write_p = (pen_array.write_p + 1)%PEN_ARRAY_SIZE;
	if(pen_array.write_p == pen_array.read_p) 
	{
		pen_array.read_p = (pen_array.read_p + 1)%PEN_ARRAY_SIZE;
	}

    if(touchpad_event_cb)
	{
		(touchpad_event_cb)(cur_pen_event);
	}
	
	return TRUE;
}

boolean	touchpad_read_array(pen_event_type*pen_event)
{
	if( pen_array.read_p == pen_array.write_p)
	{
		return FALSE;
	}

	memmove((char *)pen_event,(char *)&pen_array.pen_data[pen_array.read_p],sizeof(pen_event_type));
	pen_array.read_p = (pen_array.read_p + 1)%PEN_ARRAY_SIZE;
	return TRUE;
}

boolean touchpad_get_pen_position(pen_event_type  *pen_data)
{
	return touchpad_read_array(pen_data);
}

boolean touchpad_get_pen_value(pen_value_type  *pen_value)
{
	memmove((char*)pen_value,(char*)&cur_pen_value,sizeof(pen_value_type));
	return TRUE;
}

boolean touchpad_press()
{
	gpio_tlmm_config(TOUCHPAD_XL_OUT);
	gpio_tlmm_config(TOUCHPAD_XR_IN);
	gpio_tlmm_config(TOUCHPAD_YD_IN);
	gpio_tlmm_config(TOUCHPAD_YU_IN);
	gpio_out(TOUCHPAD_XL_OUT,GPIO_HIGH_VALUE);
	if(gpio_in((GPIO_SignalType)TOUCHPAD_YU_IN)==GPIO_HIGH_VALUE)
    {
        MSG_FATAL("[TP]: touchpad_press TRUE",0,0,0);
        return TRUE;
    }
    else
    {
        MSG_FATAL("[TP]: touchpad_press FALSE",0,0,0);
        return FALSE;
    }
}

void touchpad_polling(int4 ms_interval)
{
	pen_event_type pen_event;
	qword uptime_ms;

	MSG_FATAL("[TP]: touchpad_polling!",0,0,0);
    if ( touchpad_dev_init == FALSE )
	{
		touchpad_init();
	}
	
    if(touchpad_press())
    {
        memcpy((char*)&pen_event,(char*)&cur_pen_event,sizeof(pen_event));
        
        if(touchpad_read_value(&cur_pen_value)) 
        {
            if(touchpad_mv_2_xy(&pen_event,&cur_pen_value))
            {
                clk_uptime_ms(uptime_ms);   /* Returned uptime, in millisecond units. */
                cur_pen_event.update_tick = uptime_ms[0];
                switch(cur_pen_event.pen_state){
                case PEN_NONE:
                    cur_pen_event.pen_state = PEN_DOWN;
                    down_tick =  cur_pen_event.update_tick;
                    current_tick = down_tick;
                    cur_pen_event.pen_x = 240-pen_event.pen_x;
                    cur_pen_event.pen_y = 320-pen_event.pen_y;
                    touchpad_write_array(&cur_pen_event);
                    break;

                case PEN_DOWN:
                case PEN_MOVE:
                case PEN_STAY:
                    cur_pen_event.pen_state = PEN_MOVE;
                    current_tick = cur_pen_event.update_tick;
                    if(pen_event.pen_x == cur_pen_event.pen_x && pen_event.pen_y == cur_pen_event.pen_y)
                    {
                        cur_pen_event.pen_state = PEN_STAY;
                    }
                    else
                    {
                        cur_pen_event.pen_x =240- pen_event.pen_x;
                        cur_pen_event.pen_y =320- pen_event.pen_y;
                        touchpad_write_array(&cur_pen_event);
                    }
                    break;

                case PEN_UP:
                    // Error State
                    MSG_FATAL("[TP]: touchpad_polling ERROR Penup State", 0, 0, 0);
                    break;
                default:
                    // Nothing todo
                    break;
                }
            }
        }
        clk_reg( &touchpad_clk_cb, touchpad_start_next_cycle, touchpad_scan_interval, 0, FALSE );
    }
    else
	{
        touchpad_disable_polling();
        MSG_FATAL("cur_pen_event.pen_state===%d",cur_pen_event.pen_state,0,0);
		if(cur_pen_event.pen_state != PEN_NONE)
		{
			cur_pen_event.pen_state = PEN_UP;
        	touchpad_write_array(&cur_pen_event);
        	cur_pen_event.pen_state = PEN_NONE;
		}
		 /* Install an ISR to detect when  pen up*/
		touchpad_isr_open();
	}
	return;	
}

boolean	touchpad_init_scanner(void)
{
	gpio_tlmm_config(TOUCHPAD_YD_IN);
	gpio_tlmm_config(TOUCHPAD_YU_IN);
	gpio_tlmm_config(TOUCHPAD_XL_IN);
	gpio_tlmm_config(TOUCHPAD_XR_IN);
	return TRUE;
}

static uint16 touchpad_read_x(void)
{
	gpio_tlmm_config(TOUCHPAD_YD_IN);
	gpio_tlmm_config(TOUCHPAD_YU_IN);
	gpio_tlmm_config(TOUCHPAD_XL_OUT);
	gpio_tlmm_config(TOUCHPAD_XR_OUT);
	gpio_out(TOUCHPAD_XL_OUT,GPIO_HIGH_VALUE);
	gpio_out(TOUCHPAD_XR_OUT,GPIO_LOW_VALUE);
	
    // Read from ADC1
	return (uint16)adc_read(ADC_TOUCHPAD_X);
}

static uint16 touchpad_read_y(void)
{
    gpio_tlmm_config(TOUCHPAD_YD_OUT);
	gpio_tlmm_config(TOUCHPAD_YU_OUT);
	gpio_tlmm_config(TOUCHPAD_XL_IN);
	gpio_tlmm_config(TOUCHPAD_XR_IN);
	gpio_out(TOUCHPAD_YD_OUT,GPIO_HIGH_VALUE);
	gpio_out(TOUCHPAD_YU_OUT,GPIO_LOW_VALUE);
    
	// Read from ADC0
	return (uint16)adc_read(ADC_TOUCHPAD_Y);
}

// Sample touch data
boolean  touchpad_evaluate_sample(uint16 val0,uint16 val1,uint16 val2, int16*value)
{
    int16    diff0,diff1,diff2;
    boolean ret = FALSE;

    // Calculate the absolute value of the differences of the sample
    diff0 = val0 - val1;
    diff1 = val1 - val2;
    diff2 = val2 - val0;
    diff0 = diff0 > 0  ? diff0 : -diff0;
    diff1 = diff1 > 0  ? diff1 : -diff1;
    diff2 = diff2 > 0  ? diff2 : -diff2;

    // Eliminate the one away from other two and add the two
    if(diff0 <=DELTA_MV && diff2 <=DELTA_MV&& diff1 <=DELTA_MV)  
    {
        if (diff0 < diff1) 
        {
            *value = (uint16)(val0 + ((diff2 < diff0) ? val2 : val1));
            ret = TRUE;
        }
        else  
        {
            *value = (uint16)(val2 + ((diff2 < diff1) ? val0 : val1));
            ret = TRUE;
        }
        (*value) >>= 1;
    }
    return ret;
}

/*===========================================================================

FUNCTION touchpad_read_value
DESCRIPTION
 读取笔的状态和点击产生的电压值。

===========================================================================*/
boolean touchpad_read_value(pen_value_type *pen_value)
{
	pen_value->pen_x_mv = touchpad_read_x();
	pen_value->pen_y_mv = touchpad_read_y();

	//值对调
	pen_value->pen_x_mv = pen_value->pen_x_mv;
	pen_value->pen_y_mv = pen_value->pen_y_mv;
	if(pen_value->pen_x_mv<=(-INIT_MVX0))
	{
		pen_value->pen_x_mv = (-INIT_MVX0)+1;
		
	}
	if(pen_value->pen_y_mv<=(-INIT_MVY0))
	{
		pen_value->pen_y_mv = (-INIT_MVY0)+1;
		
	}
   
    MSG_FATAL("touchpad_read_value x = %d y = %d",pen_value->pen_x_mv,pen_value->pen_y_mv,0);
    pen_value->pen_x_mv = touchpad_read_x();
	pen_value->pen_y_mv = touchpad_read_y();
    MSG_FATAL("0000touchpad_read_value x = %d y = %d",pen_value->pen_x_mv,pen_value->pen_y_mv,0);
    return TRUE;
}

boolean touchpad_check_cal_data(int16 mvx0, int16 mvy0, int32 kmvx, int32 kmvy)
{
	if(ABS(mvx0) > 2*ABS(INIT_MVX0) ||
	   ABS(mvy0) > 2*ABS(INIT_MVY0) ||
	   ABS(kmvx) > 2*MIN_KMVX || ABS(kmvx) < MIN_KMVX ||
	   ABS(kmvy) > 2*MIN_KMVY || ABS(kmvy) < MIN_KMVY)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

/*===========================================================================

FUNCTION touchpad_adjust
DESCRIPTION
把 (从NV  读取的)笔校准参数设置到本驱动

===========================================================================*/
boolean touchpad_adjust(pen_cal_type *adjust_data)
{
    if(adjust_data == NULL)
    {
        pen_cal_data.mvx0 = INIT_MVX0;
    	pen_cal_data.mvy0 = INIT_MVY0;
    	pen_cal_data.kmvx = INIT_KMVX;
    	pen_cal_data.kmvy = INIT_KMVY;	
        return TRUE;
    }
    
	if(!touchpad_check_cal_data(adjust_data->mvx0,adjust_data->mvy0,adjust_data->kmvx,adjust_data->kmvy))
	{
		return FALSE;
	}
    
	memcpy((char*)&pen_cal_data,(char*)adjust_data,sizeof(pen_cal_data));
	return TRUE;
}

/*===========================================================================
FUNCTION touchpad_adjust
DESCRIPTION
计算笔校准参数
===========================================================================*/
boolean touchpad_adjust_cal(pen_cal_param_type* pen_param,byte param_num,pen_cal_type *adjust_data)
{
	byte i,j; //将用于计算参数的两个点
	byte k=0; // 用于验证校准参数是否精确的点
	long delta01,delta02,delta12;
	boolean ret = TRUE;
	int16 mvx0,mvy0,x,y;
	int32 kmvx,kmvy;
	
	if(param_num < 2)
	{
		return FALSE;
	}
    
	if(param_num < 3)
	{
		i=0;
		j=1;
	}
	else
	{
		MSG_FATAL("pen_param[0].x_disp==%d,pen_param[0].y_disp==%d",pen_param[0].x_disp,pen_param[0].y_disp,0);
		MSG_FATAL("pen_param[1].x_disp==%d,pen_param[1].y_disp==%d",pen_param[1].x_disp,pen_param[1].y_disp,0);
		MSG_FATAL("pen_param[2].x_disp==%d,pen_param[2].y_disp==%d",pen_param[2].x_disp,pen_param[2].y_disp,0);
		delta01=ABS(pen_param[0].x_disp - pen_param[1].x_disp) *ABS(pen_param[0].y_disp - pen_param[1].y_disp); 
		delta02=ABS(pen_param[0].x_disp - pen_param[2].x_disp) *ABS(pen_param[0].y_disp - pen_param[2].y_disp); 
		delta12=ABS(pen_param[1].x_disp - pen_param[2].x_disp) *ABS(pen_param[1].y_disp - pen_param[2].y_disp); 
		MSG_FATAL("delta01===============%d",delta01,0,0);
		MSG_FATAL("delta02===============%d",delta02,0,0);
		MSG_FATAL("delta12===============%d",delta12,0,0);
		if(delta01 >= delta02 && delta01 >= delta12)
		{
			i=0;
			j=1;
			k=2;
		}
		else if(delta02 >= delta01 && delta02 >= delta12)
		{
			i=0;
			j=2;
			k=1;			
		}
		else 
		{
			i=1;
			j=2;
			k=0;			
		}
	}
	
    if(pen_param[i].x_mv-pen_param[j].x_mv ==0)
    {
    	return FALSE;
    	pen_param[i].x_mv++;
    }
	if(pen_param[i].y_mv-pen_param[j].y_mv ==0)
	{
		return FALSE;
		pen_param[i].y_mv++;	
	}
	kmvx = (int32)((pen_param[i].x_disp-pen_param[j].x_disp)<<TOUCHPAD_SHIFT)/(pen_param[i].x_mv-pen_param[j].x_mv);
	kmvy = (int32)((pen_param[i].y_disp-pen_param[j].y_disp)<<TOUCHPAD_SHIFT)/(pen_param[i].y_mv-pen_param[j].y_mv);
	
	if(kmvx - pen_param[i].x_mv ==0)
	{
		return FALSE;
		kmvx ++;
	}
	if(kmvy - pen_param[i].y_mv ==0)
	{
		return FALSE;
		kmvy++;
	}
	if(kmvx - pen_param[j].x_mv ==0)
	{
		return FALSE;
		kmvx++;
	}
	if(kmvx - pen_param[j].y_mv ==0)
	{
		return FALSE;
		kmvy++;
	}
	if(kmvx == 0)
	{
		return FALSE;
		kmvx++;
	}
	if(kmvy == 0)
	{
		return FALSE;
		kmvy++;
	}
	
	MSG_FATAL("kmvx===%d,,kmvx===%d",kmvx,kmvy,0);
    mvx0 = (((pen_param[i].x_disp<<TOUCHPAD_SHIFT)/kmvx - pen_param[i].x_mv)+((pen_param[j].x_disp<<TOUCHPAD_SHIFT)/kmvx - pen_param[j].x_mv))/2;
    mvy0 = (((pen_param[i].y_disp<<TOUCHPAD_SHIFT)/kmvy - pen_param[i].y_mv)+((pen_param[j].y_disp<<TOUCHPAD_SHIFT)/kmvy - pen_param[j].y_mv))/2;
	MSG_FATAL("[TP]: touchpad_adjust_cal kmvx=%d kmvy=%d", kmvx, kmvy, 0);
    MSG_FATAL("[TP]: touchpad_adjust_cal mvx0=%d mvy0=%d", mvx0, mvy0, 0);

	if(param_num >=3)
	{
		x = (kmvx*(pen_param[k].x_mv+mvx0))>>TOUCHPAD_SHIFT;
		y = (kmvy*(pen_param[k].y_mv+mvy0))>>TOUCHPAD_SHIFT;
        MSG_FATAL("x===========%d,y=====%d",x,y,0);
		if(ABS(x-pen_param[k].x_disp) > 8 || ABS(y-pen_param[k].y_disp) > 8)
		{
			ret = FALSE;
		}
	}
    MSG_FATAL("ret=======0000======%d",ret,0,0);
	if(ret)
	{
		MSG_FATAL("ret=======0011======",0,0,0);
		if(FALSE/*!touchpad_check_cal_data(mvx0,mvy0,kmvx,kmvy)*/)
		{
			ret =  FALSE;
		}
		else
		{
			if(adjust_data)
			{
                adjust_data->mvx0 = mvx0;
    			adjust_data->mvy0 = mvy0;
    			adjust_data->kmvx = kmvx;
    			adjust_data->kmvy = kmvy;
			}
		}
	}
	MSG_FATAL("ret======11111======%d",ret,0,0);
	return ret;
}

