#include "camsensor.h"

#if defined (FEATURE_CAMERA) && defined (USE_CAMSENSOR_GC0309_0P1MP)
#include "camsensor_gc0309_0p1mp.h"
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


#define CAMSENSOR_GC0309_0P1MP_FULL_SIZE_WIDTH  640//640
#define CAMSENSOR_GC0309_0P1MP_FULL_SIZE_HEIGHT 480//480

#define CAMSENSOR_GC0309_0P1MP_QTR_SIZE_WIDTH   640
#define CAMSENSOR_GC0309_0P1MP_QTR_SIZE_HEIGHT  480//512

/* For the clock, choose jitter free 48.0MHz, halve in sensor for preview mode.
 * 54MHz max for CAMIF in snapshot mode since it does not go through the VFE. */
#define CAMSENSOR_GC0309_0P1MP_GP_CLK_HZ 24000000

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

#define CAMSENSOR_GC0309_0P1MP_RESET_PIN GPIO_OUTP_75

static camera_effect_type camsensor_gc0309_0p1mp_effect = CAMERA_EFFECT_OFF;

static char camsensor_gc0309_0p1mp_sensor_name[32] =
  "GC0309 0.3MP YUV\0\0\0\0\0\0\0";

static const char camsensor_gc0309_0p1mp_sensor_suffix[]="GC0309_mu0m3yu";

static void    camsensor_gc0309_0p1mp_register(camsensor_function_table_type *camsensor_function_table_ptr);
static void    camsensor_gc0309_0p1mp_setup_camctrl_tbl(camctrl_tbl_type *camctrl_tbl_ptr);
static camera_ret_code_type camsensor_gc0309_0p1mp_set_effect(int8 effect);
static boolean initialize_gc0309_0p1mp_registers(void);
static uint16 camsensor_gc0309_0p1mp_get_snapshot_fps(uint16);
camera_ret_code_type camsensor_gc0309_0p1mp_set_wb(int8 wb);
static boolean gc0309_0p1mp_i2c_write_byte(uint8 offset, uint8 data);
static boolean gc0309_0p1mp_i2c_read_byte(uint8 offset, uint8* data);

static boolean sensor_cap_state 	= FALSE; //Preview or Capture

/* FPS supported by this sensor */
#define CAMSENSOR_30_FPS   (30*Q8)
#define CAMSENSOR_15_FPS   (15*Q8)
#define CAMSENSOR_12p5_FPS  (12.5*Q8)


static uint16 current_fps=15*Q8;
static CAMIF_InputFormatType format_preview,format_snapshot;

#define GC0309_0P1MP_SET_PAGE0 	gc0309_0p1mp_i2c_write_byte(0xfe, 0x00)
#define GC0309_0P1MP_SET_PAGE1 	gc0309_0p1mp_i2c_write_byte(0xfe, 0x01)


uint32 set_gc0309_0p1mp_param_banding(uint32 para);

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
static void gc0309_0p1mp_gamma_select(uint32 GammaLvl)
{
	switch(GammaLvl)
	{
		case 1:                                             //smallest gamma curve
			gc0309_0p1mp_i2c_write_byte( 0x9F, 0x0B ); 
			gc0309_0p1mp_i2c_write_byte( 0xA0, 0x16 ); 
			gc0309_0p1mp_i2c_write_byte( 0xA1, 0x29 ); 
			gc0309_0p1mp_i2c_write_byte( 0xA2, 0x3C ); 
			gc0309_0p1mp_i2c_write_byte( 0xA3, 0x4F ); 
			gc0309_0p1mp_i2c_write_byte( 0xA4, 0x5F ); 
			gc0309_0p1mp_i2c_write_byte( 0xA5, 0x6F ); 
			gc0309_0p1mp_i2c_write_byte( 0xA6, 0x8A ); 
			gc0309_0p1mp_i2c_write_byte( 0xA7, 0x9F ); 
			gc0309_0p1mp_i2c_write_byte( 0xA8, 0xB4 ); 
			gc0309_0p1mp_i2c_write_byte( 0xA9, 0xC6 ); 
			gc0309_0p1mp_i2c_write_byte( 0xAA, 0xD3 ); 
			gc0309_0p1mp_i2c_write_byte( 0xAB, 0xDD );  
			gc0309_0p1mp_i2c_write_byte( 0xAC, 0xE5 );  
			gc0309_0p1mp_i2c_write_byte( 0xAD, 0xF1 ); 
			gc0309_0p1mp_i2c_write_byte( 0xAE, 0xFA ); 
			gc0309_0p1mp_i2c_write_byte( 0xAF, 0xFF ); 	
			break;
			
		case 2:			
			gc0309_0p1mp_i2c_write_byte( 0x9F, 0x0E ); 
			gc0309_0p1mp_i2c_write_byte( 0xA0, 0x1C ); 
			gc0309_0p1mp_i2c_write_byte( 0xA1, 0x34 ); 
			gc0309_0p1mp_i2c_write_byte( 0xA2, 0x48 ); 
			gc0309_0p1mp_i2c_write_byte( 0xA3, 0x5A ); 
			gc0309_0p1mp_i2c_write_byte( 0xA4, 0x6B ); 
			gc0309_0p1mp_i2c_write_byte( 0xA5, 0x7B ); 
			gc0309_0p1mp_i2c_write_byte( 0xA6, 0x95 ); 
			gc0309_0p1mp_i2c_write_byte( 0xA7, 0xAB ); 
			gc0309_0p1mp_i2c_write_byte( 0xA8, 0xBF );
			gc0309_0p1mp_i2c_write_byte( 0xA9, 0xCE ); 
			gc0309_0p1mp_i2c_write_byte( 0xAA, 0xD9 ); 
			gc0309_0p1mp_i2c_write_byte( 0xAB, 0xE4 );  
			gc0309_0p1mp_i2c_write_byte( 0xAC, 0xEC ); 
			gc0309_0p1mp_i2c_write_byte( 0xAD, 0xF7 ); 
			gc0309_0p1mp_i2c_write_byte( 0xAE, 0xFD ); 
			gc0309_0p1mp_i2c_write_byte( 0xAF, 0xFF ); 
			break;
		
		case 3:
			gc0309_0p1mp_i2c_write_byte( 0x9F, 0x10 ); 
			gc0309_0p1mp_i2c_write_byte( 0xA0, 0x20 ); 
			gc0309_0p1mp_i2c_write_byte( 0xA1, 0x38 ); 
			gc0309_0p1mp_i2c_write_byte( 0xA2, 0x4E ); 
			gc0309_0p1mp_i2c_write_byte( 0xA3, 0x63 ); 
			gc0309_0p1mp_i2c_write_byte( 0xA4, 0x76 ); 
			gc0309_0p1mp_i2c_write_byte( 0xA5, 0x87 ); 
			gc0309_0p1mp_i2c_write_byte( 0xA6, 0xA2 ); 
			gc0309_0p1mp_i2c_write_byte( 0xA7, 0xB8 ); 
			gc0309_0p1mp_i2c_write_byte( 0xA8, 0xCA ); 
			gc0309_0p1mp_i2c_write_byte( 0xA9, 0xD8 ); 
			gc0309_0p1mp_i2c_write_byte( 0xAA, 0xE3 ); 
			gc0309_0p1mp_i2c_write_byte( 0xAB, 0xEB ); 
			gc0309_0p1mp_i2c_write_byte( 0xAC, 0xF0 ); 
			gc0309_0p1mp_i2c_write_byte( 0xAD, 0xF8 ); 
			gc0309_0p1mp_i2c_write_byte( 0xAE, 0xFD ); 
			gc0309_0p1mp_i2c_write_byte( 0xAF, 0xFF );
			break;
			
		case 4:
			gc0309_0p1mp_i2c_write_byte( 0x9F, 0x14 ); 
			gc0309_0p1mp_i2c_write_byte( 0xA0, 0x28 ); 
			gc0309_0p1mp_i2c_write_byte( 0xA1, 0x44 ); 
			gc0309_0p1mp_i2c_write_byte( 0xA2, 0x5D ); 
			gc0309_0p1mp_i2c_write_byte( 0xA3, 0x72 ); 
			gc0309_0p1mp_i2c_write_byte( 0xA4, 0x86 ); 
			gc0309_0p1mp_i2c_write_byte( 0xA5, 0x95 ); 
			gc0309_0p1mp_i2c_write_byte( 0xA6, 0xB1 ); 
			gc0309_0p1mp_i2c_write_byte( 0xA7, 0xC6 ); 
			gc0309_0p1mp_i2c_write_byte( 0xA8, 0xD5 ); 
			gc0309_0p1mp_i2c_write_byte( 0xA9, 0xE1 ); 
			gc0309_0p1mp_i2c_write_byte( 0xAA, 0xEA ); 
			gc0309_0p1mp_i2c_write_byte( 0xAB, 0xF1 ); 
			gc0309_0p1mp_i2c_write_byte( 0xAC, 0xF5 ); 
			gc0309_0p1mp_i2c_write_byte( 0xAD, 0xFB ); 
			gc0309_0p1mp_i2c_write_byte( 0xAE, 0xFE ); 
			gc0309_0p1mp_i2c_write_byte( 0xAF, 0xFF );
			break;
			
		case 5:								// largest gamma curve
			gc0309_0p1mp_i2c_write_byte( 0x9F, 0x15 ); 
			gc0309_0p1mp_i2c_write_byte( 0xA0, 0x2A ); 
			gc0309_0p1mp_i2c_write_byte( 0xA1, 0x4A ); 
			gc0309_0p1mp_i2c_write_byte( 0xA2, 0x67 ); 
			gc0309_0p1mp_i2c_write_byte( 0xA3, 0x79 ); 
			gc0309_0p1mp_i2c_write_byte( 0xA4, 0x8C ); 
			gc0309_0p1mp_i2c_write_byte( 0xA5, 0x9A ); 
			gc0309_0p1mp_i2c_write_byte( 0xA6, 0xB3 ); 
			gc0309_0p1mp_i2c_write_byte( 0xA7, 0xC5 ); 
			gc0309_0p1mp_i2c_write_byte( 0xA8, 0xD5 ); 
			gc0309_0p1mp_i2c_write_byte( 0xA9, 0xDF ); 
			gc0309_0p1mp_i2c_write_byte( 0xAA, 0xE8 ); 
			gc0309_0p1mp_i2c_write_byte( 0xAB, 0xEE ); 
			gc0309_0p1mp_i2c_write_byte( 0xAC, 0xF3 ); 
			gc0309_0p1mp_i2c_write_byte( 0xAD, 0xFA ); 
			gc0309_0p1mp_i2c_write_byte( 0xAE, 0xFD ); 
			gc0309_0p1mp_i2c_write_byte( 0xAF, 0xFF );
			break;
			
		default:		
			break;
	}
}

void write_gc0309_0p1mp_shutter(uint16 shutter)
{
	gc0309_0p1mp_i2c_write_byte(0xd2, 0x10);   // close AEC

	gc0309_0p1mp_i2c_write_byte(0x03, shutter/256);
	gc0309_0p1mp_i2c_write_byte(0x04, shutter & 0x00ff);
	camera_timed_wait(200);    

	gc0309_0p1mp_i2c_write_byte(0xd2, 0x90);   // Open AEC	
}/* write_GC0309_shutter */


static void gc0309_0p1mp_H_V_Switch(uint8 direction)
{
	switch(direction) 
	{
		case 1:  // normal
		    gc0309_0p1mp_i2c_write_byte(0x14, 0x10);	
		  	break;
		case 2:  // IMAGE_H_MIRROR
		    gc0309_0p1mp_i2c_write_byte(0x14, 0x11);	
		  	break;		  
		case 3:  // IMAGE_V_MIRROR
		    gc0309_0p1mp_i2c_write_byte(0x14, 0x12);	
			break;	
		case 4:  // IMAGE_HV_MIRROR
		    gc0309_0p1mp_i2c_write_byte(0x14, 0x13);	
			break;	
		default:
			break;
	} 
}


static uint16 read_gc0309_0p1mp_shutter(void)
{
	uint16 shutter;
	uint8 temp = 0;

	gc0309_0p1mp_i2c_read_byte(0x03,&temp);
	shutter = temp<<8;

	gc0309_0p1mp_i2c_read_byte(0x04,&temp);
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
void gc0309_0p1mp_write_more_registers(void)
{
	//  TODO: FAE Modify the Init Regs here!!!
	/*Customer can adjust GAMMA, MIRROR & UPSIDEDOWN here!*/
	gc0309_0p1mp_gamma_select(2);
  	gc0309_0p1mp_H_V_Switch(4);
}


void gc0309_0p1mp_set_dummy(uint16 pixels, uint16 lines)
{
	gc0309_0p1mp_i2c_write_byte(0x0f, ((lines & 0x0f00)>>4) + ((pixels & 0x0f00)>>8)); 
	gc0309_0p1mp_i2c_write_byte(0x01, pixels & 0x00ff);
	gc0309_0p1mp_i2c_write_byte(0x02, lines  & 0x00ff);
}	/* set_GC0309_dummy */


void camsensor_gc0309_0p1mp_init_camsensor_info()
{
	camsensor_info.pclk_invert=FALSE;
	camsensor_info.preview_dummy_pixels = 0;
	format_preview= CAMIF_YCbCr_Y_Cb_Y_Cr;
	format_snapshot= CAMIF_YCbCr_Y_Cb_Y_Cr;
	camsensor_camclk_po_hz=24000000;

	switch(camsensor_info.msm_id)
	{
		ERR("camsensor_gc0309_0p1mp_init_camsensor_info : %d",camsensor_info.msm_id,0,0);
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

}/* camsensor_gc0309_0p1mp_init_camsensor_info */


camera_ret_code_type camsensor_gc0309_0p1mp_set_exposure_mode(int8 mode)
{
	return CAMERA_SUCCESS;
}

camera_ret_code_type camsensor_gc0309_0p1mp_set_frame_rate
(
  uint16 fps
)
{
	if (fps > CAMSENSOR_30_FPS)
	{
		return CAMERA_INVALID_PARM;
	} 

	return CAMERA_SUCCESS;
} /* camsensor_gc0309_0p1mp_set_frame_rate() */

boolean camsensor_gc0309_0p1mp_init(camsensor_function_table_type *camsensor_function_table_ptr,
				   camctrl_tbl_type              *camctrl_tbl_ptr)
{
	uint8	sensor_id;
	
	ERR("camsensor_gc0309_0p1mp_init!",0,0,0);
	/* Input MCLK = 24MHz */
	camsensor_camclk_po_hz = 24000000;

	/* Preview must aways be se to quater size */
   	camsensor_preview_resolution  = CAMSENSOR_QTR_SIZE;
   	camsensor_snapshot_resolution = CAMSENSOR_FULL_SIZE;
  	
	/* Configure I2C parameters */
	camsensor_i2c_command.bus_id     = I2C_BUS_HW_CTRL;
	camsensor_i2c_command.slave_addr = GC0309_0P1MP_I2C_SLAVE_ID;
	/*lint -save -e655 */
	camsensor_i2c_command.options    = (i2c_options_type) (I2C_REG_DEV | I2C_START_BEFORE_READ); 
	
  	gpio_config(CAMSENSOR_GC0309_0P1MP_RESET_PIN);
  	gpio_out(CAMSENSOR_GC0309_0P1MP_RESET_PIN,1);
  	clk_busy_wait(10000);
  	gpio_out(CAMSENSOR_GC0309_0P1MP_RESET_PIN,0);
  	clk_busy_wait(10000);
  	gpio_out(CAMSENSOR_GC0309_0P1MP_RESET_PIN,1);
  	clk_busy_wait(1000);
	
    if( !gc0309_0p1mp_i2c_read_byte(0x00,&sensor_id)) 
    {
    	ERR("read sensor_id failed!",0,0,0);
        return FALSE;
    }

    ERR("sensor_id = %x",sensor_id,0,0);
    
	/* Check if it matches it with the value in Datasheet */
	if ( sensor_id != GC0309_0P1MP_SENSOR_ID)
	{
		return FALSE;
	}
	
	//gc0309_0p1mp_sensor_mode = SENSOR_PREVIEW_MODE;
	/* Initialize Sensor registers */
	if (!initialize_gc0309_0p1mp_registers())
	{
		return FALSE;
	}

	/* Register function table: */
	camsensor_gc0309_0p1mp_register(camsensor_function_table_ptr);

	/* Setup camctrl_tbl */
	camsensor_gc0309_0p1mp_setup_camctrl_tbl(camctrl_tbl_ptr);

	return TRUE;
} /* camsensor_gc0309_0p1mp_init */

static boolean initialize_gc0309_0p1mp_registers(void)
{
    // Initail Sequence Write In.
	/*init registers code.*/
	gc0309_0p1mp_i2c_write_byte(0xfe,0x80);   	
		
	GC0309_0P1MP_SET_PAGE0;       // set page0
	
	gc0309_0p1mp_i2c_write_byte(0x1a,0x16);   	
	gc0309_0p1mp_i2c_write_byte(0xd2,0x10);   // close AEC
	gc0309_0p1mp_i2c_write_byte(0x22,0x55);   // close AWB

	gc0309_0p1mp_i2c_write_byte(0x5a,0x56); 
	gc0309_0p1mp_i2c_write_byte(0x5b,0x40);
	gc0309_0p1mp_i2c_write_byte(0x5c,0x4a);		
	
	gc0309_0p1mp_i2c_write_byte(0x22,0x57);  // Open AWB
		
	gc0309_0p1mp_i2c_write_byte(0x01,0xfa); 
	gc0309_0p1mp_i2c_write_byte(0x02,0x70); 
	gc0309_0p1mp_i2c_write_byte(0x0f,0x01); 

	gc0309_0p1mp_i2c_write_byte(0xe2,0x00); 
	gc0309_0p1mp_i2c_write_byte(0xe3,0x64); 

	gc0309_0p1mp_i2c_write_byte(0x03,0x01); 
	gc0309_0p1mp_i2c_write_byte(0x04,0x2c); 

	gc0309_0p1mp_i2c_write_byte(0xd2,0x90);  

	/*
	gc0309_0p1mp_i2c_write_byte(0x01,0x6a); 
	gc0309_0p1mp_i2c_write_byte(0x02,0x25); 
	gc0309_0p1mp_i2c_write_byte(0x0f,0x00);

	gc0309_0p1mp_i2c_write_byte(0xe2,0x00); 
	gc0309_0p1mp_i2c_write_byte(0xe3,0x4b); 
		
	gc0309_0p1mp_i2c_write_byte(0xe4,0x02); 
	gc0309_0p1mp_i2c_write_byte(0xe5,0x0d); 
	gc0309_0p1mp_i2c_write_byte(0xe6,0x02); 
	gc0309_0p1mp_i2c_write_byte(0xe7,0x0d); 
	gc0309_0p1mp_i2c_write_byte(0xe8,0x02); 
	gc0309_0p1mp_i2c_write_byte(0xe9,0x0d); 
	gc0309_0p1mp_i2c_write_byte(0xea,0x05); 
	gc0309_0p1mp_i2c_write_byte(0xeb,0xdc); 
	*/

	gc0309_0p1mp_i2c_write_byte(0x05,0x00);
	gc0309_0p1mp_i2c_write_byte(0x06,0x00);
	gc0309_0p1mp_i2c_write_byte(0x07,0x00); 
	gc0309_0p1mp_i2c_write_byte(0x08,0x00); 
	gc0309_0p1mp_i2c_write_byte(0x09,0x01); 
	gc0309_0p1mp_i2c_write_byte(0x0a,0xe8); 
	gc0309_0p1mp_i2c_write_byte(0x0b,0x02); 
	gc0309_0p1mp_i2c_write_byte(0x0c,0x88); 
	gc0309_0p1mp_i2c_write_byte(0x0d,0x02); 
	gc0309_0p1mp_i2c_write_byte(0x0e,0x02); 
	gc0309_0p1mp_i2c_write_byte(0x10,0x22); 
	gc0309_0p1mp_i2c_write_byte(0x11,0x0d); 
	gc0309_0p1mp_i2c_write_byte(0x12,0x2a); 
	gc0309_0p1mp_i2c_write_byte(0x13,0x00); 
	gc0309_0p1mp_i2c_write_byte(0x15,0x0a); 
	gc0309_0p1mp_i2c_write_byte(0x16,0x05); 
	gc0309_0p1mp_i2c_write_byte(0x17,0x01); 

	gc0309_0p1mp_i2c_write_byte(0x1b,0x00); 
	gc0309_0p1mp_i2c_write_byte(0x1c,0xc1); 
	gc0309_0p1mp_i2c_write_byte(0x1d,0x08); 
	gc0309_0p1mp_i2c_write_byte(0x1e,0x20); 
	gc0309_0p1mp_i2c_write_byte(0x1f,0x16); 

	gc0309_0p1mp_i2c_write_byte(0x20,0xff); 
	gc0309_0p1mp_i2c_write_byte(0x21,0xf8); 
	gc0309_0p1mp_i2c_write_byte(0x24,0xa2); 
	gc0309_0p1mp_i2c_write_byte(0x25,0x0f);
	//output sync_mode
	gc0309_0p1mp_i2c_write_byte(0x26,0x03); 
	gc0309_0p1mp_i2c_write_byte(0x2f,0x01); 
	/////////////////////////////////////////////////////////////////////
	/////////////////////////// grab_t ////////////////////////////////
	/////////////////////////////////////////////////////////////////////
	gc0309_0p1mp_i2c_write_byte(0x30,0xf7); 
	gc0309_0p1mp_i2c_write_byte(0x31,0x40);
	gc0309_0p1mp_i2c_write_byte(0x32,0x00); 
	gc0309_0p1mp_i2c_write_byte(0x39,0x04); 
	gc0309_0p1mp_i2c_write_byte(0x3a,0x20); 
	gc0309_0p1mp_i2c_write_byte(0x3b,0x20); 
	gc0309_0p1mp_i2c_write_byte(0x3c,0x02); 
	gc0309_0p1mp_i2c_write_byte(0x3d,0x02); 
	gc0309_0p1mp_i2c_write_byte(0x3e,0x02);
	gc0309_0p1mp_i2c_write_byte(0x3f,0x02); 
	
	//gain
	gc0309_0p1mp_i2c_write_byte(0x50,0x24); 
	
	gc0309_0p1mp_i2c_write_byte(0x53,0x82); 
	gc0309_0p1mp_i2c_write_byte(0x54,0x80); 
	gc0309_0p1mp_i2c_write_byte(0x55,0x80); 
	gc0309_0p1mp_i2c_write_byte(0x56,0x82); 
	
	/////////////////////////////////////////////////////////////////////
	/////////////////////////// LSC_t  ////////////////////////////////
	/////////////////////////////////////////////////////////////////////
	gc0309_0p1mp_i2c_write_byte(0x8b,0x20); 
	gc0309_0p1mp_i2c_write_byte(0x8c,0x20); 
	gc0309_0p1mp_i2c_write_byte(0x8d,0x20); 
	gc0309_0p1mp_i2c_write_byte(0x8e,0x10); 
	gc0309_0p1mp_i2c_write_byte(0x8f,0x10); 
	gc0309_0p1mp_i2c_write_byte(0x90,0x10); 
	gc0309_0p1mp_i2c_write_byte(0x91,0x3c); 
	gc0309_0p1mp_i2c_write_byte(0x92,0x50); 
	gc0309_0p1mp_i2c_write_byte(0x5d,0x12); 
	gc0309_0p1mp_i2c_write_byte(0x5e,0x1a); 
	gc0309_0p1mp_i2c_write_byte(0x5f,0x24); 
	/////////////////////////////////////////////////////////////////////
	/////////////////////////// DNDD_t  ///////////////////////////////
	/////////////////////////////////////////////////////////////////////
	gc0309_0p1mp_i2c_write_byte(0x60,0x07); 
	gc0309_0p1mp_i2c_write_byte(0x61,0x0e); 
	gc0309_0p1mp_i2c_write_byte(0x62,0x0c); 
	gc0309_0p1mp_i2c_write_byte(0x64,0x03); 
	gc0309_0p1mp_i2c_write_byte(0x66,0xe8); 
	gc0309_0p1mp_i2c_write_byte(0x67,0x86); 
	gc0309_0p1mp_i2c_write_byte(0x68,0xa2); 
	
	/////////////////////////////////////////////////////////////////////
	/////////////////////////// asde_t ///////////////////////////////
	/////////////////////////////////////////////////////////////////////
	gc0309_0p1mp_i2c_write_byte(0x69,0x20); 
	gc0309_0p1mp_i2c_write_byte(0x6a,0x0f); 
	gc0309_0p1mp_i2c_write_byte(0x6b,0x00); 
	gc0309_0p1mp_i2c_write_byte(0x6c,0x53); 
	gc0309_0p1mp_i2c_write_byte(0x6d,0x83); 
	gc0309_0p1mp_i2c_write_byte(0x6e,0xac); 
	gc0309_0p1mp_i2c_write_byte(0x6f,0xac); 
	gc0309_0p1mp_i2c_write_byte(0x70,0x15); 
	gc0309_0p1mp_i2c_write_byte(0x71,0x33); 
	/////////////////////////////////////////////////////////////////////
	/////////////////////////// eeintp_t///////////////////////////////
	/////////////////////////////////////////////////////////////////////
	gc0309_0p1mp_i2c_write_byte(0x72,0xdc);  
	gc0309_0p1mp_i2c_write_byte(0x73,0x80);  
	//for high resolution in light scene
	gc0309_0p1mp_i2c_write_byte(0x74,0x02); 
	gc0309_0p1mp_i2c_write_byte(0x75,0x3f); 
	gc0309_0p1mp_i2c_write_byte(0x76,0x02); 
	gc0309_0p1mp_i2c_write_byte(0x77,0x54); 
	gc0309_0p1mp_i2c_write_byte(0x78,0x88); 
	gc0309_0p1mp_i2c_write_byte(0x79,0x81); 
	gc0309_0p1mp_i2c_write_byte(0x7a,0x81); 
	gc0309_0p1mp_i2c_write_byte(0x7b,0x22); 
	gc0309_0p1mp_i2c_write_byte(0x7c,0xff);
	
	
	/////////////////////////////////////////////////////////////////////
	///////////////////////////CC_t///////////////////////////////
	/////////////////////////////////////////////////////////////////////
	gc0309_0p1mp_i2c_write_byte(0x93,0x45); 
	gc0309_0p1mp_i2c_write_byte(0x94,0x00); 
	gc0309_0p1mp_i2c_write_byte(0x95,0x00); 
	gc0309_0p1mp_i2c_write_byte(0x96,0x00); 
	gc0309_0p1mp_i2c_write_byte(0x97,0x45); 
	gc0309_0p1mp_i2c_write_byte(0x98,0xf0); 
	gc0309_0p1mp_i2c_write_byte(0x9c,0x00); 
	gc0309_0p1mp_i2c_write_byte(0x9d,0x03); 
	gc0309_0p1mp_i2c_write_byte(0x9e,0x00); 
	
	
	/////////////////////////////////////////////////////////////////////
	///////////////////////////YCP_t///////////////////////////////
	/////////////////////////////////////////////////////////////////////
	gc0309_0p1mp_i2c_write_byte(0xb1,0x40); 
	gc0309_0p1mp_i2c_write_byte(0xb2,0x40); 
	gc0309_0p1mp_i2c_write_byte(0xb8,0x20); 
	gc0309_0p1mp_i2c_write_byte(0xbe,0x36); 
	gc0309_0p1mp_i2c_write_byte(0xbf,0x00); 
	/////////////////////////////////////////////////////////////////////
	///////////////////////////AEC_t///////////////////////////////
	/////////////////////////////////////////////////////////////////////
	gc0309_0p1mp_i2c_write_byte(0xd0,0xc9);  
	gc0309_0p1mp_i2c_write_byte(0xd1,0x10);  
//	gc0309_0p1mp_i2c_write_byte(0xd2,0x90);  
	gc0309_0p1mp_i2c_write_byte(0xd3,0x80);  
	gc0309_0p1mp_i2c_write_byte(0xd5,0xf2); 
	gc0309_0p1mp_i2c_write_byte(0xd6,0x16);  
	gc0309_0p1mp_i2c_write_byte(0xdb,0x92); 
	gc0309_0p1mp_i2c_write_byte(0xdc,0xa5);  
	gc0309_0p1mp_i2c_write_byte(0xdf,0x23);   
	gc0309_0p1mp_i2c_write_byte(0xd9,0x00);  
	gc0309_0p1mp_i2c_write_byte(0xda,0x00);  
	gc0309_0p1mp_i2c_write_byte(0xe0,0x09);

	gc0309_0p1mp_i2c_write_byte(0xec,0x20);  
	gc0309_0p1mp_i2c_write_byte(0xed,0x04);  
	gc0309_0p1mp_i2c_write_byte(0xee,0xa0);  
	gc0309_0p1mp_i2c_write_byte(0xef,0x40);  
	///////////////////////////////////////////////////////////////////
	///////////////////////////GAMMA//////////////////////////////////
	///////////////////////////////////////////////////////////////////
#if 0	
	gc0309_0p1mp_i2c_write_byte(0x9F,0x0F);           
	gc0309_0p1mp_i2c_write_byte(0xA0,0x1D);  
	gc0309_0p1mp_i2c_write_byte(0xA1,0x2D);
	gc0309_0p1mp_i2c_write_byte(0xA2,0x3B);
	gc0309_0p1mp_i2c_write_byte(0xA3,0x46);
	gc0309_0p1mp_i2c_write_byte(0xA4,0x50);
	gc0309_0p1mp_i2c_write_byte(0xA5,0x5A);
	gc0309_0p1mp_i2c_write_byte(0xA6,0x6B);
	gc0309_0p1mp_i2c_write_byte(0xA7,0x7B);
	gc0309_0p1mp_i2c_write_byte(0xA8,0x8A);
	gc0309_0p1mp_i2c_write_byte(0xA9,0x98);
	gc0309_0p1mp_i2c_write_byte(0xAA,0xA5);
	gc0309_0p1mp_i2c_write_byte(0xAB,0xB2);
	gc0309_0p1mp_i2c_write_byte(0xAC,0xBE);
	gc0309_0p1mp_i2c_write_byte(0xAD,0xD5);
	gc0309_0p1mp_i2c_write_byte(0xAE,0xEB);
	gc0309_0p1mp_i2c_write_byte(0xAF,0xFE);
#endif
	//Y_gamma
	gc0309_0p1mp_i2c_write_byte(0xc0,0x00);
	gc0309_0p1mp_i2c_write_byte(0xc1,0x0B);
	gc0309_0p1mp_i2c_write_byte(0xc2,0x15);
	gc0309_0p1mp_i2c_write_byte(0xc3,0x27);
	gc0309_0p1mp_i2c_write_byte(0xc4,0x39);
	gc0309_0p1mp_i2c_write_byte(0xc5,0x49);
	gc0309_0p1mp_i2c_write_byte(0xc6,0x5A);
	gc0309_0p1mp_i2c_write_byte(0xc7,0x6A);
	gc0309_0p1mp_i2c_write_byte(0xc8,0x89);
	gc0309_0p1mp_i2c_write_byte(0xc9,0xA8);
	gc0309_0p1mp_i2c_write_byte(0xca,0xC6);
	gc0309_0p1mp_i2c_write_byte(0xcb,0xE3);
	gc0309_0p1mp_i2c_write_byte(0xcc,0xFF);

	/////////////////////////////////////////////////////////////////
	/////////////////////////// ABS_t ///////////////////////////////
	/////////////////////////////////////////////////////////////////
	gc0309_0p1mp_i2c_write_byte(0xf0,0x02);
	gc0309_0p1mp_i2c_write_byte(0xf1,0x01);
	gc0309_0p1mp_i2c_write_byte(0xf2,0x00);  
	gc0309_0p1mp_i2c_write_byte(0xf3,0x30); 
	
	/////////////////////////////////////////////////////////////////
	/////////////////////////// Measure Window ///////////////////////
	/////////////////////////////////////////////////////////////////
	gc0309_0p1mp_i2c_write_byte(0xf7,0x04); 
	gc0309_0p1mp_i2c_write_byte(0xf8,0x02); 
	gc0309_0p1mp_i2c_write_byte(0xf9,0x9f);
	gc0309_0p1mp_i2c_write_byte(0xfa,0x78);

	//---------------------------------------------------------------
	GC0309_0P1MP_SET_PAGE1;

	/////////////////////////////////////////////////////////////////
	///////////////////////////AWB_p/////////////////////////////////
	/////////////////////////////////////////////////////////////////
	gc0309_0p1mp_i2c_write_byte(0x00,0xf5); 
	gc0309_0p1mp_i2c_write_byte(0x01,0x0a);  
	gc0309_0p1mp_i2c_write_byte(0x02,0x1a); 
	gc0309_0p1mp_i2c_write_byte(0x0a,0xa0); 
	gc0309_0p1mp_i2c_write_byte(0x0b,0x64); 
	gc0309_0p1mp_i2c_write_byte(0x0c,0x08);
	gc0309_0p1mp_i2c_write_byte(0x0e,0x4c); 
	gc0309_0p1mp_i2c_write_byte(0x0f,0x39); 
	gc0309_0p1mp_i2c_write_byte(0x11,0x3f); 
	gc0309_0p1mp_i2c_write_byte(0x13,0x11); 
	gc0309_0p1mp_i2c_write_byte(0x14,0x40);  
	gc0309_0p1mp_i2c_write_byte(0x15,0x40); 
	gc0309_0p1mp_i2c_write_byte(0x16,0xc2); 
	gc0309_0p1mp_i2c_write_byte(0x17,0xa8); 
	gc0309_0p1mp_i2c_write_byte(0x18,0x18);  
	gc0309_0p1mp_i2c_write_byte(0x19,0x40);  
	gc0309_0p1mp_i2c_write_byte(0x1a,0xd0); 
	gc0309_0p1mp_i2c_write_byte(0x1b,0xf5);  

	gc0309_0p1mp_i2c_write_byte(0x70,0x43); 
	gc0309_0p1mp_i2c_write_byte(0x71,0x58);  
	gc0309_0p1mp_i2c_write_byte(0x72,0x30);  
	gc0309_0p1mp_i2c_write_byte(0x73,0x48);  
	gc0309_0p1mp_i2c_write_byte(0x74,0x20);  
	gc0309_0p1mp_i2c_write_byte(0x75,0x60);  
	
	GC0309_0P1MP_SET_PAGE0;

	gc0309_0p1mp_write_more_registers();

	/*Customer can adjust GAMMA, MIRROR & UPSIDEDOWN here!*/
	gc0309_0p1mp_gamma_select(2);
  	gc0309_0p1mp_H_V_Switch(1);
	
	return TRUE;
} /* end of initialize_gc0309_0p1mp_registers. */

boolean camsensor_gc0309_0p1mp_start
(
  /* A pointer to the services layer static camsensor info struct */
  camsensor_static_params_type *camsensor_params
)
{

	ERR("camsensor_gc0309_0p1mp_set_antibanding!",0,0,0);
	camsensor_gc0309_0p1mp_init_camsensor_info();

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
	camsensor_params->sensor_model  = CAMSENSOR_ID_GC0309_0P1MP;

	/* CCD or CMOS */
	camsensor_params->sensor_type   = CAMSENSOR_CMOS;

	/* BAYER or YCbCr */
	camsensor_params->output_format = CAMSENSOR_YCBCR;

	/* A pointer to the sensor name for EXIF tags */
	camsensor_params->sensor_name = camsensor_gc0309_0p1mp_sensor_name;

	/* Pointer to sensor suffix */

	camsensor_params->sensor_suffix=camsensor_gc0309_0p1mp_sensor_suffix;

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

	camsensor_params->full_size_width  =  CAMSENSOR_GC0309_0P1MP_FULL_SIZE_WIDTH;
	camsensor_params->full_size_height =  CAMSENSOR_GC0309_0P1MP_FULL_SIZE_HEIGHT;


	/* Quarter size dimensions - 640x512 */

	camsensor_params->qtr_size_width  =  CAMSENSOR_GC0309_0P1MP_QTR_SIZE_WIDTH;
	camsensor_params->qtr_size_height =  CAMSENSOR_GC0309_0P1MP_QTR_SIZE_HEIGHT;


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
} /*camsensor_gc0309_0p1mp_start*/

static boolean gc0309_0p1mp_i2c_write_byte(uint8 offset, uint8 data) 
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
			ERR("gc0309_0p1mp_i2c_write_byte: OK %x,%x",offset,data,0);
			return TRUE;
		}
	}
	ERR("gc0309_0p1mp_i2c_write_byte: false %x,%x",offset,data,0);
	return FALSE;
}

static boolean gc0309_0p1mp_i2c_read_byte(uint8 offset, uint8 *data) 
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
			ERR("gc0309_0p1mp_i2c_read_byte: OK %x",offset,0,0);
			return TRUE;
		}
	}

	ERR("gc0309_0p1mp_i2c_read_byte: false %x",offset,0,0);
	return FALSE;
} /* end of gc0309_0p1mp_i2c_read_word. */


camera_ret_code_type camsensor_gc0309_0p1mp_set_effect(int8 effect)
{
	uint16 seteffect_delay=1;
	uint16 seteffect_zone=1;


	ERR("camsensor_gc0309_0p1mp_set_effect effect = %d",effect,0,0);
	
	switch ((camera_effect_type)effect)
	{
		case CAMERA_EFFECT_OFF:
		{
			gc0309_0p1mp_i2c_write_byte(0x23,0x00);
			gc0309_0p1mp_i2c_write_byte(0x2d,0x0a);
			gc0309_0p1mp_i2c_write_byte(0x20,0xff);
			gc0309_0p1mp_i2c_write_byte(0xd2,0x90);
			gc0309_0p1mp_i2c_write_byte(0x73,0x00);
			gc0309_0p1mp_i2c_write_byte(0x77,0x54);
			
			gc0309_0p1mp_i2c_write_byte(0xb3,0x40);
			gc0309_0p1mp_i2c_write_byte(0xb4,0x80);
			gc0309_0p1mp_i2c_write_byte(0xba,0x00);
			gc0309_0p1mp_i2c_write_byte(0xbb,0x00);
			break;
		}
		
		case CAMERA_EFFECT_MONO:
		{
			gc0309_0p1mp_i2c_write_byte(0x23,0x02);
			gc0309_0p1mp_i2c_write_byte(0x2d,0x0a);
			gc0309_0p1mp_i2c_write_byte(0x20,0xff);
			gc0309_0p1mp_i2c_write_byte(0xd2,0x90);
			gc0309_0p1mp_i2c_write_byte(0x73,0x00);

			gc0309_0p1mp_i2c_write_byte(0xb3,0x40);
			gc0309_0p1mp_i2c_write_byte(0xb4,0x80);
			gc0309_0p1mp_i2c_write_byte(0xba,0x50);
			gc0309_0p1mp_i2c_write_byte(0xbb,0xe0);
			break;
		}

		case CAMERA_EFFECT_NEGATIVE:
		{
			gc0309_0p1mp_i2c_write_byte(0x23,0x02);
			gc0309_0p1mp_i2c_write_byte(0x2d,0x0a);
			gc0309_0p1mp_i2c_write_byte(0x20,0xbf);
			gc0309_0p1mp_i2c_write_byte(0xd2,0x10);
			gc0309_0p1mp_i2c_write_byte(0x73,0x01);

			gc0309_0p1mp_i2c_write_byte(0x51,0x40);
			gc0309_0p1mp_i2c_write_byte(0x52,0x40);

			gc0309_0p1mp_i2c_write_byte(0xb3,0x40);
			gc0309_0p1mp_i2c_write_byte(0xb4,0x80);
			gc0309_0p1mp_i2c_write_byte(0xba,0x60);
			gc0309_0p1mp_i2c_write_byte(0xbb,0x00);
			break;
		}

		case CAMERA_EFFECT_SOLARIZE:
		{
			break;
		}

		case CAMERA_EFFECT_SEPIA:
		{
			gc0309_0p1mp_i2c_write_byte(0x23,0x02);
			gc0309_0p1mp_i2c_write_byte(0x2d,0x0a);
			gc0309_0p1mp_i2c_write_byte(0x20,0xff);
			gc0309_0p1mp_i2c_write_byte(0xd2,0x90);
			gc0309_0p1mp_i2c_write_byte(0x73,0x00);

			gc0309_0p1mp_i2c_write_byte(0xb3,0x40);
			gc0309_0p1mp_i2c_write_byte(0xb4,0x80);
			gc0309_0p1mp_i2c_write_byte(0xba,0xd0);
			gc0309_0p1mp_i2c_write_byte(0xbb,0x28);
			break;
		}
		
		case CAMERA_EFFECT_PASTEL:
		{
			gc0309_0p1mp_i2c_write_byte(0x23,0x02);
			gc0309_0p1mp_i2c_write_byte(0x2d,0x0a);
			gc0309_0p1mp_i2c_write_byte(0x20,0xbf);
			gc0309_0p1mp_i2c_write_byte(0xd2,0x10);
			gc0309_0p1mp_i2c_write_byte(0x73,0x01);

			gc0309_0p1mp_i2c_write_byte(0x51,0x40);
			gc0309_0p1mp_i2c_write_byte(0x52,0x40);

			gc0309_0p1mp_i2c_write_byte(0xb3,0x40);
			gc0309_0p1mp_i2c_write_byte(0xb4,0x80);
			gc0309_0p1mp_i2c_write_byte(0xba,0xc0);
			gc0309_0p1mp_i2c_write_byte(0xbb,0x20);
			break;
		}
		case CAMERA_EFFECT_MOSAIC:
		case CAMERA_EFFECT_RESIZE:
		default:
		{
			return CAMERA_NOT_SUPPORTED;
		}
	}

	camsensor_gc0309_0p1mp_effect = (camera_effect_type)effect;
	
	if(seteffect_zone> 0 && seteffect_zone< 25)
	{
		seteffect_delay=(double)(250*seteffect_zone)/(double)(current_fps/Q8);

		camera_timed_wait(seteffect_delay);
	}
	
	return CAMERA_SUCCESS;
} /* camsensor_gc0309_0p1mp_set_effect() */

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

boolean camsensor_gc0309_0p1mp_snapshot_config
(
	camsensor_static_params_type  *camsensor_params /* Other config params */
)
{
	volatile uint32 shutter=0;
    sensor_cap_state= TRUE;	

	ERR("camsensor_gc0309_0p1mp_snapshot_config!",0,0,0);
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
	shutter=read_gc0309_0p1mp_shutter();  
	write_gc0309_0p1mp_shutter(shutter);
		 

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

boolean camsensor_gc0309_0p1mp_raw_snapshot_config
(
   camsensor_static_params_type  *camsensor_params /* Other config params */
)
{
	ERR("camsensor_gc0309_0p1mp_raw_snapshot_config!",0,0,0);
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

boolean camsensor_gc0309_0p1mp_video_config
(
  camsensor_static_params_type *camsensor_params /* Other config params */
)
{
	sensor_cap_state = FALSE;
  	camsensor_params->format = format_preview;
 	camsensor_params->discardFirstFrame =  TRUE;
	
	ERR("camsensor_gc0309_0p1mp_video_config!",0,0,0);
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


void camsensor_gc0309_0p1mp_power_up(void)
{
	ERR("camsensor_gc0309_0p1mp_power_up!",0,0,0);
} /* camsensor_gc0309_0p1mp_power_up */

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

void camsensor_gc0309_0p1mp_power_down(void)
{
	ERR("camsensor_gc0309_0p1mp_power_down!",0,0,0);
} /* camsensor_gc0309_0p1mp_power_down */


camera_ret_code_type camsensor_gc0309_0p1mp_set_wb(int8 wb)
{ 
	uint8  temp_reg;
	
	gc0309_0p1mp_i2c_read_byte(0x22,&temp_reg);
		
	ERR("camsensor_gc0309_0p1mp_set_wb!",0,0,0);
	switch((camera_wb_type)wb)
	{
		case CAMERA_WB_AUTO:
		{
			gc0309_0p1mp_i2c_write_byte(0x5a,0x56); //for AWB can adjust back
			gc0309_0p1mp_i2c_write_byte(0x5b,0x40);
			gc0309_0p1mp_i2c_write_byte(0x5c,0x4a);			
			gc0309_0p1mp_i2c_write_byte(0x22,temp_reg|0x02);	 // Enable AWB
			break;
		}

		case CAMERA_WB_INCANDESCENT:
		{
			gc0309_0p1mp_i2c_write_byte(0x22,temp_reg&~0x02); 
			gc0309_0p1mp_i2c_write_byte(0x5a,0x48);
			gc0309_0p1mp_i2c_write_byte(0x5b,0x40);
			gc0309_0p1mp_i2c_write_byte(0x5c,0x5c);
			break;
		}
		case CAMERA_WB_FLUORESCENT:
		{
			gc0309_0p1mp_i2c_write_byte(0x22,temp_reg&~0x02);   
			gc0309_0p1mp_i2c_write_byte(0x5a,0x40);
			gc0309_0p1mp_i2c_write_byte(0x5b,0x42);
			gc0309_0p1mp_i2c_write_byte(0x5c,0x50);
			break;
		}
		case CAMERA_WB_DAYLIGHT:
		{
			gc0309_0p1mp_i2c_write_byte(0x22,temp_reg&~0x02);   
			gc0309_0p1mp_i2c_write_byte(0x5a,0x74);  //  50 45 40
			gc0309_0p1mp_i2c_write_byte(0x5b,0x52);
			gc0309_0p1mp_i2c_write_byte(0x5c,0x40);
			break;
		}
		
		case CAMERA_WB_CLOUDY_DAYLIGHT:
		{
			gc0309_0p1mp_i2c_write_byte(0x22,temp_reg&~0x02);   // Disable AWB 
			gc0309_0p1mp_i2c_write_byte(0x5a,0x8c); //WB_manual_gain // 5a 42 40
			gc0309_0p1mp_i2c_write_byte(0x5b,0x50);
			gc0309_0p1mp_i2c_write_byte(0x5c,0x40);
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


camera_ret_code_type camsensor_gc0309_0p1mp_set_ev_compensation(int32 compensation)
{
	camera_ret_code_type ret_val = CAMERA_SUCCESS;
	int16  parmCurNum = (int16)(compensation>>16);
	int32 max_parmVal = 12;
	int32 min_parmVal =-12;
	int32  parmVal= parmCurNum;
	uint32 new_luma_target;

	ERR("camsensor_gc0309_0p1mp_set_ev_compensation!",0,0,0);
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
        		gc0309_0p1mp_i2c_write_byte(0xb5, 0xc0);
				gc0309_0p1mp_i2c_write_byte(0xd3, 0x60);
       			break;

		    case 9:    // -3 EV
		        gc0309_0p1mp_i2c_write_byte(0xb5, 0xd0);
				gc0309_0p1mp_i2c_write_byte(0xd3, 0x68);
		        break;

		    case 10:    // -2 EV
		        gc0309_0p1mp_i2c_write_byte(0xb5, 0xe0);
				gc0309_0p1mp_i2c_write_byte(0xd3, 0x70);
		        break;

		    case 11:    // -1 EV
		        gc0309_0p1mp_i2c_write_byte(0xb5, 0xf0);
				gc0309_0p1mp_i2c_write_byte(0xd3, 0x78);
		        break;

		    case 12:   // +0 EV
		        gc0309_0p1mp_i2c_write_byte(0xb5, 0x00);
				gc0309_0p1mp_i2c_write_byte(0xd3, 0x80);
		        break;

		    case 13:    // +1 EV
		        gc0309_0p1mp_i2c_write_byte(0xb5, 0x20);
				gc0309_0p1mp_i2c_write_byte(0xd3, 0x88);
		        break;

		    case 14:    // +2 EV
		        gc0309_0p1mp_i2c_write_byte(0xb5, 0x30);
				gc0309_0p1mp_i2c_write_byte(0xd3, 0x90);
		        break;

		    case 15:    // +3 EV
		        gc0309_0p1mp_i2c_write_byte(0xb5, 0x40);
				gc0309_0p1mp_i2c_write_byte(0xd3, 0x98);
		        break;

		    case 16:    // +4 EV
		        gc0309_0p1mp_i2c_write_byte(0xb5, 0x50);
				gc0309_0p1mp_i2c_write_byte(0xd3, 0xa0);
		        break;

		    default:
		    	break;
	    }
	}
	
	return ret_val;
}

camera_ret_code_type camsensor_gc0309_0p1mp_set_brightness(int8 brightness)
{
	ERR("camsensor_gc0309_0p1mp_set_brightness!",0,0,0);
	return CAMERA_SUCCESS;
}


camera_ret_code_type camsensor_gc0309_0p1mp_set_contrast(int8 contrast)
{
	ERR("camsensor_gc0309_0p1mp_set_contrast!",0,0,0);
	return CAMERA_SUCCESS;
}/* camsensor_gc0309_0p1mp_set_contrast() */


static void camsensor_gc0309_0p1mp_setup_camctrl_tbl(camctrl_tbl_type *camctrl_tbl_ptr)
{
	ERR("camsensor_gc0309_0p1mp_get_snapshot_fps!",0,0,0);
	camctrl_tbl_ptr->asf_5x5_is_supported         = FALSE; 

	camctrl_tbl_ptr->get_snapshot_frame_per_sec = camsensor_gc0309_0p1mp_get_snapshot_fps;
}


LOCAL uint16 camsensor_gc0309_0p1mp_get_snapshot_fps(uint16 fps)
{
	ERR("camsensor_gc0309_0p1mp_get_snapshot_fps!",0,0,0);
	/* In the current driver, the snapshot fps is always 11 */
	fps = 15 * Q8;
	return fps;
}

camera_ret_code_type camsensor_gc0309_0p1mp_set_antibanding(int8 antibanding)
{
	ERR("camsensor_ov2655_set_antibanding!",0,0,0);
	
	return CAMERA_SUCCESS;
}

static void camsensor_gc0309_0p1mp_register(camsensor_function_table_type *camsensor_function_table_ptr)
{
	ERR("camsensor_gc0309_0p1mp_register!",0,0,0);
	camsensor_function_table_ptr->camsensor_start                 = camsensor_gc0309_0p1mp_start;
	camsensor_function_table_ptr->camsensor_video_config          = camsensor_gc0309_0p1mp_video_config;
	camsensor_function_table_ptr->camsensor_raw_snapshot_config   = camsensor_gc0309_0p1mp_raw_snapshot_config;
	camsensor_function_table_ptr->camsensor_snapshot_config       = camsensor_gc0309_0p1mp_snapshot_config;
	camsensor_function_table_ptr->camsensor_power_up              = camsensor_gc0309_0p1mp_power_up;
	camsensor_function_table_ptr->camsensor_power_down            = camsensor_gc0309_0p1mp_power_down;
	camsensor_function_table_ptr->camsensor_set_frame_rate        = camsensor_gc0309_0p1mp_set_frame_rate;  

	camsensor_function_table_ptr->camsensor_set_effect            = camsensor_gc0309_0p1mp_set_effect;

	camsensor_function_table_ptr->camsensor_set_brightness        = camsensor_gc0309_0p1mp_set_brightness;
	camsensor_function_table_ptr->camsensor_set_contrast          = camsensor_gc0309_0p1mp_set_contrast;
	camsensor_function_table_ptr->camsensor_set_antibanding       = camsensor_gc0309_0p1mp_set_antibanding;
	camsensor_function_table_ptr->camsensor_set_exposure_mode 	  = camsensor_gc0309_0p1mp_set_exposure_mode;
	camsensor_function_table_ptr->camsensor_set_ev_compensation	  = camsensor_gc0309_0p1mp_set_ev_compensation; 
	camsensor_function_table_ptr->camsensor_set_wb				  = camsensor_gc0309_0p1mp_set_wb;
} /* camsensor_mt9d112_mu2m0yu_register */


#endif

