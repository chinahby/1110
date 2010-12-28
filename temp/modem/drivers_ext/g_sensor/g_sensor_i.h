#ifndef __G_SENSOR_I_H_
#define __G_SENSOR_I_H_
/*===========================================================================
        
              G-Sensor Driver Internal Header

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
#include "customer.h"
#include "gpio_60x0.h"
#include "bio.h"
#include "i2c.h"
#include "clk.h"
#include "msg.h"

typedef enum
{
    G_SENSOR_NOT_INIT,
    G_SENSOR_POWER_DOWN,
    G_SENSOR_POWER_UP,
    G_SENSOR_IDLE_STATUS = G_SENSOR_POWER_UP,
    
    G_SENSOR_MAX_STATUS = 0xFFFF
}   g_sensor_status_type;

typedef struct
{
    dword       dwStaticValue;      //水平静止时的输出。
    dword       dwOffestPerG;       //每个G加速度的偏移。
} g_sensor_device_info_type;

//记录G_SENSOR当前的物理工作状态
extern g_sensor_status_type g_sensor_work_status;

#define G_SENSOR_SUCCESSFUL     0
#define G_SENSOR_FAILED         1

#define delay(t)              clk_busy_wait((t)*1000)

#define G_SENSOR_CMD_TIMER      10

int g_sensor_device_init(void);

int g_sensor_power_up(void);

int g_sensor_power_down(void);


/*==============================================================================
函数: 
     g_sensor_get_device_info
             
说明: 
     获取G_sensor芯片的基本数据信息
       
参数: 
     pDeviceInfo:指向GSENSOR芯片基本数据的指针
              
返回值: GSENSOR芯片的基本数据
       
备注:
       
==============================================================================*/
void g_sensor_get_device_info(g_sensor_device_info_type *pDeviceInfo);

/*==============================================================================
函数: 
      g_sensor_get_acceleration
       
说明: 
      获取G_SENSOR在X和Y方向上的原始输出
       
参数:       
      x_poiner:  指向X轴原始输出的指针
      y_pointer: 指向Y轴原始输出的指针
        
返回值:
        G_SENSOR_FAILED：0
        G_SENSOR_SUCCESS：1
       
备注:
      当前工作状态应为G_SENSOR_POWER_UP
       
==============================================================================*/
int g_sensor_get_acceleration(int16 *x_pointer, int16 *y_pointer);

#endif //__G_SENSOR_I_H_