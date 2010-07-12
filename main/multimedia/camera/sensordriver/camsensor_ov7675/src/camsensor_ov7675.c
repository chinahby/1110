#include "camsensor.h"

#if defined (FEATURE_CAMERA) && defined (USE_CAMSENSOR_OV7675)
#include "camsensor_ov7675.h"
#include "camerai.h"
#include "ipl.h"
#include "msg.h"
#include "i2c.h"
#include "clk.h"
#include "chromatix_6550.h"

/*============================================================================
	TYPE DEFINITIONS 
============================================================================*/

// sensor's chip ID and version
#define OV7675_SENSOR_ID       				(0x76)
#define OV7675_SENSOR_VERSION  				(0x73)

#define OV7675_I2C_WRITE_ID   	            (0x42)
#define OV7675_I2C_READ_ID		            (0x43)

/* SENSOR REGISTER DEFINE */
#define OV7675_ID_REG           	        (0x0A)
#define OV7675_INFO_REG                		(0x0B)

#define CAMSENSOR_OV7675_FULL_SIZE_WIDTH    640//640
#define CAMSENSOR_OV7675_FULL_SIZE_HEIGHT   480//480

#define CAMSENSOR_OV7675_QTR_SIZE_WIDTH     160
#define CAMSENSOR_OV7675_QTR_SIZE_HEIGHT    128//512

#define MAX_EV_COMP                         25

/* FPS supported by this sensor */
#define CAMSENSOR_30_FPS                    (30*Q8)
#define CAMSENSOR_15_FPS                    (15*Q8)

#define VGA_EXPOSURE_LIMITATION 			510 //509//508

/*============================================================================
	LOCAL Variables 
============================================================================*/
static camera_effect_type   camsensor_ov7675_effect             = CAMERA_EFFECT_OFF;
static char                 camsensor_ov7675_sensor_name[32]    = "OV7675 0.3MP RGB565\0\0\0\0\0\0\0";
static const char           camsensor_ov7675_sensor_suffix[]    = "OV7675";
static boolean              sensor_cap_state 	                = FALSE; //Preview or Capture
static CAMIF_InputFormatType format_preview,format_snapshot;

/*============================================================================
	Function Declares
============================================================================*/
static void    camsensor_ov7675_register(camsensor_function_table_type *camsensor_function_table_ptr);
static void    camsensor_ov7675_setup_camctrl_tbl(camctrl_tbl_type *camctrl_tbl_ptr);
static boolean initialize_ov7675_registers(void);
static uint16  camsensor_ov7675_get_snapshot_fps(uint16);
static boolean ov7675_i2c_write_byte(uint8 offset, uint8 data);
static boolean ov7675_i2c_read_byte(uint8 offset, uint8* data);

/*============================================================================
	Function Body
============================================================================*/
static void write_ov7675_shutter(uint16 shutter)
{
    uint8 temp_reg;
    uint16 extra_exposure_lines;
    
	if(shutter<=VGA_EXPOSURE_LIMITATION)
	{
		extra_exposure_lines=0;
	}
	else
	{
		extra_exposure_lines=shutter-VGA_EXPOSURE_LIMITATION;
	}
    
	if(shutter>VGA_EXPOSURE_LIMITATION)
		shutter=VGA_EXPOSURE_LIMITATION;
		
	ov7675_i2c_write_byte(0x2D,extra_exposure_lines&0xFF);                  // ADVFL(LSB of extra exposure lines)
	ov7675_i2c_write_byte(0x2E,(extra_exposure_lines&0xFF00)>>8);           // ADVFH(MSB of extra exposure lines)
    
	ov7675_i2c_read_byte(0x04, &temp_reg);
	ov7675_i2c_write_byte(0x04,((temp_reg&0xFC) | (shutter&0x3)));		// AEC[b1~b0]
	ov7675_i2c_write_byte(0x10,((shutter&0x3FC)>>2));						// AEC[b9~b2]
	ov7675_i2c_write_byte(0x07,((shutter&0x7C00)>>10));						// AEC[b10]/AEC[b15~b10]
}/* write_ov7675_shutter */

static uint16 read_ov7675_shutter(void)
{
	uint8 temp_reg1, temp_reg2, temp_reg3, temp_reg4, temp_reg5;
	uint16 shutter, extra_exposure_lines;
    
	ov7675_i2c_read_byte(0x04, &temp_reg1);										// AEC[b1~b0]
	ov7675_i2c_read_byte(0x10, &temp_reg2);										// AEC[b9~b2]
	ov7675_i2c_read_byte(0x07, &temp_reg3);										// AEC[b15~b10]
	
	ov7675_i2c_read_byte(0x2D, &temp_reg4);
	ov7675_i2c_read_byte(0x2E, &temp_reg5);
	
	shutter=((temp_reg3&0x1F)<<10)|(temp_reg2<<2)|(temp_reg1&0x3);	// AEC[b10]/AEC[b15~b10]
	extra_exposure_lines=(temp_reg5<<8)|(temp_reg4);
	
	shutter=shutter+extra_exposure_lines;

	return shutter;
}/* read_ov7675_shutter */

static void ov7675_set_dummy(uint16 pixels, uint16 lines)
{
	ov7675_i2c_write_byte(0x2A,((pixels&0x700)>>4));
	ov7675_i2c_write_byte(0x2B,(pixels&0xFF));
	ov7675_i2c_write_byte(0x92,(lines&0xFF));
	ov7675_i2c_write_byte(0x93,((lines&0xFF00)>>8));
}	/* ov7675_set_dummy */

static void ov7675_set_window(uint16 startx,uint16 starty,uint16 width, uint16 height)
{
	uint16 endx=(startx+width-1);
	uint16 endy=(starty+height-1);
	uint8  temp_reg1, temp_reg2;
    
    ov7675_i2c_read_byte(0x03, &temp_reg1);
	temp_reg1=(temp_reg1&0xF0);
    ov7675_i2c_read_byte(0x32, &temp_reg2);
	temp_reg2=(temp_reg2&0xC0);
    
	// Horizontal
	ov7675_i2c_write_byte(0x32,0x80|((endx&0x7)<<3)|(startx&0x7));	// b[5:3]:HREF end low 3bits. b[2:0]:HREF start low 3bits.
	ov7675_i2c_write_byte(0x17,(startx&0x7F8)>>3);			// HREF start high 8bits
	ov7675_i2c_write_byte(0x18,(endx&0x7F8)>>3);			// HREF end high 8bits
	// Vertical
	ov7675_i2c_write_byte(0x03,temp_reg1|((endy&0x3)<<2)|(starty&0x3));	// b[3:2]:VREF end low 2bits. b[1:0]:VREF start low 2bits.
	ov7675_i2c_write_byte(0x19,(starty&0x3FC)>>2);   			// VREF start high 8bits
	ov7675_i2c_write_byte(0x1A,(endy&0x3FC)>>2);		   	// VREF end high 8bits
}	/* config_OV7675_window */

static camera_ret_code_type camsensor_ov7675_set_exposure_mode(int8 mode)
{
	return CAMERA_SUCCESS;
}

static camera_ret_code_type camsensor_ov7675_set_frame_rate(uint16 fps)
{
	if (fps > CAMSENSOR_30_FPS)
	{
		return CAMERA_INVALID_PARM;
	} 

	return CAMERA_SUCCESS;
} /* camsensor_ov7675_set_frame_rate() */

boolean camsensor_ov7675_init(camsensor_function_table_type *camsensor_function_table_ptr,
				              camctrl_tbl_type              *camctrl_tbl_ptr)
{
	uint8	sensor_id;
	
	ERR("camsensor_ov7675_init!",0,0,0);
    
	/* Input MCLK = 24MHz */
	camsensor_camclk_po_hz = 24000000;
    camsensor_camclk_po_hz = camsensor_config_camclk_po(camsensor_camclk_po_hz);
    
	/* Preview must aways be se to quater size */
   	camsensor_preview_resolution  = CAMSENSOR_QTR_SIZE;
   	camsensor_snapshot_resolution = CAMSENSOR_FULL_SIZE;
  	
	/* Configure I2C parameters */
	camsensor_i2c_command.bus_id     = I2C_BUS_HW_CTRL;
	camsensor_i2c_command.slave_addr = OV7675_I2C_WRITE_ID;
    
	/*lint -save -e655 */
	camsensor_i2c_command.options    = (i2c_options_type) (I2C_REG_DEV | I2C_START_BEFORE_READ); 
    
    // Reset Sensor
	camera_timed_wait(10);	//ovt
	ov7675_i2c_write_byte(0x12,0x80);
	camera_timed_wait(50);	//02
	
    if( !ov7675_i2c_read_byte(OV7675_ID_REG,&sensor_id)) 
    {
    	ERR("read sensor_id failed!",0,0,0);
        return FALSE;
    }
    
    ERR("sensor_id 1 = %x",sensor_id,0,0);
    
	/* Check if it matches it with the value in Datasheet */
	if ( sensor_id != OV7675_SENSOR_ID)
	{
		return FALSE;
	}
    
    ov7675_i2c_read_byte(OV7675_INFO_REG,&sensor_id);
    ERR("sensor_id 2 = %x",sensor_id,0,0);
    
	/* Initialize Sensor registers */
	if (!initialize_ov7675_registers())
	{
		return FALSE;
	}

	/* Register function table: */
	camsensor_ov7675_register(camsensor_function_table_ptr);

	/* Setup camctrl_tbl */
	camsensor_ov7675_setup_camctrl_tbl(camctrl_tbl_ptr);
	return TRUE;
} /* camsensor_ov7675_init */

static boolean initialize_ov7675_registers(void)
{
    // Initail Sequence Write In.
    // Set Format
    ov7675_i2c_write_byte(0x11,0x00);//时钟分频 = MCLK
    ov7675_i2c_write_byte(0x3a,0x0C);//YUV输出顺序
    ov7675_i2c_write_byte(0x3D,0xC0);//GAMMA允许，UV饱和度自动调整
    ov7675_i2c_write_byte(0x12,0x00);//输出格式YUV
    ov7675_i2c_write_byte(0x15,0x40);//设置HREF为HYNC
    ov7675_i2c_write_byte(0xc1,0x7f);//测试模式?
    
    // Set Window
    ov7675_i2c_write_byte(0x17,0x13);//水平窗口起始点高8位
    ov7675_i2c_write_byte(0x18,0x01);//水平窗口大小高8位
    ov7675_i2c_write_byte(0x32,0xbF);//水平输出指示
    ov7675_i2c_write_byte(0x19,0x02);//垂直窗口起始点高8位
    ov7675_i2c_write_byte(0x1a,0x7b);//垂直窗大小高8位
    ov7675_i2c_write_byte(0x03,0x0a);//垂直输出指示
    
    ov7675_i2c_write_byte(0x0c,0x00);//通用控制寄存器3 待机时钟数据输出三态设置
    ov7675_i2c_write_byte(0x3e,0x00);//通用控制寄存器14 PCLK分频和控制
    ov7675_i2c_write_byte(0x70,0x3a);//SCALING_XSC
    ov7675_i2c_write_byte(0x71,0x35);//SCALING_YSC
    ov7675_i2c_write_byte(0x72,0x11);//测试模式
    ov7675_i2c_write_byte(0x73,0xf0);//测试模式
    ov7675_i2c_write_byte(0xa2,0x02);//测试模式

    ov7675_i2c_write_byte(0x13,0xe0);//通用控制寄存器8 自动增益等功能使能
    ov7675_i2c_write_byte(0x00,0x00);//增益控制
    ov7675_i2c_write_byte(0x10,0x00);//自动曝光
    ov7675_i2c_write_byte(0x0d,0x40);//通用控制寄存器 4 窗口比例设置
    ov7675_i2c_write_byte(0x14,0x28);//通用控制寄存器9 自动增益最大值
    ov7675_i2c_write_byte(0xa5,0x06);//50Hz 曝光步数限制
    ov7675_i2c_write_byte(0xab,0x07);//60Hz 曝光步数限制
    ov7675_i2c_write_byte(0x24,0x62);//自动曝光/自动增益稳定范围
    ov7675_i2c_write_byte(0x25,0x58);//自动曝光/自动增益稳定范围（下限）
    ov7675_i2c_write_byte(0x26,0x93);//自动曝光/自动增益快速调节范围
    ov7675_i2c_write_byte(0x9f,0x78);//测试模式
    ov7675_i2c_write_byte(0xa0,0x68);//测试模式
    ov7675_i2c_write_byte(0xa1,0x03);//测试模式
    ov7675_i2c_write_byte(0xa6,0xD8);//测试模式
    ov7675_i2c_write_byte(0xa7,0xD8);//测试模式
    ov7675_i2c_write_byte(0xa8,0xf0);//测试模式
    ov7675_i2c_write_byte(0xa9,0x90);//测试模式
    ov7675_i2c_write_byte(0xaa,0x14);//测试模式
    ov7675_i2c_write_byte(0x13,0xe5);//通用控制寄存器8 自动增益等功能使能
    
    ov7675_i2c_write_byte(0x0e,0x61);//测试模式
    ov7675_i2c_write_byte(0x0f,0x4b);//通用控制寄存器6 时序设置
    ov7675_i2c_write_byte(0x16,0x02);//测试模式
    ov7675_i2c_write_byte(0x1e,0x07);//镜像/翻转
    ov7675_i2c_write_byte(0x21,0x02);//测试模式
    ov7675_i2c_write_byte(0x22,0x91);//测试模式
    ov7675_i2c_write_byte(0x29,0x07);//测试模式
    ov7675_i2c_write_byte(0x33,0x0b);//测试模式
    ov7675_i2c_write_byte(0x35,0x0b);//测试模式
    ov7675_i2c_write_byte(0x37,0x1d);//测试模式
    ov7675_i2c_write_byte(0x38,0x71);//测试模式
    ov7675_i2c_write_byte(0x39,0x2a);//测试模式
    ov7675_i2c_write_byte(0x3c,0x78);//通用控制寄存器12 行同步设置
    ov7675_i2c_write_byte(0x4d,0x40);//测试模式
    ov7675_i2c_write_byte(0x4e,0x20);//测试模式
    ov7675_i2c_write_byte(0x69,0x00);//固定增益控制

    ov7675_i2c_write_byte(0x6b,0x0a);//PLL控制
    ov7675_i2c_write_byte(0x74,0x10);//数字增益控制
    ov7675_i2c_write_byte(0x8d,0x4f);//测试模式
    ov7675_i2c_write_byte(0x8e,0x00);//测试模式
    ov7675_i2c_write_byte(0x8f,0x00);//测试模式
    ov7675_i2c_write_byte(0x90,0x00);//测试模式
    ov7675_i2c_write_byte(0x91,0x00);//测试模式
    ov7675_i2c_write_byte(0x96,0x00);//测试模式
    ov7675_i2c_write_byte(0x9a,0x80);//测试模式
    ov7675_i2c_write_byte(0xb0,0x84);//测试模式
    ov7675_i2c_write_byte(0xb1,0x0c);//测试模式
    ov7675_i2c_write_byte(0xb2,0x0e);//测试模式
    ov7675_i2c_write_byte(0xb3,0x82);//测试模式
    ov7675_i2c_write_byte(0xb8,0x0a);//测试模式

    ov7675_i2c_write_byte(0xbb, 0xa1); //blc target 
    
    //lenc
    ov7675_i2c_write_byte(0x62,0x00);//0x10
    ov7675_i2c_write_byte(0x63,0x00);//0x0b
    ov7675_i2c_write_byte(0x64,0x0f);
    ov7675_i2c_write_byte(0x65,0x07);
    ov7675_i2c_write_byte(0x66,0x05);
    ov7675_i2c_write_byte(0x94,0x0e);
    ov7675_i2c_write_byte(0x95,0x12); //0x10  

    //gamma
    ov7675_i2c_write_byte(0x7a, 0x28); 
    ov7675_i2c_write_byte(0x7b, 0x04); 
    ov7675_i2c_write_byte(0x7c, 0x09); 
    ov7675_i2c_write_byte(0x7d, 0x16); 
    ov7675_i2c_write_byte(0x7e, 0x30); 
    ov7675_i2c_write_byte(0x7f, 0x3E); 
    ov7675_i2c_write_byte(0x80, 0x4B); 
    ov7675_i2c_write_byte(0x81, 0x59); 
    ov7675_i2c_write_byte(0x82, 0x67); 
    ov7675_i2c_write_byte(0x83, 0x72); 
    ov7675_i2c_write_byte(0x84, 0x7c); 
    ov7675_i2c_write_byte(0x85, 0x8e); 
    ov7675_i2c_write_byte(0x86, 0x9e); 
    ov7675_i2c_write_byte(0x87, 0xB6); 
    ov7675_i2c_write_byte(0x88, 0xcc); 
    ov7675_i2c_write_byte(0x89, 0xE2); 

    ov7675_i2c_write_byte(0x7a, 0x24); 
    ov7675_i2c_write_byte(0x7b, 0x04); 
    ov7675_i2c_write_byte(0x7c, 0x07); 
    ov7675_i2c_write_byte(0x7d, 0x12); 
    ov7675_i2c_write_byte(0x7e, 0x2f); 
    ov7675_i2c_write_byte(0x7f, 0x3f); 
    ov7675_i2c_write_byte(0x80, 0x4d); 
    ov7675_i2c_write_byte(0x81, 0x5a); 
    ov7675_i2c_write_byte(0x82, 0x69); 
    ov7675_i2c_write_byte(0x83, 0x74); 
    ov7675_i2c_write_byte(0x84, 0x7f); 
    ov7675_i2c_write_byte(0x85, 0x91); 
    ov7675_i2c_write_byte(0x86, 0x9e); 
    ov7675_i2c_write_byte(0x87, 0xBb); 
    ov7675_i2c_write_byte(0x88, 0xd2); 
    ov7675_i2c_write_byte(0x89, 0xE5); 
    
    //awb from HQ
    ov7675_i2c_write_byte(0x43, 0x0a);
    ov7675_i2c_write_byte(0x44, 0xf2);
    ov7675_i2c_write_byte(0x45, 0x39);
    ov7675_i2c_write_byte(0x46, 0x62);
    ov7675_i2c_write_byte(0x47, 0x3d);
    ov7675_i2c_write_byte(0x48, 0x55);
    ov7675_i2c_write_byte(0x59, 0x83);
    ov7675_i2c_write_byte(0x5a, 0x0d);
    ov7675_i2c_write_byte(0x5b, 0xcd);
    ov7675_i2c_write_byte(0x5c, 0x8c);
    ov7675_i2c_write_byte(0x5d, 0x77);
    ov7675_i2c_write_byte(0x5e, 0x16);
    ov7675_i2c_write_byte(0x6a, 0x40);
    ov7675_i2c_write_byte(0x6c, 0x0a);
    ov7675_i2c_write_byte(0x6d, 0x65);
    ov7675_i2c_write_byte(0x6e, 0x11);
    ov7675_i2c_write_byte(0x6f, 0x9f);//0x9e

    ov7675_i2c_write_byte(0x6a,0x40);
    ov7675_i2c_write_byte(0x01,0x56);
    ov7675_i2c_write_byte(0x02,0x44);
    ov7675_i2c_write_byte(0x13,0xe7);    
  
    //cmx
    ov7675_i2c_write_byte(0x4f, 0x96);
    ov7675_i2c_write_byte(0x50, 0x9b);
    ov7675_i2c_write_byte(0x51, 0x05);
    ov7675_i2c_write_byte(0x52, 0x1a);
    ov7675_i2c_write_byte(0x53, 0x7f);
    ov7675_i2c_write_byte(0x54, 0x99);
    ov7675_i2c_write_byte(0x58, 0x1a);
    
    ov7675_i2c_write_byte(0x3f,0x02);
    ov7675_i2c_write_byte(0x75,0x64);
    ov7675_i2c_write_byte(0x76,0xe1);
    ov7675_i2c_write_byte(0x4c,0x00);
    ov7675_i2c_write_byte(0x77,0x01);
    ov7675_i2c_write_byte(0x3D,0xC2);
    ov7675_i2c_write_byte(0x4b,0x09);
    ov7675_i2c_write_byte(0xc9,0x60);
    ov7675_i2c_write_byte(0x41,0x38);    
    ov7675_i2c_write_byte(0x56,0x40);

    ov7675_i2c_write_byte(0x34,0x11);
    ov7675_i2c_write_byte(0x3b,0x0a);   
    ov7675_i2c_write_byte(0xa4,0x88);
    ov7675_i2c_write_byte(0x96,0x00);
    ov7675_i2c_write_byte(0x97,0x30);
    ov7675_i2c_write_byte(0x98,0x20);
    ov7675_i2c_write_byte(0x99,0x30);
    ov7675_i2c_write_byte(0x9a,0x84);
    ov7675_i2c_write_byte(0x9b,0x29);
    ov7675_i2c_write_byte(0x9c,0x03);
    ov7675_i2c_write_byte(0x9d,0x4c);
    ov7675_i2c_write_byte(0x9e,0x3f);
    ov7675_i2c_write_byte(0x78,0x04);
    
    ov7675_i2c_write_byte(0x79,0x01);
    ov7675_i2c_write_byte(0xc8,0xf0);
    ov7675_i2c_write_byte(0x79,0x0f);
    ov7675_i2c_write_byte(0xc8,0x00);
    ov7675_i2c_write_byte(0x79,0x10);
    ov7675_i2c_write_byte(0xc8,0x7e);
    ov7675_i2c_write_byte(0x79,0x0a);
    ov7675_i2c_write_byte(0xc8,0x80);
    ov7675_i2c_write_byte(0x79,0x0b);
    ov7675_i2c_write_byte(0xc8,0x01);
    ov7675_i2c_write_byte(0x79,0x0c);
    ov7675_i2c_write_byte(0xc8,0x0f);
    ov7675_i2c_write_byte(0x79,0x0d);
    ov7675_i2c_write_byte(0xc8,0x20);
    ov7675_i2c_write_byte(0x79,0x09);
    ov7675_i2c_write_byte(0xc8,0x80);
    ov7675_i2c_write_byte(0x79,0x02);
    ov7675_i2c_write_byte(0xc8,0xc0);
    ov7675_i2c_write_byte(0x79,0x03);
    ov7675_i2c_write_byte(0xc8,0x40);
    ov7675_i2c_write_byte(0x79,0x05);
    ov7675_i2c_write_byte(0xc8,0x30);
    ov7675_i2c_write_byte(0x79,0x26);
	return TRUE;
} /* end of initialize_ov7675_registers. */

static boolean camsensor_ov7675_start( camsensor_static_params_type *camsensor_params)
{
	/* Initialize CAMIF operation mode */
	camsensor_params->camif_config.SyncMode    = CAMIF_APS;
	camsensor_params->camif_config.HSyncEdge   = CAMIF_High;
	camsensor_params->camif_config.VSyncEdge   = CAMIF_High;
	camsensor_params->camif_efs_config.EFS_EOL = 0x0000;
	camsensor_params->camif_efs_config.EFS_SOL = 0x0000;
	camsensor_params->camif_efs_config.EFS_EOF = 0x0000;
	camsensor_params->camif_efs_config.EFS_SOF = 0x0000;


	/* ------------------  Sensor-specific Config -------------- */
	/* Make/model of sensor */
	camsensor_params->sensor_model  = CAMSENSOR_OV7675_ID;

	/* CCD or CMOS */
	camsensor_params->sensor_type   = CAMSENSOR_CMOS;

	/* BAYER or YCbCr */
	camsensor_params->output_format = CAMSENSOR_YCBCR;

	/* A pointer to the sensor name for EXIF tags */
	camsensor_params->sensor_name = camsensor_ov7675_sensor_name;

	/* Pointer to sensor suffix */

	camsensor_params->sensor_suffix=camsensor_ov7675_sensor_suffix;

	/* What is the maximum FPS that can be supported by this sensor in video mode? */
	camsensor_params->max_video_fps   = 15 * Q8;

	/* Application assigned FPS in video mode */
	camsensor_params->video_fps       = 15 * Q8;

	/* Snapshot mode operation */
	camsensor_params->max_preview_fps = CAMSENSOR_15_FPS;


	/* May be assigned with max_preview_fps or nightshot_fps. */
	camsensor_params->preview_fps     = camsensor_params->max_preview_fps;

	/* AEC is not required for YUV sensor from VFE side */
	camsensor_params->aec_enable=FALSE;
	/* AWB is not required for YUV sensor from VFE side */
	camsensor_params->awb_enable=FALSE;
	/* AF is not required for YUV sensor from VFE side */
	camsensor_params->af_enable=FALSE;
    
	camsensor_params->num_possible_frame_rates = 1;
	/* Define these frame rates */
	/* By convention, the highest frame rate will be first in the
	array (zeroth index) and the lowest last (in order). */
	camsensor_params->frame_rate_array[0].fps = (uint16) (15*256.0); /* Q8 */
	camsensor_params->frame_rate_array[0].use_in_auto_frame_rate = TRUE;
    
	camsensor_params->frame_rate_array[1].fps = (uint16) (30.0*256.0); /* Q8 */
	camsensor_params->frame_rate_array[1].use_in_auto_frame_rate = FALSE;

	/* Sensor output capability */

	/* Full size dimensions - 1280x1024 */

	camsensor_params->full_size_width  =  CAMSENSOR_OV7675_FULL_SIZE_WIDTH;
	camsensor_params->full_size_height =  CAMSENSOR_OV7675_FULL_SIZE_HEIGHT;


	/* Quarter size dimensions - 640x512 */

	camsensor_params->qtr_size_width  =  CAMSENSOR_OV7675_QTR_SIZE_WIDTH;
	camsensor_params->qtr_size_height =  CAMSENSOR_OV7675_QTR_SIZE_HEIGHT;


	switch(camsensor_preview_resolution)
	{
		case CAMSENSOR_QTR_SIZE:

			camsensor_params->preview_dx_decimation = \
			            camsensor_params->full_size_width * Q12 / \
			          camsensor_params->qtr_size_width;
			camsensor_params->preview_dy_decimation = \
			          camsensor_params->full_size_height * Q12 / \
			          camsensor_params->qtr_size_height;

			camsensor_params->camsensor_width  = \
			          camsensor_params->qtr_size_width;
			camsensor_params->camsensor_height = \
			          camsensor_params->qtr_size_height;
			camsensor_params->format = CAMIF_YCbCr_Y_Cb_Y_Cr;

			break;

		case CAMSENSOR_FULL_SIZE:

			camsensor_params->preview_dx_decimation = Q12;
			camsensor_params->preview_dy_decimation = Q12;
			camsensor_params->camsensor_width  = \
			          camsensor_params->full_size_width;
			camsensor_params->camsensor_height = \
			          camsensor_params->full_size_height;
			camsensor_params->format = CAMIF_YCbCr_Y_Cb_Y_Cr;

			break;

		default:

			MSG_ERROR("Unrecognized preview resolution, assuming full size",0,0,0);

			camsensor_params->camsensor_width  = \
			      camsensor_params->full_size_width;
			camsensor_params->camsensor_height = \
			      camsensor_params->full_size_height;

			break;
    }
	/* Pixel Clock Should be inverted for this Sensor */

	camsensor_params->pclk_invert = camsensor_info.pclk_invert;


#if defined FEATURE_WHITE_LED_FLASH || defined FEATURE_STROBE_FLASH
	camsensor_params->support_auto_flash = FALSE;
#else
	camsensor_params->support_auto_flash = FALSE;
#endif

	return TRUE;
} /*camsensor_ov7675_start*/

static boolean ov7675_i2c_write_byte(uint8 offset, uint8 data) 
{
	static uint8 writebyte;
	uint8 i;

	writebyte  = data;
    
	camsensor_i2c_command.addr.reg = offset;
	camsensor_i2c_command.buf_ptr  = (byte *)(&writebyte);
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

static boolean ov7675_i2c_read_byte(uint8 offset, uint8 *data) 
{
	static uint8 readbyte; 
	uint8  i;

	if (data == NULL)
	{
		return FALSE;
	}
    
	camsensor_i2c_command.addr.reg = offset;
	camsensor_i2c_command.buf_ptr  = (byte *)(&readbyte);
	camsensor_i2c_command.len      = 1;

	for (i =0; i < 3; ++i)
	{
		if (i2c_read(&camsensor_i2c_command) == I2C_SUCCESS)
		{
			*data  = readbyte;
			return TRUE;
		}
	}
	return FALSE;
} /* end of ov7675_i2c_read_word. */

static camera_ret_code_type camsensor_ov7675_set_effect(int8 effect)
{
    return CAMERA_NOT_SUPPORTED;
} /* camsensor_ov7675_set_effect() */

/*===========================================================================

FUNCTION      camsensor_ov7675_snapshot_config

DESCRIPTION
              Configure the camera sensor for Snapshot 

DEPENDENCIES
  None

RETURN VALUE
  TRUE if successful
  FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/

static boolean camsensor_ov7675_snapshot_config( camsensor_static_params_type  *camsensor_params)
{
	volatile uint32 shutter=0;
    sensor_cap_state= TRUE;	

	ERR("camsensor_ov7675_snapshot_config!",0,0,0);
	camsensor_params->format = format_snapshot;
	camsensor_params->camsensor_width  = \
	        camsensor_params->full_size_width;
	camsensor_params->camsensor_height = \
	        camsensor_params->full_size_height;

	/* CAMIF frame */
	camsensor_params->camif_frame_config.pixelsPerLine = \
	        camsensor_params->full_size_width*2;
	camsensor_params->camif_frame_config.linesPerFrame = \
	        camsensor_params->full_size_width;

	/* CAMIF window */
	camsensor_params->camif_window_width_config.firstPixel = 0;
	camsensor_params->camif_window_width_config.lastPixel  = \
	        (camsensor_params->full_size_width*2)-1;

	camsensor_params->camif_window_height_config.firstLine = 0;
	camsensor_params->camif_window_height_config.lastLine  = \
	        camsensor_params->full_size_height-1;

	/********************************
	*   Normal camera capture mode  *
	********************************/
	shutter=read_ov7675_shutter();  
	write_ov7675_shutter(shutter);
		 

	/* Wait for one frame before reconfiguring the VFE since either zero or one
	* frames will be output after commencing the snapshot program, depending on
	* when the I2C transaction took place relative to the active frame. */
	camera_timed_wait(150);

	camsensor_current_resolution = camsensor_snapshot_resolution;
	
	return TRUE;
}
/*===========================================================================

FUNCTION      camsensor_ov7675_raw_snapshot_config

DESCRIPTION
              Configure the camera sensor for RAW Snapshot

DEPENDENCIES
  None

RETURN VALUE
  TRUE if successful
  FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
static boolean camsensor_ov7675_raw_snapshot_config(camsensor_static_params_type  *camsensor_params)
{
	ERR("camsensor_ov7675_raw_snapshot_config!",0,0,0);
	return TRUE;
}

/*===========================================================================

FUNCTION      camsensor_ov7675_video_config

DESCRIPTION
              Configure the camera sensor for Preview

DEPENDENCIES
  None

RETURN VALUE
  TRUE if successful
  FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
static boolean camsensor_ov7675_video_config(camsensor_static_params_type *camsensor_params)
{
	sensor_cap_state = FALSE;
  	camsensor_params->format = format_preview;
 	camsensor_params->discardFirstFrame =  TRUE;
	
	ERR("camsensor_ov7675_video_config!",0,0,0);
  	switch ( camsensor_preview_resolution )
    {
    	case CAMSENSOR_QTR_SIZE:

			/* Set the current dimensions */
			camsensor_params->camsensor_width  = \
			            camsensor_params->qtr_size_width;
			camsensor_params->camsensor_height = \
			            camsensor_params->qtr_size_height;

			/* CAMIF frame */
			camsensor_params->camif_frame_config.pixelsPerLine = \
			            camsensor_params->qtr_size_width*2;

			camsensor_params->camif_frame_config.linesPerFrame = \
			            camsensor_params->qtr_size_height;

			/* CAMIF Window */
			camsensor_params->camif_window_width_config.firstPixel = \
			                 camsensor_info.preview_dummy_pixels*2;

			camsensor_params->camif_window_width_config.lastPixel  = \
			          (camsensor_info.preview_dummy_pixels) \
			      +(camsensor_params->qtr_size_width*2)-1;

			camsensor_params->camif_window_height_config.firstLine = 0;

			camsensor_params->camif_window_height_config.lastLine  = \
			            camsensor_params->qtr_size_height-1;
      		break;


		case CAMSENSOR_FULL_SIZE:
			/* Set the current dimensions */
			camsensor_params->camsensor_width  = \
			            camsensor_params->full_size_width;
			camsensor_params->camsensor_height = \
			            camsensor_params->full_size_height;

			/* CAMIF frame */
			camsensor_params->camif_frame_config.pixelsPerLine = \
			            camsensor_params->full_size_width*2;

			camsensor_params->camif_frame_config.linesPerFrame = \
			            camsensor_params->full_size_width;

			/* CAMIF window */
			camsensor_params->camif_window_width_config.firstPixel = 0;

			camsensor_params->camif_window_width_config.lastPixel  = \
			           (camsensor_params->full_size_width*2)-1;

			camsensor_params->camif_window_height_config.firstLine = 0;

			camsensor_params->camif_window_height_config.lastLine  = \
			           camsensor_params->full_size_height-1;

			break;

    	case CAMSENSOR_INVALID_SIZE:

       		return FALSE;

  	}

	return TRUE;
}


static void camsensor_ov7675_power_up(void)
{
	ERR("camsensor_ov7675_power_up!",0,0,0);
} /* camsensor_ov7675_power_up */

/*===========================================================================

FUNCTION      camsensor_ov7675_power_down

DESCRIPTION
Power down the sensor, using I2C driver.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void camsensor_ov7675_power_down(void)
{
	ERR("camsensor_ov7675_power_down!",0,0,0);
} /* camsensor_ov7675_power_down */

static camera_ret_code_type camsensor_ov7675_set_wb(int8 wb)
{ 
	uint16 rgain=0x80, ggain=0x80, bgain=0x80;
	uint8  temp_reg;
	
	ov7675_i2c_read_byte(0x13,&temp_reg);
		
	ERR("camsensor_ov7675_set_wb!",0,0,0);
	switch((camera_wb_type)wb)
	{
		case CAMERA_WB_AUTO:
		{
			ov7675_i2c_write_byte(0x13,temp_reg|0x2);   // Enable AWB
			break;
		}
        
		case CAMERA_WB_INCANDESCENT:
		{
			ov7675_i2c_write_byte(0x6a,0x40);
			ov7675_i2c_write_byte(0x13,temp_reg&~0x2);  // Disable AWB				
			ov7675_i2c_write_byte(0x01,0x8c);
			ov7675_i2c_write_byte(0x02,0x59);
			break;
		}
        
		case CAMERA_WB_FLUORESCENT:
		{
			ov7675_i2c_write_byte(0x6a,0x40);
			ov7675_i2c_write_byte(0x13,temp_reg&~0x2);  // Disable AWB				
			ov7675_i2c_write_byte(0x01,0x7e);
			ov7675_i2c_write_byte(0x02,0x49);
			break;
		}
        
		case CAMERA_WB_DAYLIGHT:
		{
			ov7675_i2c_write_byte(0x6a,0x40);
			ov7675_i2c_write_byte(0x13,temp_reg&~0x2);  // Disable AWB				
			ov7675_i2c_write_byte(0x01,0x52);
			ov7675_i2c_write_byte(0x02,0x66);
			break;
		}
		
		case CAMERA_WB_CLOUDY_DAYLIGHT:
		{
			ov7675_i2c_write_byte(0x6a,0x40);
			ov7675_i2c_write_byte(0x13,temp_reg&~0x2);  // Disable AWB				
			ov7675_i2c_write_byte(0x01,0x52);
			ov7675_i2c_write_byte(0x02,0x6c);
			break;
		}
		
		case CAMERA_WB_TWILIGHT:
		case CAMERA_WB_SHADE :
		case CAMERA_WB_CUSTOM:
		default:
			return CAMERA_NOT_SUPPORTED;
	}//end of switch
	
	return CAMERA_SUCCESS;
}

static camera_ret_code_type camsensor_ov7675_set_ev_compensation(int32 compensation)
{
	camera_ret_code_type ret_val = CAMERA_SUCCESS;
	int16  parmCurNum = (int16)(compensation>>16);
	int32 max_parmVal = 12;
	int32 min_parmVal =-12;
	int32  parmVal= parmCurNum;
	uint32 new_luma_target;

	ERR("camsensor_ov7675_set_ev_compensation!",0,0,0);
	if ( parmVal > max_parmVal )
	{
		ret_val =  CAMERA_INVALID_PARM;
	}
	else if ( parmVal < min_parmVal )
	{
		ret_val =  CAMERA_INVALID_PARM;
	}

	new_luma_target = (uint32) parmVal + max_parmVal;

	if( new_luma_target >= MAX_EV_COMP)
	{
		ret_val=CAMERA_INVALID_PARM;
	}
	else
	{
		switch(new_luma_target)
		{
			case 8:    // -4 EV
        		ov7675_i2c_write_byte(0x55, 0xa8);
       			break;

		    case 9:    // -3 EV
		        ov7675_i2c_write_byte(0x55, 0xa0);
		        break;

		    case 10:    // -2 EV
		        ov7675_i2c_write_byte(0x55, 0x98);
		        break;

		    case 11:    // -1 EV
		        ov7675_i2c_write_byte(0x55, 0x90);
		        break;

		    case 12:   // +0 EV
		        ov7675_i2c_write_byte(0x55, 0x00);
		        break;

		    case 13:    // +1 EV
		        ov7675_i2c_write_byte(0x55, 0x10);
		        break;

		    case 14:    // +2 EV
		        ov7675_i2c_write_byte(0x55, 0x18);
		        break;

		    case 15:    // +3 EV
		        ov7675_i2c_write_byte(0x55, 0x20);
		        break;

		    case 16:    // +4 EV
		        ov7675_i2c_write_byte(0x55, 0x28);
		        break;

		    default:
		    	break;
	    }
	}
	
	return ret_val;
}

static camera_ret_code_type camsensor_ov7675_set_brightness(int8 brightness)
{
	ERR("camsensor_ov7675_set_brightness!",0,0,0);
	return CAMERA_SUCCESS;
}


static camera_ret_code_type camsensor_ov7675_set_contrast(int8 contrast)
{
	ERR("camsensor_ov7675_set_contrast!",0,0,0);
	return CAMERA_SUCCESS;
}/* camsensor_ov7675_set_contrast() */

static void camsensor_ov7675_setup_camctrl_tbl(camctrl_tbl_type *camctrl_tbl_ptr)
{
	ERR("camsensor_ov7675_get_snapshot_fps!",0,0,0);
	camctrl_tbl_ptr->asf_5x5_is_supported         = FALSE; 
	camctrl_tbl_ptr->get_snapshot_frame_per_sec = camsensor_ov7675_get_snapshot_fps;
}

static uint16 camsensor_ov7675_get_snapshot_fps(uint16 fps)
{
	ERR("camsensor_ov7675_get_snapshot_fps!",0,0,0);
	/* In the current driver, the snapshot fps is always 11 */
	fps = 15 * Q8;
	return fps;
}

camera_ret_code_type camsensor_ov7675_set_antibanding(int8 antibanding)
{
	ERR("camsensor_ov2655_set_antibanding!",0,0,0);
	return CAMERA_SUCCESS;
}

static void camsensor_ov7675_register(camsensor_function_table_type *camsensor_function_table_ptr)
{
	ERR("camsensor_ov7675_register!",0,0,0);
	camsensor_function_table_ptr->camsensor_start                 = camsensor_ov7675_start;
	camsensor_function_table_ptr->camsensor_video_config          = camsensor_ov7675_video_config;
	camsensor_function_table_ptr->camsensor_raw_snapshot_config   = camsensor_ov7675_raw_snapshot_config;
	camsensor_function_table_ptr->camsensor_snapshot_config       = camsensor_ov7675_snapshot_config;
	camsensor_function_table_ptr->camsensor_power_up              = camsensor_ov7675_power_up;
	camsensor_function_table_ptr->camsensor_power_down            = camsensor_ov7675_power_down;
	camsensor_function_table_ptr->camsensor_set_frame_rate        = camsensor_ov7675_set_frame_rate;  

	camsensor_function_table_ptr->camsensor_set_effect            = camsensor_ov7675_set_effect;

	camsensor_function_table_ptr->camsensor_set_brightness        = camsensor_ov7675_set_brightness;
	camsensor_function_table_ptr->camsensor_set_contrast          = camsensor_ov7675_set_contrast;
	camsensor_function_table_ptr->camsensor_set_antibanding       = camsensor_ov7675_set_antibanding;
	camsensor_function_table_ptr->camsensor_set_exposure_mode 	  = camsensor_ov7675_set_exposure_mode;
	camsensor_function_table_ptr->camsensor_set_ev_compensation	  = camsensor_ov7675_set_ev_compensation; 
	camsensor_function_table_ptr->camsensor_set_wb				  = camsensor_ov7675_set_wb;
} /* camsensor_mt9d112_mu2m0yu_register */
#endif

