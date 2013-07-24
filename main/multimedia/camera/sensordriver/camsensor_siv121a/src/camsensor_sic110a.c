#include "camsensor.h"

#if defined (FEATURE_CAMERA) && defined (USE_CAMSENSOR_SIC110A)
#include "camsensor_sic110a.h"
#include "camerai.h"
#include "ipl.h"
#include "msg.h"
#include "i2c.h"
#include "clk.h"
#include "chromatix_6550.h"
#include "disp_options.h"
#include "camera_svcs.h"

/*============================================================================
    TYPE DEFINITIONS 
============================================================================*/
#if defined(FEATURE_VERSION_C180) || defined(FEATURE_VERSION_C11)
#define T_QSC1110
#endif

#if defined(FEATURE_VERSION_W516) || defined(FEATURE_VERSION_W208S)
#ifdef T_QSC1110
#ifndef FEATURE_VERSION_K212
#define CAMSENSOR_SIC110A_RESET_PIN         GPIO_OUTPUT_10
#endif
#else
#define CAMSENSOR_SIC110A_RESET_PIN         GPIO_OUTPUT_53
#endif
#else
#ifdef T_QSC1110
#ifndef FEATURE_VERSION_K212
#define CAMSENSOR_SIC110A_RESET_PIN         GPIO_OUTPUT_10
#endif
#else
#define CAMSENSOR_SIC110A_RESET_PIN         GPIO_OUTPUT_62
#endif
#endif
#ifndef T_QSC1110
#define SIC110A_OUTFORMAT_RGB565
#endif
// sensor's chip ID and version
#define SIC110A_SENSOR_ID                   (0x0D)
#define SIC110A_SENSOR_VERSION              (0x02)

#define SIC110A_I2C_WRITE_ID                (0x62)
#define SIC110A_I2C_READ_ID                 (0x63)

/* SENSOR REGISTER DEFINE */
#define SIC110A_ID_REG                      (0x01)
#define SIC110A_INFO_REG                    (0x02)


#define CAMSENSOR_SIC110A_FULL_SIZE_WIDTH    352//640
#define CAMSENSOR_SIC110A_FULL_SIZE_HEIGHT   288//480

#define MAX_EV_COMP                          25

/* FPS supported by this sensor */
#define CAMSENSOR_30_FPS                    (30*Q8)
#define CAMSENSOR_15_FPS                    (15*Q8)

/*============================================================================
    LOCAL Variables 
============================================================================*/
static camera_effect_type   camsensor_sic110a_effect             = CAMERA_EFFECT_OFF;
static char                 camsensor_sic110a_sensor_name[32]    = "SIC110A 0.1MP RGB565\0\0\0\0\0\0\0";
static const char           camsensor_sic110a_sensor_suffix[]    = "SIC110A";

/*============================================================================
    Function Declares
============================================================================*/
static void    camsensor_sic110a_register(camsensor_function_table_type *camsensor_function_table_ptr);
static void    camsensor_sic110a_setup_camctrl_tbl(camctrl_tbl_type *camctrl_tbl_ptr);
static boolean initialize_sic110a_registers(uint16 dx, uint16 dy);
static uint16  camsensor_sic110a_get_snapshot_fps(uint16 fps);
static boolean sic110a_i2c_write_byte(uint8 offset, uint8 data);
static boolean sic110a_i2c_read_byte(uint8 offset, uint8* data);

/*============================================================================
    Function Body
============================================================================*/
static camera_ret_code_type camsensor_sic110a_set_exposure_mode(int8 mode)
{
    return CAMERA_SUCCESS;
}

static camera_ret_code_type camsensor_sic110a_set_frame_rate(uint16 fps)
{
    if (fps > CAMSENSOR_30_FPS)
    {
        return CAMERA_INVALID_PARM;
    } 

    return CAMERA_SUCCESS;
} /* camsensor_sic110a_set_frame_rate() */


uint8 camsensor_sic110a_ycbcr_active(void)
{
	return 1;
}

uint8 camsensor_sic110a_ycbcr_unactive(void)
{
	return 0;
}

boolean camsensor_sic110a_init(camsensor_function_table_type *camsensor_function_table_ptr,
                               camctrl_tbl_type              *camctrl_tbl_ptr)
{ 
    uint8   sensor_id;
    uint8 i=150;
    MSG_FATAL("camsensor_sic110a_init!",0,0,0);
    
    /* Input MCLK = 24MHz */
    camsensor_camclk_po_hz = 2400000;
    camsensor_camclk_po_hz = camsensor_config_camclk_po(camsensor_camclk_po_hz);
    
    /* Preview must aways be se to quater size */
    camsensor_preview_resolution  = CAMSENSOR_QTR_SIZE;
    camsensor_snapshot_resolution = CAMSENSOR_FULL_SIZE;
    
    /* Configure I2C parameters */
    camsensor_i2c_command.bus_id     = I2C_BUS_HW_CTRL;
    camsensor_i2c_command.slave_addr = SIC110A_I2C_WRITE_ID;
    
    /*lint -save -e655 */
    camsensor_i2c_command.options    = (i2c_options_type) (I2C_REG_DEV | I2C_START_BEFORE_READ); 
#ifndef FEATURE_VERSION_K212
    CAMERA_CONFIG_GPIO(CAMSENSOR_SIC110A_RESET_PIN);
    gpio_out(CAMSENSOR_SIC110A_RESET_PIN,0);
    camera_timed_wait(10);
    gpio_out(CAMSENSOR_SIC110A_RESET_PIN,1);
#endif
    // Reset Sensor
    camera_timed_wait(10);  //ovt
    if( !sic110a_i2c_write_byte(0x00,0x00))
    {
        MSG_FATAL("Block Select Error!",0,0,0);
        return FALSE;
    }
    
    if( !sic110a_i2c_read_byte(SIC110A_ID_REG,&sensor_id)) 
    {
        MSG_FATAL("read sensor_id failed!",0,0,0);
        return FALSE;
    }
    if ( (sensor_id != 0x0D) )
	{
		MSG_FATAL("sensor_id 1......FALSE",0,0,0);
		return FALSE;
	} 
    
    MSG_FATAL("sensor_id 1 = %x",sensor_id,0,0);
    
    /* Check if it matches it with the value in Datasheet */
    if ( sensor_id != SIC110A_SENSOR_ID)
    {
        return FALSE;
    }
    
    sic110a_i2c_read_byte(SIC110A_INFO_REG,&sensor_id);
    MSG_FATAL("sensor_id 2 = %x",sensor_id,0,0);

    if ( (sensor_id != 0x02)&&(sensor_id != 0x01))
	{
		MSG_FATAL("sensor_id 2 = ......FALSE",0,0,0);
		return FALSE;
	} 

    initialize_sic110a_registers(CAMSENSOR_SIC110A_FULL_SIZE_WIDTH, CAMSENSOR_SIC110A_FULL_SIZE_HEIGHT);
    
    /* Register function table: */
    camsensor_sic110a_register(camsensor_function_table_ptr);

    /* Setup camctrl_tbl */
    camsensor_sic110a_setup_camctrl_tbl(camctrl_tbl_ptr);
    
    MSG_FATAL("camsensor_sic110a_init Exit",0,0,0);
    
    return TRUE;
} /* camsensor_sic110a_init */

static boolean initialize_sic110a_registers(uint16 dx, uint16 dy)
{
#ifndef T_QSC1110
    uint16 x,y;
    uint8 internel_resize = FALSE;
    if(dx > CAMSENSOR_SIC110A_FULL_SIZE_WIDTH)
    {
        dx = CAMSENSOR_SIC110A_FULL_SIZE_WIDTH;
    }
    
    if(dy > CAMSENSOR_SIC110A_FULL_SIZE_HEIGHT)
    {
        dy = CAMSENSOR_SIC110A_FULL_SIZE_HEIGHT;
    }
    
    x = (CAMSENSOR_SIC110A_FULL_SIZE_WIDTH-dx)>>1;
    y = (CAMSENSOR_SIC110A_FULL_SIZE_HEIGHT-dy)>>1;

    //dy = CAMSENSOR_SIC110A_FULL_SIZE_HEIGHT-y;

#endif
    //Sensor Block Setting  ###Don't Change###
    sic110a_i2c_write_byte(0x00, 0x00); 
    sic110a_i2c_write_byte(0x04, 0x00); 
    sic110a_i2c_write_byte(0x05, 0x03); 
    sic110a_i2c_write_byte(0x10, 0x05);
    
    sic110a_i2c_write_byte(0x11, 0x36); 
    sic110a_i2c_write_byte(0x12, 0x13); 
    
    sic110a_i2c_write_byte(0x40, 0x00); 
    sic110a_i2c_write_byte(0x41, 0x16); 
    sic110a_i2c_write_byte(0x42, 0x32); 
    sic110a_i2c_write_byte(0X43, 0X80); 
    
    //SIC110A 50Hz Setting MCLK : 12MHz
    sic110a_i2c_write_byte(0x20, 0x00); //00
    sic110a_i2c_write_byte(0x21, 0x00);//00
    sic110a_i2c_write_byte(0x22, 0x00);//00
    sic110a_i2c_write_byte(0x23, 0x00);
    
    sic110a_i2c_write_byte(0x00, 0x01); 
    sic110a_i2c_write_byte(0x34, 0x18);//4B);
    
    //AE Control
    sic110a_i2c_write_byte(0x10, 0x80); 
    sic110a_i2c_write_byte(0x11, 0x03);//0x03
    sic110a_i2c_write_byte(0x12, 0x80); 
    sic110a_i2c_write_byte(0x13, 0x80); 
    sic110a_i2c_write_byte(0x14, 0x75); 
    sic110a_i2c_write_byte(0x1C, 0x1A); 
    
    sic110a_i2c_write_byte(0x40, 0x4F); 
    sic110a_i2c_write_byte(0x41, 0x08); 
    
    //AWB Control
    sic110a_i2c_write_byte(0x00, 0x02); 
    sic110a_i2c_write_byte(0x10, 0xD3); 
    sic110a_i2c_write_byte(0x11, 0xC0); 
    sic110a_i2c_write_byte(0x13, 0x80); 
    sic110a_i2c_write_byte(0x14, 0x80); 
    sic110a_i2c_write_byte(0x15, 0xFE); 
    sic110a_i2c_write_byte(0x16, 0x70); 
    sic110a_i2c_write_byte(0x17, 0xEA); 
    sic110a_i2c_write_byte(0x18, 0x80); 
    sic110a_i2c_write_byte(0x19, 0xA0); 
    sic110a_i2c_write_byte(0x1A, 0x70); 
    sic110a_i2c_write_byte(0x1B, 0xA0); 
    sic110a_i2c_write_byte(0x1C, 0x70); 
    sic110a_i2c_write_byte(0x1D, 0xB0); 
    sic110a_i2c_write_byte(0x1E, 0x70); 
    sic110a_i2c_write_byte(0x20, 0xFE); 
    sic110a_i2c_write_byte(0x21, 0x98); 
    sic110a_i2c_write_byte(0x22, 0x9D); 
    sic110a_i2c_write_byte(0x23, 0x18); 
    sic110a_i2c_write_byte(0x25, 0x20); 
    sic110a_i2c_write_byte(0x26, 0x0F); 
    sic110a_i2c_write_byte(0x27, 0x00); 
    sic110a_i2c_write_byte(0x28, 0x00); 
    sic110a_i2c_write_byte(0x29, 0xD0); 
    sic110a_i2c_write_byte(0x2A, 0x90); 
    sic110a_i2c_write_byte(0x30, 0x05);
    sic110a_i2c_write_byte(0x41, 0x04); 
    sic110a_i2c_write_byte(0x44, 0x13); 
    sic110a_i2c_write_byte(0x45, 0x6B); 
    sic110a_i2c_write_byte(0x46, 0xA3); 
    
    //RGB to YCbCr (CSC) No used
    sic110a_i2c_write_byte(0x50, 0x33); 
    sic110a_i2c_write_byte(0x51, 0x20); 
    sic110a_i2c_write_byte(0x52, 0xE5); 
    sic110a_i2c_write_byte(0x53, 0xFB); 
    sic110a_i2c_write_byte(0x54, 0x13); 
    sic110a_i2c_write_byte(0x55, 0x26); 
    sic110a_i2c_write_byte(0x56, 0x07); 
    sic110a_i2c_write_byte(0x57, 0xF5); 
    sic110a_i2c_write_byte(0x58, 0xEA); 
    sic110a_i2c_write_byte(0x59, 0x21); 
    
    //D65, D30, D20
    sic110a_i2c_write_byte(0x63, 0x93); 
    sic110a_i2c_write_byte(0x64, 0xBF); 
    sic110a_i2c_write_byte(0x65, 0x93); 
    sic110a_i2c_write_byte(0x66, 0xBF); 
    sic110a_i2c_write_byte(0x67, 0xB0); 
    sic110a_i2c_write_byte(0x68, 0xA0); 
    sic110a_i2c_write_byte(0x69, 0xB0); 
    sic110a_i2c_write_byte(0x6A, 0xA0); 
    
    //IDP Control
    sic110a_i2c_write_byte(0x00, 0x03); 
    sic110a_i2c_write_byte(0x10, 0xEF); 
#ifndef T_QSC1110
    sic110a_i2c_write_byte(0x11, 0x1d);  // PCLK, HSync, VSync (L,H,L)
#else
    sic110a_i2c_write_byte(0x11, 0x0D);  // PCLK, HSync, VSync (L,H,L)
#endif
#ifdef SIC110A_OUTFORMAT_RGB565
    sic110a_i2c_write_byte(0x12, 0x0C); //0x3c
#else
    sic110a_i2c_write_byte(0x12, 0x9D);  // YCbCr 
#endif

    //Shading
#ifndef T_QSC1110
    sic110a_i2c_write_byte(0x27, 0xFD); //0x11 0xFD
    sic110a_i2c_write_byte(0x28, 0xCB); //0x11 0xCB
    sic110a_i2c_write_byte(0x29, 0xA8); //0x55 0xA8
    sic110a_i2c_write_byte(0x2A, 0x64); //0x43 0x64
#else
    sic110a_i2c_write_byte(0x27, 0x11); //0x11 0xFD
    sic110a_i2c_write_byte(0x28, 0x11); //0x11 0xCB
    sic110a_i2c_write_byte(0x29, 0x55); //0x55 0xA8
    sic110a_i2c_write_byte(0x2A, 0x43); //0x43 0x64
#endif
    sic110a_i2c_write_byte(0x2B, 0x21); //0x21 0x21
    sic110a_i2c_write_byte(0x2C, 0x00); //R left # right
    sic110a_i2c_write_byte(0x2D, 0x00); //R top # bottom 
    sic110a_i2c_write_byte(0x2E, 0x00); //G left # right 
    sic110a_i2c_write_byte(0x2F, 0x00); //G top # bottom 
    sic110a_i2c_write_byte(0x30, 0x00); //B left # right 
    sic110a_i2c_write_byte(0x31, 0x00); //B top # bottom 
    sic110a_i2c_write_byte(0x32, 0x5B); 
    sic110a_i2c_write_byte(0x33, 0x4B); 
    
    //Gamma 
    sic110a_i2c_write_byte(0x34, 0x00); 
    sic110a_i2c_write_byte(0x35, 0x0B); 
    sic110a_i2c_write_byte(0x36, 0x15); 
    sic110a_i2c_write_byte(0x37, 0x29); 
    sic110a_i2c_write_byte(0x38, 0x4A); 
    sic110a_i2c_write_byte(0x39, 0x66); 
    sic110a_i2c_write_byte(0x3A, 0x7A); 
    sic110a_i2c_write_byte(0x3B, 0x8D); 
    sic110a_i2c_write_byte(0x3C, 0x9D); 
    sic110a_i2c_write_byte(0x3D, 0xAA); 
    sic110a_i2c_write_byte(0x3E, 0xB6); 
    sic110a_i2c_write_byte(0x3F, 0xCB); 
    sic110a_i2c_write_byte(0x40, 0xDE); 
    sic110a_i2c_write_byte(0x41, 0xF0); 
    sic110a_i2c_write_byte(0x42, 0xF8); 
    sic110a_i2c_write_byte(0x43, 0xFF); 
    
    //DPC De-noise
    sic110a_i2c_write_byte(0x44, 0xBB); 
    sic110a_i2c_write_byte(0x45, 0x10); 
    sic110a_i2c_write_byte(0x46, 0x27); 
    sic110a_i2c_write_byte(0x47, 0x08); 
    sic110a_i2c_write_byte(0x48, 0x08); 
    sic110a_i2c_write_byte(0x49, 0x8F); 
    sic110a_i2c_write_byte(0x4A, 0x33); 
    sic110a_i2c_write_byte(0x4B, 0x30); //De-noise Start Gain 
    sic110a_i2c_write_byte(0x4C, 0xDD); 
    sic110a_i2c_write_byte(0x4D, 0x00); 
    sic110a_i2c_write_byte(0x4E, 0xFF); 
    sic110a_i2c_write_byte(0x4F, 0x04); //LPF Threshold
    sic110a_i2c_write_byte(0x50, 0x3A); 
    sic110a_i2c_write_byte(0x51, 0x2F); 
    sic110a_i2c_write_byte(0x52, 0x84); 
    sic110a_i2c_write_byte(0x53, 0x74);
    
    //D65 CMA
    sic110a_i2c_write_byte(0x54, 0x3E); 
    sic110a_i2c_write_byte(0x55, 0xC9); 
    sic110a_i2c_write_byte(0x56, 0xF9); 
    sic110a_i2c_write_byte(0x57, 0x13); 
    sic110a_i2c_write_byte(0x58, 0x26); 
    sic110a_i2c_write_byte(0x59, 0x07); 
    sic110a_i2c_write_byte(0x5A, 0xF2); 
    sic110a_i2c_write_byte(0x5B, 0xBC); 
    sic110a_i2c_write_byte(0x5C, 0x52); 
    
    //D30 CMA
    sic110a_i2c_write_byte(0x5D, 0x48); 
    sic110a_i2c_write_byte(0x5E, 0xC0); 
    sic110a_i2c_write_byte(0x5F, 0xF9); 
    sic110a_i2c_write_byte(0x60, 0x0E); 
    sic110a_i2c_write_byte(0x61, 0x2D); 
    sic110a_i2c_write_byte(0x62, 0x05); 
    sic110a_i2c_write_byte(0x63, 0xEA); 
    sic110a_i2c_write_byte(0x64, 0xC4); 
    sic110a_i2c_write_byte(0x65, 0x52); 
    
    //D20 CMA
    sic110a_i2c_write_byte(0x66, 0x3A); 
    sic110a_i2c_write_byte(0x67, 0xCB); 
    sic110a_i2c_write_byte(0x68, 0xFB); 
    sic110a_i2c_write_byte(0x69, 0x0E); 
    sic110a_i2c_write_byte(0x6A, 0x2D); 
    sic110a_i2c_write_byte(0x6B, 0x05); 
    sic110a_i2c_write_byte(0x6C, 0xEA); 
    sic110a_i2c_write_byte(0x6D, 0xC4); 
    sic110a_i2c_write_byte(0x6E, 0x52); 
    
    sic110a_i2c_write_byte(0x6F, 0xC0); 
    
    //Edge
    sic110a_i2c_write_byte(0x70, 0x20); //Edge type1 upper gain
    sic110a_i2c_write_byte(0x71, 0x40); //Edge type1 down gain
    sic110a_i2c_write_byte(0x72, 0x01); 
    sic110a_i2c_write_byte(0x73, 0x01); 
    sic110a_i2c_write_byte(0x74, 0x20); 
    sic110a_i2c_write_byte(0x75, 0x20); 
    sic110a_i2c_write_byte(0x76, 0x30); //Edge type1 Suppress start gain 
    sic110a_i2c_write_byte(0x77, 0x10); 
    sic110a_i2c_write_byte(0x78, 0x20); 
    sic110a_i2c_write_byte(0x79, 0x20); //Edge type2 upper gain
    sic110a_i2c_write_byte(0x7A, 0x40); //Edge type2 down gain 
    sic110a_i2c_write_byte(0x7B, 0x03); 
    sic110a_i2c_write_byte(0x7C, 0x03); 
    sic110a_i2c_write_byte(0x7D, 0x20); 
    sic110a_i2c_write_byte(0x7E, 0x20); 
    sic110a_i2c_write_byte(0x7F, 0x30); //Edge type2 Suppress start gain
    sic110a_i2c_write_byte(0x80, 0x10); 
    sic110a_i2c_write_byte(0x81, 0x40); 
    sic110a_i2c_write_byte(0x82, 0x7E); 
    sic110a_i2c_write_byte(0x83, 0x10); //Flat Region Sensing Threshold
    
    //Saturation Gain
    sic110a_i2c_write_byte(0x85, 0x10); 
    sic110a_i2c_write_byte(0x86, 0x12); 
    sic110a_i2c_write_byte(0x87, 0x12); 
    sic110a_i2c_write_byte(0x88, 0x00); 
    
    sic110a_i2c_write_byte(0x8C, 0xFF); 
    sic110a_i2c_write_byte(0x8D, 0x00); 
    sic110a_i2c_write_byte(0x8E, 0xFF); 
    sic110a_i2c_write_byte(0x8F, 0x00); 
    sic110a_i2c_write_byte(0x90, 0xFF); 
    sic110a_i2c_write_byte(0x91, 0x00); 
    
    sic110a_i2c_write_byte(0x92, 0x20); 
    sic110a_i2c_write_byte(0x93, 0x48); 
#ifndef T_QSC1110
    //Windowing
    sic110a_i2c_write_byte(0xA0, (byte)((x>>8)<<6)|((dx>>8)<<4)|((y>>8)<<3)|((dy>>8)<<2));
    sic110a_i2c_write_byte(0xA1, (byte)x); 
    sic110a_i2c_write_byte(0xA2, (byte)dx); 
    sic110a_i2c_write_byte(0xA3, (byte)y); 
    sic110a_i2c_write_byte(0xA4, (byte)dy);     
    camera_timed_wait(100);  //ovt
    MSG_FATAL("sic110a_i2c_write_byte x=%d dx=%d",x,dx,0);    
    MSG_FATAL("sic110a_i2c_write_byte y=%d dy=%d",y,dy,0);
#else
    sic110a_i2c_write_byte(0xA0, 0x14);
    sic110a_i2c_write_byte(0xA1, 0x00); 
    sic110a_i2c_write_byte(0xA2, 0xa0);  //0xa0
    sic110a_i2c_write_byte(0xA3, 0x00); 
    sic110a_i2c_write_byte(0xA4, 0x80);  //0x80
#endif
    //BLC value gain
    sic110a_i2c_write_byte(0xA8, 0x80); 
    sic110a_i2c_write_byte(0xA9, 0x80); 
    sic110a_i2c_write_byte(0xAA, 0x80); 
    sic110a_i2c_write_byte(0xAB, 0x80); 
    
    sic110a_i2c_write_byte(0xB0, 0x02); //De-noise control by Analog gain reference
    
    //Sesnor On
    sic110a_i2c_write_byte(0x00, 0x00); 
#ifndef T_QSC1110
    sic110a_i2c_write_byte(0x03, 0x05);
#else
    sic110a_i2c_write_byte(0x03, 0xf5);
#endif


    /*Customer can adjust GAMMA, MIRROR & UPSIDEDOWN here!*/
    return TRUE;
} /* end of initialize_sic110a_registers_snapshot. */

static boolean camsensor_sic110a_start( camsensor_static_params_type *camsensor_params)
{
    /* Initialize CAMIF operation mode */
    camsensor_params->camif_config.SyncMode    = CAMIF_APS;
    camsensor_params->camif_config.HSyncEdge   = CAMIF_High;
    camsensor_params->camif_config.VSyncEdge   = CAMIF_Low;
    camsensor_params->camif_efs_config.EFS_EOL = 0x0000;
    camsensor_params->camif_efs_config.EFS_SOL = 0x0000;
    camsensor_params->camif_efs_config.EFS_EOF = 0x0000;
    camsensor_params->camif_efs_config.EFS_SOF = 0x0000;


    /* ------------------  Sensor-specific Config -------------- */
    /* Make/model of sensor */
    camsensor_params->sensor_model  = CAMSENSOR_SIC110A_ID;
    //use_camsensor_sic110a = TRUE;
    //use_camsensor_siv121a = FALSE;
    /* CCD or CMOS */
    camsensor_params->sensor_type   = CAMSENSOR_CMOS;
#ifdef T_QSC1110
	camsensor_params->format = CAMIF_YCbCr_Cr_Y_Cb_Y;
#endif
    /* BAYER or YCbCr */
    camsensor_params->output_format = CAMSENSOR_YCBCR;

    /* A pointer to the sensor name for EXIF tags */
    camsensor_params->sensor_name = camsensor_sic110a_sensor_name;

    /* Pointer to sensor suffix */

    camsensor_params->sensor_suffix=camsensor_sic110a_sensor_suffix;

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
    camsensor_params->full_size_width  = CAMSENSOR_SIC110A_FULL_SIZE_WIDTH;
    camsensor_params->full_size_height = CAMSENSOR_SIC110A_FULL_SIZE_HEIGHT;
    
    camsensor_params->qtr_size_width  =   CAMSENSOR_SIC110A_FULL_SIZE_WIDTH;
    camsensor_params->qtr_size_height =  CAMSENSOR_SIC110A_FULL_SIZE_HEIGHT;
    camsensor_params->pclk_invert     =  camsensor_info.pclk_invert;
    
#if defined FEATURE_WHITE_LED_FLASH || defined FEATURE_STROBE_FLASH
    camsensor_params->support_auto_flash = FALSE;
#else
    camsensor_params->support_auto_flash = FALSE;
#endif

    return TRUE;
} /*camsensor_sic110a_start*/

static boolean sic110a_i2c_write_byte(uint8 offset, uint8 data) 
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

static boolean sic110a_i2c_read_byte(uint8 offset, uint8 *data) 
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
} /* end of sic110a_i2c_read_word. */

static camera_ret_code_type camsensor_sic110a_set_effect(int8 effect)
{
    return CAMERA_NOT_SUPPORTED;
} /* camsensor_sic110a_set_effect() */

/*===========================================================================

FUNCTION      camsensor_sic110a_snapshot_config

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

static boolean camsensor_sic110a_snapshot_config( camsensor_static_params_type  *camsensor_params)
{
	uint16 x,y;
    uint8 internel_resize = FALSE;
    /* Set the current dimensions */
    camsensor_params->camsensor_width  = camera_dx;
    camsensor_params->camsensor_height = camera_dy;
    
    /* CAMIF frame */
    camsensor_params->camif_frame_config.pixelsPerLine = camera_dx*2;
    camsensor_params->camif_frame_config.linesPerFrame = camera_dy;
    
    /* CAMIF Window */
    camsensor_params->camif_window_width_config.firstPixel = 0; //°´CLK
    camsensor_params->camif_window_width_config.lastPixel  = camsensor_params->camif_window_width_config.firstPixel+(camera_dx*2);
    
    camsensor_params->camif_window_height_config.firstLine = 0;
    camsensor_params->camif_window_height_config.lastLine  = camsensor_params->camif_window_height_config.firstLine+camera_dy;
    camsensor_params->pixel_clock = 1;

	MSG_FATAL("sic110a_i2c_write_byte y=%d dy=%d",y,camera_dy,0);
    MSG_FATAL("sic110a_i2c_write_byte x=%d dx=%d",x,camera_dx,0);
    initialize_sic110a_registers(camera_dx, camera_dy);

    return TRUE;
}
/*===========================================================================

FUNCTION      camsensor_sic110a_raw_snapshot_config

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
static boolean camsensor_sic110a_raw_snapshot_config(camsensor_static_params_type  *camsensor_params)
{
    ERR("camsensor_sic110a_raw_snapshot_config!",0,0,0);
    return TRUE;
}

/*===========================================================================

FUNCTION      camsensor_sic110a_video_config

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
static boolean camsensor_sic110a_video_config(camsensor_static_params_type *camsensor_params)
{
        /* Set the current dimensions */

    /* Sensor output data format */
    #ifdef T_QSC1110
	camsensor_params->discardFirstFrame = TRUE;
	camsensor_params->format = CAMIF_YCbCr_Cr_Y_Cb_Y;

	switch (camsensor_preview_resolution)
	{
		case CAMSENSOR_QTR_SIZE:
			/* Set the current dimensions */
			camsensor_params->camsensor_width = camsensor_params->qtr_size_width;
			camsensor_params->camsensor_height = camsensor_params->qtr_size_height;
			/* CAMIF frame */
			camsensor_params->camif_frame_config.pixelsPerLine = camsensor_params->qtr_size_width*2;
			camsensor_params->camif_frame_config.linesPerFrame = camsensor_params->qtr_size_height;
			break;

		case CAMSENSOR_FULL_SIZE:
			/* Set the current dimensions */
			camsensor_params->camsensor_width = camsensor_params->full_size_width;
			camsensor_params->camsensor_height = camsensor_params->full_size_height;
			/* CAMIF frame */
			camsensor_params->camif_frame_config.pixelsPerLine = camsensor_params->full_size_width*2;
			camsensor_params->camif_frame_config.linesPerFrame = camsensor_params->full_size_height;
			break;

		default:
			return FALSE;
	} /* camsensor_preview_resolution */

	/* CAMIF window */
	MSG_FATAL("camsensor_params->camsensor_width=====%d",camsensor_params->camsensor_width,0,0);
	MSG_FATAL("camsensor_params->camsensor_height=====%d",camsensor_params->camsensor_height,0,0);
	camsensor_params->camif_window_width_config.firstPixel = 0;
	camsensor_params->camif_window_width_config.lastPixel  = camsensor_params->camif_window_width_config.firstPixel + camsensor_params->camsensor_width*2 - 1;
	camsensor_params->camif_window_height_config.firstLine = 0;
	camsensor_params->camif_window_height_config.lastLine = camsensor_params->camif_window_height_config.firstLine + camsensor_params->camsensor_height*2 - 1;

	//	camsensor_SIV121A_ycbcr_write_sensor (camsensor_preview_resolution);//yty add
	camsensor_current_resolution = camsensor_preview_resolution;
	#else
	 /* Set the current dimensions */
    camsensor_params->camsensor_width  = camera_preview_dx;
    camsensor_params->camsensor_height = camera_preview_dy;

    /* CAMIF frame */
    camsensor_params->camif_frame_config.pixelsPerLine =camera_preview_dx*2;
    camsensor_params->camif_frame_config.linesPerFrame =camera_preview_dy;
    
    /* CAMIF Window */
    camsensor_params->camif_window_width_config.firstPixel = 0; //°´CLK
    camsensor_params->camif_window_width_config.lastPixel  = camsensor_params->camif_window_width_config.firstPixel+(camera_preview_dx*2);

    camsensor_params->camif_window_height_config.firstLine = 0;
    camsensor_params->camif_window_height_config.lastLine  = camsensor_params->camif_window_height_config.firstLine+camera_preview_dy;
    camsensor_params->pixel_clock = 1;
    initialize_sic110a_registers(camera_preview_dx, camera_preview_dy);
	#endif
    return TRUE;
}

static void camsensor_sic110a_power_up(void)
{
    ERR("camsensor_sic110a_power_up!",0,0,0);
} /* camsensor_sic110a_power_up */

/*===========================================================================

FUNCTION      camsensor_sic110a_power_down

DESCRIPTION
Power down the sensor, using I2C driver.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void camsensor_sic110a_power_down(void)
{
    sic110a_i2c_write_byte(0x00, 0x00);
    sic110a_i2c_write_byte(0x03, 0x02); //sensor sleep mode

    ERR("camsensor_sic110a_power_down!",0,0,0);
} /* camsensor_sic110a_power_down */

static camera_ret_code_type camsensor_sic110a_set_wb(int8 wb)
{
    sic110a_i2c_write_byte(0x00,0x03); //bank 3
    ERR("camsensor_sic110a_set_wb!",0,0,0);
    switch((camera_wb_type)wb)
    {
        case CAMERA_WB_AUTO:
        {
            sic110a_i2c_write_byte(0x10, 0xD3);
            break;
        }
        
        case CAMERA_WB_INCANDESCENT:
        {
            sic110a_i2c_write_byte(0x10, 0x00);  // disable AWB
            sic110a_i2c_write_byte(0x60, 0x98);
            sic110a_i2c_write_byte(0x61, 0xC8);
            break;
        }
        
        case CAMERA_WB_FLUORESCENT:
        {
            sic110a_i2c_write_byte(0x10, 0x00);  // disable AWB
            sic110a_i2c_write_byte(0x60, 0xAA);
            sic110a_i2c_write_byte(0x61, 0xBE);
            break;
        }
        
        case CAMERA_WB_DAYLIGHT:
        {
            sic110a_i2c_write_byte(0x10, 0x00);  // disable AWB
            sic110a_i2c_write_byte(0x60, 0xC2);
            sic110a_i2c_write_byte(0x61, 0x9E);
            break;
        }
        
        case CAMERA_WB_CLOUDY_DAYLIGHT:
        {
            sic110a_i2c_write_byte(0x10, 0x00);  // disable AWB
            sic110a_i2c_write_byte(0x60, 0xD0);
            sic110a_i2c_write_byte(0x61, 0x88);
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

static camera_ret_code_type camsensor_sic110a_set_ev_compensation(int32 compensation)
{
    camera_ret_code_type ret_val = CAMERA_SUCCESS;
    int16  parmCurNum = (int16)(compensation>>16);
    int32 max_parmVal = 12;
    int32 min_parmVal =-12;
    int32  parmVal= parmCurNum;
    uint32 new_luma_target;

    ERR("camsensor_sic110a_set_ev_compensation!",0,0,0);
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
        sic110a_i2c_write_byte(0x00,0x02); //bank 1
        switch(new_luma_target)
        {
            case 8:    // -4 EV
                sic110a_i2c_write_byte(0x12, 0x40);
                sic110a_i2c_write_byte(0x13, 0x40);
                sic110a_i2c_write_byte(0x14, 0x40);
                sic110a_i2c_write_byte(0x70, 0xd4);
                break;

            case 9:    // -3 EV
                sic110a_i2c_write_byte(0x12, 0x50);
                sic110a_i2c_write_byte(0x13, 0x50);
                sic110a_i2c_write_byte(0x14, 0x50);
                sic110a_i2c_write_byte(0x70, 0xd4);
                break;

            case 10:    // -2 EV
                sic110a_i2c_write_byte(0x12, 0x60);
                sic110a_i2c_write_byte(0x13, 0x60);
                sic110a_i2c_write_byte(0x14, 0x60);
                sic110a_i2c_write_byte(0x70, 0xd4);
                break;

            case 11:    // -1 EV
                sic110a_i2c_write_byte(0x12, 0x70);
                sic110a_i2c_write_byte(0x13, 0x70);
                sic110a_i2c_write_byte(0x14, 0x70);
                sic110a_i2c_write_byte(0x70, 0xd4);
                break;

            case 12:   // +0 EV
                sic110a_i2c_write_byte(0x12, 0x80);
                sic110a_i2c_write_byte(0x13, 0x80);
                sic110a_i2c_write_byte(0x14, 0x80);
                sic110a_i2c_write_byte(0x70, 0x00);
                break;

            case 13:    // +1 EV
                sic110a_i2c_write_byte(0x12, 0x90);
                sic110a_i2c_write_byte(0x13, 0x90);
                sic110a_i2c_write_byte(0x14, 0x90);
                sic110a_i2c_write_byte(0x70, 0x00);
                break;

            case 14:    // +2 EV
                sic110a_i2c_write_byte(0x12, 0xA0);
                sic110a_i2c_write_byte(0x13, 0xA0);
                sic110a_i2c_write_byte(0x14, 0xA0);
                sic110a_i2c_write_byte(0x70, 0x00);
                break;

            case 15:    // +3 EV
                sic110a_i2c_write_byte(0x12, 0xB0);
                sic110a_i2c_write_byte(0x13, 0xB0);
                sic110a_i2c_write_byte(0x14, 0xB0);
                sic110a_i2c_write_byte(0x70, 0x00);
                break;

            case 16:    // +4 EV
                sic110a_i2c_write_byte(0x12, 0xC0);
                sic110a_i2c_write_byte(0x13, 0xC0);
                sic110a_i2c_write_byte(0x14, 0xC0);
                sic110a_i2c_write_byte(0x70, 0x00);
                break;

            default:
                break;
        }
    }

    return ret_val;
}

static camera_ret_code_type camsensor_sic110a_set_brightness(int8 brightness)
{
    ERR("camsensor_sic110a_set_brightness!",0,0,0);
    return CAMERA_SUCCESS;
}


static camera_ret_code_type camsensor_sic110a_set_contrast(int8 contrast)
{
    ERR("camsensor_sic110a_set_contrast!",0,0,0);
    return CAMERA_SUCCESS;
}/* camsensor_sic110a_set_contrast() */

static void camsensor_sic110a_setup_camctrl_tbl(camctrl_tbl_type *camctrl_tbl_ptr)
{
    ERR("camsensor_sic110a_get_snapshot_fps!",0,0,0);
    camctrl_tbl_ptr->asf_5x5_is_supported         = FALSE; 
    camctrl_tbl_ptr->get_snapshot_frame_per_sec = camsensor_sic110a_get_snapshot_fps;
}

static uint16 camsensor_sic110a_get_snapshot_fps(uint16 fps)
{
    ERR("camsensor_sic110a_get_snapshot_fps!",0,0,0);
    /* In the current driver, the snapshot fps is always 11 */
    fps = 15 * Q8;
    return fps;
}

camera_ret_code_type camsensor_sic110a_set_antibanding(int8 antibanding)
{
    ERR("camsensor_ov2655_set_antibanding!",0,0,0);
    return CAMERA_SUCCESS;
}

static void camsensor_sic110a_register(camsensor_function_table_type *camsensor_function_table_ptr)
{
    ERR("camsensor_sic110a_register!",0,0,0);
    camsensor_function_table_ptr->camsensor_start                 = camsensor_sic110a_start;
    camsensor_function_table_ptr->camsensor_video_config          = camsensor_sic110a_video_config;
    camsensor_function_table_ptr->camsensor_raw_snapshot_config   = camsensor_sic110a_raw_snapshot_config;
    camsensor_function_table_ptr->camsensor_snapshot_config       = camsensor_sic110a_snapshot_config;
    camsensor_function_table_ptr->camsensor_power_up              = camsensor_sic110a_power_up;
    camsensor_function_table_ptr->camsensor_power_down            = camsensor_sic110a_power_down;
    camsensor_function_table_ptr->camsensor_set_frame_rate        = camsensor_sic110a_set_frame_rate;  

    camsensor_function_table_ptr->camsensor_set_effect            = camsensor_sic110a_set_effect;

    camsensor_function_table_ptr->camsensor_set_brightness        = camsensor_sic110a_set_brightness;
    camsensor_function_table_ptr->camsensor_set_contrast          = camsensor_sic110a_set_contrast;
    camsensor_function_table_ptr->camsensor_set_antibanding       = camsensor_sic110a_set_antibanding;
    camsensor_function_table_ptr->camsensor_set_exposure_mode     = camsensor_sic110a_set_exposure_mode;
    camsensor_function_table_ptr->camsensor_set_ev_compensation   = camsensor_sic110a_set_ev_compensation; 
    camsensor_function_table_ptr->camsensor_set_wb                = camsensor_sic110a_set_wb;
} /* camsensor_mt9d112_mu2m0yu_register */
#endif

