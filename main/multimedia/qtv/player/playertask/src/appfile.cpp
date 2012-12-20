/* =======================================================================
                              AppFile.cpp 
DESCRIPTION
  This class parses the application file and provides methods for
  accessing the data.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  
Portions copyrighted by PacketVideo Corporation; 
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated; 
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/player/playertask/main/latest/src/appfile.cpp#9 $
$DateTime: 2010/01/04 23:14:42 $
$Change: 1127650 $


========================================================================== */

//======================================================================
//   INCLUDES AND VARIABLE DEFINITIONS
//======================================================================

//----------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "AppFile.h"
#include "byte_order_utils.h"

#include "oscl_string_utils.h"

//----------------------------------------------------------------------
// Defines
//----------------------------------------------------------------------
#define EXIT_AFTER_PLAYBACK     1
#define LAUNCH_URL          2
#define FREEZE_ON_LAST_FRAME    4

#define DISABLE_REPOSITIONING   1
#define DOWNLOAD_URL        2
#define PURGE_DOWNLOAD        4
#define DOWNLOAD_PLAYBACK_CTRL    24
#define DISABLE_USER_PLAYBACK   32

//----------------------------------------------------------------------
// Type Declarations
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Global Constant Definitions
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Global Data Definitions
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Static Variable Definitions
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Function Prototypes
//----------------------------------------------------------------------

/*======================================================================
FUNCTION:
  AppFile

DESCRIPTION:
  The constructor initializes the application file information, 
  including the playlist.
  
INPUT/OUTPUT PARAMETERS:
  None.
  
RETURN VALUE:
  None
    
SIDE EFFECTS:
  None
======================================================================*/
AppFile::AppFile()
{
  CurrentClip = 0;
  NumberOfClips = 0;
  ValidData = FALSE;
  LaunchURL = NULL;
  EndOfClipFlags=0;
  PVXFileVersion=0;
  SubscriberAuthorization=0;

  for(int i=0; i<MAXIMUM_CLIPS_IN_PLAYLIST; i++) {
    PlayList[i].ClipName = NULL;
    PlayList[i].StartTime = 0;
    PlayList[i].StopTime = 0;
    PlayList[i].Flags = 0;
    PlayList[i].DigitalSignature = NULL;
    PlayList[i].Source = PV_NONE;
  }
  memset(&Key,0,sizeof( KEY_DEF)); 
} //AppFile

/*======================================================================
FUNCTION:
  ~AppFile

DESCRIPTION:
  The destructor releases any memory allocated by the class.
  
INPUT/OUTPUT PARAMETERS:
  None.
  
RETURN VALUE:
  None
    
SIDE EFFECTS:
  None
======================================================================*/
AppFile::~AppFile()
{
  if( LaunchURL ) 
   QTV_Free(LaunchURL);
   
  for (int i=0; i<NumberOfClips; i++) {
    if (PlayList[i].ClipName) {
      QTV_Free(PlayList[i].ClipName);
      PlayList[i].ClipName = NULL;
    }
    if (PlayList[i].DigitalSignature) {
      QTV_Free(PlayList[i].DigitalSignature);
      PlayList[i].DigitalSignature = NULL;
    }
  }
}   // ~AppFile

/*======================================================================
FUNCTION:
  ParseApplicationFile

DESCRIPTION:
  This function determines if the specified file is a valid PVX file.
  If it is, allocate the file buffer and check for the Success/failure of 
  parsing of PVX file.

INPUT/OUTPUT PARAMETERS:
  Filename  - Name of the file to Parse

RETURN VALUE:
  Status of operation

SIDE EFFECTS:
  None
======================================================================*/
ERROR_CODE AppFile::ParseApplicationFile(char *Filename)
{

  OSCL_FILE * hPvxFile;
  USHORT  fileSize;
  DWORD nBytesRead;
  char  *pvx_file_data;
  ERROR_CODE retVal = PV_NO_ERROR;
#ifndef FEATURE_WINCE
  hPvxFile = OSCL_FileOpen((OSCL_STRING)Filename,"rb");
#else
#error code not present
#endif // #ifndef FEATURE_WINCE

  if( hPvxFile == NULL )
  {
    return PV_INVALID_PVX_FILE;
  }


  // Read the first 2 bytes  of the file to get the file size.  If the bytes
  // couldn't be read then generate an error.

  nBytesRead = OSCL_FileRead((char *) &fileSize, 1,sizeof(fileSize), hPvxFile);

  if (nBytesRead != sizeof(fileSize)) {

    (void)OSCL_FileClose(hPvxFile);

    return PV_INVALID_PVX_FILE;
  }

  // The first 2 bytes contained the file size.  Allocate space to read in the
  // remainder of the file and read it.  If the file couldn't be read then
  // release the space and generate an error.
  little_endian_to_host((char *)&fileSize, sizeof(fileSize));
  pvx_file_data = (char*)QTV_Malloc(fileSize);
  if (pvx_file_data == NULL)
  {
    (void)OSCL_FileClose(hPvxFile);
    return PV_INSUFFICIENT_MEMORY;
  }  
  else 
  {
    nBytesRead=OSCL_FileRead(pvx_file_data, 1,fileSize, hPvxFile);

    if (nBytesRead == 0 || nBytesRead != fileSize)
    {
      QTV_Free(pvx_file_data);
      (void)OSCL_FileClose(hPvxFile);
      return PV_INVALID_PVX_FILE;
    }
  }
  (void)OSCL_FileClose(hPvxFile);

  if( retVal == PV_NO_ERROR)
  {
    retVal = ParseApplicationFileInBuffer(pvx_file_data,fileSize);
  }
  
  if(pvx_file_data != NULL)
  {
    QTV_Free(pvx_file_data);
    pvx_file_data = NULL;
  }
  return retVal;
} 
/*======================================================================
FUNCTION:
  ParseApplicationFileInBuffer

DESCRIPTION:
  This function determines if the specified file is a valid PVX file.
  If it is, parses the file and saves the information.

INPUT/OUTPUT PARAMETERS:
  Filename  - Address of the of the file to parse and filesize

RETURN VALUE:
  Status of operation

SIDE EFFECTS:
  None
======================================================================*/
ERROR_CODE AppFile::ParseApplicationFileInBuffer(char *PvxFileData,USHORT FileSize)
{
  USHORT  file_length;
  USHORT  objectLength;
  int   i;
  char  *pvx_ptr;
  pvx_ptr = PvxFileData;
  file_length = FileSize;  

  // Get the PVX file version ID and increment the file pointer.  If
  // there aren't any more bytes remaining then generate an error.
  if( file_length < sizeof(PVXFileVersion) )
  {
    return PV_INVALID_PVX_FILE;
  }
  memcpy(&PVXFileVersion, pvx_ptr, sizeof(PVXFileVersion));
  little_endian_to_host((char *)&PVXFileVersion, sizeof(PVXFileVersion));
  pvx_ptr += sizeof(PVXFileVersion);
  file_length -= sizeof(PVXFileVersion);  

  // Get the subscriber authorization.  This is the number of bytes between
  // the end of this field and the start of the first URL.
  if( file_length < sizeof(SubscriberAuthorization) )
  {
    return PV_INVALID_PVX_FILE;
  }
  memcpy(&SubscriberAuthorization, pvx_ptr, sizeof(SubscriberAuthorization));
  little_endian_to_host((char *)&SubscriberAuthorization, sizeof(SubscriberAuthorization));
  pvx_ptr += sizeof(SubscriberAuthorization);
  file_length -= sizeof(SubscriberAuthorization);
  
  // Get the User ID.  The first 2 bytes contains the length.
  if( file_length < sizeof(objectLength) )
  {
    return PV_INVALID_PVX_FILE;
  }
  memcpy(&objectLength, pvx_ptr, sizeof(objectLength));
  little_endian_to_host((char *)&objectLength, sizeof(objectLength));
  file_length -= sizeof(objectLength);
  pvx_ptr += sizeof(objectLength);
  if( file_length < objectLength || (objectLength >= (MAX_USER_ID_LEN + 1)))
  {
    return PV_INVALID_PVX_FILE;    
  }
  memcpy(Key.UserID, pvx_ptr, objectLength); 
  Key.UserID[objectLength] = '\0';
  file_length -= objectLength;
  pvx_ptr += objectLength;
  

  // Get the Application-specific string.  The first 2 bytes contains the length.
  if( file_length < sizeof(objectLength) )
  {
    return PV_INVALID_PVX_FILE;
  }
  memcpy(&objectLength, pvx_ptr, sizeof(objectLength));
  little_endian_to_host((char *)&objectLength, sizeof(objectLength));
  file_length -= sizeof(objectLength);
  pvx_ptr += sizeof(objectLength);
  if( file_length < objectLength || (objectLength >= (MAX_INFO_LEN +1)))
  {
    return PV_INVALID_PVX_FILE;
  }
  memcpy(Key.ApplicationInfo, pvx_ptr, objectLength);/*removed & before key*/
  Key.ApplicationInfo[objectLength] = '\0';
  file_length -= objectLength;
  pvx_ptr += objectLength;
  

  // Get the security indicator.
  if (file_length < 1) {
    return PV_INVALID_PVX_FILE;
  }
  Key.Security = *pvx_ptr++;
  file_length--;

  // Get the authentication session ID.
  if (file_length < MAX_SESSION_ID_LEN) {
    return PV_INVALID_PVX_FILE;
  }
  memcpy(Key.SessionID, pvx_ptr, MAX_SESSION_ID_LEN);
  Key.SessionID[MAX_SESSION_ID_LEN] = '\0';
  pvx_ptr += MAX_SESSION_ID_LEN;
  file_length -= MAX_SESSION_ID_LEN;

  // Get the expiry date/time.
  if (file_length < MAX_DATE_LEN) {
    return PV_INVALID_PVX_FILE;
  }
  memcpy(Key.ExpiryDate, pvx_ptr, MAX_DATE_LEN);
  Key.ExpiryDate[MAX_DATE_LEN] = '\0';
  pvx_ptr += MAX_DATE_LEN;
  file_length -= MAX_DATE_LEN;

  // Get the random filler data.
  if (file_length < MAX_FILLER_LEN) {
    return PV_INVALID_PVX_FILE;
  }
  memcpy(Key.Filler, pvx_ptr, MAX_FILLER_LEN);
  Key.Filler[MAX_FILLER_LEN] = '\0';
  pvx_ptr += MAX_FILLER_LEN;
  file_length -= MAX_FILLER_LEN;

  // Get the number of clips in the play list.
  // For Imperial, play list is not implemented and num of clips == 1
  NumberOfClips = 1;
  /******************************************************
  memcpy(&NumberOfClips, pvx_ptr, sizeof(NumberOfClips));
  pvx_ptr += sizeof(NumberOfClips);
  file_length -= sizeof(NumberOfClips);
  if (file_length <= 0) {
    return PV_INVALID_PVX_FILE;
  }
  ********************************************************/

  // If the object length is 0 then this is probably an old PVX file which didn't
  // have a key definition.  If that's the case this section will just be skipped.
  for (i=0; i<NumberOfClips; i++) {
    if (file_length == 0) { /* file_length is unsigned */
     return PV_INVALID_PVX_FILE;
    }

    // Get the digital signature.
    if (file_length < MAX_SIGNATURE_LEN) {
     return PV_INVALID_PVX_FILE;
    }
    if (PlayList[i].DigitalSignature) {
      QTV_Free(PlayList[i].DigitalSignature);
      PlayList[i].DigitalSignature = NULL;
    }
    PlayList[i].DigitalSignature = (BYTE*)QTV_Malloc(sizeof(BYTE)*(MAX_SIGNATURE_LEN+1));
    if(PlayList[i].DigitalSignature == NULL)
    {
      return PV_INSUFFICIENT_MEMORY;
    }
    memcpy(PlayList[i].DigitalSignature, pvx_ptr, MAX_SIGNATURE_LEN);
    PlayList[i].DigitalSignature[MAX_SIGNATURE_LEN] = '\0';
    pvx_ptr += MAX_SIGNATURE_LEN;
    file_length -= MAX_SIGNATURE_LEN;

    // Retrieve the URL
    if (file_length < sizeof(objectLength)) 
    {
      return PV_INVALID_PVX_FILE;
    }
    memcpy(&objectLength, pvx_ptr, sizeof(objectLength));
    little_endian_to_host((char *)&objectLength, sizeof(objectLength));
    file_length -= sizeof(objectLength);
    pvx_ptr += sizeof(objectLength);

    if (file_length < objectLength ) 
    {
      return PV_INVALID_PVX_FILE;
    }
    
    int urlLength = objectLength + 1;
    
    if (PlayList[i].ClipName){
      QTV_Free(PlayList[i].ClipName);
      PlayList[i].ClipName = NULL;
    }

    PlayList[i].ClipName = (char*)QTV_Malloc(urlLength); 

    if(PlayList[i].ClipName == NULL )
    {
      return PV_INSUFFICIENT_MEMORY;
    }

    memcpy(PlayList[i].ClipName, pvx_ptr, objectLength);
    pvx_ptr += objectLength;
    file_length -= objectLength;
    PlayList[i].ClipName[objectLength]='\0';

    if(!strincmp(PlayList[i].ClipName, "RTSP://", 7) || 
         !strincmp(PlayList[i].ClipName, "RTSPT://", 8) )
    {
      PlayList[i].Source = PV_REMOTE;
    }
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
    else if(!strincmp(PlayList[i].ClipName, "HTTP://", 7))
    {    
      PlayList[i].Source = PV_DOWNLOAD;
    }
#endif
    else if(!strincmp(PlayList[i].ClipName, "FILE://", 7))
    {
      if(urlLength >= 7)
       urlLength -= 7; // Reduce url length by length of "file://"      
      
      (void)memmove( PlayList[i].ClipName, PlayList[i].ClipName + 7, urlLength);

      PlayList[i].Source = PV_LOCAL;      
    }
    else
    {
      PlayList[i].Source = PV_NONE;
    }
    
    // Get the playback start and stop times.
    if (file_length < sizeof(PlayList[i].StartTime)) 
    {
      return PV_INVALID_PVX_FILE;
    }
    memcpy(&PlayList[i].StartTime, pvx_ptr, sizeof(PlayList[i].StartTime));
    little_endian_to_host((char *)&PlayList[i].StartTime, sizeof(PlayList[i].StartTime));
    pvx_ptr += sizeof(PlayList[i].StartTime);
    file_length -= sizeof(PlayList[i].StartTime);
    
    if (file_length < sizeof(PlayList[i].StopTime)) 
    {
      return PV_INVALID_PVX_FILE;
    }
    memcpy(&PlayList[i].StopTime, pvx_ptr, sizeof(PlayList[i].StopTime));
    little_endian_to_host((char *)&PlayList[i].StopTime, sizeof(PlayList[i].StopTime));
    pvx_ptr += sizeof(PlayList[i].StopTime);
    file_length -= sizeof(PlayList[i].StopTime);
    

    // Get the playback flags.
    if (file_length < sizeof(PlayList[i].Flags)) 
    { 
      return PV_INVALID_PVX_FILE;
    }

    memcpy(&PlayList[i].Flags, pvx_ptr, sizeof(PlayList[i].Flags));
    little_endian_to_host((char *)&PlayList[i].Flags, sizeof(PlayList[i].Flags));
    pvx_ptr += sizeof(PlayList[i].Flags);
    file_length -= sizeof(PlayList[i].Flags);
    
  }
  
  /********************************************************
  // Volume, loop count and zoom level fields are not in Imperial

  // Get the volume.
  memcpy(&Volume, pvx_ptr, sizeof(Volume));
  pvx_ptr += sizeof(Volume);
  file_length -= sizeof(Volume);
  if (file_length <= 0) {
    return PV_INVALID_PVX_FILE;
  }

  // Get the loop count.
  memcpy(&LoopCount, pvx_ptr, sizeof(LoopCount));
  pvx_ptr += sizeof(LoopCount);
  file_length -= sizeof(LoopCount);
  if (file_length <= 0) {
    return PV_INVALID_PVX_FILE;
  }

  // Get the zoom level.
  memcpy(&ZoomLevel, pvx_ptr, sizeof(ZoomLevel));
  pvx_ptr += sizeof(ZoomLevel);
  file_length -= sizeof(ZoomLevel);
  if (file_length <= 0) {
    return PV_INVALID_PVX_FILE;
  }
  *********************************************************/
  
  // Get the end-of-clip flags.
  if (file_length < sizeof(EndOfClipFlags)) 
  {  
      return PV_INVALID_PVX_FILE;
  }

  memcpy(&EndOfClipFlags, pvx_ptr, sizeof(EndOfClipFlags));
  pvx_ptr += sizeof(EndOfClipFlags);
  file_length -= sizeof(EndOfClipFlags);
  

  // Get the URL to launch after playback of the list.  
  // The first 2 bytes contains the length.
  if (file_length < sizeof(objectLength)) 
  {
    return PV_INVALID_PVX_FILE;
  }
  memcpy(&objectLength, pvx_ptr, sizeof(objectLength));
  little_endian_to_host((char *)&objectLength, sizeof(objectLength));
  file_length -= sizeof(objectLength);
  pvx_ptr += sizeof(objectLength);
  if (file_length < objectLength) { /* file_length is unsigned */
      return PV_INVALID_PVX_FILE;
  }
  else
  { 
    if ( objectLength > 0) 
    {
      if(LaunchURL) 
    {
        QTV_Free(LaunchURL);
    }

    LaunchURL = (char*)QTV_Malloc(objectLength+1);

    if(LaunchURL == NULL )
    {
      return PV_INSUFFICIENT_MEMORY;  
    }
      memcpy(LaunchURL, pvx_ptr, objectLength);
      LaunchURL[objectLength] = '\0';
    file_length -= objectLength;
    pvx_ptr += objectLength;

  }
  else 
  {
    LaunchURL = NULL;
  }
  }

  ValidData = (PlayList[0].Source != PV_NONE);

    // Check for conflicting flags
  ULONG start, stop;
  (void)GetClipTimes( &start, &stop);
  if( AllowDownload() &&
            (start != 0 || ExitAfterPlayback() ||
             LaunchURLAfterPlayback() || AllowRepositioning()) ) {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "ParseApplicationFile: FAILED - Download specification conflicts with");
          QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "start=%d or stop=%d or ", start, stop);
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "exit after playback=%d or", ExitAfterPlayback());
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "launch after playback=%d or", LaunchURLAfterPlayback());
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "allow repositioning=%d", AllowRepositioning());
          return PV_INVALID_PVX_FILE;
  }
  if( !AllowDownload() && 
      (DisableUserPlayback() ||
       PurgeDownloadFile() ||
       GetDownloadPlayback() != NOT_AUTO_START )) {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "ParseApplicationFile: FAILED - Non-download specification conflicts with");
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "disable user playback=%d or", DisableUserPlayback());
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "purge download file=%d or", PurgeDownloadFile());
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "get download playback=%d or", GetDownloadPlayback());
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "URL source=%d", PlayList[0].Source);
          return PV_INVALID_PVX_FILE;
  }

  return PV_NO_ERROR;
} // ParseApplicationFile


/*======================================================================
FUNCTION:
  IsValidAppFile

DESCRIPTION:
  This function returns TRUE if the file provided to ParseApplicationFile
  was a valid PVX file and FALSE otherwise.

INPUT/OUTPUT PARAMETERS:
  None

RETURN VALUE:
  TRUE if the application file was a PVX file.

SIDE EFFECTS:
  None
======================================================================*/
BOOL AppFile::IsValidAppFile()
{
  return ValidData;
}    // IsValidAppFile


/*======================================================================
FUNCTION:
  GetClipSource

DESCRIPTION:
  This function returns the source (LOCAL, REMOTE, or DOWNLOAD) of the
  specified item in the play list.  If no item is specified it returns
  the source of the current clip.

INPUT/OUTPUT PARAMETERS:
  ClipNumber  - Index of a file in the play list or -1 for the current item.

RETURN VALUE:
  Clip source

SIDE EFFECTS:
  None
======================================================================*/
CLIP_SOURCE AppFile::GetClipSource(int ClipNumber)
{
  if (ClipNumber >= 0)
    return PlayList[ClipNumber].Source;
  else
    return PlayList[CurrentClip].Source;
} // GetClipSource


/*======================================================================
FUNCTION:
  GetClipName

DESCRIPTION:
  This function returns the URN of a specified clip in a play list.

INPUT/OUTPUT PARAMETERS:
  ClipNumber  - Index of a file in the play list or -1 for the current item.

RETURN VALUE:
  Clip URN

SIDE EFFECTS:
  None
======================================================================*/
char *AppFile::GetClipName(int  ClipNumber)
{
  if (ClipNumber >= 0)
    return PlayList[ClipNumber].ClipName;
  else
    return PlayList[CurrentClip].ClipName;
} // GetClipName


/*======================================================================
FUNCTION:
  GetClipTimes

DESCRIPTION:
  This function returns the start and stop time of a clip in a play list.

INPUT/OUTPUT PARAMETERS:
  StartTime - Start time of the clip in milliseconds.
  StopTime  - Stop time of the clip in milliseconds.
  ClipNumber  - Index of a file in the play list or -1 for the current item.

RETURN VALUE:
  PV_NO_ERROR

SIDE EFFECTS:
  None
======================================================================*/
ERROR_CODE AppFile::GetClipTimes(ULONG *StartTime, ULONG *StopTime, int ClipNumber)
{
  if (ClipNumber >= 0) {
    *StartTime = (ULONG)PlayList[ClipNumber].StartTime;
    *StopTime = (ULONG)PlayList[ClipNumber].StopTime;
  }
  else {
    *StartTime = (ULONG)PlayList[CurrentClip].StartTime;
    *StopTime = (ULONG)PlayList[CurrentClip].StopTime;
  }

  return PV_NO_ERROR;
} // GetClipTimes


/*======================================================================
FUNCTION:
  AllowDownload

DESCRIPTION:
  This function returns TRUE if the file can be downloaded.

INPUT/OUTPUT PARAMETERS:
  ClipNumber  - Index of a file in the play list or -1 for the current item.

RETURN VALUE:
  TRUE if download is allowed.

SIDE EFFECTS:
  None
======================================================================*/
BOOL AppFile::AllowDownload(int ClipNumber)
{
  int rv;
  
  if (ClipNumber >= 0)
    rv = (PlayList[ClipNumber].Flags & DOWNLOAD_URL) >> 1;
  else
    rv = (PlayList[CurrentClip].Flags & DOWNLOAD_URL) >> 1;

  return (rv != 0);
}       // AllowDownload                                                 


/*======================================================================
FUNCTION:
  AllowRepositioning

DESCRIPTION:
  This function returns TRUE if random positioning within the clip is
  permitted.

INPUT/OUTPUT PARAMETERS:
  ClipNumber  - Index of a file in the play list or -1 for the current item.

RETURN VALUE:
  TRUE if random positioning is allowed.

SIDE EFFECTS:
  None
======================================================================*/
BOOL AppFile::AllowRepositioning(int ClipNumber)
{
  if (ClipNumber >= 0)
    return !(PlayList[ClipNumber].Flags & DISABLE_REPOSITIONING);
  else
    return !(PlayList[CurrentClip].Flags & DISABLE_REPOSITIONING);
}                                 // AllowRepositioning

/*======================================================================
FUNCTION:
  GetFlags

DESCRIPTION:
  This function returns all the playback control flags.

INPUT/OUTPUT PARAMETERS:
  ClipNumber  - Index of a file in the play list or -1 for the current item.

RETURN VALUE:
  flags.

SIDE EFFECTS:
  None
======================================================================*/
unsigned int AppFile::GetFlags( int ClipNumber  )
{
  if (ClipNumber >= 0)
    return PlayList[ClipNumber].Flags ;
  else
    return PlayList[CurrentClip].Flags ;
}

/*======================================================================
FUNCTION:
  PurgeDownloadFile

DESCRIPTION:
  This function returns TRUE if the downloaded file should be deleted
  after the session ends.  

INPUT/OUTPUT PARAMETERS:
  ClipNumber  - Index of a file in the play list or -1 for the current item.

RETURN VALUE:
  TRUE if the downloaded file should be deleted after the session ends.  

SIDE EFFECTS:
  None
======================================================================*/
BOOL AppFile::PurgeDownloadFile(int ClipNumber)
{
  int rv;
  if (ClipNumber >= 0)
    rv = (PlayList[ClipNumber].Flags & PURGE_DOWNLOAD) >> 2;
  else
    rv = (PlayList[CurrentClip].Flags & PURGE_DOWNLOAD) >> 2;

  return (rv != 0);
}                                 // PurgeDownloadFile


/*======================================================================
FUNCTION:
  GetDownloadPlayback

DESCRIPTION:
  This function returns the download playback control type of the clip  

INPUT/OUTPUT PARAMETERS:
  ClipNumber  - Index of a file in the play list or -1 for the current item.

RETURN VALUE:
  NOT_AUTO_START if playback is never automatically started
  AUTO_START if playback starts automatically when download is completed
  START_ASAP if playback starts as soon as possible

SIDE EFFECTS:
  None
======================================================================*/
DOWNLOAD_PLAYBACK AppFile::GetDownloadPlayback(int ClipNumber)
{
  unsigned int n;
  if (ClipNumber >= 0)
    n = (PlayList[ClipNumber].Flags & DOWNLOAD_PLAYBACK_CTRL)>>3;
  else
    n = (PlayList[CurrentClip].Flags & DOWNLOAD_PLAYBACK_CTRL)>>3;
  return (DOWNLOAD_PLAYBACK)n;
}                                        // GetDownloadPlayback


/*======================================================================
FUNCTION:
  DisableUserPlayback

DESCRIPTION:
  This function returns TRUE if user playback of downloaded clip is
  disabled

INPUT/OUTPUT PARAMETERS:
  ClipNumber  - Index of a file in the play list or -1 for the current item.

RETURN VALUE:
  TRUE if user playback of downloaded clip is disabled  

SIDE EFFECTS:
  None
======================================================================*/
BOOL AppFile::DisableUserPlayback(int ClipNumber)
{
  int rv;
  if (ClipNumber >= 0)
    rv = (PlayList[ClipNumber].Flags & DISABLE_USER_PLAYBACK) >> 5;
  else
    rv = (PlayList[CurrentClip].Flags & DISABLE_USER_PLAYBACK) >> 5;

  return (rv != 0);
}                                           // DisableUserPlayback


/*======================================================================
FUNCTION:
  GetKey

DESCRIPTION:
  This function returns the key data contained in the application file.

INPUT/OUTPUT PARAMETERS:
  ClipNumber  - Index of a file in the play list or -1 for the current item.

RETURN VALUE:
  Pointer to the key structure.

SIDE EFFECTS:
  None
======================================================================*/
KEY_DEF *AppFile::  GetKey(int ClipNumber)
{
  SetDigitalRights(ClipNumber);
  return  &Key; 
} // GetKey

/*======================================================================
FUNCTION:
  SetDigitalRights

DESCRIPTION:
  This function saves the digital signature of a file in the play list.

INPUT/OUTPUT PARAMETERS:
  ClipNumber  - Index of a file in the play list or -1 for the current item.

RETURN VALUE:
  None

SIDE EFFECTS:
  None
======================================================================*/
void AppFile::SetDigitalRights(int ClipNumber)
{
  if (ClipNumber >= 0)
    Key.Signature = PlayList[ClipNumber].DigitalSignature;
  else
    Key.Signature = PlayList[CurrentClip].DigitalSignature;
} // SetDigitalRights

/*======================================================================
FUNCTION:
  GetCurrentClip

DESCRIPTION:
  This function returns index of the current clip in the play list.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  Index of the current clip in the play list.

SIDE EFFECTS:
  None
======================================================================*/
int AppFile::GetCurrentClip()
{
  return CurrentClip;
}                                 // GetCurrentClip

/*======================================================================
FUNCTION:
  SetCurrentClip

DESCRIPTION:
  This function sets the index of the playlist to the indicated value.

INPUT/OUTPUT PARAMETERS:
  ClipNumber  - Index of a file in the play list.

RETURN VALUE:
  None

SIDE EFFECTS:
  None
======================================================================*/
void AppFile::SetCurrentClip(int ClipNumber)
{
  CurrentClip = ClipNumber;
} // SetCurrentClip

/*======================================================================
FUNCTION:
  GetTotalClips

DESCRIPTION:
  This function returns the total number of clips in the play list.

INPUT/OUTPUT PARAMETERS:
  None

RETURN VALUE:
  Number of clips in the play list.

SIDE EFFECTS:
  None
======================================================================*/
int AppFile::GetTotalClips()
{
  return NumberOfClips;
}                                           // GetTotalClips

/*======================================================================
FUNCTION:
  ExitAfterPlayback

DESCRIPTION:
  This function returns TRUE if this flag was set in the application file.

INPUT/OUTPUT PARAMETERS:
  None

RETURN VALUE:
  TRUE if the application is to terminate after the clip finishes.

SIDE EFFECTS:
  None
======================================================================*/
BOOL AppFile::ExitAfterPlayback()
{
  return ((EndOfClipFlags & EXIT_AFTER_PLAYBACK) != 0);
}                                                             // ExitAfterPlayback

/*======================================================================
FUNCTION:
  LaunchURLAfterPlayback

DESCRIPTION:
  This function returns TRUE if this flag was set in the application file.

INPUT/OUTPUT PARAMETERS:
  None

RETURN VALUE:
  TRUE if a URL is to be launched after the clip finishes.

SIDE EFFECTS:
  None
======================================================================*/
char *AppFile::LaunchURLAfterPlayback()

{
  if ((EndOfClipFlags & LAUNCH_URL) != 0)
    return LaunchURL;
  else
    return NULL;
} // LaunchURLAfterPlayback

/*======================================================================
FUNCTION:
  IsSecured

DESCRIPTION:
  This function returns TRUE if the security flag was set in the
  application file.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  TRUE if the file security flag was set.

SIDE EFFECTS:
  None
======================================================================*/
BOOL AppFile::IsSecured()
{
  return (Key.Security == TRUE);
}                                               // IsSecured

/*======================================================================
FUNCTION:
  FreezeOnLastFrame

DESCRIPTION:
  This function returns TRUE if this flag was set in the application file.

INPUT/OUTPUT PARAMETERS:
  None

RETURN VALUE:
  TRUE if the clip is to end on the last frame, FALSE if a black display.

SIDE EFFECTS:
  None
======================================================================*/
BOOL AppFile::FreezeOnLastFrame()
{
  return ((EndOfClipFlags & FREEZE_ON_LAST_FRAME) != 0);
}                                                // FreezeOnLastFrame

/*********************************************************************
// Not implemented in Imperial

======================================================================
FUNCTION:
  GetVolume

DESCRIPTION:
  This function returns volume specified in the application file.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  Volume, as a percentage.

SIDE EFFECTS:
  None
======================================================================
int AppFile::GetVolume()
{
  return Volume;
} // GetVolume

======================================================================
FUNCTION:
  GetLoopCount

DESCRIPTION:
  This function returns the loop count specified in the application file.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  Number of times the clip is to loop.

SIDE EFFECTS:
  None
======================================================================
int AppFile::GetLoopCount()
{
  return LoopCount;
} // GetLoopCount
***********************************************************************/
