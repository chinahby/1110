/*===========================================================================

        G-Sensor Driver for MXC6202 Chipset

DESCRIPTION
  All the declarations and definitions necessary to support interaction
  with the G-Sensor functions for the MSM60x0.

REFERENCES

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

===========================================================================*/
#include "g_sensor.h"
#include "g_sensor_i.h"
 
//--------------------------------------------------------------------------    
#define  KICK_WATCHDOG()   /*lint -e717 do ... while(0) */  \
                           do {                             \
   (void)outp(HWIO_WDOG_RESET_ADDR, HWIO_WDOG_RESET_RMSK);  \
                              }                             \
                           while (0)                        \
                           /*lint +e717 */
//--------------------------------------------------------------------------        
                   
    
//MXC6202 Register address    
#define CONTROL_REG             0x00

#define G_SENSOR_PD_MASK        0x01
#define G_SENSOR_PW_ON          0x00
#define G_SENSOR_PW_OFF         0x01

#define G_SENSOR_ST_MASK        0x02
#define G_SENSOR_ST_ON          0x02
#define G_SENSOR_ST_OFF         0x00

#define X_AXIS_MSB_REG          0x01
#define X_AXIS_LSB_REG          0x02
#define Y_AXIS_MSB_REG          0x03
#define Y_AXIS_LSB_REG          0x04

#define REG_SIZE                5
#define REG_READ_SIZE           REG_SIZE
#define REG_WRITE_SIZE          1

#define G_SENSOR_I2C_SLAVE_ADDR       0x2A

//芯片的基本输出
#define GS_MXC6202_STATIC_BASE    2048      //ox800
#define GS_MXC6202_OFFSET_PER_G   512      //ox200

static byte i2c_data_buf[REG_SIZE]  = {0};

static i2c_rw_cmd_type i2c_rw_cmd;

//Reset write buffer value to chipset reset value;
LOCAL void g_sensor_init_i2c_cmd_data()
{    
    i2c_rw_cmd.bus_id = I2C_BUS_HW_CTRL;
    i2c_rw_cmd.slave_addr = G_SENSOR_I2C_SLAVE_ADDR;
    i2c_rw_cmd.options = I2C_REG_DEV;
    i2c_rw_cmd.buf_ptr = i2c_data_buf;
    i2c_rw_cmd.addr.reg = 0;
    i2c_rw_cmd.len = 0;
    i2c_rw_cmd.last_index = 0;
}    

LOCAL i2c_status_type g_sensor_read_i2c_cmd()
{   
    i2c_status_type     result;
    
    i2c_rw_cmd.buf_ptr = i2c_data_buf;
    
    i2c_rw_cmd.len = REG_READ_SIZE;
    i2c_rw_cmd.last_index = 0;    
    
    result = i2c_read(&i2c_rw_cmd);    
    
    return result;    
}

//This is a shadow write functions
LOCAL i2c_status_type g_sensor_write_i2c_cmd(word mask, word data)
{   
    data &= mask; 
    
    i2c_data_buf[CONTROL_REG] = (i2c_data_buf[CONTROL_REG] & (~(mask))) | data;    
    i2c_rw_cmd.buf_ptr = &i2c_data_buf[CONTROL_REG];    
    i2c_rw_cmd.len = 1;
    i2c_rw_cmd.last_index = 0;    
    
    return i2c_write(&i2c_rw_cmd);
}

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
void g_sensor_get_device_info(g_sensor_device_info_type *pDeviceInfo)
{
    pDeviceInfo->dwStaticValue = GS_MXC6202_STATIC_BASE;
    pDeviceInfo->dwOffestPerG = GS_MXC6202_OFFSET_PER_G;    
    
    return;    
}

/*==============================================================================
函数: 
     g_sensor_device_init
             
说明: 
     初始化G_sensor的物理设备
       
参数: 
              
返回值:
        G_SENSOR_FAILED：0
        G_SENSOR_SUCCESS：1
       
备注:
       
==============================================================================*/
int g_sensor_device_init()
{
    int  iResult = G_SENSOR_FAILED;
    
    g_sensor_init_i2c_cmd_data();
    
    iResult = g_sensor_power_down();
    //You can do some other initialize work here
    
    return iResult;
}

/*==============================================================================
函数: 
     g_sensor_power_up
             
说明: 
     打开G_SENSOR的物理设备
       
参数: 

              
返回值:
        G_SENSOR_FAILED：0
        G_SENSOR_SUCCESS：1      
备注:

==============================================================================*/
int g_sensor_power_up()
{   
    int  iResult = G_SENSOR_FAILED;
    
    iResult = g_sensor_write_i2c_cmd(G_SENSOR_PD_MASK, G_SENSOR_PW_ON);
    
    if(G_SENSOR_SUCCESSFUL == iResult)
    {
        g_sensor_work_status = G_SENSOR_POWER_UP;    
    }
    
    return iResult;
}

/*==============================================================================
函数: 
      g_sensor_power_down
             
说明: 
     关掉G_SENSOR的物理设备
       
参数:         
              
返回值:
        G_SENSOR_FAILED：0
        G_SENSOR_SUCCESS：1
       
备注:  

==============================================================================*/
int g_sensor_power_down()
{
    int  iResult = G_SENSOR_FAILED;
    
    iResult = g_sensor_write_i2c_cmd(G_SENSOR_PD_MASK, G_SENSOR_PW_OFF);
    
    if(G_SENSOR_SUCCESSFUL == iResult)
    {
        g_sensor_work_status = G_SENSOR_POWER_DOWN;    
    }
    
    return iResult;
}

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
int g_sensor_get_acceleration(int16 *x_pointer, int16 *y_pointer)
{
   int16 x_acc = GS_MXC6202_STATIC_BASE, y_acc = GS_MXC6202_STATIC_BASE;
   int iResult = G_SENSOR_FAILED;

   if(G_SENSOR_POWER_UP == g_sensor_work_status)  
    {
        iResult = g_sensor_read_i2c_cmd();
                
        if(G_SENSOR_SUCCESSFUL == iResult )
        {
            x_acc = ((int16)i2c_data_buf[X_AXIS_MSB_REG]) << 8; 
            x_acc += (int16)i2c_data_buf[X_AXIS_LSB_REG];
            y_acc = ((int16)i2c_data_buf[Y_AXIS_MSB_REG]) << 8;
            y_acc += (int16)i2c_data_buf[Y_AXIS_LSB_REG];
        }
        else
        {
#ifdef g_sensor_log_enable            
            MSG_ERROR("gsensor read fail ", 0, 0, 0);
#endif
        }
    }

   *x_pointer = x_acc;
   *y_pointer = y_acc;
#ifdef g_sensor_log_enable       
   MSG_ERROR("gsensor x_acc is 0x%X", x_acc, 0, 0);
   MSG_ERROR("gsensor y_acc is 0x%X", y_acc, 0, 0);
#endif
    return iResult; 
}