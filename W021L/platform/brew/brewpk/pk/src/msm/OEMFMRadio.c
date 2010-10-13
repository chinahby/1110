/*==============================================================================
File:
   OEMFMRadio.c

Services:
   FM Radio reciever related Services

General Description:
   Base level definitions, typedefs, etc. for AEEIFMRadio

Public Classes And Static Functions:
   OEMFMRadio

Initialization and Sequencing Requirements:

Copyright © 2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR

==============================================================================*/

/*=============================================================================

  $Header: //depot/asic/qsc1100/platform/brew/brewpk/pk/src/msm/OEMFMRadio.c#7 $
  $DateTime: 2009/06/04 07:14:24 $
  $Author: seemanta $
  $Change: 931783 $
                      EDIT HISTORY FOR FILE

when         who     what, where, why
----------   ---     ----------------------------------------------------------

=============================================================================*/
#include "customer.h"
#include "AEEIFMRadio.h"
#include "AEEFMRadio.bid"
#include "AEEShell.h"
#include "AEEStdLib.h"


#ifdef FEATURE_FM

#include "fmrds_api.h"
#include "AEE_OEM.h"
#include "OEMFMRadio_priv.h"
#include "snd.h"


/*===========================================================================

                    PRIVATE DATA

===========================================================================*/



/*===========================================================================

                    DATA STRUCTURES

===========================================================================*/

typedef struct {

   const AEEVTBL(IFMRadio) *pvt;

   //ref count
   uint32 m_nRefs;

   //shell ptr
   IShell *m_pIShell;

   // the call back function
   AEECallback *m_pCBs[IFMRADIO_NR_CALLBACKS];

   // the status of the function to be returned in.
   uint32 *m_pCB_Status[IFMRADIO_NR_CALLBACKS];

   // indicates the current Fidelity Mode.
   AEEFMRadioFidelityMode m_nCurrentFidelityMode;

   // indicates the radio mode Tx/Rx mode
   int nRadioMode;

   // indicates the power mode
   int nPwrMode;

   //indicates current band whether U.S.-Europe/Japan,if 1=EU/US and 0= Japan
   //AEERadioBand m_nCurrentBand;

   // indicates the current signal strength, will always be stored in any
   // event cb function handler.
   int m_nCurrentSignalStrength;

   //indicates the previous signal strength stored, we can use this to notify
   //listeners
   int m_nPrevSignalStrength;

   //indicates tuned station frequency
   int m_nTunedStationFreq;

   //stores pointer, for storing scanned channels list.
   int *m_pScanChnlsDest;

   //indicates the length of the scanned channels list
   int m_nScanChnlsDestLen;

   //stores pointer, for the number fo channels actually found.
   int *m_pScanNumChannels;

   //list of events in which third parties are interested
   FMRadio_Callback  *m_pOEMFMEventsCB[(int)FMRDS_EV_MAX_EVENT];

   //Indicates the call back function registered by app
   AEECallback *m_pRadioEventNotifyCB;

   //indicates app registerd events status
   int m_nRadioEventNotifyStatus[EVT_FMRADIO_MAX];

   //if we fail to communicate with driver we set this to TRUE
   boolean  m_bIsDriverDisConnected;

   //Client Id
   int nClientId;

}OEMFMRadio;

/*===========================================================================

                        LOCAL/STATIC Functions

===========================================================================*/
int OEMFMRadio_New(IShell *ps, AEECLSID cls, void **ppObj);

static uint32 OEMFMRadio_AddRef(IFMRadio *pIFMRadio);

static uint32 OEMFMRadio_Release(IFMRadio *pIFMRadio);

static int OEMFMRadio_QueryInterface(IFMRadio  *pIFMRadio,
                                     AEECLSID  clsid,
                                     void      **ppobj);

static void OEMFMRadio_OnEventNotify(IFMRadio     *pIFMRadio,
                                     AEECallback  *pcb);

static int OEMFMRadio_GetEvents(IFMRadio          *pIFMRadio,
                                AEERadioEvents    *pnRadioEvents,
                                int               nSize,
                                int               *pnReqSize);

static void OEMFMRadio_SetStation(IFMRadio      *pIFMRadio,
                                  int           nFrequency,
                                  int           *pnResult,
                                  AEECallback   *pcb);

static int OEMFMRadio_GetTunedStationFrequency(IFMRadio  *pIFMRadio,
                                               int       *pnStationFreq);

static void OEMFMRadio_SetBand(IFMRadio        *pIFMRadio,
                               AEERadioBand    nBand,
                               int             *pnResult,
                               AEECallback     *pcb);

static int OEMFMRadio_EnablePlay(IFMRadio *pIFMRadio,boolean  bVal);

static int OEMFMRadio_GetValidFreqRange(IFMRadio  *pIFMRadio,
                                        int       *pFreqMin,
                                        int       *pFreqMax);

static void OEMFMRadio_GetStrongestChannels(
      IFMRadio                   *pIFMRadio,
      int                        *pnChannels,
      int                        nChannels,
      int                        *pnReqChannels,
      int                        *pnResult,
      AEECallback                *pcb,
      AEERadioSearchDirection    nSearchVal);

static int OEMFMRadio_GetSignalStrength(IFMRadio   *pIFMRadio,
                                        int        *pSignalStrength);

static int OEMFMRadio_GetMaxSignalStrength(IFMRadio  *pIFMRadio,
                                           int       *pMaxSignalStrength);

static int OEMFMRadio_IsFMServiceAvailable(IFMRadio  *pIFMRadio,
                                           boolean   *pBool);

static int OEMFMRadio_SetFidelityMode(IFMRadio                 *pIFMRadio,
                                      AEEFMRadioFidelityMode   nVal);

static int OEMFMRadio_GetFidelityMode(IFMRadio                *pIFMRadio,
                                      AEEFMRadioFidelityMode  *pnVal);

int OEMFMRadioModel_New(IShell * pIShell,AEECLSID ClsId,void ** ppObj);

static void OEMFMRadio_CallBackCancel(AEECallback *pcb);

static void OEMFMRadio_FreeData(OEMFMRadio *pOEMFMRadio);

static void OEMFMRadio_NotifyOEMEventListeners(fmrds_event_msg_type *pFMRDS_Event);

static void OEMFMRadio_NotifyEventListeners(void);

static void OEMFMRadio_FMRDS_Events_CB(fmrds_event_msg_type);

static void OEMFMRadio_Register_Events_CB_Cancel(FMRadio_Callback *pcb);

static void OEMFMRadio_OnEventNotify_CB_Cancel(AEECallback *pcb);

static void OEMFMRadio_Snd_CB_Notify(OEMFMRadio *pMe, snd_status_type status);

static __inline void OEMFMRadio_Notify_SignalStrengthDiff(OEMFMRadio *pMe);

/*===========================================================================

                        LOCAL/STATIC DATA

===========================================================================*/
static const VTBL(IFMRadio) gIFMRadio =
{
  OEMFMRadio_AddRef,
  OEMFMRadio_Release,
  OEMFMRadio_QueryInterface,
  OEMFMRadio_OnEventNotify,
  OEMFMRadio_GetEvents,
  OEMFMRadio_SetStation,
  OEMFMRadio_GetTunedStationFrequency,
  OEMFMRadio_SetBand,
  OEMFMRadio_EnablePlay,
  OEMFMRadio_GetValidFreqRange,
  OEMFMRadio_GetStrongestChannels,
  OEMFMRadio_GetSignalStrength,
  OEMFMRadio_GetMaxSignalStrength,
  OEMFMRadio_IsFMServiceAvailable, 
  OEMFMRadio_SetFidelityMode,
  OEMFMRadio_GetFidelityMode
};


static OEMFMRadio *sOEMFMRadioPtr = NULL;
static OEMFMRadio sOEMFMRadio = {0,};

/* FMRadio settings */
static tsFmrdsRadioSettingsType gfmrdsRadioSettingsTemp = {0,};
static tsFmrdsRadioSettingsType gfmrdsRadioSettingsCurrent = {0,};

/* whether to enable debug messages or not. */
static boolean fmrds_enable_dbgs = TRUE;

#if 0
/* subscribed svc handle */
 static fmrds_sub_handle_type HandleType = {0,};
/*=============================================================================
Function:
   brew_api_subscribe_svc_cb

Description:
   Call back that would be called after subscribing.

Parameters:
  fmrds_handle : The fmrds_sub_handle_type parm

Return Value:
   None

Comments:
   Callback that would be called upon successful subscribing.

Side Effects:

See Aslo:

=============================================================================*/
void brew_api_subscribe_svc_cb(fmrds_sub_handle_type fmrds_handle)
{
   HandleType = fmrds_handle;
}

#endif 
/*=============================================================================
OEMFMRadio_New

Description:
   To create new interface of IFMRadio.

Prototype:
   int OEMFMRadio_New(IShell *ps, AEECLSID cls, void **ppObj)

Parameters:
   *ps     [in]: pointer to IShell object
   cls     [in]: Specifies the class id.
   **ppobj [in\out]: returns the pointer to the newly created object.

Return Value:

   AEE_SUCCESS      : If it was able to initialise the new object.
   ECLASSNOTSUPPORT : If its not a valid class ID.
   ENOMEMORY        : If the Interface was unable to allocate
                      memory.
   EFAILED          : Otherwise.
   EALREADY         : If the service has already been started.

Comments:
   None.

Side Effects:
   None.
=============================================================================*/
int OEMFMRadio_New(IShell *ps, AEECLSID cls, void **ppObj)
{
   int nResult = SUCCESS, nClientId = 0;
   fmrds_cmd_q_status_type nQStat;

   if( NULL == ppObj || AEECLSID_FMRadio != cls ){
      return EBADPARM;
   }

   *ppObj = NULL;

   /* currently this is a singleton interface */
   if( NULL != sOEMFMRadioPtr ){
      return EALREADY;
   }

   do {

      // Registering for Event notification
      nClientId = FmrdsApi_RegisterClient(&OEMFMRadio_FMRDS_Events_CB, FM_CB_EV_MASK_SEND_ALL);
      if(0 == nClientId){
         DBGPRINTF_HIGH("a registration error");
         nResult = EFAILED;
         break;
      }

      //set the FM-RDS device to RADIO_MODE_RX mode
      nQStat = FmrdsApi_SetRadioMode(RADIO_MODE_RX);

#ifdef FEATURE_FMRDS_LEGACY_SUPPORT
      if( FMRDS_CMD_Q_PENDING != nQStat && FMRDS_CMD_Q_SUCCESS != nQStat){
#else
      if( FMRDS_CMD_Q_PENDING != nQStat){
#endif
         DBGPRINTF_HIGH("Failed to set radiomode to FM Receiver: FM+RDS Radio:%d",nQStat);
         nResult = EFAILED;
         break;
      }

      //Power on the FM-RDS hardware & initialization with default settings
      gfmrdsRadioSettingsTemp.radioBand = US_EUROPE_BAND;
      gfmrdsRadioSettingsTemp.deEmphasis = DE_EMP75;
      gfmrdsRadioSettingsTemp.chSpacing = CHSPACE_200_KHZ;
      gfmrdsRadioSettingsTemp.rdsStd = RDS_STD_RBDS; 
      nQStat = FmrdsApi_EnableFm(gfmrdsRadioSettingsTemp);

#ifdef FEATURE_FMRDS_LEGACY_SUPPORT
      if( FMRDS_CMD_Q_PENDING != nQStat && FMRDS_CMD_Q_SUCCESS != nQStat){
#else
      if( FMRDS_CMD_Q_PENDING != nQStat){
#endif
         DBGPRINTF_HIGH("Failed to EnableFm():%d",nQStat);
         nResult = EFAILED;
         break;
      }

      // set the FM-RDS device in FMRDS_NORMAL_MODE
      nQStat = FmrdsApi_SetLowPowerMode(FMRDS_NORMAL_MODE);

#ifdef FEATURE_FMRDS_LEGACY_SUPPORT
      if( FMRDS_CMD_Q_PENDING != nQStat && FMRDS_CMD_Q_SUCCESS != nQStat){
#else
      if( FMRDS_CMD_Q_PENDING != nQStat){
#endif
         DBGPRINTF_HIGH("SetLowPowerMode is failed:%d",nQStat);
         nResult = EFAILED;
         break;
      }

      nQStat = FmrdsApi_SetMuteMode(NOMUTE);
#ifdef FEATURE_FMRDS_LEGACY_SUPPORT
      if( FMRDS_CMD_Q_PENDING != nQStat && FMRDS_CMD_Q_SUCCESS != nQStat){
#else
      if( FMRDS_CMD_Q_PENDING != nQStat){
#endif
         DBGPRINTF_HIGH("Set NoMute is failed:%d",nQStat);
         nResult = EFAILED;
         break;
      }

      
      sOEMFMRadioPtr = &sOEMFMRadio;

      sOEMFMRadioPtr->pvt = &gIFMRadio;

      sOEMFMRadioPtr->m_nRefs = 1;

      sOEMFMRadioPtr->m_pIShell = ps;
      (void)ISHELL_AddRef(ps);

      sOEMFMRadioPtr->nClientId = nClientId;

      sOEMFMRadioPtr->m_pRadioEventNotifyCB = NULL;

       /* this is the default band set by the driver, won't change untill
        * user calls change band. US_EUROPE_BAND value is 1 from fmrds drivers
        */
      gfmrdsRadioSettingsCurrent.radioBand = US_EUROPE_BAND;
//      sOEMFMRadioPtr->m_nCurrentBand = (uint32)US_EUROPE_BAND + 1;


      MEMSET(sOEMFMRadioPtr->m_nRadioEventNotifyStatus,
             0,
             sizeof(sOEMFMRadioPtr->m_nRadioEventNotifyStatus));
      
      //over ride the ptr, as we have it in global ptr.
      *ppObj = (void *)sOEMFMRadioPtr;

   }while(0);

   if ( SUCCESS != nResult ){

      /* debugging msgs */
     DBGPRINTF_FATAL("Unable to start the FMRadio Service %s",__FILE__,0);

     OEMFMRadio_FreeData(sOEMFMRadioPtr);
   }

   return nResult;
}
   
/*=============================================================================
OEMFMRadio_AddRef

Description:
   Increment the reference count for this instance of IFMRadio.

Parameters:
   pIFMRadio [in]: Ptr to instnace of IFMRadio.

Return Value:
    Updated Reference Count.

Comments:

Side Effects:

See Also:
=============================================================================*/
static uint32 OEMFMRadio_AddRef(IFMRadio *pIFMRadio)
{
   OEMFMRadio *pMe = (OEMFMRadio *) (void *)pIFMRadio;

   if( NULL != pMe) {
      return (++pMe->m_nRefs);
   }

   return 0;
}

/*=============================================================================
OEMFMRadio_Release

Description:
   Decrement the reference count for this instance of IFMRadio.

Parameters:
   pIFMRadio [in]: Ptr to instnace of IFMRadio.

Return Value:
    Decremented reference count, once the refrence count reaches zero the
    object is deleted from memory

Comments:

Side Effects:

See Also:
=============================================================================*/
static uint32 OEMFMRadio_Release(IFMRadio *pIFMRadio)
{
   OEMFMRadio *pMe = (OEMFMRadio *) (void *)pIFMRadio;

   if( NULL == pMe) {
      return 0;
   }

   if( 0 != --pMe->m_nRefs) {
      return pMe->m_nRefs;
   }

   //free the complete data
   OEMFMRadio_FreeData(pMe);

   return 0;
}

/*=============================================================================

OEMFMRadio_FreeData

DESCRIPTION: 
   Free an instance of IFMRadio.

PARAMETERS:
   pOEMFMRadio [in]: Ptr to instnace of OEMFRadio.

RETURN VALUE:
   None

COMMENTS:
   None

SIDE EFFECTS:
   None

SEE ALSO:
   None
=============================================================================*/
static void OEMFMRadio_FreeData(OEMFMRadio *pOEMFMRadio)
{
   if(NULL != pOEMFMRadio){

      FmrdsApi_DeRegisterClient((uint8)sOEMFMRadioPtr->nClientId);

      (void)ISHELL_Release(pOEMFMRadio->m_pIShell);

      MEMSET(&sOEMFMRadio,0,sizeof(sOEMFMRadio));

      sOEMFMRadioPtr = NULL;
   }

   return;
}

/*=============================================================================
OEMFMRadio_QueryInterface

Description:
   Query for an interface of IFMRadio

Parameters:
   po    : Ptr to instnace of IFMRadio.
   iid   : Interface ID.
   ppo   : pointer to pointer to queried object.

Return Value:

   SUCCESS          : If we are able to get the reference
   ECLASSNOTSUPPORT : If we cant find that class

Comments:

Side Effects:

See Also:
=============================================================================*/
static int OEMFMRadio_QueryInterface(IFMRadio  *pIFMRadio,
                                     AEECLSID  iid,
                                     void      **ppo)
{
   int nResult  = ECLASSNOTSUPPORT;

  if( NULL == pIFMRadio || NULL == ppo) {
     return EFAILED;
  }

  *ppo = NULL;

  if( AEEIID_IFMRadio == iid ) {

     *ppo = (void *)pIFMRadio;

     (void)IFMRadio_AddRef(pIFMRadio);

     nResult  = SUCCESS;
  }

  return nResult;
}

/*=============================================================================

OEMFMRadio_OnEventNotify

Description:
   It will notify the users for regestered events.

Prototype:
   void OEMFMRadio_OnEventNotify(IFMRadio *pIFMRadio, AEECallback *pcb);

Parameters:
   pIFMRadio     [in]: The IFMRadio interface object
   pcb           [in]: Pointer to the CB that would be fired once we have any
                       events

Return Value:
   None

Comments:
   Users will have to re-register their callback after every event notification
   
See Also:
   None.
   
=============================================================================*/
static void OEMFMRadio_OnEventNotify(IFMRadio *pIFMRadio, AEECallback *pcb)
{

   OEMFMRadio *pMe = (OEMFMRadio *)(void *)pIFMRadio;

   if(NULL == pMe || NULL == pcb){
      return;
   }

   CALLBACK_Cancel(pcb);
   pcb->pNext = pMe->m_pRadioEventNotifyCB;
   pMe->m_pRadioEventNotifyCB = pcb;
   pcb->pfnCancel = OEMFMRadio_OnEventNotify_CB_Cancel;
   pcb->pCancelData = NULL;

   //note the context so that we can notify the apps in the right context
   pcb->pmc = AEE_GetAppContext();
}

/*=============================================================================

OEMFMRadio_OnEventNotify_CB_Cancel

Description:
   Cancels the pervious enqued call back.

Prototype:
   void OEMFMRadio_OnEventNotify_CB_Cancel(AEECallback *pcb);

Parameters:
   pcb : Pointer to the call back that has to be cancelled

Return Value:
   None

Comments:
   None.
   
See Also:
   None.
   
=============================================================================*/
static void OEMFMRadio_OnEventNotify_CB_Cancel(AEECallback *pcb)
{

   OEMFMRadio *pMe = sOEMFMRadioPtr;
   AEECallback **lfp, *pcbTry;

   if( NULL == pcb ){
      return;
   }

   pcb->pfnCancel = NULL;

   if(NULL != pMe->m_pRadioEventNotifyCB){

      lfp = &pMe->m_pRadioEventNotifyCB;

      for ( ; pcb != (pcbTry = *lfp); lfp = &pcbTry->pNext);
      
      *lfp = pcb->pNext;
   }
   

   return;
}

/*=============================================================================

OEMFMRadio_GetEvents

Description:
   Returns the list of events.

Prototype:
   int OEMFMRadio_GetEvents(IFMRadio          *pIFMRadio,
                            AEERadioEvents    *pnRadioEvents,
                            int               nSize,
                            int               *pnReqSize);

Parameters:
   pIFMRadio         [in]: Pointer to the IFMRadio interface object
   pnRadioEvents [in/out]: Array to hold the list of events
   nSize             [in]: Size of the the pnRadioEvents array
   pnReqSize        [out]: On returns contains the total size needed for all
                           the events in number of bytes.

Return Value:
   AEE_SUCCESS    : If succeeds in returning the event(s) information
   AEE_EBADPARM   : If pnReqSize is invalid
   AEE_EFAILED    : Otherwise

Comments:
   None.

See Also:
   None
=============================================================================*/
static int OEMFMRadio_GetEvents(IFMRadio          *pIFMRadio,
                                AEERadioEvents    *pnRadioEvents,
                                int               nSize,
                                int               *pnReqSize)
{
   OEMFMRadio *pMe = (OEMFMRadio *)(void *)pIFMRadio;
   int nResult = SUCCESS;
   int nEvents = 0, nIndex = 0;


   if(NULL == pMe || NULL == pnReqSize){
      return EBADPARM;
   }

   // Check the number of event received
   for(nIndex = 0; nIndex < EVT_FMRADIO_MAX; nIndex++){

      if(0 != pMe->m_nRadioEventNotifyStatus[nIndex]){
         nEvents++;
      }
   }

   *pnReqSize = nEvents * sizeof(AEERadioEvents);

   if(NULL != pnRadioEvents){
      
      nEvents = 1;
      for(nIndex = 0; nIndex < EVT_FMRADIO_MAX; nIndex++ ){
         
         if(0 != pMe->m_nRadioEventNotifyStatus[nIndex] &&
               ((nEvents * sizeof(AEERadioEvents)) <= nSize)){

            //Reset to zero & fill the pnRadioEvents with Event ID.
            pMe->m_nRadioEventNotifyStatus[nIndex] = 0;
            *(pnRadioEvents + nEvents - 1) = nIndex + 1;
            nEvents++;
         }
      }

   }

   return nResult;
}

/*=============================================================================
OEMFMRadio_SetStation

Description:
   Changes the current station frequency to the new station and the FM Radio
   will be tuned to the new station.

Parameters:
   pIFMRadio        [in]: The IFMRadio object.
   nFrequency       [in]: Frequency of the new radio station 
                          Valid Range ( 76000 >= Frequency <= 90000 for Japan)
                          ( 87500 >= Frequency <= 108000 for rest of World)
   pnResult         [out]: Would contain the status of call back function, if 
                          the station is set successfully then would say
                          SUCCESS else EFAILED.                          
   pcb              [in]: AEECallback that will be notified when the station
                          is set.

Return Value:
   None

Comments:
   If this function is unable to change to new station then it will play the
   previous playing station.The user should check for the status of pResult
   only after the callback is called.

Side Effects:
   None.

See Also:
   None.
=============================================================================*/
static void OEMFMRadio_SetStation(IFMRadio     *pIFMRadio,
                                  int          nFrequency,
                                  int          *pnResult,
                                  AEECallback  *pcb)
{
   OEMFMRadio *pMe = (OEMFMRadio *)(void *)pIFMRadio;
   int nResult = SUCCESS;
   fmrds_cmd_q_status_type nQStat;


   if( NULL == pMe || NULL == pcb || NULL == pnResult ) {
      return ;
   }
   
   do {
      
      /* Cancel the CB if it was queued */
      CALLBACK_Cancel(pcb);

      /* store the call back */
      pMe->m_pCBs[IFMRADIO_CB_SETSTN_IDX] = pcb;

      /* store the result, which contains the status of the call */
      pMe->m_pCB_Status[IFMRADIO_CB_SETSTN_IDX] = (uint32 *)(void *)pnResult;

      /* the cancel function to be called.for the callback. */
      pcb->pfnCancel = OEMFMRadio_CallBackCancel;

      pcb->pCancelData = (void *)IFMRADIO_CB_SETSTN_IDX;

      pcb->pmc = AEE_GetAppContext();

      /* check the freq range acc. to the current band, the current band values
       * are as per driver info
       */
      if( US_EUROPE_BAND == (gfmrdsRadioSettingsCurrent.radioBand)){
         if( IFMRADIO_US_EUROPE_BAND_MIN > nFrequency || 
               IFMRADIO_US_EUROPE_BAND_MAX < nFrequency){
            
            AEEFMRadio_MSG_LOW("Invalid Frequency specified",0,0,0);
            break;
         }
      }

      else if( JAPAN_STANDARD_BAND == (gfmrdsRadioSettingsCurrent.radioBand)){
         if( IFMRADIO_JAPAN_BAND_MIN > nFrequency   ||
               IFMRADIO_JAPAN_BAND_MAX < nFrequency){

            AEEFMRadio_MSG_LOW("Invalid Frequency specified",0,0,0);
            break;
         }
      }

      nFrequency = nFrequency / 10;

      nQStat = FmrdsApi_SetStation(nFrequency);

      /* we would call the AEECallBack once the station is set and we Rx a
       * corresponding event notification from driver.
       */
#ifdef FEATURE_FMRDS_LEGACY_SUPPORT
      if( FMRDS_CMD_Q_PENDING != nQStat && FMRDS_CMD_Q_SUCCESS != nQStat){
#else
      if( FMRDS_CMD_Q_PENDING != nQStat){
#endif
         nResult = EFAILED;
      }


   }while(0);

   if( SUCCESS != nResult){

      *pnResult = EFAILED;

      pcb->pfnCancel = NULL;
      pcb->pCancelData = NULL;

      AEE_ResumeCallback(pcb,(ACONTEXT *)pcb->pmc);
   }
      
   return;

}

/*=============================================================================
OEMFMRadio_GetTunedStationFrequency

Description:
   Gets the tuned station frequency.

Parameters:
   pIFMRadio            [in]: The IFMRadio object.
   pnStationFreq       [out]: Specifies tuned frequency

Return Value:
   Returns the AEE return value.
   
   SUCCESS:  If the tuned frequency was retrieved successfully
   EFAILED:  Otherwise

Comments:
   None

See Also:
   None

=============================================================================*/
static int OEMFMRadio_GetTunedStationFrequency(IFMRadio  *pIFMRadio,
                                               int       *pnStationFreq)
{

   OEMFMRadio *pMe = (OEMFMRadio *)(void *)pIFMRadio;

   if(NULL == pMe || NULL == pnStationFreq){
      return EBADPARM;
   }

   *pnStationFreq = pMe->m_nTunedStationFreq;

   return SUCCESS;
}

/*=============================================================================
OEMFMRadio_SetBand

Description:
   Set's the band of the FM-Radio to which it has tune to

Parameters:
   pIFMRadio  [in] :  The IFMRadio object
   nBand     [out] :  The band which we have to set to
   pnResult  [out] :  Would contain the status of call back function, if 
                      the station is set successfully then would say
                      SUCCESS else EFAILED.                          
   pcb       [in]  :  AEECallback that will be notified when the station
                      is set.

Return Value:
   None

Comments:
   If the band isn't set the default one would be AEEFMRADIO_BAND_OTHERS
   (87.5 to 108.0 MHz)

Side Effects:

See Also:
=============================================================================*/
static void OEMFMRadio_SetBand(IFMRadio        *pIFMRadio,
                               AEERadioBand    nBand,
                               int             *pnResult,
                               AEECallback     *pcb)
{
   OEMFMRadio *pMe = (OEMFMRadio *)(void *)pIFMRadio;
   int nResult = SUCCESS;
   fmrds_cmd_q_status_type nQStat;


   if( NULL == pMe || NULL == pcb || NULL == pnResult || 
                                              AEEFMRADIO_BAND_JAPAN < nBand) {
      return ;
   }
   
   do {

      /* Cancel the CB if it was queued elsewhere */
      CALLBACK_Cancel(pcb);

      /* the band info which we have is different from driver header files
       * we need to update them oneday to keep in sync with drivers,
       * AEEFMRADIO_BAND_OTHERS = US_EUROPE_BAND
       * AEEFMRADIO_BAND_JAPAN  = JAPAN_STANDARD_BAND
       * we dont have a mapping for JAPAN_WIDE_BAND which is from 76-108
       */
      if( AEEFMRADIO_BAND_OTHERS == nBand ) {
         //map it to US_EUROPE_BAND
         nBand--;
      }
      /* else dont do anything as we dont have the mapping correctly */

      /* store the call back */
      pMe->m_pCBs[IFMRADIO_CB_SET_BAND] = pcb;

      /* store the result, which contains the status of the call */
      pMe->m_pCB_Status[IFMRADIO_CB_SET_BAND] = (uint32 *)(void *)pnResult;

      /* the cancel function to be called.for the callback. */
      pcb->pfnCancel = OEMFMRadio_CallBackCancel;

      pcb->pCancelData = (void *)IFMRADIO_CB_SET_BAND;

      pcb->pmc = AEE_GetAppContext();

      gfmrdsRadioSettingsTemp.radioBand = nBand;

      nQStat = FmrdsApi_ConfigureRadio(gfmrdsRadioSettingsTemp);
      
      /* we would call the AEECallBack once the station is set and we Rx a
       * corresponding event notification from driver.
       */
#ifdef FEATURE_FMRDS_LEGACY_SUPPORT
      if( FMRDS_CMD_Q_PENDING != nQStat && FMRDS_CMD_Q_SUCCESS != nQStat){
#else
      if( FMRDS_CMD_Q_PENDING != nQStat){
#endif
         nResult = EFAILED;
      }
      
   }while(0);

   if( SUCCESS != nResult){

      *pnResult = EFAILED;

      pcb->pfnCancel = NULL;
      pcb->pCancelData = NULL;

      AEE_ResumeCallback(pcb,(ACONTEXT *)pcb->pmc);
   }
      
   return;

}

/*==============================================================================
OEMFMRadio_EnablePlay

Description:
   Enables or disables the playing of radio based on the second parameter

Parameters:
   pIFMRadio    [in]: The IFMRadio object.
   bVal         [in]: Boolean which will decide to enable or disable the
                      playing of radio

Return Value:
   Returns the AEE return value.
   
   SUCCESS:  If function succeeds.
   EFAILED:  If not successful

Comments:
   None

See Also:
   None

==============================================================================*/
static int OEMFMRadio_EnablePlay(IFMRadio *pIFMRadio,boolean  bVal)
{
   OEMFMRadio *pMe = (OEMFMRadio *)(void *)pIFMRadio;
   int nResult = SUCCESS;
   fmrds_cmd_q_status_type nQStat;

   if( NULL == pMe){
      return EBADPARM;
   }

   if( FALSE == bVal){
      snd_set_aux_line_in ((snd_cb_func_ptr_type)OEMFMRadio_Snd_CB_Notify,
                           (void *)pMe,
                           SND_AUX_LINE_IN_MODE_OFF);
      nQStat = FmrdsApi_SetMuteMode(MUTEBOTH);
   }
   else {
      snd_set_aux_line_in ((snd_cb_func_ptr_type)OEMFMRadio_Snd_CB_Notify,
                           (void *)pMe,
                           SND_AUX_LINE_IN_MODE_ON);
      nQStat = FmrdsApi_SetMuteMode(NOMUTE);
   }

#ifdef FEATURE_FMRDS_LEGACY_SUPPORT
      if( FMRDS_CMD_Q_PENDING != nQStat && FMRDS_CMD_Q_SUCCESS != nQStat){
#else
      if( FMRDS_CMD_Q_PENDING != nQStat){
#endif
      nResult = EFAILED;
   }

   return nResult;  
}


/*==============================================================================
OEMFMRadio_GetValidFreqRange

Description:
   Gets the valid frequency for the FM-Radio in that particular geographical
   location

Parameters:
   pIFMRadio     [in]: The IFMRadio object.
   pFreqMin     [out]: pointer to frequency, where the minimum of the
                       frequency range will be returned.
   pFreqMax     [out]: pointer to frequency, where the maximum of the
                       frequency range will be returned.

Return Value:
   Returns the AEE return value.
   SUCCESS:  If succeeds in retrieving the valid range.
   EFAILED:  If not successful

Comments:
   This value would be same through out the world except for Japan the range
   is 76.0 MHz to 90.0 MHz while the rest of world is 87.5 MHz to 108.0 MHz

   The value returned would be in kHz, so for 
    Japan  - 76000 (Min) 90000 (Max)
    Others - 87500 (Min) 108000 (Max)

See Also:
   None

==============================================================================*/
static int OEMFMRadio_GetValidFreqRange(IFMRadio  *pIFMRadio,
                                        int       *pFreqMin,
                                        int       *pFreqMax)
{
   OEMFMRadio *pMe = (OEMFMRadio *)(void *)pIFMRadio;

   if( NULL == pMe || NULL == pFreqMin || NULL == pFreqMax ){
      return EBADPARM;
   }

   if( AEEFMRADIO_BAND_OTHERS == (gfmrdsRadioSettingsCurrent.radioBand + 1)) {
      *pFreqMin = IFMRADIO_OTHERS_BAND_MIN;
      *pFreqMax = IFMRADIO_OTHERS_BAND_MAX;
   }

   else if( AEEFMRADIO_BAND_JAPAN == (gfmrdsRadioSettingsCurrent.radioBand + 1)){
      *pFreqMin = IFMRADIO_JAPAN_BAND_MIN;
      *pFreqMax = IFMRADIO_JAPAN_BAND_MAX;
   }

   return SUCCESS;
}


/*=============================================================================

OEMFMRadio_GetStrongestChannels

Description:
   Get the given number of strongest channels. Caller should allocate the
   memory for the number of channels required

Prototype:
   void OEMFMRadio_GetStrongestChannels(
         IFMRadio                   *pIFMRadio,
         int                        *pnChannels,
         int                        nChannels,
         int                        *pnReqChannels,
         int                        *pnResult,
         AEECallback                *pcb,
         AEERadioSearchDirection    nSearchVal)


Parameters:
   pIFMRadio            [in]: The IFMRadio object.
   pnChannels       [in/out]: pointer to channels. Memory should be allocated by
                              caller.
   nChannels            [in]: Indicates the size of the pChannels
   pnReqChannels    [in/out]: Indicates the number of channels requested upon return
                              would hold the actual channels filled
   pnResult            [out]: Indicates whether the call was successful or not.If
                              the call succeeds contains AEE_SUCCESS else AEE_EFAILED
   pcb                  [in]: Callback that would be called when the strongest
                              channels are compiled
   nSearchVal           [in]: Specifies the type of search to be made.

Return Value:
   None

Comments:
   The user should check for the status of pnResult only after the user is
   notified  through callback. pNumChannels would contain the actual number
   of channels retrieved successfully.

   If nSearchVal is AEERADIO_STN_SEARCH_UP/DOWN the driver searches for the
   next available strongest channel in the given direction. If 
   AEERADIO_STN_SEARCH_ALL is mentioned it scans the entire band for 
   strongest channels.

See Also:
   None

==============================================================================*/
static void OEMFMRadio_GetStrongestChannels(
      IFMRadio                   *pIFMRadio,
      int                        *pnChannels,
      int                        nChannels,
      int                        *pnReqChannels,
      int                        *pnResult,
      AEECallback                *pcb,
      AEERadioSearchDirection    nSearchVal)
{

   OEMFMRadio *pMe = (OEMFMRadio *)(void *)pIFMRadio;
   int nResult = SUCCESS;
   int nQStat = 0;
   tsFmrdsRadioSrchModeType  fmrdsRadioSrchMode = {0.};

   if ( NULL == pMe || NULL == pnReqChannels || NULL == pnChannels || 
         0 == nChannels || NULL == pnResult || NULL == pcb ){
      return;
   }

   if ( AEERADIO_STN_SEARCH_ALL < nSearchVal ){
      return;
   }

   do {

      /* Cancel the CB if it was queued elsewhere */
      CALLBACK_Cancel(pcb);

      /* store the call back ptr */
      pMe->m_pCBs[IFMRADIO_CB_GET_CHNLS] = pcb;

      /* store the result, which contains the status of the call */
      pMe->m_pCB_Status[IFMRADIO_CB_GET_CHNLS] = (uint32 *)pnResult;

      /* store the location which tells how many channels are scanned [out] */
      pMe->m_pScanNumChannels = pnReqChannels;

      /* store the array pointer, where the copied channels have to go. */
      pMe->m_pScanChnlsDest = pnChannels;

      /* store the length of scan channels dest list */
      pMe->m_nScanChnlsDestLen = nChannels;

      /* call back to be called once if the user want to cancel */
      pcb->pfnCancel = OEMFMRadio_CallBackCancel;
      pcb->pCancelData = (void *)IFMRADIO_CB_GET_CHNLS;

      pcb->pmc = AEE_GetAppContext();
      
      fmrdsRadioSrchMode.srchMode = SEARCHMODE_SEEK ; 
      fmrdsRadioSrchMode.scanTime = DWELL_PERIOD_3S;
      fmrdsRadioSrchMode.srchDir = (efm_search_direction)nSearchVal;
      fmrdsRadioSrchMode.srchRssiLev =SEARCH_RSSI_LEVEL_STRONG; 

      nQStat = FmrdsApi_SearchStations(fmrdsRadioSrchMode);
#ifdef FEATURE_FMRDS_LEGACY_SUPPORT
      if( FMRDS_CMD_Q_PENDING != nQStat && FMRDS_CMD_Q_SUCCESS != nQStat){
#else
      if( FMRDS_CMD_Q_PENDING != nQStat){
#endif
         nResult = EFAILED;
      }

   }while(0);

   if( SUCCESS != nResult){

      *pnResult = EFAILED;

      pcb->pfnCancel = NULL;
      pcb->pCancelData = NULL;

      AEE_ResumeCallback(pcb,(ACONTEXT *)pcb->pmc);
   }
      
   return;
}


/*=============================================================================
OEMFMRadio_GetSignalStrength

Description:
   Gets the signal strength of the currently tuned station

Parameters:
   pIFMRadio         [in]: The IFMRadio object.
   pSignalStrength  [out]: Signal strength of the station

Return Value:
   Returns the AEE return value.
   SUCCESS:  If the signal strength was retrieved successfully
   EFAILED:  Otherwise.

Comments: 
   None

See Also:
   None

=============================================================================*/
static int OEMFMRadio_GetSignalStrength(IFMRadio   *pIFMRadio,
                                        int        *pSignalStrength)

{
   OEMFMRadio *pMe = (OEMFMRadio *)(void *)pIFMRadio;

   if ( NULL == pMe || NULL == pSignalStrength ){
      return EBADPARM;
   }

   *pSignalStrength = pMe->m_nCurrentSignalStrength;

   return SUCCESS;
}

/*=============================================================================
OEMFMRadio_GetMaxSignalStrength

Description:
   Gets the Maximum signal strength available for any station

Parameters:
   pIFMRadio            [in]: The IFMRadio object.
   pMaxSignalStrength  [out]: Maximum Signal strength for any station

Return Value:
   Returns the AEE return value.
   
   SUCCESS:  If the signal strength was retrieved successfully
   EFAILED:  Otherwise

Comments:
   None

See Also:
   None

=============================================================================*/
static int OEMFMRadio_GetMaxSignalStrength(IFMRadio  *pIFMRadio,
                                           int       *pMaxSignalStrength)
{
   OEMFMRadio *pMe = (OEMFMRadio *)(void *)pIFMRadio;

   if( NULL == pMe || NULL == pMaxSignalStrength){
      return EBADPARM;
   }

   *pMaxSignalStrength = IFMRADIO_MAX_SIGNAL_STRENGTH;

   return SUCCESS;
}


/*=============================================================================
OEMFMRadio_IsFMServiceAvailable

Description:
   Find the FM Radio Service is Available.

Parameters:
   pIFMRadio  [in]: The IFMRadio object.
   bVal      [out]: Pointer to Boolean in which TRUE will returned if service
                    is available otherwise FALSE will be returned

Return Value:
   Returns the AEE return value.

   SUCCESS:  If succeeds in retrieving the FM-Service availability.
   EFAILED:      If not successful

Comments:
   If the communications with the drivers loose then FM service availability
   will be FALSE

See Also:
   None


=============================================================================*/
static int OEMFMRadio_IsFMServiceAvailable(IFMRadio *pIFMRadio,boolean  *pBool)
{
  OEMFMRadio *pMe = (OEMFMRadio *)(void *)pIFMRadio;

  if( NULL == pMe || NULL == pBool) {
    return EFAILED;
  }

  if ( TRUE == pMe->m_bIsDriverDisConnected ){
     *pBool = FALSE;
  }
  else {
     *pBool = TRUE;
  }


  return SUCCESS;

}

/*=============================================================================
OEMFMRadio_SetFidelityMode

Description:
   Set the fidelity Mode of the FM Radio

Parameters:
   pIFMRadio [in]: The IFMRadio object.
   nVal      [in]: The Fidelity Mode

Return Value:
   Returns the AEE return value.
   SUCCESS:  If the function was able to set the fidelity mode successfully
   EFAILED:      Otherwise.

Comments:
   None

See Also:
   None

=============================================================================*/
static int OEMFMRadio_SetFidelityMode(IFMRadio                *pIFMRadio,
                                      AEEFMRadioFidelityMode  nVal)
{
   OEMFMRadio *pMe = (OEMFMRadio *)(void *)pIFMRadio;
   int nResult = SUCCESS;
   fmrds_cmd_q_status_type nQStat;

   if( NULL == pMe){
      return nResult;
   }

   if( AEEFIDELITY_MODE_MONO < nVal){
      return nResult;
   }

   nQStat = FmrdsApi_SetStereoMode((teFmrdsStereoMonoModeType)nVal);
#ifdef FEATURE_FMRDS_LEGACY_SUPPORT
      if( FMRDS_CMD_Q_PENDING != nQStat && FMRDS_CMD_Q_SUCCESS != nQStat){
#else
      if( FMRDS_CMD_Q_PENDING != nQStat){
#endif
      nResult = EFAILED;
   }

   return nResult;
}
/*=============================================================================
OEMFMRadio_GetFidelityMode

Description:
   Get the fidelity Mode of the FM Radio

Parameters:
   pIFMRadio    [in]: The IFMRadio object.
   pVal     [in/out]: On success contains the current fidelity mode. 

Return Value:
   Returns the AEE return value.
   SUCCESS   :  If the function was able to get the fidelity mode successfully
   EFAILED   :  Otherwise.

Comments:
   None

See Also:
   None

=============================================================================*/
static int OEMFMRadio_GetFidelityMode(IFMRadio                 *pIFMRadio,
                                      AEEFMRadioFidelityMode   *pnVal)
{
   OEMFMRadio *pMe = (OEMFMRadio *)(void *)pIFMRadio;

   if( NULL == pMe || NULL == pnVal){
      return EFAILED;
   }

   *pnVal = pMe->m_nCurrentFidelityMode;

   return SUCCESS;
}

/*=============================================================================
OEMFMRadio_CallBackCancel

Description:
   Cancels the callback enqued

Parameters:
   pcb   [in] : pointer to the AEECallback to cancel 

Return Value:
   None

Comments:
   None

See Also:
   None

=============================================================================*/
static void OEMFMRadio_CallBackCancel(AEECallback *pcb)
{
   uint32 nCBVal =0;
   OEMFMRadio *pMe = sOEMFMRadioPtr;

   if( NULL == pcb ){
      return;
   }

   nCBVal = (uint32 )*(&(pcb->pCancelData));

   switch (nCBVal){

      case IFMRADIO_CB_SETSTN_IDX : 
         
         pMe->m_pCBs[IFMRADIO_CB_SETSTN_IDX] = NULL;
         pMe->m_pCB_Status[IFMRADIO_CB_SETSTN_IDX] = NULL;

         break;

      case IFMRADIO_CB_GET_CHNLS:

         pMe->m_pCBs[IFMRADIO_CB_GET_CHNLS]=NULL;
         pMe->m_pCB_Status[IFMRADIO_CB_GET_CHNLS] = NULL;
         pMe->m_pScanNumChannels = NULL;
         pMe->m_nScanChnlsDestLen = 0;
         pMe->m_pScanChnlsDest = NULL;

         /* cancel the previously enqued scan */
         //(void)fmrds_api_cancel_scan(); //>> prasad <<
         
         break;

      case IFMRADIO_CB_SET_BAND:

         pMe->m_pCBs[IFMRADIO_CB_SET_BAND]=NULL;
         pMe->m_pCB_Status[IFMRADIO_CB_SET_BAND] = NULL;

         break;

      default: /*which aren't handled yet */
         
         break;
         
   }
}


/*=============================================================================
OEMFMRadio_Register_Events

Description:
   Lets third party applications register to listen for specific events
   from the FM-RDS driver
   
Parameters:
   pcb                :  Pointer to the FMRadio_Callback
   pefmrds_event_type : List of events in which the apps is interested
   nNumEvents         : Number of events in the above list

Return Value:
   SUCCESS     : If we are able to enque the command correctly
   EFAILED     : Otherwise

Comments:
   None.

See Also:
   None.

=============================================================================*/
int OEMFMRadio_Register_Events(FMRadio_Callback   *pcb,
                               efmrds_event_type  *pefmrds_event_type,
                               uint32             nNumEvents)
{
   int nResult = SUCCESS;
   int Ctr = 0;
   OEMFMRadio *pMe = sOEMFMRadioPtr;
   fmrds_cmd_q_status_type nQStat;

   if ( NULL == pcb || NULL == pcb->pfnNotify || NULL == pefmrds_event_type ){
      return EBADPARM;
   }

   /* check if FMRadio has been started or not, we have a wierd dependency that
    * FMRadio should be started first before starting IFMRDS
    */
   if ( NULL == pMe ){
      DBGPRINTF_FATAL("Please Create an Instance of AEECLSID_FMRadio before call to IFMRDS");
      return ENOTALLOWED; 
   }

   /* user can't listen for more than that's available */
   if ( (uint32)FMRDS_EV_MAX_EVENT  <  nNumEvents ) {
      return EBADPARM;
   }

   for (Ctr = 0; Ctr < nNumEvents; Ctr++){

      if ( *(pefmrds_event_type+Ctr) < FMRDS_EV_MAX_EVENT){

         pcb->pNext = pMe->m_pOEMFMEventsCB[*(pefmrds_event_type+Ctr)];
         pMe->m_pOEMFMEventsCB[*(pefmrds_event_type+Ctr)] = pcb;
         
         if(FMRDS_EV_RAW_RDS_RBDS_BLOCK_DATA ==  *(pefmrds_event_type+Ctr)){

            // RDS processing options, By default, no RDS groups will 
            // be passed to the client by fmrds drivers.
            tsFmrdsRxProcOptType fmrdsRxProcOpt = {0,};
            fmrdsRxProcOpt.enRdsGrps = 0xFFFF;
            fmrdsRxProcOpt.rdsBufSz = 100;
            fmrdsRxProcOpt.enRdsChangeFiltr = 0;
            fmrdsRxProcOpt.enPSUncrtbleBlks = 0;
            fmrdsRxProcOpt.enRdsBlkEProc = 0;
            fmrdsRxProcOpt.enRdsAutoAF = 0;
            fmrdsRxProcOpt.rdsReturnRawBlocks = TRUE;


            //set the mode temporarily to RAW RDS, need to see how we can do 
            //based on zone location info
            nQStat = FmrdsApi_SetRdsRxProcessingOptions(fmrdsRxProcOpt);
//            nQStat = fmrds_api_set_rds_mode(RDS_MODE_RAW); 
#ifdef FEATURE_FMRDS_LEGACY_SUPPORT
            if( FMRDS_CMD_Q_PENDING != nQStat && FMRDS_CMD_Q_SUCCESS != nQStat){
#else
      if( FMRDS_CMD_Q_PENDING != nQStat){
#endif
               AEEFMRadio_MSG_ERROR("Failed to do FmrdsApi_SetRdsRxProcessingOptions(),returned:%d",
                     (int)nQStat,0,0);
               nResult = EFAILED;
               break;
            }
         }

      }
   }

   pcb->pfnCancel = (FMRadioCBCancel *)OEMFMRadio_Register_Events_CB_Cancel;
   pcb->pCancelData = NULL ;

   return nResult;
}


/*=============================================================================
OEMFMRadio_Register_Events_CB_Cancel

Description:
   Lets the enqued call back be cancelled 
   
Parameters:
   pcb :  Pointer to the FMRadio_Callback

Return Value:
   None

Comments:
   This cancels the CB for all the events if it finds a matching CB

See Also:
   None

=============================================================================*/
void OEMFMRadio_Register_Events_CB_Cancel(FMRadio_Callback *pcb)
{
   int Ctr = 0;
   OEMFMRadio *pMe = sOEMFMRadioPtr;
   FMRadio_Callback **lfp, *pcbTry;
   
   if ( NULL == pMe || NULL == pcb ){
      return;
   }

   for ( Ctr = 0; Ctr < (int)FMRDS_EV_MAX_EVENT; Ctr++ ){
      
      if ( NULL != pMe->m_pOEMFMEventsCB[Ctr] ){

         lfp = &pMe->m_pOEMFMEventsCB[Ctr];

         for ( ;pcb != (pcbTry = *lfp) ; lfp = &pcbTry->pNext) ;

         *lfp = pcb->pNext;
      }
   }
}

/*=============================================================================
Function:
   OEMFMRadio_NotifyOEMEventListeners

Description:
   Notifies the clients for registered events

Parameters:
   fmrds_event : event info structure

Return Value:
   None

Comments:
   None

Side Effects:
   None

=============================================================================*/
static void OEMFMRadio_NotifyOEMEventListeners(fmrds_event_msg_type *pFMRDS_Event)
{
   OEMFMRadio *pMe = sOEMFMRadioPtr;
   FMRadio_Callback *pcb = NULL;

   if ( NULL == pMe ){
      return;
   }

   if ( FMRDS_EV_MAX_EVENT <= pFMRDS_Event->fmrdsEventName ){
      return;
   }

   pcb = pMe->m_pOEMFMEventsCB[(int)pFMRDS_Event->fmrdsEventName];

   while( NULL != pcb ){
      pcb->pfnNotify(pcb->pNotifyData,pFMRDS_Event);
      pcb = pcb->pNext;
   }

   return;
}

/*=============================================================================
Function:
   OEMFMRadio_NotifyEventListeners

Description:
   Notifies the apps for registered events

Parameters:
   None
   
Return Value:
   None

Comments:
   None

Side Effects:
   None

=============================================================================*/
static void OEMFMRadio_NotifyEventListeners(void)
{
   OEMFMRadio *pMe = sOEMFMRadioPtr;
   AEECallback *pcb = NULL;

   if ( NULL == pMe ){
      return;
   }

   pcb = pMe->m_pRadioEventNotifyCB;

   while( NULL != pcb ){
      
      AEE_ResumeCallback(pcb,(ACONTEXT *)pcb->pmc);
      pcb = pcb->pNext;
   }

   return;
}

/*=============================================================================
Function:
   OEMFMRadio_Snd_CB_Notify

Description:
   Callback notification from sound task about the commands we issue

Parameters:
   pMe    : ptr to OEMFMRadio structure
   status : staus info from snd task

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:

=============================================================================*/
static void OEMFMRadio_Snd_CB_Notify(OEMFMRadio *pMe, snd_status_type status)
{
   (void)pMe;
   if ( SND_SUCCESS == status ){
      AEEFMRadio_MSG_LOW("Command to snd task succeeded",0,0,0);
   }
   else {
      AEEFMRadio_MSG_LOW("Command to snd task failed with status:%d",status,0,0);
   }
}

/*=============================================================================
Function:
   OEMFMRadio_Notify_SignalStrengthDiff

Description:
   Function to check the signal strength difference and notify listners

Parameters:
   pMe : OEMFMRadio ptr
   
Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:

=============================================================================*/
static __inline void OEMFMRadio_Notify_SignalStrengthDiff(OEMFMRadio *pMe)
{

   if( pMe->m_nPrevSignalStrength != pMe->m_nCurrentSignalStrength ) {

      pMe->m_nRadioEventNotifyStatus[EVT_FMRADIO_SIGNAL_STRENGTH_CHANGE - 1] = 1;

      OEMFMRadio_NotifyEventListeners();
   }

}

/*=============================================================================
Function:
   OEMFMRadio_FMRDS_Events_CB

Description:
   Event callback from FMRDS layer

Parameters:
   fmrds_event : event info structure

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:

=============================================================================*/
static void OEMFMRadio_FMRDS_Events_CB(fmrds_event_msg_type fmrds_event)
{
   OEMFMRadio *pMe = sOEMFMRadioPtr;

   if( NULL == pMe){
      return;
   }

   DBGPRINTF_LOW("Recieved Event:%d",fmrds_event.fmrdsEventName,0,0);

   switch(fmrds_event.fmrdsEventName){

      case FMRDS_EV_ENABLE_FM:
         {

            AEEFMRadio_MSG_LOW("Enable FM",0,0,0);

            if(FMRDS_EV_SUCCESS == fmrds_event.fmrdsEvResult){

               gfmrdsRadioSettingsCurrent.radioBand = fmrds_event.fmrdsEventMsgBody.\
                                                      tufmrdsStatusParameters.fmrdsEnableFmEv.radioSettings.radioBand;
               gfmrdsRadioSettingsCurrent.deEmphasis = fmrds_event.fmrdsEventMsgBody.\
                                                       tufmrdsStatusParameters.fmrdsEnableFmEv.radioSettings.deEmphasis;
               gfmrdsRadioSettingsCurrent.chSpacing  = fmrds_event.fmrdsEventMsgBody.\
                                                       tufmrdsStatusParameters.fmrdsEnableFmEv.radioSettings.chSpacing;
               gfmrdsRadioSettingsCurrent.rdsStd =fmrds_event.fmrdsEventMsgBody.\
                                                  tufmrdsStatusParameters.fmrdsEnableFmEv.radioSettings.rdsStd;
               gfmrdsRadioSettingsCurrent.bandRange = fmrds_event.fmrdsEventMsgBody.\
                                                      tufmrdsStatusParameters.fmrdsEnableFmEv.radioSettings.bandRange;
            }

         
         }
         break;

      case FMRDS_EV_DISABLE_FM:
         AEEFMRadio_MSG_LOW("Disable FM",0,0,0);
         break;

      case FMRDS_EV_CFG_RADIO:
         {

            AEECallback *pcb= pMe->m_pCBs[IFMRADIO_CB_SET_BAND];

            AEEFMRadio_MSG_LOW("Configure FM Radio",0,0,0);

            gfmrdsRadioSettingsCurrent.radioBand = fmrds_event.fmrdsEventMsgBody.\
                                                   tufmrdsStatusParameters.fmrdsConfigRadioEv.radioSettings.radioBand;
            gfmrdsRadioSettingsCurrent.deEmphasis = fmrds_event.fmrdsEventMsgBody.\
                                                    tufmrdsStatusParameters.fmrdsConfigRadioEv.radioSettings.deEmphasis;
            gfmrdsRadioSettingsCurrent.chSpacing  = fmrds_event.fmrdsEventMsgBody.\
                                                    tufmrdsStatusParameters.fmrdsConfigRadioEv.radioSettings.chSpacing;
            gfmrdsRadioSettingsCurrent.rdsStd =fmrds_event.fmrdsEventMsgBody.\
                                               tufmrdsStatusParameters.fmrdsConfigRadioEv.radioSettings.rdsStd;
            gfmrdsRadioSettingsCurrent.bandRange = fmrds_event.fmrdsEventMsgBody.\
                                                   tufmrdsStatusParameters.fmrdsConfigRadioEv.radioSettings.bandRange;

            if ( NULL != pcb ){
               //dispatch the CB
               *pMe->m_pCB_Status[IFMRADIO_CB_SET_BAND] = fmrds_event.fmrdsEvResult;

               AEE_ResumeCallback(pcb, (ACONTEXT *)pcb->pmc);
            }
         }

         break;

      case FMRDS_EV_MUTE_MODE_SET:
         AEEFMRadio_MSG_LOW("MUTE mode set",0,0,0);
         break;

      case FMRDS_EV_STEREO_MODE_SET:
            if (FM_STEREO_PROGRAM   == fmrds_event.fmrdsEventMsgBody.tufmrdsStatusParameters.\
                                                                  fmrdsTuneStatusEv.stereoProgram ) {
               pMe->m_nCurrentFidelityMode = AEEFIDELITY_MODE_STEREO;
            }
            else {
               pMe->m_nCurrentFidelityMode = AEEFIDELITY_MODE_MONO;
            }

         break;

      case FMRDS_EV_RADIO_STATION_SET:
         {

            AEECallback *pcb = pMe->m_pCBs[IFMRADIO_CB_SETSTN_IDX];

            //check if the user has already cancelled the call back.
            if(FMRDS_EV_SUCCESS == fmrds_event.fmrdsEvResult){
                pMe->m_nTunedStationFreq = fmrds_event.fmrdsEventMsgBody.\
                                           tufmrdsStatusParameters.fmrdsStationSetEv.stationFreq * 10;

               //we now wish to test for RDS sychronisation
               (void)FmrdsApi_GetRdsSyncStatus();
               if( NULL != pcb ){
                  *pMe->m_pCB_Status[IFMRADIO_CB_SETSTN_IDX] = SUCCESS;
               }
            }
            else {
               if( NULL != pcb ){
                  *pMe->m_pCB_Status[IFMRADIO_CB_SETSTN_IDX] = EFAILED;
               }
            }

            if( NULL != pcb ){
               AEE_ResumeCallback(pcb,(ACONTEXT *)pcb->pmc);
            }
         }
         break;

      case FMRDS_EV_RADIO_TUNE_STATUS:
         if(FMRDS_EV_SUCCESS == fmrds_event.fmrdsEvResult){
            pMe->m_nPrevSignalStrength = pMe->m_nCurrentSignalStrength;
            pMe->m_nCurrentSignalStrength = fmrds_event.fmrdsEventMsgBody.\
                                            tufmrdsStatusParameters.fmrdsTuneStatusEv.rssi;
            pMe->m_nTunedStationFreq = fmrds_event.fmrdsEventMsgBody.\
                                            tufmrdsStatusParameters.fmrdsTuneStatusEv.stationFreq * 10;
            if (FM_STEREO_PROGRAM   == fmrds_event.fmrdsEventMsgBody.tufmrdsStatusParameters.\
                                                                  fmrdsTuneStatusEv.stereoProgram ) {
               pMe->m_nCurrentFidelityMode = AEEFIDELITY_MODE_STEREO;
            }
            else {
               pMe->m_nCurrentFidelityMode = AEEFIDELITY_MODE_MONO;
            }
         }
         OEMFMRadio_Notify_SignalStrengthDiff(pMe);
         break;

      case FMRDS_EV_RDS_LOCK_STATUS:
         AEEFMRadio_MSG_LOW("RDS Lock Status",0,0,0);
         pMe->m_nPrevSignalStrength = pMe->m_nCurrentSignalStrength;
         pMe->m_nCurrentSignalStrength = fmrds_event.fmrdsEventMsgBody.\
                                         tufmrdsStatusParameters.fmrdsRdsLockStatusEv.rssi;
         break;

#if 0
      case FMRDS_EV_RSSI_INFO:
         pMe->m_nCurrentSignalStrength = fmrds_event.fmrdsEventMsgBody.\
                                         tufmrdsStatusParameters.fmrdsRssiInfoEv.rssi;
         break;
#endif

      case FMRDS_EV_RADIO_MODE_SET:
          pMe->nRadioMode = fmrds_event.fmrdsEventMsgBody.tufmrdsStatusParameters.\
                            fmrdsRadioModeEv.radioMode;
         break;

      case FMRDS_EV_PWR_MODE_SET:
          AEEFMRadio_MSG_LOW("PWR mode set",0,0,0);
          pMe->nPwrMode = fmrds_event.fmrdsEventMsgBody.tufmrdsStatusParameters.\
                          fmrdsPwrModeEv.pwrMode;
          break;

      case FMRDS_EV_RX_VOLUME_LEVEL_SET:
          AEEFMRadio_MSG_LOW("Volume level set evet",0,0,0);
          break;

      case FMRDS_EV_SERVICE_AVAILABLE:
         {
            AEECallback *pcb = pMe->m_pRadioEventNotifyCB;
            pMe->m_nRadioEventNotifyStatus[EVT_FMRADIO_SERVICE_AVIALBLE - 1] = 1;

            if(NULL != pcb){
               OEMFMRadio_NotifyEventListeners();
            }

            AEEFMRadio_MSG_LOW("FMRDS service availability ",0,0,0);
         }
         break;

      case FMRDS_EV_RDS_RX_OPTIONS_SET:
         AEEFMRadio_MSG_LOW("RDS Options set event",0,0,0);
         break;

      case FMRDS_EV_RAW_RDS_RBDS_DATA:
         AEEFMRadio_MSG_LOW("Received event for availability of RDS/RBDS data",0,0,0);
         break;
           
      case FMRDS_EV_SEARCH_COMPLETE:
         {

            uint32 Ctr = 0;
            uint16 *pTmpLoc = NULL;
            uint8 nNumChannelsFound =0;
   
            AEECallback *pcb = pMe->m_pCBs[IFMRADIO_CB_GET_CHNLS];
         
             /* the user has cancelled the CALLBACK */
            if( NULL == pcb ){
               break;
            }

            if( SERVICE_NOT_AVAILABLE == fmrds_event.fmrdsEventMsgBody.\
                  tufmrdsStatusParameters.fmrdsSrchComplete.serviceAvble) {
               *pMe->m_pCB_Status[IFMRADIO_CB_GET_CHNLS] = EFAILED;
            }
            else {
               *pMe->m_pCB_Status[IFMRADIO_CB_GET_CHNLS] = SUCCESS;
               nNumChannelsFound = fmrds_event.fmrdsEventMsgBody.\
                              tufmrdsStatusParameters.fmrdsSrchStationsList.noOfStationsFound;

               if ( nNumChannelsFound > pMe->m_nScanChnlsDestLen ) {
                  nNumChannelsFound = pMe->m_nScanChnlsDestLen;
                  *pMe->m_pScanNumChannels = nNumChannelsFound;
               }
               else {
                  *pMe->m_pScanNumChannels = nNumChannelsFound;
               
                  //check again
                  nNumChannelsFound = *pMe->m_pScanNumChannels ;
               }

               pTmpLoc = fmrds_event.fmrdsEventMsgBody.\
                         tufmrdsStatusParameters.fmrdsSrchStationsList.pStationList;

               for(Ctr =0; Ctr < nNumChannelsFound; Ctr++){
                  int nStation = *pTmpLoc++ * 10;
                  *pMe->m_pScanChnlsDest++ = nStation;
               }
   
            }

            // do a proper context switching.
            AEE_ResumeCallback(pcb,(ACONTEXT *)pcb->pmc);
         }
         break;

#ifdef FEATURE_FM_SERVICE_AVAILABLE
      case FMRDS_SERVICE_AVAILABLE:
         
         if(0 == fmrds_event.fmrdsEventMsgBody.tufmrdsStatusParameters.\
               fmrdsFmServiceAvailableEv.serviceAvailable){
         pMe->m_bIsDriverDisConnected = TRUE;
         }
         else{
            pMe->m_bIsDriverDisConnected = FALSE;
         }
#endif
      default:
         break;
   }

   OEMFMRadio_NotifyOEMEventListeners(&fmrds_event);
}

#endif /* FEATURE_FM */


