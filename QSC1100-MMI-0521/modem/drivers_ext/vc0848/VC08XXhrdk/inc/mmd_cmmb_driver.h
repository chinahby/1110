/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2008                                                     
 * ------------------------------------------------------------------------------
 * [File Name] 		: mmd_cmmb_driver.h
 * [Description] 		: CMMB mode header file
 * [Author] 			: anton liu
 * [Date Of Creation] 	: 2008-09-01
 * [Platform] 			: Platform independent
 * [Note] 				: None
 *
 * Modification History : 
 * ------------------------------------------------------------------------------
 * Date        			Author     			Modifications
 * ------------------------------------------------------------------------------
 *******************************************************************************/
/*******************************************************************************
 * This source code has been made available to you by VIMICRO on an
 * AS-IS basis. Anyone receiving this source code is licensed under VIMICRO
 * copyrights to use it in any way he or she deems fit, including copying it,
 * modifying it, compiling it, and redistributing it either with or without
 * modifications. Any person who transfers this source code or any derivative
 * work must include the VIMICRO copyright notice and this paragraph in
 * the transferred software.
 *******************************************************************************/
 
#ifndef _MMD_CMMB_DRIVER_H_
#define _MMD_CMMB_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "mmp_rdk.h"
#include "cmmb_timer_control.h"
#define CMMB_PRINTF MMDPrintf


#define ROTATE_IN_CORE 0
#define FW_DOWNLOAD_CORE	0

#if ROTATE_IN_CORE
#define CMMB_LCD_ROTATE(degree)
#else
#define CMMB_LCD_ROTATE(degree) Lcd_Rotate(degree)
#endif


#define 		CMMB_FOPEN_READ_BINARY 			"r"
#define 		CMMB_FOPEN_WRITE_BINARY 		"w"
#define 		CMMB_BUF_COUNT					3

#define LANG_SIZE	3
#define SCHEDULE_TITLE_SIZE 110
#define SCHEDULE_DATE_SIZE 12	// First 10 bytes available
#define SCHEDULE_TIME_SIZE 10	// First 8 bytes available
#define SERVICE_NAME_SIZE 41
#define EB_PUB_SIZE		128	
#define EB_TEXT_SIZE	130

extern MMD_S08 CMMB_FW_START[];
extern MMD_S08 CMMB_FW_END[];
extern MMD_U32 cmmb_mfs_buf_count;

enum {
	MMD_CMMB_NO_ERROR 				= 0, 	/* operation successful*/
	MMD_CMMB_ERR_MALLOC				= 1,	/* malloc error*/
	MMD_CMMB_ERR_COREFAIL			= 2,	/* Core operation fail*/
	MMD_CMMB_ERR_SLOT				= 3,	/* CMMB Time Slot err */
	MMD_CMMB_NO_INFO				= 4,	/*CMMB no  information avaliable*/
	MMD_CMMB_ERR_ACKTIMEOUT			= 38 	/* wait command ack timeout */
};

enum {
	CMMB_MODE_INNO_PLAY	= 0, 
	CMMB_MODE_FILE_PLAY
};

enum {
	FW_DOWNLOAD_FLASH	= 0, 
	FW_DOWNLOAD_AP
};

enum {
	CMMB_RECORD_DISABLE	= 0, 
	CMMB_RECORD_ONLY,
	CMMB_RECORD_PLAY
};

//: Definition of ESG service type
enum {
	ESG_TYPE_SERVICE	= 1, 
	ESG_TYPE_SERVICEAUX,
	ESG_TYPE_SCHEDULE,
	ESG_TYPE_CONTENT,
	ESG_TYPE_SERVICEPARA
};


typedef struct{
	MMD_S32 sync;						// sync status.
	MMD_U32 current_frequency;		// current Tuner working frequency
	MMD_S32 signal_strength;				// signal strength(-99dB - 0dB, recommand range[-85, -70])
	MMD_S32 ldpc_err_percent;				// LDPC error percentage(0% - 100%)
} CMMB_SysStatus;

typedef struct{
	MMD_U32 ldpc_total_count;
	MMD_U32 ldpc_error_count;
	MMD_U32 rs_total_count;
	MMD_U32 rs_error_count;
}CMMB_ERR_INFO;


typedef struct{
	MMD_U32 freq_point;
	MMD_U32 ts_start0;
	MMD_U32 ts_count0;
	MMD_U32 demod0;
	MMD_U32 ts_start1;
	MMD_U32 ts_count1;
	MMD_U32 demod1;
}INNO_LabTest_Para;

typedef struct{
	INNO_LabTest_Para lab_test_para;
	MMD_S32 strength;
	MMD_S32 scope;
	MMD_S32 test_times;
	MMD_S32 pn_dir;
}INNO_ManufactureTest_Para;


typedef struct _CMMB_FILE_  CMMB_FILE;
struct _CMMB_FILE_
{
	void* file_handle;
	char file_name[100];
	unsigned int file_len;
	unsigned int file_end;
	char* tmp_buf;
} ;


typedef struct 
{
	MMD_U32 chnl_info;
	MMD_U32 wServiceId;
}tFrameDesc;

typedef struct
{
	MMD_U32		nMfNum;	//复用帧数量
	tFrameDesc	tFrameDesc[40];

} tCMCT;

typedef struct
{
	MMD_U32 core_cmmb_buffer[CMMB_BUF_COUNT];
	MMD_U32 core_cmmb_buflen;
	MMD_U32 channel_no;
	tCMCT ch_info;
} CMMB_INFO;

typedef struct
{
	MMD_U32 size_x;
	MMD_U32 size_y;
	MMD_U32 full_screen;
} CMMB_LCD_INFO;

//: For ESG applications
//: Service Element structure
typedef struct 
{
	MMD_U16 id;					//	Service ID
	MMD_U08 service_class;		//	Service Class, 1-TV, 2-Audio BroadCasting
	MMD_U08 lang[LANG_SIZE];				//	"zho"	for Chinese
	MMD_U08 name[SERVICE_NAME_SIZE];				//	Service name string, e.g CCTV-1
	MMD_U08 for_free;				//	For Free flag, 1 for free, 0 for charge
} CMMB_ServiceElement;

//: Service Schedule structure
typedef struct 
{
	MMD_U16 schedule_id;	//	Only ID for schedule
	MMD_U16 service_id;	//	Service ID as Service Element 
	MMD_U16 content_id;
	MMD_U08 lang[LANG_SIZE];		//	"zho"	for Chinese
	MMD_U08 title[SCHEDULE_TITLE_SIZE];	//	programe title
	MMD_U08 date[SCHEDULE_DATE_SIZE];		// 	UTC Date, xxxx-xx-xx (year-month-day)
	MMD_U08 time[SCHEDULE_TIME_SIZE];		//	UTC Time, xx:xx:xx	(hour:min:sec)
	MMD_U08 for_free;		//	For Free flag, 1 for free, 0 for charge(免费/付费)
	MMD_U08 live;			//	1 for live, 0 for record(直播/录播)
	MMD_U08 repeat;		//	1 for repeat, 0 for new(重播/首播)
} CMMB_ServiceSchedule;

typedef struct 
{
	MMD_U08 start_date[SCHEDULE_DATE_SIZE];
	MMD_U08 start_time[SCHEDULE_TIME_SIZE];
	MMD_U08 duration[SCHEDULE_TIME_SIZE];
	MMD_U08 lang[LANG_SIZE+1];				//	"zho"	for Chinese
	MMD_U08 theme[SCHEDULE_TITLE_SIZE+2];				//	Service name string, e.g CCTV-1
} CMMB_ProgramGuide_Element;

typedef struct 
{
	MMD_U32 available;
	CMMB_ProgramGuide_Element programe_guide[2];
} CMMB_ProgramGuide;

typedef struct 
{
	MMD_U08 trigger;
	MMD_U08 type;
	MMD_U08 level;
	MMD_U08 charset;	//	0 for GB2312
	MMD_U08 start_date[SCHEDULE_DATE_SIZE];	
	MMD_U08 start_time[SCHEDULE_TIME_SIZE];		
	MMD_U08 duration[SCHEDULE_TIME_SIZE];
	MMD_U08 lang[LANG_SIZE+1];
	MMD_U08 publisher[EB_PUB_SIZE];
	MMD_U08 text[EB_TEXT_SIZE];
	MMD_U08	triggerMsgLevel;	// 3 bits
	MMD_U08 nBandWidth;		    // Bandwidth, 4 bits
	MMD_U08	nFreq;			// Target freq in which the message is broadcasting, 8 bits
	MMD_U08	netLevel;			// Network Level
	MMD_U16	netID;				// Network ID, 16 bits
	MMD_U32	dwCenterFreq;		// frequency, which is no use, 32 bits		
} CMMB_Simple_EB_Element;

typedef struct 
{
	MMD_U32 available;
	CMMB_Simple_EB_Element simple_eb_element;
} CMMB_Simple_EB;


typedef struct _ESG_BUF_INFO
{
	MMD_U32	BUF_SIZE;
	MMD_U32	BUF_NUM;
}ESG_BUF_INFO, *pESG_BUF_INFO;

typedef struct _ESG_BUF_ADDR
{
	MMD_U32	BUF_ADDR[4];	
}ESG_BUF_ADDR, *pESG_BUF_ADDR;




typedef struct
{
	MMD_U32	buffer_address;		/* buffer pointer */
   	MMD_U32 buffer_size;		/* buffer size*/
	MMD_U32	data_length;		/* bytes in buffer */
    MMD_U32	cursor_read;		/* read cursor in buffer */
	MMD_U32	cursor_write;		/* write cursor in buffer */
	MMD_U32	element_num;		/* Number of ESG element  in buffer*/
	MMD_U32	is_over;			/* flag to identify buffer data finsh, 0: is not last block data, 1: is last data block */
} CMMB_ESG_Buf;


typedef struct
{
	CMMB_ESG_Buf esg_buffer[4];	
   	MMD_U32 buffer_count;
	MMD_U32 buffer_read;
	MMD_U32 buffer_write;
} CMMB_ESG_Buf_Info;

///:~

//:	MFS buffer structure
typedef struct
{
	MMD_U32	buffer_address;		/* buffer pointer */
   	MMD_U32 	buffer_size;		/* buffer size*/
	MMD_U32	data_length;		/* bytes in buffer */
	MMD_U32	cursor_read;		/* read cursor in buffer */
	MMD_U32	cursor_write;		/* write cursor in buffer */
	MMD_U32	is_over;			/* flag to identify buffer data finsh, 0: is not last block data, 1: is last data block */
} CMMB_MFS_Buf;

typedef struct
{
	CMMB_MFS_Buf mfs_buffer[CMMB_BUF_COUNT];	
   	MMD_U32 buffer_count;
	MMD_U32 buffer_read;
	MMD_U32 buffer_write;
} CMMB_MFS_Buf_Info;

///:~

MMD_U32 MMD_Mode_CMMB_Lisr(MMD_U32 cmd);
MMD_U32 MMD_Mode_CMMB_Hisr(MMD_U32 cmd);
MMD_U32 MMD_CMMB_SearchChannel(CMMB_INFO*  pcmmb_info);
MMD_U32 MMD_CMMB_CLEAN_CACHE(MMD_U32 addr, MMD_U32 size);

MMD_U32 MMD_CMMB_INIT(CMMB_INFO*  pcmmb_info, MMD_U32 play_mode, MMD_U32 fw_option, MMD_U08* pFW, MMD_U32 fw_len);
MMD_U32 MMD_CMMB_LCD_INIT(CMMB_LCD_INFO* lcd_info);

MMD_U32 MMD_CMMB_PLAY(MMD_U32 ch_info, MMD_U32 record_option);
MMD_U32 MMD_CMMB_STOP(void);
MMD_U32 MMD_CMMB_QUIT(void);
MMD_U32 MMD_CMMB_LOADBLOCK(unsigned int index, unsigned int data_len, unsigned int cur_read);
MMD_U32 MMD_CMMB_GetService(MMD_U32 service_ch_info, CMMB_ESG_Buf_Info* esg_buf_info);
MMD_U32 MMD_CMMB_GetSchedule(MMD_U32 service_ch_info, CMMB_ESG_Buf_Info* esg_buf_info);

MMD_U32 MMD_CMMB_ESG_BUF_INIT(ESG_BUF_INFO* pInfo, ESG_BUF_ADDR* pAddr);
MMD_U32 MMD_CMMB_ESG_BUF_RELEASE(void);

MMD_U32 MMD_CMMB_GET_SYS_STATUS(CMMB_SysStatus* cmmb_status);
MMD_U32 MMD_CMMB_SET_VOLUME(MMD_U32 volume);
MMD_U32 MMD_CMMB_GPIO_SELECT(MMD_U32 reset_pin, MMD_U32 interrupt_pin);

MMD_U32 MMD_CMMB_SET_BUF_FULL(MMD_U32 index);
MMD_U32 MMD_CMMB_SET_BUF_Empty(MMD_U32 index);
MMD_U32 MMD_CMMB_CHECK_BUF_FULL(MMD_U32 index);
MMD_U32 MMD_CMMB_ESG_GET_BUF(MMD_U32 buf_addr, CMMB_ESG_Buf* cmmb_esg_buf, MMD_S08* esg_data);
MMD_U32 MMD_CMMB_ESG_DEMUX_OVER(void);
MMD_U32 MMD_CMMB_FIND_ESG_SLOT(CMMB_INFO* cmmb_info, MMD_U32* esg_channel_info);
MMD_U32 MMD_ESG_STOP(void);
MMD_U32 MMD_CMMB_GET_PROGRAME_GUIDE(CMMB_ProgramGuide* programe_guide);
MMD_U32 MMD_CMMB_GET_EB(CMMB_Simple_EB* eb);

MMD_U32 MMD_CMMB_RECORD_START(MMD_U32 record_mode);
MMD_U32 MMD_CMMB_RECORD_STOP(void);
MMD_U32 MMD_CMMB_RECORD_PLY_START(void);
MMD_U32 MMD_CMMB_RECORD_PLY_STOP(void);

MMD_U32 MMD_CMMB_LAB_TEST_START(INNO_LabTest_Para* lab_test_para);
MMD_U32 MMD_CMMB_GET_LAB_TEST_STATUS(CMMB_SysStatus* sys_status, CMMB_ERR_INFO* err_info);
MMD_U32 MMD_CMMB_LAB_TEST_STOP(void);
MMD_U32 MMD_CMMB_MANUFACTURE_TEST_START(INNO_ManufactureTest_Para* m_test_para);
MMD_U32 MMD_CMMB_MANUFACTURE_TEST_STOP(void);
MMD_U32 MMD_CMMB_GET_MANUFACTURE_TEST_STATUS(MMD_U32* pass);

#ifdef __cplusplus
}
#endif

#endif /*_MMD_FILE_DRIVER_H_*/
