#include "AEE_OEM.h"
#include "AEEFile.h"
#include "AEEConfig.h"
#include "AEECard.h"
#include "CARD.BID"
#include "AEECARDSESSION_NOTIFIER.BID"
#include "AEECARDSESSION.BID"
#include "CARD_MODEL.bid"
#include "OEMFS.h"
#include "Ui.h"			// for  AEE_APP_SIG		

#include "OEMOMH.h"
#include "gsdi.h"

#ifdef FEATURE_OEMOMH

#define OMH_DOWNLOAD_INFO_FILE	"fs:/sys/omhdownloadinfo.dat"
#define BREW_DLOAD_SIZE 		207
#define BREW_AEP_SIZE			1
#define BREW_SID_SIZE			32

#ifdef FEATURE_APP_SVCPRG
int OEM_GetCachedConfig(AEEConfigItem i, void * pBuff, int nSize);
#endif  //def FEATURE_APP_SVCPRG

typedef struct _BREWDLOADEF
{
	uint32		cid;
	uint8		akey[DL_AKEY_SIZE];
	char 		ads[DL_MAX_SERVER];
	uint16		flags;
	uint8		apolicy;
	char			reserved[128];
} BREWDLOADEF;

typedef enum 
{
	READ_NONE,
	READ_BREW_AEP_EF,
	READ_BREW_SID_EF,
	READ_BREW_DLOAD_EF
} ReadType;

typedef struct _OEMOMHConfig
{
	ICard*					m_pICard;
	ICardSession*			m_pICardSession;
	ICardSessionNotifier *	m_pICardNotifier;
	ModelListener			m_tListener;

	// the download infor read from the omh ruim card
	AEEDownloadInfo			m_tDldInfo;

	// is the card ready
	boolean 				m_bOMHCardReady;

	//callback
	AEECallback				m_cbRead;
	AEECallback				m_cbRegister;

	BREWDLOADEF *			m_pBREWDLOADEF;
	uint8					m_nAEP;
	char 					m_pSID[BREW_SID_SIZE];
	ReadType				m_eReadType;	
	boolean					m_bExiting;

	AEECardSessionReadTpStatus	*m_pReadStatus;
	AEECardSessionCmdStatus 	m_tCmdStatus;
}OEMOMHConfig;

extern gsdi_uim_omh_cap_type gsdi_uim_omh_cap;

 OEMOMHConfig omhConfig;
 int OEMOMH_SaveInfo(void);
 int OEMOMH_DeleteApps(void);
 void OEMOMH_Reset(void);
 void OEMOMH_CleanupAndReset(void);
 int OEMOMH_ReadBREWEFs(void);


#define OMH_DIF_USE_A_KEY		0x0001
//#define OMH_DIF_TEST_ALLOWED
#define OMH_DIF_MIN_FOR_SID		0x0002
#define OMH_DIF_PREPAY			0x0004
#define OMH_DIF_NO_AUTO_ACK		0x0008
#define OMH_DIF_SID_ENCODE		0x0010
#define OMH_DIF_SID_VALIDATE_ALL	0x0020
#define OMH_DIF_RUIM_DEL_OVERRIDE	0x0040
#define OMH_DIF_AUTO_UPGRADE	0x0100
#define OMH_DIF_NO_LAUNCH_MOD_ACK_DISABLED	0x0200
//#define OMH_DIF_SENSITIVE_SID
uint32* cfgi_len = 0;
 void OEMOMH_RegisterCb(OEMOMHConfig *po)
{
	DBGPRINTF("in register callback");
	// Now client ID can be valid
}
 void OEMOMH_InitBREWDownload()
{
	IDownload *pIDownload = 0;
	IConfig*   pConfig   = NULL;

	DBGPRINTF("OEMOMH_InitBREWDownload >>");
	ISHELL_CreateInstance(AEE_GetShell(), AEECLSID_DOWNLOAD, (void **)&pIDownload);
	ISHELL_CreateInstance(AEE_GetShell(), AEECLSID_CONFIG, (void**)&pConfig);

	if (pIDownload && pConfig)
	{
		DBGPRINTF("OEMOMH_InitBREWDownload_10");
		if (omhConfig.m_bOMHCardReady)
		{
			AEEMobileInfo MobileInfo;
			char * pSID=NULL;
			int nResult = AEE_SUCCESS;

			ADSInfo m_adsInfo;
			boolean bRet;

			DBGPRINTF("OEMOMH_InitBREWDownload_20 SID %s", omhConfig.m_pSID);
			m_adsInfo.dwCarrierID = omhConfig.m_tDldInfo.dwCarrierID;
			m_adsInfo.dwPlatformID = omhConfig.m_tDldInfo.dwPlatformID;
			m_adsInfo.dwAuth = omhConfig.m_tDldInfo.nAuth;
			m_adsInfo.wFlags = omhConfig.m_tDldInfo.wFlags;
			STRNCPY(m_adsInfo.szServer, "cool.qualcomm.com", DL_MAX_SERVER);
			bRet = IDOWNLOAD_SetADS(pIDownload, & m_adsInfo); // force update
			STRNCPY(m_adsInfo.szServer, (char*)omhConfig.m_tDldInfo.szServer, DL_MAX_SERVER);
            DBGPRINTF("OEMOMH_InitBREWDownload_20_1 %s", m_adsInfo.szServer);
			bRet = IDOWNLOAD_SetADS(pIDownload, & m_adsInfo);

    		if (omhConfig.m_tDldInfo.wFlags & DIF_MIN_FOR_SID)
			{
				//Get the current CFGI_MOBILEINFO
				nResult=ICONFIG_GetItem(pConfig, CFGI_MOBILEINFO,&MobileInfo, sizeof(MobileInfo));
				if(nResult == AEE_SUCCESS)
				{
				     pSID = STRDUP(MobileInfo.szMobileID);
                     //Added this new statement                                 
                     STRLCPY(omhConfig.m_pSID, MobileInfo.szMobileID, 15+1);
				     DBGPRINTF("OEMOMH_InitBREWDownload_30 SID %s", pSID);
                }
            	else
                {
                	 DBGPRINTF("Could not get MOBILEINFO %d", nResult);
                }
            }
		    else if(STRLEN(omhConfig.m_pSID))
			{
				DBGPRINTF("OEMOMH_InitBREWDownload_20 SID %s", omhConfig.m_pSID);
				pSID = STRDUP(omhConfig.m_pSID);
			}
			else
			{
				DBGPRINTF("OEMOMH_InitBREWDownload_20 SID is NULL");
			}

			if (pSID)
			{
				IDOWNLOAD_SetSubscriberID(pIDownload, pSID, STRLEN(pSID));
			}
			FREEIF(pSID);
		}
		IDOWNLOAD_Release(pIDownload);
		ICONFIG_Release(pConfig);

	}
	DBGPRINTF("<< OEMOMH_InitBREWDownload");
}

 void OEMOMH_ReadCb(OEMOMHConfig *po)
{
	uint32           nCmdStatus;

	DBGPRINTF("OEMOMH_ReadCb >>");
	if(NULL == po) 
	{
		DBGPRINTF("OEMOMH_ReadCb_10 omhConfig is NULL");
		return;
	}
	nCmdStatus = omhConfig.m_pReadStatus->nCmdStatus;
	
	omhConfig.m_bOMHCardReady = FALSE;
	if (nCmdStatus == SUCCESS)
	{
		DBGPRINTF("OEMOMH_ReadCb_20");
		if (omhConfig.m_eReadType == READ_BREW_DLOAD_EF)
		{	
			BREWDLOADEF *pEF = omhConfig.m_pBREWDLOADEF;			
			pEF->cid = NTOHL(pEF->cid);

			// Fix the mapping from CDG 166 R-UIM to BREW download flag
			{
				uint16 tmpFlag = 0;
				uint16 tmp = pEF->flags;

				tmpFlag |= (tmp & OMH_DIF_USE_A_KEY) ? DIF_USE_A_KEY : 0;
				tmpFlag |= (tmp & OMH_DIF_MIN_FOR_SID) ? DIF_MIN_FOR_SID : 0;
				tmpFlag |= (tmp & OMH_DIF_PREPAY) ? DIF_PREPAY : 0;
				tmpFlag |= (tmp & OMH_DIF_NO_AUTO_ACK) ? DIF_NO_AUTO_ACK : 0;
// DIF_SID_ENCODE must be not set
				//tmpFlag |= (tmp & OMH_DIF_SID_ENCODE) ? DIF_SID_ENCODE : 0;
				tmpFlag |= (tmp & OMH_DIF_SID_VALIDATE_ALL) ? DIF_SID_VALIDATE_ALL : 0;
				tmpFlag |= (tmp & OMH_DIF_RUIM_DEL_OVERRIDE) ? DIF_RUIM_DEL_OVERRIDE : 0;
				tmpFlag |= (tmp & OMH_DIF_AUTO_UPGRADE) ? DIF_AUTO_UPGRADE : 0;
				tmpFlag |= (tmp & OMH_DIF_NO_LAUNCH_MOD_ACK_DISABLED) ? DIF_NO_LAUNCH_MOD_ACK_DISABLED : 0;
				pEF->flags = tmpFlag;
			}

			DBGPRINTF("read flags 0x%x", pEF->flags);

			if (pEF->cid > 0 && pEF->ads[0] != '\0')
			{
				BREWDLOADEF *pEF = omhConfig.m_pBREWDLOADEF;
				uint32 lastCID = omhConfig.m_tDldInfo.dwCarrierID; //  bCIDChanged = (pEF->cid != omhConfig.m_tDldInfo.dwCarrierID);
				AEEDownloadInfo aDefaultInfo;
				
				DBGPRINTF("OEMOMH_ReadCb_40");

				omhConfig.m_bOMHCardReady = TRUE;
				omhConfig.m_tDldInfo.dwCarrierID  = pEF->cid;			     	
				STRNCPY((char*)omhConfig.m_tDldInfo.szServer,pEF->ads, DL_MAX_SERVER);
				MEMCPY((void *)omhConfig.m_tDldInfo.bAKey, pEF->akey, DL_AKEY_SIZE);					
				omhConfig.m_tDldInfo.wFlags       = pEF->flags;
				omhConfig.m_tDldInfo.nAuth        = pEF->apolicy;
				omhConfig.m_eReadType = READ_NONE;

				// retrieve the default info parameters
				OEM_GetCachedConfig(CFGI_DOWNLOAD, (void *)&aDefaultInfo, sizeof(AEEDownloadInfo));

				// copy the BKEy and platform id 
				omhConfig.m_tDldInfo.dwPlatformID = aDefaultInfo.dwPlatformID;
				MEMCPY((void *)omhConfig.m_tDldInfo.bBKey, (void *)aDefaultInfo.bBKey, DL_BKEY_SIZE);

				if (lastCID != pEF->cid)
				{
					DBGPRINTF("OEMOMH_ReadCb_50 CID changed old %d new %d", lastCID, pEF->cid);
					OEMOMH_SaveInfo();

					// if we are resetting, no need to read more
					if (OEMOMH_DeleteApps())
					{
						DBGPRINTF("OEMOMH_ReadCb_60");
						OEMOMH_Cleanup();
						OEMOMH_Reset();
						return;
					}
				}

				// continue to read EFs BREWAEP and BREWSID
				omhConfig.m_pReadStatus->pReadData->nLen = BREW_AEP_SIZE;
				omhConfig.m_pReadStatus->pReadData->pData = (uint8*)&omhConfig.m_nAEP;
				omhConfig.m_eReadType = READ_BREW_AEP_EF;
				nCmdStatus = ICARDSESSION_ReadTransparent(omhConfig.m_pICardSession,  AEECARDSESSION_CDMA_BREW_AEP, 
					0, BREW_AEP_SIZE, omhConfig.m_pReadStatus, &omhConfig.m_cbRead);
				if (nCmdStatus != SUCCESS)
				{
					DBGPRINTF("unable to read BREW AEP EF");
				}					
			}		
		}
		else if (omhConfig.m_eReadType == READ_BREW_AEP_EF)
		{						
			DBGPRINTF("OEMOMH_ReadCb_70");
			omhConfig.m_tDldInfo.nPolicy = omhConfig.m_nAEP;
			
			omhConfig.m_pReadStatus->pReadData->nLen = BREW_SID_SIZE;
			omhConfig.m_pReadStatus->pReadData->pData = (uint8*)omhConfig.m_pSID;
			omhConfig.m_eReadType = READ_BREW_SID_EF;
			nCmdStatus = ICARDSESSION_ReadTransparent(omhConfig.m_pICardSession,  AEECARDSESSION_CDMA_BREW_SID, 
					0, 0, omhConfig.m_pReadStatus, &omhConfig.m_cbRead);
			if (nCmdStatus != SUCCESS)
			{
				DBGPRINTF("unable to read BREW SID EF");
			}						
		}
		else if (omhConfig.m_eReadType == READ_BREW_SID_EF)
		{										
			DBGPRINTF("OEMOMH_ReadCb_80");
			omhConfig.m_bOMHCardReady = TRUE;						
			omhConfig.m_eReadType = READ_NONE;

			// Save download flag and SID
			OEMOMH_SaveInfo();
			// update BREW cache and we are done, apps are deleted 
			// when the DLOAD EF is read and a CID changed is detected
			OEMOMH_InitBREWDownload();
			OEMOMH_Cleanup();
		}
	}
	else
	{
		DBGPRINTF("OEMOMH_ReadCb_90");
		// Treat it as OMH card without BREW enabled
		OEMOMH_CleanupAndReset();
	}
	DBGPRINTF("<< OEMOMH_ReadCb");
}

 void OEMOMH_CleanupAndReset()
{
	IFileMgr *pIFileMgr = 0;
	AEEDownloadInfo aDefaultInfo;

	DBGPRINTF("OEMOMH_CleanupAndReset >>");
	OEM_GetCachedConfig(CFGI_DOWNLOAD, (void *)&aDefaultInfo, sizeof(AEEDownloadInfo));

	// remove the fs:/sys/omhdownloadinfo.dat since we did not see an OMH card
	ISHELL_CreateInstance(AEE_GetShell(), AEECLSID_FILEMGR, (void **)&pIFileMgr);		
	IFILEMGR_Remove(pIFileMgr, OMH_DOWNLOAD_INFO_FILE);
	IFILEMGR_Release(pIFileMgr);

	OEMOMH_Cleanup();
	if (omhConfig.m_tDldInfo.dwCarrierID != aDefaultInfo.dwCarrierID)
	{
		DBGPRINTF("OEMOMH_CleanupAndReset_10");
		if (OEMOMH_DeleteApps())
		{
			DBGPRINTF("OEMOMH_CleanupAndReset_20");
			OEMOMH_Reset();
		}
	}
	DBGPRINTF("<< OEMOMH_CleanupAndReset");
}

 int OEMOMH_ReadBREWEFs()
{
	int nErr = AEE_SUCCESS;
	omhConfig.m_bOMHCardReady = FALSE;

	DBGPRINTF("OEMOMH_ReadBREWEFs >>");
	omhConfig.m_pReadStatus= (AEECardSessionReadTpStatus *)MALLOC(sizeof(AEECardSessionReadTpStatus));
	if(NULL == omhConfig.m_pReadStatus) 
	{
		nErr = ENOMEMORY;
		return nErr;
	}

	omhConfig.m_pReadStatus->pReadData = (AEECardSessionData *)MALLOC(sizeof(AEECardSessionData));
	if(NULL == omhConfig.m_pReadStatus->pReadData) 
	{
		nErr = ENOMEMORY;
		return nErr;
	}	
	omhConfig.m_pReadStatus->pReadData->nLen = BREW_DLOAD_SIZE;
	omhConfig.m_pBREWDLOADEF = MALLOC(BREW_DLOAD_SIZE);
	omhConfig.m_pReadStatus->pReadData->pData = (uint8*)omhConfig.m_pBREWDLOADEF;
	omhConfig.m_eReadType = READ_BREW_DLOAD_EF;

	nErr = ICARDSESSION_ReadTransparent(
		omhConfig.m_pICardSession, 
		AEECARDSESSION_CDMA_BREW_DLOAD, 
		0,
		BREW_DLOAD_SIZE,
		omhConfig.m_pReadStatus, 
		&omhConfig.m_cbRead
	);
	
	if (nErr != SUCCESS)
	{
		DBGPRINTF("ICARDSESSION_ReadTransparent failed");
	}
	else 
	{
		DBGPRINTF("ICARDSESSION_ReadTransparent SUCCESS");
	}

	DBGPRINTF("<< OEMOMH_ReadBREWEFs %d", nErr);
	return nErr;
}

int OEMOMH_RmDir(const char *dirName)
{
	int nErr = SUCCESS;
	IShell* pIShell = AEE_GetShell();
	IFileMgr * pIFileMgr = 0;
	FileInfo aInfo;

	nErr = ISHELL_CreateInstance(pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr);
	DBGPRINTF("OEMOMH_RmDir >> %d", nErr);

	// Get rid of the directories first
   	while (TRUE)
	{
		if ((nErr = IFILEMGR_EnumInit(pIFileMgr, dirName, TRUE)) == SUCCESS)
		{
			if (IFILEMGR_EnumNext(pIFileMgr, &aInfo))
			{
				nErr = OEMOMH_RmDir(aInfo.szName);
				//DBGPRINTF("OEMOMH_RmDir_10 RmStat %d", nErr);
			}
			else
			{
				//DBGPRINTF("OEMOMH_RmDir_20 EnumNext %d", nErr);
				break;
			}
		}
		else
		{
			//DBGPRINTF("OEMOMH_RmDir_30 EnumInit %d", nErr);
			break;
		}
   	}

	// Get rid of the files now
   	while (TRUE)
	{
		if ((nErr = IFILEMGR_EnumInit(pIFileMgr, dirName, FALSE)) == SUCCESS)
		{
			if (IFILEMGR_EnumNext(pIFileMgr, &aInfo))
			{
				nErr = IFILEMGR_Remove(pIFileMgr, aInfo.szName);
				//DBGPRINTF("OEMOMH_RmDir_40 RmStat %d", nErr);
			}
			else
			{
				//DBGPRINTF("OEMOMH_RmDir_50 EnumNext %d", nErr);
				break;
			}
		}
		else
		{
			//DBGPRINTF("OEMOMH_RmDir_60 EnumInit %d", nErr);
			break;
		}
   	}

	//DBGPRINTF("OEMOMH_RmDir_70 RmDir %s", dirName);
	nErr = IFILEMGR_RmDir(pIFileMgr, dirName);
	//DBGPRINTF("OEMOMH_RmDir_80 RmDirStat %d", nErr);

	IFILEMGR_Release(pIFileMgr);
	return nErr;
}

 int OEMOMH_DeleteModule(const char * moduleId)
{
	int nErr = AEE_SUCCESS;
	char dirName[MAX_FILE_NAME];

	DBGPRINTF("OEMOMH_DeleteModule >>");
	SNPRINTF(dirName, MAX_FILE_NAME, "%s%s", AEEFS_MOD_DIR, moduleId);
	DBGPRINTF("%s", dirName);

	OEMOMH_RmDir(dirName);
	DBGPRINTF("<< OEMOMH_DeleteModule");
	return nErr;
}

 void OEMOMH_Reset()
{
	DBGPRINTF("OEMOMH_Reset >>");
	omhConfig.m_bExiting = TRUE;

	// Reset handset
	ISHELL_Reset(AEE_GetShell(), AEERESET_HARD);
	DBGPRINTF("<< OEMOMH_Reset");
}

#define	MCF_PICTURE_DIR			10888 
#define	MCF_MUSIC_LOCK_DIR		10889 
#define	MCF_MOVIE_DIR			10890 
#define	MCF_THEME_DIR			10891 
#define	MCF_TEXT_DIR			10892 
#define	MCF_SOUND_LOCK_DIR		16225 
#define	MCF_SOUND_DIR			18067 
#define	MCF_LICENSE_LOCK_DIR	19240 
#define	MCF_THEME_LOCK_DIR		19917 
#define	MCF_TEXT_LOCK_DIR		19918 
#define	MCF_PICTURE_LOCK_DIR	19919 
#define	MCF_MUSIC_DIR			19920 
#define	MCF_MOVIE_LOCK_DIR		19922 
#define	MCF_LINK_LOCK_DIR		19923 
#define	MCF_LINK_DIR			19924 

int OEMOMH_DeleteApps()
{
	// find all the module in the fs:/brew/mif 
	uint32 moduleId = 0;
	int nErr = AEE_SUCCESS;
	boolean	bReset = FALSE;
	boolean	bContinue = TRUE;
	IShell* pIShell = AEE_GetShell();
	IFileMgr * pIFileMgr = 0;
	FileInfo aInfo;
	// find all the module in the fs:/brew/mif 

	DBGPRINTF("OEMOMH_DeleteApps >>");
	nErr = ISHELL_CreateInstance(pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr);
   	while (bContinue)
	{
		nErr = IFILEMGR_EnumInit(pIFileMgr, AEEFS_MIF_DIR, FALSE);
		//DBGPRINTF("OEMOMH_DeleteApps_10");
		while(bContinue = IFILEMGR_EnumNext(pIFileMgr, &aInfo))
		{
			int len = 0;
			char *tmp = STRRCHR(aInfo.szName, '/') + 1;
			char buf[MAX_FILE_NAME];

			//DBGPRINTF("OEMOMH_DeleteApps_20");
			len = STRLEN(tmp);
			STRNCPY(buf, tmp, len -4);	// the .mif extension
			buf[len-4] = 0;		
			moduleId = ATOI(buf);	// check to see if it is a module with numbers

			// Skip MCF directories
			if (MCF_PICTURE_DIR == moduleId ||
				MCF_MUSIC_LOCK_DIR == moduleId ||
				MCF_MOVIE_DIR == moduleId ||
				MCF_THEME_DIR == moduleId ||
				MCF_TEXT_DIR == moduleId ||
				MCF_SOUND_LOCK_DIR == moduleId ||
				MCF_SOUND_DIR == moduleId ||
				MCF_LICENSE_LOCK_DIR == moduleId ||
				MCF_THEME_LOCK_DIR == moduleId ||
				MCF_TEXT_LOCK_DIR == moduleId ||
				MCF_PICTURE_LOCK_DIR == moduleId ||
				MCF_MUSIC_DIR == moduleId ||
				MCF_MOVIE_LOCK_DIR == moduleId ||
				MCF_LINK_LOCK_DIR == moduleId ||
				MCF_LINK_DIR == moduleId)
				continue;

			if (moduleId > 0)
			{
				// we find a dynamic module
				DBGPRINTF("OEMOMH_DeleteApps_30 %s", buf);
				nErr = OEMOMH_DeleteModule(buf);
				if (AEE_SUCCESS != nErr)
					continue;

				// remove the mif file
				nErr = IFILEMGR_Remove(pIFileMgr, aInfo.szName);
				DBGPRINTF("OEMOMH_DeleteApps_40 %d", nErr);
				if (AEE_SUCCESS != nErr)
					continue;
				bReset = TRUE;
				break; // start enum operation again
			}
		}
	}
	DBGPRINTF("OEMOMH_DeleteApps_50");

	OEMOMH_RmDir("fs:/sys/download/");

	IFILEMGR_Release(pIFileMgr);

	DBGPRINTF("<< OEMOMH_DeleteApps");
	return bReset;
}

int OEMOMH_LoadSaved()
{
	int nErr = AEE_SUCCESS;
	IShell *pIShell = AEE_GetShell();
	IFile* pFile = 0;	
	IFileMgr *pIFileMgr = 0;
	int lenSID = 0;

	DBGPRINTF("OEMOMH_LoadSaved >>");
	nErr = ISHELL_CreateInstance(pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr);
	if (!nErr)
	{
		pFile = IFILEMGR_OpenFile(pIFileMgr, OMH_DOWNLOAD_INFO_FILE, _OFM_READ);
		if(pFile)
		{
			/* Read from File to AEEDownloadInfo struct */
			if(IFILE_Read(pFile, (void*)&omhConfig.m_tDldInfo, sizeof(AEEDownloadInfo)) != sizeof(AEEDownloadInfo))
			{               		
				nErr = EFAILED;				
				/* Read failed for some reason. Destroy the file and re-write. */
				IFILEMGR_Remove(pIFileMgr, OMH_DOWNLOAD_INFO_FILE);
			}
            
			if(IFILE_Read(pFile, (void*)&lenSID, sizeof(lenSID)) != sizeof(lenSID))
			{               		
				nErr = EFAILED;				
				IFILEMGR_Remove(pIFileMgr, OMH_DOWNLOAD_INFO_FILE);
			}
			if(lenSID > 0 && IFILE_Read(pFile, (void*)omhConfig.m_pSID, lenSID) != lenSID)
			{               		
				nErr = EFAILED;				
				IFILEMGR_Remove(pIFileMgr, OMH_DOWNLOAD_INFO_FILE);
			}


			IFILE_Release(pFile);
		}
		else
		{
			// file does not exist, we must have bee using default setting last time
			OEM_GetCachedConfig(CFGI_DOWNLOAD, (void *)&omhConfig.m_tDldInfo, sizeof(AEEDownloadInfo));

			nErr = EFAILED;				

		}
	}

	IFILEMGR_Release(pIFileMgr);
	DBGPRINTF("<< OEMOMH_LoadSaved");
	return nErr;
}

int OEMOMH_SaveInfo()
{
	int nErr = AEE_SUCCESS;
	IShell *pIShell = AEE_GetShell();
	IFile* pFile = 0;	
	IFileMgr * pIFileMgr = 0;
    int lenSID = 0;

	DBGPRINTF("OEMOMH_SaveInfo >>");
	nErr = ISHELL_CreateInstance(pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr);
	if (!nErr)
	{
 		IFILEMGR_Remove(pIFileMgr, OMH_DOWNLOAD_INFO_FILE);
		pFile = IFILEMGR_OpenFile(pIFileMgr, OMH_DOWNLOAD_INFO_FILE, _OFM_CREATE);
		if(pFile)
		{
			/* Read from File to AEEDownloadInfo struct */
			if(IFILE_Write(pFile, (void*)&omhConfig.m_tDldInfo, sizeof(AEEDownloadInfo)) != sizeof(AEEDownloadInfo))
			{               		
				nErr = EFAILED;				
				/* Read failed for some reason. Destroy the file and re-write. */
				IFILEMGR_Remove(pIFileMgr, OMH_DOWNLOAD_INFO_FILE);
			}
            
			if((omhConfig.m_pSID[0] != 0) && (!(omhConfig.m_tDldInfo.wFlags & DIF_MIN_FOR_SID)))
			{
				lenSID = STRLEN(omhConfig.m_pSID) + 1;
			}
			// 
			// if DIF_MIN_FOR_SID is true, set the length as that of IMSI
			//
			else //IMSI is always 15 bytes length
			{
				lenSID = 15 + 1;
			}
			if(IFILE_Write(pFile, (void*)&lenSID, sizeof(lenSID)) != sizeof(lenSID))
			{               		
				nErr = EFAILED;				
				IFILEMGR_Remove(pIFileMgr, OMH_DOWNLOAD_INFO_FILE);
			}
			if(lenSID > 0 && IFILE_Write(pFile, (void*)omhConfig.m_pSID, lenSID) != lenSID)
			{               		
				nErr = EFAILED;				
				IFILEMGR_Remove(pIFileMgr, OMH_DOWNLOAD_INFO_FILE);
			}

			IFILE_Release(pFile);
		}
		else
		{
			// unable to create file
			nErr = EFAILED;
		}
	}  
	IFILEMGR_Release(pIFileMgr);
	DBGPRINTF("<< OEMOMH_SaveInfo");
	return nErr;
}


int OEMOMH_Init()
{
	int nErr = AEE_SUCCESS;
	IShell *pIShell = AEE_GetShell();

	DBGPRINTF("OEMOMH_Init >>");
	MEMSET(&omhConfig, 0, sizeof(OEMOMHConfig));
	nErr = ISHELL_CreateInstance(pIShell, AEECLSID_CARD, (void **)&omhConfig.m_pICard);		
	if (!nErr)
	{
		DBGPRINTF("OEMOMH_Init_10");
		nErr = ISHELL_CreateInstance(pIShell, AEECLSID_CARDSESSION, (void **)&omhConfig.m_pICardSession);
		if (!nErr)
		{
			DBGPRINTF("OEMOMH_Init_20");
			CALLBACK_Init(&omhConfig.m_cbRegister, (PFNNOTIFY)OEMOMH_RegisterCb, (void*)(&omhConfig));

			// ICARDSESSION_Register is async operation for obtaining client ID
			// Client ID can only be valid after OEMOMH_RegisterCb is invoked
			ICARDSESSION_Register(omhConfig.m_pICardSession, &omhConfig.m_tCmdStatus, &omhConfig.m_cbRegister);
		}
	}

	if (!nErr)
	{				
		DBGPRINTF("OEMOMH_Init_30");
		nErr = ISHELL_CreateInstance(pIShell, AEECLSID_CARDSESSION_NOTIFIER, (void **)&omhConfig.m_pICardNotifier);
	}

	omhConfig.m_bExiting = 0;
	omhConfig.m_pBREWDLOADEF = 0;
	omhConfig.m_pReadStatus = 0;
	omhConfig.m_eReadType = READ_NONE;
	omhConfig.m_nAEP = 0;
	MEMSET(omhConfig.m_pSID,  0, BREW_SID_SIZE);

	CALLBACK_Init(&omhConfig.m_cbRead, (PFNNOTIFY)OEMOMH_ReadCb, (void*)(&omhConfig));

	// load the parameters used last time
	OEMOMH_LoadSaved();

	// update BREW cache
	//OEMOMH_InitBREWDownload();

	DBGPRINTF("<< OEMOMH_Init %d", nErr);
	return nErr;
}


void OEMOMH_Cleanup()
{
	DBGPRINTF("OEMOMH_Cleanup >>");
	if (omhConfig.m_pICardNotifier)
	{
		IBASE_Release((IBase *)omhConfig.m_pICardNotifier);
		// OEMCardSessionNotifier_Release(omhConfig.m_pICardNotifier);	
	}
	if (omhConfig.m_pICardSession)
	{
		IBASE_Release((IBase *)omhConfig.m_pICardSession);
	}
	if (omhConfig.m_pICard)
	{
		IBASE_Release((IBase *)omhConfig.m_pICard);
	}

	omhConfig.m_pICardNotifier = 0;
	omhConfig.m_pICardSession =  0;

	if (omhConfig.m_pReadStatus)
	{
		if (omhConfig.m_pReadStatus->pReadData)
		{			
			FREE(omhConfig.m_pReadStatus->pReadData);
		}
		FREE(omhConfig.m_pReadStatus);
	}
	if (omhConfig.m_pBREWDLOADEF)
	{
		FREE(omhConfig.m_pBREWDLOADEF);
	}
	omhConfig.m_pReadStatus = 0;
	omhConfig.m_pBREWDLOADEF = 0;	
	DBGPRINTF("<< OEMOMH_Cleanup");
}

void OEMOMH_Notify(uint32 dwMask, void *pNotifyData, uint32 sClsId)
{
	DBGPRINTF("OEMOMH_Notify >>");
	if (sClsId == AEECLSID_CARDSESSION_NOTIFIER)
	{
		DBGPRINTF("OEMOMH_Notify_10");
		if (dwMask & NMASK_CARDSESSION_CARD_EVENT)
		{		
			AEECardSessionCardEventData *pCardEvtData = (AEECardSessionCardEventData *)pNotifyData;	
			DBGPRINTF("OEMOMH_Notify_20");
			if(pCardEvtData->dwCardEvent == AEECARDSESSION_NOTIFIER_CARD_READY)
			{			
				DBGPRINTF("OEMOMH_Notify_30");
				if (gsdi_uim_omh_cap.omh_enabled)
				{
					DBGPRINTF("OEMOMH_Notify_40");
					if (gsdi_uim_omh_cap.brew_enabled)
					{
						DBGPRINTF("OEMOMH_Notify_50");
						// AEECardSessionCardReadyData *pReadyData = &(pCardEvtData->uEventData.sCardReady);			
						OEMOMH_ReadBREWEFs();		
					}
					else
					{
						DBGPRINTF("OEMOMH_Notify_60");
						// Use default OEM NV setting
						OEMOMH_CleanupAndReset();
					}
				}
			}
		}
	}
	DBGPRINTF("<< OEMOMH_Notify");
}

void OEMOMH_Notify2()
{
	if (gsdi_uim_omh_cap.omh_enabled)
	{
		if (gsdi_uim_omh_cap.brew_enabled)
		{
			OEMOMH_ReadBREWEFs();       
		}
		else
		{
			OEMOMH_CleanupAndReset();
		}
	}
}


int OEMOMH_GetConfig(AEEConfigItem i, void * pBuff, int nSize)
{
	DBGPRINTF("OEMOMH_GetConfig >>");
    
	if (pBuff == 0)
	{
		DBGPRINTF("OEMOMH_GetConfig_10");
		return EBADPARM;
	}

	if (i != CFGI_DOWNLOAD && i != CFGI_SUBSCRIBERID && i != CFGI_SUBSCRIBERID_LEN)
	{
		DBGPRINTF("OEMOMH_GetConfig_20");
		// we just manage these three configure items
		return EUNSUPPORTED;
	}

	// try OMH_DOWNLOAD_INFO_FILE first if OMHCard is not Ready
	if (omhConfig.m_bExiting == FALSE && (omhConfig.m_bOMHCardReady || !OEMOMH_LoadSaved()))
	{
		DBGPRINTF("OEMOMH_GetConfig_30");
		if (i == CFGI_DOWNLOAD)
		{
			AEEDownloadInfo * pdi = (AEEDownloadInfo *)pBuff;
			DBGPRINTF("OEMOMH_GetConfig_40");
			if (nSize < sizeof(AEEDownloadInfo))
				return EBADPARM;
            DBGPRINTF("OEMOMH_GetConfig_40_1 %s",omhConfig.m_tDldInfo.szServer);
			*pdi = omhConfig.m_tDldInfo;		
		}
		else if (i == CFGI_SUBSCRIBERID)
		{
			DBGPRINTF("OEMOMH_GetConfig_50");
			if(nSize < (STRLEN((char*)omhConfig.m_pSID) + 1))
				return EBADPARM;

			STRLCPY((char*)pBuff, omhConfig.m_pSID, STRLEN(omhConfig.m_pSID)+1); 
		}
		else if (i == CFGI_SUBSCRIBERID_LEN)
		{
			if((omhConfig.m_pSID[0] != 0) && (!(omhConfig.m_tDldInfo.wFlags & DIF_MIN_FOR_SID)))
			{
				DBGPRINTF("OEMOMH_GetConfig_60");
				*(int *)pBuff = STRLEN(omhConfig.m_pSID) + 1;
			}
			// 
			// if DIF_MIN_FOR_SID is true, set the length as that of IMSI
			//
			else //IMSI is always 15 bytes length
			{
				DBGPRINTF("OEMOMH_GetConfig_601");
				*(int *)pBuff = 15 + 1;
			}
		}
		return SUCCESS;
	}
	else
	{
		DBGPRINTF("OEMOMH_GetConfig_70");
		// fallback to the OEM cached default setting persisted in fs:/sys/priv/oemconfig.dat
		return OEM_GetCachedConfig(i, pBuff, nSize);
	}
	DBGPRINTF("<< OEMOMH_GetConfig");
}

#if 0
void Get_config(DEVICE *pMe) 
{ 
    int nResult = AEE_SUCCESS; 

    ISHELL_CreateInstance(AEE_GetShell(), AEECLSID_CONFIG, (void **)&(pMe->m_pIConfig)); 
    if ( pMe->m_pIConfig == NULL ) 
    { 
        DBGPRINTF("Cannot create IConfig instance"); 
        return; // IConfig instance creation failed 
    } 
    pMe->pInfo.pData = MALLOC(sizeof(uint32)); 
    if ( pMe->pInfo.pData == NULL ) 
    { 
        DBGPRINTF("MALLOC(sizeof(uint32) Error"); 

        ICONFIG_Release(pMe->m_pIConfig); 
        pMe->m_pIConfig = NULL; 
        return; 
    } 

    pMe->pInfo.pcbUserCallback = (AEECallback *)MALLOC(sizeof(AEECallback)); 
    if ( !pMe->pInfo.pcbUserCallback ) 
    { 
        DBGPRINTF("MALLOC(sizeof(AEECallback)) Error"); 

        ICONFIG_Release(pMe->m_pIConfig); 
        pMe->m_pIConfig = NULL; 
        FREEIF(pMe->pInfo.pData); 
        return ; 
    } 

    CALLBACK_Init(pMe->pInfo.pcbUserCallback, Config_Len_Cb, (void*)pMe); 
    nResult = ICONFIG_GetItem(pMe->m_pIConfig, CFGI_HOME_SVC_PVDR_NAME_LEN, &pMe->pInfo, sizeof(uint32)); 
    if ( SUCCESS != nResult ) 
    { 
        DBGPRINTF("Get_config1() return nResult = %d.\n", nResult); 
        // cancel the callback 
        CALLBACK_Cancel(pMe->pInfo.pcbUserCallback); 
        FREEIF(pMe->pInfo.pData); 
        ICONFIG_Release(pMe->m_pIConfig); 
        pMe->m_pIConfig = NULL; 
    } 
    DBGPRINTF("Get_config1()<<"); 
} 


/* To read the actual length of EF from RUIM card */ 
void Config_Len_Cb(void* po) 
{ 
    uint32 * pLen; 
    DEVICE *pMe = (DEVICE*)po; 
    int nResult = AEE_SUCCESS; 

    DBGPRINTF(">>Config_Len_Cb()"); 

    pLen = (uint32*)pMe->pInfo.pData; 
    cfgi_len = *pLen; 
    if ( SUCCESS != pMe->pInfo.nStatus ) 
    { 
        CALLBACK_Cancel(pMe->pInfo.pcbUserCallback); 
        FREEIF(pMe->pInfo.pData); 
        ICONFIG_Release(pMe->m_pIConfig); 
        pMe->m_pIConfig = NULL; 
        return; 
    } 
    DBGPRINTF("Get_config() return MCP Len=%d", *pLen); 

    pMe->pInfo.pData = MALLOC(*pLen); 
    if ( pMe->pInfo.pData == NULL ) 
    { 
        return; 
    } 
    CALLBACK_Init(pMe->pInfo.pcbUserCallback, Config_Data_Cb, (void*)pMe); 
    nResult = ICONFIG_GetItem(pMe->m_pIConfig, CFGI_HOME_SVC_PVDR_NAME, &pMe->pInfo, *pLen); 
    if ( SUCCESS != nResult ) 
    { 
        DBGPRINTF("***LQP Config_Len_Cb() return nResult = %d.\n", nResult); 
        // cancel the callback 
        CALLBACK_Cancel(pMe->pInfo.pcbUserCallback); 
        FREEIF(pMe->pInfo.pData); 
        ICONFIG_Release(pMe->m_pIConfig); 
        pMe->m_pIConfig = NULL; 
    } 
    FREEIF(pLen); 
    DBGPRINTF("Config_Len_Cb()<<"); 
} 

void Config_Data_Cb(void* po) 
{ 
    DEVICE* pMe = (DEVICE*) po; 
    BrowserCP_TLV_type ** tlv_ptr_ptr; 
    MMSICP_TLV_type ** mms_icp_tlv_ptr_ptr; 
    BrowserBookmark_TLV_type ** bbm_tlv_ptr_ptr; 
    MMS_Config_type mms_config; 
    ROOT_Certificate_type ** root_certi_tlv_ptr_ptr; 
    app_label_info_type * app_label_info_ptr; 
    unsigned int num_tlvs; 

    DBGPRINTF(">>Config_Data_Cb()"); 
    //char *psStr = NULL; 

    if ( AEE_SUCCESS != pMe->pInfo.nStatus ) 
    { 
        DBGPRINTF("Config_Data_Cb pMe->pInfo.nStatus = %d.\n", pMe->pInfo.nStatus); 
        ICONFIG_Release(pMe->m_pIConfig); 
        pMe->m_pIConfig = NULL; 
        FREEIF(pMe->pInfo.pData); 
        return; 
    } 
    if(cfgi_len>700) 
        cfgi_len = 700; 
    Hex2String(pMe->pInfo.pData, temp, cfgi_len); 
    DBGPRINTF("Config_Data_Cb get mcp = %s.\n", temp); 
    FREEIF(pMe->pInfo.pData); 
    ICONFIG_Release(pMe->m_pIConfig); 
    pMe->m_pIConfig = NULL; 
    DBGPRINTF("Config_Data_Cb()<<"); 
}
#endif 

#include "OEMSVC.h"

#define AEECLSID_MOBILESHOP	0x01010EA7

extern void OEMDSS_SetAppType(uint32 uAppType);
void OEMOMH_SetProfile(AEECLSID cls, AEEEvent evt)
{
 	boolean fDynamic = FALSE;
    boolean bBrowser = FALSE;
 	uint32 itemId;

 	itemId = ISHELL_GetClassItemID(AEE_GetShell(), cls);
 	fDynamic = (itemId < FAKE_MODULE_ID);
 	if (AEECLSID_MOBILESHOP == cls)
 	{
 		fDynamic = TRUE;
 	}
    else if(AEECLSID_UCWEB == cls)
    {
        bBrowser = TRUE;
    }
    
 	switch(evt)
 	{
 		case EVT_APP_START:
            if(fDynamic)
            {
                OEMDSS_SetAppType(DA_BREW_TYPE);
            }
            else if(bBrowser)
            {
                OEMDSS_SetAppType(DA_WAP_TYPE);
            }
            else
            {
                OEMDSS_SetAppType(0);
            }
 			break;
 		case EVT_APP_STOP:
 			OEMDSS_SetAppType(0);
 			break;
 		case EVT_APP_SUSPEND:
 			OEMDSS_SetAppType(0);
 			break;
 		case EVT_APP_RESUME:
 			if(fDynamic)
            {
                OEMDSS_SetAppType(DA_BREW_TYPE);
            }
            else if(bBrowser)
            {
                OEMDSS_SetAppType(DA_WAP_TYPE);
            }
            else
            {
                OEMDSS_SetAppType(0);
            }
 			break;
 		default:
 			return;
 	}
}

#endif

