#include "ait_interface.h"
#include "AIT_YUV420VdoPly_IF.h"

#if defined(AIT_3GP_SUPPORT)

static YUV420BufInfo *m_ptrYUV420frameBufInfo = NULL;

unsigned short AIT_YUV420_VdoPlayInit(YUV420BufInfo* ptrYUV420frameBufInfo,unsigned char bFullScreen)
{
	YUV420BufInfo* p = ptrYUV420frameBufInfo;
	m_ptrYUV420frameBufInfo = ptrYUV420frameBufInfo;

	if(bFullScreen)
		AIT701_vdoply_ScreenModeSwitch(1);
	else
		AIT701_vdoply_ScreenModeSwitch(0);

	//Show first frame of vidoe
	AIT701_vdoply_YUVBuf_playback(m_ptrYUV420frameBufInfo, 0,0);

	//Update video player OSD
	if(p->osd_Title.w!=0 &&p->osd_Title.h!=0)
		AIT701_cam_update_osd(p->pOsdBufTitle, p->osd_Title.x,p->osd_Title.y,p->osd_Title.w,p->osd_Title.h);

	if(p->osd_Status.w!=0 &&p->osd_Status.h!=0)
		AIT701_cam_update_osd(p->pOsdBufStatus, p->osd_Status.x,p->osd_Status.y,p->osd_Status.w,p->osd_Status.h);

}



unsigned short AIT_YUV420_VdoPlayScreenModeSwitch(unsigned char bFullScreen)
{

	if(bFullScreen)
		AIT701_vdoply_ScreenModeSwitch(1);
	else
		AIT701_vdoply_ScreenModeSwitch(0);

}


unsigned short AIT_YUV420_VdoPlayHdlr(void)
{

	AIT701_vdoply_YUVBuf_playback(m_ptrYUV420frameBufInfo,0 ,0 ,0);
}


unsigned short AIT_YUV420_VdoPlayUpdateOSD(void)
{
	YUV420BufInfo* p = m_ptrYUV420frameBufInfo;
	
	if(p->osd_Title.w!=0 &&p->osd_Title.h!=0)
		AIT701_cam_update_osd(p->pOsdBufTitle, p->osd_Title.x,p->osd_Title.y,p->osd_Title.w,p->osd_Title.h);

	if(p->osd_Status.w!=0 &&p->osd_Status.h!=0)
		AIT701_cam_update_osd(p->pOsdBufStatus, p->osd_Status.x,p->osd_Status.y,p->osd_Status.w,p->osd_Status.h);


}

unsigned short AIT_YUV420_VdoPlayStop(void)
{

	AIT701_avi_playback_stop();
}


unsigned short AIT_YUV420_VdoPlayTest(void)
{
	int i =0 ;
	int j=0;
	YUV420BufInfo p ;


	p.u16VideoWidth = 176;    //Availagle Video width & Height
	p.u16VideoHeight = 144;
	 
	p.u16YBufWidth = 240;
	p.u16UVBufWidth = 120;

	p.ptrYbuf = (unsigned char*)YUV420RAW;
	p.ptrUbuf = (unsigned char*)(YUV420RAW+p.u16YBufWidth*p.u16VideoHeight);
	p.ptrVbuf = (unsigned char*)(YUV420RAW+p.u16YBufWidth*p.u16VideoHeight+p.u16UVBufWidth*p.u16VideoHeight/2);

	p.pOsdBufTitle = (kal_uint16 *)GetVdoPly_Osd_Buffer();
	p.osd_Title.x = 0; 
	p.osd_Title.y= 18; 
	p.osd_Title.w = 176; 
	p.osd_Title.h = 18;

	p.pOsdBufStatus = (kal_uint16 *)GetVdoPly_Osd_Buffer()+176*(220-58)*2;
	p.osd_Status.x = 0; 
	p.osd_Status.y = 220-58; 
	p.osd_Status.w = 176; 
	p.osd_Status.h = 58;



	AIT_YUV420_VdoPlayInit(&p,0);

	for(j=0;j<2;++j)
	{
		for(i=0;i<100;i++)
			AIT_YUV420_VdoPlayHdlr();

		AIT_YUV420_VdoPlayScreenModeSwitch(1);

		for(i=0;i<100;i++)
			AIT_YUV420_VdoPlayHdlr();
	}

	return 0;


}
#endif /*#define AIT_3GP_SUPPORT*/
