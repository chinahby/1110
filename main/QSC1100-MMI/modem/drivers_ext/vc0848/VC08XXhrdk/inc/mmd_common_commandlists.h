/* All rights reserved, Copyright (C) vimicro LIMITED 2003-2005                                             */
/*------------------------------------------------------------------------------------------------------------
 Command lists
------------------------------------------------------------------------------------------------------------*/

#ifndef _MMD_COMMON_COMMAND_LISTS_H_
#define _MMD_COMMON_COMMAND_LISTS_H_

/******************************************************************************
  MACRO DEFINITION used in host and device side,   
******************************************************************************/

#define RDK_MODE_NUM_MAX        32 // same as the major cmd number
#define CMD_MASK_MAJOR      (0xff<<8)
#define CMD_MASK_MIN            (0xff)

//-----------------------------------------------
//(1) Host to device command defination

#define MAJ_SYS     0x0000
#define MAJ_PIC     0x0100
#define MAJ_PLY     0x0200
#define MAJ_VID     0x0300
#define MAJ_IMG     0x0400
#define MAJ_FIL     0x0500
#define MAJ_USB     0x0600
#define MAJ_PWR     0x0700
#define MAJ_AREC    0x0800 /* Audio recode */
#define MAJ_MREC    0x0900 /* Movie(MPEG4+AMR) recode */
#define MAJ_LCD         0x0A00
#define MAJ_REC  	0x0B00
#define MAJ_CMMB  	0x0C00		//	#define MAJ_UNDEF3  0x0C00
#define MAJ_RV  		0x0D00
#define MAJ_UNDEF5  0x0E00
#define MAJ_UNDEF6  0x0F00
#define MAJ_UNDEF7  0x1000
#define MAJ_UNDEF8  0x1100
#define MAJ_UNDEF9  0x1200
#define MAJ_UNDEF10 0x1300
#define MAJ_UNDEF11 0x1400
#define MAJ_UNDEF12 0x1500
#define MAJ_UNDEF13 0x1600
#define MAJ_UNDEF14 0x1700
#define MAJ_UNDEF15 0x1800
#define MAJ_UNDEF16 0x1900
#define MAJ_UNDEF17 0x1A00
#define MAJ_UNDEF18 0x1B00
#define MAJ_UNDEF19 0x1C00
#define MAJ_UNDEF20 0x1D00
#define MAJ_UNDEF21 0x1E00
#define MAJ_DBG         0x1F00

#define MIN_RESERVED			0xFF

#define	CHACK_CORE		0xEE

//Minor commands(MAJ_SYS)
#define MIN_SYS_INITIALIZE  0x00
#define MIN_SYS_SDSETTING   0x01
#define MIN_SYS_RESTART     0x02
#define MIN_SYS_TERMINATE   0x03
#define MIN_SYS_GETINFO     0x04
#define MIN_SYS_LV1CMDERR   0x05
#define MIN_SYS_LV2CMDERR   0x06
#define MIN_SYS_INTERNALERR 0x07
#define MIN_SYS_CHANGEMODE  0x08
#define MIN_SYS_GETCRRMODE  0x09
#define MIN_SYS_GETCOREVERSION  0x0A
#define MIN_SYS_QUIT    0x0B
#define MIN_SYS_SEND_SENSOR_INFO					0x0C
#define MIN_SYS_SNR_PRIVIEW_ON					0x0D
#define MIN_SYS_SNR_PRIVIEW_OFF					0x0E
#define MIN_SYS_MALLOC								0x10
#define MIN_SYS_FREE								0x11
#define MIN_SYS_SHOW_MALLOC_INFO					0x12
#define MIN_SYS_GET_MEM_POLL_INFO				0x13
#define MIN_SYS_FILL_MEM							0x14
#define MIN_SYS_SET_MEM							0x15
#define MIN_SYS_GET_CODE_MAP						0x16
#define MIN_SYS_CREATE_MEM_POOL					0x17
#define MIN_SYS_FUNC_REGISTER						0x18
#define MIN_SYS_MEM_SET							0x19
#define MIN_SYS_FILL_MEM_DATA					    0x20
#define MIN_SYS_ICACHE_CLEAN		    				0x21
#define MIN_SYS_SET_CLK					              0x22    /*add by wanglixin 08-10-09 */
#define MIN_SYS_GET_CLK					              0x23    /*add by wanglixin 08-10-09 */
#define MIN_SYS_PS_MODE					              0x24    /*add by wanglixin 08-10-09 */
#define MIN_SYS_CODEC_ONLY                                          0x25

//Minor commands(MAJ_RV)
#define MIN_RV_FILE_OPEN                			0x01
#define MIN_RV_FILE_PLAY                			0x02
#define MIN_RV_FILE_STOP                			0x03
#define MIN_RV_FILE_PAUSE               			0x04
#define MIN_RV_FILE_RESUME              			0x05
#define MIN_RV_FILE_SEEK               			0x06
#define MIN_RV_FILE_FASTPLAY				0x07
#define MIN_RV_FILE_CLOSE               			0x08

#define MIN_RV_FILE_TOTALTIME				0x09//add by aihouyin
#define MIN_RV_FILE_PROPERTY				0x0a
#define MIN_RV_FILE_PLAYTIME				0x0b//add by aihouyin end

#define MIN_RV_FILE_GETINDEX				0x0c
#define MIN_RV_FILE_GETOFFSET				0x0d//add by aihouyin end
#define MIN_RV_FIFO_INIT         				0x10
#define MIN_RV_FIFO_WRITE         				0x11
#define MIN_RV_FIFO_RELEASE       			0x12

#define MIN_RV_CREATE_MEM_POOL         			0x20
#define MIN_RV_DELETE_MEM_POOL         			0x21

#define MIN_RV_LCD_SET_SIZE         				0x30
#define MIN_RV_LCD_CHANGE_SIZE         			0x31

//Minor commands(MAJ_PLY)
#define MIN_PLY_AUDIO_DECODER_INIT			0x01
#define MIN_PLY_AUDIO_DECODER_RELEASE		0x02
#define MIN_PLY_AUDIO_DECODER_START			0x03
#define MIN_PLY_AUDIO_DECODER_STOP			0x04
#define MIN_PLY_AUDIO_DECODER_SEEK			0x05
#define MIN_PLY_AUDIO_DECODER_SET_OUT_PARA	0x06
#define MIN_PLY_MP4								0x07
#define MIN_PLY_AUDIO_DECODER_PAUSE			0x0a
#define MIN_PLY_AUDIO_DECODER_RESUME		0x0b

#define MIN_PLY_AUDIO_BUF_INIT				0x10
#define MIN_PLY_AUDIO_BUF_RELEASE				0x11
#define MIN_PLY_AUDIO_FILL_SRC_BUF			0x12
#define MIN_PLY_AUDIO_FILL_PCM_BUF			0x13
#define MIN_PLY_AUDIO_START_DMA_OUT			0x14
#define MIN_PLY_AUDIO_DECODE_FRAMES			0x15

#define MIN_PLY_VIDEO_BUF_INIT					0x17
#define MIN_PLY_VIDEO_BUF_RELEASE				0x18
#define MIN_PLY_VIDEO_FILL_SRC_BUF			0x19
#define MIN_PLY_AUDIO_CODEC_INIT				0x1a
#define MIN_PLY_VIDEO_DECODER_START			0x20

//record
#define MIN_PLY_VIDEO_RECORDE_STABLE_BUF_INIT			0x21
#define MIN_PLY_VIDEO_RECORDE_STABLE_BUF_RELEASE        0x22
#define MIN_PLY_VIDEO_RECORDE_STARTTIMER        0x23
#define MIN_REC_VIDEO_BUF_INIT					0x24
#define MIN_REC_VIDEO_BUF_RELEASE				0x25
#define MIN_REC_VIDEO_FRAMEINFO_INIT			0x26
#define MIN_REC_VIDEO_FRAMEINFO_RELEASE			0x27
#define MIN_PLY_AUDIO_PARAM_INIT				0x28

//Minor message(MAJ_PLY)
#define MSG_PLY_AUDIO_DECODE_ERROR			0x81
#define MSG_PLY_AUDIO_STREAM_NODATA			0x82

// 878 define, change later
#define MIN_MEDIA_WORKMODE_INITIALIZE                	0x00
#define MIN_MEDIA_WORKMODE_EXIT               			0x01
#define MIN_MEDIA_PREVIEW_ON          				0x02
#define MIN_MEDIA_PREVIEW_OFF               			0x03
#define MIN_MEDIA_CODEC_PLAY_MODE         			0x04
#define MIN_MEDIA_CODEC_RECORD_MODE               		0x05

#define MIN_MEDIA_PLY_FILE_OPEN                			0x10
#define MIN_MEDIA_PLY_FILE_CLOSE               			0x11
#define MIN_MEDIA_PLY_FILE_PLAY                			0x12
#define MIN_MEDIA_PLY_FILE_STOP                			0x13
#define MIN_MEDIA_PLY_FILE_PAUSE               			0x14
#define MIN_MEDIA_PLY_FILE_RESUME              			0x15
#define MIN_MEDIA_PLY_FILE_SEEK               			0x16
#define MIN_MEDIA_PLY_FILE_FASTPLAY				0x17
#define MIN_MEDIA_PLY_FILE_GETINDEX				0x1b
#define MIN_MEDIA_PLY_FILE_GETOFFSET				0x1c

#define MIN_MEDIA_PLY_FILE_GETTOTALTIME			0x20
#define MIN_MEDIA_PLY_FILE_GETPLAYTIME 			0x21
#define MIN_MEDIA_PLY_FILE_GETPROPERTY			0x22
#define MIN_MEDIA_PLY_FILE_SETPROPERTY			0x23
#define MIN_MEDIA_PLY_AUDIO_POSTPROC_SETPARAM 	0x24
#define MIN_MEDIA_PLY_AUDIO_POSTPROC_ENABLE	 	0x25
#define MIN_MEDIA_PLY_AUDIO_POSTPROC_DISABLE	0x26
#define MIN_MEDIA_PLY_VIDEO_POSTPROC_SETPARAM 	0x27
#define MIN_MEDIA_PLY_VIDEO_POSTPROC_ENABLE	 	0x28
#define MIN_MEDIA_PLY_VIDEO_POSTPROC_DISABLE	0x29


#define MIN_MEDIA_REC_FILE_OPEN                			0x40
#define MIN_MEDIA_REC_FILE_CLOSE               			0x41
#define MIN_MEDIA_REC_FILE_START                			0x42
#define MIN_MEDIA_REC_FILE_STOP                			0x43

#define MIN_MEDIA_REC_FILE_SETTOTALTIME			0x50
#define MIN_MEDIA_REC_FILE_SETPROPERTY			0x51
#define MIN_MEDIA_REC_AUDIO_PREPROC_SETPARAM 	0x52
#define MIN_MEDIA_REC_AUDIO_PREPROC_ENABLE	 	0x53
#define MIN_MEDIA_REC_AUDIO_PREPROC_DISABLE		0x54
#define MIN_MEDIA_REC_VIDEO_PREPROC_SETPARAM 	0x55
#define MIN_MEDIA_REC_VIDEO_PREPROC_ENABLE	 	0x56
#define MIN_MEDIA_REC_VIDEO_PREPROC_DISABLE		0x57

//record
#define MIN_PLY_AUDIO_REC_BUF_INIT				0x60
#define MIN_PLY_AUDIO_ENCODER_INIT				0x61	
#define MIN_AUDIO_REC_STR_INIT					0x62
#define MIN_AUDIO_REC_STR_UPDATA					0x63
#define MIN_MEDIA_REC_AUDIO_INPUT_DMA			0x58
#define MIN_REC_AUDIO_START						0X70
#define MIN_WRITE_AUDIO_BS						0x77
#define MIN_AUDIO_REC_SET_MODE					0x78
#define MIN_AUDIO_REC_SET_SPST					0x79


//Minor commands(MAJ_IMG)
#define MIN_IMG_ALLOCGBUF   	0x00
#define MIN_IMG_COPYGBUF    	0x01

//Minor commands(MAJ_FIL)
#define MIN_FIL_RESERVED      		0x00
#define MIN_FIL_INIT 				0x01
#define MIN_FIL_READSECTORS        	0x02
#define MIN_FIL_STARTTOREAD 		0x03
#define MIN_FIL_WRITESECTORS 		0x04
#define MIN_FIL_STARTTOWRITE 		0x05

#define MIN_CMMB_INIT			0x33
#define MIN_CMMB_SEARCH		0x34
#define MIN_CMMB_PLAY			0x35
#define MIN_CMMB_STOP			0x36
#define MIN_CMMB_QUIT			0x37
#define MIN_CMMB_LOADBLOCK	0x38
#define MIN_CMMB_LCDINIT	0x39
#define MIN_CMMB_GET_SERVICE		0x3a
#define MIN_CMMB_GET_SCHEDULE	0x3b
#define MIN_CMMB_ESG_BUF_INIT	0x3c
#define MIN_CMMB_ESG_BUF_RELEASE	0x3d
#define MIN_CMMB_ESG_SET_EMPTY	0x3e
#define MIN_CMMB_ESG_STOP	0x3f
#define MIN_CMMB_GET_SYS_STATUS	0x40
#define MIN_CMMB_SET_VOLUME	0x41
#define MIN_CMMB_GPIO_SELECT	0x42
#define MIN_CMMB_ESG_SET_FULL	0x43
#define MIN_CMMB_GET_PROGRAMEGUIDE	0x44
#define MIN_CMMB_GET_EB	0x45
#define MIN_CMMB_GET_SYS_ERRINFO	0x46
#define MIN_CMMB_CHECK_EB_FLAG	0x47
#define MIN_CMMB_GET_CA_STATUS	0x48
#define MIN_CMMB_GET_CAS_INFO		0x49
#define MIN_CMMB_TC_INTERACT	0x4a
#define MIN_CMMB_PAUSE			0x4b
#define MIN_CMMB_RESUME			0x4c
#define MIN_CMMB_SET_STATUS_REG			0x4d
#define MIN_CMMB_CHECK_PLAYING			0x4e
#define MIN_CMMB_SET_ERR_THRESHOLD			0x4f
#define MIN_CMMB_CLEAR_CACHE	0x50
#define MIN_CMMB_RECORD_START	0x51
#define MIN_CMMB_RECORD_STOP	0x52
#define MIN_CMMB_RECORD_PAUSE	0x53
#define MIN_CMMB_RECORD_RESUME	0x54
#define MIN_CMMB_RECORD_PLY_START	0x55
#define MIN_CMMB_RECORD_PLY_STOP	0x56
#define MIN_CMMB_RECORD_PLY_PAUSE	0x57
#define MIN_CMMB_RECORD_PLY_RESUME	0x58
#define MIN_CMMB_SET_NOTICE_IMAGE	0x59
#define MIN_CMMB_LAB_TEST_START	0x60
#define MIN_CMMB_LAB_TEST_STOP	0x61
#define MIN_CMMB_MANUFACTURE_TEST_START	0x62
#define MIN_CMMB_MANUFACTURE_TEST_STOP	0x63
#define MIN_CMMB_MANUFACTURE_TEST_STOP	0x63
#define MIN_CMMB_NOTICE_IMAGE_CTL	0x64


//Minor commands(MAJ_DBG),
#define MIN_DBG_TEST            0x00
#define MIN_DBG_ENABLE      0x01
#define MIN_DBG_DISABLE     0x02
#define MIN_DBG_DISPLAYHELP     0x03
#define MIN_DBG_SEND        0x04

//Minor commands(MAJ_USB)
#define MIN_USB_ENABLE	0x00
#define MIN_USB_DISABLE 0x01
#define MIN_USB_PROPERTY	0x02

#define MIN_USB_PLUGIN				0x33
#define MIN_USB_PLUGOUT			0x34

//Minor commands(MAJ_LCD)
#define MIN_LCD_PANELSWITCH	0x00
#define MIN_LCD_SETGWIN 0x01
#define MIN_LCD_UPDATEGWIN 0x02
#define MIN_LCD_UPDATEBG 0x03
#define MIN_LCD_RENDERGBUF 0x04
#define MIN_LCD_RENDERGBLOCK 0x05
#define MIN_LCD_HIDEWIN 0x06
#define MIN_LCD_SETBLEND 0x07
#define MIN_LCD_SETOVERLAY 0x08
#define MIN_LCD_SET_MAIN_LCDIF_PARA 0x09
#define MIN_LCD_SET_SUB_LCDIF_PARA 0x0A

//-----------------------------------------------
//(2) Device to host command defination,
// 64~79, Device to host cmd, D2H
#define MAJ_D2H_PLY      	     0x4000
#define MAJ_D2H_SYS      	     0x4100
#define MAJ_D2H_UNDEF3      0x4200
#define MAJ_D2H_UNDEF4      0x4300
#define MAJ_D2H_UNDEF5      0x4400
#define MAJ_D2H_UNDEF6      0x4500
#define MAJ_D2H_UNDEF7      0x4600
#define MAJ_D2H_UNDEF8      0x4700
#define MAJ_D2H_UNDEF9      0x4800
#define MAJ_D2H_UNDEF10     0x4900
#define MAJ_D2H_UNDEF11 	0x4A00
#define MAJ_D2H_UNDEF12     0x4B00
#define MAJ_D2H_UNDEF13     0x4C00
#define MAJ_D2H_UNDEF14     0x4D00
#define MAJ_D2H_UNDEF15     0x4E00
#define MAJ_D2H_DBG     		0x4F00


//Minor commands(MAJ_D2H_PLY)
// form 0xE0~0xEF is media PLY/REC command  to host
#define MIN_D2H_PLY_FILE_END					0xE0
#define MIN_D2H_PLY_TIME_TOTAL				0xE1
#define MIN_D2H_PLY_TIME_PLAY					0xE2
#define MIN_D2H_REC_FILE_END					0xE3
#define MIN_D2H_COMMADN_UNDEF4				0xE4
#define MIN_D2H_COMMADN_UNDEF5				0xE5
#define MIN_D2H_COMMADN_UNDEF6				0xE6
#define MIN_D2H_COMMADN_UNDEF7				0xE7
#define MIN_D2H_COMMADN_UNDEF8				0xE8
#define MIN_D2H_COMMADN_UNDEF9				0xE9
#define MIN_D2H_COMMADN_UNDEF10				0xEA
#define MIN_D2H_COMMADN_UNDEF11				0xEB
#define MIN_D2H_COMMADN_UNDEF12				0xEC
#define MIN_D2H_COMMADN_UNDEF13				0xED
#define MIN_D2H_COMMADN_UNDEF14				0xEE
#define MIN_D2H_COMMADN_UNDEF15				0xEF
// form 0xF0~0xFF is error command to host
#define MIN_D2H_PLY_FILE_ERROR				0xF0
#define MIN_D2H_REC_FILE_ERROR				0xF1
#define MIN_D2H_AUDIO_READ_ERROR				0xF2
#define MIN_D2H_AUDIO_DECODE_ERROR			0xF3
#define MIN_D2H_VIDEO_READ_ERROR				0xF4
#define MIN_D2H_VIDEO_DECODE_ERROR			0xF5
#define MIN_D2H_AUDIO_WRITE_ERROR			0xF6
#define MIN_D2H_AUDIO_ENCODE_ERROR			0xF7
#define MIN_D2H_VIDEO_WRITE_ERROR			0xF8
#define MIN_D2H_VIDEO_ENCODE_ERROR			0xF9


#define MIN_D2H_SYS_USB_PLUG_IN				0xF0
#define MIN_D2H_SYS_USB_PLUG_OUT				0xF1
#define MIN_D2H_SYS_SD_PLUG_IN				0xF2//add by sunlei
#define MIN_D2H_SYS_SD_PLUG_OUT				0xF3//add by sunlei
#define MIN_D2H_SYS_RESERVED03				0xF4
#define MIN_D2H_SYS_RESERVED04				0xF5
#define MIN_D2H_SYS_RESERVED05				0xF6
#define MIN_D2H_SYS_RESERVED06				0xF7
#define MIN_D2H_SYS_RESERVED07				0xF8
#define MIN_D2H_SYS_RESERVED08				0xF9

//Minor commands(MAJ_SYS)
#define MIN_D2H_DBG_SENDSTRING  0x00


/******************************************************************************
  MACRO DEFINITION only used in host side  
******************************************************************************/
// mode definination related to major command
#define MODE_SYS            (1<<0x00)
#define MODE_PIC            (1<<0x01)
#define MODE_PLY            (1<<0x02)
#define MODE_VID            (1<<0x03)
#define MODE_IMG            (1<<0x04)
#define MODE_FIL            (1<<0x05)
#define MODE_USB            (1<<0x06)
#define MODE_PWR            (1<<0x07)
#define MODE_AREC           (1<<0x08) 
#define MODE_MREC           (1<<0x09) 
#define MODE_LCD            (1<<0x0A)
#define MODE_UNDEF2     (1<<0x0B)
#define MODE_UNDEF3     (1<<0x0C)
#define MODE_UNDEF4     (1<<0x0D)
#define MODE_UNDEF5     (1<<0x0E)
#define MODE_UNDEF6     (1<<0x0F)
#define MODE_UNDEF7     (1<<0x10)
#define MODE_UNDEF8     (1<<0x11)
#define MODE_UNDEF9     (1<<0x12)
#define MODE_UNDEF10        (1<<0x13)
#define MODE_UNDEF11        (1<<0x14)
#define MODE_UNDEF12        (1<<0x15)
#define MODE_UNDEF13        (1<<0x16)
#define MODE_UNDEF14        (1<<0x17)
#define MODE_UNDEF15        (1<<0x18)
#define MODE_UNDEF16        (1<<0x19)
#define MODE_UNDEF17        (1<<0x1A)
#define MODE_UNDEF18        (1<<0x1B)
#define MODE_UNDEF19        (1<<0x1C)
#define MODE_UNDEF20        (1<<0x1D)
#define MODE_BYPASS     (1<<0x1E)
#define MODE_DBG            (0x80000000)    //in order to avoid warning , (1<<0x1F)

//互斥的模式的相应bit置1
#define MODE_SYS_MUTEX      MODE_BYPASS              // 0x00
#define MODE_PIC_MUTEX      MODE_BYPASS     // 0x01
#define MODE_PLY_MUTEX      MODE_BYPASS     // 0x02
#define MODE_VID_MUTEX      MODE_BYPASS     // 0x03
#define MODE_IMG_MUTEX  MODE_BYPASS     // 0x04
#define MODE_FIL_MUTEX      MODE_BYPASS     // 0x05
#define MODE_USB_MUTEX  MODE_BYPASS     // 0x06
#define MODE_PWR_MUTEX  MODE_BYPASS     // 0x07
#define MODE_AREC_MUTEX MODE_BYPASS         // 0x08
#define MODE_MREC_MUTEX MODE_BYPASS     // 0x09
#define MODE_LCD_MUTEX  MODE_BYPASS        // 0x0A
#define MODE_UNDEF2_MUTEX  0        // 0x0B
#define MODE_UNDEF3_MUTEX  0        // 0x0C
#define MODE_UNDEF4_MUTEX  0        // 0x0D
#define MODE_UNDEF5_MUTEX  0        // 0x0E
#define MODE_UNDEF6_MUTEX  0        // 0x0F
#define MODE_UNDEF7_MUTEX  0        // 0x10
#define MODE_UNDEF8_MUTEX  0        // 0x11
#define MODE_UNDEF9_MUTEX  0        // 0x12
#define MODE_UNDEF10_MUTEX  0       // 0x13
#define MODE_UNDEF11_MUTEX  0       // 0x14
#define MODE_UNDEF12_MUTEX  0       // 0x15
#define MODE_UNDEF13_MUTEX  0       // 0x16
#define MODE_UNDEF14_MUTEX  0       // 0x17
#define MODE_UNDEF15_MUTEX  0       // 0x18
#define MODE_UNDEF16_MUTEX  0       // 0x19
#define MODE_UNDEF17_MUTEX  0       // 0x1A
#define MODE_UNDEF18_MUTEX  0       // 0x1B
#define MODE_UNDEF19_MUTEX  0       // 0x1C
#define MODE_UNDEF20_MUTEX  0       // 0x1D
#define MODE_BYPASS_MUTEX  0xBFFFFFFF           // 0x1E
#define MODE_DBG_MUTEX  0       // 0x1F


//error deal 
#define	CMD_FAILED		0X01
#define	PLAY_FAILED	0x02
#define	COLSE_FILE_FAILED	0x03

#endif /* _MMD_COMMON_COMMAND_LISTS_H_ */

