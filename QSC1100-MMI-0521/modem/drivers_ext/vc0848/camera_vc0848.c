/*============================================================================
                         INCLUDE FILES
============================================================================*/
#include "OEMCameraVC0848.h"
#include "VIM_COMMON.h"
#include "VIM_HIGH_API.h"
#include "Dog.h"
#include "Fs.h"

#include "VC08XXhrdk/apps/media/media_player.h"
#include "bio_mb6100.h"
#include "adie_regdefs.h"
#include "err.h"
/*===========================================================================

                      CameraDevice DECLARATIONS

===========================================================================*/
#define CAMERA_JPEG_BUFF_SIZE  (176*220) 
//(1600*1200)
 boolean camera_capture_continued = FALSE;
 camera_frame_type camera_frame;
extern camera_resize_type camera_resize;
 
dword camera_jpeg_buff_aligned[(CAMERA_JPEG_BUFF_SIZE+3)/4];
//dword camera_jpeg_buffer[CAMERA_JPEG_BUFF_SIZE];
 byte *camera_jpeg_buff = (byte *)camera_jpeg_buff_aligned;
 dword frame_count; 
 dword camera_jpeg_size;
dword camera_frame_size[500];

void* JPEG_BUFF_POINTER=(void*)CAMERA_JPEG_BUFF_SIZE;

typedef struct 
{
     uint16 capwidth;
     uint16 capheight;		  
     VIM_HAPI_CPTURE_QUALITY capquality;
} camera_data_vc0848;

camera_data_vc0848  camera_data;


 /*===========================================================================
FUNCTION      CAMERA_START_PREVIEW

DESCRIPTION
              Enters previow state.

              Can be executed in all but INIT state.

              Command is queued and processed at camera_process_func().

DEPENDENCIES
  None

RETURN VALUE
  camera_ret_code_type
  If successful,
   1. CAMERA_RSP_CB_SUCCESS callback if function is executed successfully.
      then,
   2. CAMERA_EVT_CB_TICK callback at tick_interval if display is enabled.
   3. CAMERA_EXIT_CB_DONE if preview is terminated.
   4. CAMERA_EXIT_CB_ERROR if there is a resource error.
   5. CAMERA_EXIT_CB_ABORT.
   6. CAMERA_EXIT_CB_DSP_IDLE

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_start_preview(camera_cb_f_type callback,void *client_data)
{
	int ret;	
  	camera_ret_code_type ret_code = CAMERA_SUCCESS;
  	camera_cb_type       ret_cb;
  	ret=VIM_HAPI_SetWorkMode(VIM_HAPI_MODE_CAMERAON);//Set the work mode
  	if(ret)
		return(ret);
  	ret=VIM_HAPI_SetCaptureParameter(camera_data.capwidth,camera_data.capheight);
	if(ret)
		return(ret);
  	ret=VIM_HAPI_SetCaptureQuality(camera_data.capquality);
	if(ret)
		return(ret); 

    ret = VIM_HAPI_SetPreviewParameter(0, 0, 176, 220);
    if(ret)
        return(ret);

	ret=VIM_HAPI_SetPreviewMode(VIM_HAPI_PREVIEW_ON,VIM_HAPI_MAXPIXEL);
	if(ret)
		return(ret);
  	ret_cb = CAMERA_RSP_CB_SUCCESS;

  	if((ret_code == CAMERA_SUCCESS) && (callback != NULL))
  	{
    		(callback)(ret_cb, client_data, CAMERA_FUNC_START_PREVIEW, NULL);
  	}
  	return ret_code;
}

int camera_preview_test(void)
{
    int ret;
  	ret=VIM_HAPI_SetWorkMode(VIM_HAPI_MODE_CAMERAON);//Set the work mode
  	if(ret)
		return(ret);
    
  	ret=VIM_HAPI_SetCaptureParameter(640,480);
	if(ret)
		return(ret);
    
  	ret=VIM_HAPI_SetCaptureQuality(VIM_HAPI_CPTURE_QUALITY_HIGH);
	if(ret)
		return(ret); 

    ret = VIM_HAPI_SetPreviewParameter(0, 0, 176, 220);
    if(ret)
        return(ret);

	ret = VIM_HAPI_SetPreviewMode(VIM_HAPI_PREVIEW_ON,VIM_HAPI_MAXPIXEL);
    return ret;
}
/*===========================================================================

FUNCTION      CAMERA_TAKE_PICTURE

DESCRIPTION
              The next frame is taken for encoding. Raw data is returned
              in CAMERA_EXIT_CB_DONE camera_frame_type.

              The picture buffer is allocated by the camera service, and must
              be released by the application via either camera_encode_picture
              or camera_release_picture.

DEPENDENCIES
  None

RETURN VALUE
  camera_ret_code_type
  If successful,
   1. CAMERA_RSP_CB_SUCCESS callback if function is executed successfully.
      then,
   2. CAMERA_EXIT_CB_DONE when picture is taken
   3. CAMERA_EXIT_CB_ERROR if there is a resource error.
   4. CAMERA_EXIT_CB_DSP_IDLE when DSP becomes idle
   5. CAMERA_EXIT_CB_ABORT callback if DSP fatal error

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_take_picture( uint16 quality, boolean multi, camera_cb_f_type callback, void * client_data)
{
    int ret;
    camera_ret_code_type ret_code;
    dog_report( DOG_UI_RPT );
  

    ret=(int)VIM_HAPI_CaptureStill(VIM_HAPI_RAM_SAVE,
                       JPEG_BUFF_POINTER,
                       0x50000,
                      NULL);
   if(ret!=0)
  {
      camera_capture_continued = multi;
      ret_code = CAMERA_SUCCESS;
  }
  else
  {
      camera_capture_continued = FALSE;
      ret_code = CAMERA_NO_PICTURE;
  }

    dog_report( DOG_UI_RPT );

  if((ret_code == CAMERA_SUCCESS) && (callback != NULL))
  {
      (callback)(CAMERA_RSP_CB_SUCCESS, client_data, CAMERA_FUNC_TAKE_PICTURE, NULL);

      camera_frame.format = CAMERA_JPEG;
      camera_frame.dx = camera_resize.frame.width;
      camera_frame.dy = camera_resize.frame.height;
      camera_frame.rotation = camera_resize.frame.rotate;
      camera_frame.buffer = camera_jpeg_buff;

      (callback)(CAMERA_EXIT_CB_DONE, client_data, CAMERA_FUNC_TAKE_PICTURE, (dword)&camera_frame);
  }
  else
  {
      VIM_HAPI_CaptureJpeg_Release();
  }

  return ret_code;
}
/*==============================================================================
frame_record 保存每帧图片
===============================================================================*/
void frame_record(char *movname,uint32 startpor)
{
     word       fs_handle;
     fs_rsp_msg_type      fs_rsp; 
     int ret;
     ret=(int)VIM_HAPI_StartCaptureMp4(startpor);
   if(!ret)
   {
      fs_open(movname, FS_OA_READWRITE,NULL, NULL, &fs_rsp);
      fs_handle = fs_rsp.open.handle;
      fs_seek_write(fs_handle,FS_SEEK_EOF,0,camera_jpeg_buff,camera_jpeg_size,NULL,&fs_rsp);
      fs_close(fs_handle, NULL, &fs_rsp);
      camera_frame_size[frame_count] = camera_jpeg_size;
      frame_count++;
      VIM_HAPI_CaptureJpeg_Release();    
   }

   dog_report( DOG_UI_RPT );              
}

/*===========================================================================
frame_record_begin
开始录像
============================================================================*/
void frame_record_begin(char *movname)
{
  fs_handle_type       fs_handle;
  fs_rsp_msg_type      fs_rsp;
  fs_open_xparms_type  fs_open_parms; 
  
  fs_open_parms.create.cleanup_option   = FS_OC_CLOSE;
  fs_open_parms.create.buffering_option = FS_OB_PROHIBIT;
  fs_open_parms.create.attribute_mask   = FS_FA_UNRESTRICTED;

  fs_open(movname,FS_OA_CREATE, &fs_open_parms, NULL, &fs_rsp);
  fs_handle = fs_rsp.open.handle;
  fs_close(fs_handle, NULL, &fs_rsp);
  frame_count = 0;
}
/*===========================================================================

FUNCTION      CAMERA_STOP_PREVIEW

DESCRIPTION
              Exit previow state.

              Can only be executed in PREVIEW state.

              Command is queued and processed at camera_process_func().

DEPENDENCIES
  Must be in preview state.
  Use start_preview callback.

RETURN VALUE
  camera_ret_code_type
  If successful,
   1. CAMERA_RSP_CB_SUCCESS callback if function is executed successfully.
      then,
   2. CAMERA_EXIT_CB_DONE if preview is terminated.

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_stop_preview(void)
{
  camera_ret_code_type ret_code = CAMERA_SUCCESS;
  
  ret_code=VIM_HAPI_SetLCDWorkMode(EDSP_DISPLAY_MODE_NORMAL,0);//modify
  //edsp_AutoTransferOnOff(OFF);

  return ret_code;
}


/*==================================================================

==================================================================*/
int camera_disable_frame_callback( void *dst)
{
    video_stream_infor	*dst_buffer;
    ME_U32  	dst_address, dstbuffer_length;
	    
	//ME_ASSERT_PARAM_RETURN((dst!=NULL));
	
	dst_buffer  = (video_stream_infor	*)dst;
	//get src address, data length and dst address
	dst_address	= dst_buffer->buffer_address + dst_buffer->cursor_write;
	dstbuffer_length = dst_buffer->buffer_size -dst_buffer->cursor_write;
	return VIM_HAPI_CaptureStill(VIM_HAPI_RAM_SAVE,(void*)dst_address,dstbuffer_length,NULL);
	
}

/*==================================================================

==================================================================*/
int camera_release_picture(void)
{
 
   VIM_HAPI_CaptureJpeg_Release(); 
 
   VIM_HAPI_DisplayJpeg_Release();
   return VIM_SUCCEED;
}
/*==================================================================

==================================================================*/
int camera_stop_record(void)
{
    int ret;
    ret=(int)VIM_HAPI_StopCaptureMp4(); 
    return ret;	
}
/*==================================================================

==================================================================*/
int camera_enable_frame_callback(void)
{
    VIM_HAPI_DisplayJpeg_Release();	
    return VIM_SUCCEED;	
}

#define SAMPLE_CODE // define this feature for camera_preview test.

#ifdef SAMPLE_CODE
//#define VC0848_FILE_SYS_TEST
#endif

#define TST_LCD_WIDTH 176
#define TST_LCD_HEIGHT 220

#ifndef Uart_Printf
#define  Uart_Printf MMDPrintf
#endif

#ifndef MMD_FILE_SURPPORT_UNICODE
#define MMD_FILE_MAXPATH 64
#define MMD_FILE_MAXNAME 64
#else
#define MMD_FILE_MAXPATH (512+64) //1024   //short name 25   
#define MMD_FILE_MAXNAME (512+64) //1024
#endif

//arm7 arm9 mod
#define MMD_FILE_ATTR_ARC      0x20  //archive
#define MMD_FILE_ATTR_DIR      0x10   //directory
#define MMD_FILE_ATTR_VOLUME   0x08 //VOLUME
#define MMD_FILE_ATTR_SYSTEM   0x04 //system
#define MMD_FILE_ATTR_HIDDEN   0x02  //hidden
#define MMD_FILE_ATTR_READONLY 0x01  //read only
#define MMD_FILE_ATTR_NORMAL 0x00  //normal

#define MMD_FILE_SURPPORT_UNICODE


typedef struct {
 unsigned long cluster;
 unsigned long sector;
 unsigned long sectorend;
 unsigned long pos;
} MMD_FILE_POS;



typedef struct {
    MMD_S08 filename[MMD_FILE_MAXNAME]; /*file name+ext*/
#ifdef MMD_FILE_SURPPORT_UNICODE
    MMD_S08 shortname[15];  /*file name+ext*/
    MMD_S08 attr;   // 1 is dic  2 is file
#endif
    MMD_U32 filesize;   
    MMD_S08 openmode[3];    
    MMD_S08 lnameflag;  // 0 is short name  1 is the long name unicode
    MMD_U32 namelen;    
    MMD_S08 path [MMD_FILE_MAXPATH];    /* long file name  */
    MMD_S08 lname[MMD_FILE_MAXPATH];
    MMD_U16 time;
    MMD_U16 date;
    MMD_U32 cluster;//add by sunlei 2007-09-18,for liurenxue
    MMD_FILE_POS pos;
    MMD_U08 media;
    MMD_U08 reserved[3];
} MMD_FILE_FIND_INFO; 

//jpeg decoder




#ifdef SAMPLE_CODE
 
unsigned char tstBuf[176*220*2];
int iPlayEndFlag=0;

void tstReadReg(unsigned int regaddr,unsigned int size)
{
	int i;
	unsigned int regvalue=0;
	for(i=0;i<size;i++)
	{
		regvalue=MMD_SYS_Reg_Get(regaddr+i*4);
		MSG_FATAL("Addr=0x%08X,Data=0x%08X", (regaddr+i*4), regvalue,0);
	}

	return;
}
void tstRefreshLCD()
{
 blayer_params b_property;
 

 b_property.layer=MMD_LCD_B0LAYER;
 b_property.fmt=MMD_LCD_BLAYER_RGB565;
 b_property.src_w=TST_LCD_WIDTH;
 b_property.src_h=TST_LCD_HEIGHT;
 b_property.fo_x=0;
 b_property.fo_y=0; 
 b_property.so_x=0;
 b_property.so_y=0;
 b_property.so_w=TST_LCD_WIDTH;
 b_property.so_h=TST_LCD_HEIGHT;
 MMD_LCD_Init();
//MD_LCD_SetGwin( &b_property);
 

 memset(tstBuf, 0xFF, sizeof(tstBuf));
//MD_LCD_RenderGbuf(MMD_LCD_B0LAYER,0,0,TST_LCD_WIDTH,TST_LCD_HEIGHT,(MMD_U08*)tstBuf);
 //D_LCD_UpdateGwin(MMD_LCD_B0LAYER, 0,0,TST_LCD_WIDTH,TST_LCD_HEIGHT);
 MMD_LCD_Refresh(MMD_LCD_B0LAYER,0,0,TST_LCD_WIDTH,TST_LCD_HEIGHT,(MMD_U08*)tstBuf,0);
 MMD_USER_WaitMSEC(500);
 
 memset(tstBuf, 0, sizeof(tstBuf));
 //D_LCD_RenderGbuf(MMD_LCD_B0LAYER,0,0,TST_LCD_WIDTH,TST_LCD_HEIGHT,(MMD_U08*)tstBuf);
 //D_LCD_UpdateGwin(MMD_LCD_B0LAYER, 0,0,TST_LCD_WIDTH,TST_LCD_HEIGHT);
 MMD_LCD_Refresh(MMD_LCD_B0LAYER,0,0,TST_LCD_WIDTH,TST_LCD_HEIGHT,(MMD_U08*)tstBuf,0);
 MMD_USER_WaitMSEC(500);
 
 memset(tstBuf, 0x0f, sizeof(tstBuf));
//MD_LCD_RenderGbuf(MMD_LCD_B0LAYER,0,0,TST_LCD_WIDTH,TST_LCD_HEIGHT,(MMD_U08*)tstBuf);
//MD_LCD_UpdateGwin(MMD_LCD_B0LAYER, 0,0,TST_LCD_WIDTH,TST_LCD_HEIGHT);
MMD_LCD_Refresh(MMD_LCD_B0LAYER,0,0,TST_LCD_WIDTH,TST_LCD_HEIGHT,(MMD_U08*)tstBuf,0);
}

#if 0 // debug for vc0848 lcd update control
void lcd_vc0848_update(const void *buf_ptr,
                 signed short src_width,
                 signed short src_starting_row,
                 signed short src_starting_column,
                 signed short num_of_rows,
                 signed short num_of_columns,
                 signed short dst_starting_row,
                 signed short dst_starting_column)
{
    static boolean bFirstRun = TRUE;
    static blayer_params b_property;
    
    MMD_LCD_BLY_Release();
    if(bFirstRun == TRUE)
    {
       b_property.layer=MMD_LCD_B0LAYER;
       b_property.fmt=MMD_LCD_BLAYER_RGB565;
       b_property.src_w=TST_LCD_WIDTH;
       b_property.src_h=TST_LCD_HEIGHT;
       b_property.fo_x=src_starting_row;
       b_property.fo_y=src_starting_column; 
       b_property.so_x=dst_starting_row;
       b_property.so_y=dst_starting_column;
       b_property.so_w=num_of_rows;
       b_property.so_h=num_of_columns;
       MMD_LCD_Init();
       bFirstRun = FALSE;
    }
    MMD_LCD_SetGwin(&b_property);
    MMD_LCD_RenderGbuf(MMD_LCD_B0LAYER,dst_starting_row,dst_starting_column,num_of_rows,num_of_columns,(MMD_U08*)buf_ptr);
    MMD_LCD_UpdateGwin(MMD_LCD_B0LAYER,dst_starting_row,dst_starting_column,num_of_rows,num_of_columns);
}
#endif // if 0

void tstFileSystem()
{
#ifndef VC0848_FILE_SYS_TEST    
 MMD_FILE_FIND_INFO gFindFile;
 F_FIND FileFind;
 int result=0;
 int filenum=0;
 char BPath[30]={'*',0,'.',0,'*',0,0,0};
 
 result=MMD_FILE_Mount();
 
 result=MMD_FILE_FindFirst((MMD_U08*)BPath,&FileFind);
 
 if(result==0) 
 {
  do 
  {
   if(!(FileFind.attr&MMD_FILE_ATTR_DIR))
   {  
      
    gFindFile.attr = FileFind.attr;
    memcpy(gFindFile.filename,FileFind.filename,MMD_FILE_MAXNAME);
    gFindFile.filesize = FileFind.filesize;
    memcpy(gFindFile.path,FileFind.filename,MMD_FILE_MAXPATH);
    memcpy(gFindFile.shortname,FileFind.filename,15);
    gFindFile.lnameflag = 1;
    gFindFile.namelen = MMD_FILE_MAXPATH;
    memcpy(gFindFile.lname,FileFind.filename,MMD_FILE_MAXNAME);
    //AASP_PrintUnicodeStr((unsigned char *)FileFind.filename);
    //MMI_FileList_AddItem(gFindFile.filename,gFindFile.shortname,gFindFile.attr,gFindFile.lnameflag,gFindFile.namelen);
    filenum++; 
   }
  }while (!MMD_FILE_FindNext(&FileFind));
  for(result=0;result<1000;result++);
 }
#endif // VC0848_FILE_SYS_TEST 
}
#if 0
 void vc_set_audiobias(void)
{
   ISound           *pSound; 
   AEESoundInfo     soundInfo;
 
   if (ISHELL_CreateInstance(AEE_GetShell(), AEECLSID_SOUND, (void **)&pSound)!=AEE_SUCCESS)
   {
      return;
   }
 
   soundInfo.eDevice = AEE_SOUND_DEVICE_HANDSET;
   soundInfo.eMethod = AEE_SOUND_METHOD_VOICE;
   soundInfo.eAPath = AEE_SOUND_APATH_BOTH;
 
   soundInfo.eEarMuteCtl = AEE_SOUND_MUTECTL_UNMUTED;
   soundInfo.eMicMuteCtl = AEE_SOUND_MUTECTL_MUTED;
 
   (void)ISOUND_Set(pSound,  &soundInfo);
   ISOUND_SetDevice(pSound);
 
   //ISOUND_SetVolume(pSound, GET_ISOUND_VOL_LEVEL(0));
   MSG_ERROR("==finished MMD_USER_SetAudio_Power setting==", 0, 0, 0);
}
#endif
extern void tstCapture(void);
void tstPreview()
{
 int ret;
 /*MMD_U32 width=176;
 MMD_U32 height=144;
 MMD_U32 ret_width,  ret_height;*/
 MMD_PREVIEW_OUTINFOR preview_param;
 
 MMD_U16 w,h;//
 MMD_U08 quality;
 MMD_U32 iSerNO=0;
 //media_engine_initialize();
 VC_GPIO_CTRL(42, HI); //enable camera power
 disppriv_set_window(0, 0, MMD_LCD_WIDTH, MMD_LCD_HEIGHT);
 MMD_SYS_PMU_SWITCH(0);
 MMD_USER_WaitMICRS(1000);
 
 ret=MMD_SYS_Mode_Switch(3);
FARF(MSG,("MMD_SYS_Mode_Switch=%d",ret));
ret=MMD_SYS_Codec_Switch(6);
FARF(MSG,("MMD_SYS_Codec_Switch=%d",ret));
 //ret=VIM_MAPI_AutoFindSensor(); // fing sensor first
  //(MSG,("VIM_MAPI_AutoFindSensor=%d",ret));
// ret=MMD_FILE_Mount();

 MMD_Media_System_Init();
 media_work_mode_exit(VIM_MEDIA_MODE_RECORD_AUDIO_AND_VIDEO);
 media_work_mode_initialize(VIM_MEDIA_MODE_CAPTURE);
 
 
 w=1600;
 h=1200;
 quality=15;
 
 preview_param.display_width= TST_LCD_WIDTH;
 preview_param.display_height= 132;//TST_LCD_HEIGHT;//270;//
 preview_param.display_xpos= 0;
 preview_param.display_ypos= 0;
 preview_param.display_effect= MMD_MEDIA_SPECIAL_NORMAL;
 MMD_Media_Set_Capture_FrameMode(MMD_MEDIA_CAPTURE_NOFRAME);
 ret = MMD_Media_Set_Preview_Parameter(&preview_param);
 ret = MMD_Media_Set_Capture_Resolution(w, h);
 ret = MMD_Media_Set_Capture_Quality(quality);
 ret =MMD_Media_Set_Thumbnail_Resolution(176, 132);
 MMD_LCD_BLY_Release();
 //ret=VIM_MAPI_AutoFindSensor(); // fing sensor first
 //FARF(MSG,("VIM_MAPI_AutoFindSensor=%d",ret));
 ret =MMD_Media_Preview_On();
 FARF(MSG,("MMD_Media_Preview_On=%d",ret));
 MMD_USER_WaitMICRS(3000*10);
// ret = MMD_FILE_Mount();
 //MSG_FATAL("%d == ret",ret, 0, 0);
 tstCapture();
}


 

 void testFileSystem()
{
 F_FILE * pfile=NULL;
 MMD_U32 i=1,j=0;
 int ret = 0;
 MMD_S08 testCreat[] = "test.2",fileno[4],tmp[64],fMode[2]="w";
 MSG_FATAL("find sd card %d", ret, 0, 0);
 MSG_FATAL("===========find sd card %d==============", ret, 0, 0);
 ret=MMD_FILE_Mount();
 MSG_FATAL("===========find sd card %d==============", ret, 0, 0);
 if(ret)
 {
    // if mount file failed, turn off the camera power
    VC_GPIO_CTRL(42, LO);
    MMD_SYS_PMU_SWITCH(1);
    MMD_USER_WaitMICRS(3000);
    return;
 }
 //for(i=0;i<100;i++)
 {
  //memset(testCreat,0,64);
  memset(fileno,0,4);
  memset(tmp,0,64);
  //strcpy(testCreat,"test.1");
  //sprintf(fileno,"%d",i);
  //strcat(testCreat,fileno);
  for(j=0;j<6;j++)
  {tmp[2*j]=testCreat[j];}
 
  MMD_FILE_Open((MMD_U08 *)tmp,(MMD_U08 *)fMode,&pfile);
  MMD_FILE_Close(pfile);
  MMD_USER_WaitMICRS(1000);
 }
}

MMD_PLYFILE  test_pPlayFileHandle;
 
static char tst_file[] = {'2', 0, '.', 0, 'j', 0,'p', 0, 'g', 0, 0, 0};  
//static char tst_file[] = {'s', 0, 'h', 0, 'u', 0,'i', 0, 'j', 0, 'i', 0, 'n', 0, 'g', 0, '.', 0, 'm', 0, 'p', 0, '3', 0, 0, 0};  
  
 void tstCreateFile(void)
 {
  int   count;
  char str[40];
  int ret,i; 
  int namelength;
  UINT32 f_len = 0;
  UINT8 f_mode[] = {'w', '+', 0, 0, 0};
  char filename[FILE_LEN_UTF8], sfilenum[4];
  char ufilename[FILE_LEN_UNICODE]; 
  RMEDIA_FILE  test_recMediaFile;
  MMD_PLYFILE  test_pfilehandle;
  F_FILE * pFile = NULL;
  ret=MMD_FILE_Mount();
  if(ret)
  {
     return;
  }
#if 0   
  memset(filename,0,FILE_LEN_UTF8);
  memset(&test_recMediaFile,0,sizeof(RMEDIA_FILE));
  
  test_recMediaFile.v_recwidth=800;
  test_recMediaFile.v_recheight=600;
  test_recMediaFile.v_codec=MEDIA_VIDEO_TYPE_JPG;
  //sprintf(filename,tst_file);
  
  test_recMediaFile.fs_filetype=MMD_MEDIA_FILE_TYPE_JPEG;
  test_recMediaFile.fs_filestore =1;
  
  namelength = 5; //strlen((char*)filename);
  
  memset(ufilename,0,FILE_LEN_UNICODE);
  for(i=0;i<namelength;i++)
   {ufilename[2*i]=filename[i];}
  memcpy((char*)test_recMediaFile.fs_filename, (const char *)tst_file, namelength*2);
  
  
  //ret=MMD_Recorder_File_Creat(NULL, &test_recMediaFile, &test_pfilehandle);
  ret = MMD_FILE_Open((MMD_U08*)tst_file, f_mode, &pFile);
  if(ret!=0)
   return;
#if 0  
  ret = MMD_FILE_Write(pFile, (MMD_U08*)tst_file, sizeof(tst_file), &f_len);
  if(ret != 0)
  {
    return;
  }
#endif  
  ret = MMD_FILE_Close(pFile);
  if(ret != 0)
  {
    return;
  }
  //ret=MMD_Recorder_File_Close(NULL, test_pfilehandle);
#endif    
  
 }

 
void tstCapture(void)
{
#include "bio_mb6100.h"    
 int   count;
 char str[40];
 int ret,i; 
 int namelength;
 char filename[FILE_LEN_UTF8], sfilenum[4];
 char ufilename[FILE_LEN_UNICODE]; 
 RMEDIA_FILE  test_recMediaFile;
 MMD_PLYFILE  test_pfilehandle;
 //tstPreview();
 ret=MMD_FILE_Mount();
 MSG_FATAL("test the fle mount %d ", ret, 0, 0);
 if(ret)
 {
    // if mount file failed, turn off the camera power
    VC_GPIO_CTRL(42, LO);
    MMD_SYS_PMU_SWITCH(1);
    MMD_USER_WaitMICRS(3000);
    return;
 }
 memset(filename,0,FILE_LEN_UTF8);
 memset(&test_recMediaFile,0,sizeof(RMEDIA_FILE));
 
 test_recMediaFile.v_recwidth=1600;
 test_recMediaFile.v_recheight=1200;
 test_recMediaFile.v_codec=MEDIA_VIDEO_TYPE_JPG;
 sprintf(filename,"imgtest.jpg");
 
 test_recMediaFile.fs_filetype=MMD_MEDIA_FILE_TYPE_JPEG;
 test_recMediaFile.fs_filestore =1;
 
 namelength = strlen((char*)filename);
 memset(ufilename,0,FILE_LEN_UNICODE);
 for(i=0;i<namelength;i++)
  {ufilename[2*i]=filename[i];}
 memcpy((char*)test_recMediaFile.fs_filename, (const char *)ufilename, namelength*2);
 MMD_LCD_BLY_Release();
 
 ret=MMD_Recorder_File_Creat(NULL, &test_recMediaFile, &test_pfilehandle);
 MSG_FATAL("MMD_Recorder_File_Creat, return=%d",ret,0,0);
 if(ret!=0)
  return;
 ret=MMD_Recorder_File_Start( test_pfilehandle,NULL);
 
 MSG_FATAL("MMD_Recorder_File_Start, return=%d",ret,0,0);
 if(ret!=0)
  return;
 // Check the capturing state until it has stopped.
    
 MMD_Media_Preview_Off();
   
 ret=MMD_Recorder_File_Close(NULL, test_pfilehandle);
 
 MSG_FATAL("MMD_Recorder_File_Close, return=%d",ret,0,0);
 //User_Play_Record_Voice(); // not define this func here now, remove it temply
 MMD_Media_Preview_On();
 
}
 
 
 
void tstAudioRec()
{
 int ret,namelength,i,recTime;
 char filename[FILE_LEN_UTF8],ufilename[FILE_LEN_UNICODE];
 RMEDIA_FILE  test_recMediaFile;
 MMD_PLYFILE  test_pfilehandle;
  MMD_SYS_Mode_Switch(SCENE_REC_AMR);
 //D_SYS_Codec_Switch(CODEC_SAMR);
 ADIE_OUTM(ADIE_CODEC_EN2_R, ADIE_CODEC_EN2_EN_MICBIAS_M, ADIE_CODEC_EN2_EN_MICBIAS_ENA_V);
 ret=MMD_FILE_Mount();
 MMD_Media_System_Init();
 media_work_mode_exit(VIM_MEDIA_MODE_CAPTURE);
 ret=MMD_Media_Mode_Initialzie(MMD_MEDIA_MODE_RECORD_AUDIO_ONLY);
 recTime = 100;
 memset(filename,0,FILE_LEN_UTF8);
 memset(&test_recMediaFile,0,sizeof(RMEDIA_FILE));
 test_recMediaFile.fs_filetype=MMD_MEDIA_FILE_TYPE_AMR;
 test_recMediaFile.fs_filestore =1;  
 test_recMediaFile.f_limitedsize=5*1024*1024;//free space
 test_recMediaFile.f_limitedtime=86400000;//0x7fffffff;
  
 test_recMediaFile.a_codec=MEDIA_AUDIO_TYPE_SAMR;
 //AMR=4750, 5150, 5900, 6700, 7400, 7950, 10200, 12200\n, MP3=96000,128000,192000
 test_recMediaFile.a_recbitrate=12200;
 test_recMediaFile.a_recbits=16;
 test_recMediaFile.a_recchn=1;
 test_recMediaFile.a_recsmplrate=8000;
 

 sprintf(filename,"tstreca.amr");
 printf("Start Record File (%s) \n",filename);
 namelength = strlen((char*)filename);
 memset(ufilename,0,FILE_LEN_UNICODE);
 for(i=0;i<namelength;i++)
  {ufilename[2*i]=filename[i];}
 memcpy((char*)test_recMediaFile.fs_filename, (const char *)ufilename, namelength*2);
 MMD_Media_SetRecodeChannel(MMD_AUDIOCODEC_LCHANNEL);
MMD_Media_Codec_Set_Record_Mode(0, CODEC_MICPHONEIN);
MMD_Media_Codec_Set_Mute(0, 0);
MMD_Media_Codec_Set_Record_Volume(100, 100);
 ret=MMD_Recorder_File_Creat(NULL, &test_recMediaFile, &test_pfilehandle);
 if(ret!=0)
  return ;
 ret=MMD_Recorder_File_Start( test_pfilehandle,NULL);


#if 1 
 while(recTime)
 {
  recTime--;
  MMD_USER_WaitMICRS(1000);
 };
 
 MMD_Recorder_File_Stop(test_pfilehandle);
 MMD_USER_WaitMSEC(500);
 ret=MMD_Recorder_File_Close(NULL, test_pfilehandle);
 MSG_FATAL("Close File Ret (%d)",ret,0,0);
#endif 
}
 
void tstVideoRec()
{
 int ret,namelength,i,recTime;
 unsigned int size=50,addr=0x60110000;
 char filename[FILE_LEN_UTF8],ufilename[FILE_LEN_UNICODE];
 RMEDIA_FILE  test_recMediaFile;
 MMD_PLYFILE  test_pfilehandle;
 MMD_PREVIEW_OUTINFOR preview_param;
 ADIE_OUTM(ADIE_CODEC_EN2_R, ADIE_CODEC_EN2_EN_MICBIAS_M, ADIE_CODEC_EN2_EN_MICBIAS_ENA_V);
 
  MMD_SYS_Mode_Switch(SCENE_REC_AMR);
 
 ret=MMD_FILE_Mount();
MSG_FATAL("MMD_FILE_Mount ret(%d)",ret, 0, 0);
 MMD_Media_System_Init();
 
 media_work_mode_exit(VIM_MEDIA_MODE_CAPTURE);
 media_work_mode_initialize(VIM_MEDIA_MODE_RECORD_AUDIO_AND_VIDEO);
 
 preview_param.display_width= 176;//TST_LCD_WIDTH;
 preview_param.display_height= 132;//TST_LCD_HEIGHT;
 preview_param.display_xpos= 0;
 preview_param.display_ypos= 0;
 preview_param.display_effect= MMD_MEDIA_SPECIAL_NORMAL;
 
 ret =MMD_Media_Set_Preview_Parameter(&preview_param);
 ret =MMD_Media_Set_Video_Resolution(176, 144);
 ret =MMD_Media_Preview_On();
 MSG_FATAL("MMD_Media_Preview_On ret(%d)",ret, 0, 0);
 for(i=0;i<=9;i++)
{
	MMD_USER_WaitMICRS(1000);
}
 MSG_FATAL("come in video record", 0, 0, 0);
 recTime = 100;
 memset(filename,0,FILE_LEN_UTF8);
 memset(&test_recMediaFile,0,sizeof(RMEDIA_FILE));
 test_recMediaFile.fs_filetype=MMD_MEDIA_FILE_TYPE_3GPP;
 test_recMediaFile.fs_filestore =1;  
 test_recMediaFile.f_limitedsize=5*1024*1024;//free space
 test_recMediaFile.f_limitedtime=86400000;
  
 test_recMediaFile.a_codec=MEDIA_AUDIO_TYPE_SAMR;
 //AMR=4750, 5150, 5900, 6700, 7400, 7950, 10200, 12200\n, MP3=96000,128000,192000
 test_recMediaFile.a_recbitrate=12200;
 test_recMediaFile.a_recbits=16;
 test_recMediaFile.a_recchn=1;
 test_recMediaFile.a_recsmplrate=8000;
 
 test_recMediaFile.v_recfps = 15;
 test_recMediaFile.v_recwidth=176;//TST_LCD_WIDTH;//352;
 test_recMediaFile.v_recheight=144;//TST_LCD_HEIGHT;//288;
 test_recMediaFile.v_codec=MEDIA_VIDEO_TYPE_MP4V;
 
 
 
 sprintf(filename,"tstrecv.3gp");
 
 namelength = strlen((char*)filename);
 memset(ufilename,0,FILE_LEN_UNICODE);
 for(i=0;i<namelength;i++)
  {ufilename[2*i]=filename[i];}
 memcpy((char*)test_recMediaFile.fs_filename, (const char *)ufilename, namelength*2);
 MMD_Media_SetRecodeChannel(MMD_AUDIOCODEC_LCHANNEL);
MMD_Media_Codec_Set_Record_Mode(0, CODEC_MICPHONEIN);
MMD_Media_Codec_Set_Mute(0, 0);
MMD_Media_Codec_Set_Record_Volume(100, 100);
 ret=MMD_Recorder_File_Creat(NULL, &test_recMediaFile, &test_pfilehandle);
 if(ret!=0)
  return ;
 ret=MMD_Recorder_File_Start( test_pfilehandle,NULL);
 //MMD_Media_Codec_Set_Play_Volume(100, 100);
 MSG_FATAL("recored file start wait 10s", 0, 0, 0);
#if 1
 while(recTime)
 {
  recTime--;
  MMD_USER_WaitMICRS(1000);
 };
 
 MSG_FATAL("recored file start wait 10s stop", 0, 0, 0);
 MMD_Recorder_File_Stop(test_pfilehandle);
 tstReadReg(addr,size);
 MMD_USER_WaitMICRS(1000);
 ret=MMD_Recorder_File_Close(NULL, test_pfilehandle);
 MMD_Media_Preview_Off();
 MSG_FATAL("Close File Ret (%d)",ret,0,0);
#endif 
}

void TEST_PlayerCallback(MMD_U32 command, MMD_U32 param1, MMD_U32 param2)
{
 
 Uart_Printf("MMI_PlayerCallback: command=0x%08X  \r\n",command);
    switch(command)
     {
  case MMD_D2H_PLY_FILE_END:
  Uart_Printf("media file play finish! filehandle = 0x%08X \r\n",param2);
  iPlayEndFlag = 1;
         break;
        case MMD_D2H_PLY_FILE_ERROR: 
         Uart_Printf("media file play error! filehandle = 0x%08X \r\n",param2);
         break;
    default:
  Uart_Printf("Unsupported data acknowledge, command=0x%08X  \r\n",command);
  break;
     }
 
}

void tstDecodebackjpg(char * filename)
 {
    int   ret = 0,namelength,i,bufferlen=0;
    char*    pExtName,ufilename[1024];
    PMEDIA_FILE  test_plyMediaFile;
    MMD_PLYFILE  test_pPlayFileHandle;
    MSG_FATAL("decode jpeg test", 0, 0, 0);
    MMD_Media_System_Init();
    MMD_Media_Mode_Initialzie(MMD_MEDIA_MODE_PLAY_AUDIO_AND_VIDEO);
    MMD_SYS_Mode_Switch(SCENE_MEDIAPLAY);
    MMD_SYS_Codec_Switch(CODEC_MID);
    
    ret=MMD_FILE_Mount();   
    DBGPRINTF("DEBUG:Mount SD Card Ret (%d)\n",ret);
    
    namelength = strlen((char*)filename);
    memset(ufilename,0,1024);
    for(i=0;i<namelength;i++)
       {ufilename[2*i]=filename[i];}
    
    memset(&test_plyMediaFile,0,sizeof(PMEDIA_FILE));
    memcpy((char*)test_plyMediaFile.fs_filename,  (const char *)ufilename, namelength*2);
 
    test_plyMediaFile.fs_filestore = 1;//storetype;FILE_IN_CORE_FILE
    test_plyMediaFile.a_outbits = 16;
    test_plyMediaFile.a_outchn = 2;
    test_plyMediaFile.a_outsmplrate = 48000;//8000;//48000;
    test_plyMediaFile.v_outheight = 220;//270;//180;
    test_plyMediaFile.v_outwidth = 176;
    test_plyMediaFile.v_outeffect = 8;
    test_plyMediaFile.v_outxpos = 0;
    test_plyMediaFile.v_outypos = 0;
    test_plyMediaFile.v_outrotation = MMD_MEDIA_ROTATE_0;
    test_plyMediaFile.v_outeffect = MMD_MEDIA_SPECIAL_NORMAL;
    test_plyMediaFile.v_jpgmode= MMD_MEDIA_JPEG_MODE_DECODE;
    bufferlen=test_plyMediaFile.v_outheight*test_plyMediaFile.v_outwidth*2;
    test_plyMediaFile.v_jpgdecodeaddr=(MMD_U32)MMD_Malloc(bufferlen);
    test_plyMediaFile.v_jpgdecodelen=bufferlen;
    iPlayEndFlag = 0;
    
    ret = MMD_Player_File_Open(&test_plyMediaFile, &test_pPlayFileHandle);
    MSG_FATAL("%d =ret", ret, 0, 0);
    ret = MMD_Player_File_Play(test_pPlayFileHandle,(PLYCallback)TEST_PlayerCallback);
    MSG_FATAL("%d =ret", ret, 0, 0);
    while(!iPlayEndFlag)
    {
       MMD_USER_WaitMICRS(1000);
    }  
    
    MMD_Player_File_Close(test_pPlayFileHandle, NULL, NULL);
 
    //tstLCD_Refresh_RGB565((MMD_U08 *)test_plyMediaFile.v_jpgdecodeaddr,0,0,176,220);
    ret=MMD_LCD_Refresh(MMD_LCD_B0LAYER, 0, 0, 176, 220, (MMD_U08 *)test_plyMediaFile.v_jpgdecodeaddr, 0);
    MSG_FATAL("%d =ret", ret, 0, 0);
    MMD_Free((void *)test_plyMediaFile.v_jpgdecodeaddr);
    
    
 }

 
void tstPlayback3gp(char * filename)
{
 int  ret = 0,namelength,i;
 char*  pExtName,ufilename[FILE_LEN_UNICODE];
 PMEDIA_FILE  test_plyMediaFile;
 MMD_PLYFILE  test_pPlayFileHandle_3gp;
 
 MMD_SYS_Mode_Switch(SCENE_MEDIAPLAY);
 MMD_SYS_Codec_Switch(CODEC_AMR);

MMD_LCD_BLY_Release();
 
 media_work_mode_initialize(MMD_MEDIA_MODE_PLAY_AUDIO_AND_VIDEO);
 
 ret=MMD_FILE_Mount();
 MSG_FATAL("MMD_FILE_Mount ret(%d)",ret, 0, 0);
 memset(&test_plyMediaFile,0,sizeof(PMEDIA_FILE));
 
 namelength = strlen((char*)filename);
 memset(ufilename,0,FILE_LEN_UNICODE);
 for(i=0;i<namelength;i++)
  {ufilename[2*i]=filename[i];}
 memcpy((char*)test_plyMediaFile.fs_filename, (const char *)ufilename, namelength*2);
 
 
 test_plyMediaFile.fs_filestore = 1;//storetype;FILE_IN_CORE_FILE
 test_plyMediaFile.a_outbits = 16;
 test_plyMediaFile.a_outchn = 2;
 test_plyMediaFile.a_outsmplrate = 8000;//8000;//48000;
 test_plyMediaFile.v_outheight = 144;//TST_LCD_HEIGHT;//270;//180;
 test_plyMediaFile.v_outwidth = 176;//TST_LCD_WIDTH;
 test_plyMediaFile.v_outeffect = 8;
 test_plyMediaFile.v_outxpos = 0;
 test_plyMediaFile.v_outypos = 0;
 test_plyMediaFile.v_outrotation = MMD_MEDIA_ROTATE_0;
 test_plyMediaFile.v_outeffect = MMD_MEDIA_SPECIAL_NORMAL;
 
 MMD_Media_Set_AudioMode(1);
MMD_Media_Codec_Set_Play_Mode(0,0);
MMD_Media_Codec_Set_Mute(1,0);
MMD_Media_Codec_Set_Play_Volume(100, 100);

MSG_FATAL("start play 3gp(%d)",ret, 0, 0);

 ret = MMD_Player_File_Open(&test_plyMediaFile, &test_pPlayFileHandle_3gp);
 MSG_FATAL("MMD_Player_File_Open ret(%d)",ret, 0, 0);
 iPlayEndFlag = 0;
 ret =MMD_Player_File_Play(test_pPlayFileHandle_3gp,(PLYCallback)TEST_PlayerCallback);
  MSG_FATAL("MMD_Player_File_Play ret(%d)",ret, 0, 0);
#if 1  ////MODIFY 20081231
 while(!iPlayEndFlag)
 {
  MMD_USER_WaitMSEC(100);
 }
 printf("Play End!\n");
 MMD_Player_File_Close(test_pPlayFileHandle_3gp,NULL,NULL);
 MSG_FATAL("MMD_Player_File_Close ret(%d)",ret, 0, 0);
 test_pPlayFileHandle_3gp = NULL;
 MMD_LCD_BLY_Release();
#endif 
 return;
 
}
 

 
void tstPlaybackmp3(char * filename)
{
 int  ret = 0,namelength,i;
 char*  pExtName,ufilename[FILE_LEN_UNICODE];
 unsigned int size=50,addr=0x60110000;
 PMEDIA_FILE  test_plyMediaFile;
 
 MMD_SYS_Mode_Switch(2);
 MMD_SYS_Codec_Switch(CODEC_MP3);


 MMD_LCD_BLY_Release();
 
 media_work_mode_initialize(MMD_MEDIA_MODE_PLAY_AUDIO_ONLY);
 
 ret=MMD_FILE_Mount();
 MSG_FATAL("%d == ret", ret, 0, 0);
 memset(&test_plyMediaFile,0,sizeof(PMEDIA_FILE));
 
 namelength = strlen((char*)filename);
 memset(ufilename,0,FILE_LEN_UNICODE);
 for(i=0;i<namelength;i++)
  {ufilename[2*i]=filename[i];}
  
 memcpy((char*)test_plyMediaFile.fs_filename, (const char *)ufilename, namelength*2);
 
 
 test_plyMediaFile.fs_filestore = 1;//storetype;1: SD, FILE_IN_CORE_FILE  0: NOR
 test_plyMediaFile.a_outbits = 16;
 test_plyMediaFile.a_outchn = 2;
 test_plyMediaFile.a_outsmplrate = 48000;//8000;//48000;
 test_plyMediaFile.v_outheight = TST_LCD_HEIGHT;//270;//180;
 test_plyMediaFile.v_outwidth = TST_LCD_WIDTH;
 test_plyMediaFile.v_outeffect = 8;
 test_plyMediaFile.v_outxpos = 0;
 test_plyMediaFile.v_outypos = 0;
 test_plyMediaFile.v_outrotation = MMD_MEDIA_ROTATE_0;
 test_plyMediaFile.v_outeffect = MMD_MEDIA_SPECIAL_NORMAL;
 //test_plyMediaFile.fs_filetype = MMD_MEDIA_FILE_TYPE_MP3;
 
 ret=MMD_Media_Set_AudioMode(MMD_MEDIA_AUDIO_PCM);
 MSG_FATAL("%d == ret", ret, 0, 0);
 //MMD_Media_PlayMixEnable();
 
 //ret=MMD_Media_Codec_Set_Play_Mode(0,0);
  ret=MMD_Media_Codec_Set_Play_Mode(MMD_MEDIA_STEROECHANNEL, MMD_MEDIA_HEADPHONEOUT);
 MSG_FATAL("%d == ret", ret, 0, 0);
//MMD_Media_Codec_Set_Mute(1,0);
ret=MMD_Media_Codec_Set_Play_Volume(100, 100);
 MSG_FATAL("%d == ret", ret, 0, 0);


 ret = MMD_Player_File_Open(&test_plyMediaFile, &test_pPlayFileHandle);
 MSG_FATAL("%d= ret", ret, 0, 0);
MMD_Set_HpSpkType(); 
 iPlayEndFlag = 0;
 ret =MMD_Player_File_Play(test_pPlayFileHandle,(PLYCallback)TEST_PlayerCallback);
 //tstReadReg(addr,size);
 MSG_FATAL("%d= ret", ret, 0, 0);

 MSG_FATAL("start play ret(%d)",ret, 0, 0);
 #if 1
 while(!iPlayEndFlag)
 {
   MMD_USER_WaitMICRS(1000);
  //MMD_USER_WaitMSEC(100);
 }
MSG_FATAL("start end",0, 0, 0);
 ret=MMD_Player_File_Close(test_pPlayFileHandle,NULL,NULL);
 MSG_FATAL("%d == ret", ret, 0, 0);
 test_pPlayFileHandle = NULL;
 MMD_LCD_BLY_Release();
#endif
 return;
 
}


 
void tstPlaybackmp4(char * filename)
{
 int  ret = 0,namelength,i;
 char*  pExtName,ufilename[FILE_LEN_UNICODE];
 PMEDIA_FILE  test_plyMediaFile;
 //MMD_PLYFILE  test_pPlayFileHandle;
 
 MMD_SYS_Mode_Switch(SCENE_MEDIAPLAY);
 MMD_SYS_Codec_Switch(CODEC_AMR);

 MMD_LCD_BLY_Release();
 
 media_work_mode_initialize(MMD_MEDIA_MODE_PLAY_AUDIO_AND_VIDEO);
 
 ret=MMD_FILE_Mount();
 memset(&test_plyMediaFile,0,sizeof(PMEDIA_FILE));
 
 namelength = strlen((char*)filename);
 memset(ufilename,0,FILE_LEN_UNICODE);
 for(i=0;i<namelength;i++)
  {ufilename[2*i]=filename[i];}
 memcpy((char*)test_plyMediaFile.fs_filename, (const char *)ufilename, namelength*2);
 
 
 test_plyMediaFile.fs_filestore = 1;//storetype;FILE_IN_CORE_FILE
 test_plyMediaFile.a_outbits = 16;
 test_plyMediaFile.a_outchn = 2;
 //test_plyMediaFile.a_outsmplrate = 48000;//8000;//48000;
 test_plyMediaFile.v_outheight = TST_LCD_HEIGHT;//270;//180;
 test_plyMediaFile.v_outwidth = TST_LCD_WIDTH;
 //test_plyMediaFile.v_outeffect = 8;
 test_plyMediaFile.v_outxpos = 0;
 test_plyMediaFile.v_outypos = 0;
 //test_plyMediaFile.v_outrotation = MMD_MEDIA_ROTATE_0;
 test_plyMediaFile.v_outeffect = MMD_MEDIA_SPECIAL_NORMAL;
 MMD_Media_Set_AudioMode(1);
MMD_Media_Codec_Set_Play_Mode(0,0);
MMD_Media_Codec_Set_Mute(1,0);
MMD_Media_Codec_Set_Play_Volume(100, 100);
 ret = MMD_Player_File_Open(&test_plyMediaFile, &test_pPlayFileHandle);
 
 iPlayEndFlag = 0;
 ret =MMD_Player_File_Play(test_pPlayFileHandle,(PLYCallback)TEST_PlayerCallback);
#if 0 
 while(!iPlayEndFlag)
 {
  MMD_USER_WaitMSEC(100);
 }
 printf("Play End!\n");
 MMD_Player_File_Close(test_pPlayFileHandle,NULL,NULL);
 test_pPlayFileHandle = NULL;
 MMD_LCD_BLY_Release();
#endif 
 return;
 
}

int iRecordEndFlag=0;
void TEST_recorderCallback(MMD_U32 command, MMD_U32 param1, MMD_U32 param2)
{
	
	Uart_Printf("TEST_PlayerCallback: command=0x%08X  \r\n",command);
   	switch(command)
    	{
	 case MMD_D2H_REC_FILE_OK:
	 case MMD_D2H_REC_FILE_END:
		Uart_Printf("media file recorder  finish! filehandle = 0x%08X \r\n",param1);
		iRecordEndFlag = 1;
        	break;
    case MMD_D2H_REC_FILE_ERROR:	
      	 	Uart_Printf("media file recorder error! filehandle = 0x%08X \r\n",param1);
		iRecordEndFlag = 1;
      	 	break;
	 case MMD_D2H_CAP_AUD_PLY:
	 	break;
	 case MMD_D2H_CAP_RAM_FILELEN:	
      	 	Uart_Printf("jpg image capure in bb ram, length = 0x%08X \r\n",param1);
		iRecordEndFlag = param1;
      	 	break;	
   	default:
		Uart_Printf("Unsupported data acknowledge, command=0x%08X  \r\n",command);
		break;
    	}
}

void tstLineinRec()
{
	int ret,namelength,i,recTime;
	char filename[512],ufilename[1024];
	RMEDIA_FILE  test_recMediaFile;
	MMD_PLYFILE  test_pfilehandle;
 	//MMD_SYS_Mode_Switch(SCENE_REC_AMR);
	//MMD_SYS_Codec_Switch(CODEC_SAMR);
	
	ret=MMD_FILE_Mount();
	MMD_Media_System_Init();

	ret=MMD_Media_Mode_Initialzie(MMD_MEDIA_MODE_RECORD_AUDIO_ONLY);
	recTime = 20;
	memset(filename,0,512);
	memset(&test_recMediaFile,0,sizeof(RMEDIA_FILE));
	memset(filename,0,64);
	
	test_recMediaFile.fs_filetype=MMD_MEDIA_FILE_TYPE_MP3;
	test_recMediaFile.fs_filestore =1;		
	test_recMediaFile.f_limitedsize=5*1024*1024;//free space
	test_recMediaFile.f_limitedtime=86400000;//0x7fffffff;
		
	test_recMediaFile.a_codec=MEDIA_AUDIO_TYPE_MP3;
	//AMR=4750, 5150, 5900, 6700, 7400, 7950, 10200, 12200\n, MP3=96000,128000,192000
	test_recMediaFile.a_recbitrate=96000;
	test_recMediaFile.a_recbits=16;
	test_recMediaFile.a_recchn=2;
	test_recMediaFile.a_recsmplrate=32000;

	sprintf(filename,"tstRecA.mp3");
	//printf("Start Record File (%s) \n",filename);
	namelength = strlen((char*)filename);
	memset(ufilename,0,1024);
	for(i=0;i<namelength;i++)
		{ufilename[2*i]=filename[i];}
	memcpy((char*)test_recMediaFile.fs_filename, (const char *)ufilename, namelength*2);
	MMD_Media_Codec_Set_Record_Volume(90,90);
	MMD_Media_Codec_Set_Record_Mode(MMD_MEDIA_STEROECHANNEL,CODEC_LINEIN);
	ret=MMD_Recorder_File_Creat(TEST_recorderCallback, &test_recMediaFile, &test_pfilehandle);
   FARF(MSG,("MMD_Recorder_File_Creat=%d",ret));
	if(ret!=0)
		return ;
	ret=MMD_Recorder_File_Start( test_pfilehandle,NULL);
	FARF(MSG,("MMD_Recorder_File_Start=%d",ret));
	
	while(recTime)
	{
		recTime--;
		MMD_USER_WaitMICRS(1000*10);
	}

	ret=MMD_Recorder_File_Stop(test_pfilehandle);
	FARF(MSG,("MMD_Recorder_File_Stop=%d",ret));
	while(!iRecordEndFlag)
	{
		MMD_USER_WaitMICRS(1000);
	}
	
	ret=MMD_Recorder_File_Close(NULL, test_pfilehandle);
   FARF(MSG,("MMD_Recorder_File_Close=%d",ret));

}


void tstPlaybackamr(char * filename)
{
 int  ret = 0,namelength,i;
 char*  pExtName,ufilename[FILE_LEN_UNICODE];
 PMEDIA_FILE  test_plyMediaFile;
 //MMD_PLYFILE  test_pPlayFileHandle;
 
 MMD_SYS_Mode_Switch(SCENE_MEDIAPLAY);
 MMD_SYS_Codec_Switch(CODEC_AMR);

 
 MMD_LCD_BLY_Release();
 ret=MMD_FILE_Mount();
 media_work_mode_initialize(MMD_MEDIA_MODE_PLAY_AUDIO_ONLY);
 
 memset(&test_plyMediaFile,0,sizeof(PMEDIA_FILE));
 memcpy((char*)test_plyMediaFile.fs_filename, (const char *)filename, FILE_LEN_UNICODE);
 
 test_plyMediaFile.fs_filestore = 1;//storetype;FILE_IN_CORE_FILE
 test_plyMediaFile.a_outbits = 16;
 test_plyMediaFile.a_outchn = 2;
 test_plyMediaFile.a_outsmplrate = 48000;//8000;//48000;
 test_plyMediaFile.v_outheight = TST_LCD_HEIGHT;//270;//180;
 test_plyMediaFile.v_outwidth = TST_LCD_WIDTH;
 test_plyMediaFile.v_outeffect = 8;
 test_plyMediaFile.v_outxpos = 0;
 test_plyMediaFile.v_outypos = 0;
 test_plyMediaFile.v_outrotation = MMD_MEDIA_ROTATE_0;
 test_plyMediaFile.v_outeffect = MMD_MEDIA_SPECIAL_NORMAL;
 MMD_Media_Set_AudioMode(1);
		MMD_Media_Codec_Set_Play_Mode(0,0);
 ret = MMD_Player_File_Open(&test_plyMediaFile, &test_pPlayFileHandle);
 iPlayEndFlag = 0;
 ret =MMD_Player_File_Play(test_pPlayFileHandle,(PLYCallback)TEST_PlayerCallback);
#if 0 
 while(!iPlayEndFlag)
 {
  MMD_USER_WaitMSEC(100);
 }
 
 MMD_Player_File_Close(test_pPlayFileHandle, NULL, NULL);
 test_pPlayFileHandle = NULL;
 MMD_LCD_BLY_Release();
 printf("Play End!\n");
#endif 
}
 
#if 0
void tstPlaybackjpg(char * filename)
{
 int  ret = 0,namelength,i;
 char*  pExtName,ufilename[FILE_LEN_UNICODE];
 PMEDIA_FILE  test_plyMediaFile;
 MMD_PLYFILE  test_pPlayFileHandle;
 
 MMD_SYS_Mode_Switch(SCENE_MEDIAPLAY);
 MMD_SYS_Codec_Switch(CODEC_AMR);

 //MMD_LCD_BLY_Release();
 
 ret=MMD_FILE_Mount();
 media_work_mode_initialize(MMD_MEDIA_MODE_PLAY_AUDIO_AND_VIDEO);
 
 memset(&test_plyMediaFile,0,sizeof(PMEDIA_FILE));
 memcpy((char*)test_plyMediaFile.fs_filename, (const char *)filename, FILE_LEN_UNICODE);
 
 test_plyMediaFile.fs_filestore = 1;//storetype;FILE_IN_CORE_FILE
 test_plyMediaFile.a_outbits = 16;
 test_plyMediaFile.a_outchn = 2;
 test_plyMediaFile.a_outsmplrate = 48000;//8000;//48000;
 test_plyMediaFile.v_outheight = TST_LCD_HEIGHT;//270;//180;
 test_plyMediaFile.v_outwidth = TST_LCD_WIDTH;
 test_plyMediaFile.v_outeffect = 8;
 test_plyMediaFile.v_outxpos = 0;
 test_plyMediaFile.v_outypos = 0;
 test_plyMediaFile.v_outrotation = MMD_MEDIA_ROTATE_0;
 test_plyMediaFile.v_outeffect = MMD_MEDIA_SPECIAL_NORMAL;
 MMD_Media_Set_AudioMode(1);
MMD_Media_Codec_Set_Play_Mode(0,0);
 
 ret = MMD_Player_File_Open(&test_plyMediaFile, &test_pPlayFileHandle);
 ret =MMD_Player_File_Play(test_pPlayFileHandle,NULL);

 for(i=0;i<5;i++)
 {
 	MMD_USER_WaitMSEC(200);
 }
 MMD_Player_File_Close(test_pPlayFileHandle, NULL, NULL);
 test_pPlayFileHandle = NULL;
 //MMD_LCD_BLY_Release();
}
 #else
extern void gpio_invert_pupd_state(uint32 gpio_number);
extern void gpio_recover_pupd_state(uint32 gpio_number);
 
void tstUDiskEnable()
{
 USB_MASS_INFO test_usb_mass;
 
 //gpio_invert_pupd_state(43); // enable USB_SEL for VC0848 USB selection
 MMD_SYS_Mode_Switch(SCENE_USB);
 
 test_usb_mass.device =0;
 test_usb_mass.speed =0;
 MMD_USB_Enable(USB_MASS_STORAGE, NULL);
 MMD_USER_WaitMSEC(100);
 MMD_USB_SetProperty(USB_MASS_STORAGE, &test_usb_mass);
 MMD_USER_WaitMSEC(100);
 //MMD_USB_Disable(USB_MASS_STORAGE);
}

 void tstPlaybackjpg(char * filename)
{
#if 0
	int 	ret = 0,namelength,i;
	char* 	pExtName,ufilename[1024];
	PMEDIA_FILE  test_plyMediaFile;
	MMD_PLYFILE  test_pPlayFileHandle;
	
	MMD_SYS_Mode_Switch(SCENE_MEDIAPLAY);
	MMD_SYS_Codec_Switch(CODEC_MID);
	
	ret=MMD_FILE_Mount();
	//media_work_mode_initialize(MMD_MEDIA_MODE_PLAY_AUDIO_AND_VIDEO);
	MMD_Media_Mode_Initialzie(MMD_MEDIA_MODE_PLAY_AUDIO_AND_VIDEO);
	namelength = strlen((char*)filename);
	memset(ufilename,0,1024);
	for(i=0;i<namelength;i++)
		{ufilename[2*i]=filename[i];}
	
	memset(&test_plyMediaFile,0,sizeof(PMEDIA_FILE));
	//memcpy((char*)test_plyMediaFile.fs_filename, (const char *)filename, 512);
	memcpy((char*)test_plyMediaFile.fs_filename, (const char *)ufilename, 1024);

	test_plyMediaFile.fs_filestore = 1;//storetype;FILE_IN_CORE_FILE
	test_plyMediaFile.a_outbits = 16;
	test_plyMediaFile.a_outchn = 2;
	test_plyMediaFile.a_outsmplrate = 48000;//8000;//48000;
	test_plyMediaFile.v_outheight = 270;//270;//180;
	test_plyMediaFile.v_outwidth = 240;
	test_plyMediaFile.v_outeffect = 8;
	test_plyMediaFile.v_outxpos = 0;
	test_plyMediaFile.v_outypos = 0;
	test_plyMediaFile.v_outrotation = MMD_MEDIA_ROTATE_0;
	test_plyMediaFile.v_outeffect = MMD_MEDIA_SPECIAL_NORMAL;
	
	ret = MMD_Player_File_Open(&test_plyMediaFile, &test_pPlayFileHandle);
	ret =MMD_Player_File_Play(test_pPlayFileHandle,NULL);
	MMD_USER_WaitMICRS(2000);
	MMD_Player_File_Close(test_pPlayFileHandle, NULL, NULL);
#else
	 int  ret = 0,namelength,i;
	 char*  pExtName,ufilename[1024];
 	 PMEDIA_FILE  test_plyMediaFile;
        MMD_PLYFILE  test_pPlayFileHandle;
        
        MMD_SYS_Mode_Switch(SCENE_MEDIAPLAY);
        MMD_SYS_Codec_Switch(CODEC_AMR);
        
        //MMD_LCD_BLY_Release();
        
        ret=MMD_FILE_Mount();
	MSG_FATAL("MMD_FILE_Mount ret(%d)",ret, 0, 0);
       //media_work_mode_initialize(MMD_MEDIA_MODE_PLAY_AUDIO_AND_VIDEO);
	MMD_Media_Mode_Initialzie(MMD_MEDIA_MODE_PLAY_AUDIO_AND_VIDEO);
	   MMD_LCD_SetOverlay(1,0,0,0);

	namelength = strlen((char*)filename);
	memset(ufilename,0,1024);
	for(i=0;i<namelength;i++)
		{ufilename[2*i]=filename[i];}
        
        memset(&test_plyMediaFile,0,sizeof(PMEDIA_FILE));
        //memcpy((char*)test_plyMediaFile.fs_filename, (const char *)filename, 512);
        memcpy((char*)test_plyMediaFile.fs_filename, (const char *)ufilename, 1024);
        
        test_plyMediaFile.fs_filestore = 1;//storetype;FILE_IN_CORE_FILE
        test_plyMediaFile.a_outbits = 16;
        test_plyMediaFile.a_outchn = 2;
        test_plyMediaFile.a_outsmplrate = 48000;//8000;//48000;
        test_plyMediaFile.v_outheight = 220;//TST_LCD_HEIGHT;//270;//180;
        test_plyMediaFile.v_outwidth = 176;//TST_LCD_WIDTH;
        test_plyMediaFile.v_outeffect = 8;
        test_plyMediaFile.v_outxpos = 0;
        test_plyMediaFile.v_outypos = 0;
        test_plyMediaFile.v_outrotation = MMD_MEDIA_ROTATE_0;
        test_plyMediaFile.v_outeffect = MMD_MEDIA_SPECIAL_NORMAL;
        MMD_Media_Set_AudioMode(1);
        MMD_Media_Codec_Set_Play_Mode(0,0);
		
        MSG_FATAL("start display jpg(%d)",ret, 0, 0);
	
        ret = MMD_Player_File_Open(&test_plyMediaFile, &test_pPlayFileHandle);
	 MSG_FATAL("MMD_Player_File_Open ret(%d)",ret, 0, 0);
        ret =MMD_Player_File_Play(test_pPlayFileHandle,NULL);
        MSG_FATAL("MMD_Player_File_Play ret(%d)",ret, 0, 0);
      	 MMD_USER_WaitMICRS(2000);
	
        MMD_Player_File_Close(test_pPlayFileHandle, NULL, NULL);
	 MSG_FATAL("MMD_Player_File_Close ret(%d)",ret, 0, 0);	
	 MSG_FATAL("end display ret(%d)",ret, 0, 0);
        test_pPlayFileHandle = NULL;
        //MMD_LCD_BLY_Release();
#endif

}
 #endif
 
void tstUDiskDisable()
{
 //gpio_recover_pupd_state(43); // disable USB_SEL for vc0848 USB selection  
 MMD_USB_Disable(USB_MASS_STORAGE);    
}
#endif



