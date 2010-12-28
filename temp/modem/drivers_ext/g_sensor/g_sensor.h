#ifndef __G_SENSOR_H_
#define __G_SENSOR_H_
/*===========================================================================
        
              G-Sensor Driver Public Header

DESCRIPTION
  All the declarations and definitions necessary to support interaction
  with the FM Radio functions for the MSM60x0.

REFERENCES

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

===========================================================================*/

#include "comdef.h"

#define HS_GSENSOR_ENABLE_DELAY 20
#define HS_PEDOMETER_TIME   20
#define HS_SHAKE_TIME       40
#define HS_NAVIGATION_TIME  500


//#define     g_sensor_log_enable

typedef enum
{      
    G_SENSOR_NONE               = 0,
    G_SENSOR_ENABLE             = 0x0001, 
    G_SENSOR_DISABLE            = 0x0002,
    G_SENSOR_PEDOMETER          = 0x0004, 
    G_SENSOR_PEDOMETER_PAUSE    = 0x0008,
    G_SENSOR_PEDOMETER_DISABLE  = 0X0010,   
    G_SENSOR_SHAKE              = 0x0020,
    G_SENSOR_SHAKE_DISABLE      = 0x0040,    
    G_SENSOR_VIDEO_CIRC         = 0x0080,
    G_SENSOR_VIDEO_CIRC_DISABLE = 0x0100,
    G_SENSOR_NAVIGATION_ENABLE  = 0x0200, 
    G_SENSOR_NAVIGATION         = 0x0400,
    G_SENSOR_NAVIGATION_DISABLE = G_SENSOR_DISABLE
    
}   g_sensor_cmd_type;

typedef enum
{   
    MMI_GSENSOR_SHAKE_OPEN,
    MMI_GSENSOR_SHAKE_CLOSE,
    MMI_GSENSOR_SHAKE_OPEN_IN_IDLE,
    MMI_GSENSOR_SHAKE_CLOSE_IN_IDLE,
    
}   mmi_g_sensor_state_type;

extern g_sensor_cmd_type g_sensor_cmd;
extern mmi_g_sensor_state_type mmi_g_sensor_state;

/*==============================================================================
函数: 
     mmi_g_sensor_process
             
说明: 
     g_sensor for MMI接口函数
       
参数: 
     g_sensor 处理事件类型, 若需要ENABLE事件与其他任务一起处理,请使用以下方式,
     如:mmi_g_sensor_process(G_SENSOR_ENABLE | G_SENSOR_PEDOMETER)
              
返回值:无
       
备注:
       应先完成函数g_sensor_init()
==============================================================================*/
extern void mmi_g_sensor_process(g_sensor_cmd_type);


/*==============================================================================
函数: 
     g_sensor_init
             
说明: 
     初始化G_sensor
       
参数: 
              
返回值:无
       
备注:
       应先调用g_sensor_device_init()
==============================================================================*/
boolean g_sensor_init(void);

/*==============================================================================
函数: 
     g_sensor_enable
             
说明: 
     确认G_sensor已经处于工作状态
       
参数: 
              
返回值: 无
       
备注:
      Enable的时候只是改变标志,并不实际打开物理设备
==============================================================================*/
void g_sensor_enable(void);

/*==============================================================================
函数: 
     g_sensor_disable
             
说明: 
     确认G_sensor已经处于非工作状态
       
参数: 
              
返回值: 无
       
备注:
     Disable时改变标志,同时关闭物理设备    
==============================================================================*/
void g_sensor_disable(void);

extern void g_sensor_pedometer(void);

extern void g_sensor_pedometer_pause(void);

extern void g_sensor_shake(void);

extern void g_sensor_video_circ(void);

extern int ReadPedoCounter(void);

extern void ClrPedoCounter(void);

extern void ClrShakeSum(void);

extern void ClrVideoCircSum(void);

extern void g_sensor_nav_get_std(void);

extern void g_sensor_navigation(void);
#endif //__G_SENSOR_H_