#ifndef AEECARDSESSIONNOTIFIER_H
#define AEECARDSESSIONNOTIFIER_H

/*============================================================================
FILE:  AEECardSessionNotifier.h

SERVICES:  AEE ICardSession Notifier Interface

GENERAL DESCRIPTION:
        Base level definitions, typedefs, etc.

        Copyright © 2006-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/icardsession/rel/07H1_2/inc/AEECardSessionNotifier.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/07/07   sk      Added support for NAA Refresh
10/25/06   tml     Added initial support for Universal PIN replacement support
08/25/06   tml     Added Refresh Event Notification support 
08/14/06   tml     Initial release.
=============================================================================*/


#include "AEE.h"
#include "AEECardSession.h"

/**********************************************************************

 ICardSessionNotifier Interface

*********************************************************************/

/*===========================================================================

                    TYPE DEFINITIONS

===========================================================================*/
/* Event Mask for ICardSessionNotifier */
#define NMASK_CARDSESSION_CARD_EVENT                 0x01
#define NMASK_CARDSESSION_PIN_EVENT                  0x02
#define NMASK_CARDSESSION_PERSONALIZATION_EVENT      0x04

/* NMASK_CARDSESSION_CARD_EVENT's event indication */
#define AEECARDSESSION_NOTIFIER_CARD_INSERTED   1
#define AEECARDSESSION_NOTIFIER_APP_SELECTED    2
#define AEECARDSESSION_NOTIFIER_CARD_ERROR      3
#define AEECARDSESSION_NOTIFIER_CARD_READY      4
#define AEECARDSESSION_NOTIFIER_CARD_REFRESH    5

/* NMASK_CARDSESSION_PIN_EVENT's event indication */
#define AEECARDSESSION_NOTIFIER_PIN_ENABLED_NOT_VERIFIED  1
#define AEECARDSESSION_NOTIFIER_PIN_DISABLED              2
#define AEECARDSESSION_NOTIFIER_PIN_ENABLED_VERIFIED      3
#define AEECARDSESSION_NOTIFIER_PIN_UNBLOCKED             4
#define AEECARDSESSION_NOTIFIER_PIN_CHANGED               5
#define AEECARDSESSION_NOTIFIER_PIN_BLOCKED               6
#define AEECARDSESSION_NOTIFIER_PIN_PERM_BLOCKED          7

/* PIN Replacement for PIN EVENT */
#define AEECARDSESSION_NOTIFIER_PIN_REPLACE_NOT_APPLICABLE     1
#define AEECARDSESSION_NOTIFIER_PIN_REPLACED_BY_UNIVERSAL      2
#define AEECARDSESSION_NOTIFIER_PIN_NOT_REPLACED_BY_UNIVERSAL  3

/* Refresh Mode for AEECARDSESSION_NOTIFIER_CARD_REFRESH */
#define AEECARDSESSION_NOTIFIER_REFRESH_RESET              1
#define AEECARDSESSION_NOTIFIER_REFRESH_SIM_INIT           2
#define AEECARDSESSION_NOTIFIER_REFRESH_SIM_INIT_FCN       3
#define AEECARDSESSION_NOTIFIER_REFRESH_FCN                4
#define AEECARDSESSION_NOTIFIER_REFRESH_SIM_INIT_FULL_FCN  5
#define AEECARDSESSION_NOTIFIER_REFRESH_APP_RESET          6
#define AEECARDSESSION_NOTIFIER_REFRESH_3G_SESSION_RESET   7

/* Refresh Stage for AEECARDSESSION_NOTIFIER_CARD_REFRESH */
#define AEECARDSESSION_NOTIFIER_REFRESH_STAGE_START                       1
#define AEECARDSESSION_NOTIFIER_REFRESH_STAGE_IN_PROGRESS_APP_DEACTIVATED 2
#define AEECARDSESSION_NOTIFIER_REFRESH_STAGE_IN_PROGRESS_APP_ACTIVATED   3
#define AEECARDSESSION_NOTIFIER_REFRESH_STAGE_END_SUCCESS                 4
#define AEECARDSESSION_NOTIFIER_REFRESH_STAGE_END_FAILED                  5

/* This structure contains the event structure when an 
** AEECARDSESSION_NOTIFIER_CARD_INSERTED event is being sent out.
** pAppIDList contains the list of Application ID determined by nNumApp 

DESCRIPTION: 
   dwNumApp    - Number of Application in case of UICC
   aAppIDList  - The UICC/ICC Application List.  
*/
typedef struct
{ 
  uint32                 dwNumApp;
  AEECardSessionAppData  aAppIDList[AEECARDSESSION_MAX_APPS]; 
}AEECardSessionCardInsertedData;

/* This structure contains the event structure when an 
** AEECARDSESSION_NOTIFIER_APP_SELECTED event is being sent out.  
** The pAppID indicates the application ID that has been selected 
** for provisioning information.  
** This event is applicable only when a UICC card is inserted. 

DESCRIPTION: 
   nChannelID  - Channel ID to which the notification came from
   aAppID      - The UICC Application ID being selected. 
*/
typedef struct
{ 
  int32                    nChannelID;
  AEECardSessionAppData    aAppID; 
}AEECardSessionAppSelectedData;

/* This structure contains the data structure for Application Info
** This includes the App data as well as the pin data

DESCRIPTION: 
   dwTech      - AEECARDSESSION_SUB_SIM or
                 AEECARDSESSION_SUB_RUIM or
                 AEECARDSESSION_SUB_UICC
   nAidLen     - Length of the Aid (applicable if UICC)
   aAid        - Aid (applicable if UICC)
   aLabel      - Label (applicable if UICC)
   dwPin1Status, dwPuk1Status,
   dwPinUniversalStatus        - AEECARDSESSION_PIN_ENABLED_VERIFIED, 
                                 AEECARDSESSION_PIN_ENABLED_NOT_VERIFIED,
                                 AEECARDSESSION_PIN_DISABLED,
                                 AEECARDSESSION_PIN_BLOCKED,
                                 AEECARDSESSION_PIN_PERM_BLOCKED,
                                 AEECARDSESSION_PIN_NOT_INITIALIZED
   dwPin1NumRetries, dwPuk1NumRetries,
   dwPin2NumRetries, dwPuk2NumRetries,
   dwPinUniversalNumRetries, dwPukUniversalNumRetries
                               - Number of retries left for the PINs and PUKs
*/
typedef struct
{
  uint32                 dwTech;
  int32                  nAidLen;
  uint8                  aAid[AEECARDSESSION_MAX_APP_ID_LEN];
  AECHAR                 aLabel[AEECARDSESSION_MAX_APP_LABEL_LEN];
  uint32                 dwPin1Status;
  uint32                 dwPin1NumRetries;
  uint32                 dwPuk1NumRetries;
  uint32                 dwPin2Status;
  uint32                 dwPin2NumRetries;
  uint32                 dwPuk2NumRetries;
  uint32                 dwPinUniversalStatus;
  uint32                 dwPinUniversalNumRetries;
  uint32                 dwPukUniversalNumRetries;
}AEECardSessionAppInfo;

/* This structure contains the event structure when an 
** AEECARDSESSION_NOTIFIER_CARD_READY event is being sent out.    
** The aAppInfoList is the list valid provisioning applciations.  

DESCRIPTION: 
   dwNumApp    - Number of Applications
   aAppInfoList- The UICC/ICC Application Information, including App ID, Pin.  
*/
typedef struct
{
  uint32                 dwNumApp;
  AEECardSessionAppInfo  aAppInfoList[AEECARDSESSION_MAX_APPS]; 
}AEECardSessionCardReadyData;

/* This structure contains the event structure when an 
** AEECARDSESSION_NOTIFIER_CARD_ERROR event is being sent out.  

DESCRIPTION: 
   aAppIDList  - The UICC/ICC Applications
*/
typedef struct
{ 
  uint32                 dwNumApp;
  AEECardSessionAppData  aAppIDList[AEECARDSESSION_MAX_APPS]; 
}AEECardSessionCardErrorData;

/* This structure contains the event structure when an 
** AEECARDSESSION_NOTIFIER_CARD_REFRESH event is being sent out.  

DESCRIPTION: 
   aAppID        - The UICC Application ID being Refreshed. 
   dwRefreshMode -  AEECARDSESSION_NOTIFIER_REFRESH_RESET             
                    AEECARDSESSION_NOTIFIER_REFRESH_SIM_INIT   
                    AEECARDSESSION_NOTIFIER_REFRESH_SIM_INIT_FCN      
                    AEECARDSESSION_NOTIFIER_REFRESH_FCN               
                    AEECARDSESSION_NOTIFIER_REFRESH_SIM_INIT_FULL_FCN  
                    AEECARDSESSION_NOTIFIER_REFRESH_APP_RESET          
                    AEECARDSESSION_NOTIFIER_REFRESH_3G_SESSION_RESET
   dwRefreshStage - AEECARDSESSION_NOTIFIER_REFRESH_STAGE_START                 
                    AEECARDSESSION_NOTIFIER_REFRESH_STAGE_IN_PROGRESS_APP_DEACTIVATED 
                    AEECARDSESSION_NOTIFIER_REFRESH_STAGE_IN_PROGRESS_APP_ACTIVATED   
                    AEECARDSESSION_NOTIFIER_REFRESH_STAGE_END_SUCCESS                 
                    AEECARDSESSION_NOTIFIER_REFRESH_STAGE_END_FAILED 
*/
typedef struct
{ 
  AEECardSessionAppData       aAppID;
  uint32                      dwRefreshMode;
  uint32                      dwRefreshStage;
  AEECardSessionRefreshData   dwFileList;
}AEECardSessionCardRefreshData;

/* This structure is used for any Card related events notification.  
** Based on the specific card event, different structure of 
** the union should be looked at.

DESCRIPTION: 
   dwCardEvent - AEECARDSESSION_NOTIFIER_CARD_INSERTED or
                 AEECARDSESSION_NOTIFIER_APP_SELECTED or
                 AEECARDSESSION_NOTIFIER_CARD_ERROR or
                 AEECARDSESSION_NOTIFIER_CARD_READY or
                 AEECARDSESSION_NOTIFIER_CARD_REFRESH
   uEventData  - Union structure based on the dwCardEvent
*/
typedef struct
{
  uint32              dwCardEvent;
  union
  {
    AEECardSessionCardInsertedData  sCardInserted;
    AEECardSessionCardReadyData     sCardReady;
    AEECardSessionCardErrorData     sCardError;
    AEECardSessionAppSelectedData   sAppSelected;
    AEECardSessionCardRefreshData   sCardRefresh;
  }uEventData; 
}AEECardSessionCardEventData;

/* This structure is used for any Pin related events notification.  
** This data structure should include slot and Application ID

NOTE: 
For PIN_SAP event, if applicable, the dwNumPinRetries will be set to 0xFF 
and dwNumPukRetries will be set to 0xFF

DESCRIPTION: 
   dwNumApp       - Number of Applications that this pin event is applciable to
   aAppIDList     - The Application where the pin information was applicable. 
   dwPinID        - AEECARDSESSION_PIN_1 or
                    AEECARDSESSION_PIN_2 or
                    AEECARDSESSION_PIN_UNIVERSAL or
                    AEECARDSESSION_PIN_SAP or
                    AEECARDSESSION_PHONE_CODE
   dwPinEvent     - AEECARDSESSION_NOTIFIER_PIN_ENABLED_NOT_VERIFIED or
                    AEECARDSESSION_NOTIFIER_PIN_DISABLED or
                    AEECARDSESSION_NOTIFIER_PIN_ENABLED_VERIFIED or
                    AEECARDSESSION_NOTIFIER_PIN_UNBLOCKED or
                    AEECARDSESSION_NOTIFIER_PIN_CHANGED or
                    AEECARDSESSION_NOTIFIER_PIN_BLOCKED or
                    AEECARDSESSION_NOTIFIER_PIN_PERM_BLOCKED
   dwNumPinRetries - Number of retries for the Pin
   dwNumPukRetries - Number of retries for the Puk
   dwPinReplacement - AEECARDSESSION_NOTIFIER_PIN_REPLACE_NOT_APPLICABLE or
                      AEECARDSESSION_NOTIFIER_PIN_REPLACED_BY_UNIVERSAL or
                      AEECARDSESSION_NOTIFIER_PIN_NOT_REPLACED_BY_UNIVERSAL
*/
typedef struct
{ 
  uint32                 dwNumApp;
  AEECardSessionAppData  aAppIDList[AEECARDSESSION_MAX_APPS];     
  uint32                 dwPinID;
  uint32                 dwPinEvent;
  uint32                 dwNumPinRetries;
  uint32                 dwNumPukRetries;
  uint32                 dwPinReplacement;
}AEECardSessionPinEventData;

#endif    /* #define AEECARDSESSIONNOTIFIER_H */

