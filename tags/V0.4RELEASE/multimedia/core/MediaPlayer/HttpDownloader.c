/*===========================================================================
FILE: HttpDownloader.c

SERVICES: Sample implementation of downloading media clip data via network.

DESCRIPTION
  This file contains the implementation of downloading media clips via HTTP.

       Copyright  2000-2006 QUALCOMM Incorporated.
                    All Rights Reserved.
                   QUALCOMM Proprietary/GTDR

$Header: //depot/asic/msmshared/apps/MediaPlayer/HttpDownloader.c#44 $

===========================================================================*/

/*===========================================================================
                     INCLUDES AND VARIABLE DEFINITIONS
===========================================================================*/
#include "target.h"             /* Target specific definitions             */
#include "customer.h"           /* Customer specific definitions           */

#include "AEEComdef.h"

#include "HttpDownloader.h"

#include <stdio.h>    /* for sscanf() usage below */
#include <string.h>   /* for strstr() usage below */

/* Each HTTP GET attempts to download 20k bytes of data */
#define HTTP_GET_SIZE (20*1024)
#define HTTP_OK  200
//#define HTTP_PARTIAL_RESPONSE 206
//#define PS_STATE_INIT 1
//#define PS_STATE_DOWNLOAD 2
//#define PS_STATE_COMPLETE 3
//#define PS_STATE_ERROR 4

#define LINGER_TIME_SECONDS   2

/*===========================================================================
                      PUBLIC DATA DECLARATIONS
===========================================================================*/
#if defined(FEATURE_APP_MPEG4) && (defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))

#ifndef FEATURE_APP_MEDIA_VIDEO
static uint32 pendingHttpSize = 0;
static uint32 copiedHttpSize  = 0;
static uint32 bodySize = 0;
#endif //FEATURE_APP_MEDIA_VIDEO
static uint16 default_linger_time = 30; /* in seconds */

extern uint32 HttpDownloader_maxBufferSize;  /* set in MediaPlayer.c */
extern char* HttpDownloader_sharedBuffer;
extern boolean HttpDownloader_entireClipInBuffer;
extern int HTTPDownloadInterface;
extern boolean HttpDownloader_suspended;
extern boolean HttpDownloader_resuming;



/*-------------------------------------------------------------------
            Function Prototypes
-------------------------------------------------------------------*/
static void ReleaseObj(void ** ppObj);
static void WebAction_Stop(CHttpBlock* psBlk);
void PS_Free_web(CHttpBlock* psBlk);
void Search_And_Store(char * dst, const char * src, const char *searchStr, unsigned int destLen);
boolean http_ps_blockInitialize(CHttpBlock* psBlk, const char * pszStream);
void http_ps_get_size(CHttpBlock* psHttpBlock);
void http_ps_get_chunk(CHttpBlock** ppsBlk);
static void WebAction_GotResp(CHttpBlock** ppsBlk);
#ifndef FEATURE_APP_MEDIA_VIDEO
static const char* HTTP_Stream_next_string(const char *str);
static int  HTTP_Stream_GetContentRange( const char *str );
static void WebAction_readMore(CHttpBlock** ppsBlk);
boolean http_ps_blockUpdate(CHttpBlock* psBlk);
#endif // !FEATURE_APP_MEDIA_VIDEO

/*===========================================================================
   This function is used to free an Interface
 ===========================================================================*/
static void ReleaseObj(void ** ppObj)
{
  if (*ppObj) {
    DBGPRINTF_MED("MP: calling IBASE_Release()");
    (void)IBASE_Release(((IBase *)*ppObj));
    DBGPRINTF_MED("MP: IBASE_Release() done");
    *ppObj = NULL;
  }
}

/*===========================================================================
   This function is used to free datastructures used for each transaction
   Cancel any active web transaction callbacks
 ===========================================================================*/
static void WebAction_Stop(CHttpBlock* psBlk)
{
  // this cancels any pending web transaction, or readable on the
  // response body.  if nothing is pending, this has no effect
  CALLBACK_Cancel(&psBlk->m_webCallback);
  CALLBACK_Cancel(&psBlk->m_readMoreCallback);
  ReleaseObj((void **)&psBlk->m_pIWebResp);/* let response go */
}

void PS_Free_web(CHttpBlock* psBlk)
{
  DBGPRINTF_MED("MP: PS_Free_web()");

  if (NULL == psBlk)
  {
    return;
  }

  /* Clear datastructures and cancel callbacks used for this HTTP GET
    transaction*/
  WebAction_Stop(psBlk);
  if (NULL != psBlk->m_pIWeb)
  {
    ReleaseObj((void **)&psBlk->m_pIWeb);
    psBlk->m_pIWeb = NULL;
  }
  
  psBlk->bWebPstreaming = FALSE;

  if(psBlk->szURL != NULL)
  {
    FREE(psBlk->szURL);
    psBlk->szURL = NULL;
  }

  if (psBlk->m_pINetMgr != NULL)
  {
    //uint16 retval;
    uint32 retval2;

    /* restore default linger time - commented out */
    /* PS_CleanHttpDownload() is called when the player window is deleted.  If the linger time */
    /* is restored, then the connection will remain active for that specified time    */
    /* outside the player window (e.g. in the playlist and when MediaPlayer exits).   */
    //DBGPRINTF_MED("MP: Restoring NETMGR_SetLinger with time %d", default_linger_time);
    //retval = INETMGR_SetLinger(psBlk->m_pINetMgr, default_linger_time);
    //DBGPRINTF_MED("MP: NETMGR_SetLinger returns %d", retval);
    DBGPRINTF_MED("MP: calling INETMGR_Release()");
    retval2 = INETMGR_Release(psBlk->m_pINetMgr);
    DBGPRINTF_MED("MP: INETMGR_Release() returns %d", retval2);
    psBlk->m_pINetMgr = NULL;
  }
}

void PS_CleanHttpDownload(CHttpBlock* psBlk)
{
  DBGPRINTF_MED("MP: PS_CleanHttpDownload()");

  if (NULL == psBlk)
  {
    return;
  }

  PS_Free_web(psBlk);

  if(psBlk->bKDDIExt)
  {
    FREE(psBlk->m_pKDDIblk);
    psBlk->m_pKDDIblk = NULL;
    psBlk->bKDDIExt = FALSE;
  }  
}

void Search_And_Store(char * dst, const char * src, const char *searchStr, unsigned int destLen)
{
  char *param = STRSTR(src,searchStr);
  if(param){
    param += STRLEN(searchStr);
    while(*param != 0x0A && (destLen -1)){
      *dst++ = *param++;
      destLen--;
    }
    *dst='\0';
  }
}

boolean http_ps_SetUpWeb(CHttpBlock* psBlk)
{
  int retval;

  DBGPRINTF_MED("MP: http_ps_SetUpWeb()");

  /*
   * The default linger time for maintaining an inactive connection
   * is 30 seconds, but there is no reason MediaPlayer needs to keep
   * the connection open after downloading the clip, so reduce it
   * via the INETMGR class.
   */
  psBlk->m_pINetMgr = NULL;
  retval = ISHELL_CreateInstance(psBlk->psHttpIShell,
                                 AEECLSID_NET,
                                 (void **)&psBlk->m_pINetMgr);
  DBGPRINTF_MED("MP: Create of INetMgr returns %d", retval);

  if (psBlk->m_pINetMgr == NULL)
  {
    DBGPRINTF_HIGH("MP: psBlk->m_pINetMgr = NULL!");
  }
  else
  {
    /* The INETMGR_SetLinger() method only affects the linger time on m_pINetMgr */
    /* inside MediaPlayer.  The global default (of 30 seconds for every INETMGR  */
    /* when first created) is only changeable via an OEM config interface tool.  */
    DBGPRINTF_MED("MP: calling INETMGR_SetLinger with time %d", LINGER_TIME_SECONDS);
    default_linger_time = INETMGR_SetLinger(psBlk->m_pINetMgr, LINGER_TIME_SECONDS);
    DBGPRINTF_MED("MP: NETMGR_SetLinger returns %d", default_linger_time);
  }

  retval = ISHELL_CreateInstance(psBlk->psHttpIShell,
                                 AEECLSID_WEB,
                                 (void **)&psBlk->m_pIWeb);
  DBGPRINTF_MED("MP: Create of IWEB returns %d", retval);

  return (NULL != psBlk->m_pIWeb);
}


boolean http_ps_blockInitialize(CHttpBlock* psBlk, const char * pszStream)
{
  DBGPRINTF_MED("MP: http_ps_blockInitialize()");

  (void)http_ps_SetUpWeb(psBlk);

  psBlk->szURL = NULL;
  if ( NULL != psBlk->m_pIWeb)
  {
    /* Streaming initiated */
    psBlk->bWebPstreaming = TRUE;

    // We don't want to override the current state when 
    // we restore
    if (HttpDownloader_resuming)
    {
#ifndef  FEATURE_APP_MEDIA_VIDEO
      // Update our begin and end chunks to match what we already have in the buffer
      (void)http_ps_blockUpdate(psBlk);
#endif
    }
    else
    {
      psBlk->firstChunk = TRUE;
      psBlk->web_download_state = PS_STATE_INIT;    

      psBlk->sharedHttpBuf = NULL;
      psBlk->totalPSBufSize = 0;

      psBlk->rangeBegin = 0;
#ifdef FEATURE_APP_MEDIA_VIDEO
#error code not present
#else
      copiedHttpSize = 0;
      psBlk->rangeEnd = HTTP_GET_SIZE;
#endif
      psBlk->m_pMedia = NULL;
      psBlk->m_pKDDIblk = NULL;
    }

    if(pszStream)
    {
      int scan_ret = 0;

      psBlk->m_pKDDIblk = (CKDDIExt *)MALLOC(sizeof(CKDDIExt));
      if(!psBlk->m_pKDDIblk)
      {
        psBlk->bKDDIExt = FALSE;
        return FALSE;
      }
      psBlk->bKDDIExt = TRUE;
      /*Extract the 3gpp2 streaming parameters.*/
      Search_And_Store(psBlk->m_pUrl,pszStream,"URL=",PS_MAX_URL_LEN);
      Search_And_Store(psBlk->m_pKDDIblk->pMime,pszStream,"MIME=",MISC_3GPP2_LEN);
      Search_And_Store(psBlk->m_pKDDIblk->pCopyRight,pszStream,"COPYRIGHT=",MISC_3GPP2_LEN);
      Search_And_Store(psBlk->m_pKDDIblk->pStandBy,pszStream,"STANDBY=",MISC_3GPP2_LEN);
      Search_And_Store(psBlk->m_pKDDIblk->pDisposition,pszStream,"DISPOSITION=",MISC_3GPP2_LEN);
      Search_And_Store(psBlk->m_pKDDIblk->pDuration,pszStream,"DURATION=",MISC_3GPP2_LEN);

      Search_And_Store(psBlk->m_pKDDIblk->pSize,pszStream,"SIZE=",MISC_3GPP2_LEN);
      scan_ret = sscanf(psBlk->m_pKDDIblk->pSize,"%d",&psBlk->totalPSBufSize);
      if (scan_ret == EOF)
      {
        DBGPRINTF_HIGH("MP: sscanf returns EOF");
      }

      Search_And_Store(psBlk->m_pKDDIblk->pTitle,pszStream,"TITLE=",MISC_3GPP2_LEN);
      Search_And_Store(psBlk->m_pKDDIblk->pAC,pszStream,"AC=",MISC_3GPP2_LEN);
      Search_And_Store(psBlk->m_pKDDIblk->pProxyAddress,pszStream,"PROXYADDRESS=",MISC_3GPP2_LEN);
      Search_And_Store(psBlk->m_pKDDIblk->pProxyPort,pszStream,"PROXYPORT=",MISC_3GPP2_LEN);
      Search_And_Store(psBlk->m_pKDDIblk->pProxyName,pszStream,"PROXYNAME=",MISC_3GPP2_LEN);
      Search_And_Store(psBlk->m_pKDDIblk->pProxyPassword,pszStream,"PROXYPASSWORD=",MISC_3GPP2_LEN);
    }
    else
    {
      psBlk->bKDDIExt = FALSE;
    }
    return TRUE;
  }
  else
  {
    psBlk->bWebPstreaming = FALSE;
    psBlk->bKDDIExt = FALSE;
    return FALSE;
  }
}

void http_ps_get_size(CHttpBlock* psHttpBlock)
{
  /*Perform the HTTP HEAD request*/
  return;
}

#ifndef FEATURE_APP_MEDIA_VIDEO
boolean http_ps_blockUpdate(CHttpBlock* psBlk)
{
  if ( copiedHttpSize < psBlk->totalPSBufSize )
  {/* There is more data to download */
    bodySize = 0; /* Reset response body size*/
    psBlk->web_download_state = PS_STATE_DOWNLOAD;
    /* Set up the next byterange to be downloaded */
    psBlk->rangeBegin = copiedHttpSize;
    psBlk->rangeEnd   = psBlk->rangeBegin + HTTP_GET_SIZE;
    if(psBlk->rangeEnd >= psBlk->totalPSBufSize)
    {/* Make sure that rangeEnd is always less then total pseudo
      stream size */
      psBlk->rangeEnd = psBlk->totalPSBufSize - 1;
    }
    if(psBlk->rangeBegin <= psBlk->totalPSBufSize -1 )
    { /* make sure that rangeBegin is less total pseudo stream size*/
      return TRUE;
    }
    else
    {
      // This case should never be possible, but we should make sure that we
      // handle it.
      DBGPRINTF_MED("MP: entire clip downloaded, %d bytes", copiedHttpSize);
      psBlk->web_download_state = PS_STATE_COMPLETE;
      HttpDownloader_entireClipInBuffer = TRUE;
      return FALSE;
    }
  }
  else
  {
    DBGPRINTF_MED("MP: entire clip downloaded, %d bytes", copiedHttpSize);
    psBlk->web_download_state = PS_STATE_COMPLETE;
    HttpDownloader_entireClipInBuffer = TRUE;
    return FALSE;
  }
}
#endif // FEATURE_APP_MEDIA_VIDEO

/*===========================================================================
   This function is called to issue HTTP Partial GET request.
   rangeStart and rangeEnd specify the byte range to be downloaded from an
   HTTP 1.1 server
 ===========================================================================*/
void http_ps_get_chunk(CHttpBlock** ppsBlk)
{
  CHttpBlock* psBlk = NULL;
  unsigned int begin = 0; 
  unsigned int end = 0;
  int32 sprintf_ret = 0;

  if (NULL == ppsBlk)
  {
    DBGPRINTF_ERROR("MP: We were passed a NULL pointer in WebAction_readMore");
    return;
  }

  psBlk = *ppsBlk;

  if (NULL == psBlk)
  {
    DBGPRINTF_ERROR("MP: Our Callback data is NULL in WebAction_readMore");
    return;
  }

  DBGPRINTF_MED("MP: http_ps_get_chunk()");

  begin = psBlk->rangeBegin;
  end = psBlk->rangeEnd;

  if (HttpDownloader_suspended)
  {
    DBGPRINTF_MED("MP: we are suspended, thus exiting http_ps_get_chunk()");
    return;
  }

  /* Format a Range:bytes header to be used in HTTP GET request */
  sprintf_ret = SNPRINTF(psBlk->rangeHeader, sizeof(psBlk->rangeHeader),
                        "Range:bytes= %d-%d\r\n", begin, end);
  DBGPRINTF_MED("MP: sprintf returns %d", sprintf_ret);

  /* Intialize a callback, which should be called as soon as
  	the transaction finishes */
  CALLBACK_Init(&psBlk->m_webCallback,WebAction_GotResp,ppsBlk);

  /* Issue HTTP Partial GET request using IWEB */
  if(psBlk->bKDDIExt)
  {
    char pUrl[PS_MAX_URL_LEN];

    STRLCPY(pUrl,psBlk->m_pUrl, PS_MAX_URL_LEN);

    DBGPRINTF_MED("MP: bKDDIExt is TRUE");
    switch (psBlk->web_download_state)
    {
    case PS_STATE_INIT:
      DBGPRINTF_MED("MP: web_download_state = PS_STATE_INIT");
      break;

    case PS_STATE_DOWNLOAD:
      DBGPRINTF_MED("MP: web_download_state = PS_STATE_DOWNLOAD");
      break;

    case PS_STATE_COMPLETE:
      DBGPRINTF_MED("MP: web_download_state = PS_STATE_COMPLETE");
      break;
    case PS_STATE_ERROR:
      DBGPRINTF_HIGH("MP: web_download_state = PS_STATE_ERROR");
      break;

    default:
      DBGPRINTF_HIGH("MP: unknown web_download_state = %d", psBlk->web_download_state);
      break;
    }

    if(psBlk->web_download_state == PS_STATE_INIT)
    {
      (void)STRLCAT(pUrl,"?data=evdo-4&ac=", PS_MAX_URL_LEN);
      (void)STRLCAT(pUrl,psBlk->m_pKDDIblk->pAC, PS_MAX_URL_LEN);
      (void)STRLCAT(pUrl,"&ts=2", PS_MAX_URL_LEN);
    }
    else if(psBlk->web_download_state == PS_STATE_DOWNLOAD)
    {
      (void)STRLCAT(pUrl,"?data=evdo-4&ac=", PS_MAX_URL_LEN);
      (void)STRLCAT(pUrl,psBlk->m_pKDDIblk->pAC, PS_MAX_URL_LEN);
      (void)STRLCAT(pUrl,"&ts=3", PS_MAX_URL_LEN);
    }
    else if(psBlk->web_download_state == PS_STATE_COMPLETE ||
            psBlk->web_download_state == PS_STATE_ERROR)
    {
      (void)STRLCAT(pUrl,"?ac=", PS_MAX_URL_LEN);
      (void)STRLCAT(pUrl,psBlk->m_pKDDIblk->pAC, PS_MAX_URL_LEN);
      if(psBlk->web_download_state == PS_STATE_COMPLETE)
        (void)STRLCAT(pUrl,"&ts=4", PS_MAX_URL_LEN);
      else
        (void)STRLCAT(pUrl,"&ts=5", PS_MAX_URL_LEN);

      /*RangeHeader WEBOPT not required*/
      (void)STRLCPY(psBlk->m_pKDDIblk->pUrl,pUrl, PS_MAX_URL_LEN);
      IWEB_GetResponse(psBlk->m_pIWeb,
                       (psBlk->m_pIWeb,
                        &psBlk->m_pIWebResp,
                        &psBlk->m_webCallback,
                        psBlk->m_pKDDIblk->pUrl,
                        WEBOPT_END));
      return;
    }
    (void)STRLCPY(psBlk->m_pKDDIblk->pUrl,pUrl, PS_MAX_URL_LEN);
#ifndef FEATURE_APP_MEDIA_VIDEO
    DBGPRINTF_MED("MP: Trying to get http chunk %d - %d", begin, end);
    IWEB_GetResponse(psBlk->m_pIWeb,
                     (psBlk->m_pIWeb,
                      &psBlk->m_pIWebResp,
                      &psBlk->m_webCallback,
                      psBlk->m_pKDDIblk->pUrl,
                      WEBOPT_HEADER, psBlk->rangeHeader,
                      WEBOPT_END));
#else
#error code not present
#endif // FEATURE_APP_MEDIA_VIDEO
  }
  else{
    WebOpt woption_idle_time[3];
    int retval;

    DBGPRINTF_MED("MP: bKDDIExt is FALSE");

    woption_idle_time[0].nId = WEBOPT_IDLECONNTIMEOUT;
    woption_idle_time[0].pVal = (void *)LINGER_TIME_SECONDS;
    woption_idle_time[1].nId = WEBOPT_FLAGS;
    woption_idle_time[1].pVal = (void *)WEBREQUEST_NOKEEPALIVE;
    woption_idle_time[2].nId = WEBOPT_END;
    woption_idle_time[2].pVal = NULL;
    retval = IWEB_AddOpt(psBlk->m_pIWeb, woption_idle_time);
    DBGPRINTF_MED("MP: IWEB_AddOpt returns %d", retval);
#ifndef FEATURE_APP_MEDIA_VIDEO
    DBGPRINTF_MED("MP: Trying to get http chunk %d - %d", begin, end);
    IWEB_GetResponse(psBlk->m_pIWeb,
                     (psBlk->m_pIWeb,
                      &psBlk->m_pIWebResp,
                      &psBlk->m_webCallback,
                      psBlk->m_pUrl,
                      WEBOPT_HEADER, psBlk->rangeHeader,
                      WEBOPT_END));
#else
#error code not present
#endif // FEATURE_APP_MEDIA_VIDEO
  }
}



#ifndef FEATURE_APP_MEDIA_VIDEO
/*===========================================================================
   This function is a callback for reading HTTP GET responses in a
   pseudo streaming session. This concept is driven by the fact that
   a HTTP GET response may arrive in several chunks of HTTP payload
   data. [A full HTTP response may take several TCP segments, which may not
   reach the destination at the same time. This callback will be called
   every time a partial/full HTTP response becomes available from network ]
 ===========================================================================*/

static void WebAction_readMore(CHttpBlock** ppsBlk)
{
  CHttpBlock* psBlk = NULL;
  /* Count of bytes read in an ISOURCE_Read() operation */
  int32 nBytes = 0;

  if (NULL == ppsBlk)
  {
    DBGPRINTF_ERROR("MP: We were passed a NULL pointer in WebAction_readMore");
    return;
  }

  psBlk = *ppsBlk;

  if (NULL == psBlk)
  {
    DBGPRINTF_ERROR("MP: Our Callback data is NULL in WebAction_readMore");
    return;
  }

  if (HttpDownloader_suspended)
  {
    DBGPRINTF_MED("MP: we are suspended, thus exiting WebAction_readMore()");
    return;
  }

  do
  {
    /*Read HTTP GET response and copy the available data into
      pseudo stream shared buffer */

    nBytes = ISOURCE_Read(psBlk->m_pWR->pisMessage,
                          psBlk->sharedHttpBuf+copiedHttpSize,
                          (int32)(MIN(psBlk->totalPSBufSize-copiedHttpSize,HTTP_GET_SIZE+1)));

    switch (nBytes)
    {
    case ISOURCE_END:
      DBGPRINTF_MED("MP: ISOURCE_Read() receives ISOURCE_END");
      break;

    case ISOURCE_ERROR:
      DBGPRINTF_MED("MP: ISOURCE_Read() receives ISOURCE_ERROR");
      break;

    case ISOURCE_WAIT:
      DBGPRINTF_MED("MP: ISOURCE_Read() receives ISOURCE_WAIT");
      break;

    default:
      if (nBytes > 0)
        DBGPRINTF_MED("MP: ISOURCE_Read() read %d bytes", nBytes);
      else
        DBGPRINTF_MED("MP: ISOURCE_Read() returns unknown error %d", nBytes);
      break;
    }

    if (nBytes > 0)
    {
      /* pwin->m_BodySize is the total no of bytes received
         in a HTTP GET response. It accumulates the bytes
         read in each successful ISOURCE_Read() operation
      */
      bodySize += (uint32)nBytes;

      /*copiedHttpSize accumulates the total no of bytes received from
       HTTP server,which are also copied to PS shared buffer.*/
      copiedHttpSize += (uint32)nBytes;
    }

    /* Continue to READ while more data is available */
  }while(nBytes > 0);

  if (ISOURCE_WAIT == nBytes)
  {/* Client has to wait to receive more data from network */

    /* Function to call, when there is more data to be read from network */
    CALLBACK_Init(&psBlk->m_readMoreCallback,
                  WebAction_readMore,
                  ppsBlk);

    /* Register the above function as a callback */
    ISOURCE_Readable(psBlk->m_pWR->pisMessage,
                     &psBlk->m_readMoreCallback);

  }
  else if (ISOURCE_END == nBytes)
  {
    /* Complete HTTP GET response has been read from network and copied
       to shared buffer */
    
    DBGPRINTF_MED("MP: Finished getting http chunk %d - %d", psBlk->rangeBegin, psBlk->rangeEnd);    

    /* Clean the data structures used for this HTTP GET web transaction */
    WebAction_Stop(psBlk);

    if( PS_STATE_INIT == psBlk->web_download_state)
    { /* Client is about to open pseudo stream using Qtv */
      boolean bEventPosted = FALSE;

      /* Fill PS shared buffer pointer */
      psBlk->m_md.pData   = psBlk->sharedHttpBuf;

      /* Fill size of shared buffer */
      psBlk->m_md.dwSize  = psBlk->totalPSBufSize;

      /* Fill PS buffer class id */
      psBlk->m_md.clsData = HTTPDownloadInterface;

      /* Post EVT_CREATEPSEUDOMEDIA event to self.
      Actual media creation takes place in the
      event handler for EVT_CREATEPSEUDOMEDIA */

      psBlk->m_bufferChunk.dwSize   = copiedHttpSize;
      psBlk->m_bufferChunk.pData  = (void*) psBlk->sharedHttpBuf;

      bEventPosted = ISHELL_PostEvent(psBlk->psHttpIShell,
                                      AEECLSID_MEDIAPLAYER,
                                      EVT_CREATEPSEUDOMEDIA,
                                      0,
                                      0);
      if (bEventPosted == TRUE)
      {
        DBGPRINTF_MED("MP: event posted to create pseudo media");
      }
      else
      {
        DBGPRINTF_HIGH("MP: event NOT posted to create pseudo media");
      }
      /* return from this function */
    } /* End of if  if( PS_STATE_INIT == pwin->web_download_state) */
    else if (PS_STATE_DOWNLOAD == psBlk->web_download_state)
    { /* Client is still downloading using HTTP GETs. Pseudo stream
         initialization is already done in state PS_STATE_INIT */

      /* Downloaded Chunk size.(Complete HTTP GET response body) */
      psBlk->m_bufferChunk.dwSize = bodySize;
      psBlk->m_bufferChunk.pData  = NULL; /* Ignore this field */
      pendingHttpSize -= bodySize; /* To be downloaded data */

      DBGPRINTF_MED("MP: bodySize = %d", bodySize);
      DBGPRINTF_MED("MP: pendingHttpSize = %d", pendingHttpSize);

      if (psBlk->m_pMedia)
      {
        int retval;
        /* Update shared buffer write ptr offset using IMEDIA */
        retval = IMEDIA_SetPseudoStreamMediaData(psBlk->m_pMedia,
                                        &psBlk->m_bufferChunk);
        switch (retval)
        {
        case SUCCESS:
          DBGPRINTF_MED("MP: SUCCESS from IMEDIA_SetPseudoStreamMediaData");
          break;
        case MM_PENDING:
          DBGPRINTF_MED("MP: MM_PENDING from IMEDIA_SetPseudoStreamMediaData");
          break;
        case EBADPARM:
          DBGPRINTF_HIGH("MP: EBADPARM from IMEDIA_SetPseudoStreamMediaData");
          break;
        case EBADSTATE:
          DBGPRINTF_HIGH("MP: EBADSTATE from IMEDIA_SetPseudoStreamMediaData");
          break;
        case EUNSUPPORTED:
          DBGPRINTF_MED("MP: EUNSUPPORTED from IMEDIA_SetPseudoStreamMediaData");
          break;
        default:
          DBGPRINTF_MED("MP: unknown %d value from IMEDIA_SetPseudoStreamMediaData", retval);
          break;
        }
      }

      /* Reset response body size. Reset occurs after the completion
         of every HTTP GET transaction */
      bodySize = 0;
      if(http_ps_blockUpdate(psBlk))
      {
        /*Request the next data chunk*/
        http_ps_get_chunk(ppsBlk);
      }
      else
      {/* Download is over */
        if(psBlk->bKDDIExt)
        {
          /*Initiate Normal signal completion*/
          http_ps_get_chunk(ppsBlk);
        }
      }
    }/* End of if ( PS_STATE_DOWNLOAD == pwin->web_download_state) */
  } /* End of  if (ISOURCE_END == nBytes) */
  else if (nBytes == ISOURCE_ERROR)
  { /* A network error occured during this HTTP GET transaction */
    /* We can retry this chunk, if required. */
    /* In the current implementation, we just give up. */

    DBGPRINTF_ERROR("MP: ISOURCE_ERROR getting http chunk %d - %d, only got %d bytes", psBlk->rangeBegin, psBlk->rangeEnd, bodySize);



    // Update Qtv with the size of this partial chunk
    //psBlk->m_bufferChunk.dwSize = (uint32)nBytes;
    psBlk->m_bufferChunk.dwSize = bodySize;
    psBlk->m_bufferChunk.pData  = NULL ; /* ignore */
    pendingHttpSize -= bodySize; // To be downloaded data
    if (psBlk->m_pMedia)          
    {
      int retval;
      /* Update shared buffer write ptr offset using IMEDIA */
      retval = IMEDIA_SetPseudoStreamMediaData(psBlk->m_pMedia,
                                      &psBlk->m_bufferChunk);
      switch (retval)
      {
      case SUCCESS:
        DBGPRINTF_MED("MP: SUCCESS from IMEDIA_SetPseudoStreamMediaData");
        break;
      case MM_PENDING:
        DBGPRINTF_MED("MP: MM_PENDING from IMEDIA_SetPseudoStreamMediaData");
        break;
      case EBADPARM:
        DBGPRINTF_HIGH("MP: EBADPARM from IMEDIA_SetPseudoStreamMediaData");
        break;
      case EBADSTATE:
        DBGPRINTF_HIGH("MP: EBADSTATE from IMEDIA_SetPseudoStreamMediaData");
        break;
      case EUNSUPPORTED:
        DBGPRINTF_MED("MP: EUNSUPPORTED from IMEDIA_SetPseudoStreamMediaData");
        break;
      default:
        DBGPRINTF_MED("MP: unknown %d value from IMEDIA_SetPseudoStreamMediaData", retval);
        break;
      }
    }
    // If we did get some bytes, lets continue trying
    // Otherwise, giveup
    if (bodySize > 0)
    {
      DBGPRINTF_MED("MP: Trying to recover from ISOURCE_ERROR.  Start getting data with the first byte we did not get");
      /* Reset response body size. Reset occurs after the completion
         of every HTTP GET transaction */
      bodySize = 0;
      if(http_ps_blockUpdate(psBlk))
      {
        /*Request the next data chunk*/
        http_ps_get_chunk(ppsBlk);
      }
      else
      {/* Download is over */
        if(psBlk->bKDDIExt)
        {
          /*Initiate Normal signal completion*/
          http_ps_get_chunk(ppsBlk);
        }
      }
    }
    else
    {
      DBGPRINTF_MED("MP: Unable to recover from ISOURCE_ERROR.");
      if(psBlk->bKDDIExt)
      {
        /*Initiate Ab-Normal signal completion*/
        psBlk->web_download_state = PS_STATE_ERROR;
        http_ps_get_chunk(ppsBlk);
      }
      else
      {
        PS_CleanHttpDownload(psBlk);
        FREE(psBlk);
        *ppsBlk = NULL;
      }
    }

  } /* End of if (nBytes == ISOURCE_ERROR) */
} /* End of the function WebAction_readMore() */
#endif // FEATURE_APP_MEDIA_VIDEO

/*===========================================================================
   This function is a callback for handling HTTP GET Responses in a
   pseudo streaming session
===========================================================================*/
static void WebAction_GotResp(CHttpBlock **ppsBlk)
{
  CHttpBlock* psBlk = NULL;
  IShell*     pIShell;
  
  if (NULL == ppsBlk)
  {
    DBGPRINTF_ERROR("MP: We were passed a NULL pointer in WebAction_GotResp");
    return;
  }

  psBlk = *ppsBlk;

  if (NULL == psBlk)
  {
    DBGPRINTF_ERROR("MP: Our Callback data is NULL in WebAction_GotResp");
    return;
  }

  if (HttpDownloader_suspended)
  {
    DBGPRINTF_MED("MP: we are suspended, thus exiting WebAction_GotResp()");
    return;
  }

  DBGPRINTF_MED("MP: Got response for http chunk %d - %d", psBlk->rangeBegin, psBlk->rangeEnd);

  /* Obtain WebRespInfo structure using IWEBRESP interface */
  psBlk->m_pWR = IWEBRESP_GetInfo(psBlk->m_pIWebResp);
  if (NULL == psBlk->m_pWR || NULL == psBlk->m_pWR->pisMessage
      || psBlk->web_download_state == PS_STATE_COMPLETE
      || psBlk->web_download_state == PS_STATE_ERROR)
  {
    boolean bEventPosted = FALSE;

    if (psBlk->m_pWR == NULL)
      DBGPRINTF_MED("MP: psBlk->m_pWR is NULL");
    else if (psBlk->m_pWR->pisMessage == NULL)
      DBGPRINTF_MED("MP: psBlk->m_pWR->pisMessage is NULL");

    switch (psBlk->web_download_state)
    {
    case PS_STATE_INIT:
      DBGPRINTF_MED("MP: web_download_state = PS_STATE_INIT");
      break;
    case PS_STATE_DOWNLOAD:
      DBGPRINTF_MED("MP: web_download_state = PS_STATE_DOWNLOAD");
      break;
    case PS_STATE_COMPLETE:
      DBGPRINTF_MED("MP: web_download_state = PS_STATE_COMPLETE");
      break;
    case PS_STATE_ERROR:
      DBGPRINTF_HIGH("MP: web_download_state = PS_STATE_ERROR");
      break;
    default:
      DBGPRINTF_HIGH("MP: unknown web_download_state = %d", psBlk->web_download_state);
      break;
    }

    /*Stop the creation of this media and go to next file if in continuous mode*/
    DBGPRINTF_MED("MP: posting event to abort media creation.");
    
    pIShell = AEE_GetShell();
    bEventPosted = ISHELL_PostEvent(pIShell,
                                    AEECLSID_MEDIAPLAYER,
                                    EVT_CREATEPSEUDOMEDIA,
                                    MM_STATUS_ABORT,
                                    0);
    if (bEventPosted == TRUE)
    {
      DBGPRINTF_MED("MP: event posted to abort pseudo media creation");
    }
    else
    {
      DBGPRINTF_HIGH("MP: event NOT posted to abort pseudo media creation");
    }

    PS_CleanHttpDownload(psBlk);
    FREE(psBlk);
    *ppsBlk = NULL;
    return;
  }

  psBlk->firstChunk = FALSE;

  if (PS_STATE_INIT == psBlk->web_download_state)
  { /* State 1:
      1) Compute pseudo stream size
      2) Allocate a linear shared buffer of size computed in above step
      3) Copy received data into shared buffer
      Note: Algorithm to compute/obtain pseudo stream size is TBD

      If FEATURE_APP_MEDIA_VIDEO is enabled, we
      1) compute pseudo stream size
      2) call back into MediaPlayer to create the Media.

      Pseudo stream size can be computed by parsing "Content-Range" header
      in a partial HTTP GET response.

      Author's opinion:
      Client should obtain Pseudo stream sizefrom HTTP server, by issuing
      HTTP HEAD request. Currently, there appears to be no support for
      HTTP HEAD method in IWeb. HTTP HEAD gives a client the ability to
      obtain pseudo stream size, without actually downloading any data
    */
    
#ifdef FEATURE_APP_MEDIA_VIDEO    
#error code not present
#else
    copiedHttpSize = 0;
#endif

    /* Get total file size */
    if (HTTP_OK == psBlk->m_pWR->nCode)  /* Complete file is returned */
    {
      if (psBlk->m_pWR->lContentLength <= 0)
      {
#ifdef FEATURE_APP_MEDIA_VIDEO
#error code not present
#else
        psBlk->totalPSBufSize = HttpDownloader_maxBufferSize;
#endif
        DBGPRINTF_MED("MP: psBlk->totalPSBufSize set to max = %d, IContentLength = %d",
          psBlk->totalPSBufSize, psBlk->m_pWR->lContentLength);
      }
      else
      {
        psBlk->totalPSBufSize = (uint32)(psBlk->m_pWR->lContentLength);
        DBGPRINTF_MED("MP: psBlk->totalPSBufSize = %d", psBlk->totalPSBufSize);
      }
    }
    else
    {           
      /* Initial implementation uses 1.5 MB fixed size buffer */
      //parse the response and get the file size
#ifdef FEATURE_APP_MEDIA_VIDEO
#error code not present
#else
      int nRet; 
      nRet = (HTTP_Stream_GetContentRange( (const char *) psBlk->m_pWR->pisMessage ));
      //If there's an error, assign it the default size
      if ( nRet <= 0 )
      {
        psBlk->totalPSBufSize = HttpDownloader_maxBufferSize;
        DBGPRINTF_MED("MP: maxBufferSize = %d", HttpDownloader_maxBufferSize);
      }
      else
      {
         psBlk->totalPSBufSize = (uint32)nRet;
         DBGPRINTF_MED("MP: psBlk->totalPSBufSize = %d, IContentLength = %d",
          psBlk->totalPSBufSize, psBlk->m_pWR->lContentLength);
      }
#endif // FEATURE_APP_MEDIA_VIDEO
    }
#ifdef FEATURE_APP_MEDIA_VIDEO
#error code not present
#else
    pendingHttpSize = psBlk->totalPSBufSize;
    bodySize = 0;
    DBGPRINTF_MED("MP: pendingHttpSize = %d", pendingHttpSize);

    /* Allocate pseudo streaming shared buffer */
    if(HttpDownloader_sharedBuffer != NULL)
    {  /* Required for continuous mode pseudo streaming*/
      FREE(HttpDownloader_sharedBuffer);
      HttpDownloader_sharedBuffer = NULL;
      HttpDownloader_entireClipInBuffer = FALSE;
      DBGPRINTF_MED("MP: assigning HttpDownloader_entireClipInBuffer = FALSE");
    }
    psBlk->sharedHttpBuf = MALLOC(psBlk->totalPSBufSize);
    if(NULL == psBlk->sharedHttpBuf)
    {  /* Not enough buffer space*/
      /* Clear data structs, cancel callbacks and return to home */
      DBGPRINTF_HIGH("MP: unable to allocate enough memory for file, stopping play.");
      PS_CleanHttpDownload(psBlk);
      FREE(psBlk);
      *ppsBlk = NULL;
      return;
    }
    /* Initialize HttpDownloader_sharedBuffer, will free it in CPlayerWin_Delete(). */
    HttpDownloader_sharedBuffer = psBlk->sharedHttpBuf;
#endif // FEATURE_APP_MEDIA_VIDEO
  }

  /* Read from network. If any data is available, copy to shared buffer */
#ifndef FEATURE_APP_MEDIA_VIDEO
  WebAction_readMore(ppsBlk);
#endif // FEATURE_APP_MEDIA_VIDEO

}

#ifndef FEATURE_APP_MEDIA_VIDEO
/*===========================================================================

   This function is called when parsing an HTTP stream.  It advances
   the str pointer to point to the next "string" by skipping the null character.
===========================================================================*/

static const char* HTTP_Stream_next_string(const char *str)
{
  const char *s;
  s = str;
  for( s = str; *s++; );
  return( s );
}
/*===========================================================================

   This function is called to parse an HTTP stream for the "Content-Range" tag.
   It returns the total size of the downloaded file.
===========================================================================*/

static int HTTP_Stream_GetContentRange( const char *str )
{
  int search_limit = 1000; //We'll search the first 1000 bytes of the stream
  const char * pOrgStr = str;
  const char * ptr = str;
  int end = 0, start = 0, filesize = 0;
  while( ( str - pOrgStr ) < search_limit )
  {
    ptr = strstr( str, "Content-Range" );
    if( ptr != NULL )
    {
      int scan_ret = 0;
      DBGPRINTF_MED("MP: sizing content: %s", str);
      //We found it.  Now get the total file size.  Here's an example of the Content-Range string:
      //Content-Range\0 bytes 0-10000/550120\r\n
      // Skip past "Content-Range: "
      ptr += 15;
      // skip past any other white space
      ptr = strstr( ptr, "bytes" );
      if (ptr == NULL)
      {
        DBGPRINTF_HIGH("MP: Can'f find size, incorrectly formatted stream");
        return -1; //Incorrectly formatted stream
      }
      scan_ret = sscanf( ptr, "bytes %d-%d/%d", &start, &end, &filesize );
      if (scan_ret == EOF)
      {
        DBGPRINTF_HIGH("MP: sscanf returns EOF");
      }
      DBGPRINTF_MED("MP: HTTP_Stream_GetContentRange returns %d", filesize);
      return filesize;
    }
    str = HTTP_Stream_next_string( str );
  }
  DBGPRINTF_HIGH("MP: incorrectly formatted stream");
  return -1;//Incorrectly formatted stream
}
#endif //FEATURE_APP_MEDIA_VIDEO

#endif //defined(FEATURE_APP_MPEG4) && (defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))

