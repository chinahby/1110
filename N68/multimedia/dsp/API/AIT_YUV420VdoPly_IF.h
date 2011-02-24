#ifndef AIT_YUV420VDOPLY_H
#define AIT_YUV420VDOPLY_H
#include "cam_if_ait_api.h"
#include "AIT_Interface.h"
#if defined(AIT_3GP_SUPPORT)

/*
This structure defined in a8_common.h.
typedef struct
{
	u_short x;
	u_short y;
	u_short w;
	u_short h;
} RECT_AIT;

This structure defined in cam_if_ait_api.h.
typedef struct 
{
	unsigned char* ptrYbuf;
	unsigned char* ptrUbuf;
	unsigned char* ptrVbuf;

	unsigned short u16VideoWidth;    //Availagle Video width & Height
	unsigned short u16VideoHeight;
	 
	unsigned short u16YBufWidth;        // Memory size of video = w + span!A 0<=span<=64!A span  16x, x=integer
	unsigned short u16UVBufWidth;

	unsigned short * pOsdBufTitle;	//RBG565 OSD Buffer for title
	unsigned short * pOsdBufStatus;	//RBG565 OSD Buffer for status & progress bar.

	// OSD update range, original arix from PANEL start(x,y)
	RECT_AIT osd_Title;

	RECT_AIT osd_Status;

 
}YUV420BufInfo;

*/




unsigned short AIT_YUV420_VdoPlayInit(YUV420BufInfo* ptrYUV420frameBufInfo,unsigned char bFullScreen);
unsigned short AIT_YUV420_VdoPlayScreenModeSwitch(unsigned char bFullScreen);
unsigned short AIT_YUV420_VdoPlayHdlr(void);
unsigned short AIT_YUV420_VdoPlayUpdateOSD(void);
unsigned short AIT_YUV420_VdoPlayStop(void);
#endif
#endif
