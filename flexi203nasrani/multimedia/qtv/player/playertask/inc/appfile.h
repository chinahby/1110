#ifndef __APP_FILE_H__
#define __APP_FILE_H__

/* =======================================================================
                              AppFile.h
DESCRIPTION
  Meaningful description of the definitions contained in this file.
  Description must specify if the module is portable specific, mobile
  specific, or common to both, and it should alert the reader if the
  module contains any conditional definitions which tailors the module to
  different targets.
  
Portions copyrighted by PacketVideo Corporation; 
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated; 
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/player/playertask/main/latest/inc/appfile.h#7 $
$DateTime: 2008/05/08 12:36:04 $
$Change: 656312 $


========================================================================== */

//oscl includes
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "qcplayer.h"

//redefine windows types to basic types.
#define BOOL bool
#define BYTE unsigned char
#define ULONG unsigned long
#define USHORT unsigned short
#define DWORD unsigned int

//stuff from pvplayer "common.h"
#define MAX_TITLE_LEN   256
#define MAX_DESCRIPTION_LEN 256
#define MAX_COPYRIGHT_LEN 256
#define MAX_AUTHOR_LEN    256
#define MAX_RATING_LEN    256
#define MAX_OWNER_LEN   256
#define MAX_ERROR_LEN   256
#define MAX_USER_ID_LEN   256
#define MAX_SESSION_ID_LEN  36
#define MAX_DATE_LEN    20
#define MAX_INFO_LEN    256
#define MAX_FILLER_LEN    16
#define MAX_SIGNATURE_LEN 40
#define MAX_RASREG_LEN       (2*RAS_MaxEntryName+1) // twice the length to store the ras entry name

typedef enum
{
  PV_NO_ERROR,
  PV_NO_CLIP,
  PV_NO_AUDIO_DEVICE,
  PV_NO_VIDEO_DEVICE,
  PV_NO_RENDER,
  PV_UNABLE_TO_CREATE_THREAD,
  PV_UNABLE_TO_RESUME_THREAD,
  PV_UNABLE_TO_SET_THREAD_PRIORITY,
  PV_UNABLE_TO_CREATE_EVENT,
  PV_UNABLE_TO_SET_EVENT,
  PV_UNABLE_TO_RESET_EVENT,
  PV_UNABLE_TO_SET_PRIORITY,
  PV_INVALID_FILE_TYPE,
  PV_INSUFFICIENT_MEMORY,
  PV_INVALID_URL,
  PV_NO_AUDIO,
  PV_INVALID_FILE_HANDLE,
  PV_INVALID_PVX_FILE,
  PV_INVALID_SDP_FILE,
  PV_UNABLE_TO_SEND_SDP_REQUEST,
  PV_SDP_INVALID_RESPONSE,
  PV_SDP_HTTP_301_RESPONSE,
  PV_SDP_HTTP_302_RESPONSE,
  PV_SDP_HTTP_305_RESPONSE,
  PV_SDP_HTTP_307_RESPONSE,
  PV_SDP_HTTP_ERROR_RESPONSE,
  PV_INVALID_SDP_URL,
  PV_UNABLE_TO_CONNECT,
  PV_UNABLE_TO_GET_STREAM_INFO,
  PV_UNABLE_TO_PAUSE,
  PV_UNABLE_TO_RESUME,
  PV_UNABLE_TO_PLAY,
  PV_UNABLE_TO_STOP,
  PV_NO_VALID_CONNECTION,
  PV_NO_DATA,
  PV_CANCEL_CONNECTION,
  PV_EOS,
  PV_THREAD_ERROR,
  PV_INVALID_BITMAP,
  PV_UNABLE_TO_INITIALIZE_DEVICE,
  PV_INVALID_COMMAND_LINE,
  PV_RAS_ERROR,
  PV_UNABLE_TO_LOAD_LIBRARY,
  PV_INVALID_PARAM,
  PV_UNSUPPORTED_CODEC,
  PV_INVALID_STATE,
  PV_PARTIAL_DATA,
  PV_SERVER_FAILURE
} ERROR_CODE;

typedef enum
{
  PV_NONE
  , PV_LOCAL
  , PV_REMOTE
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
  , PV_DOWNLOAD
#endif
} CLIP_SOURCE;

typedef enum
{
  NOT_AUTO_START,
  AUTO_START,
  START_ASAP
} DOWNLOAD_PLAYBACK;

typedef struct _APP_FILE_DATA_
{

  char  *ClipName;

  int   StartTime;
  int   StopTime;
  unsigned int    Flags;
  BYTE  *DigitalSignature;
  CLIP_SOURCE Source;
} APP_FILE_DATA;

typedef struct KEY_DEF_
{
  BOOL ValidData;             // Indicates if the key structure contains valid data
  BYTE Security;              // Indicates if security is enabled
  BYTE UserID[MAX_USER_ID_LEN+1];     // Unique user id generated during provisioning
  BYTE SessionID[MAX_SESSION_ID_LEN+1]; // Authentication session ID
  BYTE ExpiryDate[MAX_DATE_LEN+1];    // Expiry date/time
  BYTE ApplicationInfo[MAX_INFO_LEN+1]; // Application specific string
  BYTE Filler[MAX_FILLER_LEN+1];      // Random filler field
  BYTE *Signature;            // Digital signature
  ULONG StartTime;            // Requested clip start time
  ULONG StopTime;             // Requested clip stop time
} KEY_DEF;
//end of stuff from pvplayer "common.h"


#define MAXIMUM_CLIPS_IN_PLAYLIST 1

class AppFile 
{
public:

  KEY_DEF Key;

  AppFile();
  ~AppFile();

  ERROR_CODE ParseApplicationFile(char *AppFile);


  BOOL IsValidAppFile();


  char *GetClipName(int ClipNumber = -1);

  CLIP_SOURCE GetClipSource(int ClipNumber = -1);
  ERROR_CODE GetClipTimes(ULONG *StartTime, ULONG *StopTime, int ClipNumber = -1);
  BOOL AllowRepositioning(int ClipNumber = -1);
  BOOL AllowDownload(int ClipNumber = -1);
  BOOL PurgeDownloadFile( int ClipNumber = -1 );
  DOWNLOAD_PLAYBACK GetDownloadPlayback( int ClipNumber = -1 );
  BOOL DisableUserPlayback( int ClipNumber = -1 );
  unsigned int GetFlags( int ClipNumber = -1 );

   KEY_DEF * GetKey(int ClipNumber = -1);
  void SetDigitalRights(int ClipNumber = -1);

  int GetCurrentClip();
  void SetCurrentClip(int ClipNumber = -1);
  int GetTotalClips();

  BOOL ExitAfterPlayback();
  char *LaunchURLAfterPlayback();

  BOOL IsSecured();
  BOOL FreezeOnLastFrame();

  // Not included in Imperial
  int  GetVolume() { return 0;}
  int  GetLoopCount() { return 0;}

private:
  BOOL  ValidData;
  USHORT EndOfClipFlags;
  USHORT PVXFileVersion;
  USHORT SubscriberAuthorization;  
  ERROR_CODE ParseApplicationFileInBuffer(char *PvxFileData,USHORT FileSize); 
  char *LaunchURL;

  int NumberOfClips;
  int CurrentClip;
  APP_FILE_DATA PlayList[MAXIMUM_CLIPS_IN_PLAYLIST];

  /******************************************
  // Not included in Imperial

  USHORT Volume;
  USHORT LoopCount;
  USHORT ZoomLevel;
  *******************************************/
};
#endif // __ APP_FILE_H__
