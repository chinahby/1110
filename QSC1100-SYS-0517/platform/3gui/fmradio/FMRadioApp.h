#ifndef FMRADIO_H
#define FMRADIO_H
/*=============================================================================

FILE: FMRadio.h

SERVICES: QCT FM Radio Application

GENERAL DESCRIPTION:
.h file for FM Radio App.

(c) COPYRIGHT 2007 QUALCOMM Incorporated.
                   All Rights Reserved.

                   QUALCOMM Proprietary
=============================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/20/07   sg      Initial revision.
===========================================================================*/

/*=============================================================================

                       INCLUDES

=============================================================================*/
//#include "customer.h"

/* Brew libraries */
//#include "OEMFeatures.h"
#include "AEE.h"
#include "AEEShell.h"
#include "AEEAppGen.h"
#include "AEEMenu.h"
#include "AEEIFMRadio.h"
#include "AEEIFMRDS.h"

/*=============================================================================

                       AUX APP USER EVENTS

=============================================================================*/
#define FMRADIO_MAX_STR_SIZE 50
#define FMRADIO_RADIOTEXT_SIZE 64

/*=============================================================================

                       DATA STRUCTURE

=============================================================================*/
typedef struct
{
  AEEApplet a;    /* First element of this structure must be AEEApplet. */

  IFMRadio *m_pIFMRadio;      /* FM Radio object */
  IFMRDS   *m_pIFMRDS;        /* FM RDS object */
  IFileMgr *m_pIFileMgr;      /* File manager for storing volume, station info */
  ISound   *m_pISound;        /* ISound class pointer */
  int       m_MinFreq;        /* Minimum Freq for the current band */
  int       m_MaxFreq;        /* Maximum Freq for the current band */
  int       m_CurFreq;        /* Current freq */
  int       m_FreqGap;        /* Frequency step size */
  int       m_CurVol;         /* Current volume level */
  int       m_CurSigStrength; /* Current signal strength */
  int       m_MaxSigStrength; /* Max signal strength */
  int       m_StrongChannel;  /* Next strongest channel */

  /* Geneal callback */
  AEECallback m_FMCallback;
  int       m_FMCallbackResult;
  /* Set station callback */
  AEECallback m_SetStationCB;
  int       m_SetStationCBResult;
  /* Callback for signal strength changes */
  AEECallback m_SignalStrengthCB;
  int       m_SignalStrengthCBResult;
  /* Callback for long key press tuning */
  AEECallback m_GetStrongChannelCB;
  int       m_GetStrongChannelCBResult;
  /*Callback for RDS info*/
  AEECallback m_FMRDSInfoCB;
  int m_FMRDSInfoCBResult;
  AECHAR m_radio_text[2*FMRADIO_RADIOTEXT_SIZE+1];
  AECHAR m_nPICode;
  uint8  m_nPTYCode;
  AECHAR m_rt_tmp[FMRADIO_RADIOTEXT_SIZE+1];

  AECHAR    m_PrevKeyPress;   /* Last key pressed */
  boolean   m_bLongKeyPress;  /* Whether there has been a long key press. */
  AEERadioSearchDirection m_SearchDir;
  int       m_NumChannels;
  AECHAR    m_VolumeStr[FMRADIO_MAX_STR_SIZE];
  AECHAR    m_SignalStr[FMRADIO_MAX_STR_SIZE];
} FMRadioApp;

extern char *gpFMRadioResourceFile;

#define FMCheckAEEReturnStatus(nReturnStatus)  ((nReturnStatus) == SUCCESS? TRUE: FALSE)

/*=============================================================================

                       FUNCTION DECLARATIONS

=============================================================================*/

#endif /* FMRADIO_H */
