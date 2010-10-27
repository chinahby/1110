/*===========================================================================

FILE: OEMCamera.c

SERVICES: ICamera interface implementation

DESCRIPTION
   This file implements BREW ICamera. ICamera controls the device camera
   and allows recording of snapshot or movie.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable
===========================================================================*/
/*============================================================================

                      EDIT HISTORY FOR FILE

when      who    what, where, why
 --------  -----  --------------------------------------------------------------
10/25/10  sm    wangliang init
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "OEMFeatures.h"

#include "OEMCriticalSection.h"
#include "OEMFS.h"
#include "AEE_OEMFile.h"
#include "AEEStdLib.h"
#include "cam_module.h"
#include "OEMCameraEx.h"
/*===========================================================================
Feature condition - FEATURE_CAMERA_LCD_DIRECT_MODE is under condition of FEATURE_MDP
They are defined in different cust files and condition could not be set there.
Workaround in .c files to make sure no compiling issue happens in the case.
===========================================================================*/
/*===========================================================================

                      MACRO DEFINITIONS

===========================================================================*/

/*===========================================================================

                      CameraDevice DECLARATIONS

===========================================================================*/

/*===========================================================================

                      ICamera DECLARATIONS

===========================================================================*/
static boolean       OEMCamera_FilePath(OEMCamera * pme, void * pData);
static int           OEMCamera_Preview(OEMCamera * pme);
static int           OEMCamera_RecordMovie(OEMCamera * pme);
static int           OEMCamera_RecordSnapshot(OEMCamera * pme);

/*===============================================================================

                     CameraDevice Function Definitions

===============================================================================*/
/*===============================================================================

                     ICamera Function Definitions

===============================================================================*/
/*==================================================================

==================================================================*/
void OEMCamera_Init(void)
{
  // Empty
}

/****************/
/*==================================================================

==================================================================*/
int OEMCamera_New(AEECLSID cls, IBitmap * pFrame, uint16 wSize, OEMINSTANCE * ph)
{
	OEMCamera *pme = NULL;
	int        nRet = SUCCESS;


	if (!ph)
		return EBADPARM;

	// Alloc memory for the object
	pme = (OEMCamera *)MALLOC(sizeof(OEMCamera));

	if (!pme)
	{
		nRet = ENOMEMORY;
		goto Done;
	}

	pme->m_nMode = CAM_MODE_READY;

	// By default, file size is disabled by setting param to zero
	pme->m_nFileSize = OEMCAMERA_FILE_SIZE_DISABLED;
	// Assume we will be using Snapshot mode

	*ph = (OEMINSTANCE)pme;
		return nRet;

Done:
	if (ph)
		*ph = NULL;
	FREE(pme);
	return nRet;
}

/*==================================================================

==================================================================*/
int OEMCamera_QueryInterface(OEMINSTANCE h, AEECLSID idReq, void ** ppo)
{
	return ECLASSNOTSUPPORT;
}

/*==================================================================

==================================================================*/

/*==================================================================

==================================================================*/
int OEMCamera_SetParm(OEMINSTANCE h, int16 nParmID, int32 p1, int32 p2)
{
	int               nRet = SUCCESS;
	OEMCamera *       pme = (OEMCamera *)h;

	switch (nParmID)
	{
		case CAM_PARM_QUALITY:
		// Quality is treated differenty for snapshot VS movie modes
		if (pme->m_nPreviewType == CAM_PREVIEW_SNAPSHOT)
		{
			// Valid range for snapshot quality param is 1 to 100
			// equal the quality percent (for snapshot camera only).
			if ((p1 >= OEMCAMERA_QUALITY_MIN_VALUE) && (p1 <= OEMCAMERA_QUALITY_MAX_VALUE))
				pme->m_nQuality = (int16)p1;
			else
				nRet = EBADPARM;
		}
		else if (pme->m_nPreviewType == CAM_PREVIEW_MOVIE)
		{

			if ((p1 >= OEMCAMCORDER_QUALITY_MIN_VALUE) && (p1 <= OEMCAMCORDER_QUALITY_MAX_VALUE))
			{
				pme->m_nQuality = (int16)p1;
			}
			else
			{
				nRet = EBADPARM;
			}
		}
		break;

		case CAM_PARM_FLASH_CTL:
			if (p1)
			{
			}
			else
			nRet = EBADPARM;
			break;

		case CAM_PARM_NIGHTSHOT_MODE:

		break;

		case CAM_PARM_EFFECT:
		
			if (p1)
			{
			}
			else
			{
				nRet = EBADPARM;
			}

			break;

		case CAM_PARM_FLASH:
			break;

		case CAM_PARM_FILE_SIZE:
			// File size will take precidence over Quality if it is non zero
			// Units are Kilobytes for this param
			if ((p1 >= OEMCAMERA_FILE_SIZE_MIN_VALUE) && (p1 <= OEMCAMERA_FILE_SIZE_MAX_VALUE))
			{
				pme->m_nFileSize = (int32)p1;
			}
			else
			{
				nRet = EBADPARM;
			}
			break;

		case CAM_PARM_IS_SUPPORT:  // Fall through...
		case CAM_PARM_IS_MOVIE: // This cannot be set by user
		case CAM_PARM_PIXEL_COUNT:
		case CAM_PARM_VIDEO_ENCODE_LIST: // This cannot be set by user
		case CAM_PARM_AUDIO_ENCODE_LIST: // This cannot be set by user
		case CAM_PARM_SIZE_LIST: // This cannot be set by user
		case CAM_PARM_FPS_LIST: // This cannot be set by user
		default:
			nRet = EUNSUPPORTED;
		break;
	}

	return nRet;
}

/*==================================================================

==================================================================*/
int OEMCamera_GetParm(OEMINSTANCE h, int16 nParmID, int32 * pP1, int32 * pP2)
{
   OEMCamera *       pme = (OEMCamera *)h;
   int               nRet = SUCCESS;

   return nRet;
}

/*==================================================================

==================================================================*/
int OEMCamera_Start(OEMINSTANCE h, int16 nMode, uint32 dwParam)
{
	OEMCamera * pme = (OEMCamera *)h;
	int         nRet = EFAILED;


	if (nMode == CAM_MODE_PREVIEW)
	{
		nRet = OEMCamera_Preview(pme);
	}
	else if (nMode == CAM_MODE_SNAPSHOT)
   	{
		nRet = OEMCamera_RecordSnapshot(pme);
	}
	else
    {
    	nRet = EBADPARM;
	}

	return nRet;
}

/*==================================================================

==================================================================*/
int OEMCamera_Stop(OEMINSTANCE h)
{
	OEMCamera * pme = (OEMCamera *)h;

	AIT701_cam_exit_preview();

	return SUCCESS;
}
/*==================================================================

==================================================================*/
int OEMCamera_EncodeSnapshot(OEMINSTANCE hInstance)
{
   OEMCamera *          pme = (OEMCamera *)hInstance;
   int                  nRet;

   return (nRet);
}

/*==================================================================

==================================================================*/

/*===============================================================================

                     LOCAL FUNCTIONS

===============================================================================*/
static boolean OEMCamera_FilePath(OEMCamera * pme, void * pData)
{
   const char *cpszFile = (const char *)pData;
   char *      pszFS = 0;
   int         nLen;
   int         bRet = FALSE;

   return bRet;
}
/*==================================================================

==================================================================*/
static int OEMCamera_Preview(OEMCamera * pme)
{
	ext_camera_para_struct ext_cam_para = {0};

	AIT701_cam_preview(&ext_cam_para);
	return SUCCESS;
}
/*==================================================================

==================================================================*/
static int OEMCamera_RecordSnapshot(OEMCamera * pme)
{
	return SUCCESS;
}
