#ifndef TOUCHPAD_H
#define TOUCHPAD_H
/*===========================================================================

 Touchpad.h
 Copyright(c) Anylook
===========================================================================*/


#include "comdef.h"      /* Definitions for byte, word, etc.     */
#include "customer.h"    /* Customer specific definitions        */
#include "target.h"      /* Target specific definitions          */
#include "hs.h"
#include "gpio_int.h"

/*===========================================================================

                      MACRO DECLARATIONS

===========================================================================*/
#define PEN_ARRAY_SIZE			32
#define DEBUG_PEN_POINT_ON_LCD	1

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
typedef enum 
{
	PEN_NONE=0,		//笔状态不是以下的情况,指笔已经抬起。
	PEN_DOWN,		//笔刚按下
	PEN_STAY,		//笔按住不动
	PEN_MOVE,		//笔按住移动
	PEN_UP			//笔刚抬起
}PEN_STATE_TYPE;

typedef enum 
{
	PEN_NORMAL_MODE = 0,
	PEN_DISPTRACK_MODE, // Display the pen track on Disp device
	PEN_MAX_MODE
}PEN_MODE_TYPE;

typedef struct
{
	PEN_STATE_TYPE  pen_state;		/* The current pen state                                */
	int32           update_tick;    /* the tick (ms) when the pen_x pen_y were updated */		
	int16           pen_x; 	  		/* if pen hits the pad, it holds the x position       */
	int16           pen_y;        	/* if pen hits the pad, it holds the y position       */
}pen_event_type;

typedef struct
{
	int16 pen_x_mv; 	  	/* if pen hits the pad, it holds the x volt value      */
	int16 pen_y_mv;         /* if pen hits the pad, it holds the y volt value      */
}pen_value_type;

typedef struct
{
	int16 x_mv; 	  	/*点击屏幕上" 十" 字, 读到的 x 方向电压     */
	int16 y_mv;         /*点击屏幕上" 十" 字, 读到的 y 方向电压     */
	int16 x_disp;       /*屏幕上" 十" 字的x 方向的LCD 坐标          */  	
	int16 y_disp;       /*屏幕上" 十" 字的y 方向的LCD 坐标          */  	
}pen_cal_param_type;


//笔坐标 和状态缓冲数据结构：
typedef struct 
{
	int16 read_p;
	int16 write_p;
	pen_event_type  pen_data[PEN_ARRAY_SIZE];
}pen_data_array;

//笔校准数据结构：
typedef PACKED struct 
{
	int16 mvx0;		// x=kmvx*(mvx+mvx0);  假设为线性
	int16 mvy0;		// y=kmvy*(mvy+mvy0);
	int32 kmvx;  	// x方向的斜率的1024倍
	int32 kmvy; 	// y方向的斜率的1024倍
}pen_cal_type;

typedef struct 
{
	boolean fixed;  //是否固定: 1固定为起始颜色， 0: 由起始颜色变化到终止颜色。
	int32 begin;    //起始颜色
	int32 end;      //终止颜色
}pen_color;

typedef enum 
{
	NORMAL_STYLE,
	PEN_STYLE,
	BRUSH_STYLE,		
}pen_style;


//笔迹风格结构：
typedef struct 
{
	byte pixels;		//笔迹宽度
	pen_color color;
	pen_style style;
}handwritting_cfg_type;

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/
typedef boolean (*touchpad_event_pfn)( pen_event_type pen_event);

void    touchpad_init( void );
boolean touchpad_register( touchpad_event_pfn cb);
boolean touchpad_adjust(pen_cal_type *pen_adjust_data);
boolean touchpad_adjust_cal(pen_cal_param_type* pen_param,byte param_num,pen_cal_type *cal_data);
void    touchpad_xy_2_mv(int16 x, int16 y, int16 *x_mv, int16 *y_mv);
boolean touchpad_get_pen_position(pen_event_type  *pen_data);
boolean touchpad_get_pen_value(pen_value_type  *pen_value);
int16   touchpad_get_pen_array_len(void);
boolean touchpad_clr_pen_data_buf(void);
boolean touchpad_set_scan_mode( PEN_MODE_TYPE mode ); 
PEN_MODE_TYPE touchpad_get_scan_mode(void);
void    touchpad_pass_pen_data( pen_event_type pen_event);
boolean touchpad_isr_open(void);
void    touchpad_pen_isr(void);
boolean touchpad_isr_close(void);
boolean touchpad_enable_polling(void);
void    touchpad_polling(int4 ms_intervals);
void    touchpad_disable_polling(void);
boolean touchpad_read_value(pen_value_type *pen_value);
boolean touchpad_lock(void);
boolean touchpad_unlock(void);
boolean touchpad_handwritting_cfg(handwritting_cfg_type* pen_cfg);
#if DEBUG_PEN_POINT_ON_LCD
boolean disp_one_pixel_on_lcd(pen_event_type pen_event);
#endif
#endif /* TOUCHPAD_H */







