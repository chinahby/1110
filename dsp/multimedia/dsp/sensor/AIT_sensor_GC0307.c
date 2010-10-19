#include	"A8_common.h"
#include	"AIT700_EBIbus.h"
#include	"A8_sensor.h"
#include 	"ait_interface.h"

#define	SENSOR_ID					(0xa0)
#define	SENSOR_I2C_TYPE				(A8_I2C_1A1D)
#define SENSOR_PREVIEW_VH_POLARITY  (A8_SENSOR_HOR_POS|A8_SENSOR_VER_POS)
#define SENSOR_FULL_VH_POLARITY     (A8_SENSOR_HOR_POS|A8_SENSOR_VER_POS)
#define SENSOR_PREVIEW_LATCH_COUNT  (A8_PHASE_COUNTER_NUM_3)
#define SENSOR_FULL_LATCH_COUNT     (A8_PHASE_COUNTER_NUM_2)
#define SENSOR_PREVIEW_YUV_FORMAT   (A8_SENSOR_YCBYCR) //(A8_SENSOR_CBYCRY) //
#define SENSOR_FULL_YUV_FORMAT      (A8_SENSOR_YCRYCB) //(A8_SENSOR_CBYCRY) //
#define SENSOR_LATCH_EDGE           (A8_SENSOR_POS_EDGE)

#define	SENSOR_I2C_ID  			    (0x42)
#define	SENSOR_PREVIEW_WIDTH  	    (640)
#define	SENSOR_PREVIEW_HEIGHT  	    (480)
#define	SENSOR_FULL_WIDTH  		    (640)
#define	SENSOR_FULL_HEIGHT  	    (480)


u_short gc_0307_read_cmos_sensor(u_short Reg)
{
	u_short ret;
	A8L_GetSenReg(Reg,&ret);
	return ret;
}

u_short gc_0307_write_cmos_sensor(u_short Reg, u_short Value)
{

	return A8L_SetSenReg(Reg,Value);

}
typedef unsigned char kal_uint8;
//typedef unsigned char kal_bool; 
typedef unsigned short kal_uint16;
static t_sensor_mode_attr	full_mode;
static t_sensor_mode_attr	preview_mode;

extern	u_char	gbSensorInited;
unsigned char gc_effect_on;
unsigned char gc_reinit = 1;

uint8  gc_0307_start_grab_x_offset=0, gc_0307_start_grab_y_offset=0;
boolean  gc_0307_gVGAmode=KAL_TRUE,gc_0307_sensor_night_mode = KAL_FALSE;
boolean gc_0307_MPEG4_encode_mode=KAL_FALSE;


static u_short	Get_Sensor_ID(u_short*	SensorID);

uint8  gc_0307_normal_gain=0;//, gc_0307_night_gain=GC_0307_SENSOR_NIGHT_MODE_GAIN;
uint8  gc_0307_preview_pclk_division=0, gc_0307_capture_pclk_division=0;
uint16 gc_0307_dummy_pixels=0, gc_0307_dummy_lines=0, gc_0307_extra_exposure_lines=0;
uint16 gc_0307_exposure_lines=0,gc_0307_w_blank=0,gc_0307_h_blank=0;
//kal_uint16 gc_0307_sensor_global_gain=BASEGAIN, 
uint16 	gc_0307_sensor_gain_base=0x0;
boolean gc_0307_g_bMJPEG_mode;





__align(2)const static u_char	Sensor_Init[]=
{


#if 0
		0x43  ,0x00, 
		0x44  ,0xa2, 
	
	//========= close some functions 
	// open them after configure their parmameters
		0x40  ,0x10, 
		0x41  ,0x00, 			
		0x42  ,0x10,					  	
		0x47  ,0x00, //mode1,				  	
		0x48  ,0xc3, //mode2, 	
		0x49  ,0x00, //dither_mode 		
		0x4a  ,0x00, //clock_gating_en
		0x4b  ,0x00, //mode_reg3
		0x4E  ,0x23, //sync mode
		0x4F  ,0x01, //AWB, AEC, every N frame	
	
	//========= frame timing
		0x01  ,0x6a, //HB
		0x02  ,0x25, //VB
		0x1C  ,0x00, //Vs_st
		0x1D  ,0x00, //Vs_et
		0x10  ,0x00, //high 4 bits of VB, HB
		0x11  ,0x05, //row_tail,  AD_pipe_number
	
		0x03  ,0x01, //row_start
		0x04  ,0x2c,
	//========= windowing
		0x05  ,0x00, //row_start
		0x06  ,0x00,
		0x07  ,0x00, //col start
		0x08  ,0x00, 
		0x09  ,0x01, //win height
		0x0A  ,0xE8,
		0x0B  ,0x02, //win width, pixel array only 640
		0x0C  ,0x80,
	
	//========= analog
		0x0D  ,0x22, //rsh_width
					  
		0x0E  ,0x02, //CISCTL mode2,  

			  
		0x12  ,0x70, //7 hrst, 6_4 darsg,
		0x13  ,0x00, //7 CISCTL_restart, 0 apwd
		0x14  ,0x00, //NA
		0x15  ,0xba, //7_4 vref
		0x16  ,0x13, //5to4 _coln_r,  __1to0__da18
		0x17  ,0x52, //opa_r, ref_r, sRef_r
	//	0x18  ,0xc0, //analog_mode, best case for left band.
	
		0x1E  ,0x0d, //tsp_width 		   
		0x1F  ,0x32, //sh_delay
	
	//========= offset
		0x47  ,0x00,  //7__test_image, __6__fixed_pga, __5__auto_DN, __4__CbCr_fix, 
				//__3to2__dark_sequence, __1__allow_pclk_vcync, __0__LSC_test_image
		0x19  ,0x06,  //pga_o			 
		0x1a  ,0x06,  //pga_e			 
	
		0x31  ,0x00,  //4	//pga_oFFset ,	 high 8bits of 11bits
		0x3B  ,0x00,  //global_oFFset, low 8bits of 11bits
	
		0x59  ,0x0f,  //offset_mode 	
		0x58  ,0x88,  //DARK_VALUE_RATIO_G,  DARK_VALUE_RATIO_RB
		0x57  ,0x08,  //DARK_CURRENT_RATE
		0x56  ,0x77,  //PGA_OFFSET_EVEN_RATIO, PGA_OFFSET_ODD_RATIO
	
	//========= blk
		0x35  ,0xd8,  //blk_mode

		0x36  ,0x40,  
	
		0x3C  ,0x00, 
		0x3D  ,0x00, 
		0x3E  ,0x00, 
		0x3F  ,0x00, 
	
		0xb5  ,0x70, 
		0xb6  ,0x40, 
		0xb7  ,0x00, 
		0xb8  ,0x38, 
		0xb9  ,0xc3, 		  
		0xba  ,0x0f, 
	
		0x7e  ,0x35, 
		0x7f  ,0x86, 
	
		0x5c  ,0x78, //78
		0x5d  ,0x88, //88
	
	
	//========= manual_gain 
		0x61  ,0x80, //manual_gain_g1	
		0x63  ,0x80, //manual_gain_r
		0x65  ,0x98, //manual_gai_b, 0xa0=1.25, 0x98=1.1875
		0x67  ,0x80, //manual_gain_g2
		0x68  ,0x18, //global_manual_gain	 2.4bits
	
	//=========CC _R
		0x69  ,0x58,  //54
		0x6A  ,0xf6,  //ff
		0x6B  ,0xfb,  //fe
		0x6C  ,0xf4,  //ff
		0x6D  ,0x5a,  //5f
		0x6E  ,0xe6,  //e1

		0x6f  ,0x00, 	
	
	//=========lsc							  
		0x70  ,0x14, 
		0x71  ,0x1c, 
		0x72  ,0x20, 
	
		0x73  ,0x10, 	
		0x74  ,0x3c, 
		0x75  ,0x52, 
	
	//=========dn																			 
		0x7d  ,0x2f,  //dn_mode   	
		0x80  ,0x0c, //when auto_dn, check 7e,7f
		0x81  ,0x0c,
		0x82  ,0x44,
																						
	//dd																		   
		0x83  ,0x18,  //DD_TH1 					  
		0x84  ,0x18,  //DD_TH2 					  
		0x85  ,0x04,  //DD_TH3 																							  
		0x87  ,0x34,  //32 b DNDD_low_range X16,  DNDD_low_range_C_weight_center					
	
	   
	//=========intp-ee																		   
		0x88  ,0x04,  													   
		0x89  ,0x01,  										  
		0x8a  ,0x50,//60  										   
		0x8b  ,0x50,//60  										   
		0x8c  ,0x07,  												 				  
																					  
		0x50  ,0x0c,   						   		
		0x5f  ,0x3c, 																					 
																					 
		0x8e  ,0x02,  															  
		0x86  ,0x02,  																  
																					
		0x51  ,0x20,  																
		0x52  ,0x08,  
		0x53  ,0x00, 
	
	
	//========= YCP 
	//contrast_center																			  
		0x77  ,0x80, //contrast_center 																  
		0x78  ,0x00, //fixed_Cb																		  
		0x79  ,0x00, //fixed_Cr																		  
		0x7a  ,0x00, //luma_offset 																																							
		0x7b  ,0x40, //hue_cos 																		  
		0x7c  ,0x00, //hue_sin 																		  
																							 
	//saturation																				  
		0xa0  ,0x40, //global_saturation
		0xa1  ,0x44, //luma_contrast																	  
		0xa2  ,0x38, //saturation_Cb																	  
		0xa3  ,0x38, //saturation_Cr
																				
		0xa4  ,0xc8, 																  
		0xa5  ,0x02, 
		0xa6  ,0x28, 																			  
		0xa7  ,0x02, 
	
	//skin																								  
		0xa8  ,0xee, 															  
		0xa9  ,0x12, 															  
		0xaa  ,0x01, 														  
		0xab  ,0x20, 													  
		0xac  ,0xf0, 														  
		0xad  ,0x10, 															  
		
	//========= ABS
		0xae  ,0x18, 
		0xaf  ,0x74, 
		0xb0  ,0xe0, 	  
		0xb1  ,0x20, 
		0xb2  ,0x6c, 
		0xb3  ,0xc0, 
		0xb4  ,0x04, 
		
	//========= AWB 
		0xbb  ,0x42, 
		0xbc  ,0x60,
		0xbd  ,0x50,
		0xbe  ,0x50,
	
		0xbf  ,0x0c, 
		0xc0  ,0x06, 
		0xc1  ,0x60, 
		0xc2  ,0xf1,  //f1
		0xc3  ,0x40,
		0xc4  ,0x1c, //18//20
		0xc5  ,0x56,  //33
		0xc6  ,0x1d, 

		0xca  ,0x70, 
		0xcb  ,0x70, 
		0xcc  ,0x78,
	
		0xcd  ,0x80, //R_ratio 									 
		0xce  ,0x80, //G_ratio  , cold_white white 								   
		0xcf  ,0x80, //B_ratio  	
	
	//=========  aecT  
		0x20  ,0x06,//0x02 
		0x21  ,0xc0, 
		0x22  ,0x60,    
		0x23  ,0x88, 
		0x24  ,0x96, 
		0x25  ,0x30, 
		0x26  ,0xd0, 
		0x27  ,0x00, 
	
	/*	0x28  ,0x01, //AEC_exp_level_1bit11to8   
		0x29  ,0xf4, //AEC_exp_level_1bit7to0	  
		0x2a  ,0x02, //AEC_exp_level_2bit11to8   
		0x2b  ,0xbc, //AEC_exp_level_2bit7to0			 
		0x2c  ,0x03, //AEC_exp_level_3bit11to8   659 - 8FPS,  8ca - 6FPS  //	 
		0x2d  ,0xe8, //AEC_exp_level_3bit7to0			 
		0x2e  ,0x09, //AEC_exp_level_4bit11to8   4FPS 
		0x2f  ,0xc4, //AEC_exp_level_4bit7to0	 */

		0x28  ,0x02, //AEC_exp_level_1bit11to8   
		0x29  ,0x0d, //AEC_exp_level_1bit7to0	  
		0x2a  ,0x02, //AEC_exp_level_2bit11to8   
		0x2b  ,0x0d, //AEC_exp_level_2bit7to0			 
		0x2c  ,0x02, //AEC_exp_level_3bit11to8   659 - 8FPS,  8ca - 6FPS  //	 
		0x2d  ,0x0d, //AEC_exp_level_3bit7to0			 
		0x2e  ,0x05, //AEC_exp_level_4bit11to8   4FPS 
		0x2f  ,0xdc, //AEC_exp_level_4bit7to0	
	
		0x30  ,0x20, 						  
		0x31  ,0x00, 					   
		0x32  ,0x1c, 
		0x33  ,0x90, 			  
		0x34  ,0x10,	
	
		0xd0  ,0x34, 
			   
		0xd1  ,0x50, //AEC_target_Y						   
		0xd2  ,0x61,//0xf2 	  
		0xd4  ,0x96, 
		0xd5  ,0x01, // william 0318
		0xd6  ,0x4b, //antiflicker_step 					   
		0xd7  ,0x03, //AEC_exp_time_min ,william 20090312			   
		0xd8  ,0x02, 
			   
		0xdd  ,0x22,//0x12 
	  															
	//========= measure window										
		0xe0  ,0x03, 						 
		0xe1  ,0x02, 							 
		0xe2  ,0x27, 								 
		0xe3  ,0x1e, 				 
		0xe8  ,0x3b, 					 
		0xe9  ,0x6e, 						 
		0xea  ,0x2c, 					 
		0xeb  ,0x50, 					 
		0xec  ,0x73, 		 
	
	//========= close_frame													
		0xed  ,0x00, //close_frame_num1 ,can be use to reduce FPS				 
		0xee  ,0x00, //close_frame_num2  
		0xef  ,0x00, //close_frame_num
	
	// page1
		0xf0  ,0x01, //select page1 
	
		0x00  ,0x20, 							  
		0x01  ,0x20, 							  
		0x02  ,0x20, 									
		0x03  ,0x20, 							
		0x04  ,0x78, 
		0x05  ,0x78, 					 
		0x06  ,0x78, 								  
		0x07  ,0x78, 									 
	
	
	
		0x10  ,0x04, 						  
		0x11  ,0x04,							  
		0x12  ,0x04, 						  
		0x13  ,0x04, 							  
		0x14  ,0x01, 							  
		0x15  ,0x01, 							  
		0x16  ,0x01, 						 
		0x17  ,0x01, 						 
		  
													 
		0x20  ,0x00, 					  
		0x21  ,0x00, 					  
		0x22  ,0x00, 						  
		0x23  ,0x00, 						  
		0x24  ,0x00, 					  
		0x25  ,0x00, 						  
		0x26  ,0x00, 					  
		0x27  ,0x00,  						  
	
		0x40  ,0x11, 
	
	//=============================lscP 
		0x45  ,0x06, 	 
		0x46  ,0x06, 			 
		0x47  ,0x05, 
	
		0x48  ,0x04, 	
		0x49  ,0x03, 		 
		0x4a  ,0x03, 
	

		0x62  ,0xd8, 
		0x63  ,0x24, 
		0x64  ,0x24,
		0x65  ,0x24, 
		0x66  ,0xd8, 
		0x67  ,0x24,
	
		0x5a  ,0x00, 
		0x5b  ,0x00, 
		0x5c  ,0x00, 
		0x5d  ,0x00, 
		0x5e  ,0x00, 
		0x5f  ,0x00, 
	
	
	//============================= ccP 
	
		0x69  ,0x03, //cc_mode
		  
	//CC_G
		0x70  ,0x5d, 
		0x71  ,0xed, 
		0x72  ,0xff, 
		0x73  ,0xe5, 
		0x74  ,0x5f, 
		0x75  ,0xe6, 
	
      //CC_B
		0x76  ,0x41, 
		0x77  ,0xef, 
		0x78  ,0xff, 
		0x79  ,0xff, 
		0x7a  ,0x5f, 
		0x7b  ,0xfa, 	 
	
	
	//============================= AGP
	
		0x7e  ,0x00,  
		0x7f  ,0x20,   //00
		0x80  ,0x48,   //c8
		0x81  ,0x06,  
		0x82  ,0x08,  
	
		0x83  ,0x23,  
		0x84  ,0x38,  
		0x85  ,0x4F,  
		0x86  ,0x61,  
		0x87  ,0x72,  
		0x88  ,0x80,  
		0x89  ,0x8D,  
		0x8a  ,0xA2,  
		0x8b  ,0xB2,  
		0x8c  ,0xC0,  
		0x8d  ,0xCA,  
		0x8e  ,0xD3,  
		0x8f  ,0xDB,  
		0x90  ,0xE2,  
		0x91  ,0xED,  
		0x92  ,0xF6,  
		0x93  ,0xFD,  
	
	//about gamma1 is hex r oct
		0x94  ,0x04,  
		0x95  ,0x0E,  
		0x96  ,0x1B,  
		0x97  ,0x28,  
		0x98  ,0x35,  
		0x99  ,0x41,  
		0x9a  ,0x4E,  
		0x9b  ,0x67,  
		0x9c  ,0x7E,  
		0x9d  ,0x94,  
		0x9e  ,0xA7,  
		0x9f  ,0xBA,  
		0xa0  ,0xC8,  
		0xa1  ,0xD4,  
		0xa2  ,0xE7,  
		0xa3  ,0xF4,  
		0xa4  ,0xFA, 
	
	//========= open functions	
		0xf0  ,0x00, //set back to page0	
		0x40  ,0x7e, 
		0x41  ,0x2F,

/////  Ce¡Ñ¢FOa¢G?£g¡ÒOuGC0307£gA?£gInoI¡P-¡Ña¢G?DeOaI?E¡ÓDT¡MAEy¡Mo?A¡¦aA¡Ò¢G?EcIA:
		0x0F  ,0x30, //CISCTL mode1	
		0x45  ,0x27,
		0x47  ,0x2c, 	

/////  EAOO2?I?£gA¡P-¡ÑaoI?£gInEe?¡L¢G??I?¡±?EO¡Ó?O¡M¡¦OA!!!!!!


#if 0
//  IMAGE_NORMAL:
	write_cmos_sensor(0x0f, 0x30);
	write_cmos_sensor(0x45, 0x27);
	write_cmos_sensor(0x47, 0x2c);			

// IMAGE_H_MIRROR:
	write_cmos_sensor(0x0f, 0x20);
	write_cmos_sensor(0x45, 0x26);
	write_cmos_sensor(0x47, 0x28);	
	
// IMAGE_V_MIRROR:			
	write_cmos_sensor(0x0f, 0x10);
	write_cmos_sensor(0x45, 0x25);
	write_cmos_sensor(0x47, 0x24);			

// IMAGE_HV_MIRROR:	   // 180
	write_cmos_sensor(0x0f, 0x00);
	write_cmos_sensor(0x45, 0x24);
	write_cmos_sensor(0x47, 0x20);	
#endif

#else
//addr, data
// Initail Sequence Write In.

	//========= close output
		//0x18 ,0xf0,
	 	0x43  ,0x00, 
	 	0x44  ,0xa2, 
	
	//========= close some functions
	// open them after configure their parmameters
	 	0x40  ,0x10, 
	 	0x41  ,0x00, 			
	 	0x42  ,0x10,					  	
	 	0x47  ,0x00, 			  	
	 	0x48  ,0xc3, 
	 	0x49  ,0x00,	
	 	0x4a  ,0x00,
	 	0x4b  ,0x00, 
	 	0x4E  ,0x23,
	 	0x4F  ,0x01, 
	//========= frame timing
		0x01  ,0x6a, //HB
		0x02  ,0x0c, //VB
		0x1C  ,0x00, //Vs_st
		0x1D  ,0x00, //Vs_et
		0x10  ,0x00, //high 4 bits of VB, HB
		0x11  ,0x05, //row_tail,  AD_pipe_number
	
	
	//========= windowing
		0x05  ,0x00, //row_start
		0x06  ,0x00,
		0x07  ,0x00, //col start
		0x08  ,0x00, 
		0x09  ,0x01, //win height
		0x0A  ,0xE8,
		0x0B  ,0x02, //win width, pixel array only 640
		0x0C  ,0x80,
	
	//========= analog
		0x0D  ,0x22, //rsh_width
					  
		0x0E  ,0x02, //CISCTL mode2,  

			  
		0x12  ,0x70, //7 hrst, 6_4 darsg,
		0x13  ,0x00, //7 CISCTL_restart, 0 apwd
		0x14  ,0x00, //NA
		0x15  ,0xba, //7_4 vref
		0x16  ,0x13, //5to4 _coln_r,  __1to0__da18
		0x17  ,0x52, //opa_r, ref_r, sRef_r
	//write_cmos_sensor(	0x18  ,0xc0, //analog_mode, best case for left band.
	
		0x1E  ,0x0d, //tsp_width 		   
		0x1F  ,0x32, //sh_delay
	
	//========= offset
		0x47  ,0x00,  //7__test_image, __6__fixed_pga, __5__auto_DN, __4__CbCr_fix, 
				//__3to2__dark_sequence, __1__allow_pclk_vcync, __0__LSC_test_image
		0x19  ,0x06,  //pga_o			 
		0x1a  ,0x06,  //pga_e			 
	
		0x31  ,0x00,  //4	//pga_oFFset ,	 high 8bits of 11bits
		0x3B  ,0x00,  //global_oFFset, low 8bits of 11bits
	
		0x59  ,0x0f,  //offset_mode 	
		0x58  ,0x88,  //DARK_VALUE_RATIO_G,  DARK_VALUE_RATIO_RB
		0x57  ,0x08,  //DARK_CURRENT_RATE
		0x56  ,0x77,  //PGA_OFFSET_EVEN_RATIO, PGA_OFFSET_ODD_RATIO
	
	//========= blk
		0x35  ,0xd8,  //blk_mode

		0x36  ,0x40,  
	
		0x3C  ,0x00, 
		0x3D  ,0x00, 
		0x3E  ,0x00, 
		0x3F  ,0x00, 
	
		0xb5  ,0x70, 
		0xb6  ,0x40, 
		0xb7  ,0x00, 
		0xb8  ,0x38, 
		0xb9  ,0xc3, 		  
		0xba  ,0x0f, 
	
		0x7e  ,0x35, 
		0x7f  ,0x86, 
	
		0x5c  ,0x68, //78
		0x5d  ,0x78, //88
	
	
	//========= manual_gain 
		0x61  ,0x80, //manual_gain_g1	
		0x63  ,0x80, //manual_gain_r
		0x65  ,0x98, //manual_gai_b, 0xa0=1.25, 0x98=1.1875
		0x67  ,0x80, //manual_gain_g2
		0x68  ,0x18, //global_manual_gain	 2.4bits
	
	//=========CC _R
		0x69  ,0x58,  //54
		0x6A  ,0xf6,  //ff
		0x6B  ,0xfb,  //fe
		0x6C  ,0xf4,  //ff
		0x6D  ,0x5a,  //5f
		0x6E  ,0xe6,  //e1

		0x6f  ,0x00, 	
	
	//=========lsc							  
		0x70  ,0x14, 
		0x71  ,0x1c, 
		0x72  ,0x20, 
	
		0x73  ,0x10, 	
		0x74  ,0x3c, 
		0x75  ,0x52, 
	
	//=========dn																			 
		0x7d  ,0x2f,  //dn_mode   	
		0x80  ,0x0c, //when auto_dn, check 7e,7f
		0x81  ,0x0c,
		0x82  ,0x44,
																						
	//dd																		   
		0x83  ,0x18,  //DD_TH1 					  
		0x84  ,0x18,  //DD_TH2 					  
		0x85  ,0x04,  //DD_TH3 																							  
		0x87  ,0x34,  //32 b DNDD_low_range X16,  DNDD_low_range_C_weight_center					
	
	   
	//=========intp-ee																		   
		0x88  ,0x04,  													   
		0x89  ,0x01,  										  
		0x8a  ,0x50,//60  										   
		0x8b  ,0x50,//60  										   
		0x8c  ,0x07,  												 				  
																					  
		0x50  ,0x0c,   						   		
		0x5f  ,0x3c, 																					 
																					 
		0x8e  ,0x02,  															  
		0x86  ,0x02,  																  
																					
		0x51  ,0x20,  																
		0x52  ,0x08,  
		0x53  ,0x00, 
	
	
	//========= YCP 
	//contrast_center																			  
		0x77  ,0x80, //contrast_center 																  
		0x78  ,0x00, //fixed_Cb																		  
		0x79  ,0x00, //fixed_Cr																		  
		0x7a  ,0x00, //luma_offset 																																							
		0x7b  ,0x40, //hue_cos 																		  
		0x7c  ,0x00, //hue_sin 																		  
																							 
	//saturation																				  
		0xa0  ,0x40, //global_saturation
		0xa1  ,0x40, //luma_contrast																	  
		0xa2  ,0x34, //saturation_Cb																	  
		0xa3  ,0x34, //saturation_Cr
																				
		0xa4  ,0xc8, 																  
		0xa5  ,0x02, 
		0xa6  ,0x28, 																			  
		0xa7  ,0x02, 
	
	//skin																								  
		0xa8  ,0xee, 															  
		0xa9  ,0x12, 															  
		0xaa  ,0x01, 														  
		0xab  ,0x20, 													  
		0xac  ,0xf0, 														  
		0xad  ,0x10, 															  
		
	//========= ABS
		0xae  ,0x18, 
		0xaf  ,0x74, 
		0xb0  ,0xe0, 	  
		0xb1  ,0x20, 
		0xb2  ,0x6c, 
		0xb3  ,0x40, 
		0xb4  ,0x04, 
		
	//========= AWB 
		0xbb  ,0x42, 
		0xbc  ,0x60, 
		0xbd  ,0x50, 
		0xbe  ,0x50, 
	
		0xbf  ,0x0c, 
		0xc0  ,0x06, 
		0xc1  ,0x60, 
		0xc2  ,0xf1,  //f4
		0xc3  ,0x40, 
		0xc4  ,0x1c, //18
		0xc5  ,0x56, 
		0xc6  ,0x1d,   

		0xca  ,0x70, 
		0xcb  ,0x70, 
		0xcc  ,0x78,
	
		0xcd  ,0x80, //R_ratio 									 
		0xce  ,0x80, //G_ratio  , cold_white white 								   
		0xcf  ,0x80, //B_ratio  	
	
	//=========  aecT  
		0x20  ,0x06, 
		0x21  ,0xc0, 
		0x22  ,0x60,    
		0x23  ,0x88, 
		0x24  ,0x96, 
		0x25  ,0x30, 
		0x26  ,0xd0, 
		0x27  ,0x00, 
	
		0x28  ,0x01, //AEC_exp_level_1bit11to8   
		0x29  ,0xf4, //AEC_exp_level_1bit7to0	  
		0x2a  ,0x02, //AEC_exp_level_2bit11to8   
		0x2b  ,0xbc, //AEC_exp_level_2bit7to0			 
		0x2c  ,0x03, //AEC_exp_level_3bit11to8   659 - 8FPS,  8ca - 6FPS  //	 
		0x2d  ,0xe8, //AEC_exp_level_3bit7to0			 
		0x2e  ,0x09, //AEC_exp_level_4bit11to8   4FPS 
		0x2f  ,0xc4, //AEC_exp_level_4bit7to0	 
	
		0x30  ,0x20, 						  
		0x31  ,0x00, 					   
		0x32  ,0x1c, 
		0x33  ,0x90, 			  
		0x34  ,0x10,	
	
		0xd0  ,0x34, 
			   
		0xd1  ,0x50, //AEC_target_Y						   
		0xd2  ,0x61, 	  
		0xd4  ,0x96, 
		0xd5  ,0x01, 
		0xd6  ,0x96, //antiflicker_step 					   
		0xd7  ,0x03, //AEC_exp_time_min 			   
		0xd8  ,0x02, 
			   
		0xdd  ,0x22, 
	  															
	//========= measure window										
		0xe0  ,0x03, 						 
		0xe1  ,0x02, 							 
		0xe2  ,0x27, 								 
		0xe3  ,0x1e, 				 
		0xe8  ,0x3b, 					 
		0xe9  ,0x6e, 						 
		0xea  ,0x2c, 					 
		0xeb  ,0x50, 					 
		0xec  ,0x73, 		 
	
	//========= close_frame													
		0xed  ,0x00, //close_frame_num1 ,can be use to reduce FPS				 
		0xee  ,0x00, //close_frame_num2  
		0xef  ,0x00, //close_frame_num
	
	// page1
		0xf0  ,0x01, //select page1 
	
		0x00  ,0x20, 							  
		0x01  ,0x20, 							  
		0x02  ,0x20, 									
		0x03  ,0x20, 							
		0x04  ,0x78, 
		0x05  ,0x78, 					 
		0x06  ,0x78, 								  
		0x07  ,0x78, 									 
	
	
	
		0x10  ,0x04, 						  
		0x11  ,0x04,							  
		0x12  ,0x04, 						  
		0x13  ,0x04, 							  
		0x14  ,0x01, 							  
		0x15  ,0x01, 							  
		0x16  ,0x01, 						 
		0x17  ,0x01, 						 
		  
													 
		0x20  ,0x00, 					  
		0x21  ,0x00, 					  
		0x22  ,0x00, 						  
		0x23  ,0x00, 						  
		0x24  ,0x00, 					  
		0x25  ,0x00, 						  
		0x26  ,0x00, 					  
		0x27  ,0x00,  						  
	
		0x40  ,0x11, 
	
	//=============================lscP 
		0x45  ,0x06, 	 
		0x46  ,0x06, 			 
		0x47  ,0x05, 
	
		0x48  ,0x04, 	
		0x49  ,0x03, 		 
		0x4a  ,0x03, 
	

		0x62  ,0xd8, 
		0x63  ,0x24, 
		0x64  ,0x24, 
		0x65  ,0x24, 
		0x66  ,0xd8, 
		0x67  ,0x24,
	
		0x5a  ,0x00, 
		0x5b  ,0x00, 
		0x5c  ,0x00, 
		0x5d  ,0x00, 
		0x5e  ,0x00, 
		0x5f  ,0x00, 
	
	
	//============================= ccP 
	
		0x69  ,0x03, //cc_mode
		  
	//CC_G
		0x70  ,0x5d, 
		0x71  ,0xed, 
		0x72  ,0xff, 
		0x73  ,0xe5, 
		0x74  ,0x5f, 
		0x75  ,0xe6, 
	
      //CC_B
		0x76  ,0x41, 
		0x77  ,0xef, 
		0x78  ,0xff, 
		0x79  ,0xff, 
		0x7a  ,0x5f, 
		0x7b  ,0xfa, 	 
	
	
	//============================= AGP
	
		0x7e  ,0x00,  
	#if defined(C41_BAR)
		0x7f  ,0x20,  //c41  peter 200906
		0x80  ,0x48,   //c41  peter 200906
	#else
		0x7f  ,0x00,  //default
		0x80  ,0xc8,  
	#endif
		0x81  ,0x06,  
		0x82  ,0x08,  

	#if defined(C42_BAR)
	//  a little gamma , /////////////////
		0x83  ,0x13,  
		0x84  ,0x23,  
		0x85  ,0x35,  
		0x86  ,0x44,  
		0x87  ,0x53,  
		0x88  ,0x60,  
		0x89  ,0x6D,  
		0x8a  ,0x84,  
		0x8b  ,0x98,  
		0x8c  ,0xaa,  
		0x8d  ,0xb8,  
		0x8e  ,0xc6,  
		0x8f  ,0xd1,  
		0x90  ,0xdb,  
		0x91  ,0xea,  
		0x92  ,0xF5,  
		0x93  ,0xFb, 
	/////////////////////////////
	#else
		0x83  ,0x23,  
		0x84  ,0x38,  
		0x85  ,0x4F,  
		0x86  ,0x61,  
		0x87  ,0x72,  
		0x88  ,0x80,  
		0x89  ,0x8D,  
		0x8a  ,0xA2,  
		0x8b  ,0xB2,  
		0x8c  ,0xC0,  
		0x8d  ,0xCA,  
		0x8e  ,0xD3,  
		0x8f  ,0xDB,  
		0x90  ,0xE2,  
		0x91  ,0xED,  
		0x92  ,0xF6,  
		0x93  ,0xFD,  
	#endif
	//about gamma1 is hex r oct
		0x94  ,0x04,  
		0x95  ,0x0E,  
		0x96  ,0x1B,  
		0x97  ,0x28,  
		0x98  ,0x35,  
		0x99  ,0x41,  
		0x9a  ,0x4E,  
		0x9b  ,0x67,  
		0x9c  ,0x7E,  
		0x9d  ,0x94,  
		0x9e  ,0xA7,  
		0x9f  ,0xBA,  
		0xa0  ,0xC8,  
		0xa1  ,0xD4,  
		0xa2  ,0xE7,  
		0xa3  ,0xF4,  
		0xa4  ,0xFA, 
	
	//========= open functions	
		0xf0  ,0x00, //set back to page0																		  
		0x40  ,0x7e, 
		0x41  ,0x2F,
#if 0
	#if defined(SWITCH_CAMERA_SUPPORT)
		#if defined(H167_BAR)
			kal_prompt_trace(MOD_ATCI,"!!!!!!!!GC0307_preview() numberofcamera = %d",numberofcamera);
			if(numberofcamera==SECONDARY_CAMERA)
				gc0307_image_mirror = IMAGE_H_MIRROR;
			else
				gc0307_image_mirror = IMAGE_NORMAL;
		#elif defined(C42_BAR)
			if(numberofcamera==SECONDARY_CAMERA)//front
			{
				gc0307_image_mirror = IMAGE_V_MIRROR;
				0x15, 0xba,
			}
			else //front
			{
				gc0307_image_mirror = IMAGE_HV_MIRROR;
				0x15, 0xbb,
			}
		#endif
	#else
		#if defined(C28_BAR)
			gc0307_image_mirror = IMAGE_HV_MIRROR;
		#else
			gc0307_image_mirror = IMAGE_NORMAL;
		#endif
	#endif
#endif

//		case IMAGE_NORMAL:
			0x0f, 0xb2,
			0x45, 0x27,
			0x47, 0x2c,
			//0x47, 0xac,			//TEst Patter
#if 0
//		case IMAGE_H_MIRROR:
			0x0f, 0xa2,
			0x45, 0x26,
			0x47, 0x28,						

//		case IMAGE_V_MIRROR:			
			0x0f, 0x92,
			0x45, 0x25,
			0x47, 0x24,			

//		case IMAGE_HV_MIRROR:      // 180
			0x0f, 0x82,
			0x45, 0x24,
			0x47, 0x20,  //bi3,bit2 active

#endif

#if 0
	//=========delay 200ms
	Delayms_GC(200,
#endif	
#if 0
//10 fps
	0x01  ,0x32,	//0xfa
	0x02  ,0x70,	//0x70
	0x10  ,0x01,	//0x01
	0xd6  ,0x3c,// 0x42	//0x3c	//0x32
	0x28  ,0x01,// 0x01	//0x01	//0x01
	0x29  ,0xa4,// 0xce	//0xa4	//0x5e
	0x2a  ,0x01,// 0x01	//0x01	//0x01
	0x2b  ,0xa4, //0xce	//0xa4	//0x5e
	0x2c  ,0x01, //0x01	//0x01	//0x01
	0x2d  ,0xa4, //0xce	//0xa4	//0x5e
	0x2e  ,0x04, //0x05	//0x04	//0x03
	0x2f  ,0xb0, //0x28	//0xb0	//0xe8
#else
// 8fps
	0x01  ,0xfa,
	0x02  ,0x70,
	0x10  ,0x01,
	0xd6  ,0x32,
	0x28  ,0x01,
	0x29  ,0x5e,
	0x2a  ,0x01,
	0x2b  ,0x5e,
	0x2c  ,0x01,
	0x2d  ,0x5e,
	0x2e  ,0x03,
	0x2f  ,0xe8,
#endif

/*	
 gc_0307_MPEG4_encode_mode=KAL_FALSE;  			      //12.5fps   10fps  	8fps 
			gc_0307_write_cmos_sensor(	0x01  ,0x6a);// 0xd7	//0x32	//0xfa
			gc_0307_write_cmos_sensor(	0x02  ,0x25);// 0x28	//0x70	//0x70
			gc_0307_write_cmos_sensor(	0x10  ,0x00);// 0x00	//0x01	//0x01
			gc_0307_write_cmos_sensor(	0xd6  ,0x4b);// 0x42	//0x3c	//0x32
			gc_0307_write_cmos_sensor(	0x28  ,0x02);// 0x01	//0x01	//0x01
			gc_0307_write_cmos_sensor(	0x29  ,0x0d);// 0xce	//0xa4	//0x5e
			gc_0307_write_cmos_sensor(	0x2a  ,0x02);// 0x01	//0x01	//0x01
			gc_0307_write_cmos_sensor(	0x2b  ,0x0d); //0xce	//0xa4	//0x5e
			gc_0307_write_cmos_sensor(	0x2c  ,0x02); //0x01	//0x01	//0x01
			gc_0307_write_cmos_sensor(	0x2d  ,0x0d); //0xce	//0xa4	//0x5e
			gc_0307_write_cmos_sensor(	0x2e  ,0x05); //0x05	//0x04	//0x03
			gc_0307_write_cmos_sensor(	0x2f  ,0xdc); //0x28	//0xb0	//0xe8
*/
	//=========open output
	//0x43  ,0x40,
	//0x44  ,0xE0,


#endif	
	0xFF, 0xFF,
};

__align(2)const static u_char	Sensor_DSC_Preview[] =
{
NULL,
};
__align(2)const static u_char	Sensor_Video_Preview[]=
{
NULL,
};
__align(2)const static u_char	Sensor_Normal_Capture[] = 
{
NULL,
};
__align(2)const static u_char  Sensor_Full_Capture[] = 
{
NULL,
};
__align(2)const static u_char Sensor_Full_Capture_ScaleUp[] =
{
NULL,
};
__align(2)const static u_char  SENSOR_EFFECT_NORMAL[] = 
{	//Normal
NULL,
};
__align(2)const static u_char  SENSOR_EFFECT_1[] = 
{	//BW
NULL,
};
__align(2)const static u_char  SENSOR_EFFECT_2[] = 
{	// Bluish 2
NULL,
};
__align(2)const static u_char  SENSOR_EFFECT_3[] = 
{	// Negative 3
NULL,
};
__align(2)const static u_char  SENSOR_EFFECT_4[] = 
{	// Reddish 4
NULL,
};
__align(2)const static u_char  SENSOR_EFFECT_5[] = 
{	// Antique 5
NULL,
};
__align(2)const static u_char  SENSOR_EFFECT_6[] = 
{	// Greenish 6
NULL,
};
__align(2)const static u_char  SENSOR_EFFECT_7[] = 
{
NULL,
};

__align(2)const static u_char  SENSOR_EV_0[] = 
{	// EV -2
NULL,
};
__align(2)const static u_char  SENSOR_EV_1[] = 
{	// EV -1.5
NULL,
};
__align(2)const static u_char  SENSOR_EV_2[] = 
{	// EV -1
NULL,
};
__align(2)const static u_char  SENSOR_EV_3[] = 
{	// EV -0.5
NULL,
};
__align(2)const static u_char  SENSOR_EV_4[] = 
{	// EV 0
NULL,
};
__align(2)const static u_char  SENSOR_EV_5[] = 
{	// EV +0.5
NULL,
};
__align(2)const static u_char  SENSOR_EV_6[] = 
{	// EV +1
NULL,
};
__align(2)const static u_char  SENSOR_EV_7[] = 
{	// EV +1.5
NULL,
};
__align(2)const static u_char  SENSOR_EV_8[] = 
{	// EV +2
NULL,
};

__align(2)const static u_char  SENSOR_WB_AUTO[] = 
{	// Auto 0
NULL,
};	
__align(2)const static u_char  SENSOR_WB_1[] = 
{	// Sunny 1
NULL,
};	
__align(2)const static u_char  SENSOR_WB_2[] = 
{
NULL,
};	
__align(2)const static u_char  SENSOR_WB_3[] = 
{	// Indoor home 3
NULL,
};	
__align(2)const static u_char  SENSOR_WB_4[] = 
{	// Indoor office 4
NULL,
};	
__align(2)const static u_char  SENSOR_WB_5[] = 
{
NULL
};
__align(2)const static u_char  SENSOR_WB_6[] = 
{
NULL
};

__align(2)const static u_char  SENSOR_NIGHT_MODE_ON[] = 
{ 
NULL,
};
__align(2)const static u_char  SENSOR_NIGHT_MODE_OFF[] = 
{
NULL,
};

__align(2)const static u_char  SENSOR_DEBAND_50[] = 
{
	0x01,0x8c,
	0x02,0x28,
	0xd6,0x58,
	0xff,0xff
};
__align(2)const static u_char  SENSOR_DEBAND_60[] = 
{
	0x01,0x9d,
	0x02,0x1e,
	0xd6,0x48,
	0xff,0xff
};

__align(2)const static u_char  SENSOR_ROTATE_NOMAL[] = 
{
NULL,
};
__align(2)const static u_char  SENSOR_ROTATE_MIRROR[] = 
{
NULL,
};
__align(2)const static u_char  SENSOR_ROTATE_FLIP[] = 
{
NULL,
};
__align(2)const static u_char  SENSOR_ROTATE_MIRROR_FLIP[] = 
{
NULL,
};

static u_short	Sensor_Effect(u_short EffectMode)
{
	u_short	*tab_addr,tab_len;
	u_char  temp_reg;
	u_short ret;
	
	return 0;

	
	temp_reg=gc_0307_read_cmos_sensor(0x47);
	switch(EffectMode)
	{
		case 0://CAM_EFFECT_ENC_NORMAL:		
			if(gc_0307_sensor_night_mode)
				gc_0307_write_cmos_sensor(0x41,0x0f);
			else
				gc_0307_write_cmos_sensor(0x41,0x2f);			
			gc_0307_write_cmos_sensor(0x40,0x7e);
			gc_0307_write_cmos_sensor(0x42,0x10);
//			gc_0307_write_cmos_sensor(0x47,0x2c);
			gc_0307_write_cmos_sensor(0x47,temp_reg & 0xef);
			gc_0307_write_cmos_sensor(0x48,0xc3);
			gc_0307_write_cmos_sensor(0x8a,0x50);//60
			gc_0307_write_cmos_sensor(0x8b,0x50);
			gc_0307_write_cmos_sensor(0x8c,0x07);
			gc_0307_write_cmos_sensor(0x50,0x0c);
			gc_0307_write_cmos_sensor(0x77,0x80);
			#if defined(C41_BAR)
			gc_0307_write_cmos_sensor(0xa1,0x44);//c41 peter
			#else
			gc_0307_write_cmos_sensor(0xa1,0x40);
			#endif
			gc_0307_write_cmos_sensor(0x7a,0x00);
			gc_0307_write_cmos_sensor(0x78,0x00);
			gc_0307_write_cmos_sensor(0x79,0x00);
			gc_0307_write_cmos_sensor(0x7b,0x40);
			gc_0307_write_cmos_sensor(0x7c,0x00);

			gc_effect_on = KAL_FALSE;
			break;		
		case 1://CAM_EFFECT_ENC_GRAYSCALE:
			if(gc_0307_sensor_night_mode)
				gc_0307_write_cmos_sensor(0x41,0x0f);
			else
				gc_0307_write_cmos_sensor(0x41,0x2f);			
			gc_0307_write_cmos_sensor(0x40,0x7e);
			gc_0307_write_cmos_sensor(0x42,0x10);
			gc_0307_write_cmos_sensor(0x47,temp_reg | 0x10);
			gc_0307_write_cmos_sensor(0x48,0xc3);
			gc_0307_write_cmos_sensor(0x8a,0x60);
			gc_0307_write_cmos_sensor(0x8b,0x60);
			gc_0307_write_cmos_sensor(0x8c,0x07);
			gc_0307_write_cmos_sensor(0x50,0x0c);
			gc_0307_write_cmos_sensor(0x77,0x80);
			gc_0307_write_cmos_sensor(0xa1,0x40);
			gc_0307_write_cmos_sensor(0x7a,0x00);
			gc_0307_write_cmos_sensor(0x78,0x00);
			gc_0307_write_cmos_sensor(0x79,0x00);
			gc_0307_write_cmos_sensor(0x7b,0x40);
			gc_0307_write_cmos_sensor(0x7c,0x00);					
			gc_effect_on = KAL_TRUE;	
			break;		
		case 2://CAM_EFFECT_ENC_SEPIA:
			if(gc_0307_sensor_night_mode)
				gc_0307_write_cmos_sensor(0x41,0x0f);
			else
				gc_0307_write_cmos_sensor(0x41,0x2f);		
			gc_0307_write_cmos_sensor(0x40,0x7e);
			gc_0307_write_cmos_sensor(0x42,0x10);
			gc_0307_write_cmos_sensor(0x47,temp_reg | 0x10);
			gc_0307_write_cmos_sensor(0x48,0xc3);
			gc_0307_write_cmos_sensor(0x8a,0x60);
			gc_0307_write_cmos_sensor(0x8b,0x60);
			gc_0307_write_cmos_sensor(0x8c,0x07);
			gc_0307_write_cmos_sensor(0x50,0x0c);
			gc_0307_write_cmos_sensor(0x77,0x80);
			gc_0307_write_cmos_sensor(0xa1,0x40);
			gc_0307_write_cmos_sensor(0x7a,0x00);
			gc_0307_write_cmos_sensor(0x78,0xc0);
			gc_0307_write_cmos_sensor(0x79,0x20);
			gc_0307_write_cmos_sensor(0x7b,0x40);
			gc_0307_write_cmos_sensor(0x7c,0x00);					
			gc_effect_on = KAL_TRUE;
			break;		
		case 3://CAM_EFFECT_ENC_COLORINV:		
			if(gc_0307_sensor_night_mode)
				gc_0307_write_cmos_sensor(0x41,0x4f);
			else
				gc_0307_write_cmos_sensor(0x41,0x6f);
			//gc_0307_write_cmos_sensor(0x41,0x6f);//[6]			
			gc_0307_write_cmos_sensor(0x40,0x7e);
			gc_0307_write_cmos_sensor(0x42,0x10);
			//gc_0307_write_cmos_sensor(0x47,0x2c);
			gc_0307_write_cmos_sensor(0x47,temp_reg & 0xef);
			gc_0307_write_cmos_sensor(0x48,0xc3);
			gc_0307_write_cmos_sensor(0x8a,0x60);
			gc_0307_write_cmos_sensor(0x8b,0x60);
			gc_0307_write_cmos_sensor(0x8c,0x07);
			gc_0307_write_cmos_sensor(0x50,0x0c);
			gc_0307_write_cmos_sensor(0x77,0x80);
			gc_0307_write_cmos_sensor(0xa1,0x40);
			gc_0307_write_cmos_sensor(0x7a,0x00);
			gc_0307_write_cmos_sensor(0x78,0x00);
			gc_0307_write_cmos_sensor(0x79,0x00);
			gc_0307_write_cmos_sensor(0x7b,0x40);
			gc_0307_write_cmos_sensor(0x7c,0x00);
		
			gc_effect_on = KAL_TRUE;	
			break;		
		case 4://CAM_EFFECT_ENC_SEPIAGREEN:
			if(gc_0307_sensor_night_mode)
				gc_0307_write_cmos_sensor(0x41,0x0f);
			else
				gc_0307_write_cmos_sensor(0x41,0x2f);			
			gc_0307_write_cmos_sensor(0x40,0x7e);
			gc_0307_write_cmos_sensor(0x42,0x10);
			gc_0307_write_cmos_sensor(0x47,temp_reg | 0x10);
			gc_0307_write_cmos_sensor(0x48,0xc3);
			gc_0307_write_cmos_sensor(0x8a,0x60);
			gc_0307_write_cmos_sensor(0x8b,0x60);
			gc_0307_write_cmos_sensor(0x8c,0x07);
			gc_0307_write_cmos_sensor(0x50,0x0c);
			gc_0307_write_cmos_sensor(0x77,0x80);
			gc_0307_write_cmos_sensor(0xa1,0x40);
			gc_0307_write_cmos_sensor(0x7a,0x00);
			gc_0307_write_cmos_sensor(0x78,0xc0);
			gc_0307_write_cmos_sensor(0x79,0xc0);
			gc_0307_write_cmos_sensor(0x7b,0x40);
			gc_0307_write_cmos_sensor(0x7c,0x00);				
			gc_effect_on = KAL_TRUE;
			break;					
		case 5://CAM_EFFECT_ENC_SEPIABLUE:
			if(gc_0307_sensor_night_mode)
				gc_0307_write_cmos_sensor(0x41,0x0f);
			else
				gc_0307_write_cmos_sensor(0x41,0x2f);		
			gc_0307_write_cmos_sensor(0x40,0x7e);
			gc_0307_write_cmos_sensor(0x42,0x10);
			gc_0307_write_cmos_sensor(0x47,temp_reg | 0x10);
			gc_0307_write_cmos_sensor(0x48,0xc3);
			gc_0307_write_cmos_sensor(0x8a,0x60);
			gc_0307_write_cmos_sensor(0x8b,0x60);
			gc_0307_write_cmos_sensor(0x8c,0x07);
			gc_0307_write_cmos_sensor(0x50,0x0c);
			gc_0307_write_cmos_sensor(0x77,0x80);
			gc_0307_write_cmos_sensor(0xa1,0x40);
			gc_0307_write_cmos_sensor(0x7a,0x00);
			gc_0307_write_cmos_sensor(0x78,0x70);
			gc_0307_write_cmos_sensor(0x79,0x00);
			gc_0307_write_cmos_sensor(0x7b,0x3f);
			gc_0307_write_cmos_sensor(0x7c,0xf5);					
			gc_effect_on = KAL_TRUE;
			break;								
		case 6://CAM_EFFECT_ENC_GRAYINV:
			if(gc_0307_sensor_night_mode)
				gc_0307_write_cmos_sensor(0x41,0x4f);
			else
			gc_0307_write_cmos_sensor(0x41,0x6f);			
			gc_0307_write_cmos_sensor(0x40,0x7e);
			gc_0307_write_cmos_sensor(0x42,0x10);
			gc_0307_write_cmos_sensor(0x47,temp_reg | 0x10);
			gc_0307_write_cmos_sensor(0x48,0xc3);
			gc_0307_write_cmos_sensor(0x8a,0x60);
			gc_0307_write_cmos_sensor(0x8b,0x60);
			gc_0307_write_cmos_sensor(0x8c,0x07);
			gc_0307_write_cmos_sensor(0x50,0x0c);
			gc_0307_write_cmos_sensor(0x77,0x80);
			gc_0307_write_cmos_sensor(0xa1,0x40);
			gc_0307_write_cmos_sensor(0x7a,0x00);
			gc_0307_write_cmos_sensor(0x78,0x00);
			gc_0307_write_cmos_sensor(0x79,0x00);
			gc_0307_write_cmos_sensor(0x7b,0x40);
			gc_0307_write_cmos_sensor(0x7c,0x00);	
			gc_effect_on = KAL_TRUE;
			break;							
		case 7://CAM_EFFECT_ENC_COPPERCARVING:
			if(gc_reinit)  // is re-enter camera,wait AEC stable
			{
				Delayms(100);
				//Delayms_GC(300);
				gc_reinit=KAL_FALSE;
			}								
			gc_0307_write_cmos_sensor(0x41,0x00);			
			gc_0307_write_cmos_sensor(0x40,0x3e);
			gc_0307_write_cmos_sensor(0x42,0x14);
			gc_0307_write_cmos_sensor(0x47,temp_reg | 0x10);
			gc_0307_write_cmos_sensor(0x48,0xc2);
			gc_0307_write_cmos_sensor(0x8d,0xff);
			gc_0307_write_cmos_sensor(0x8a,0xf0);
			gc_0307_write_cmos_sensor(0x8b,0xf0);
			gc_0307_write_cmos_sensor(0x8c,0x00);
			gc_0307_write_cmos_sensor(0x50,0x08);
			gc_0307_write_cmos_sensor(0xdb,0x50);
			gc_0307_write_cmos_sensor(0xb0,0xff);
			gc_0307_write_cmos_sensor(0x77,0xab);
			gc_0307_write_cmos_sensor(0xa1,0xff);
			gc_0307_write_cmos_sensor(0x7a,0x7f);
			gc_0307_write_cmos_sensor(0x78,0xc0);
			gc_0307_write_cmos_sensor(0x79,0x20);
			gc_0307_write_cmos_sensor(0x7b,0x40);
			gc_0307_write_cmos_sensor(0x7c,0x00);			
			gc_effect_on = KAL_TRUE;
			break;								
		case 8://CAM_EFFECT_ENC_BLUECARVING:
			if(gc_reinit)  // is re-enter camera,wait AEC stable
			{
				Delayms(100);
				//Delayms_GC(300);
				gc_reinit=KAL_FALSE;
			}					
			gc_0307_write_cmos_sensor(0x41,0x00);			
			gc_0307_write_cmos_sensor(0x40,0x3e);
			gc_0307_write_cmos_sensor(0x42,0x14);
			gc_0307_write_cmos_sensor(0x47,temp_reg | 0x10);
			gc_0307_write_cmos_sensor(0x48,0xc2);
			gc_0307_write_cmos_sensor(0x8d,0xff);
			gc_0307_write_cmos_sensor(0x8a,0xf0);
			gc_0307_write_cmos_sensor(0x8b,0xf0);
			gc_0307_write_cmos_sensor(0x8c,0x00);
			gc_0307_write_cmos_sensor(0x50,0x08);
			gc_0307_write_cmos_sensor(0xdb,0x50);
			gc_0307_write_cmos_sensor(0xb0,0xff);
			gc_0307_write_cmos_sensor(0x77,0xab);
			gc_0307_write_cmos_sensor(0xa1,0xff);
			gc_0307_write_cmos_sensor(0x7a,0x7f);
			gc_0307_write_cmos_sensor(0x78,0x70);
			gc_0307_write_cmos_sensor(0x79,0x00);
			gc_0307_write_cmos_sensor(0x7b,0x40);
			gc_0307_write_cmos_sensor(0x7c,0x00);
			gc_effect_on = KAL_TRUE;
			break;								
		case 9://CAM_EFFECT_ENC_CONTRAST:
			if(gc_reinit)  // is re-enter camera,wait AEC stable
			{
				Delayms(100);
				//Delayms_GC(300);
				gc_reinit=KAL_FALSE;
			}								
			gc_0307_write_cmos_sensor(0x41,0x00);			
			gc_0307_write_cmos_sensor(0x40,0x3e);
			gc_0307_write_cmos_sensor(0x42,0x14);
			gc_0307_write_cmos_sensor(0x47,temp_reg | 0x10);
			gc_0307_write_cmos_sensor(0x48,0xc2);
			gc_0307_write_cmos_sensor(0x8d,0xff);
			gc_0307_write_cmos_sensor(0x8a,0xf0);
			gc_0307_write_cmos_sensor(0x8b,0xf0);
			gc_0307_write_cmos_sensor(0x8c,0x00);
			gc_0307_write_cmos_sensor(0x50,0x08);
			gc_0307_write_cmos_sensor(0xdb,0x4a);
			gc_0307_write_cmos_sensor(0xb0,0xff);
			gc_0307_write_cmos_sensor(0x77,0x93);
			gc_0307_write_cmos_sensor(0xa1,0xff);
			gc_0307_write_cmos_sensor(0x7a,0x50);
			gc_0307_write_cmos_sensor(0x78,0x00);
			gc_0307_write_cmos_sensor(0x79,0x00);
			gc_0307_write_cmos_sensor(0x7b,0x40);
			gc_0307_write_cmos_sensor(0x7c,0x00);			
			gc_effect_on = KAL_TRUE;
			break;						
		case 10://CAM_EFFECT_ENC_EMBOSSMENT:
			if(gc_reinit)  // is re-enter camera,wait AEC stable
			{
				Delayms(100);
				//Delayms_GC(300);
				gc_reinit=KAL_FALSE;
			}								
			gc_0307_write_cmos_sensor(0x41,0x00);			
			gc_0307_write_cmos_sensor(0x40,0x3e);
			gc_0307_write_cmos_sensor(0x42,0x14);
			gc_0307_write_cmos_sensor(0x47,temp_reg | 0x10);
			gc_0307_write_cmos_sensor(0x48,0xc2);
			gc_0307_write_cmos_sensor(0x8d,0xff);
			gc_0307_write_cmos_sensor(0x8a,0xf0);
			gc_0307_write_cmos_sensor(0x8b,0xf0);
			gc_0307_write_cmos_sensor(0x8c,0x00);
			gc_0307_write_cmos_sensor(0x50,0x08);
			gc_0307_write_cmos_sensor(0xdb,0x4a);
			gc_0307_write_cmos_sensor(0xb0,0xff);
			gc_0307_write_cmos_sensor(0x77,0x93);
			gc_0307_write_cmos_sensor(0xa1,0xff);
			gc_0307_write_cmos_sensor(0x7a,0x50);
			gc_0307_write_cmos_sensor(0x78,0x00);
			gc_0307_write_cmos_sensor(0x79,0x00);
			gc_0307_write_cmos_sensor(0x7b,0x40);
			gc_0307_write_cmos_sensor(0x7c,0x00);
			
			gc_effect_on = KAL_TRUE;
			break;			
		case 11://CAM_EFFECT_ENC_SKETCH:
			if(gc_reinit)  // is re-enter camera,wait AEC stable
			{
				Delayms(100);
				//Delayms_GC(300);
				gc_reinit=KAL_FALSE;
			}								
			gc_0307_write_cmos_sensor(0x41,0x00);			
			gc_0307_write_cmos_sensor(0x40,0x3e);
			gc_0307_write_cmos_sensor(0x42,0x14);
			gc_0307_write_cmos_sensor(0x47,temp_reg | 0x10);
			gc_0307_write_cmos_sensor(0x48,0xc2);
			gc_0307_write_cmos_sensor(0x8d,0xff);
			gc_0307_write_cmos_sensor(0x8a,0xf0);
			gc_0307_write_cmos_sensor(0x8b,0xf0);
			gc_0307_write_cmos_sensor(0x8c,0x00);
			gc_0307_write_cmos_sensor(0x50,0x08);
			gc_0307_write_cmos_sensor(0xdb,0x4a);
			gc_0307_write_cmos_sensor(0xb0,0xff);
			gc_0307_write_cmos_sensor(0x77,0x93);
			gc_0307_write_cmos_sensor(0xa1,0xff);
			gc_0307_write_cmos_sensor(0x7a,0x50);
			gc_0307_write_cmos_sensor(0x78,0x00);
			gc_0307_write_cmos_sensor(0x79,0x00);
			gc_0307_write_cmos_sensor(0x7b,0x40);
			gc_0307_write_cmos_sensor(0x7c,0x00);
			
			gc_effect_on = KAL_TRUE;
			break;				
		case 12://CAM_EFFECT_ENC_BLACKBOARD:
			if(gc_reinit)  // is re-enter camera,wait AEC stable
			{
				Delayms(100);
				//Delayms_GC(300);
				gc_reinit=KAL_FALSE;
			}								
			gc_0307_write_cmos_sensor(0x41,0x00);			
			gc_0307_write_cmos_sensor(0x40,0x3e);
			gc_0307_write_cmos_sensor(0x42,0x14);
			gc_0307_write_cmos_sensor(0x47,temp_reg | 0x10);
			gc_0307_write_cmos_sensor(0x48,0xc2);
			gc_0307_write_cmos_sensor(0x8d,0xff);
			gc_0307_write_cmos_sensor(0x8a,0xf0);
			gc_0307_write_cmos_sensor(0x8b,0xf0);
			gc_0307_write_cmos_sensor(0x8c,0x00);
			gc_0307_write_cmos_sensor(0x50,0x08);
			gc_0307_write_cmos_sensor(0xdb,0x46);
			gc_0307_write_cmos_sensor(0xb0,0xff);
			gc_0307_write_cmos_sensor(0x77,0xa8);
			gc_0307_write_cmos_sensor(0xa1,0xff);
			gc_0307_write_cmos_sensor(0x7a,0x98);
			gc_0307_write_cmos_sensor(0x78,0x00);
			gc_0307_write_cmos_sensor(0x79,0x00);
			gc_0307_write_cmos_sensor(0x7b,0x40);
			gc_0307_write_cmos_sensor(0x7c,0x00);
			
			gc_effect_on = KAL_TRUE;
			break;	
		case 13://CAM_EFFECT_ENC_JEAN:
		case 14://CAM_EFFECT_ENC_OIL:			
		case 15://CAM_EFFECT_ENC_WHITEBOARD:
			if(gc_reinit)  // is re-enter camera,wait AEC stable
			{
				Delayms(300);
				gc_reinit=KAL_FALSE;
			}								
			gc_0307_write_cmos_sensor(0x41,0x00);			
			gc_0307_write_cmos_sensor(0x40,0x3e);
			gc_0307_write_cmos_sensor(0x42,0x14);
			gc_0307_write_cmos_sensor(0x47,temp_reg | 0x10);
			gc_0307_write_cmos_sensor(0x48,0xc2);
			gc_0307_write_cmos_sensor(0x8d,0xff);
			gc_0307_write_cmos_sensor(0x8a,0xf0);
			gc_0307_write_cmos_sensor(0x8b,0xf0);
			gc_0307_write_cmos_sensor(0x8c,0x00);
			gc_0307_write_cmos_sensor(0x50,0x08);
			gc_0307_write_cmos_sensor(0xdb,0x50);
			gc_0307_write_cmos_sensor(0xb0,0xff);
			gc_0307_write_cmos_sensor(0x77,0xab);
			gc_0307_write_cmos_sensor(0xa1,0xff);
			gc_0307_write_cmos_sensor(0x7a,0x7f);
			gc_0307_write_cmos_sensor(0x78,0x00);
			gc_0307_write_cmos_sensor(0x79,0x00);
			gc_0307_write_cmos_sensor(0x7b,0x40);
			gc_0307_write_cmos_sensor(0x7c,0x00);
			
			gc_effect_on = KAL_TRUE;
			break;			
			
		default:
			ret = KAL_FALSE;
	}
	sys_IF_ait_delay1ms(20);
	return ret;

}

static u_short	Sensor_EV(u_short EvStep)
{
	u_short	*tab_addr,tab_len;
    u_short fRet = 0;
	return 0;

	switch(EvStep)
	{
		case 0://CAM_EV_NEG_4_3:    
            		gc_0307_write_cmos_sensor(0x7a, 0xc0);
			#if defined(C28_BAR)
			gc_0307_write_cmos_sensor(0xd1, 0x28);//only for CSTN for c28
			#else
			gc_0307_write_cmos_sensor(0xd1, 0x30);
			#endif
			break;		
		case 1://CAM_EV_NEG_3_3:
            		gc_0307_write_cmos_sensor(0x7a, 0xd0);
			#if defined(C28_BAR)
			gc_0307_write_cmos_sensor(0xd1, 0x30);//only for CSTN for c28
			#else
			gc_0307_write_cmos_sensor(0xd1, 0x38);
			#endif
			break;		
		case 2://CAM_EV_NEG_2_3:
            		gc_0307_write_cmos_sensor(0x7a, 0xe0);
			#if defined(C28_BAR)
			gc_0307_write_cmos_sensor(0xd1, 0x38);//only for CSTN for c28
			#else
			gc_0307_write_cmos_sensor(0xd1, 0x40);
			#endif
			break;				
		case 3://CAM_EV_NEG_1_3:
            		gc_0307_write_cmos_sensor(0x7a, 0xf0);
			#if defined(C28_BAR)
       		gc_0307_write_cmos_sensor(0xd1, 0x40);//only for CSTN for c28
       	#else
			gc_0307_write_cmos_sensor(0xd1, 0x48);
		#endif
			break;				
		case 4://CAM_EV_ZERO:
            		gc_0307_write_cmos_sensor(0x7a, 0x00);
			#if defined(C28_BAR)
			gc_0307_write_cmos_sensor(0xd1, 0x40);	//only for CSTN for c28
			#else
			gc_0307_write_cmos_sensor(0xd1, 0x50);
			#endif
			break;				
		case 5://CAM_EV_POS_1_3:
            gc_0307_write_cmos_sensor(0x7a, 0x20);
			gc_0307_write_cmos_sensor(0xd1, 0x58);
			break;				
		case 6://CAM_EV_POS_2_3:
            gc_0307_write_cmos_sensor(0x7a, 0x30);
			gc_0307_write_cmos_sensor(0xd1, 0x60);
			break;				
		case 7://CAM_EV_POS_3_3:
            gc_0307_write_cmos_sensor(0x7a, 0x40);
			gc_0307_write_cmos_sensor(0xd1, 0x68);
			break;				
		case 8://CAM_EV_POS_4_3:	
            gc_0307_write_cmos_sensor(0x7a, 0x50);
			gc_0307_write_cmos_sensor(0xd1, 0x70);
			break;
		default:
			return KAL_TRUE;    
	}			

	return KAL_FALSE;
}


static u_short	Sensor_WhiteBalance(u_short WbMode)
{
	u_short	*tab_addr,tab_len;
	u_short fRet = 0;

	uint8  temp_reg;
	

	temp_reg=gc_0307_read_cmos_sensor(0x41);

	if(gc_effect_on)
		return KAL_FALSE;
#if 0
	switch(WbMode)
	{
		case CAM_WB_AUTO:
			gc_0307_write_cmos_sensor(0xc7,0x4c); //for AWB can adjust back
			gc_0307_write_cmos_sensor(0xc8,0x40);
			gc_0307_write_cmos_sensor(0xc9,0x4a);			
			gc_0307_write_cmos_sensor(0x41,temp_reg|0x04);	 // Enable AWB

			break;	
		case CAM_WB_CLOUD:
			gc_0307_write_cmos_sensor(0x41,temp_reg&~0x04);   // Enable AWB 
			gc_0307_write_cmos_sensor(0xc7,0x5a); //WB_manual_gain
			gc_0307_write_cmos_sensor(0xc8,0x42);
			gc_0307_write_cmos_sensor(0xc9,0x40);

			break;		
		case CAM_WB_DAYLIGHT:   // tai yang guang
			gc_0307_write_cmos_sensor(0x41,temp_reg&~0x04);   // Enable AWB 
			gc_0307_write_cmos_sensor(0xc7,0x50);
			gc_0307_write_cmos_sensor(0xc8,0x45);
			gc_0307_write_cmos_sensor(0xc9,0x40);			
			break;		

		case CAM_WB_INCANDESCENCE:   // bai re guang
			gc_0307_write_cmos_sensor(0x41,temp_reg&~0x04);   // Enable AWB 
			gc_0307_write_cmos_sensor(0xc7,0x48);
			gc_0307_write_cmos_sensor(0xc8,0x40);
			gc_0307_write_cmos_sensor(0xc9,0x5c);

			break;		
		case CAM_WB_FLUORESCENT:   //ri guang deng
			gc_0307_write_cmos_sensor(0x41,temp_reg&~0x04);   // Enable AWB 
			gc_0307_write_cmos_sensor(0xc7,0x40);
			gc_0307_write_cmos_sensor(0xc8,0x42);
			gc_0307_write_cmos_sensor(0xc9,0x50);
			break;		

		case CAM_WB_TUNGSTEN:   // wu si deng
			gc_0307_write_cmos_sensor(0x41,temp_reg&~0x04);   // Enable AWB 
			gc_0307_write_cmos_sensor(0xc7,0x40);
			gc_0307_write_cmos_sensor(0xc8,0x54);
			gc_0307_write_cmos_sensor(0xc9,0x70);
			break;
	

		default:
			return KAL_FALSE;			
	}	
#endif	
	sys_IF_ait_delay1ms(20);
	return KAL_FALSE;		
}

static u_short	Sensor_NightMode(mmp_sensor_night_mode NightMode)
{
	u_short	*tab_addr,tab_len;
	u_short temp=0;
	
	return 0;

	A8L_GetSenReg(0x41,&temp);

	switch(NightMode)
	{
		case	A8_SENSOR_NIGHT_MODE_OFF:
				gc_0307_g_bMJPEG_mode = 0;
				A8L_SetSenReg(	0xdd  ,0x22);
				A8L_SetSenReg(	0x41  ,temp | 0x20); 
				A8L_SetSenReg(	0x21  ,0xc0);				
				break;	;																																																			
		default:
				gc_0307_g_bMJPEG_mode = 1;
				//tab_addr = (u_short*)SENSOR_NIGHT_MODE_ON;
				//tab_len = sizeof(SENSOR_NIGHT_MODE_ON);
				A8L_SetSenReg(0xdd  ,0x32);
				A8L_SetSenReg(	0x41  ,temp & ~0x20);
				A8L_SetSenReg(	0xb0  ,0x10);
				A8L_SetSenReg(	0x21  ,0xf0);
				
				break;	
	}				
	sys_IF_ait_delay1ms(100);
   	return A8_NO_ERROR;//	A8L_SendSensorSettingTable(tab_addr,tab_len);	
}

static u_short	Sensor_DeBand(mmp_sensor_deband DeBandMode)
{
	uint8 banding;

#if 0
	switch(DeBandMode)
	{
		case CAM_BANDING_50HZ:
			if(gc_0307_g_bMJPEG_mode)   // MPEG mode , 12M
			{
				gc_0307_write_cmos_sensor(0x01,0x9a);
				gc_0307_write_cmos_sensor(0x02,0x20);
				gc_0307_write_cmos_sensor(0xd6,0x41);			
			}
			else   // 16M
			{
				gc_0307_write_cmos_sensor(0x01,0x8c);
				gc_0307_write_cmos_sensor(0x02,0x28);
				gc_0307_write_cmos_sensor(0xd6,0x58);
			}

			break;                        
		case CAM_BANDING_60HZ:
			if(gc_0307_g_bMJPEG_mode)   // MPEG mode , 12M
			{
				gc_0307_write_cmos_sensor(0x01,0x9D);  
				gc_0307_write_cmos_sensor(0x02,0x1e);  
				gc_0307_write_cmos_sensor(0xd6,0x36);			
			}
			else   // 16M
			{
				gc_0307_write_cmos_sensor(0x01,0x9d);
				gc_0307_write_cmos_sensor(0x02,0x1e);
				gc_0307_write_cmos_sensor(0xd6,0x48);  
			}

			break;
		default:
			return KAL_FALSE;		
	}	
#endif	
	return KAL_FALSE;		
}

static u_short	Sensor_Rotate(mmp_sensor_rotate Rotate)
{
	u_short	*tab_addr,tab_len;
		return 0;

	switch (Rotate)
	{
		case A8_SENSOR_ROTATE_NORMAL:
			gc_0307_write_cmos_sensor(0x0f, 0xb2);
			gc_0307_write_cmos_sensor(0x45, 0x27);
			gc_0307_write_cmos_sensor(0x47, 0x2c);			
		break;
		case A8_SENSOR_ROTATE_MIRROR:
			gc_0307_write_cmos_sensor(0x0f, 0xa2);
			gc_0307_write_cmos_sensor(0x45, 0x26);
			gc_0307_write_cmos_sensor(0x47, 0x28);						
		break;
		case A8_SENSOR_ROTATE_FILP:			
			gc_0307_write_cmos_sensor(0x0f, 0x92);
			gc_0307_write_cmos_sensor(0x45, 0x25);
			gc_0307_write_cmos_sensor(0x47, 0x24);			
		break;
		case A8_SENSOR_ROTATE_MIRROR_FLIP:      // 180
			gc_0307_write_cmos_sensor(0x0f, 0x82);
			gc_0307_write_cmos_sensor(0x45, 0x24);
			gc_0307_write_cmos_sensor(0x47, 0x20);  //bi3,bit2 active
		break;
	}	
   	return	A8L_SendSensorSettingTable(tab_addr,tab_len);		
}

static u_short 	Sensor_FrameRate(u_short FrameRate)
{}

static void	Sensor_Enable(u_short enable)
{
	u_char retVal = 0;
	AIT_Message_P1("GC0307 enable=%d\r\n", enable);
	
	if(enable)
	{
		A800_SetGPIO(AIT_GPIO_SENSOR_POWER_CTL);		    
		A8L_SetSensorPowerPin(1);
		A8L_SetSensorResetPin(1); 
		A8L_SetSensorEnablePin(0);
		sys_IF_ait_delay1ms(10);
		
		A8L_VIF_Init(&preview_mode);
		//		AITS_I2C_Init();
		
		sys_IF_ait_delay1ms(10);
		A8L_SetSensorResetPin(0);

		sys_IF_ait_delay1ms(30); 
		A8L_SetSensorResetPin(1); 
		sys_IF_ait_delay1ms(10);
		//Initial I2C I/F
		A8L_SetSensorIF(A8_SENSOR_SCK_PIN, A8_ON);
		A8L_SetSensorIF(A8_SENSOR_SDA_PIN, A8_ON);
		sys_IF_ait_delay1ms(10);

	    gbSensorInited = 1;    		
	}
	else
	{
		A8L_SetSensorResetPin(0);
		sys_IF_ait_delay1ms(1);

		//Turn off Sensor MCLK
		A8L_SensorClockOutput(A8_OFF);

		A8L_SetSensorEnablePin(1);	
		A8L_SetSensorIF(A8_SENSOR_SCK_PIN, A8_OFF);
		A8L_SetSensorIF(A8_SENSOR_SDA_PIN, A8_OFF);	
		A8L_SetSensorPowerPin(0);		    			        	
		A800_ClearGPIO(AIT_GPIO_SENSOR_POWER_CTL);
#if 0	
		SetA8RegB(0x6050,GetA8RegB(0x6050)|0x07);
		sys_IF_ait_delay1ms(100);	
		//Set to low first
		//If hardware sensor power can not disable, it is better to set SCK and SDA to high or floating
		SetA8RegB(0x6051,GetA8RegB(0x6051)&(~0x07));
		
		//A810H_SetRegB(0x6050,A810H_GetRegB(0x6050)|0x01);
		//A810H_SetRegB(0x6051,A810H_GetRegB(0x6051)&0xFE);
		sys_IF_ait_delay1ms(1);
		SetA8RegB(0x6051,GetA8RegB(0x6051)|0x01);
		sys_IF_ait_delay1ms(1);
	    //sensor PWDN
    	A8L_SetSensorEnablePin(1);
        sys_IF_ait_delay1ms(10);
	    //sensor power LDO
	    A8L_SetSensorPowerPin(0);
	    sys_IF_ait_delay1ms(10);        
#endif	    
		gbSensorInited = 0;	
	}	
	
	return;
}
static u_short	Get_Sensor_ID(u_short*	SensorID)
{
	u_short	fRet,sensor_id=0;
	
	if(!gbSensorInited)
		Sensor_Enable(1);
		
	fRet = A8L_GetSenReg(0x00,&sensor_id);
	*SensorID = sensor_id;
	MSG_FATAL("sensor_id = 0x%x",sensor_id,0,0);

	if(!gbSensorInited)
		Sensor_Enable(0);
		
	return	fRet;
}

static u_short ChangeSensorAE(void)
{
	return 0;
}

static u_short SetSensorResolution(mmp_sensor_mode sensor_mode)
{
	u_short	*tab_addr,tab_len;
    u_short fRet = 0;
	
	switch(sensor_mode)
	{
		case	SENSOR_INIT:
                A8L_VIF_Init(&preview_mode); 
                A8L_SetSensorAttr(&preview_mode);            
				tab_addr = (u_short *)Sensor_Init;
				tab_len = sizeof(Sensor_Init);
				fRet = A8L_SendSensorSettingTable(tab_addr,tab_len);
				sys_IF_ait_delay1ms(20);
				A8L_SetSenReg(0x43  ,0x40);
				A8L_SetSenReg(0x44  ,0xE2);				
                gbSensorInited = 1;
                return fRet;
			break;	
		case	SENSOR_DSC_PREVIEW:
				tab_addr = (u_short *)Sensor_DSC_Preview;
				tab_len = sizeof(Sensor_DSC_Preview);				
			break;
        case    SENSOR_VIDEO_PREVIEW:
				tab_addr = (u_short *)Sensor_Video_Preview;
				tab_len = sizeof(Sensor_Video_Preview);				
			break;            
		case	SENSOR_NORMAL_CAP_MODE:
				tab_addr = (u_short *)Sensor_Normal_Capture;
				tab_len = sizeof(Sensor_Normal_Capture);				
			break;
		case	SENSOR_FULL_RESOL_MODE:
                A8L_VIF_Init(&full_mode); 
                A8L_SetSensorAttr(&full_mode);            
				tab_addr = (u_short *)Sensor_Full_Capture;
				tab_len = sizeof(Sensor_Full_Capture);				
			break;
		case	SENSOR_SCALE_UP_MODE:
                fRet = ChangeSensorAE();
				tab_addr = (u_short *)Sensor_Full_Capture_ScaleUp;
				tab_len = sizeof(Sensor_Full_Capture_ScaleUp);				
            break;			
	}
    if(!fRet)
    	return	A8L_SendSensorSettingTable(tab_addr,tab_len);
    else
        return  fRet;
}

static t_sensor_mode_attr	preview_mode =
{
	SENSOR_PREVIEW_VH_POLARITY,
	SENSOR_PREVIEW_LATCH_COUNT,
	SENSOR_PREVIEW_YUV_FORMAT,
	SENSOR_LATCH_EDGE,
	1,
	1,
	SENSOR_PREVIEW_WIDTH,
	SENSOR_PREVIEW_HEIGHT,
	0,
	SENSOR_PREVIEW_LATCH_COUNT
};

static t_sensor_mode_attr	full_mode =
{
	SENSOR_FULL_VH_POLARITY,
	SENSOR_FULL_LATCH_COUNT,
	SENSOR_FULL_YUV_FORMAT,
	SENSOR_LATCH_EDGE,
	1,
	1,
	SENSOR_FULL_WIDTH,
	SENSOR_FULL_HEIGHT,
	0,
	SENSOR_FULL_LATCH_COUNT 
};

t_sensor_manager sensor_gc0307 = 
{
    SENSOR_ID,
	SENSOR_I2C_ID,
	SENSOR_I2C_TYPE,
	&preview_mode,
	&full_mode,
	SetSensorResolution,
	Sensor_Enable,
	Get_Sensor_ID,
	Sensor_Effect,
	Sensor_EV,
	Sensor_WhiteBalance,
	Sensor_NightMode,
	Sensor_DeBand,
	Sensor_Rotate,
	Sensor_FrameRate
};
