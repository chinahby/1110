#include "camsensor.h"

#if defined (FEATURE_CAMERA) && defined (USE_CAMSENSOR_SIV121A)
#include "camsensor_siv121a.h"
#include "camerai.h"
#include "ipl.h"
#include "msg.h"
#include "i2c.h"
#include "clk.h"
#include "chromatix_6550.h"

/*============================================================================
	TYPE DEFINITIONS 
============================================================================*/
typedef enum
{
	SENSOR_PREVIEW_MODE,
	SENSOR_SNAPSHOT_MODE
}sensor_mode_type;


#define CAMSENSOR_SIV121A_FULL_SIZE_WIDTH  640//640
#define CAMSENSOR_SIV121A_FULL_SIZE_HEIGHT 480//480

#define CAMSENSOR_SIV121A_QTR_SIZE_WIDTH   640
#define CAMSENSOR_SIV121A_QTR_SIZE_HEIGHT  480//512

/* For the clock, choose jitter free 48.0MHz, halve in sensor for preview mode.
 * 54MHz max for CAMIF in snapshot mode since it does not go through the VFE. */
#define CAMSENSOR_SIV121A_GP_CLK_HZ 24000000

/* FPS supported by this sensor */
#define CAMSENSOR_30_FPS   (30*Q8)
#define CAMSENSOR_15_FPS   (15*Q8)
#define CAMSENSOR_7p5_FPS  (7.5*Q8)

/***************************************************************
* The following defines are used as UI parameters
***************************************************************/
#define CAMSENSOR_MIN_BRIGHTNESS 0

#define CAMSENSOR_MAX_BRIGHTNESS 10

#define CAMSENSOR_MIN_CONTRAST 0

#define CAMSENSOR_MAX_CONTRAST 10

#define MAX_EV_COMP 25

#define CAMSENSOR_SIV121A_RESET_PIN GPIO_OUTPUT_13

static camera_effect_type camsensor_siv121a_effect = CAMERA_EFFECT_OFF;

static char camsensor_siv121a_sensor_name[32] =
  "SIV121A 0.3MP RGB565\0\0\0\0\0\0\0";

static const char camsensor_siv121a_sensor_suffix[]="SIV121A";

static void    camsensor_siv121a_register(camsensor_function_table_type *camsensor_function_table_ptr);
static void    camsensor_siv121a_setup_camctrl_tbl(camctrl_tbl_type *camctrl_tbl_ptr);
static camera_ret_code_type camsensor_siv121a_set_effect(int8 effect);
static boolean initialize_siv121a_registers(void);
static uint16 camsensor_siv121a_get_snapshot_fps(uint16);
camera_ret_code_type camsensor_siv121a_set_wb(int8 wb);
static boolean siv121a_i2c_write_byte(uint8 offset, uint8 data);
static boolean siv121a_i2c_read_byte(uint8 offset, uint8* data);

static boolean sensor_cap_state 	= FALSE; //Preview or Capture

/* FPS supported by this sensor */
#define CAMSENSOR_30_FPS   (30*Q8)
#define CAMSENSOR_15_FPS   (15*Q8)
#define CAMSENSOR_12p5_FPS  (12.5*Q8)


static uint16 current_fps=15*Q8;
static CAMIF_InputFormatType format_preview,format_snapshot;

#define SIV121A_SET_PAGE0 	siv121a_i2c_write_byte(0xfe, 0x00)
#define SIV121A_SET_PAGE1 	siv121a_i2c_write_byte(0xfe, 0x01)


uint32 set_siv121a_param_banding(uint32 para);

/*************************************************************************
* FUNCTION
*  GC0309_GAMMA_Select
*
* DESCRIPTION
*  This function config the GAMMA Curve of GC0308 fitting for  
*  different LCD performances.
*
* PARAMETERS
*  GammaLvl : Different Gamma curves. Typical level value is from 0 to 5 which 
*                  means small level gamma to high level gamma.
*
* RETURNS
*  void
*
* GLOBALS AFFECTED
*
*************************************************************************/
static void siv121a_gamma_select(uint32 GammaLvl)
{
	switch(GammaLvl)
	{
		case 1:                                             //smallest gamma curve
			siv121a_i2c_write_byte( 0x9F, 0x0B ); 
			siv121a_i2c_write_byte( 0xA0, 0x16 ); 
			siv121a_i2c_write_byte( 0xA1, 0x29 ); 
			siv121a_i2c_write_byte( 0xA2, 0x3C ); 
			siv121a_i2c_write_byte( 0xA3, 0x4F ); 
			siv121a_i2c_write_byte( 0xA4, 0x5F ); 
			siv121a_i2c_write_byte( 0xA5, 0x6F ); 
			siv121a_i2c_write_byte( 0xA6, 0x8A ); 
			siv121a_i2c_write_byte( 0xA7, 0x9F ); 
			siv121a_i2c_write_byte( 0xA8, 0xB4 ); 
			siv121a_i2c_write_byte( 0xA9, 0xC6 ); 
			siv121a_i2c_write_byte( 0xAA, 0xD3 ); 
			siv121a_i2c_write_byte( 0xAB, 0xDD );  
			siv121a_i2c_write_byte( 0xAC, 0xE5 );  
			siv121a_i2c_write_byte( 0xAD, 0xF1 ); 
			siv121a_i2c_write_byte( 0xAE, 0xFA ); 
			siv121a_i2c_write_byte( 0xAF, 0xFF ); 	
			break;
			
		case 2:			
			siv121a_i2c_write_byte( 0x9F, 0x0E ); 
			siv121a_i2c_write_byte( 0xA0, 0x1C ); 
			siv121a_i2c_write_byte( 0xA1, 0x34 ); 
			siv121a_i2c_write_byte( 0xA2, 0x48 ); 
			siv121a_i2c_write_byte( 0xA3, 0x5A ); 
			siv121a_i2c_write_byte( 0xA4, 0x6B ); 
			siv121a_i2c_write_byte( 0xA5, 0x7B ); 
			siv121a_i2c_write_byte( 0xA6, 0x95 ); 
			siv121a_i2c_write_byte( 0xA7, 0xAB ); 
			siv121a_i2c_write_byte( 0xA8, 0xBF );
			siv121a_i2c_write_byte( 0xA9, 0xCE ); 
			siv121a_i2c_write_byte( 0xAA, 0xD9 ); 
			siv121a_i2c_write_byte( 0xAB, 0xE4 );  
			siv121a_i2c_write_byte( 0xAC, 0xEC ); 
			siv121a_i2c_write_byte( 0xAD, 0xF7 ); 
			siv121a_i2c_write_byte( 0xAE, 0xFD ); 
			siv121a_i2c_write_byte( 0xAF, 0xFF ); 
			break;
		
		case 3:
			siv121a_i2c_write_byte( 0x9F, 0x10 ); 
			siv121a_i2c_write_byte( 0xA0, 0x20 ); 
			siv121a_i2c_write_byte( 0xA1, 0x38 ); 
			siv121a_i2c_write_byte( 0xA2, 0x4E ); 
			siv121a_i2c_write_byte( 0xA3, 0x63 ); 
			siv121a_i2c_write_byte( 0xA4, 0x76 ); 
			siv121a_i2c_write_byte( 0xA5, 0x87 ); 
			siv121a_i2c_write_byte( 0xA6, 0xA2 ); 
			siv121a_i2c_write_byte( 0xA7, 0xB8 ); 
			siv121a_i2c_write_byte( 0xA8, 0xCA ); 
			siv121a_i2c_write_byte( 0xA9, 0xD8 ); 
			siv121a_i2c_write_byte( 0xAA, 0xE3 ); 
			siv121a_i2c_write_byte( 0xAB, 0xEB ); 
			siv121a_i2c_write_byte( 0xAC, 0xF0 ); 
			siv121a_i2c_write_byte( 0xAD, 0xF8 ); 
			siv121a_i2c_write_byte( 0xAE, 0xFD ); 
			siv121a_i2c_write_byte( 0xAF, 0xFF );
			break;
			
		case 4:
			siv121a_i2c_write_byte( 0x9F, 0x14 ); 
			siv121a_i2c_write_byte( 0xA0, 0x28 ); 
			siv121a_i2c_write_byte( 0xA1, 0x44 ); 
			siv121a_i2c_write_byte( 0xA2, 0x5D ); 
			siv121a_i2c_write_byte( 0xA3, 0x72 ); 
			siv121a_i2c_write_byte( 0xA4, 0x86 ); 
			siv121a_i2c_write_byte( 0xA5, 0x95 ); 
			siv121a_i2c_write_byte( 0xA6, 0xB1 ); 
			siv121a_i2c_write_byte( 0xA7, 0xC6 ); 
			siv121a_i2c_write_byte( 0xA8, 0xD5 ); 
			siv121a_i2c_write_byte( 0xA9, 0xE1 ); 
			siv121a_i2c_write_byte( 0xAA, 0xEA ); 
			siv121a_i2c_write_byte( 0xAB, 0xF1 ); 
			siv121a_i2c_write_byte( 0xAC, 0xF5 ); 
			siv121a_i2c_write_byte( 0xAD, 0xFB ); 
			siv121a_i2c_write_byte( 0xAE, 0xFE ); 
			siv121a_i2c_write_byte( 0xAF, 0xFF );
			break;
			
		case 5:								// largest gamma curve
			siv121a_i2c_write_byte( 0x9F, 0x15 ); 
			siv121a_i2c_write_byte( 0xA0, 0x2A ); 
			siv121a_i2c_write_byte( 0xA1, 0x4A ); 
			siv121a_i2c_write_byte( 0xA2, 0x67 ); 
			siv121a_i2c_write_byte( 0xA3, 0x79 ); 
			siv121a_i2c_write_byte( 0xA4, 0x8C ); 
			siv121a_i2c_write_byte( 0xA5, 0x9A ); 
			siv121a_i2c_write_byte( 0xA6, 0xB3 ); 
			siv121a_i2c_write_byte( 0xA7, 0xC5 ); 
			siv121a_i2c_write_byte( 0xA8, 0xD5 ); 
			siv121a_i2c_write_byte( 0xA9, 0xDF ); 
			siv121a_i2c_write_byte( 0xAA, 0xE8 ); 
			siv121a_i2c_write_byte( 0xAB, 0xEE ); 
			siv121a_i2c_write_byte( 0xAC, 0xF3 ); 
			siv121a_i2c_write_byte( 0xAD, 0xFA ); 
			siv121a_i2c_write_byte( 0xAE, 0xFD ); 
			siv121a_i2c_write_byte( 0xAF, 0xFF );
			break;
			
		default:		
			break;
	}
}

void write_siv121a_shutter(uint16 shutter)
{
	siv121a_i2c_write_byte(0xd2, 0x10);   // close AEC

	siv121a_i2c_write_byte(0x03, shutter/256);
	siv121a_i2c_write_byte(0x04, shutter & 0x00ff);
	camera_timed_wait(200);    

	siv121a_i2c_write_byte(0xd2, 0x90);   // Open AEC	
}/* write_GC0309_shutter */


static void siv121a_H_V_Switch(uint8 direction)
{
	switch(direction) 
	{
		case 1:  // normal
		    siv121a_i2c_write_byte(0x14, 0x10);	
		  	break;
		case 2:  // IMAGE_H_MIRROR
		    siv121a_i2c_write_byte(0x14, 0x11);	
		  	break;		  
		case 3:  // IMAGE_V_MIRROR
		    siv121a_i2c_write_byte(0x14, 0x12);	
			break;	
		case 4:  // IMAGE_HV_MIRROR
		    siv121a_i2c_write_byte(0x14, 0x13);	
			break;	
		default:
			break;
	} 
}


static uint16 read_siv121a_shutter(void)
{
	uint16 shutter;
	uint8 temp = 0;

	siv121a_i2c_read_byte(0x03,&temp);
	shutter = temp<<8;

	siv121a_i2c_read_byte(0x04,&temp);
	shutter |= temp&0xff;
	
	return shutter;
}/* read_GC0309_shutter */

/*************************************************************************
* FUNCTION
*	GC0309_Write_More_Registers
*
* DESCRIPTION
*	This function is served for FAE to modify the necessary Init Regs. Do not modify the regs
*     in init_GC0309() directly.
*
* PARAMETERS
*	None
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
void siv121a_write_more_registers(void)
{
	//  TODO: FAE Modify the Init Regs here!!!
	/*Customer can adjust GAMMA, MIRROR & UPSIDEDOWN here!*/
	siv121a_gamma_select(2);
  	siv121a_H_V_Switch(4);
}


void siv121a_set_dummy(uint16 pixels, uint16 lines)
{
	siv121a_i2c_write_byte(0x0f, ((lines & 0x0f00)>>4) + ((pixels & 0x0f00)>>8)); 
	siv121a_i2c_write_byte(0x01, pixels & 0x00ff);
	siv121a_i2c_write_byte(0x02, lines  & 0x00ff);
}	/* set_GC0309_dummy */


void camsensor_siv121a_init_camsensor_info()
{
	camsensor_info.pclk_invert=FALSE;
	camsensor_info.preview_dummy_pixels = 0;
	format_preview= CAMIF_YCbCr_Y_Cb_Y_Cr;
	format_snapshot= CAMIF_YCbCr_Y_Cb_Y_Cr;
	camsensor_camclk_po_hz=24000000;

	switch(camsensor_info.msm_id)
	{
		ERR("camsensor_siv121a_init_camsensor_info : %d",camsensor_info.msm_id,0,0);
		case MSM6280:
		case MSM6290:
		   camsensor_info.pclk_invert=FALSE;
		   break;


		case MSM6800B:
		    camsensor_info.pclk_invert=FALSE;
		    camsensor_camclk_po_hz=49000000;
			break;

		case MSM6246:
		case MSM6260:
		   camsensor_info.preview_dummy_pixels = 1;
		   format_preview=CAMIF_YCbCr_Y_Cb_Y_Cr;
		   format_snapshot=CAMIF_YCbCr_Y_Cb_Y_Cr;
		   //fmt_reg_val= 0x0200;
		   break;

		case SC2X:
		  camsensor_info.pclk_invert=FALSE;
		  camsensor_info.preview_dummy_pixels = 1;
		  format_preview = CAMIF_YCbCr_Y_Cb_Y_Cr;
		  format_snapshot= CAMIF_YCbCr_Y_Cb_Y_Cr;
		  //fmt_reg_val=0x0200;
		camsensor_camclk_po_hz=54000000;
		  break;

		default:
		  break;
	}

}/* camsensor_siv121a_init_camsensor_info */


camera_ret_code_type camsensor_siv121a_set_exposure_mode(int8 mode)
{
	return CAMERA_SUCCESS;
}

camera_ret_code_type camsensor_siv121a_set_frame_rate
(
  uint16 fps
)
{
	if (fps > CAMSENSOR_30_FPS)
	{
		return CAMERA_INVALID_PARM;
	} 

	return CAMERA_SUCCESS;
} /* camsensor_siv121a_set_frame_rate() */

static boolean g_bEnterTest = FALSE;
boolean camsensor_siv121a_init(camsensor_function_table_type *camsensor_function_table_ptr,
				   camctrl_tbl_type              *camctrl_tbl_ptr)
{
	uint8	sensor_id;
	
	ERR("camsensor_siv121a_init!",0,0,0);
	/* Input MCLK = 24MHz */
	camsensor_camclk_po_hz = 24000000;
    camsensor_camclk_po_hz = camsensor_config_camclk_po(camsensor_camclk_po_hz);
    
	/* Preview must aways be se to quater size */
   	camsensor_preview_resolution  = CAMSENSOR_QTR_SIZE;
   	camsensor_snapshot_resolution = CAMSENSOR_FULL_SIZE;
  	
	/* Configure I2C parameters */
	camsensor_i2c_command.bus_id     = I2C_BUS_HW_CTRL;
	camsensor_i2c_command.slave_addr = 0x66;//SIV121A_I2C_WRITE_ID;
    
	/*lint -save -e655 */
	camsensor_i2c_command.options    = (i2c_options_type) (I2C_REG_DEV | I2C_START_BEFORE_READ); 

    clk_busy_wait(2000);
  	CAMERA_CONFIG_GPIO(CAMSENSOR_SIV121A_RESET_PIN);
  	gpio_out(CAMSENSOR_SIV121A_RESET_PIN,1);
  	clk_busy_wait(50000);
  	gpio_out(CAMSENSOR_SIV121A_RESET_PIN,0);
  	clk_busy_wait(150000);
  	gpio_out(CAMSENSOR_SIV121A_RESET_PIN,1);
  	clk_busy_wait(10000);

    while(g_bEnterTest)
    {
        siv121a_i2c_write_byte(0x00,0x00);
    }
    
    while(g_bEnterTest)
    {
        siv121a_i2c_write_byte(0x00,0xFF);
    }
    
    if( !siv121a_i2c_write_byte(0x00,0x00))
    {
        ERR("Block Select Error!",0,0,0);
        return FALSE;
    }
    
    if( !siv121a_i2c_read_byte(SIV121A_ID_REG,&sensor_id)) 
    {
    	ERR("read sensor_id failed!",0,0,0);
        return FALSE;
    }
    
    ERR("sensor_id = %x",sensor_id,0,0);
    
	/* Check if it matches it with the value in Datasheet */
	if ( sensor_id != SIV121A_SENSOR_ID)
	{
		return FALSE;
	}
	
	//siv121a_sensor_mode = SENSOR_PREVIEW_MODE;
	/* Initialize Sensor registers */
	if (!initialize_siv121a_registers())
	{
		return FALSE;
	}

	/* Register function table: */
	camsensor_siv121a_register(camsensor_function_table_ptr);

	/* Setup camctrl_tbl */
	camsensor_siv121a_setup_camctrl_tbl(camctrl_tbl_ptr);

	return TRUE;
} /* camsensor_siv121a_init */

static boolean initialize_siv121a_registers(void)
{
    // SNR Block [Vendor recommended value ##Don't change##]
    siv121a_i2c_write_byte(0x00,0x00);
    siv121a_i2c_write_byte(0x03,0x04); //0x04
    siv121a_i2c_write_byte(0x10,0x83); 
    //siv121a_i2c_write_byte(0x13,0x17);
    //siv121a_i2c_write_byte(0x15,0x22); 

    siv121a_i2c_write_byte(0x00,0x01);
    siv121a_i2c_write_byte(0x04,0x00);
    siv121a_i2c_write_byte(0x05,0x02); 
    siv121a_i2c_write_byte(0x11,0x25);
    siv121a_i2c_write_byte(0x12,0x21); 
    siv121a_i2c_write_byte(0x13,0x15);
    siv121a_i2c_write_byte(0x15,0xC2); 
    
    //50HZ 12MHZ 12.5FPS
    siv121a_i2c_write_byte(0x20,0x00); 
    siv121a_i2c_write_byte(0x21,0x6e);
    siv121a_i2c_write_byte(0x23,0x1d);
    siv121a_i2c_write_byte(0x00,0x02);
    siv121a_i2c_write_byte(0x34,0x42);

    siv121a_i2c_write_byte(0x00,0x02);
    siv121a_i2c_write_byte(0x11,0x08); 
    siv121a_i2c_write_byte(0x12,0x80); 
    siv121a_i2c_write_byte(0x13,0x80);
    siv121a_i2c_write_byte(0x14,0x80);
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
    siv121a_i2c_write_byte(0x10,0xD0);
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
    siv121a_i2c_write_byte(0x25,0x20);
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
    siv121a_i2c_write_byte(0x12,0x9D); // Y,Cb,Cr order sequence
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
#if 0
    siv121a_i2c_write_byte(0x40,0x00);
    siv121a_i2c_write_byte(0x41,0x21);
    siv121a_i2c_write_byte(0x42,0x32);
    siv121a_i2c_write_byte(0x43,0x43);
    siv121a_i2c_write_byte(0x44,0x55);
    siv121a_i2c_write_byte(0x45,0x55);
    siv121a_i2c_write_byte(0x46,0x11); //R left # right
    siv121a_i2c_write_byte(0x47,0x23); //R top # bottom
    siv121a_i2c_write_byte(0x48,0x10); //Gr left # right
    siv121a_i2c_write_byte(0x49,0x12); //Gr top # bottom
    siv121a_i2c_write_byte(0x4A,0x01); //Gb left # right
    siv121a_i2c_write_byte(0x4B,0x12); //Gb top # bottom
    siv121a_i2c_write_byte(0x4C,0x00); //B left # right
    siv121a_i2c_write_byte(0x4D,0x11); //Br top # bottom
    siv121a_i2c_write_byte(0x4E,0x04);
    siv121a_i2c_write_byte(0x4F,0x50);
    siv121a_i2c_write_byte(0x50,0xF6);
    siv121a_i2c_write_byte(0x51,0x80);
    siv121a_i2c_write_byte(0x52,0x00);
    siv121a_i2c_write_byte(0x53,0x00);
    siv121a_i2c_write_byte(0x54,0x00);
    siv121a_i2c_write_byte(0x55,0x00);
#else
    siv121a_i2c_write_byte(0x40,0x0a);
    siv121a_i2c_write_byte(0x41,0xba);
    siv121a_i2c_write_byte(0x42,0x32);
    siv121a_i2c_write_byte(0x43,0x32);
    siv121a_i2c_write_byte(0x44,0x21);
    siv121a_i2c_write_byte(0x45,0x00);                                                  
    siv121a_i2c_write_byte(0x46,0x24);   // left R gain[7:4], right R gain[3:0] 0x22        
    siv121a_i2c_write_byte(0x47,0x22);   // top R gain[7:4], bottom R gain[3:0]             
    siv121a_i2c_write_byte(0x48,0x23);   // left Gr gain[7:4], right Gr gain[3:0]           
    siv121a_i2c_write_byte(0x49,0x50);   // top Gr gain[7:4], bottom Gr gain[3:0]           
    siv121a_i2c_write_byte(0x4a,0x14);   // left Gb gain[7:4], right Gb gain[3:0]           
    siv121a_i2c_write_byte(0x4b,0x12);   // top Gb gain[7:4], bottom Gb gain[3:0]           
    siv121a_i2c_write_byte(0x4c,0x03);   // left B gain[7:4], right B gain[3:0]             
    siv121a_i2c_write_byte(0x4d,0x21);   // top B gain[7:4], bottom B gain[3:0]             
    siv121a_i2c_write_byte(0x4e,0x04);   // X-axis center high[3:2], Y-axis center high[1:0]
    siv121a_i2c_write_byte(0x4f,0x46);   // X-axis center low[7:0]                          
    siv121a_i2c_write_byte(0x50,0xf6);   // Y-axis center low[7:0]                          
    siv121a_i2c_write_byte(0x51,0x84);   // Shading Center Gain                             
    siv121a_i2c_write_byte(0x52,0x00);   // Shading R Offset                                
    siv121a_i2c_write_byte(0x53,0x00);   // Shading Gr Offset                               
    siv121a_i2c_write_byte(0x54,0x00);   // Shading Gb Offset                               
    siv121a_i2c_write_byte(0x55,0x00);   // Shading B Offset  
#endif

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

    siv121a_i2c_write_byte(0xc0,0x24);
    siv121a_i2c_write_byte(0xc1,0x00);
    siv121a_i2c_write_byte(0xc2,0x80);
    siv121a_i2c_write_byte(0xc3,0x00);
    siv121a_i2c_write_byte(0xc4,0xe0);

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
	siv121a_gamma_select(2);
  	siv121a_H_V_Switch(1);
	
	return TRUE;
} /* end of initialize_siv121a_registers. */

boolean camsensor_siv121a_start
(
  /* A pointer to the services layer static camsensor info struct */
  camsensor_static_params_type *camsensor_params
)
{

	ERR("camsensor_siv121a_set_antibanding!",0,0,0);
	camsensor_siv121a_init_camsensor_info();

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
	camsensor_params->sensor_model  = CAMSENSOR_SIV121A_ID;

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

	/* Full size dimensions - 1280x1024 */

	camsensor_params->full_size_width  =  CAMSENSOR_SIV121A_FULL_SIZE_WIDTH;
	camsensor_params->full_size_height =  CAMSENSOR_SIV121A_FULL_SIZE_HEIGHT;


	/* Quarter size dimensions - 640x512 */

	camsensor_params->qtr_size_width  =  CAMSENSOR_SIV121A_QTR_SIZE_WIDTH;
	camsensor_params->qtr_size_height =  CAMSENSOR_SIV121A_QTR_SIZE_HEIGHT;


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
} /*camsensor_siv121a_start*/

static boolean siv121a_i2c_write_byte(uint8 offset, uint8 data) 
{
	static uint8 writebyte;
	uint8 i;

	writebyte  = data;
    
    //camsensor_i2c_command.slave_addr = SIV121A_I2C_WRITE_ID;
	camsensor_i2c_command.addr.reg = offset;
	camsensor_i2c_command.buf_ptr  = (byte *)(&writebyte);
	camsensor_i2c_command.len      = 1;

	for (i = 0; i < 3; ++i)
	{
		if (i2c_write(&camsensor_i2c_command) == I2C_SUCCESS)
		{
			ERR("siv121a_i2c_write_byte: OK %x,%x",offset,data,0);
			return TRUE;
		}
	}
	ERR("siv121a_i2c_write_byte: false %x,%x",offset,data,0);
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
    
    //camsensor_i2c_command.slave_addr = SIV121A_I2C_READ_ID;
	camsensor_i2c_command.addr.reg = offset;
	camsensor_i2c_command.buf_ptr  = (byte *)(&readbyte);
	camsensor_i2c_command.len      = 1;

	for (i =0; i < 3; ++i)
	{
		if (i2c_read(&camsensor_i2c_command) == I2C_SUCCESS)
		{
			*data  = readbyte;
			ERR("siv121a_i2c_read_byte: OK %x",offset,0,0);
			return TRUE;
		}
	}

	ERR("siv121a_i2c_read_byte: false %x",offset,0,0);
	return FALSE;
} /* end of siv121a_i2c_read_word. */


camera_ret_code_type camsensor_siv121a_set_effect(int8 effect)
{
	uint16 seteffect_delay=1;
	uint16 seteffect_zone=1;


	ERR("camsensor_siv121a_set_effect effect = %d",effect,0,0);
	
	switch ((camera_effect_type)effect)
	{
		case CAMERA_EFFECT_OFF:
		{
			siv121a_i2c_write_byte(0x23,0x00);
			siv121a_i2c_write_byte(0x2d,0x0a);
			siv121a_i2c_write_byte(0x20,0xff);
			siv121a_i2c_write_byte(0xd2,0x90);
			siv121a_i2c_write_byte(0x73,0x00);
			siv121a_i2c_write_byte(0x77,0x54);
			
			siv121a_i2c_write_byte(0xb3,0x40);
			siv121a_i2c_write_byte(0xb4,0x80);
			siv121a_i2c_write_byte(0xba,0x00);
			siv121a_i2c_write_byte(0xbb,0x00);
			break;
		}
		
		case CAMERA_EFFECT_MONO:
		{
			siv121a_i2c_write_byte(0x23,0x02);
			siv121a_i2c_write_byte(0x2d,0x0a);
			siv121a_i2c_write_byte(0x20,0xff);
			siv121a_i2c_write_byte(0xd2,0x90);
			siv121a_i2c_write_byte(0x73,0x00);

			siv121a_i2c_write_byte(0xb3,0x40);
			siv121a_i2c_write_byte(0xb4,0x80);
			siv121a_i2c_write_byte(0xba,0x50);
			siv121a_i2c_write_byte(0xbb,0xe0);
			break;
		}

		case CAMERA_EFFECT_NEGATIVE:
		{
			siv121a_i2c_write_byte(0x23,0x02);
			siv121a_i2c_write_byte(0x2d,0x0a);
			siv121a_i2c_write_byte(0x20,0xbf);
			siv121a_i2c_write_byte(0xd2,0x10);
			siv121a_i2c_write_byte(0x73,0x01);

			siv121a_i2c_write_byte(0x51,0x40);
			siv121a_i2c_write_byte(0x52,0x40);

			siv121a_i2c_write_byte(0xb3,0x40);
			siv121a_i2c_write_byte(0xb4,0x80);
			siv121a_i2c_write_byte(0xba,0x60);
			siv121a_i2c_write_byte(0xbb,0x00);
			break;
		}

		case CAMERA_EFFECT_SOLARIZE:
		{
			break;
		}

		case CAMERA_EFFECT_SEPIA:
		{
			siv121a_i2c_write_byte(0x23,0x02);
			siv121a_i2c_write_byte(0x2d,0x0a);
			siv121a_i2c_write_byte(0x20,0xff);
			siv121a_i2c_write_byte(0xd2,0x90);
			siv121a_i2c_write_byte(0x73,0x00);

			siv121a_i2c_write_byte(0xb3,0x40);
			siv121a_i2c_write_byte(0xb4,0x80);
			siv121a_i2c_write_byte(0xba,0xd0);
			siv121a_i2c_write_byte(0xbb,0x28);
			break;
		}
		
		case CAMERA_EFFECT_PASTEL:
		{
			siv121a_i2c_write_byte(0x23,0x02);
			siv121a_i2c_write_byte(0x2d,0x0a);
			siv121a_i2c_write_byte(0x20,0xbf);
			siv121a_i2c_write_byte(0xd2,0x10);
			siv121a_i2c_write_byte(0x73,0x01);

			siv121a_i2c_write_byte(0x51,0x40);
			siv121a_i2c_write_byte(0x52,0x40);

			siv121a_i2c_write_byte(0xb3,0x40);
			siv121a_i2c_write_byte(0xb4,0x80);
			siv121a_i2c_write_byte(0xba,0xc0);
			siv121a_i2c_write_byte(0xbb,0x20);
			break;
		}
		case CAMERA_EFFECT_MOSAIC:
		case CAMERA_EFFECT_RESIZE:
		default:
		{
			return CAMERA_NOT_SUPPORTED;
		}
	}

	camsensor_siv121a_effect = (camera_effect_type)effect;
	
	if(seteffect_zone> 0 && seteffect_zone< 25)
	{
		seteffect_delay=(double)(250*seteffect_zone)/(double)(current_fps/Q8);

		camera_timed_wait(seteffect_delay);
	}
	
	return CAMERA_SUCCESS;
} /* camsensor_siv121a_set_effect() */

/*===========================================================================

FUNCTION      CAMSENSOR_SIV100A_SNAPSHOT_CONFIG

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

boolean camsensor_siv121a_snapshot_config
(
	camsensor_static_params_type  *camsensor_params /* Other config params */
)
{
	volatile uint32 shutter=0;
    sensor_cap_state= TRUE;	

	ERR("camsensor_siv121a_snapshot_config!",0,0,0);
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
	shutter=read_siv121a_shutter();  
	write_siv121a_shutter(shutter);
		 

	/* Wait for one frame before reconfiguring the VFE since either zero or one
	* frames will be output after commencing the snapshot program, depending on
	* when the I2C transaction took place relative to the active frame. */
	camera_timed_wait(150);

	camsensor_current_resolution = camsensor_snapshot_resolution;
	
	return TRUE;
}
/*===========================================================================

FUNCTION      CAMSENSOR_SIV100A_RAW_SNAPSHOT_CONFIG

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

boolean camsensor_siv121a_raw_snapshot_config
(
   camsensor_static_params_type  *camsensor_params /* Other config params */
)
{
	ERR("camsensor_siv121a_raw_snapshot_config!",0,0,0);
	return TRUE;
}

/*===========================================================================

FUNCTION      CAMSENSOR_SIV100A_VIDEO_CONFIG

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

boolean camsensor_siv121a_video_config
(
  camsensor_static_params_type *camsensor_params /* Other config params */
)
{
	sensor_cap_state = FALSE;
  	camsensor_params->format = format_preview;
 	camsensor_params->discardFirstFrame =  TRUE;
	
	ERR("camsensor_siv121a_video_config!",0,0,0);
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


void camsensor_siv121a_power_up(void)
{
	ERR("camsensor_siv121a_power_up!",0,0,0);
} /* camsensor_siv121a_power_up */

/*===========================================================================

FUNCTION      camsensor_mt9d112_mu2m0yu_POWER_DOWN

DESCRIPTION
Power down the sensor, using I2C driver.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/

void camsensor_siv121a_power_down(void)
{
	ERR("camsensor_siv121a_power_down!",0,0,0);
} /* camsensor_siv121a_power_down */


camera_ret_code_type camsensor_siv121a_set_wb(int8 wb)
{ 
	uint8  temp_reg;
	
	siv121a_i2c_read_byte(0x22,&temp_reg);
		
	ERR("camsensor_siv121a_set_wb!",0,0,0);
	switch((camera_wb_type)wb)
	{
		case CAMERA_WB_AUTO:
		{
			siv121a_i2c_write_byte(0x5a,0x56); //for AWB can adjust back
			siv121a_i2c_write_byte(0x5b,0x40);
			siv121a_i2c_write_byte(0x5c,0x4a);			
			siv121a_i2c_write_byte(0x22,temp_reg|0x02);	 // Enable AWB
			break;
		}

		case CAMERA_WB_INCANDESCENT:
		{
			siv121a_i2c_write_byte(0x22,temp_reg&~0x02); 
			siv121a_i2c_write_byte(0x5a,0x48);
			siv121a_i2c_write_byte(0x5b,0x40);
			siv121a_i2c_write_byte(0x5c,0x5c);
			break;
		}
		case CAMERA_WB_FLUORESCENT:
		{
			siv121a_i2c_write_byte(0x22,temp_reg&~0x02);   
			siv121a_i2c_write_byte(0x5a,0x40);
			siv121a_i2c_write_byte(0x5b,0x42);
			siv121a_i2c_write_byte(0x5c,0x50);
			break;
		}
		case CAMERA_WB_DAYLIGHT:
		{
			siv121a_i2c_write_byte(0x22,temp_reg&~0x02);   
			siv121a_i2c_write_byte(0x5a,0x74);  //  50 45 40
			siv121a_i2c_write_byte(0x5b,0x52);
			siv121a_i2c_write_byte(0x5c,0x40);
			break;
		}
		
		case CAMERA_WB_CLOUDY_DAYLIGHT:
		{
			siv121a_i2c_write_byte(0x22,temp_reg&~0x02);   // Disable AWB 
			siv121a_i2c_write_byte(0x5a,0x8c); //WB_manual_gain // 5a 42 40
			siv121a_i2c_write_byte(0x5b,0x50);
			siv121a_i2c_write_byte(0x5c,0x40);
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


camera_ret_code_type camsensor_siv121a_set_ev_compensation(int32 compensation)
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
		switch(new_luma_target)
		{
			case 8:    // -4 EV
        		siv121a_i2c_write_byte(0xb5, 0xc0);
				siv121a_i2c_write_byte(0xd3, 0x60);
       			break;

		    case 9:    // -3 EV
		        siv121a_i2c_write_byte(0xb5, 0xd0);
				siv121a_i2c_write_byte(0xd3, 0x68);
		        break;

		    case 10:    // -2 EV
		        siv121a_i2c_write_byte(0xb5, 0xe0);
				siv121a_i2c_write_byte(0xd3, 0x70);
		        break;

		    case 11:    // -1 EV
		        siv121a_i2c_write_byte(0xb5, 0xf0);
				siv121a_i2c_write_byte(0xd3, 0x78);
		        break;

		    case 12:   // +0 EV
		        siv121a_i2c_write_byte(0xb5, 0x00);
				siv121a_i2c_write_byte(0xd3, 0x80);
		        break;

		    case 13:    // +1 EV
		        siv121a_i2c_write_byte(0xb5, 0x20);
				siv121a_i2c_write_byte(0xd3, 0x88);
		        break;

		    case 14:    // +2 EV
		        siv121a_i2c_write_byte(0xb5, 0x30);
				siv121a_i2c_write_byte(0xd3, 0x90);
		        break;

		    case 15:    // +3 EV
		        siv121a_i2c_write_byte(0xb5, 0x40);
				siv121a_i2c_write_byte(0xd3, 0x98);
		        break;

		    case 16:    // +4 EV
		        siv121a_i2c_write_byte(0xb5, 0x50);
				siv121a_i2c_write_byte(0xd3, 0xa0);
		        break;

		    default:
		    	break;
	    }
	}
	
	return ret_val;
}

camera_ret_code_type camsensor_siv121a_set_brightness(int8 brightness)
{
	ERR("camsensor_siv121a_set_brightness!",0,0,0);
	return CAMERA_SUCCESS;
}


camera_ret_code_type camsensor_siv121a_set_contrast(int8 contrast)
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


LOCAL uint16 camsensor_siv121a_get_snapshot_fps(uint16 fps)
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
	camsensor_function_table_ptr->camsensor_set_exposure_mode 	  = camsensor_siv121a_set_exposure_mode;
	camsensor_function_table_ptr->camsensor_set_ev_compensation	  = camsensor_siv121a_set_ev_compensation; 
	camsensor_function_table_ptr->camsensor_set_wb				  = camsensor_siv121a_set_wb;
} /* camsensor_mt9d112_mu2m0yu_register */


#endif

