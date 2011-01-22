/*=============================================================================
  FILE: MPConfigFile.c

  SERVICES:class obbject Implementations

  GENERAL DESCRIPTION:

  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright @2007 QUALCOMM Incorporated. All Rights Reserved.
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPConfigFile.c#10 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/

#include "MPConfigFile.h"
#include "MPConfig.h"
#include "MPSettings.h"

// there are two configuration files, 
//   MP_CONFIG_FILE_NAME is a text file for saving MP data
//   MP_CONFIG_DATA_FILE_NAME is used for serializing the objects.

//config file name
#define  MP_CONFIG_FILE_NAME                "mediaplayer.ini"
#define  MP_CONFIG_FILE_VERSION             "FileVersion"
#define  MP_CONFIG_FILE_VERSION_NUM         "1.0.00"

//binary file name for serializing object data
#define  MP_CONFIG_DATA_FILE_NAME           "mediaplayer.dat"
#define  MP_CONFIG_DATA_FILE_VERSION_NUM    "1.0.00"
#define  MP_DATA_ITEM_EQ                    "EqualizerData"
#define  MP_DATA_ITEM_AUDIO_EXPANDER        "AudioExpanderData"

// section name
#define  MP_SEC_DEFAULT                     "Default"
#define  MP_BINARY                          "Binary"
#define  MP_AUTOPLAY                        "AutoPlay"
#define  MP_CONTMODE                        "ContMode"
#define  MP_VOLUMELEVEL                     "VolumeLevel"
#define  MP_DISPCONTRAST                    "DisplayContrast"
#define  MP_PS_BW                           "PSBandWidth"
#define  MP_QOS_ENABLE                      "QosEnable"
#define  MP_DEFAULTPROFILE                  "DefaultProfile"
#define  MP_PROFILE                         "CurrentProfile"
#define  MP_RTSP_STREAM_REC                 "RTSPStreamRecord"
#define MP_KDDI_HTTP_EXT                    "KDDIHTTPExtension"
#define MP_REC_AUDIO_FORMAT                 "RecordAudioFormat"
#define MP_REC_SAMPLE_RATE                  "RecordSampleRate"
#define MP_REC_AUDIO_CHANNEL                "RecordAudioChannel"
#define MP_REC_AUDIO_PATH                   "RecordAudioPath"
#define MP_REC_SAVE_LOCATION                "RecordSaveLocation"
#define MP_REPLAY_SEEK_TIME                 "ReplaySeekTime"
#define MP_ICONTENT_MODE                    "IContentMode"
#define MP_PLAY_FROM_BUFFER                 "PlayFromBuffer"
#define MP_PLAY_AUDIO_IN_BK                 "PlayAudioInBackground"
#define MP_REDRAW_AT_CLIP_END               "RedrawAtClipEnd"
#define MP_STREAMING_REC_SAVE_TO_FLASH      "SaveStreamingRecordFileToFlash"
#define MP_HTTP_DOWNLOAD_INTERFACE          "HTTPDownloadInterface"
#define MP_AUDIO_PAN_VALUE                  "AudioPanValue"
#define MP_AUDIO_SHOW_SPEC                  "AudioShowSPEC"
#define MP_QTV_PROG_DNLD                    "ProgressDownloadByQTV"
#define MP_MIMIC_ENCRYPT_DNLD               "MimicEncryptDownload"
#define MP_RINGER_PLAY_WITH_QTV             "PlayRingerWithQTV"
#define MP_RINGER_PLAY_WITH_NO_QTV          "PlayRingerWithNoQTV"
#define MP_PROG_DNLD_FILE_SAVE_FLASH        "ProgressDownloadFileSaveFlash"
#define MP_ASCALE_VALUE_X                   "AscaleValueX"      
#define MP_ASCALE_VALUE_Y                   "AscaleValueY"
#define MP_OVERLAYS                         "Overlays"
#define MP_FRAME_CALLBACK                   "FrameCallback"
#define MP_ALPHA_BLEND                      "AlphaBlend"
#define MP_HIGH_PERFORMANCE_MODE            "HighPerformanceMode"
#define MP_ENABLE_SPECTRUM_ANALYZER         "EnableSpectrumAnalyzer"
#define MP_ENABLE_EQUALIZER                 "EnableEqualizer"
#define MP_ENABLE_AUDIO_EXPANDER            "EnableAudioExpander"
#define MP_SHOW_AUDIO_EXPANDER              "ShowAudioExpander"
#define MP_SHOW_SPECTRUM_ANALYZER           "ShowSpectrumAnalyzer"
#define MP_FILTER_ID                        "FilterID"
#define MP_SLIDER_ID                        "SliderID"
#define MP_EQ_DATA                          "EQData"
#define MP_AUDIO_EXPANDER_DATA              "AudioExpanderData"
#define MP_ENABLE_ANGLED_MIXING             "EnableAngledMixing"
#define MP_USE_ANGLED_MIXING_DATA_0         "UseAngledMixingData_0"
#define MP_USE_ANGLED_MIXING_DATA_1         "UseAngledMixingData_1"
#define MP_ANGLED_MIXING_DATA_0             "AngledMixingData_0"
#define MP_ANGLED_MIXING_DATA_1             "AngledMixingData_1"
#define MP_ANGLED_MIXING_LAST_UPDATE_ID     "AngledMixingLastUpdateID"
#define MP_QTV_SUSPEND_MODE                 "QtvSuspendMode"
#define MP_KEYMAP_0                         "KeyMapping_0"
#define MP_KEYMAP_1                         "KeyMapping_1"
#define MP_KEYMAP_2                         "KeyMapping_2"
#define MP_KEYMAP_3                         "KeyMapping_3"
#define MP_KEYMAP_4                         "KeyMapping_4"
#define MP_KEYMAP_5                         "KeyMapping_5"
#define MP_KEYMAP_6                         "KeyMapping_6"
#define MP_KEYMAP_7                         "KeyMapping_7"
#define MP_KEYMAP_8                         "KeyMapping_8"
#define MP_KEYMAP_9                         "KeyMapping_9"
#define MP_KEYMAP_STAR                      "KeyMapping_Star"
#define MP_KEYMAP_POUND                     "KeyMapping_Pound"
#define MP_LOCAL_PLAY_PULL_MODE             "LocalPlayPullMode" 

typedef struct _ItemDataSize
{
  uint16   m_nVerStringSize;
  uint16   m_nItemNameSize;
  uint32   m_nItemDataSize;
}ItemDataSize;

extern int HTTPDownloadInterface;

extern CKeyMapping MPPW_KeyMappings[];

#if defined(MP_FEATURE_VIDEO) && defined(FEATURE_QTV_STREAM_RECORD)
extern boolean bRTSPstreamRecord;
#endif

#if(defined( FEATURE_MP_EQ_UI) && defined( FEATURE_MP_EQ))
static void MPSaveAudioConfigData(CMediaPlayer* pme, CMPConfig* pcfg);
static void MPReadAudioConfigData(CMediaPlayer* pme, CMPConfig* pcfg);
#endif //#if(defined( FEATURE_MP_EQ_UI) && defined( FEATURE_MP_EQ))

// Currently, the following functions are only referenced by MPSaveAudioConfigData and 
// MPReadAudioConfigData() Thus, when the audio features are not defined, these functions
// become unreferenced.  If non-audio features use these in the future, remove #if defined.
 
#if(defined( FEATURE_MP_EQ_UI) && defined( FEATURE_MP_EQ))
static int MPConfigFileWriteBinary(CMediaPlayer* pme, CMPConfig* pcfg, IFile *pIFile, const char* pSection, const char* pItem, void* pData, int size);
static void* MPConfigFileReadBinary(CMediaPlayer* pme, CMPConfig* pcfg, const char* pSection, const char* pItem);
// read data string from config file by given section and item name
static const char* MPConfigFileReadString(CMPConfig* pcfg, const char* pSection, const char* pItem);


// This function creates an entry in the .ini file with "[" + pItem + "]" = MP_CONFIG_DATA_FILE_NAME
// and saves pData into MP_CONFIG_DATA_FILE_NAME
static int MPConfigFileWriteBinary(CMediaPlayer* pme, CMPConfig* pcfg, IFile *pIFile, const char* pSection, const char* pItem, void* pData, int size)
{

  if(pIFile && pItem)
  {
    ItemDataSize itemSize;
    // save an entry in ini file
    (void)MPConfigWriteString(pcfg, pSection, pItem, MP_CONFIG_DATA_FILE_NAME);

    // now, start saving into .dat file
    itemSize.m_nVerStringSize = STRLEN(MP_CONFIG_DATA_FILE_VERSION_NUM);
    itemSize.m_nItemNameSize  = STRLEN(pItem);
    itemSize.m_nItemDataSize  = size;

    // save size information
    (void)IFILE_Write(pIFile, (void*)&itemSize, sizeof(itemSize));

    if(itemSize.m_nVerStringSize)
    {
      (void)IFILE_Write(pIFile, MP_CONFIG_DATA_FILE_VERSION_NUM, itemSize.m_nVerStringSize);
    }

    if(itemSize.m_nItemNameSize)
    {
      (void)IFILE_Write(pIFile, pItem, itemSize.m_nItemNameSize);
    }

    if(itemSize.m_nItemDataSize)
    {
      (void)IFILE_Write(pIFile, pData, itemSize.m_nItemDataSize);
    }
  }
  else
  {
    DBGPRINTF_ERROR("MP: pIFile or pItemSize is NULL.");
    return EFAILED;
  }

  return SUCCESS;
}

static const char* MPConfigFileReadString(CMPConfig* pcfg, const char* pSection, const char* pItem)
{
  return MPConfigReadString(pcfg, pSection, pItem);
}

// this function first reads the binary file name from the .ini file and then opens the file to read.
// the return is the binary object
static void* MPConfigFileReadBinary(CMediaPlayer* pme, CMPConfig* pcfg, const char* pSection, const char* pItem)
{
  IFileMgr     *pIFileMgr = NULL;
  IFile        *pIFile    = NULL;
  const char   *pDataFile = NULL; // .dat file name
  ItemDataSize  itemSize;         // item header 
  char          buf[128];         // string buffer
  void         *pData = NULL;
  int           nSize = 0;        // store data object
  int           nReadSize = 0;

  if(NULL == pcfg || NULL == pSection || NULL == pItem)
  {
    DBGPRINTF_ERROR("MP: Invelid parm.");
    return NULL;
  }
  
  pDataFile = MPConfigFileReadString(pcfg, pSection, pItem);
  if(NULL == pDataFile)
  {
    DBGPRINTF_ERROR("MP: error reading binary for section = %s, item = %s", pSection, pItem);
    return NULL;
  }
  // now, the pDataFile stores the .dat file name

  if(SUCCESS == ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr))
  {
    // create file for reading
    pIFile = IFILEMGR_OpenFile(pIFileMgr, pDataFile, _OFM_READ);
    if(NULL == pIFile)
    {
      DBGPRINTF_ERROR("MP: failed to open file = %s.", pDataFile);
      IFILEMGR_Release(pIFileMgr);
      return NULL;
    }
  }
  else
  {
    DBGPRINTF_ERROR("MP: failed to create FileMgr.");
    return NULL;
  }

  while(1)
  {
    MEMSET(&itemSize, 0, sizeof(itemSize));
    nReadSize = IFILE_Read(pIFile, &itemSize, sizeof(ItemDataSize)); 
    if (nReadSize == sizeof(ItemDataSize))
    {
      if(itemSize.m_nVerStringSize)
      {
        // read version string
        MEMSET(buf, 0, sizeof(buf));
        if(IFILE_Read(pIFile, buf, itemSize.m_nVerStringSize) == itemSize.m_nVerStringSize)
        {
          DBGPRINTF_HIGH("MP: config data file, version = %s.", buf);
        }
        else
        {
          DBGPRINTF_HIGH("MP: cannot read config data file version string.");
        }
      }
  
      if(itemSize.m_nItemNameSize)
      {
        // read item name string
        MEMSET(buf, 0, sizeof(buf));
        if(IFILE_Read(pIFile, buf, itemSize.m_nItemNameSize) == itemSize.m_nItemNameSize)
        {
          DBGPRINTF_HIGH("MP: config data file, item name = %s.", buf);
        }
        else
        {
          DBGPRINTF_HIGH("MP: cannot read item name from config data file.");
        }
      }
      
      if(itemSize.m_nItemDataSize)
      {
        // read item data
        if(nSize < itemSize.m_nItemDataSize)
        {
          // if itemSize.m_nItemDataSize is larger, reallocate memory
          if(pData)
          {
            FREE(pData);
          }
          pData = MALLOC(itemSize.m_nItemDataSize);
          if(pData)
          {
            nSize = itemSize.m_nItemDataSize;
          }
          else
          {
            DBGPRINTF_ERROR("MP: failed to allocate memory.");
            IFILE_Release(pIFile);
            IFILEMGR_Release(pIFileMgr);
            return NULL;
          }
        }
        
        if(IFILE_Read(pIFile, pData, itemSize.m_nItemDataSize) != itemSize.m_nItemDataSize)
        {
          DBGPRINTF_HIGH("MP: error reading config data.");
          FREE(pData);
          IFILE_Release(pIFile);
          IFILEMGR_Release(pIFileMgr);
          return NULL;
        }
      }

      if(STRCMP(buf, pItem) == 0)
      {
        // this is the data we need.
        break;
      }
    }
    else
    {
      // either we are at end of the file or not enough data to read.
      DBGPRINTF_HIGH("MP: config data file %s, read size, nReadSize = %d.", pDataFile, nReadSize);
      break;
    }
  }

  IFILE_Release(pIFile);
  IFILEMGR_Release(pIFileMgr);

  return pData;
}
#endif //#if(defined( FEATURE_MP_EQ_UI) && defined( FEATURE_MP_EQ))

int MPConfigFile_Read(CMediaPlayer *pme)
{
  CMPConfig* pcfg;
  const char* pChar;

  pcfg = MPConfigMake(pme->a.m_pIShell, MP_CONFIG_FILE_NAME);
  if(NULL == pcfg)
  {
    DBGPRINTF_ERROR("MP: MPConfigMake() failed.");
    return EFAILED;
  }

  // read configuration file version string
  pChar = MPConfigReadString(pcfg, MP_CONFIG_FILE_VERSION, MP_CONFIG_FILE_VERSION_NUM);
  if(pChar)
  {
    DBGPRINTF_HIGH("MP: configuration file version = %s", pChar);
  }
  else
  {
    DBGPRINTF_ERROR("MP: Error reading configuration file version.");
  }


  pme->m_bAutoPlay = MPConfigReadBoolean(pcfg, MP_SEC_DEFAULT, MP_AUTOPLAY, FALSE);
  pme->m_nContMode = MPConfigReadInt(pcfg, MP_SEC_DEFAULT, MP_CONTMODE, CONT_MODE_OFF);
  pme->m_wVolumeLevel = MPConfigReadInt(pcfg, MP_SEC_DEFAULT, MP_VOLUMELEVEL, VOLUME_TWO);

#ifdef FEATURE_MP4_LCD_CONTRAST
  {
    disp_info_type di = disp_get_info();
    pme->m_dispContrast = MPConfigReadInt(pcfg, MP_SEC_DEFAULT, MP_DISPCONTRAST, di.contrast_default);
  }
#endif

#ifdef FEATURE_APP_MPEG4
#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  pme->m_bPSbwLevel = MPConfigReadInt(pcfg, MP_SEC_DEFAULT, MP_PS_BW, PS_BW_LVL2);
  pme->m_bKddiExt = MPConfigReadBoolean(pcfg, MP_SEC_DEFAULT, MP_KDDI_HTTP_EXT, FALSE);
#endif //#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
#endif //FEATURE_APP_MPEG4


#if defined(MP_FEATURE_VIDEO) && \
 (defined(FEATURE_QTV_QOS_SELECTION)||defined(FEATURE_QTV_PDP_SELECTION))
  pme->m_NetPolicy.m_bQoSEnable = MPConfigReadBoolean(pcfg, MP_SEC_DEFAULT, MP_QOS_ENABLE, FALSE);
  pme->m_NetPolicy.m_bDefaultProfile = MPConfigReadBoolean(pcfg, MP_SEC_DEFAULT, MP_DEFAULTPROFILE, TRUE);
  pme->m_NetPolicy.m_nProfile = MPConfigReadInt(pcfg, MP_SEC_DEFAULT, MP_PROFILE, 1);
#endif

#if defined(MP_FEATURE_VIDEO) && defined(FEATURE_QTV_STREAM_RECORD)
  pme->m_bRTSPstreamRec = MPConfigReadBoolean(pcfg, MP_SEC_DEFAULT, MP_RTSP_STREAM_REC, FALSE);
  bRTSPstreamRecord = pme->m_bRTSPstreamRec;
#endif


#ifdef FEATURE_MP_RECORDER_SETTINGS
  pme->m_AudioFormat = MPConfigReadInt(pcfg, MP_SEC_DEFAULT, MP_REC_AUDIO_FORMAT, MM_QCP_FORMAT_FIXED_FULL_13K);
  pme->m_SampleRate = MPConfigReadInt(pcfg, MP_SEC_DEFAULT, MP_REC_SAMPLE_RATE, CMX_AF_SAMPLE_RATE_8000);
  pme->m_AudioChannel = MPConfigReadInt(pcfg, MP_SEC_DEFAULT, MP_REC_AUDIO_CHANNEL, CMX_AF_AAC_CHANNEL_MONO);
  pme->m_RecordPath = MPConfigReadInt(pcfg, MP_SEC_DEFAULT, MP_REC_AUDIO_PATH, MM_APATH_LOCAL);
  pme->m_SaveLocation = MPConfigReadInt(pcfg, MP_SEC_DEFAULT, MP_REC_SAVE_LOCATION, SAVELOC_FLASH);
#endif

  pme->m_nSeekTime = MPConfigReadInt(pcfg, MP_SEC_DEFAULT, MP_REPLAY_SEEK_TIME, MP_SEEK_TIME);

#if (defined(MP_FEATURE_ICONTENT) || defined(MP_FEATURE_ICONTENTHANDLER))
  pme->m_bIContentMode = MPConfigReadBoolean(pcfg, MP_SEC_DEFAULT, MP_ICONTENT_MODE, FALSE);
#endif //#if (defined(MP_FEATURE_ICONTENT) || defined(MP_FEATURE_ICONTENTHANDLER))

  pme->m_bPlayFromBuffer = MPConfigReadBoolean(pcfg, MP_SEC_DEFAULT, MP_PLAY_FROM_BUFFER, FALSE);
  pme->m_bPlayAudioInBackground = MPConfigReadBoolean(pcfg, MP_SEC_DEFAULT, MP_PLAY_AUDIO_IN_BK, FALSE);
  pme->m_bRedrawScreenAtClipEnd = MPConfigReadBoolean(pcfg, MP_SEC_DEFAULT, MP_REDRAW_AT_CLIP_END, FALSE);

#if defined(FEATURE_APP_MPEG4) && (defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))
  pme->m_bPvxSavetoFlash = MPConfigReadBoolean(pcfg, MP_SEC_DEFAULT, MP_STREAMING_REC_SAVE_TO_FLASH, FALSE);
#endif

#if defined(FEATURE_APP_MPEG4) && (defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))
  HTTPDownloadInterface = MPConfigReadInt(pcfg, MP_SEC_DEFAULT, MP_HTTP_DOWNLOAD_INTERFACE, MMD_HTTP_PUSH_BUFFER_PLAYBACK);
#endif

  pme->m_nAudioPanValue = MPConfigReadInt(pcfg, MP_SEC_DEFAULT, MP_AUDIO_PAN_VALUE, (MM_MAX_PAN / 2));
  pme->m_bShowAudioSpec = MPConfigReadBoolean(pcfg, MP_SEC_DEFAULT, MP_AUDIO_SHOW_SPEC, FALSE);

#if defined(FEATURE_APP_MPEG4) && (defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || defined(FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2))
  pme->m_bCallQtvProgDnld = MPConfigReadBoolean(pcfg, MP_SEC_DEFAULT, MP_QTV_PROG_DNLD, TRUE);
  pme->m_MimicEncryptDnld = MPConfigReadInt(pcfg, MP_SEC_DEFAULT, MP_MIMIC_ENCRYPT_DNLD, FALSE);
#endif
  
#if defined (FEATURE_APP_MPEG4) && defined (FEATURE_QTV_AUDIO_RINGER)
  pme->m_bPlayRingerWithQTV =MPConfigReadBoolean(pcfg, MP_SEC_DEFAULT, MP_RINGER_PLAY_WITH_QTV, FALSE);
  pme->m_bPlayRingerNoQTV = MPConfigReadBoolean(pcfg, MP_SEC_DEFAULT, MP_RINGER_PLAY_WITH_NO_QTV, FALSE);
#endif

#if defined(FEATURE_APP_MPEG4) && defined(FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2)
  pme->m_bProgDLFileSaveLoc = MPConfigReadBoolean(pcfg, MP_SEC_DEFAULT, MP_PROG_DNLD_FILE_SAVE_FLASH, TRUE);
#endif

#if defined(FEATURE_QTV_MDP_ASCALE)
  pme->m_nAScaleXValue = MPConfigReadInt(pcfg, MP_SEC_DEFAULT, MP_ASCALE_VALUE_X, MP_ASCALE_DEFAULT_VALUE);
  pme->m_nAScaleYValue = MPConfigReadInt(pcfg, MP_SEC_DEFAULT, MP_ASCALE_VALUE_Y, MP_ASCALE_DEFAULT_VALUE);
#endif

#if defined(FEATURE_MMOVERLAY)
  pme->m_bOverlays = MPConfigReadBoolean(pcfg, MP_SEC_DEFAULT, MP_OVERLAYS, FALSE);
  pme->m_bFrameCallbacks = MPConfigReadBoolean(pcfg, MP_SEC_DEFAULT, MP_FRAME_CALLBACK, FALSE);
  pme->m_nAlphaBlend = MPConfigReadInt(pcfg, MP_SEC_DEFAULT, MP_ALPHA_BLEND, MP_ALPHA_BLEND_DEFAULT);
#endif

  pme->m_bHighPerformanceMode = MPConfigReadBoolean(pcfg, MP_SEC_DEFAULT, MP_HIGH_PERFORMANCE_MODE, FALSE);
  pme->m_bLocalPlayPullMode = MPConfigReadBoolean(pcfg, MP_SEC_DEFAULT, MP_LOCAL_PLAY_PULL_MODE, FALSE);

#ifdef FEATURE_APP_MPEG4
  pme->m_bQtvSuspendPause = MPConfigReadBoolean(pcfg, MP_SEC_DEFAULT, MP_QTV_SUSPEND_MODE, TRUE);
#endif

  // Read all the key mappings
  MPPW_KeyMappings[0] = (CKeyMapping)MPConfigReadInt(pcfg, MP_SEC_DEFAULT, MP_KEYMAP_0, MPPW_KeyMappings[0]);
  MPPW_KeyMappings[1] = (CKeyMapping)MPConfigReadInt(pcfg, MP_SEC_DEFAULT, MP_KEYMAP_1, MPPW_KeyMappings[1]);
  MPPW_KeyMappings[2] = (CKeyMapping)MPConfigReadInt(pcfg, MP_SEC_DEFAULT, MP_KEYMAP_2, MPPW_KeyMappings[2]);
  MPPW_KeyMappings[3] = (CKeyMapping)MPConfigReadInt(pcfg, MP_SEC_DEFAULT, MP_KEYMAP_3, MPPW_KeyMappings[3]);
  MPPW_KeyMappings[4] = (CKeyMapping)MPConfigReadInt(pcfg, MP_SEC_DEFAULT, MP_KEYMAP_4, MPPW_KeyMappings[4]);
  MPPW_KeyMappings[5] = (CKeyMapping)MPConfigReadInt(pcfg, MP_SEC_DEFAULT, MP_KEYMAP_5, MPPW_KeyMappings[5]);
  MPPW_KeyMappings[6] = (CKeyMapping)MPConfigReadInt(pcfg, MP_SEC_DEFAULT, MP_KEYMAP_6, MPPW_KeyMappings[6]);
  MPPW_KeyMappings[7] = (CKeyMapping)MPConfigReadInt(pcfg, MP_SEC_DEFAULT, MP_KEYMAP_7, MPPW_KeyMappings[7]);
  MPPW_KeyMappings[8] = (CKeyMapping)MPConfigReadInt(pcfg, MP_SEC_DEFAULT, MP_KEYMAP_8, MPPW_KeyMappings[8]);
  MPPW_KeyMappings[9] = (CKeyMapping)MPConfigReadInt(pcfg, MP_SEC_DEFAULT, MP_KEYMAP_9, MPPW_KeyMappings[9]);
  MPPW_KeyMappings[10] = (CKeyMapping)MPConfigReadInt(pcfg, MP_SEC_DEFAULT, MP_KEYMAP_STAR, MPPW_KeyMappings[10]); 
  MPPW_KeyMappings[11] = (CKeyMapping)MPConfigReadInt(pcfg, MP_SEC_DEFAULT, MP_KEYMAP_POUND, MPPW_KeyMappings[11]);

#if(defined( FEATURE_MP_EQ_UI) && defined( FEATURE_MP_EQ))
  MPReadAudioConfigData(pme, pcfg);
#endif //#if(defined( FEATURE_MP_EQ_UI) && defined( FEATURE_MP_EQ))

  MPCONFIG_FREEIF(pcfg);

  return SUCCESS;
}
int MPConfigFile_Write(CMediaPlayer *pme)
{
  CMPConfig* pcfg;
  IFileMgr *pIFileMgr = NULL;

  (void)ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr);
  if (pIFileMgr)
  {
    if (IFILEMGR_Test(pIFileMgr, MP_CONFIG_FILE_NAME) == SUCCESS)
		{
			if(SUCCESS != IFILEMGR_Remove(pIFileMgr, MP_CONFIG_FILE_NAME))
			{
				DBGPRINTF_ERROR("MP: failed to delete %s.", MP_CONFIG_FILE_NAME);
			}
		}

    if (IFILEMGR_Test(pIFileMgr, MP_CONFIG_DATA_FILE_NAME) == SUCCESS)
		{
			if(SUCCESS != IFILEMGR_Remove(pIFileMgr, MP_CONFIG_DATA_FILE_NAME))
			{
				DBGPRINTF_ERROR("MP: failed to delete %s.", MP_CONFIG_DATA_FILE_NAME);
			}
		}

	  IFILEMGR_Release(pIFileMgr);
  }

  pcfg = MPConfigMake(pme->a.m_pIShell, MP_CONFIG_FILE_NAME);
  if(NULL == pcfg)
  {
    DBGPRINTF_ERROR("MP: MPConfigMake() failed.");
    return EFAILED;
  }

  // write configuration file version string
  (void)MPConfigWriteString(pcfg, MP_CONFIG_FILE_VERSION, MP_CONFIG_FILE_VERSION, MP_CONFIG_FILE_VERSION_NUM);

  (void)MPConfigWriteBoolean(pcfg, MP_SEC_DEFAULT, MP_AUTOPLAY, pme->m_bAutoPlay);
  (void)MPConfigWriteInt(pcfg, MP_SEC_DEFAULT, MP_CONTMODE, pme->m_nContMode);
  (void)MPConfigWriteInt(pcfg, MP_SEC_DEFAULT, MP_VOLUMELEVEL, (int32)pme->m_wVolumeLevel);

#ifdef FEATURE_MP4_LCD_CONTRAST
  (void)MPConfigWriteInt(pcfg, MP_SEC_DEFAULT, MP_DISPCONTRAST, (int32)pme->m_dispContrast);
#endif

#ifdef FEATURE_APP_MPEG4
#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  (void)MPConfigWriteInt(pcfg, MP_SEC_DEFAULT, MP_PS_BW, (int32)pme->m_bPSbwLevel);
#endif //#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
#endif //FEATURE_APP_MPEG4


#if defined(MP_FEATURE_VIDEO) && \
 (defined(FEATURE_QTV_QOS_SELECTION)||defined(FEATURE_QTV_PDP_SELECTION))
  (void)MPConfigWriteBoolean(pcfg, MP_SEC_DEFAULT, MP_QOS_ENABLE, (int32)pme->m_NetPolicy.m_bQoSEnable);
  (void)MPConfigWriteBoolean(pcfg, MP_SEC_DEFAULT, MP_DEFAULTPROFILE, (int32)pme->m_NetPolicy.m_bDefaultProfile);
  (void)MPConfigWriteInt(pcfg, MP_SEC_DEFAULT, MP_PROFILE, (int32)pme->m_NetPolicy.m_nProfile);
#endif

#if defined(MP_FEATURE_VIDEO) && defined(FEATURE_QTV_STREAM_RECORD)
  (void)MPConfigWriteBoolean(pcfg, MP_SEC_DEFAULT, MP_RTSP_STREAM_REC, (int32)pme->m_bRTSPstreamRec);
#endif

#ifdef FEATURE_APP_MPEG4
#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  (void)MPConfigWriteBoolean(pcfg, MP_SEC_DEFAULT, MP_KDDI_HTTP_EXT, (int32)pme->m_bKddiExt);
#endif
#endif

#ifdef FEATURE_MP_RECORDER_SETTINGS
  (void)MPConfigWriteInt(pcfg, MP_SEC_DEFAULT, MP_REC_AUDIO_FORMAT, (int32)pme->m_AudioFormat);
  (void)MPConfigWriteInt(pcfg, MP_SEC_DEFAULT, MP_REC_SAMPLE_RATE, (int32)pme->m_SampleRate);
  (void)MPConfigWriteInt(pcfg, MP_SEC_DEFAULT, MP_REC_AUDIO_CHANNEL, (int32)pme->m_AudioChannel);
  (void)MPConfigWriteInt(pcfg, MP_SEC_DEFAULT, MP_REC_AUDIO_PATH, (int32)pme->m_RecordPath);
  (void)MPConfigWriteInt(pcfg, MP_SEC_DEFAULT, MP_REC_SAVE_LOCATION, (int32)pme->m_SaveLocation);
#endif
  (void)MPConfigWriteInt(pcfg, MP_SEC_DEFAULT, MP_REPLAY_SEEK_TIME, (int32)pme->m_nSeekTime);

#if (defined(MP_FEATURE_ICONTENT) || defined(MP_FEATURE_ICONTENTHANDLER))
  (void)MPConfigWriteBoolean(pcfg, MP_SEC_DEFAULT, MP_ICONTENT_MODE, (int32)pme->m_bIContentMode);
#endif //#if (defined(MP_FEATURE_ICONTENT) || defined(MP_FEATURE_ICONTENTHANDLER))

  (void)MPConfigWriteBoolean(pcfg, MP_SEC_DEFAULT, MP_PLAY_FROM_BUFFER, (int32)pme->m_bPlayFromBuffer);
  (void)MPConfigWriteBoolean(pcfg, MP_SEC_DEFAULT, MP_PLAY_AUDIO_IN_BK, (int32)pme->m_bPlayAudioInBackground);
  (void)MPConfigWriteBoolean(pcfg, MP_SEC_DEFAULT, MP_REDRAW_AT_CLIP_END, (int32)pme->m_bRedrawScreenAtClipEnd);

#if defined(FEATURE_APP_MPEG4) && (defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))
  (void)MPConfigWriteBoolean(pcfg, MP_SEC_DEFAULT, MP_STREAMING_REC_SAVE_TO_FLASH, (int32)pme->m_bPvxSavetoFlash);
#endif

#if defined(FEATURE_APP_MPEG4) && (defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))
  (void)MPConfigWriteInt(pcfg, MP_SEC_DEFAULT, MP_HTTP_DOWNLOAD_INTERFACE, (int32)HTTPDownloadInterface);
#endif

  (void)MPConfigWriteInt(pcfg, MP_SEC_DEFAULT, MP_AUDIO_PAN_VALUE, (int32)pme->m_nAudioPanValue);
  (void)MPConfigWriteBoolean(pcfg, MP_SEC_DEFAULT, MP_AUDIO_SHOW_SPEC, (int32)pme->m_bShowAudioSpec);

#if defined(FEATURE_APP_MPEG4) && (defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || defined(FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2))
  (void)MPConfigWriteBoolean(pcfg, MP_SEC_DEFAULT, MP_QTV_PROG_DNLD, (int32)pme->m_bCallQtvProgDnld);
  (void)MPConfigWriteInt(pcfg, MP_SEC_DEFAULT, MP_MIMIC_ENCRYPT_DNLD, (int32)pme->m_MimicEncryptDnld);
#endif

#if defined (FEATURE_APP_MPEG4) && defined (FEATURE_QTV_AUDIO_RINGER)
  (void)MPConfigWriteBoolean(pcfg, MP_SEC_DEFAULT, MP_RINGER_PLAY_WITH_QTV, (int32)pme->m_bPlayRingerWithQTV);
  (void)MPConfigWriteBoolean(pcfg, MP_SEC_DEFAULT, MP_RINGER_PLAY_WITH_NO_QTV, (int32)pme->m_bPlayRingerNoQTV);
#endif

#if defined(FEATURE_APP_MPEG4) && defined(FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2)
  (void)MPConfigWriteBoolean(pcfg, MP_SEC_DEFAULT, MP_PROG_DNLD_FILE_SAVE_FLASH, (int32)pme->m_bProgDLFileSaveLoc);
#endif

#if defined(FEATURE_QTV_MDP_ASCALE)
  (void)MPConfigWriteInt(pcfg, MP_SEC_DEFAULT, MP_ASCALE_VALUE_X, (int32)pme->m_nAScaleXValue);
  (void)MPConfigWriteInt(pcfg, MP_SEC_DEFAULT, MP_ASCALE_VALUE_Y, (int32)pme->m_nAScaleYValue);
#endif

#if defined(FEATURE_MMOVERLAY)
 (void)MPConfigWriteBoolean(pcfg, MP_SEC_DEFAULT, MP_OVERLAYS, (int32)pme->m_bOverlays);
 (void)MPConfigWriteBoolean(pcfg, MP_SEC_DEFAULT, MP_FRAME_CALLBACK, (int32)pme->m_bFrameCallbacks);
 (void)MPConfigWriteInt(pcfg, MP_SEC_DEFAULT, MP_ALPHA_BLEND, (int32)pme->m_nAlphaBlend);
#endif

 (void)MPConfigWriteBoolean(pcfg, MP_SEC_DEFAULT, MP_HIGH_PERFORMANCE_MODE, (int32)pme->m_bHighPerformanceMode);
 (void)MPConfigWriteBoolean(pcfg, MP_SEC_DEFAULT, MP_LOCAL_PLAY_PULL_MODE, (int32)pme->m_bLocalPlayPullMode);

#ifdef FEATURE_APP_MPEG4
  (void)MPConfigWriteBoolean(pcfg, MP_SEC_DEFAULT, MP_QTV_SUSPEND_MODE, (int32)pme->m_bQtvSuspendPause);
#endif

  // Read all the key mappings
  (void)MPConfigWriteInt(pcfg, MP_SEC_DEFAULT, MP_KEYMAP_0, (int32)MPPW_KeyMappings[0]);
  (void)MPConfigWriteInt(pcfg, MP_SEC_DEFAULT, MP_KEYMAP_1, (int32)MPPW_KeyMappings[1]);
  (void)MPConfigWriteInt(pcfg, MP_SEC_DEFAULT, MP_KEYMAP_2, (int32)MPPW_KeyMappings[2]);
  (void)MPConfigWriteInt(pcfg, MP_SEC_DEFAULT, MP_KEYMAP_3, (int32)MPPW_KeyMappings[3]);
  (void)MPConfigWriteInt(pcfg, MP_SEC_DEFAULT, MP_KEYMAP_4, (int32)MPPW_KeyMappings[4]);
  (void)MPConfigWriteInt(pcfg, MP_SEC_DEFAULT, MP_KEYMAP_5, (int32)MPPW_KeyMappings[5]);
  (void)MPConfigWriteInt(pcfg, MP_SEC_DEFAULT, MP_KEYMAP_6, (int32)MPPW_KeyMappings[6]);
  (void)MPConfigWriteInt(pcfg, MP_SEC_DEFAULT, MP_KEYMAP_7, (int32)MPPW_KeyMappings[7]);
  (void)MPConfigWriteInt(pcfg, MP_SEC_DEFAULT, MP_KEYMAP_8, (int32)MPPW_KeyMappings[8]);
  (void)MPConfigWriteInt(pcfg, MP_SEC_DEFAULT, MP_KEYMAP_9, (int32)MPPW_KeyMappings[9]);
  (void)MPConfigWriteInt(pcfg, MP_SEC_DEFAULT, MP_KEYMAP_STAR, (int32)MPPW_KeyMappings[10]); 
  (void)MPConfigWriteInt(pcfg, MP_SEC_DEFAULT, MP_KEYMAP_POUND, (int32)MPPW_KeyMappings[11]);

#if(defined( FEATURE_MP_EQ_UI) && defined( FEATURE_MP_EQ))
 MPSaveAudioConfigData(pme, pcfg);
#endif //#if(defined( FEATURE_MP_EQ_UI) && defined( FEATURE_MP_EQ))

  MPCONFIG_FREEIF(pcfg);

  return SUCCESS;
}

#if(defined( FEATURE_MP_EQ_UI) && defined( FEATURE_MP_EQ))
static void MPSaveAudioConfigData(CMediaPlayer* pme, CMPConfig* pcfg)
{
  MPSetting* psetting = pme->m_pSetting;
  IFileMgr     *pIFileMgr = NULL;
  IFile        *pIFile = NULL;

  if(SUCCESS == ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr))
  {
    if (SUCCESS != IFILEMGR_Test(pIFileMgr, MP_CONFIG_DATA_FILE_NAME))
    {
      // create file
      pIFile = IFILEMGR_OpenFile(pIFileMgr, MP_CONFIG_DATA_FILE_NAME, _OFM_CREATE);
    }
    else
    {
      // create file for appending
      pIFile = IFILEMGR_OpenFile(pIFileMgr, MP_CONFIG_DATA_FILE_NAME, _OFM_APPEND);
    }
  }
  else
  {
    DBGPRINTF_ERROR("MP: failed to create pIFileMgr in MPSaveAudioConfigData.");
    return;
  }

  if(pcfg && psetting)
  {
    (void)MPConfigWriteBoolean(pcfg, MP_SEC_DEFAULT, MP_ENABLE_SPECTRUM_ANALYZER, (int32)psetting->m_bEnableAnalyzer);
    (void)MPConfigWriteBoolean(pcfg, MP_SEC_DEFAULT, MP_ENABLE_EQUALIZER, (int32)psetting->m_bEnableEqualizer);
    (void)MPConfigWriteBoolean(pcfg, MP_SEC_DEFAULT, MP_ENABLE_AUDIO_EXPANDER, (int32)psetting->m_bEnableAudioExpander);
    (void)MPConfigWriteBoolean(pcfg, MP_SEC_DEFAULT, MP_SHOW_SPECTRUM_ANALYZER, (int32)psetting->m_bShowAnalyzer);
    (void)MPConfigWriteInt(pcfg, MP_SEC_DEFAULT, MP_FILTER_ID, (int32)psetting->m_nFilterID);
    (void)MPConfigWriteInt(pcfg, MP_SEC_DEFAULT, MP_SLIDER_ID, (int32)psetting->m_nSliderID);

#if (defined(FEATURE_MP_EQ) && defined(MP_FEATURE_AUDIO_QENSEMBLE))
#error code not present
#endif //#if (defined(FEATURE_MP_EQ) && defined(MP_FEATURE_AUDIO_QENSEMBLE))

#ifdef FEATURE_MP_EQ
    if (pIFile)
    {
      // We enable the buffered I/O and then turn it off.
      (void)IFILE_SetCacheSize(pIFile, SCS_DEFAULT);
      (void)MPConfigFileWriteBinary(pme, pcfg, pIFile, MP_BINARY, MP_DATA_ITEM_EQ, &psetting->m_EqData, sizeof(stEqualizer));
      (void)MPConfigFileWriteBinary(pme, pcfg, pIFile, MP_BINARY, MP_DATA_ITEM_AUDIO_EXPANDER, &psetting->m_AudioExpanderData, sizeof(stAudioExp));
#if defined(MP_FEATURE_AUDIO_QENSEMBLE)    
#error code not present
#endif //#if defined(MP_FEATURE_AUDIO_QENSEMBLE)
      // Turning the buffered I/O off
      (void)IFILE_SetCacheSize(pIFile, 0);
    }
#endif //#ifdef FEATURE_MP_EQ
  }
    
  // Now release the file resources
  (void)IFILE_Release(pIFile);
  if(pIFileMgr)
  {
    (void)IFILEMGR_Release(pIFileMgr);
  }
}

static void MPReadAudioConfigData(CMediaPlayer* pme, CMPConfig* pcfg)
{
  MPSetting* psetting = pme->m_pSetting;

  if(pcfg && psetting)
  {
    psetting->m_bEnableAnalyzer = MPConfigReadBoolean(pcfg, MP_SEC_DEFAULT, MP_ENABLE_SPECTRUM_ANALYZER, FALSE);
    psetting->m_bEnableEqualizer = MPConfigReadBoolean(pcfg, MP_SEC_DEFAULT, MP_ENABLE_EQUALIZER, FALSE);
    psetting->m_bEnableAudioExpander = MPConfigReadBoolean(pcfg, MP_SEC_DEFAULT, MP_ENABLE_AUDIO_EXPANDER, FALSE);
    psetting->m_bShowAnalyzer = MPConfigReadBoolean(pcfg, MP_SEC_DEFAULT, MP_SHOW_SPECTRUM_ANALYZER, FALSE);
    psetting->m_nFilterID = MPConfigReadInt(pcfg, MP_SEC_DEFAULT, MP_FILTER_ID, 0);
    psetting->m_nSliderID = MPConfigReadInt(pcfg, MP_SEC_DEFAULT, MP_SLIDER_ID, 0);
    

#if (defined(FEATURE_MP_EQ) && defined(MP_FEATURE_AUDIO_QENSEMBLE))
#error code not present
#endif //#if (defined(FEATURE_MP_EQ) && defined(MP_FEATURE_AUDIO_QENSEMBLE))

#ifdef FEATURE_MP_EQ
    {
      void *pData;
      
      pData = MPConfigFileReadBinary(pme, pcfg, MP_BINARY, MP_DATA_ITEM_EQ);
      if(pData)
      {
        MEMCPY(&psetting->m_EqData, pData, sizeof(stEqualizer));
        FREE(pData);
      }
    
      pData = MPConfigFileReadBinary(pme, pcfg, MP_BINARY, MP_DATA_ITEM_AUDIO_EXPANDER);
      if(pData)
      {
        MEMCPY(&psetting->m_AudioExpanderData, pData, sizeof(stAudioExp));
        FREE(pData);
      }
#if defined(MP_FEATURE_AUDIO_QENSEMBLE)
#error code not present
#endif //#if defined(MP_FEATURE_AUDIO_QENSEMBLE)
    }
#endif //#ifdef FEATURE_MP_EQ
  }
}
#endif //#if(defined( FEATURE_MP_EQ_UI) && defined( FEATURE_MP_EQ))

