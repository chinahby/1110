/*=============================================================================

FILE: FMRadioApp.c

SERVICES: FM Radio Application

GENERAL DESCRIPTION:
Creates and Starts FM Radio Application

PUBLIC CLASSES AND STATIC FUNCTIONS:


INITIALIZATION AND SEQUENCING REQUIREMENTS:
None.

(c) COPYRIGHT 2008 QUALCOMM Incorporated.
                   All Rights Reserved.

                   QUALCOMM Proprietary
=============================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE
$Header: //depot/asic/msmshared/apps/FMRadio/FMRadioApp.c#7 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/23/09   kcp     Fix Fmradio crash CR 170540
10/09/08   sg      Fix frequency display per the new IFMRadio API
12/20/07   sg      Initial Revision

===========================================================================*/

/*===========================================================================

                      INCLUDES

===========================================================================*/
#include "FMRadioApp.h"

#include "AEE.h"
#include "AEEShell.h"
#include "AEEStdLib.h"
#include "AEEAppGen.h"
#include "AEEMenu.h"
#include "AEEFile.h"
#include "AEEDisp.h"

#include "AEEFMRadio.bid"

#include "AEEFMRDS.bid"

#include "fmradioapp.brh"
#include "fmradioapp.bid"


/*===========================================================================

                      Constant Definitions

===========================================================================*/
#define FMRADIO_CFG_FILE   "fmradioapp.cfg"
#define FMRADIO_PREF_FILE  "fmradioapp_pref.txt"

#define FMRADIO_LONGKEYPRESS    1500   /* Long key press period */
#define FMRADIO_POST_CTOR_DELAY 1500   /* Delay for IFMRadio, IFMRDS contructors */
#define FMRADIO_BAND_OTHERS 'O'
#define FMRADIO_BAND_JAPAN  'J'
#define FMRADIO_DEFAULT_GAP '2'

#define FMRADIO_PSNAME_SIZE    8
#define FMRADIO_MAX_VOLUME 7


char *gpFMRadioResourceFile = FMRADIOAPP_RES_FILE;

/*===========================================================================

                      STATIC DATA

===========================================================================*/
typedef struct
{
   int m_LastFreq;
   int m_LastVol;
} FMRadio_Config;

typedef struct
{
   char m_Band;
   char m_Gap;
} FMRadio_Pref;

/*===========================================================================

                      FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

                      STATIC and PUBLIC FUNCTIONS

===========================================================================*/
static boolean FMRadioApp_UpdateMainDlg(FMRadioApp *pMe);
static boolean FMRadioApp_UpdateFreq(FMRadioApp *pMe);
static boolean FMRadioApp_UpdateVolume(FMRadioApp *pMe);
static boolean FMRadioApp_UpdateSignalStrength(FMRadioApp *pMe);
static void FMRadioApp_SetStationCB(void *pUser);
static boolean FMRadioApp_LongKeyPressCB(FMRadioApp *pMe);
static boolean FMRadioApp_UpdateStaticCtl(FMRadioApp *pMe, int16 ctlID, AECHAR *ptitle, 
                                          AECHAR *ptext, AEEFont fntTitle, AEEFont fntText);
static boolean FMRadioApp_InitRDSInfoDlg(FMRadioApp *pMe, IDialog* pIDialog);


/*===========================================================================
FUNCTION FMRadioApp_InitAppData

DESCRIPTION
  Initializing the application data

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
boolean FMRadioApp_InitAppData(IApplet* po)
{
   FMRadioApp *pMe = (FMRadioApp *)po;
   int nReturnStatus;
   boolean ret_val = FALSE;

   do
   {
      nReturnStatus = ISHELL_CreateInstance(pMe->a.m_pIShell, 
                                            AEECLSID_FMRadio, 
                                            (void **)&pMe->m_pIFMRadio);
      if (nReturnStatus != SUCCESS || pMe->m_pIFMRadio == NULL)
      {
         // FMRadio creation failed
         break;
      }

      nReturnStatus = ISHELL_CreateInstance(pMe->a.m_pIShell, 
                                            AEECLSID_FMRDS, 
                                            (void **)&pMe->m_pIFMRDS);
      if (nReturnStatus != SUCCESS || pMe->m_pIFMRDS == NULL)
      {
         // FMRDS creation failed
         break;
      }

	  nReturnStatus = ISHELL_CreateInstance(pMe->a.m_pIShell, 
                                            AEECLSID_SOUND, 
                                            (void **)&pMe->m_pISound);
      if (nReturnStatus != SUCCESS || pMe->m_pISound == NULL)
      {
         // Sound creation failed
         break;
      }
      ISOUND_RegisterNotify( pMe->m_pISound, NULL, NULL );

      nReturnStatus = ISHELL_CreateInstance(pMe->a.m_pIShell,
                                            AEECLSID_FILEMGR,
                                            (void **) &pMe->m_pIFileMgr);

      if(nReturnStatus != SUCCESS || pMe->m_pIFileMgr == NULL) 
      {
         // File Manager creation failed
         break;
      }

      ret_val = TRUE;
   }
   while (0);

   return ret_val;
} /* End FMRadioApp_InitAppData */

/*===========================================================================
FUNCTION FMRadioApp_FreeAppData

DESCRIPTION
  Release the application data

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
void FMRadioApp_FreeAppData(IApplet* po)
{
   FMRadioApp *pMe = (FMRadioApp*)po;

   if (NULL == pMe)
   {
      return;
   }

   if (pMe->m_pIFMRadio != NULL)
   {
      IFMRadio_EnablePlay(pMe->m_pIFMRadio, FALSE);
      IFMRadio_Release(pMe->m_pIFMRadio);
      pMe->m_pIFMRadio = NULL;
   }

   if (pMe->m_pIFMRDS != NULL)
   {
      IFMRDS_Release(pMe->m_pIFMRDS);
      pMe->m_pIFMRDS = NULL;
   }

   if (pMe->m_pISound != NULL)
   {
      ISOUND_Release(pMe->m_pISound);
      pMe->m_pISound = NULL;
   }

   if (pMe->m_pIFileMgr != NULL)
   {
      IFILEMGR_Release(pMe->m_pIFileMgr);
      pMe->m_pIFileMgr = NULL;
   }

} /* End FMRadioApp_FreeAppData */

/*===========================================================================
FUNCTION FMRadioApp_GetStrongChannelCB

DESCRIPTION

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
static void FMRadioApp_GetStrongChannelCB(void *pUser)
{
   FMRadioApp *pMe = (FMRadioApp *)pUser;

   if (pMe == NULL)
   {
      return;
   }

   if (pMe->m_GetStrongChannelCBResult == AEE_SUCCESS)
   {
      // Set the new frequency in IFMRadio interface
      IFMRadio_SetStation(pMe->m_pIFMRadio, pMe->m_StrongChannel, 
                          &pMe->m_SetStationCBResult, &pMe->m_SetStationCB);
   }
}

/*===========================================================================
FUNCTION FMRadioApp_LongKeyPressCB

DESCRIPTION
  Called by the timer when FMRADIO_LONGKEYPRESS time expires

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
static boolean FMRadioApp_LongKeyPressCB(FMRadioApp *pMe)
{
   if (NULL == pMe)
   {
      return FALSE;
   }

   if (pMe->m_PrevKeyPress == AVK_UP || pMe->m_PrevKeyPress == AVK_DOWN)
   {
      AECHAR buffer[FMRADIO_MAX_STR_SIZE];
      int nResult = ISHELL_LoadResString(pMe->a.m_pIShell,
                                         gpFMRadioResourceFile,
                                         IDS_STATION_SEARCHING,
                                         buffer,
                                         ARR_SIZE(buffer)*sizeof(AECHAR));

      if (nResult == 0)
      {
         return FALSE;
      }

      // Display "Searching..." in place of signal strength
      FMRadioApp_UpdateStaticCtl(pMe, IDC_STATIC_SIGNAL_STRENGTH, NULL, buffer, AEE_FONT_NORMAL, AEE_FONT_NORMAL);

      pMe->m_bLongKeyPress = TRUE;

      pMe->m_SearchDir = (pMe->m_PrevKeyPress == AVK_UP) ? AEERADIO_STN_SEARCH_UP : AEERADIO_STN_SEARCH_DOWN;
      pMe->m_NumChannels = 0;
      IFMRadio_GetStrongestChannels(pMe->m_pIFMRadio, &pMe->m_StrongChannel, 1, &pMe->m_NumChannels, 
                                    &pMe->m_GetStrongChannelCBResult, &pMe->m_GetStrongChannelCB,
                                    pMe->m_SearchDir);
   }

   return TRUE;
}

/*===========================================================================
FUNCTION FMRadioApp_SetStationCB

DESCRIPTION

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
static void FMRadioApp_SetStationCB(void *pUser)
{
   FMRadioApp *pMe = (FMRadioApp *)pUser;

   if (pMe == NULL)
   {
      return;
   }

   if (pMe->m_SetStationCBResult == AEE_SUCCESS)
   {
      // If the key is still pressed, get the next channel
      if (pMe->m_bLongKeyPress)
      {
         IFMRadio_GetStrongestChannels(pMe->m_pIFMRadio, &pMe->m_StrongChannel, 1, &pMe->m_NumChannels, 
                                       &pMe->m_GetStrongChannelCBResult, &pMe->m_GetStrongChannelCB,
                                       pMe->m_SearchDir);
      }
      else {
         //--->> BG <<---
         IFMRadio_EnablePlay(pMe->m_pIFMRadio,TRUE);
         //--->> BG <<---
      }
   }

   FMRadioApp_UpdateFreq(pMe);
}

/*===========================================================================
FUNCTION FMRadioApp_SetBandCB

DESCRIPTION

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
static void FMRadioApp_SetBandCB(void *pUser)
{
   IFile *config_file;
   FMRadio_Config config_data;
   FMRadioApp *pMe = (FMRadioApp *)pUser;

   if (pMe == NULL)
   {
      return;
   }

   /* Get valid freq range */
   if (pMe->m_FMCallbackResult == SUCCESS && 
       IFMRadio_GetValidFreqRange(pMe->m_pIFMRadio, &pMe->m_MinFreq, &pMe->m_MaxFreq) == SUCCESS)
   {
      pMe->m_MaxFreq -= (pMe->m_MaxFreq - pMe->m_MinFreq) % pMe->m_FreqGap;

      if (pMe->m_MinFreq < 0 || pMe->m_MaxFreq < pMe->m_MinFreq)
      {
         return; 
      }

      pMe->m_CurFreq = pMe->m_MinFreq;
      pMe->m_CurVol  = 0;

      if(pMe->m_pIFileMgr != NULL) 
      {
         /* If config file does not exit, create one */
         if ((config_file = IFILEMGR_OpenFile(pMe->m_pIFileMgr, FMRADIO_CFG_FILE, _OFM_READWRITE)) == NULL)
         {
            config_data.m_LastFreq = pMe->m_MinFreq;
            config_data.m_LastVol = 0;

            config_file = IFILEMGR_OpenFile(pMe->m_pIFileMgr, FMRADIO_CFG_FILE, _OFM_CREATE);

            if (config_file != NULL)
            {
               IFILE_Write(config_file, &config_data, sizeof(config_data));
               IFILE_Release(config_file);
            }
         }
         else
         {
            /* If unable to read the entire data, write and use default data */
            if (IFILE_Read(config_file, &config_data, sizeof(config_data)) != sizeof(config_data) ||
                config_data.m_LastFreq < pMe->m_MinFreq ||
                config_data.m_LastFreq > pMe->m_MaxFreq)
            {
               config_data.m_LastFreq = pMe->m_MinFreq;
               config_data.m_LastVol = 0;
               IFILE_Seek(config_file, _SEEK_START, 0);
               IFILE_Write(config_file, &config_data, sizeof(config_data));
            }
            IFILE_Release(config_file);
         }


         pMe->m_CurFreq = config_data.m_LastFreq;
         pMe->m_CurVol  = config_data.m_LastVol;
      }

      pMe->m_CurSigStrength = 0;
      if (IFMRadio_GetMaxSignalStrength(pMe->m_pIFMRadio, &pMe->m_MaxSigStrength) != AEE_SUCCESS)
      {
         // To avoid divide by 0 later.
         pMe->m_MaxSigStrength = 100;
      }

      FMRadioApp_UpdateMainDlg(pMe);
   }
}
/*===========================================================================
FUNCTION FMRadioApp_FMRDSInfoCB

DESCRIPTION

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
static void FMRadioApp_FMRDSInfoCB(void *pUser)
{
   FMRadioApp *pMe = (FMRadioApp *)pUser;
   AEEFMRDSParsedInfo rdsInfo;
   char   text[FMRADIO_RADIOTEXT_SIZE+1];
   IDialog *pIDialog;
   pMe->m_radio_text[0] = NULL;
   pMe->m_rt_tmp[0] = NULL;

   //CR170540 : IFMRDS_GetParsedData will modify pAFCodePairs if it is not NULL 
   //Hence we should either assign it to NULL or Allocate memory to avoid corruption
   //However App do not need any such info we will assign it to NULL    
   rdsInfo.sAFCodePairs.pAFCodePairs = NULL;
   rdsInfo.sAFCodePairs.nBufLen = 0;

   if ((IFMRDS_GetParsedData(pMe->m_pIFMRDS, AEEFMRDS_INFO_2A, &rdsInfo) != SUCCESS) ||
          (!rdsInfo.bABTextFalg))
      {
         if (IFMRDS_GetParsedData(pMe->m_pIFMRDS, AEEFMRDS_INFO_0A, &rdsInfo) == SUCCESS)
         {
            MEMCPY(text, rdsInfo.aPSName, FMRADIO_PSNAME_SIZE);
            text[FMRADIO_PSNAME_SIZE] = '\0';
            STRTOWSTR(text, pMe->m_rt_tmp, ARR_SIZE(text)*sizeof(AECHAR));
            WSTRLCPY( pMe->m_radio_text, pMe->m_rt_tmp, ARR_SIZE( pMe->m_radio_text));

            if (IFMRDS_GetParsedData(pMe->m_pIFMRDS, AEEFMRDS_INFO_0B, &rdsInfo) == SUCCESS)
            {
               MEMCPY(text, rdsInfo.aPSName, FMRADIO_PSNAME_SIZE);
               text[FMRADIO_PSNAME_SIZE] = '\0';
               STRTOWSTR(text, pMe->m_rt_tmp, ARR_SIZE(text)*sizeof(AECHAR));
               WSTRLCAT( pMe->m_radio_text, pMe->m_rt_tmp, ARR_SIZE( pMe->m_radio_text));
            }
         }
      }
      else
      {
         MEMCPY(text, rdsInfo.aRTMessage, FMRADIO_RADIOTEXT_SIZE);
         text[FMRADIO_RADIOTEXT_SIZE] = '\0';
         STRTOWSTR(text, pMe->m_rt_tmp, ARR_SIZE(text)*sizeof(AECHAR));
         WSTRLCPY( pMe->m_radio_text, pMe->m_rt_tmp, ARR_SIZE( pMe->m_radio_text));

         if (IFMRDS_GetParsedData(pMe->m_pIFMRDS, AEEFMRDS_INFO_2B, &rdsInfo) == SUCCESS &&
             rdsInfo.bABTextFalg)
         {
            MEMCPY(text, rdsInfo.aRTMessage, FMRADIO_RADIOTEXT_SIZE);
            text[FMRADIO_RADIOTEXT_SIZE] = '\0';
            STRTOWSTR(text, pMe->m_rt_tmp, ARR_SIZE(text)*sizeof(AECHAR));
            WSTRLCAT( pMe->m_radio_text, pMe->m_rt_tmp, ARR_SIZE( pMe->m_radio_text));
         }
      }
	  
   pMe->m_nPICode = rdsInfo.nPICode;
   pMe->m_nPTYCode = rdsInfo.nPTYCode;

   IFMRDS_InfoReadable(pMe->m_pIFMRDS,
					   &pMe->m_FMRDSInfoCB,
					   AEEFMRDS_INFO_0A,
					   AEEFMRDS_PARSED_DATA);
   
   pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
   if (pIDialog)
   {
     if(IDIALOG_GetID(pIDialog) == IDD_RDSINFO)
     {
        FMRadioApp_InitRDSInfoDlg(pMe,pIDialog);
     }
   }	
}

/*===========================================================================
FUNCTION FMRadioApp_SignalStrengthCB

DESCRIPTION

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
static void FMRadioApp_SignalStrengthCB(void *pUser)
{
   FMRadioApp *pMe = (FMRadioApp *)pUser;
   int req_size = 0, i;
   AEERadioEvents *radio_events;

   // Get the required size
   IFMRadio_GetEvents(pMe->m_pIFMRadio, NULL, 0, &req_size);
   radio_events = MALLOC(req_size * sizeof(AEERadioEvents));

   if (radio_events == NULL)
   {
      return;
   }

   // Get the actual events now
   IFMRadio_GetEvents(pMe->m_pIFMRadio, radio_events, req_size, &req_size);

   for (i = 0; i < req_size; ++i)
   {
      if (radio_events[i] == EVT_FMRADIO_SIGNAL_STRENGTH_CHANGE)
      {
         int signal_strength;
         if (IFMRadio_GetSignalStrength(pMe->m_pIFMRadio, &signal_strength) == AEE_SUCCESS)
         {
            pMe->m_CurSigStrength = signal_strength;
            FMRadioApp_UpdateSignalStrength(pMe);
			IFMRadio_OnEventNotify(pMe->m_pIFMRadio, &pMe->m_SignalStrengthCB);
         }
         
         break;
      }
   }

   FREEIF(radio_events);
}


/*===========================================================================
FUNCTION FMRadioApp_SetDefaultMenuLook

DESCRIPTION

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
void FMRadioApp_SetDefaultMenuLook(IShell *ps, IMenuCtl *pm)
{
  AEEItemStyle sstyle;
  AEEItemStyle ustyle;
  AEEMenuColors col;

  ISHELL_GetItemStyle( ps, AEE_IT_MENU, &ustyle, &sstyle );

  /* selection style */
  sstyle.ft = AEE_FT_RAISED;
  sstyle.xOffset = 0;
  sstyle.yOffset = 0;
  sstyle.roImage = AEE_RO_TRANSPARENT;
  ustyle.ft = AEE_FT_NONE;
  ustyle.xOffset = 2;
  ustyle.yOffset = 0;
  ustyle.roImage = AEE_RO_TRANSPARENT;
  IMENUCTL_SetStyle(pm, &ustyle, &sstyle);

  /* colors */
  col.cSelText= RGB_BLACK;
  col.wMask = MC_BACK | MC_SEL_BACK| MC_SEL_TEXT;
  col.cBack = MAKE_RGB(255, 255, 204);

  col.cSelBack = MAKE_RGB(153, 204, 204);

  IMENUCTL_SetColors(pm, &col);
}

/*===========================================================================
FUNCTION FMRadioApp_UpdateStaticCtl

DESCRIPTION
  Initializing the application data

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
static boolean FMRadioApp_UpdateStaticCtl(FMRadioApp *pMe, int16 ctlID, AECHAR *ptitle, 
                                          AECHAR *ptext, AEEFont fntTitle, AEEFont fntText)
{
   IDialog* pIDialog;
   boolean ret_val = FALSE;

   if (pMe != NULL)
   {
      pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

      if (pIDialog != NULL)
      {
         IStatic *pIStaticText = (IStatic*)IDIALOG_GetControl(pIDialog, ctlID);

         if (pIStaticText != NULL)
         {
            ISTATIC_SetActive(pIStaticText, TRUE);
            (void)ISTATIC_SetText(pIStaticText, ptitle, ptext, fntTitle, fntText);
            ret_val = ISTATIC_Redraw(pIStaticText);
         }
      }
   }

   return ret_val;
}

/*===========================================================================
FUNCTION FMRadioApp_UpdateFreq

DESCRIPTION
  

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
static boolean FMRadioApp_UpdateFreq(FMRadioApp *pMe)
{
   AECHAR fm_freq[FMRADIO_MAX_STR_SIZE];
   AECHAR format[] = {'%', 'd', '.', '%', 'd', '\0'};
   int cur_freq;
   boolean ret_val = FALSE;

   if (IFMRadio_GetTunedStationFrequency(pMe->m_pIFMRadio, &cur_freq) == SUCCESS)
   {
      pMe->m_CurFreq = cur_freq;

      WSPRINTF(fm_freq, ARR_SIZE(fm_freq)*sizeof(AECHAR), format, cur_freq/1000, (cur_freq%1000)/100);

      ret_val = FMRadioApp_UpdateStaticCtl(pMe, IDC_STATIC_FREQUENCY, NULL, fm_freq, 
                                           AEE_FONT_NORMAL, AEE_FONT_LARGE);
   }

   return ret_val;
}

/*===========================================================================
FUNCTION FMRadioApp_UpdateVolume

DESCRIPTION
 

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
static boolean FMRadioApp_UpdateVolume(FMRadioApp *pMe)
{
   AECHAR fm_volume[FMRADIO_MAX_STR_SIZE];
   AECHAR format[] = {'%', 's', ':', '%', '4', 'd', '\0'};
   int cur_vol = pMe->m_CurVol;
   if(pMe->m_CurVol> FMRADIO_MAX_VOLUME)
   	  pMe->m_CurVol =cur_vol/14;
   /* originally volume level rang is from 0 to 100 & to display it's ranging from 0 to 7  , 
      it has to menuplate before passing volume level to lower layers*/
   if (cur_vol < FMRADIO_MAX_VOLUME)
   	   cur_vol = cur_vol*14;
   else
       cur_vol = 100;
   	
   /* Update the device volume */
   ISOUND_SetVolume(pMe->m_pISound, cur_vol);

   WSPRINTF(fm_volume, ARR_SIZE(fm_volume)*sizeof(AECHAR), format, pMe->m_VolumeStr, pMe->m_CurVol);

   return FMRadioApp_UpdateStaticCtl(pMe, IDC_STATIC_VOLUME, NULL, fm_volume, 
                                     AEE_FONT_NORMAL, AEE_FONT_NORMAL);
}

/*===========================================================================
FUNCTION FMRadioApp_UpdateSignalStrength

DESCRIPTION
 

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
static boolean FMRadioApp_UpdateSignalStrength(FMRadioApp *pMe)
{
   AECHAR fm_signal[FMRADIO_MAX_STR_SIZE];
   AECHAR format[] = {'%', 's', ':', '%', '4', 'd', '\0'};
   int signal_strength = pMe->m_CurSigStrength;

   /* Convert to a percentage */
   signal_strength = (signal_strength * 100)/pMe->m_MaxSigStrength;

   WSPRINTF(fm_signal, ARR_SIZE(fm_signal)*sizeof(AECHAR), format, pMe->m_SignalStr, signal_strength);

   return FMRadioApp_UpdateStaticCtl(pMe, IDC_STATIC_SIGNAL_STRENGTH, NULL, fm_signal, 
                                     AEE_FONT_NORMAL, AEE_FONT_NORMAL);
}

/*===========================================================================
FUNCTION FMRadioApp_UpdateMainDlg

DESCRIPTION
  Initializing the application data

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
static boolean FMRadioApp_UpdateMainDlg(FMRadioApp *pMe)
{
   IStatic *pTitle = NULL, *pSignal = NULL, *pVolume = NULL;
   IMenuCtl *pIMenu = NULL;
   IDialog* pIDialog = NULL;
   AECHAR buffer[FMRADIO_MAX_STR_SIZE];
   int nResult1 = 0, nResult2 = 0, nResult3 = 0;
   boolean ret_val = FALSE;
   AEESoundInfo   si;
   int signal_strength = 0;

   if (pMe == NULL || (pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell)) == NULL)
   { /* error */
      return FALSE;
   }

   pTitle  = (IStatic*)IDIALOG_GetControl(pIDialog, (int16) IDC_STATIC_FMRADIO_TITLE);
   pSignal = (IStatic*)IDIALOG_GetControl(pIDialog, (int16) IDC_STATIC_SIGNAL_STRENGTH);
   pVolume = (IStatic*)IDIALOG_GetControl(pIDialog, (int16) IDC_STATIC_VOLUME);
   pIMenu  = (IMenuCtl*)IDIALOG_GetControl(pIDialog, (int16) IDC_SK_FMRADIO);

   if (pIMenu != NULL && pTitle != NULL && 
       pSignal != NULL && pVolume != NULL)
   {
      /* Set the softkey menu style */
      FMRadioApp_SetDefaultMenuLook(pMe->a.m_pIShell, pIMenu);

      /* Set the title and content */
      nResult1 = ISHELL_LoadResString(pMe->a.m_pIShell,
                                      gpFMRadioResourceFile,
                                      IDS_FMRADIO_TITLE,
                                      buffer,
                                      ARR_SIZE(buffer)*sizeof(AECHAR));

      nResult2 = ISHELL_LoadResString(pMe->a.m_pIShell,
                                      gpFMRadioResourceFile,
                                      IDS_FMRADIO_VOLUME,
                                      pMe->m_VolumeStr,
                                      ARR_SIZE(pMe->m_VolumeStr)*sizeof(AECHAR));

      nResult3 = ISHELL_LoadResString(pMe->a.m_pIShell,
                                      gpFMRadioResourceFile,
                                      IDS_SIGNAL_STRENGTH,
                                      pMe->m_SignalStr,
                                      ARR_SIZE(pMe->m_SignalStr)*sizeof(AECHAR));

      if (nResult1 != 0 && nResult2 != 0 && nResult3 != 0)
      {
         /* Display the title */
         ISTATIC_SetProperties(pTitle, ST_CENTERTEXT|ST_CENTERTITLE);
         FMRadioApp_UpdateStaticCtl(pMe, IDC_STATIC_FMRADIO_TITLE, buffer, NULL, 
                                    AEE_FONT_BOLD, AEE_FONT_NORMAL);

         // Initialize the Set Station call back structure
         pMe->m_SetStationCB.pfnCancel = NULL;
         pMe->m_SetStationCB.pfnNotify = FMRadioApp_SetStationCB;
         pMe->m_SetStationCB.pNotifyData = pMe;

         IFMRadio_SetStation(pMe->m_pIFMRadio, pMe->m_CurFreq, 
                             &pMe->m_SetStationCBResult, &pMe->m_SetStationCB);

         /* Set and display the volume information */
         ISOUND_Get(pMe->m_pISound, &si);
         si.eDevice = AEE_SOUND_DEVICE_CURRENT;
         si.eAPath = AEE_SOUND_APATH_LOCAL;
         si.eMethod = 8; //--->> BG <<---
         ISOUND_Set(pMe->m_pISound, &si);
         ISOUND_SetDevice(pMe->m_pISound);

         FMRadioApp_UpdateVolume(pMe);

         //--->> BG <<---
         //IFMRadio_EnablePlay(pMe->m_pIFMRadio, TRUE);
         //--->> BG <<---

         /* Update the signal strength */
         if (IFMRadio_GetSignalStrength(pMe->m_pIFMRadio, &signal_strength) == AEE_SUCCESS)
         {
            pMe->m_CurSigStrength = signal_strength;
            FMRadioApp_UpdateSignalStrength(pMe);
         }
         ret_val = TRUE;
      }
   }
   IDisplay_Update(pMe->a.m_pIDisplay);

   return ret_val;
} /*End FMRadioApp_UpdateMainDlg */

/*===========================================================================
FUNCTION FMRadioApp_InitMainDlg

DESCRIPTION
  Initializing the application data

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
static boolean FMRadioApp_InitMainDlg(FMRadioApp *pMe, IDialog* pIDialog)
{
   FMRadio_Pref pref_data;
   IFile *pref_file;
   AEERadioBand band;
   boolean ret_val = FALSE;
   
   pref_data.m_Band = FMRADIO_BAND_OTHERS;
   pref_data.m_Gap	= FMRADIO_DEFAULT_GAP;

   /* If preference file does not exit, create one */
   if ((pref_file = IFILEMGR_OpenFile(pMe->m_pIFileMgr, FMRADIO_PREF_FILE, _OFM_READWRITE)) == NULL)
   {
      pref_file = IFILEMGR_OpenFile(pMe->m_pIFileMgr, FMRADIO_PREF_FILE, _OFM_CREATE);

      if (pref_file != NULL)
      {
         IFILE_Write(pref_file, &pref_data, sizeof(pref_data));
         IFILE_Release(pref_file);
         ret_val = TRUE;
      }
   }
   else
   {
      /* If unable to read the entire data, write and use default data */
      if (IFILE_Read(pref_file, &pref_data, sizeof(pref_data)) != sizeof(pref_data) ||
          ((pref_data.m_Band != FMRADIO_BAND_JAPAN) &&
           (pref_data.m_Band != FMRADIO_BAND_OTHERS)))
      {
         IFILE_Seek(pref_file, _SEEK_START, 0);
         IFILE_Write(pref_file, &pref_data, sizeof(pref_data));
      }
      IFILE_Release(pref_file);
      ret_val = TRUE;
   }

   if (ret_val)
   {
      band = (pref_data.m_Band == FMRADIO_BAND_OTHERS) ? AEEFMRADIO_BAND_OTHERS : AEEFMRADIO_BAND_JAPAN;
      pMe->m_FreqGap = (int)((pref_data.m_Gap - '0')) * 100;

      // Initialize the Signal Strength change call back structure
      pMe->m_SignalStrengthCB.pfnCancel = NULL;
      pMe->m_SignalStrengthCB.pfnNotify = FMRadioApp_SignalStrengthCB;
      pMe->m_SignalStrengthCB.pNotifyData = pMe;

      // Register for signal strength changes
      IFMRadio_OnEventNotify(pMe->m_pIFMRadio, &pMe->m_SignalStrengthCB);

	  //Intialize the FMRDS data available call back structure
      pMe->m_FMRDSInfoCB.pfnCancel = NULL;
	  pMe->m_FMRDSInfoCB.pfnNotify = FMRadioApp_FMRDSInfoCB;
	  pMe->m_FMRDSInfoCB.pNotifyData = pMe;

	  //Register for the RDS info available callback
	  IFMRDS_InfoReadable(pMe->m_pIFMRDS,
					      &pMe->m_FMRDSInfoCB,
					      AEEFMRDS_INFO_0A,
					      AEEFMRDS_PARSED_DATA);
	  
	  pMe->m_radio_text[0] = NULL;
      pMe->m_rt_tmp[0] = NULL;

      // Initialize the Long Key Press call back structure
      pMe->m_GetStrongChannelCB.pfnCancel = NULL;
      pMe->m_GetStrongChannelCB.pfnNotify = FMRadioApp_GetStrongChannelCB;
      pMe->m_GetStrongChannelCB.pNotifyData = pMe;

      pMe->m_FMCallback.pfnCancel = NULL;
      pMe->m_FMCallback.pfnNotify = FMRadioApp_SetBandCB;
      pMe->m_FMCallback.pNotifyData = pMe;

      IFMRadio_SetBand(pMe->m_pIFMRadio,
                       band,
                       &pMe->m_FMCallbackResult,
                       &pMe->m_FMCallback);
   }

   return ret_val;
}


/*===========================================================================
FUNCTION FMRadioApp_InitErrDlg

DESCRIPTION
  Initializing the error dialog

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
static boolean FMRadioApp_InitErrDlg(FMRadioApp *pMe, IDialog* pIDialog)
{
   IStatic *pErrTitle = NULL;
   IMenuCtl *pIMenu = NULL;
   AECHAR buffer1[FMRADIO_MAX_STR_SIZE];
   AECHAR buffer2[FMRADIO_MAX_STR_SIZE];
   int nResult1 = 0, nResult2 = 0;
   boolean ret_val = FALSE;

   if (pMe == NULL || pIDialog == NULL)
   { /* error */
      return FALSE;
   }

   pErrTitle  = (IStatic*)IDIALOG_GetControl(pIDialog, (int16) IDC_STATIC_FMRADIO_UNAVAIL);
   pIMenu  = (IMenuCtl*)IDIALOG_GetControl(pIDialog, (int16) IDC_SK_FMRADIO_OK);

   if (pIMenu != NULL && pErrTitle != NULL)
   {
      /* Set the softkey menu style */
      FMRadioApp_SetDefaultMenuLook(pMe->a.m_pIShell, pIMenu);

      /* Set the title and content */
      nResult1 = ISHELL_LoadResString(pMe->a.m_pIShell,
                                      gpFMRadioResourceFile,
                                      IDS_FMRADIO_TITLE,
                                      buffer1,
                                      ARR_SIZE(buffer1)*sizeof(AECHAR));

      nResult2 = ISHELL_LoadResString(pMe->a.m_pIShell,
                                      gpFMRadioResourceFile,
                                      IDS_FMRADIO_UNAVAIL,
                                      buffer2,
                                      ARR_SIZE(buffer2)*sizeof(AECHAR));

      if (nResult1 != 0 && nResult2 != 0)
      {
         ISTATIC_SetProperties(pErrTitle, ST_CENTERTEXT|ST_CENTERTITLE|ST_MIDDLETEXT);
         ISTATIC_SetActive(pErrTitle, TRUE);
         FMRadioApp_UpdateStaticCtl(pMe, IDC_STATIC_FMRADIO_UNAVAIL, buffer1, buffer2, 
                                    AEE_FONT_BOLD, AEE_FONT_NORMAL);

         ret_val = TRUE;
      }
   }

   return ret_val;
} /*End FMRadioApp_InitErrDlg */

/*===========================================================================
FUNCTION FMRadioApp_InitRDSInfoDlg

DESCRIPTION
  Initializing the RDS Info dialog

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
static boolean FMRadioApp_InitRDSInfoDlg(FMRadioApp *pMe, IDialog* pIDialog)
{
   IStatic *pTitle = NULL;
   IStatic *pFreq = NULL;
   IStatic *pPI = NULL;
   IStatic *pPTY = NULL;
   IStatic *pText = NULL;
   IMenuCtl *pIMenu = NULL;
   AECHAR buffer[FMRADIO_MAX_STR_SIZE+2*FMRADIO_RADIOTEXT_SIZE];
   int nResult = 0;
   boolean ret_val = FALSE;

   if (pMe == NULL || pIDialog == NULL)
   { /* error */
      return FALSE;
   }

   pTitle = (IStatic*)IDIALOG_GetControl(pIDialog, (int16) IDC_RDSINFO_TITLE);
   pFreq = (IStatic*)IDIALOG_GetControl(pIDialog, (int16) IDC_CUR_FREQ);
   pPI = (IStatic*)IDIALOG_GetControl(pIDialog, (int16) IDC_PI);
   pPTY = (IStatic*)IDIALOG_GetControl(pIDialog, (int16) IDC_PTY);
   pText = (IStatic*)IDIALOG_GetControl(pIDialog, (int16) IDC_RADIO_TEXT);
   pIMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, (int16) IDC_SK_RDSINFO);

   if (pTitle == NULL || pFreq == NULL || pPI == NULL || 
       pPTY == NULL || pText == NULL || pIMenu == NULL)
   {
      return FALSE;
   }

   /* Set the softkey menu style */
   FMRadioApp_SetDefaultMenuLook(pMe->a.m_pIShell, pIMenu);

   /* Set the title and content */
   nResult = ISHELL_LoadResString(pMe->a.m_pIShell,
                                  gpFMRadioResourceFile,
                                  IDS_RDSDATA,
                                  buffer,
                                  ARR_SIZE(buffer)*sizeof(AECHAR));

   if (nResult == 0)
   {
      return FALSE;
   }
   
   ISTATIC_SetProperties(pTitle, ST_CENTERTEXT|ST_CENTERTITLE);
   FMRadioApp_UpdateStaticCtl(pMe,IDC_RDSINFO_TITLE, buffer, NULL, 
                              AEE_FONT_BOLD, AEE_FONT_NORMAL);

   

   /* Update PI, PTY and Radio Text */
   if(pMe->m_rt_tmp[0]!=NULL && pMe->m_nPTYCode != 0 && pMe->m_nPICode != 0)
   {
      AECHAR format[] = {'%', 's', ':', ' ', '%', 'd', '\0'};
      AECHAR fmt_txt[] = {'%', 's', ':', ' ', '%', 's', '\0'};

      /* Update PI */
      nResult = ISHELL_LoadResString(pMe->a.m_pIShell,
                                     gpFMRadioResourceFile,
                                     IDS_PI,
                                     pMe->m_rt_tmp,
                                     ARR_SIZE(pMe->m_rt_tmp)*sizeof(AECHAR));

      if (nResult == 0)
      {
         return FALSE;
      }

      WSPRINTF(buffer, ARR_SIZE(buffer)*sizeof(AECHAR), format, pMe->m_rt_tmp, pMe->m_nPICode);
	  
      FMRadioApp_UpdateStaticCtl(pMe, IDC_PI, NULL, buffer,
                                 AEE_FONT_BOLD, AEE_FONT_NORMAL);

      /* Update PTY */
      nResult = ISHELL_LoadResString(pMe->a.m_pIShell,
                                     gpFMRadioResourceFile,
                                     IDS_PTY,
                                     pMe->m_rt_tmp,
                                     ARR_SIZE(pMe->m_rt_tmp)*sizeof(AECHAR));

      if (nResult == 0)
      {
         return FALSE;
      }

      WSPRINTF(buffer, ARR_SIZE(buffer)*sizeof(AECHAR), format, pMe->m_rt_tmp, pMe->m_nPTYCode);
	  
      FMRadioApp_UpdateStaticCtl(pMe, IDC_PTY, NULL, buffer,
                                 AEE_FONT_BOLD, AEE_FONT_NORMAL);

      /* Update Radio Text */
      nResult = ISHELL_LoadResString(pMe->a.m_pIShell,
                                     gpFMRadioResourceFile,
                                     IDS_RADIO_TEXT,
                                     pMe->m_rt_tmp,
                                     ARR_SIZE(pMe->m_rt_tmp)*sizeof(AECHAR));

      if (nResult == 0)
      {
         return FALSE;
      }

	  WSPRINTF(buffer, ARR_SIZE(buffer)*sizeof(AECHAR), fmt_txt, pMe->m_rt_tmp, pMe->m_radio_text);

      FMRadioApp_UpdateStaticCtl(pMe, IDC_RADIO_TEXT,  NULL, buffer,
                                 AEE_FONT_BOLD, AEE_FONT_NORMAL);
    }
	else
	{
	  
	  AECHAR buffer[FMRADIO_MAX_STR_SIZE];
	  int nResult = ISHELL_LoadResString(pMe->a.m_pIShell,
									     gpFMRadioResourceFile,
										 IDS_STRING_NORDSINFO,
									     buffer,
									     ARR_SIZE(buffer)*sizeof(AECHAR));
	  
	  if (nResult == 0)
	  {
	    return FALSE;
	  }
	  ISTATIC_SetProperties(pText, ST_CENTERTEXT|ST_MIDDLETEXT);
	 
	  // Display "RDS Data Not available" in place of signal strength
	  FMRadioApp_UpdateStaticCtl(pMe, IDC_RADIO_TEXT, NULL, buffer, AEE_FONT_NORMAL, AEE_FONT_NORMAL);
	}
   /* Update freq field */
    {
      AECHAR tmp[FMRADIO_MAX_STR_SIZE];
      AECHAR format[] = {'%', 's', ':', ' ', '%', 'd', '.', '%', 'd', '\0'};
      int cur_freq;
      nResult = ISHELL_LoadResString(pMe->a.m_pIShell,
                                     gpFMRadioResourceFile,
                                     IDS_FREQUENCY,
                                     tmp,
                                     ARR_SIZE(tmp)*sizeof(AECHAR));

      if ((nResult == 0) || 
          (IFMRadio_GetTunedStationFrequency(pMe->m_pIFMRadio, &cur_freq) != SUCCESS))
      {
         return FALSE;
      }


      WSPRINTF(buffer, ARR_SIZE(buffer)*sizeof(AECHAR), format, tmp, cur_freq/100, (cur_freq%100)/10);

	  FMRadioApp_UpdateStaticCtl(pMe, IDC_CUR_FREQ,NULL,buffer,
                                 AEE_FONT_BOLD, AEE_FONT_NORMAL);
   }

   IDisplay_Update(pMe->a.m_pIDisplay);

   return ret_val;
} /*End FMRadioApp_InitRDSInfoDlg */
/*===========================================================================
FUNCTION FMRadioApp_RDSInfoDlgHandler

DESCRIPTION
  Initializing the RDS Info dialog

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
boolean FMRadioApp_RDSInfoDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   FMRadioApp *pMe = (FMRadioApp *)pUser;

#ifdef FEATURE_BREW_3_0
   // do not want to handle autorepeat key events
   if ((evt == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
   {
      return FALSE;
   }
#endif

   if (pMe == NULL)
   {
      return FALSE;
   }

   switch(evt)
   {
   case EVT_DIALOG_START:
      {
         FMRadioApp_InitRDSInfoDlg(pMe, (IDialog *)dwParam);
         return TRUE;
      }

   case EVT_DIALOG_INIT:
      {
         return TRUE;
      }

   case EVT_DIALOG_END:
      {
         return TRUE;
      }

   case EVT_COMMAND:
      {
         if (wParam == IDL_BACK)
         {
            int nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
            return FMCheckAEEReturnStatus(nReturnStatus);
         }

         return FALSE;
      }
   case EVT_KEY:
      {
         if (wParam == AVK_CLR)
         {
            int nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
            return FMCheckAEEReturnStatus(nReturnStatus);
         }

         return FALSE;
      }

   default:
      {
         break;
      }
   }

   return FALSE;
}
/*===========================================================================
FUNCTION FMRadioApp_MainDlgHandler

DESCRIPTION
  Initializing the RDS Info dialog

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
boolean FMRadioApp_MainDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   FMRadioApp *pMe = (FMRadioApp *)pUser;

#ifdef FEATURE_BREW_3_0
   // do not want to handle autorepeat key events
   if ((evt == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
   {
      return FALSE;
   }
#endif

   if (pMe == NULL)
   {
      return FALSE;
   }

   switch(evt)
   {
   case EVT_DIALOG_START:
      {
         IDialog *pIDialog = (IDialog *)dwParam;

         if (wParam == IDD_FMRADIO)
         {
            return FMRadioApp_InitMainDlg(pMe, pIDialog);
         }

         return FALSE;
      }
   case EVT_DIALOG_INIT:
      {
         return TRUE;
      }

   case EVT_DIALOG_END:
      {
         /* Save the current frequency and volume level */
         IFile *config_file = IFILEMGR_OpenFile(pMe->m_pIFileMgr, FMRADIO_CFG_FILE, _OFM_READWRITE);
         if (config_file != NULL)
         {
            FMRadio_Config config_data;

            config_data.m_LastFreq = pMe->m_CurFreq;
            config_data.m_LastVol = pMe->m_CurVol;
            IFILE_Write(config_file, &config_data, sizeof(config_data));
            IFILE_Release(config_file);
         }
         return TRUE;
      }

   case EVT_KEY:
      {
         switch (wParam)
         {
         case AVK_LEFT:
            {
               if (pMe->m_CurVol - 1 >= 0)
               {
                  --pMe->m_CurVol;
               }
               FMRadioApp_UpdateVolume(pMe);
               return TRUE;
            }

         case AVK_RIGHT:
            {
               if (pMe->m_CurVol + 1 <= FMRADIO_MAX_VOLUME)
               {
                  ++pMe->m_CurVol;
               }
               FMRadioApp_UpdateVolume(pMe);
               return TRUE;
            }

         case AVK_SOFT1:
            {
               int nReturnStatus = ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
               return FMCheckAEEReturnStatus(nReturnStatus);
            }

         case AVK_SOFT2:

            {
               int nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell, gpFMRadioResourceFile,
                                                       IDD_RDSINFO, NULL);
               return FMCheckAEEReturnStatus(nReturnStatus);
            }
 
            return TRUE;
         }
         return FALSE;
      }

   case EVT_KEY_PRESS:
      {
         if (wParam == AVK_UP || wParam == AVK_DOWN)
         {
            int nReturnStatus;
            pMe->m_PrevKeyPress = wParam;

            nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                                            FMRADIO_LONGKEYPRESS,
                                            (PFNNOTIFY) FMRadioApp_LongKeyPressCB,
                                            (uint32*) pMe);

            (void)FMCheckAEEReturnStatus(nReturnStatus);

            return TRUE;
         }

         return FALSE;
      }

   case EVT_KEY_RELEASE:
      {
         if (wParam == AVK_UP || wParam == AVK_DOWN)
         {
            ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) FMRadioApp_LongKeyPressCB, pMe);

            if (!pMe->m_bLongKeyPress)
            {
               int freq = pMe->m_CurFreq;
               if (wParam == AVK_UP)
               {
                  freq = (freq + pMe->m_FreqGap > pMe->m_MaxFreq) ? pMe->m_MinFreq : freq + pMe->m_FreqGap;
               }
               else
               {
                  freq = (freq - pMe->m_FreqGap < pMe->m_MinFreq) ? pMe->m_MaxFreq : freq - pMe->m_FreqGap;
               }

               //--->> BG <<---
               IFMRadio_EnablePlay(pMe->m_pIFMRadio,FALSE);
               //--->> BG <<---

               // Set the new frequency in IFMRadio interface
               IFMRadio_SetStation(pMe->m_pIFMRadio, freq, 
                                   &pMe->m_SetStationCBResult, &pMe->m_SetStationCB);
            }

            pMe->m_bLongKeyPress = FALSE;
            pMe->m_PrevKeyPress = AVK_UNDEFINED;
            return TRUE;
         }
         return FALSE;
      }

   default:
      break;
   }

   return FALSE;
}

/*===========================================================================
FUNCTION FMRadioApp_CreateMainDialog

DESCRIPTION
   This function createst the main FM Radio dialog or the error dialog depending
   on if FM Radio is available or not.

DEPENDENCIES
   FMRADIO_POST_CTOR_DELAY has elapsed

SIDE EFFECTS
  None
===========================================================================*/
static void FMRadioApp_CreateMainDialog(FMRadioApp *pMe)
{
   int nErrCode;
   boolean is_fm_avail;

   if (pMe == NULL)
   {
      return;
   }

   if (IFMRadio_IsServiceAvailable(pMe->m_pIFMRadio, &is_fm_avail) == AEE_SUCCESS && 
       is_fm_avail == TRUE)
   {
      nErrCode = ISHELL_CreateDialog(pMe->a.m_pIShell, gpFMRadioResourceFile,
                                     IDD_FMRADIO, NULL);
   }
   else
   {
      nErrCode = ISHELL_CreateDialog(pMe->a.m_pIShell, gpFMRadioResourceFile,
                                     IDD_FMRADIO_UNAVAIL, NULL);
   }

   (void)FMCheckAEEReturnStatus(nErrCode);
}

/*===========================================================================

FUNCTION FMRadioApp_HandleEvent

DESCRIPTIN
   This is the EventHandler for this app. All events to this app are handled in this
   function. All APPs must supply an Event Handler.

PROTOTYPE:
   boolean FMRadioApp_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)

PARAMETERS:
   pi: Pointer to the AEEApplet structure. This structure contains information specific
   to this applet. It was initialized during the AppCreateInstance() function.

   ecode: Specifies the Event sent to this applet

   wParam, dwParam: Event specific data.

DEPENDENCIES
   none

RETURN VALUE
   TRUE: If the app has processed the event
   FALSE: If the app did not process the event

SIDE EFFECTS
   none

COMMENTS 
===========================================================================*/
static boolean FMRadioApp_HandleEvent(FMRadioApp * pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
   if (pMe == NULL)
   {
      return FALSE;
   }

#ifdef FEATURE_BREW_3_0
   // do not want to handle autorepeat key events
   if ((eCode == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
   {
      return FALSE;
   }
#endif

   switch (eCode)
   {
   case EVT_APP_START:
      {
         AEEAppStart *aStart = (AEEAppStart*)dwParam;

         if (aStart == NULL)
         {
            return FALSE;
         }

         // Wait for the IFMRadio, IFMRDS constructors to complete
         ISHELL_SetTimer(pMe->a.m_pIShell, FMRADIO_POST_CTOR_DELAY, (PFNNOTIFY)FMRadioApp_CreateMainDialog, pMe);
         return TRUE;
      }

   case EVT_DIALOG_INIT:
      {
         IDialog *pIDialog = (IDialog *)dwParam;

         if (wParam == IDD_FMRADIO)
         {
            IDIALOG_SetEventHandler(pIDialog, FMRadioApp_MainDlgHandler, pMe);
            IDIALOG_SetProperties(pIDialog, DLG_HANDLE_ALL_EVENTS);
            return TRUE;
         }
         else if (wParam == IDD_RDSINFO)
         {
            IDIALOG_SetEventHandler(pIDialog, FMRadioApp_RDSInfoDlgHandler, pMe); 
            IDIALOG_SetProperties(pIDialog, DLG_HANDLE_ALL_EVENTS);
            return TRUE;
         }
         else if (wParam == IDD_FMRADIO_UNAVAIL)
         {
            return TRUE;
         }

         return FALSE;
      }

   case EVT_DIALOG_START:
      if (wParam == IDD_FMRADIO_UNAVAIL)
      {
         IDialog *pIDialog = (IDialog *)dwParam;

         FMRadioApp_InitErrDlg(pMe, pIDialog);
         return TRUE;
      }
      return FALSE;

   case EVT_DIALOG_END:
      if (wParam == IDD_FMRADIO_UNAVAIL)
      {
         return TRUE;
      }
      return FALSE;

   case EVT_APP_STOP:
      {
         ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY)FMRadioApp_CreateMainDialog, pMe);
         return TRUE;
      }

   case EVT_APP_SUSPEND:
      {
         /* Exiting the FMRadioApp when Suspend Event is received */  
		 int nReturnStatus;
         IFile *config_file = IFILEMGR_OpenFile(pMe->m_pIFileMgr, FMRADIO_CFG_FILE, _OFM_READWRITE);
         if (config_file != NULL)
         {
            FMRadio_Config config_data;

            config_data.m_LastFreq = pMe->m_CurFreq;
            config_data.m_LastVol = pMe->m_CurVol;
            IFILE_Write(config_file, &config_data, sizeof(config_data));
            IFILE_Release(config_file);
         }

         nReturnStatus = ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
         return FMCheckAEEReturnStatus(nReturnStatus);
      }

   case EVT_APP_RESUME:
      {
         return TRUE;
      }

   case EVT_KEY:
      {
         if (wParam == AVK_SOFT1)
         {
            int nReturnStatus = ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
            return FMCheckAEEReturnStatus(nReturnStatus);
         }
         return FALSE;
      }

   case EVT_COMMAND:
      {
         if (wParam == IDL_FMRADIO_OK)
         {
            int nReturnStatus = ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
            return FMCheckAEEReturnStatus(nReturnStatus);
         }

         return FALSE;
      }

   default:
      {
         break;
      }
   }

   return FALSE;
}  /* End FMRadioApp_HandleEvent */


/*===========================================================================

FUNCTION: AEEClsCreateInstance

DESCRIPTION
  This function is invoked while the app is being loaded. All Modules must provide this
  function. Ensure to retain the same name and parameters for this function.
  In here, the module must verify the ClassID and then invoke the AEEApplet_New() function
  that has been provided in AEEAppGen.c.

   After invoking AEEApplet_New(), this function can do app specific initialization. In this
   example, a generic structure is provided so that app developers need not change app specific
   initialization section every time except for a call to InitAppData(). This is done as follows:
   InitAppData() is called to initialize AppletData instance. It is app developers
   responsibility to fill-in app data initialization code of InitAppData(). App developer
   is also responsible to release memory allocated for data contained in AppletData -- this can be
   done in FreeAppData().

PROTOTYPE:
  int AEEAppCreateInstance(AEECLSID clsID, IShell* pIShell, IModule* pIModule,IApplet** ppApplet)

PARAMETERS:
  clsID: [in]: Specifies the ClassID of the applet which is being loaded

  pIShell: [in]: Contains pointer to the IShell interface.

  pIModule: pin]: Contains pointer to the IModule interface to the current module to which
  this app belongs

  ppApplet: [out]: On return, *ppApplet must point to a valid AEEApplet structure. Allocation
  of memory for this structure and initializing the base data members is done by AEEApplet_New().

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS: If the app needs to be loaded and if AEEApplet_New() invocation was successful
  EFAILED: If the app does not need to be loaded or if errors occurred in AEEApplet_New().
  If this function returns FALSE, the app will not be loaded.

SIDE EFFECTS
  none
===========================================================================*/
#if defined(AEE_STATIC)
int FMRadioApp_CreateInstance(AEECLSID ClsId,IShell *pIShell,IModule *pMod,void **ppObj)
#else
int AEEClsCreateInstance(AEECLSID ClsId,IShell *pIShell,IModule *pMod,void **ppObj)
#endif
{
   *ppObj = NULL;

   if (AEEApplet_New( sizeof(FMRadioApp),                     /* Size of our private class */
                      ClsId,                                  /* Our class ID */
                      pIShell,                                /* Shell interface */
                      pMod,                                   /* Module instance */
                      (IApplet**)ppObj,                       /* Return object */
                      (AEEHANDLER)FMRadioApp_HandleEvent,     /* Our event handler */
                      (PFNFREEAPPDATA)FMRadioApp_FreeAppData))/* Cleanup function */
   {
      if (FMRadioApp_InitAppData((IApplet*)*ppObj))
      {
         return(AEE_SUCCESS);
      }
      //Release the applet. This will free the memory allocated for the applet when
      (void)IAPPLET_Release(((IApplet*)*ppObj));
   }
   return(EFAILED);
} /* End FMRadioApp_CreateInstance / AEEClsCreateInstance */



#ifdef AEE_STATIC
/*=============================================================================
FUNCTION: FMRadioApp_Load

DESCRIPTION:
    This function loads the application.

PROTOTYPE:
static int AUXSettings_Load(IShell *ps, void * pHelpers, IModule ** pMod)

PARAMETERS:
--> ps: Pointer to ISHELL interface
--> pHelpers:
--> pMod:

RETURN VALUE:


COMMENTS:


SIDE EFFECTS:
none

SEE ALSO:
none
=============================================================================*/
int FMRadioApp_Load(IShell *ps, void *pHelpers, IModule **pMod)
{
   return(AEEStaticMod_New((int16)(sizeof(AEEMod)),ps,pHelpers,pMod,
                           FMRadioApp_CreateInstance,NULL));
} /* End FMRadioApp_Load */
#endif

