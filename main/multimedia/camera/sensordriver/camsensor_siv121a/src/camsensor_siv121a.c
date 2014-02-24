#include "camsensor.h"

#if defined (FEATURE_CAMERA) && defined (USE_CAMSENSOR_SIV121A)
#include "camsensor_siv121a.h"
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
//#define CAMSENSOR_HIGHQUALITY_PREVIEW
#if defined(FEATURE_VERSION_W516) || defined(FEATURE_VERSION_W208S)
#define CAMSENSOR_SIV121A_RESET_PIN         GPIO_OUTPUT_53
#else
#define CAMSENSOR_SIV121A_RESET_PIN         GPIO_OUTPUT_62
#endif

#define SIV121A_OUTFORMAT_RGB565

// sensor's chip ID and version
#define SIV121A_SENSOR_ID                   (0x92)
#define SIV121A_SENSOR_VERSION              (0x11)

#define SIV121A_I2C_WRITE_ID                (0x66)
#define SIV121A_I2C_READ_ID                 (0x67)

/* SENSOR REGISTER DEFINE */
#define SIV121A_ID_REG                      (0x01)
#define SIV121A_INFO_REG                    (0x02)

#define CAMSENSOR_SIV121A_FULL_SIZE_WIDTH    652
#define CAMSENSOR_SIV121A_FULL_SIZE_HEIGHT   490//480

#ifdef CAMSENSOR_HIGHQUALITY_PREVIEW
#define CAMSENSOR_SIV121A_QTR_SIZE_WIDTH     652
#define CAMSENSOR_SIV121A_QTR_SIZE_HEIGHT    490//480
#else
#define CAMSENSOR_SIV121A_QTR_SIZE_WIDTH     320
#define CAMSENSOR_SIV121A_QTR_SIZE_HEIGHT    240
#endif

#define MAX_EV_COMP                          25

/* FPS supported by this sensor */
#define CAMSENSOR_30_FPS                    (30*Q8)
#define CAMSENSOR_15_FPS                    (15*Q8)

/*============================================================================
    LOCAL Variables 
============================================================================*/
static camera_effect_type   camsensor_siv121a_effect             = CAMERA_EFFECT_OFF;
static char                 camsensor_siv121a_sensor_name[32]    = "SIV121A 0.3MP RGB565\0\0\0\0\0\0\0";
static const char           camsensor_siv121a_sensor_suffix[]    = "SIV121A";

/*============================================================================
    Function Declares
============================================================================*/
static void    camsensor_siv121a_register(camsensor_function_table_type *camsensor_function_table_ptr);
static void    camsensor_siv121a_setup_camctrl_tbl(camctrl_tbl_type *camctrl_tbl_ptr);
#ifndef CAMSENSOR_HIGHQUALITY_PREVIEW
static boolean initialize_siv121a_registers_preview(uint16 dx, uint16 dy);
#endif
static boolean initialize_siv121a_registers(uint16 dx, uint16 dy);
static uint16  camsensor_siv121a_get_snapshot_fps(uint16);
static boolean siv121a_i2c_write_byte(uint8 offset, uint8 data);
static boolean siv121a_i2c_read_byte(uint8 offset, uint8* data);

/*============================================================================
    Function Body
============================================================================*/
static camera_ret_code_type camsensor_siv121a_set_exposure_mode(int8 mode)
{
    return CAMERA_SUCCESS;
}

static camera_ret_code_type camsensor_siv121a_set_frame_rate(uint16 fps)
{
    if (fps > CAMSENSOR_30_FPS)
    {
        return CAMERA_INVALID_PARM;
    } 

    return CAMERA_SUCCESS;
} /* camsensor_siv121a_set_frame_rate() */

uint8 camsensor_SIV121A_ycbcr_active(void)
{
	return 0;
}

uint8 camsensor_SIV121A_ycbcr_unactive(void)
{
	return 1;
}


boolean camsensor_siv121a_init(camsensor_function_table_type *camsensor_function_table_ptr,
                               camctrl_tbl_type              *camctrl_tbl_ptr)
{
    uint8   sensor_id;
    
    MSG_FATAL("camsensor_siv121a_init!",0,0,0);
    
    /* Input MCLK = 24MHz */
    camsensor_camclk_po_hz = 24000000;
    camsensor_camclk_po_hz = camsensor_config_camclk_po(camsensor_camclk_po_hz);
    
    /* Preview must aways be se to quater size */
    camsensor_preview_resolution  = CAMSENSOR_QTR_SIZE;
    camsensor_snapshot_resolution = CAMSENSOR_FULL_SIZE;
    
    /* Configure I2C parameters */
    camsensor_i2c_command.bus_id     = I2C_BUS_HW_CTRL;
    camsensor_i2c_command.slave_addr = SIV121A_I2C_WRITE_ID;
    
    /*lint -save -e655 */
    camsensor_i2c_command.options    = (i2c_options_type) (I2C_REG_DEV | I2C_START_BEFORE_READ); 

    CAMERA_CONFIG_GPIO(CAMSENSOR_SIV121A_RESET_PIN);
    gpio_out(CAMSENSOR_SIV121A_RESET_PIN,0);
    camera_timed_wait(10);
    gpio_out(CAMSENSOR_SIV121A_RESET_PIN,1);
    
    // Reset Sensor
    camera_timed_wait(10);  //ovt
    if( !siv121a_i2c_write_byte(0x00,0x00))
    {
        MSG_FATAL("Block Select Error!",0,0,0);
        return FALSE;
    }
    
    if( !siv121a_i2c_read_byte(SIV121A_ID_REG,&sensor_id)) 
    {
        MSG_FATAL("read sensor_id failed!",0,0,0);
        return FALSE;
    }
    
    MSG_FATAL("sensor_id 1 = %x",sensor_id,0,0);
    
    /* Check if it matches it with the value in Datasheet */
    if ( sensor_id != SIV121A_SENSOR_ID)
    {
        return FALSE;
    }
    
    siv121a_i2c_read_byte(SIV121A_INFO_REG,&sensor_id);
    MSG_FATAL("sensor_id 2 = %x",sensor_id,0,0);
#ifdef CAMSENSOR_HIGHQUALITY_PREVIEW
    initialize_siv121a_registers(CAMSENSOR_SIV121A_QTR_SIZE_WIDTH, CAMSENSOR_SIV121A_QTR_SIZE_HEIGHT);
#else
    initialize_siv121a_registers_preview(CAMSENSOR_SIV121A_QTR_SIZE_WIDTH, CAMSENSOR_SIV121A_QTR_SIZE_HEIGHT);
#endif
    /* Register function table: */
    camsensor_siv121a_register(camsensor_function_table_ptr);

    /* Setup camctrl_tbl */
    camsensor_siv121a_setup_camctrl_tbl(camctrl_tbl_ptr);
    return TRUE;
} /* camsensor_siv121a_init */

#ifndef CAMSENSOR_HIGHQUALITY_PREVIEW
static boolean initialize_siv121a_registers_preview(uint16 dx, uint16 dy)
{
    uint16 x,y;
    
    if(dx > CAMSENSOR_SIV121A_QTR_SIZE_WIDTH)
    {
        dx = CAMSENSOR_SIV121A_QTR_SIZE_WIDTH;
    }
    
    if(dy > CAMSENSOR_SIV121A_QTR_SIZE_HEIGHT)
    {
        dy = CAMSENSOR_SIV121A_QTR_SIZE_HEIGHT;
    }
    
    x = (CAMSENSOR_SIV121A_QTR_SIZE_WIDTH-dx)>>1;
    y = (CAMSENSOR_SIV121A_QTR_SIZE_HEIGHT-dy)>>1;
    
    dy = CAMSENSOR_SIV121A_QTR_SIZE_HEIGHT-y;
    
    // SNR Block [Vendor recommended value ##Don't change##]
    siv121a_i2c_write_byte(0x00,0x00);
    siv121a_i2c_write_byte(0x03,0x04); //0x04
    siv121a_i2c_write_byte(0x10,0x83); 
    //siv121a_i2c_write_byte(0x13,0x17);
    //siv121a_i2c_write_byte(0x15,0x22); 

    siv121a_i2c_write_byte(0x00,0x01);
    siv121a_i2c_write_byte(0x04,0x00);
    siv121a_i2c_write_byte(0x05,0x06);
    siv121a_i2c_write_byte(0x11,0x25);
    siv121a_i2c_write_byte(0x12,0x21); 
    siv121a_i2c_write_byte(0x13,0x15);
    siv121a_i2c_write_byte(0x15,0xC2); 
    
    //50HZ 12MHZ 12.5FPS
    siv121a_i2c_write_byte(0x20,0x00); 
    siv121a_i2c_write_byte(0x21,0x00);
    siv121a_i2c_write_byte(0x22,0x00);
    siv121a_i2c_write_byte(0x23,0x00);
    siv121a_i2c_write_byte(0x24,0x00); 
    siv121a_i2c_write_byte(0x25,0x00);
    siv121a_i2c_write_byte(0x26,0x00);
    siv121a_i2c_write_byte(0x27,0x00);
    
    siv121a_i2c_write_byte(0x00,0x02);
    siv121a_i2c_write_byte(0x34,0x42);
    siv121a_i2c_write_byte(0x11,0x08); 
    siv121a_i2c_write_byte(0x12,0x66); 
    siv121a_i2c_write_byte(0x13,0x66);
    siv121a_i2c_write_byte(0x14,0x66);
    siv121a_i2c_write_byte(0x1e,0x00);
    
    siv121a_i2c_write_byte(0x40,0x30);
    siv121a_i2c_write_byte(0x41,0x20);
    siv121a_i2c_write_byte(0x42,0x20);
    siv121a_i2c_write_byte(0x43,0x00);
    siv121a_i2c_write_byte(0x44,0x00);
    siv121a_i2c_write_byte(0x45,0x00); 
    siv121a_i2c_write_byte(0x46,0x0A);
    siv121a_i2c_write_byte(0x47,0x10);
    siv121a_i2c_write_byte(0x48,0x13);
    siv121a_i2c_write_byte(0x49,0x15);
    siv121a_i2c_write_byte(0x4A,0x18);
    siv121a_i2c_write_byte(0x4B,0x1A);
    siv121a_i2c_write_byte(0x4C,0x1D);
    siv121a_i2c_write_byte(0x4D,0x20);
    siv121a_i2c_write_byte(0x4E,0x10);
    siv121a_i2c_write_byte(0x4F,0x0A);
    siv121a_i2c_write_byte(0x50,0x08);
    siv121a_i2c_write_byte(0x51,0x06);
    siv121a_i2c_write_byte(0x52,0x05);
    siv121a_i2c_write_byte(0x53,0x04);
    siv121a_i2c_write_byte(0x54,0x02);
    siv121a_i2c_write_byte(0x55,0x01);

    //AWB
    siv121a_i2c_write_byte(0x00,0x03);
    siv121a_i2c_write_byte(0x10,0xc3);//d0
    siv121a_i2c_write_byte(0x11,0xC0);
    siv121a_i2c_write_byte(0x12,0x80);
    siv121a_i2c_write_byte(0x13,0x7f);
    siv121a_i2c_write_byte(0x14,0x7f);
    siv121a_i2c_write_byte(0x15,0xFE); 
    siv121a_i2c_write_byte(0x16,0x87);
    siv121a_i2c_write_byte(0x17,0xc8);
    siv121a_i2c_write_byte(0x18,0x70);
    siv121a_i2c_write_byte(0x19,0x94);
    siv121a_i2c_write_byte(0x1A,0x6c);
    siv121a_i2c_write_byte(0x1B,0x94);
    siv121a_i2c_write_byte(0x1C,0x6c);
    siv121a_i2c_write_byte(0x1D,0x94);
    siv121a_i2c_write_byte(0x1E,0x6c);
    siv121a_i2c_write_byte(0x20,0xE8);
    siv121a_i2c_write_byte(0x21,0x30);
    siv121a_i2c_write_byte(0x22,0xA4);
    siv121a_i2c_write_byte(0x23,0x20);
    siv121a_i2c_write_byte(0x25,0x10);
    siv121a_i2c_write_byte(0x26,0x0F);
    siv121a_i2c_write_byte(0x27,0x10);
    siv121a_i2c_write_byte(0x28,0x20);
    siv121a_i2c_write_byte(0x29,0xa9);
    siv121a_i2c_write_byte(0x2A,0x8c);

    siv121a_i2c_write_byte(0x30,0x00);
    siv121a_i2c_write_byte(0x31,0x10);
    siv121a_i2c_write_byte(0x32,0x00);
    siv121a_i2c_write_byte(0x33,0x10);
    siv121a_i2c_write_byte(0x34,0x02);
    siv121a_i2c_write_byte(0x35,0x76);
    siv121a_i2c_write_byte(0x36,0x01);
    siv121a_i2c_write_byte(0x37,0xD6);
    siv121a_i2c_write_byte(0x40,0x01);
    siv121a_i2c_write_byte(0x41,0x04);
    siv121a_i2c_write_byte(0x42,0x08);
    siv121a_i2c_write_byte(0x43,0x10);
    siv121a_i2c_write_byte(0x44,0x12);
    siv121a_i2c_write_byte(0x45,0x35);
    siv121a_i2c_write_byte(0x46,0x64);
    siv121a_i2c_write_byte(0x50,0x33);
    siv121a_i2c_write_byte(0x51,0x20);
    siv121a_i2c_write_byte(0x52,0xE5);
    siv121a_i2c_write_byte(0x53,0xFB);
    siv121a_i2c_write_byte(0x54,0x13);
    siv121a_i2c_write_byte(0x55,0x26);
    siv121a_i2c_write_byte(0x56,0x07);
    siv121a_i2c_write_byte(0x57,0xF5);
    siv121a_i2c_write_byte(0x58,0xEA);
    siv121a_i2c_write_byte(0x59,0x21);

    //CMA change  -D65~A
    siv121a_i2c_write_byte(0x63,0x98); //D30 to D20 for R
    siv121a_i2c_write_byte(0x64,0xa8); //D30 to D20 for B
    siv121a_i2c_write_byte(0x65,0x98); //D20 to D30 for R
    siv121a_i2c_write_byte(0x66,0xa8); //D20 to D30 for B
    siv121a_i2c_write_byte(0x67,0xc8); //D65 to D30 for R
    siv121a_i2c_write_byte(0x68,0x9f); //D65 to D30 for B
    siv121a_i2c_write_byte(0x69,0xc8); //D30 to D65 for R
    siv121a_i2c_write_byte(0x6A,0x9f); //D30 to D65 for B

    // IDP
    siv121a_i2c_write_byte(0x00,0x04);
    siv121a_i2c_write_byte(0x10,0xFF); // IDP function enable
    siv121a_i2c_write_byte(0x11,0x1D); //0x1D // PCLK polarity
#ifdef SIV121A_OUTFORMAT_RGB565
    siv121a_i2c_write_byte(0x12,0x0B);
#else
    siv121a_i2c_write_byte(0x12,0xAD); // Y,Cb,Cr order sequence
#endif
    // DPCNR
    siv121a_i2c_write_byte(0x17,0x98);
    siv121a_i2c_write_byte(0x18,0x00);
    siv121a_i2c_write_byte(0x19,0x80); //0X40  20091130
    siv121a_i2c_write_byte(0x1A,0x40);
    siv121a_i2c_write_byte(0x1B,0x12);
    siv121a_i2c_write_byte(0x1C,0x00);
    siv121a_i2c_write_byte(0x1D,0x4f);
    siv121a_i2c_write_byte(0x1E,0x4f);
    siv121a_i2c_write_byte(0x1F,0x2F);
    siv121a_i2c_write_byte(0x20,0x04); // Normal illumiinfo start
    siv121a_i2c_write_byte(0x21,0x0F); // Dark illumiinfo start

    // Gamma
    siv121a_i2c_write_byte(0x30,0x00);
    siv121a_i2c_write_byte(0x31,0x02);
    siv121a_i2c_write_byte(0x32,0x06);
    siv121a_i2c_write_byte(0x33,0x1c);
    siv121a_i2c_write_byte(0x34,0x42);
    siv121a_i2c_write_byte(0x35,0x60);
    siv121a_i2c_write_byte(0x36,0x79);
    siv121a_i2c_write_byte(0x37,0x8c);
    siv121a_i2c_write_byte(0x38,0x9B);
    siv121a_i2c_write_byte(0x39,0xAA);
    siv121a_i2c_write_byte(0x3A,0xb6);
    siv121a_i2c_write_byte(0x3B,0xca);
    siv121a_i2c_write_byte(0x3C,0xdc);
    siv121a_i2c_write_byte(0x3D,0xef);
    siv121a_i2c_write_byte(0x3E,0xf8);
    siv121a_i2c_write_byte(0x3F,0xFF);

    // Shading
    siv121a_i2c_write_byte(0x40,0x11);
    siv121a_i2c_write_byte(0x41,0x21);
    siv121a_i2c_write_byte(0x42,0x32);
    siv121a_i2c_write_byte(0x43,0x43);
    siv121a_i2c_write_byte(0x44,0x55);
    siv121a_i2c_write_byte(0x45,0x55);                                                  
    siv121a_i2c_write_byte(0x46,0x22);   // left R gain[7:4], right R gain[3:0] 0x22        
    siv121a_i2c_write_byte(0x47,0x23);   // top R gain[7:4], bottom R gain[3:0]             
    siv121a_i2c_write_byte(0x48,0x11);   // left Gr gain[7:4], right Gr gain[3:0]           
    siv121a_i2c_write_byte(0x49,0x12);   // top Gr gain[7:4], bottom Gr gain[3:0]           
    siv121a_i2c_write_byte(0x4a,0x01);   // left Gb gain[7:4], right Gb gain[3:0]           
    siv121a_i2c_write_byte(0x4b,0x12);   // top Gb gain[7:4], bottom Gb gain[3:0]           
    siv121a_i2c_write_byte(0x4c,0x01);   // left B gain[7:4], right B gain[3:0]             
    siv121a_i2c_write_byte(0x4d,0x11);   // top B gain[7:4], bottom B gain[3:0]             
    siv121a_i2c_write_byte(0x4e,0x00);   // X-axis center high[3:2], Y-axis center high[1:0]
    siv121a_i2c_write_byte(0x4f,0xa0);   // X-axis center low[7:0]                          
    siv121a_i2c_write_byte(0x50,0x80);   // Y-axis center low[7:0]                          
    siv121a_i2c_write_byte(0x51,0x80);   // Shading Center Gain                             
    siv121a_i2c_write_byte(0x52,0x00);   // Shading R Offset                                
    siv121a_i2c_write_byte(0x53,0x00);   // Shading Gr Offset                               
    siv121a_i2c_write_byte(0x54,0x00);   // Shading Gb Offset                               
    siv121a_i2c_write_byte(0x55,0x00);   // Shading B Offset  

    // Lowlux Shading
    siv121a_i2c_write_byte(0x56,0x10);
    siv121a_i2c_write_byte(0x57,0x92);
    siv121a_i2c_write_byte(0x58,0x00);

    //Filter
    siv121a_i2c_write_byte(0x61,0xB4);
    siv121a_i2c_write_byte(0x62,0xB4);
    siv121a_i2c_write_byte(0x63,0xB4);

    // Color Matrix (D65) - Daylight
    siv121a_i2c_write_byte(0x71,0x42);
    siv121a_i2c_write_byte(0x72,0xbf);
    siv121a_i2c_write_byte(0x73,0x00);
    siv121a_i2c_write_byte(0x74,0x0F);
    siv121a_i2c_write_byte(0x75,0x31);
    siv121a_i2c_write_byte(0x76,0x00);
    siv121a_i2c_write_byte(0x77,0x00);
    siv121a_i2c_write_byte(0x78,0xbc);
    siv121a_i2c_write_byte(0x79,0x44);

    // Color Matrix (D30) - CWF
    siv121a_i2c_write_byte(0x7A,0x39);
    siv121a_i2c_write_byte(0x7B,0xCd);
    siv121a_i2c_write_byte(0x7C,0xFa);
    siv121a_i2c_write_byte(0x7D,0x13);
    siv121a_i2c_write_byte(0x7E,0x2c);
    siv121a_i2c_write_byte(0x7F,0x02);
    siv121a_i2c_write_byte(0x80,0xF9);
    siv121a_i2c_write_byte(0x81,0xC6);
    siv121a_i2c_write_byte(0x82,0x41);

    // Color Matrix (D20) - A
    siv121a_i2c_write_byte(0x83,0x38);
    siv121a_i2c_write_byte(0x84,0xbe);
    siv121a_i2c_write_byte(0x85,0x0a);
    siv121a_i2c_write_byte(0x86,0x13);
    siv121a_i2c_write_byte(0x87,0x1e);
    siv121a_i2c_write_byte(0x88,0x0F);
    siv121a_i2c_write_byte(0x89,0xF3);
    siv121a_i2c_write_byte(0x8A,0xcc);
    siv121a_i2c_write_byte(0x8B,0x41);
    siv121a_i2c_write_byte(0x8c,0x10);
                          
    // Edge - Green
    siv121a_i2c_write_byte(0x90,0x12); //Up Gain
    siv121a_i2c_write_byte(0x91,0x12); //Down Gain
    siv121a_i2c_write_byte(0x92,0x00);
    siv121a_i2c_write_byte(0x96,0x00);

    // Edge - Luminance
    siv121a_i2c_write_byte(0x9F,0x18);
    siv121a_i2c_write_byte(0xA0,0x18);
    siv121a_i2c_write_byte(0xA1,0x02);
    siv121a_i2c_write_byte(0xA2,0x02);
    siv121a_i2c_write_byte(0xA3,0x0a);
    siv121a_i2c_write_byte(0xA4,0x0a);
  
    siv121a_i2c_write_byte(0xA8,0x10);
    siv121a_i2c_write_byte(0xA9,0x0f);
    siv121a_i2c_write_byte(0xAA,0x12);
    siv121a_i2c_write_byte(0xAB,0x82);
    siv121a_i2c_write_byte(0xAe,0x40);
    siv121a_i2c_write_byte(0xAf,0x86);

    // Color suppress
    siv121a_i2c_write_byte(0xB9,0x10); //Ilimininfo Start
    siv121a_i2c_write_byte(0xBA,0x30); //Slope

    siv121a_i2c_write_byte(0xc0,(byte)((x>>8)<<6)|((dx>>8)<<4)|((y>>8)<<3)|((dy>>8)<<2));//10 40 F0 QVGA WINDOW 00 A0 80 160*128
    siv121a_i2c_write_byte(0xc1,(byte)x);
    siv121a_i2c_write_byte(0xc2,(byte)dx);
    siv121a_i2c_write_byte(0xc3,(byte)y);
    siv121a_i2c_write_byte(0xc4,(byte)dy);

    siv121a_i2c_write_byte(0xDD,0x0F); // ENHCTRL
    siv121a_i2c_write_byte(0xDE,0xfA); // NOIZCTRL

    siv121a_i2c_write_byte(0xDF,0x1F);
    siv121a_i2c_write_byte(0xE0,0x60);
    siv121a_i2c_write_byte(0xE1,0x90);
    siv121a_i2c_write_byte(0xE2,0x08);
    siv121a_i2c_write_byte(0xE3,0x08);

    // Memory speed control
    siv121a_i2c_write_byte(0xE5,0x15);
    siv121a_i2c_write_byte(0xE6,0x28);
    siv121a_i2c_write_byte(0xE7,0x04);

    
    //siv121a_i2c_write_byte(0xE8,0x08); // Test Mode

    // AE on
    siv121a_i2c_write_byte(0x00,0x02);
    siv121a_i2c_write_byte(0x10,0x80);

    siv121a_i2c_write_byte(0x70,0xd4);   //RBNCTRL
    siv121a_i2c_write_byte(0x74,0x07);
    siv121a_i2c_write_byte(0x79,0x6f);

    // Sensor on
    siv121a_i2c_write_byte(0x00,0x01);
    siv121a_i2c_write_byte(0x03,0x01);

    /*Customer can adjust GAMMA, MIRROR & UPSIDEDOWN here!*/
    return TRUE;
} /* end of initialize_siv121a_registers_preview. */
#endif

static boolean initialize_siv121a_registers(uint16 dx, uint16 dy)
{
    uint16 x,y;
    
    if(dx > CAMSENSOR_SIV121A_FULL_SIZE_WIDTH)
    {
        dx = CAMSENSOR_SIV121A_FULL_SIZE_WIDTH;
    }
    
    if(dy > CAMSENSOR_SIV121A_FULL_SIZE_HEIGHT)
    {
        dy = CAMSENSOR_SIV121A_FULL_SIZE_HEIGHT;
    }
    
    x = (CAMSENSOR_SIV121A_FULL_SIZE_WIDTH-dx)>>1;
    y = (CAMSENSOR_SIV121A_FULL_SIZE_HEIGHT-dy)>>1;

    dy = CAMSENSOR_SIV121A_FULL_SIZE_HEIGHT-y;
    
    // SNR Block [Vendor recommended value ##Don't change##]
    siv121a_i2c_write_byte(0x00,0x00);
    siv121a_i2c_write_byte(0x03,0x04); //0x04
    siv121a_i2c_write_byte(0x10,0x83); 
    //siv121a_i2c_write_byte(0x13,0x17);
    //siv121a_i2c_write_byte(0x15,0x22); 

    siv121a_i2c_write_byte(0x00,0x01);
    siv121a_i2c_write_byte(0x04,0x10);
    siv121a_i2c_write_byte(0x05,0x02);
    siv121a_i2c_write_byte(0x11,0x25);
    siv121a_i2c_write_byte(0x12,0x21); 
    siv121a_i2c_write_byte(0x13,0x15);
    siv121a_i2c_write_byte(0x15,0xC2); 
    
    //50HZ 12MHZ 12.5FPS
    siv121a_i2c_write_byte(0x20,0x00); 
    siv121a_i2c_write_byte(0x21,0x00);
    siv121a_i2c_write_byte(0x22,0x00);
    siv121a_i2c_write_byte(0x23,0x00);
    siv121a_i2c_write_byte(0x24,0x00); 
    siv121a_i2c_write_byte(0x25,0x00);
    siv121a_i2c_write_byte(0x26,0x00);
    siv121a_i2c_write_byte(0x27,0x00);
    
    siv121a_i2c_write_byte(0x00,0x02);
    siv121a_i2c_write_byte(0x34,0x42);
    siv121a_i2c_write_byte(0x11,0x08); 
    siv121a_i2c_write_byte(0x12,0x66); 
    siv121a_i2c_write_byte(0x13,0x66);
    siv121a_i2c_write_byte(0x14,0x66);
    siv121a_i2c_write_byte(0x1e,0x00);
    
    siv121a_i2c_write_byte(0x40,0x30);
    siv121a_i2c_write_byte(0x41,0x20);
    siv121a_i2c_write_byte(0x42,0x20);
    siv121a_i2c_write_byte(0x43,0x00);
    siv121a_i2c_write_byte(0x44,0x00);
    siv121a_i2c_write_byte(0x45,0x00); 
    siv121a_i2c_write_byte(0x46,0x0A);
    siv121a_i2c_write_byte(0x47,0x10);
    siv121a_i2c_write_byte(0x48,0x13);
    siv121a_i2c_write_byte(0x49,0x15);
    siv121a_i2c_write_byte(0x4A,0x18);
    siv121a_i2c_write_byte(0x4B,0x1A);
    siv121a_i2c_write_byte(0x4C,0x1D);
    siv121a_i2c_write_byte(0x4D,0x20);
    siv121a_i2c_write_byte(0x4E,0x10);
    siv121a_i2c_write_byte(0x4F,0x0A);
    siv121a_i2c_write_byte(0x50,0x08);
    siv121a_i2c_write_byte(0x51,0x06);
    siv121a_i2c_write_byte(0x52,0x05);
    siv121a_i2c_write_byte(0x53,0x04);
    siv121a_i2c_write_byte(0x54,0x02);
    siv121a_i2c_write_byte(0x55,0x01);

    //AWB
    siv121a_i2c_write_byte(0x00,0x03);
    siv121a_i2c_write_byte(0x10,0xc3);//d0
    siv121a_i2c_write_byte(0x11,0xC0);
    siv121a_i2c_write_byte(0x12,0x80);
    siv121a_i2c_write_byte(0x13,0x7f);
    siv121a_i2c_write_byte(0x14,0x7f);
    siv121a_i2c_write_byte(0x15,0xFE); 
    siv121a_i2c_write_byte(0x16,0x87);
    siv121a_i2c_write_byte(0x17,0xc8);
    siv121a_i2c_write_byte(0x18,0x70);
    siv121a_i2c_write_byte(0x19,0x94);
    siv121a_i2c_write_byte(0x1A,0x6c);
    siv121a_i2c_write_byte(0x1B,0x94);
    siv121a_i2c_write_byte(0x1C,0x6c);
    siv121a_i2c_write_byte(0x1D,0x94);
    siv121a_i2c_write_byte(0x1E,0x6c);
    siv121a_i2c_write_byte(0x20,0xE8);
    siv121a_i2c_write_byte(0x21,0x30);
    siv121a_i2c_write_byte(0x22,0xA4);
    siv121a_i2c_write_byte(0x23,0x20);
    siv121a_i2c_write_byte(0x25,0x10);
    siv121a_i2c_write_byte(0x26,0x0F);
    siv121a_i2c_write_byte(0x27,0x10);
    siv121a_i2c_write_byte(0x28,0x20);
    siv121a_i2c_write_byte(0x29,0xa9);
    siv121a_i2c_write_byte(0x2A,0x8c);

    siv121a_i2c_write_byte(0x30,0x00);
    siv121a_i2c_write_byte(0x31,0x10);
    siv121a_i2c_write_byte(0x32,0x00);
    siv121a_i2c_write_byte(0x33,0x10);
    siv121a_i2c_write_byte(0x34,0x02);
    siv121a_i2c_write_byte(0x35,0x76);
    siv121a_i2c_write_byte(0x36,0x01);
    siv121a_i2c_write_byte(0x37,0xD6);
    siv121a_i2c_write_byte(0x40,0x01);
    siv121a_i2c_write_byte(0x41,0x04);
    siv121a_i2c_write_byte(0x42,0x08);
    siv121a_i2c_write_byte(0x43,0x10);
    siv121a_i2c_write_byte(0x44,0x12);
    siv121a_i2c_write_byte(0x45,0x35);
    siv121a_i2c_write_byte(0x46,0x64);
    siv121a_i2c_write_byte(0x50,0x33);
    siv121a_i2c_write_byte(0x51,0x20);
    siv121a_i2c_write_byte(0x52,0xE5);
    siv121a_i2c_write_byte(0x53,0xFB);
    siv121a_i2c_write_byte(0x54,0x13);
    siv121a_i2c_write_byte(0x55,0x26);
    siv121a_i2c_write_byte(0x56,0x07);
    siv121a_i2c_write_byte(0x57,0xF5);
    siv121a_i2c_write_byte(0x58,0xEA);
    siv121a_i2c_write_byte(0x59,0x21);

    //CMA change  -D65~A
    siv121a_i2c_write_byte(0x63,0x98); //D30 to D20 for R
    siv121a_i2c_write_byte(0x64,0xa8); //D30 to D20 for B
    siv121a_i2c_write_byte(0x65,0x98); //D20 to D30 for R
    siv121a_i2c_write_byte(0x66,0xa8); //D20 to D30 for B
    siv121a_i2c_write_byte(0x67,0xc8); //D65 to D30 for R
    siv121a_i2c_write_byte(0x68,0x9f); //D65 to D30 for B
    siv121a_i2c_write_byte(0x69,0xc8); //D30 to D65 for R
    siv121a_i2c_write_byte(0x6A,0x9f); //D30 to D65 for B

    // IDP
    siv121a_i2c_write_byte(0x00,0x04);
    siv121a_i2c_write_byte(0x10,0xFF); // IDP function enable
    siv121a_i2c_write_byte(0x11,0x1D); //0x1D // PCLK polarity
#ifdef SIV121A_OUTFORMAT_RGB565
    siv121a_i2c_write_byte(0x12,0x0B);
#else
    siv121a_i2c_write_byte(0x12,0xAD); // Y,Cb,Cr order sequence
#endif
    
    // DPCNR
    siv121a_i2c_write_byte(0x17,0x98);
    siv121a_i2c_write_byte(0x18,0x00);
    siv121a_i2c_write_byte(0x19,0x80); //0X40  20091130
    siv121a_i2c_write_byte(0x1A,0x40);
    siv121a_i2c_write_byte(0x1B,0x12);
    siv121a_i2c_write_byte(0x1C,0x00);
    siv121a_i2c_write_byte(0x1D,0x4f);
    siv121a_i2c_write_byte(0x1E,0x4f);
    siv121a_i2c_write_byte(0x1F,0x2F);
    siv121a_i2c_write_byte(0x20,0x04); // Normal illumiinfo start
    siv121a_i2c_write_byte(0x21,0x0F); // Dark illumiinfo start

    // Gamma
    siv121a_i2c_write_byte(0x30,0x00);
    siv121a_i2c_write_byte(0x31,0x02);
    siv121a_i2c_write_byte(0x32,0x06);
    siv121a_i2c_write_byte(0x33,0x1c);
    siv121a_i2c_write_byte(0x34,0x42);
    siv121a_i2c_write_byte(0x35,0x60);
    siv121a_i2c_write_byte(0x36,0x79);
    siv121a_i2c_write_byte(0x37,0x8c);
    siv121a_i2c_write_byte(0x38,0x9B);
    siv121a_i2c_write_byte(0x39,0xAA);
    siv121a_i2c_write_byte(0x3A,0xb6);
    siv121a_i2c_write_byte(0x3B,0xca);
    siv121a_i2c_write_byte(0x3C,0xdc);
    siv121a_i2c_write_byte(0x3D,0xef);
    siv121a_i2c_write_byte(0x3E,0xf8);
    siv121a_i2c_write_byte(0x3F,0xFF);

    // Shading
    siv121a_i2c_write_byte(0x40,0x11);
    siv121a_i2c_write_byte(0x41,0x21);
    siv121a_i2c_write_byte(0x42,0x32);
    siv121a_i2c_write_byte(0x43,0x43);
    siv121a_i2c_write_byte(0x44,0x55);
    siv121a_i2c_write_byte(0x45,0x55);                                                  
    siv121a_i2c_write_byte(0x46,0x22);   // left R gain[7:4], right R gain[3:0] 0x22        
    siv121a_i2c_write_byte(0x47,0x23);   // top R gain[7:4], bottom R gain[3:0]             
    siv121a_i2c_write_byte(0x48,0x11);   // left Gr gain[7:4], right Gr gain[3:0]           
    siv121a_i2c_write_byte(0x49,0x12);   // top Gr gain[7:4], bottom Gr gain[3:0]           
    siv121a_i2c_write_byte(0x4a,0x01);   // left Gb gain[7:4], right Gb gain[3:0]           
    siv121a_i2c_write_byte(0x4b,0x12);   // top Gb gain[7:4], bottom Gb gain[3:0]           
    siv121a_i2c_write_byte(0x4c,0x01);   // left B gain[7:4], right B gain[3:0]             
    siv121a_i2c_write_byte(0x4d,0x11);   // top B gain[7:4], bottom B gain[3:0]             
    siv121a_i2c_write_byte(0x4e,0x00);   // X-axis center high[3:2], Y-axis center high[1:0]
    siv121a_i2c_write_byte(0x4f,0xa0);   // X-axis center low[7:0]                          
    siv121a_i2c_write_byte(0x50,0x80);   // Y-axis center low[7:0]                          
    siv121a_i2c_write_byte(0x51,0x80);   // Shading Center Gain                                
    siv121a_i2c_write_byte(0x52,0x00);   // Shading R Offset                                
    siv121a_i2c_write_byte(0x53,0x00);   // Shading Gr Offset                               
    siv121a_i2c_write_byte(0x54,0x00);   // Shading Gb Offset                               
    siv121a_i2c_write_byte(0x55,0x00);   // Shading B Offset  

    // Lowlux Shading
    siv121a_i2c_write_byte(0x56,0x10);
    siv121a_i2c_write_byte(0x57,0x92);
    siv121a_i2c_write_byte(0x58,0x00);

    //Filter
    siv121a_i2c_write_byte(0x61,0xB4);
    siv121a_i2c_write_byte(0x62,0xB4);
    siv121a_i2c_write_byte(0x63,0xB4);

    // Color Matrix (D65) - Daylight
    siv121a_i2c_write_byte(0x71,0x42);
    siv121a_i2c_write_byte(0x72,0xbf);
    siv121a_i2c_write_byte(0x73,0x00);
    siv121a_i2c_write_byte(0x74,0x0F);
    siv121a_i2c_write_byte(0x75,0x31);
    siv121a_i2c_write_byte(0x76,0x00);
    siv121a_i2c_write_byte(0x77,0x00);
    siv121a_i2c_write_byte(0x78,0xbc);
    siv121a_i2c_write_byte(0x79,0x44);

    // Color Matrix (D30) - CWF
    siv121a_i2c_write_byte(0x7A,0x39);
    siv121a_i2c_write_byte(0x7B,0xCd);
    siv121a_i2c_write_byte(0x7C,0xFa);
    siv121a_i2c_write_byte(0x7D,0x13);
    siv121a_i2c_write_byte(0x7E,0x2c);
    siv121a_i2c_write_byte(0x7F,0x02);
    siv121a_i2c_write_byte(0x80,0xF9);
    siv121a_i2c_write_byte(0x81,0xC6);
    siv121a_i2c_write_byte(0x82,0x41);

    // Color Matrix (D20) - A
    siv121a_i2c_write_byte(0x83,0x38);
    siv121a_i2c_write_byte(0x84,0xbe);
    siv121a_i2c_write_byte(0x85,0x0a);
    siv121a_i2c_write_byte(0x86,0x13);
    siv121a_i2c_write_byte(0x87,0x1e);
    siv121a_i2c_write_byte(0x88,0x0F);
    siv121a_i2c_write_byte(0x89,0xF3);
    siv121a_i2c_write_byte(0x8A,0xcc);
    siv121a_i2c_write_byte(0x8B,0x41);
    siv121a_i2c_write_byte(0x8c,0x10);
                          
    // Edge - Green
    siv121a_i2c_write_byte(0x90,0x12); //Up Gain
    siv121a_i2c_write_byte(0x91,0x12); //Down Gain
    siv121a_i2c_write_byte(0x92,0x00);
    siv121a_i2c_write_byte(0x96,0x00);

    // Edge - Luminance
    siv121a_i2c_write_byte(0x9F,0x18);
    siv121a_i2c_write_byte(0xA0,0x18);
    siv121a_i2c_write_byte(0xA1,0x02);
    siv121a_i2c_write_byte(0xA2,0x02);
    siv121a_i2c_write_byte(0xA3,0x0a);
    siv121a_i2c_write_byte(0xA4,0x0a);
  
    siv121a_i2c_write_byte(0xA8,0x10);
    siv121a_i2c_write_byte(0xA9,0x0f);
    siv121a_i2c_write_byte(0xAA,0x12);
    siv121a_i2c_write_byte(0xAB,0x82);
    siv121a_i2c_write_byte(0xAe,0x40);
    siv121a_i2c_write_byte(0xAf,0x86);

    // Color suppress
    siv121a_i2c_write_byte(0xB9,0x10); //Ilimininfo Start
    siv121a_i2c_write_byte(0xBA,0x30); //Slope

    siv121a_i2c_write_byte(0xc0,(byte)((x>>8)<<6)|((dx>>8)<<4)|((y>>8)<<3)|((dy>>8)<<2));//10 40 F0 QVGA WINDOW 00 A0 80 160*128
    siv121a_i2c_write_byte(0xc1,(byte)x);
    siv121a_i2c_write_byte(0xc2,(byte)dx);
    siv121a_i2c_write_byte(0xc3,(byte)y);
    siv121a_i2c_write_byte(0xc4,(byte)dy);

    siv121a_i2c_write_byte(0xDD,0x0F); // ENHCTRL
    siv121a_i2c_write_byte(0xDE,0xfA); // NOIZCTRL

    siv121a_i2c_write_byte(0xDF,0x1F);
    siv121a_i2c_write_byte(0xE0,0x60);
    siv121a_i2c_write_byte(0xE1,0x90);
    siv121a_i2c_write_byte(0xE2,0x08);
    siv121a_i2c_write_byte(0xE3,0x08);

    // Memory speed control
    siv121a_i2c_write_byte(0xE5,0x15);
    siv121a_i2c_write_byte(0xE6,0x28);
    siv121a_i2c_write_byte(0xE7,0x04);

    
    //siv121a_i2c_write_byte(0xE8,0x08); // Test Mode

    // AE on
    siv121a_i2c_write_byte(0x00,0x02);
    siv121a_i2c_write_byte(0x10,0x80);

    siv121a_i2c_write_byte(0x70,0xd4);   //RBNCTRL
    siv121a_i2c_write_byte(0x74,0x07);
    siv121a_i2c_write_byte(0x79,0x6f);

    // Sensor on
    siv121a_i2c_write_byte(0x00,0x01);
    siv121a_i2c_write_byte(0x03,0x01);

    /*Customer can adjust GAMMA, MIRROR & UPSIDEDOWN here!*/
    return TRUE;
} /* end of initialize_siv121a_registers. */

static boolean camsensor_siv121a_start( camsensor_static_params_type *camsensor_params)
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
    camsensor_params->sensor_model  = CAMSENSOR_SIV121A_ID;
    //use_camsensor_siv121a = TRUE;
    //use_camsensor_sic110a = FALSE;
    /* CCD or CMOS */
    camsensor_params->sensor_type   = CAMSENSOR_CMOS;

    /* BAYER or YCbCr */
    camsensor_params->output_format = CAMSENSOR_YCBCR;

    /* A pointer to the sensor name for EXIF tags */
    camsensor_params->sensor_name = camsensor_siv121a_sensor_name;

    /* Pointer to sensor suffix */

    camsensor_params->sensor_suffix=camsensor_siv121a_sensor_suffix;

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
    camsensor_params->full_size_width  = CAMSENSOR_SIV121A_FULL_SIZE_WIDTH;
    camsensor_params->full_size_height = CAMSENSOR_SIV121A_FULL_SIZE_HEIGHT;
    
    camsensor_params->qtr_size_width  =  CAMSENSOR_SIV121A_QTR_SIZE_WIDTH;
    camsensor_params->qtr_size_height =  CAMSENSOR_SIV121A_QTR_SIZE_HEIGHT;
    camsensor_params->pclk_invert     =  camsensor_info.pclk_invert;
    
#if defined FEATURE_WHITE_LED_FLASH || defined FEATURE_STROBE_FLASH
    camsensor_params->support_auto_flash = FALSE;
#else
    camsensor_params->support_auto_flash = FALSE;
#endif

    return TRUE;
} /*camsensor_siv121a_start*/

static boolean siv121a_i2c_write_byte(uint8 offset, uint8 data) 
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

static boolean siv121a_i2c_read_byte(uint8 offset, uint8 *data) 
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
} /* end of siv121a_i2c_read_word. */

static camera_ret_code_type camsensor_siv121a_set_effect(int8 effect)
{
    return CAMERA_NOT_SUPPORTED;
} /* camsensor_siv121a_set_effect() */

/*===========================================================================

FUNCTION      camsensor_siv121a_snapshot_config

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

static boolean camsensor_siv121a_snapshot_config( camsensor_static_params_type  *camsensor_params)
{
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
    initialize_siv121a_registers(camera_dx, camera_dy);
    return TRUE;
}
/*===========================================================================

FUNCTION      camsensor_siv121a_raw_snapshot_config

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
static boolean camsensor_siv121a_raw_snapshot_config(camsensor_static_params_type  *camsensor_params)
{
    ERR("camsensor_siv121a_raw_snapshot_config!",0,0,0);
    return TRUE;
}

/*===========================================================================

FUNCTION      camsensor_siv121a_video_config

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
static boolean camsensor_siv121a_video_config(camsensor_static_params_type *camsensor_params)
{
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
    
#ifdef CAMSENSOR_HIGHQUALITY_PREVIEW
    initialize_siv121a_registers(camera_preview_dx, camera_preview_dy);
#else
    initialize_siv121a_registers_preview(camera_preview_dx, camera_preview_dy);
#endif
    return TRUE;
}


static void camsensor_siv121a_power_up(void)
{
    ERR("camsensor_siv121a_power_up!",0,0,0);
} /* camsensor_siv121a_power_up */

/*===========================================================================

FUNCTION      camsensor_siv121a_power_down

DESCRIPTION
Power down the sensor, using I2C driver.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void camsensor_siv121a_power_down(void)
{
    siv121a_i2c_write_byte(0x00, 0x00);
    siv121a_i2c_write_byte(0x03, 0x02); //sensor sleep mode
    ERR("camsensor_siv121a_power_down!",0,0,0);
} /* camsensor_siv121a_power_down */

static camera_ret_code_type camsensor_siv121a_set_wb(int8 wb)
{
    siv121a_i2c_write_byte(0x00,0x03); //bank 3
        
    ERR("camsensor_siv121a_set_wb!",0,0,0);
    switch((camera_wb_type)wb)
    {
        case CAMERA_WB_AUTO:
        {
            siv121a_i2c_write_byte(0x10, 0xD3);
            break;
        }
        
        case CAMERA_WB_INCANDESCENT:
        {
            siv121a_i2c_write_byte(0x10, 0x00);  // disable AWB
            siv121a_i2c_write_byte(0x60, 0x98);
            siv121a_i2c_write_byte(0x61, 0xC8);
            break;
        }
        
        case CAMERA_WB_FLUORESCENT:
        {
            siv121a_i2c_write_byte(0x10, 0x00);  // disable AWB
            siv121a_i2c_write_byte(0x60, 0xAA);
            siv121a_i2c_write_byte(0x61, 0xBE);
            break;
        }
        
        case CAMERA_WB_DAYLIGHT:
        {
            siv121a_i2c_write_byte(0x10, 0x00);  // disable AWB
            siv121a_i2c_write_byte(0x60, 0xC2);
            siv121a_i2c_write_byte(0x61, 0x9E);
            break;
        }
        
        case CAMERA_WB_CLOUDY_DAYLIGHT:
        {
            siv121a_i2c_write_byte(0x10, 0x00);  // disable AWB
            siv121a_i2c_write_byte(0x60, 0xD0);
            siv121a_i2c_write_byte(0x61, 0x88);
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

static camera_ret_code_type camsensor_siv121a_set_ev_compensation(int32 compensation)
{
    camera_ret_code_type ret_val = CAMERA_SUCCESS;
    int16  parmCurNum = (int16)(compensation>>16);
    int32 max_parmVal = 12;
    int32 min_parmVal =-12;
    int32  parmVal= parmCurNum;
    uint32 new_luma_target;

    ERR("camsensor_siv121a_set_ev_compensation!",0,0,0);
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
        siv121a_i2c_write_byte(0x00,0x02); //bank 1
        switch(new_luma_target)
        {
            case 8:    // -4 EV
                siv121a_i2c_write_byte(0x12, 0x40);
                siv121a_i2c_write_byte(0x13, 0x40);
                siv121a_i2c_write_byte(0x14, 0x40);
                siv121a_i2c_write_byte(0x70, 0xd4);
                break;

            case 9:    // -3 EV
                siv121a_i2c_write_byte(0x12, 0x50);
                siv121a_i2c_write_byte(0x13, 0x50);
                siv121a_i2c_write_byte(0x14, 0x50);
                siv121a_i2c_write_byte(0x70, 0xd4);
                break;

            case 10:    // -2 EV
                siv121a_i2c_write_byte(0x12, 0x60);
                siv121a_i2c_write_byte(0x13, 0x60);
                siv121a_i2c_write_byte(0x14, 0x60);
                siv121a_i2c_write_byte(0x70, 0xd4);
                break;

            case 11:    // -1 EV
                siv121a_i2c_write_byte(0x12, 0x70);
                siv121a_i2c_write_byte(0x13, 0x70);
                siv121a_i2c_write_byte(0x14, 0x70);
                siv121a_i2c_write_byte(0x70, 0xd4);
                break;

            case 12:   // +0 EV
                siv121a_i2c_write_byte(0x12, 0x80);
                siv121a_i2c_write_byte(0x13, 0x80);
                siv121a_i2c_write_byte(0x14, 0x80);
                siv121a_i2c_write_byte(0x70, 0x00);
                break;

            case 13:    // +1 EV
                siv121a_i2c_write_byte(0x12, 0x90);
                siv121a_i2c_write_byte(0x13, 0x90);
                siv121a_i2c_write_byte(0x14, 0x90);
                siv121a_i2c_write_byte(0x70, 0x00);
                break;

            case 14:    // +2 EV
                siv121a_i2c_write_byte(0x12, 0xA0);
                siv121a_i2c_write_byte(0x13, 0xA0);
                siv121a_i2c_write_byte(0x14, 0xA0);
                siv121a_i2c_write_byte(0x70, 0x00);
                break;

            case 15:    // +3 EV
                siv121a_i2c_write_byte(0x12, 0xB0);
                siv121a_i2c_write_byte(0x13, 0xB0);
                siv121a_i2c_write_byte(0x14, 0xB0);
                siv121a_i2c_write_byte(0x70, 0x00);
                break;

            case 16:    // +4 EV
                siv121a_i2c_write_byte(0x12, 0xC0);
                siv121a_i2c_write_byte(0x13, 0xC0);
                siv121a_i2c_write_byte(0x14, 0xC0);
                siv121a_i2c_write_byte(0x70, 0x00);
                break;

            default:
                break;
        }
    }
    
    return ret_val;
}

static camera_ret_code_type camsensor_siv121a_set_brightness(int8 brightness)
{
    ERR("camsensor_siv121a_set_brightness!",0,0,0);
    return CAMERA_SUCCESS;
}


static camera_ret_code_type camsensor_siv121a_set_contrast(int8 contrast)
{
    ERR("camsensor_siv121a_set_contrast!",0,0,0);
    return CAMERA_SUCCESS;
}/* camsensor_siv121a_set_contrast() */

static void camsensor_siv121a_setup_camctrl_tbl(camctrl_tbl_type *camctrl_tbl_ptr)
{
    ERR("camsensor_siv121a_get_snapshot_fps!",0,0,0);
    camctrl_tbl_ptr->asf_5x5_is_supported         = FALSE; 
    camctrl_tbl_ptr->get_snapshot_frame_per_sec = camsensor_siv121a_get_snapshot_fps;
}

static uint16 camsensor_siv121a_get_snapshot_fps(uint16 fps)
{
    ERR("camsensor_siv121a_get_snapshot_fps!",0,0,0);
    /* In the current driver, the snapshot fps is always 11 */
    fps = 15 * Q8;
    return fps;
}

camera_ret_code_type camsensor_siv121a_set_antibanding(int8 antibanding)
{
    ERR("camsensor_ov2655_set_antibanding!",0,0,0);
    return CAMERA_SUCCESS;
}

static void camsensor_siv121a_register(camsensor_function_table_type *camsensor_function_table_ptr)
{
    ERR("camsensor_siv121a_register!",0,0,0);
    camsensor_function_table_ptr->camsensor_start                 = camsensor_siv121a_start;
    camsensor_function_table_ptr->camsensor_video_config          = camsensor_siv121a_video_config;
    camsensor_function_table_ptr->camsensor_raw_snapshot_config   = camsensor_siv121a_raw_snapshot_config;
    camsensor_function_table_ptr->camsensor_snapshot_config       = camsensor_siv121a_snapshot_config;
    camsensor_function_table_ptr->camsensor_power_up              = camsensor_siv121a_power_up;
    camsensor_function_table_ptr->camsensor_power_down            = camsensor_siv121a_power_down;
    camsensor_function_table_ptr->camsensor_set_frame_rate        = camsensor_siv121a_set_frame_rate;  

    camsensor_function_table_ptr->camsensor_set_effect            = camsensor_siv121a_set_effect;

    camsensor_function_table_ptr->camsensor_set_brightness        = camsensor_siv121a_set_brightness;
    camsensor_function_table_ptr->camsensor_set_contrast          = camsensor_siv121a_set_contrast;
    camsensor_function_table_ptr->camsensor_set_antibanding       = camsensor_siv121a_set_antibanding;
    camsensor_function_table_ptr->camsensor_set_exposure_mode     = camsensor_siv121a_set_exposure_mode;
    camsensor_function_table_ptr->camsensor_set_ev_compensation   = camsensor_siv121a_set_ev_compensation; 
    camsensor_function_table_ptr->camsensor_set_wb                = camsensor_siv121a_set_wb;
} /* camsensor_mt9d112_mu2m0yu_register */
#endif

