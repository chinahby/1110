#ifndef  MEDIAGALLERY_H
#define  MEDIAGALLERY_H
/**--------------------------------------------------------------------------
 * MediaGallery.h -- define IMediaGallery interface.
 *
 * Copyright(c) 2008 Anylook
 *
 * History:
 *
 * when         who            what and why
 * -----------  -----------    -----------------------------
 * 2008.10.11       Create.
 *
 **--------------------------------------------------------------------------
 */
#include "AEE.h"
#include "AEE_OEM.h"
#include "AEEFile.h"
#include "AEEDB.h"
#include "AEEDeviceNotifier.h"
#include "AEEVector.h"

/*-----------------------------------------------------------------------------
 *  Feature define
 *-----------------------------------------------------------------------------*/
//#ifdef FEATURE_FS_LONGPATH
/*长路径支持*/
#define FEATURE_MG_LONGPATH
//#endif

/*把SD当作一个普通文件夹浏览*/
#define FEATURE_MG_MASSCARD_EXPLORER

#define FEATURE_MG_NOHSVIDEO

/*支持按类型排序*/
#define FEATURE_SORT_TYPE

/*支持文件复制/移动功能*/
#if defined(FEATURE_CARRIER_CHINA_VERTU) || defined(AEE_SIMULATOR)
#define FEATRUE_MG_COPYMOVE
#endif

//#define FEATURE_MG_INTDBG

//
#if defined( FEATURE_SUPPORT_WAP_APP ) || defined(AEE_SIMULATOR)
//#define FEATURE_MG_WAPMMS_SUPPORT
#endif


/* ---------------------------------------------------------------------------
 * path to store meida files
 * ---------------------------------------------------------------------------
 */
/*multimedia file store path in handset */
#define MG_PHONE_ROOTDIR            "fs:/hsmm/"
/*multimedia file store path in mass storge medium*/
#ifndef AEE_SIMULATOR
#define MG_MASSCARD_ROOTDIR         AEEFS_CARD0_DIR
#else
#define MG_MASSCARD_ROOTDIR         "fs:/mmc/"
#endif
#define MUSICPLAYLIST_DIR      "fs:/shared/playlist"
#define DEFAULT_PLAYLIST       "Default playlist"

#ifdef FEATURE_VERSION_M8P
#define MG_PICTURES_FOLDER          "Gambar"
#define MG_MUSIC_FOLDER             "Musik"
#define MG_VIDEOS_FOLDER            "video"
#define MG_VOICEMEMO_FOLDER         "voicememo"
#define MG_CALLMEMO_FOLDER          "callmemo"
#define MG_DOWNLOAD_FOLDER          "download"
#else
#define MG_PICTURES_FOLDER          "pictures"
#if defined (FEATURE_VERSION_C337) || defined(FEATURE_VERSION_IC241A_MMX)
#define MG_MUSIC_FOLDER				"My Music"
#define MG_VIDEOS_FOLDER            "videos"
#else
#define MG_MUSIC_FOLDER             "music"
#define MG_VIDEOS_FOLDER            "video"
#endif
//#ifdef FEATURE_VERSION_C316
#define MG_AUTOCALLRECOED_FOLDER    "auto call record"    //Add by pyuangui 2013-01-09
//#endif
#ifdef FEATURE_VERSION_K232_Y105A
#define MG_VOICEMEMO_FOLDER         "Audio"
#define MG_VOICEMEMO_FOLDER_2       "Recorder"
#else

#define MG_VOICEMEMO_FOLDER         "voicememo"
#endif
#define MG_CALLMEMO_FOLDER          "callmemo"
#define MG_DOWNLOAD_FOLDER          "download"
#endif


#define MG_PHONEPICTURE_PATH        MG_PHONE_ROOTDIR MG_PICTURES_FOLDER  DIRECTORY_STR
#define MG_PHONEMUSIC_PATH          MG_PHONE_ROOTDIR MG_MUSIC_FOLDER     DIRECTORY_STR
#define MG_PHONEVIDEOS_PATH         MG_PHONE_ROOTDIR MG_VIDEOS_FOLDER    DIRECTORY_STR
#define MG_PHONECALLMEMO_PATH       MG_PHONE_ROOTDIR MG_CALLMEMO_FOLDER  DIRECTORY_STR
#define MG_PHONEDOWNLOAD_PATH       MG_PHONE_ROOTDIR MG_DOWNLOAD_FOLDER DIRECTORY_STR

#define MG_MASSCARDPICTURE_PATH     MG_MASSCARD_ROOTDIR MG_PICTURES_FOLDER  DIRECTORY_STR
#define MG_MASSCARDMUSIC_PATH       MG_MASSCARD_ROOTDIR MG_MUSIC_FOLDER     DIRECTORY_STR
#define MG_MASSCARDVIDEO_PATH       MG_MASSCARD_ROOTDIR MG_VIDEOS_FOLDER    DIRECTORY_STR
#ifdef FEATURE_VERSION_K232_Y105A
#define MG_MASSCARDVOICEMEMO_PATH   MG_MASSCARD_ROOTDIR MG_VOICEMEMO_FOLDER DIRECTORY_STR MG_VOICEMEMO_FOLDER_2 DIRECTORY_STR
#else
#define MG_MASSCARDVOICEMEMO_PATH   MG_MASSCARD_ROOTDIR MG_VOICEMEMO_FOLDER DIRECTORY_STR
#endif
#define MG_MASSCARDDOWNLOAD_PATH    MG_MASSCARD_ROOTDIR MG_DOWNLOAD_FOLDER DIRECTORY_STR
#ifdef FEATURE_VERSION_C316   //Add by pyuangui 2013-01-09
#define MG_MASSCARDCALLRECOED_PATH  MG_MASSCARD_ROOTDIR MG_AUTOCALLRECOED_FOLDER DIRECTORY_STR
#endif

#define MG_MASSCARDVOICEMEMO_PAHT   MG_MASSCARDVOICEMEMO_PATH 


/*BREW PEK 测试时重新定义一下路径*/
#ifdef FEATURE_PEKTEST
#undef MG_PHONE_ROOTDIR
#undef MG_PICTURES_FOLDER
#undef MG_PHONEPICTURE_PATH
#define MG_PHONE_ROOTDIR            AEEFS_MOD_DIR
#define MG_PICTURES_FOLDER          "10888"
#define MG_PHONEPICTURE_PATH        MG_PHONE_ROOTDIR MG_PICTURES_FOLDER DIRECTORY_STR
#endif


//the database name of video files
#define MG_VIDEOFILE_DB_NAME        "shared/videofiles.dat"
#define MG_WALLPAPER_CUSTPATH       "shared/cust/wallpaper."
#define MG_RINGTONE_CUSTPATH        "shared/cust/rington."


/* ---------------------------------------------------------------------------
 * macro for constant
 * ---------------------------------------------------------------------------
 */
#define MG_DIRECTORY_STR            DIRECTORY_STR
#define MG_DIR_DELIMITER            DIRECTORY_CHAR
#define MG_DIR_WDELIMITER           L'/'
#define MG_DOT_CHAR                 '.'
#define MG_DOT_WCHAR                L'.'
#define MG_LINEBREAK_WSTR           L"\n"
#define MG_DOUBLELINEBREK_WSTR      L"\n\n"

/*const data for media gallery*/
#define MG_MAX_SUBDIR_LEVEL         3 /*  subfolder number */
#define MG_MAX_FILES                1024
#ifdef FEATURE_MG_LONGPATH
#define MG_MAX_FILE_NAME            128
#else
#define MG_MAX_FILE_NAME            AEE_MAX_FILE_NAME /*64*/
#endif
#define MG_MAX_FILE_INPUTLEN        16
#define MG_MAX_FILESIZE             (uint32)(-1)/* file size in byte*/
#define MG_MAX_FILESPERDIR	         512/* file num in a directory*/
#define MG_MAX_TEXT_SIZE            512
#define MG_MAX_LINETEXTNUM          32

/*when create music play list, the max file conunt select */
#define MG_SELECT_MAX               50

/*照片解码最大的文件大小。4M。中星微在解19-20M图片直接死机。*/
#define MG_QUALIMGDECODE_MAX           4 * 128 * 1024

#define MG_WALLPAPER_MAX               2 * 128 * 1024
#define MG_WALLPAPER_PIXEL_MIN         1
#define MG_WALLPAPER_PIXEL_MAX         640

/*默认日期 */
#define MG_DEFAULT_JYEAR               1980
#define MG_DEFAULT_JMONTH              01
#define MG_DEFAULT_JDAY                01

/*Cache size for Database */
#define MG_DB_CACHE_SIZE            1024

/* ---------------------------------------------------------------------------
 * typedef define
 * ---------------------------------------------------------------------------
 */
#ifdef FEATURE_MG_LONGPATH
typedef struct _MGFileInfo
{
   char        attrib;
   uint32      dwCreationDate;
   uint32      dwSize;
   char        szName[MG_MAX_FILE_NAME];
} MGFileInfo;
#else
typedef FileInfo MGFileInfo;
#endif


typedef enum GalleryType{
   GALLERY_TYPE_ANY,
   GALLERY_PHOTO_BROWSE,
   GALLERY_VIDEO_BROWSE,
   GALLERY_MUSIC_ADDLIST,
   GALLERY_VIDEO_ADDLIST,
   GALLERY_RECORD_VIEWLIST,
   GALLERY_IMAGE_SETTING,
   GALLERY_MUSIC_SETTING,
   GALLERY_FILE_SELECT,
   GALLERY_TYPE_MAX
}GalleryType;
#define GALLERY_IMAGE_VIEWLIST   GALLERY_PHOTO_BROWSE

typedef enum SettingsType{
   SETTING_WALLPAPER,
   SETTING_RINGTONE,
   SETTING_MAX
}SettingsType;

/*database table field for file information*/
typedef enum _FileDF
{
   FDF_BASE = 0,
   FDF_NAME = FDF_BASE, //AEEDB_FT_STRING
   FDF_ATTRIB, //AEEDB_FT_BYTE
   FDF_SIZE, //AEEDB_FT_DWORD
   FDF_CREATIONDATE, //AEEDB_FT_DWORD
   FDF_MAX,
}FileDF;

/*File mime classfication*/
#define MGMIME_BASE_IMAGE       0x0001
#define MGMIME_BASE_MUSIC       0x0002
#define MGMIME_BASE_VIDEO       0x0004
#define MGMIME_BASE_ALL         0xFFFF
typedef struct _ExplorerPara{
   /*下面几项为MMS选择文件使用*/
   AEECLSID    nClsId; /*the applet which invoke the interface class ID */
   const char* cpszPath;/*the can set to NULL by default*/
   int         nFileType; /*Please refer to File mime classfication up*/
   int         nFileMaxSize;/*the maximum size of file which listed*/

   /*下面项为添加音频文件时，所使用的*/
   uint16      nSelNum;/*maximum file number to select*/

   /*下面为情景模式调用设置铃声时使用，因为当前模式不一定是情景模式中编辑设置
    * 项的模式*/
   byte        nSelectProfile;
}ExplorerPara;
#ifndef ARRAYSIZE
#define ARRAYSIZE(a)            (sizeof(a)/sizeof(a[0]))
#endif

#define MG_IN(x, min, max)      (((x) > (min)) && ((x) < (max)))
#define MG_BETWEEN(x, min, max) (((x) >= (min)) && ((x) <= (max)))

#define MG_FILEATTR_MASK		(0x0F)
#define MG_FILE_PROTECT(attrib) ((attrib & AEE_FA_FIXED) == AEE_FA_FIXED || \
                                 (attrib & AEE_FA_CONST) == AEE_FA_CONST)
#ifndef ABS
   #define ABS(VAL) (((VAL)>0)?(VAL):(-(VAL)))
#endif

// Release helper
#ifndef RELEASEIF
#define RELEASEIF(p)             RELEASEPPIF((IBase**)(void*)&p)
static __inline void RELEASEPPIF(IBase **p)
{ if (*p) { IBASE_Release(*p); *p = 0; } }
#endif


typedef char (*FileNamesBuf)[MG_MAX_FILE_NAME];
/* ---------------------------------------------------------------------------
 * implement note: pBuf is provided by component, client need copy it and shoud
 * NOT free it.  For MediaGallery will free it. nNum is also provided by
 * component. pData shoud be given by client through
 * CMediaGallery_RegisterCallback.  such as following code snippet
 * ---------------------------------------------------------------------------
 */
typedef int (*PFNMGSELECTEDCB)(void* pData, FileNamesBuf pBuf, uint32 nNum);

/*
 * ==========================================================================
 * FUNCTION     :  CMediaGallery_RegisterCallback
 * DESCRIPTION  :
 * PARAMETERS   :pData is the first parameter of pfn
 * RETURN VALUE :
 * ==========================================================================
 */
int32 CMediaGallery_RegisterCallback(PFNMGSELECTEDCB pfn, void* pData);


/*
 * ==========================================================================
 * FUNCTION     :  CMediaGallery_GetTflashStatus
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
boolean CMediaGallery_GetTflashStatus(void);


/*
 * ==========================================================================
 * FUNCTION     :  CMediaGallery_GetSaveDataFloder
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
const char * CMediaGallery_GetSaveDataFloder(uint8 nStoreMedium,
                                             uint16 nFileType);


/*
 * ==========================================================================
 * FUNCTION     :  boolean CMediaGallery_GetReturnData
 * DESCRIPTION  :  when MMS call Media gallery, we return the select file
 * path, but MMS do not know the data struct, so we do for them . In fact , it
 * just do string copy.
 * PARAMETERS   : bytes of memory that pDest pointer
 * RETURN VALUE :
 * ==========================================================================
 */
 boolean CMediaGallery_GetReturnData(const void *cpSrc,
                                     void *pDest,
                                     uint16 nDestLen);

/*
 * ==========================================================================
 * FUNCTION     :  CMediaGallery_FileExplorer
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
int32 CMediaGallery_FileExplorer(GalleryType eGType, ExplorerPara *pPara);


/*
 * ==========================================================================
 * FUNCTION     :  CMediaGallery_ClearMediaFiles
 * DESCRIPTION  :  clear files that not pre-place in variou folder of Media
 * gallery
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
int CMediaGallery_ClearMediaFiles(void *po);

/*
 * ==========================================================================
 * FUNCTION     :  CMediaGallery_SetAs
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
int32 CMediaGallery_SetAs(SettingsType eSType,
                           const char* cpszFileName);
//int32 CMediaGallery_InitVideoPlayList(IFileMgr *pFileMgr);


/*
 * ==========================================================================
 * FUNCTION     :  CMediaGallery_OpenFileInfoDatabase
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
int32 CMediaGallery_OpenFileInfoDatabase( IShell *pShell,
                                          const char *cpszDBFile,
                                          boolean bCreate,
                                          IDatabase **ppFileDB);
/*
 * ==========================================================================
 * FUNCTION     :  CMediaGallery_CreateFileInfoRecord
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
int32 CMediaGallery_CreateFileInfoRecord(IDatabase *cpFilesDB,
                                          MGFileInfo *cpInfo);

/*
 * ==========================================================================
 * FUNCTION     :  CMediaGallery_GetFileInfoRecord
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
int32 CMediaGallery_GetFileInfoRecord(IDatabase *cpFilesDB,
                                       uint16 wRecID,
                                       MGFileInfo *pInfo);


/*
 * ==========================================================================
 * FUNCTION     :  MGAppUtil_ShrinkString
 * DESCRIPTION  :  把字符串缩短,以不超过剪切矩形宽度(由 nrcdx 指定),其中不显示
 * 的部分以"..."代替。
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
boolean MGAppUtil_ShrinkString(IDisplay *pDisp,
							   int16 nrcdx,
							   AECHAR *pwszStr,
							   AECHAR **ppwszShrStr);

/* ---------------------------------------------------------------------------
 * 一些封装全局变量的函数
 * ---------------------------------------------------------------------------
 */

/*
 * ==========================================================================
 * FUNCTION     :  MediaGalleryApp_SetUSBCableConnect
 * DESCRIPTION  :  设置USB线是否connect
 * PARAMETERS   :  bConnect -- TRUE  USB cable attach
 *                             FALSE USB cable deattach
 * RETURN VALUE :
 * ==========================================================================
 */
void MediaGallery_SetUSBCableConnect(boolean bConnect);

/*
 * ==========================================================================
 * FUNCTION     :  MediaGallery_CheckUSBCableConnect
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE : TRUE -- USB cable attach
 *                FALSE -- USB cable deattach
 * ==========================================================================
 */
boolean MediaGallery_CheckUSBCableConnect(void);


/*
 * ==========================================================================
 * FUNCTION     :  MediaGallery_CheckUdiskStat
 * DESCRIPTION  :  check U-Disk status
 * PARAMETERS   :
 * RETURN VALUE : TRUE-- U-Disk in running
 *                FALSE -- U-Disk is stopped.
 * ==========================================================================
 */
boolean MediaGallery_CheckUdiskStat(void);

/*
 * ==========================================================================
 * FUNCTION     :  MediaGallery_SetPlayMusicStatus
 * DESCRIPTION  :  设置音乐播放的状态。
 * PARAMETERS   :  bPlay：FALSE -- 停止播放。
 *                        TRUE  -- 出于播放。
 * RETURN VALUE :
 * ==========================================================================
 */
void MediaGallery_SetPlayMusicStatus(boolean bPlay);

/*
 * ==========================================================================
 * FUNCTION     :  MediaGallery_GetPlayMusicStatus
 * DESCRIPTION  :  获取当前是否在播放音乐。
 * PARAMETERS   :
 * RETURN VALUE :  TRUE--正在播放。
 *                 FALSE -- 不在播放。
 * ==========================================================================
 */
boolean MediaGallery_GetPlayMusicStatus(void);


/* ---------------------------------------------------------------------------
 * Media gallery interface
 * ---------------------------------------------------------------------------
 */
typedef struct _IMediaGallery IMediaGallery;

QINTERFACE(IMediaGallery)
{
   INHERIT_IApplet(IMediaGallery);
};

#define  IMediaGallery_AddRef(p)    GET_PVTBL(p,IMediaGallery)->AddRef(p)
#define  IMediaGallery_Release(p)   GET_PVTBL(p,IMediaGallery)->Release(p)
#define  IMediaGallery_HandleEvent(p,ec,wp,dw) \
                     GET_PVTBL(p,IMediaGallery)->HandleEvent(p,ec,wp,dw)
#endif   //MEDIAGALLERY_H
