/*===========================================================================
FILE: HttpDownloader.h

SERVICES: Header file for code for downloading media clip data via network.

DESCRIPTION
  This file contains structure and other definitions for downloading media
  clips via HTTP.

       Copyright  2000-2005 QUALCOMM Incorporated.
                    All Rights Reserved.
                   QUALCOMM Proprietary/GTDR

$Header: //depot/asic/msmshared/apps/MediaPlayer/HttpDownloader.h#17 $

===========================================================================*/

#ifndef HTTP_DOWNLOADER_H
#define HTTP_DOWNLOADER_H

#define PS_MAX_URL_LEN 255

#if defined(FEATURE_APP_MPEG4) && (defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))

#ifdef AEE_STATIC
#include "OEMFeatures.h"
#endif
#include "AEEAppGen.h"        // AEEApplet structure and Applet services
#include "MediaPlayer.bid"
#include "mediaplayer_res.h"

// AEE Services used by app
#include "AEE_OEM.h"
#include "AEEStdLib.h"        // AEE Stb Lib Services
#include "AEEMedia.h"         // AEE Multimedia Services
#include "AEEMediaFormats.h"
#include "AEEMimeTypes.h"     // AEE MIME definitions

#include "AEEMediaUtil.h"     // AEE Media Utility functions

#include "AEEWeb.h"
#include "AEENet.h"
#include "AEEMediaMPEG4.h"
#ifndef IMEDIA_GetPlaybackPositions
#include "OEMMediaMPEG4.h"
#endif //IMEDIA_GetPlaybackPositions

#define EVT_CREATEPSEUDOMEDIA (EVT_USER + 4)

/*
3GPP2- KDDI Extention block:
The Multimedia Object Property String properties shall always be arranged in the following sequence:
URL, MIME, COPYRIGHT, STANDBY, DISPOSITION, DURATION, SIZE, TITLE, AC, PROXYADDRESS, PROXYPORT,
PROXYNAME, and PROXYPASSWORD
*/
#define MISC_3GPP2_LEN 64

typedef struct CKDDIExt{
	char pUrl[PS_MAX_URL_LEN];
	char pMime[MISC_3GPP2_LEN];
	char pCopyRight[MISC_3GPP2_LEN];
	char pStandBy[MISC_3GPP2_LEN];
	char pDisposition[MISC_3GPP2_LEN];
	char pDuration[MISC_3GPP2_LEN];
	char pSize[MISC_3GPP2_LEN];
	char pTitle[MISC_3GPP2_LEN];
	char pAC[MISC_3GPP2_LEN];
	char pProxyAddress[MISC_3GPP2_LEN];
	char pProxyPort[MISC_3GPP2_LEN];
	char pProxyName[MISC_3GPP2_LEN];
	char pProxyPassword[MISC_3GPP2_LEN];
}CKDDIExt;

#define PS_STATE_INIT 1
#define PS_STATE_DOWNLOAD 2
#define PS_STATE_COMPLETE 3
#define PS_STATE_ERROR 4

typedef struct CHttpBlock
{
	IWeb *               m_pIWeb; /* IWeb interface ptr */
	WebRespInfo *        m_pWR;
	IWebResp *           m_pIWebResp; /* IWebResp interface ptr */
	INetMgr *            m_pINetMgr;
	char                 m_pUrl[PS_MAX_URL_LEN]; /* The url to pseudo-stream from */
	char                 rangeHeader[64];
	IShell *             psHttpIShell;
	ISource  *           m_pIsource; /* ISource to read data */
	AEECallback          m_webCallback; /* Web callback function */
	AEECallback          m_readMoreCallback; /* ISOURCE_Readable() callback */
	//uint32  m_copiedHttpSize; /* Size of downloaded data so far*/
	int web_download_state;
	uint32 totalPSBufSize;
	char *   sharedHttpBuf;
	boolean  bWebPstreaming;
	unsigned int rangeBegin;
	unsigned int rangeEnd;
	boolean firstChunk;
        char *   szURL; /* filename, used by content handler url mode */

	/*3gpp2 - KDDI ext.*/
	boolean bKDDIExt;
	CKDDIExt * m_pKDDIblk;
	/* Media data structures */
	AEEMediaData      m_md;
	IMedia *          m_pMedia;         // IMedia-based object
  AEEMediaDataBufferChunk m_bufferChunk;       // buffer chunk

}CHttpBlock;

#endif  // FEATURE_APP_MPEG4

#endif  // HTTP_DOWNLOADER_H
