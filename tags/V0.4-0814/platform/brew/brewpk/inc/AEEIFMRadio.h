#ifndef AEEIFMRADIO_H
#define AEEIFMRADIO_H
/*==============================================================================
FILE: AEEIFMRadio.h

SERVICES: FM Radio related Services

GENERAL DESCRIPTION:
   IFMRadio implements all the APIs that will used by the FM Radio application,
   to control the FM Radio.

PUBLIC CLASSES AND STATIC FUNCTIONS:
   IFMRadio

INITIALIZATION AND SEQUENCING REQUIREMENTS:
      
          COPYRIGHT © 2006-2008 QUALCOMM Incorporated.
                   All Rights Reserved.
                   QUALCOMM Proprietary
==============================================================================*/
#include "AEEIRadio.h"

#define AEEIID_IFMRadio          0x01059908

/* event notifications for FMRadio */
#define EVT_FMRADIO_SIGNAL_STRENGTH_CHANGE      1
#define EVT_FMRADIO_SERVICE_AVIALBLE            2

/* fidelity modes */
#define AEEFIDELITY_MODE_STEREO   0
#define AEEFIDELITY_MODE_MONO     1

typedef uint32 AEEFMRadioFidelityMode;


/* FM Radio band */
#define AEEFMRADIO_BAND_OTHERS      1
#define AEEFMRADIO_BAND_JAPAN       2

/*=============================================================================
IFMRadio INTERFACE
=============================================================================*/
#define INHERIT_IFMRadio(iname)                                               \
   INHERIT_IRadio(iname);                                                     \
   int (*SetFidelityMode)(iname *po, AEEFMRadioFidelityMode nVal);            \
   int (*GetFidelityMode)(iname *po, AEEFMRadioFidelityMode *pnVal)


AEEINTERFACE_DEFINE(IFMRadio);

static __inline uint32 IFMRadio_AddRef(IFMRadio *pIFMRadio)
{
   return AEEGETPVTBL( pIFMRadio, IFMRadio)->AddRef(pIFMRadio);
}

static __inline uint32 IFMRadio_Release(IFMRadio *pIFMRadio)
{
   return AEEGETPVTBL( pIFMRadio, IFMRadio)->Release(pIFMRadio);
}

static __inline int IFMRadio_QueryInterface(IFMRadio  *pIFMRadio,
                                            AEEIID    id,
                                            void      **ppObj)
{
   return AEEGETPVTBL( pIFMRadio, IFMRadio)->QueryInterface(pIFMRadio,
                                                            id,
                                                            ppObj);
}

static __inline void IFMRadio_OnEventNotify(IFMRadio *pIFMRadio,AEECallback *pcb)
{
   AEEGETPVTBL( pIFMRadio, IFMRadio )->OnEventNotify(pIFMRadio, pcb);
}

static __inline int IFMRadio_GetEvents(IFMRadio          *pIFMRadio,
                                       AEERadioEvents    *pnRadioEvents,
                                       int               nSize,
                                       int               *pnReqSize)
{
   return AEEGETPVTBL( pIFMRadio, IFMRadio )->GetEvents(pIFMRadio,
                                                        pnRadioEvents,
                                                        nSize,
                                                        pnReqSize);
}

static __inline void IFMRadio_SetStation(IFMRadio      *pIFMRadio,
                                         int           nFrequency,
                                         int           *pnResult,
                                         AEECallback   *pcb)
{
   AEEGETPVTBL( pIFMRadio, IFMRadio)->SetStation(pIFMRadio,
                                                 nFrequency,
                                                 pnResult,
                                                 pcb);
}


static __inline int IFMRadio_GetTunedStationFrequency(IFMRadio  *pIFMRadio,
                                                      int       *pnStationFreq)
{
   return AEEGETPVTBL( pIFMRadio, IFMRadio )->GetTunedStationFrequency(pIFMRadio,
                                                                       pnStationFreq);
}

static __inline void IFMRadio_SetBand(IFMRadio        *pIFMRadio,
                                      AEERadioBand    nBand,
                                      int             *pnResult,
                                      AEECallback     *pcb)
{
   AEEGETPVTBL( pIFMRadio, IFMRadio)->SetBand(pIFMRadio,
                                              nBand,
                                              pnResult,
                                              pcb);
}

static __inline int IFMRadio_EnablePlay(IFMRadio *pIFMRadio,boolean bVal)
{
   return AEEGETPVTBL( pIFMRadio, IFMRadio)->EnablePlay(pIFMRadio,bVal);
}

static __inline int IFMRadio_GetValidFreqRange(IFMRadio *pIFMRadio,
                                               int      *pnFreqMin,
                                               int      *pnFreqMax)
{
   return AEEGETPVTBL( pIFMRadio, IFMRadio)->GetValidFreqRange(pIFMRadio,
                                                               pnFreqMin,
                                                               pnFreqMax);
}

static __inline void IFMRadio_GetStrongestChannels(
      IFMRadio                   *pIFMRadio,
      int                        *pChannels,
      int                         nChannels,
      int                        *pnReqChannels,
      int                        *pnResult,
      AEECallback                *pcb,
      AEERadioSearchDirection    nSearchVal)
{
   AEEGETPVTBL( pIFMRadio, IFMRadio)->GetStrongestChannels(pIFMRadio,
                                                           pChannels,
                                                           nChannels,
                                                           pnReqChannels,
                                                           pnResult,
                                                           pcb,
                                                           nSearchVal);
}
   
static __inline int IFMRadio_GetSignalStrength(IFMRadio *pIFMRadio,
                                               int      *pnSignalStrength)
{
   return AEEGETPVTBL( pIFMRadio, IFMRadio)->GetSignalStrength(pIFMRadio,
                                                               pnSignalStrength);
}

static __inline int IFMRadio_GetMaxSignalStrength(IFMRadio *pIFMRadio,
                                                  int      *pnMaxSignalStrength)
{
   return AEEGETPVTBL( pIFMRadio,IFMRadio)->GetMaxSignalStrength(
         pIFMRadio,
         pnMaxSignalStrength
         );
}

static __inline int IFMRadio_IsServiceAvailable(IFMRadio  *pIFMRadio,
                                                boolean   *pbBool)
{
   return AEEGETPVTBL( pIFMRadio,IFMRadio)->IsServiceAvailable(pIFMRadio,
                                                               pbBool);
}

static __inline int IFMRadio_SetFidelityMode(IFMRadio                *pIFMRadio,
                                             AEEFMRadioFidelityMode  nVal)
{
   return AEEGETPVTBL( pIFMRadio,IFMRadio)->SetFidelityMode(pIFMRadio,nVal);
}

static __inline int IFMRadio_GetFidelityMode(IFMRadio                *pIFMRadio,
                                             AEEFMRadioFidelityMode  *pnVal)
{
   return AEEGETPVTBL( pIFMRadio,IFMRadio)->GetFidelityMode(pIFMRadio,pnVal);
}

/*=============================================================================
  DATA STRUCTURE DOCUMENTATION
===============================================================================

AEEFMRadioFidelityMode

Description:
   AEEFMRadioFidelityMode is used to specify or read the audio outputs between
   mono and stereo modes
   
Definition:
   #define AEEFIDELITY_MODE_STEREO   0
   #define AEEFIDELITY_MODE_MONO     1

Members:
   AEEFIDELITY_MODE_STEREO : Indicates stereo mode
   AEEFIDELITY_MODE_MONO   : Indicates Mono mode

Comments:
   None

See Also:
   None

===============================================================================

AEERadioEvents

Description:
   The following events are thrown to the registered users

Definition:
   #define EVT_FMRADIO_SIGNAL_STRENGTH_CHANGE      1
   #define EVT_FMRADIO_SERVICE_AVIALBLE            2

Members:
   EVT_FMRADIO_SIGNAL_STRENGTH_CHANGE   : CB is fired when there is a change in
                                          the signal strength
   EVT_FMRADIO_SERVICE_AVIALBLE         : CB is fired when there is a change in
                                          the availability of FMRadio service

Comments:
   None

See Also:
   None

===============================================================================

AEERadioBand

Description:
   Set's the band to tune for the FM-Radio reciever

Definition:
   #define AEEFMRADIO_BAND_OTHERS   1
   #define AEEFMRADIO_BAND_JAPAN    2


Members:
   AEEFMRADIO_BAND_OTHERS  : Set the band to U.S./Europe (87.5 to 108.0 MHz)
   AEEFMRADIO_BAND_JAPAN   : Set the band to Japan (76 to 90 MHz) 

Comments:
   If the user doesn't set the band, the default one would be 
   AEEFMRADIO_BAND_OTHERS.

See Also:
   None

===============================================================================
   INTERFACES DOCUMENTATION
===============================================================================

IFMRadio Interface

   This interface supports the ability to interact with the device FM-Radio
   tuner. 

   The IFMRadio interface provides access to the lower layer FM-Radio tuner.
   Some of the features are,
   - Tune to a station
   - Scan for strongest channels in a band
   - Get notifications about signal strength change
   - Set the fidelity mode for the audio path
   
   Note, this interface supports only tuning of underlying FM-Radio ASIC.
   This doesn't provide any information about RDS/RBDS. For such information
   users will have to use another extension IFMRDS.

===============================================================================

IFMRadio_AddRef

Description:
   This function is inherited from IQI_AddRef().

See Also:
   IFMRadio_Release

===============================================================================

IFMRadio_Release

Description:
   This function is inherited from IQI_Release().

See Also:
   IFMRadio_AddRef
   
===============================================================================

IFMRadio_QueryInterface

Description:
   This function is inherited from IQI_QueryInterface(). 

===============================================================================

IFMRadio_OnEventNotify

Description:
   Users of IFMRadio can register to get notified about any events

Prototype:
   void IFMRadio_OnEventNotify(IFMRadio *pIFMRadio,AEECallback *pcb)

Parameters:
   pIFMRadio     [in]: The IFMRadio interface object
   pcb           [in]: Pointer to the CB that would be fired once we have any
                       events
Return Value:
   None

Comments:
   Users will have to re-register their callback after every event notification
   
See Also:
   IFMRadio_GetEvents
===============================================================================

IFMRadio_GetEvents

Description:
   Once the user is notified about any event he can retrieve the event through
   this call

Prototype:
   int IFMRadio_GetEvents(IFMRadio          *pIFMRadio,
                          AEERadioEvents    *pnRadioEvents,
                          int               nSize,
                          int               *pnReqSize);

Parameters:
   pIFMRadio         [in]: Pointer to the IFMRadio interface object
   pnRadioEvents [in/out]: Array to hold the list of events
   nSize             [in]: Size of the the pnRadioEvents array
   pnReqSize        [out]: On returns contains the total size needed for all
                           the events

Return Value:
   AEE_SUCCESS    : If the call succeeds in retrieving the event(s) information
   AEE_EBADPARM   : If pnReqSize is invalid
   AEE_EFAILED    : Otherwise

Comments:
   If there were more events that were queued up after the user has been
   notified and before this call is made, all those events are also filled

   If user passes in a valid pnRadioEvents pointer but not of complete size to
   hold all the events we fill in as much as we can, and update the pnReqSize
   with the actual size required and return the value as AEE_SUCCESS

See Also:
   None
===============================================================================

IFMRadio_SetStation

Description:
   Changes the current station frequency to the new station and the FM Radio
   will be tuned to the new station.

Prototype:
      void IFMRadio_SetStation (IFMRadio    *pIFMRadio,
                                int         nFrequency,
                                int         *pnResult,
                                AEECallback *pcb);

Parameters:      
   pIFMRadio        [in]: The IFMRadio object.
   nFrequency       [in]: Frequency of the new radio station 
                          Valid Range ( 76000 >= Frequency <= 90000 for Japan)
                          ( 87500 >= Frequency <= 108000 for rest of World)
   pnResult         [out]: Would contain the status of call back function, if 
                          the station is set successfully then would say
                          AEE_SUCCESS else AEE_EFAILED.                          
   pcb              [in]: AEECallback that will be notified when the station
                          is set.

Return Value:
   None

Comments:
   If this function is unable to change to new station then it will play the
   previous playing station.The user should check for the status of pnResult
   only after the callback is called and the pnResult should be valid till
   the CallBack is fired

See Also:
   None

===============================================================================

IFMRadio_GetTunedStationFrequency

Description:
   Returns the currently tuned station frequency if the radio is tuned to any
   station

Prototype:

   int IFMRadio_GetTunedStationFrequency(IFMRadio *pIFMRadio,int *pnStationFreq)

Parameters:
   pIFMRadio        [in]: The IFMRadio object interface
   pnStationFreq   [out]: On return holds the currently tuned station frequency

Return Value:
   AEE_SUCCESS    :  If we can succesully retrieve the currently tuned station
                     frequency
   AEE_EBADPARM   :  If the pnStationFreq is NULL
   AEE_EFAILED    :  Otherwise

Comments:
   The frequency returned would be in KHz

See Also:
   None
===============================================================================

IFMRadio_SetBand

Description:
   Set's the band of the FM-Radio to which it has tune to

Prototype:
   void IFMRadio_SetBand(IFMRadio        *pIFMRadio,
                         AEERadioBand    nBand,
                         int             *pnResult,
                         AEECallback     *pcb);

Parameters:
   pIFMRadio  [in] :  The IFMRadio object
   nBand     [out] :  The band which we have to set to
   pnResult  [out] :  Would contain the status of call back function, if 
                      the station is set successfully then would say
                      AEE_SUCCESS else AEE_EFAILED.                          
   pcb       [in]  :  AEECallback that will be notified when the station
                      is set.

Return Value:
   None

Comments:
   If the band isn't set the default one would be AEEFMRADIO_BAND_OTHERS
   (87.5 to 108.0 MHz)

See Also:
   None

===============================================================================

IFMRadio_EnablePlay

Description:
   Enables or disables the playing of radio based on the second parameter

Prototype:
   int IFMRadio_EnablePlay(IFMRadio *pIFMRadio, boolean bVal);

Parameters:
   pIFMRadio    [in]: The IFMRadio object.
   bVal         [in]: Boolean which will decide to enable or disable the
                      playing of radio

Return Value:
   Returns the AEE return value.
   
   AEE_SUCCESS:  If function succeeds.
   AEE_EFAILED:      If not successful

Comments:
   None

See Also:
   None

===============================================================================

IFMRadio_GetValidFreqRange

Description:
   Gets the valid frequency for the FM-Radio in that particular geographical
   location

Prototype:
   int IFMRadio_GetValidFreqRange(IFMRadio *pIFMRadio,
                                  int      *pnFreqMin,
                                  int      *pnFreqMax);


Parameters:
   pIFMRadio     [in]: The IFMRadio object.
   pnFreqMin    [out]: pointer to frequency, where the minimum of the
                       frequency range will be returned.
   pbFreqMax    [out]: pointer to frequency, where the maximum of the
                       frequency range will be returned.

Return Value:
   Returns the AEE return value.
   AEE_SUCCESS:  If succeeds in retrieving the valid range.
   AEE_EFAILED:  If not successful

Comments:
   This value would be same through out the world except for Japan the range
   is 76.0 MHz to 90.0 MHz while the rest of world is 87.5 MHz to 108.0 MHz

   The value returned would be in kHz, so for 
    Japan  - 76000 (Min) 90000 (Max)
    Others - 87500 (Min) 108000 (Max)

See Also:
   None

===============================================================================

IFMRadio_GetStrongestChannels

Description:
   Get the given number of strongest channels. Caller should allocate the
   memory for the number of channels required

Prototype:
   void IFMRadio_GetStrongestChannels(IFMRadio                   *pIFMRadio,
                                      int                        *pChannels,
                                      int                         nChannels,
                                      int                        *pnReqChannels,
                                      int                        *pnResult,
                                      AEECallback                *pcb,
                                      AEERadioSearchDirection    nSearchVal);

Parameters:
   pIFMRadio          [in]: The IFMRadio object.
   pChannels      [in/out]: pointer to channels. Memory should be allocated by
                            caller.
   nChannels          [in]: Indicates the size of the pChannels
   pnReqChannels  [in/out]: Indicates the number of channels requested upon return
                            would hold the actual channels filled
   pnResult          [out]: Indicates whether the call was successful or not.If
                            the call succeeds contains AEE_SUCCESS else AEE_EFAILED
   pcb                [in]: Callback that would be called when the strongest
                            channels are compiled
   nSearchVal         [in]: Specifies the type of search to be made.

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

===============================================================================

IFMRadio_GetSignalStrength

Description:
   Gets the signal strength of the currently tuned station

Prototype:
   int IFMRadio_GetSignalStrength(IFMRadio  *pIFMRadio,
                                  int       *pnSignalStrength);
Parameters:
   pIFMRadio         [in]: The IFMRadio object.
   pnSignalStrength [out]: Signal strength of the station

Return Value:
   Returns the AEE return value.
   AEE_SUCCESS:  If the signal strength was retrieved successfully
   AEE_EFAILED:      Otherwise.

Comments: 
   None

See Also:
   None

===============================================================================

IFMRadio_GetMaxSignalStrength

Description:
   Gets the Maximum signal strength available for any station

Prototype:
   int IFMRadio_GetMaxSignalStrength(IFMRadio *pIFMRadio,
                                     int      *pnMaxSignalStrength);

Parameters:
   pIFMRadio            [in]: The IFMRadio object.
   pnMaxSignalStrength [out]: Maximum Signal strength for any station

Return Value:
   Returns the AEE return value.
   
   AEE_SUCCESS:  If the signal strength was retrieved successfully
   AEE_EFAILED:      Otherwise

Comments:
   None

See Also:
   None

===============================================================================

IFMRadio_IsServiceAvailable

Description:
   Find the FM Radio Service is Available.

Prototype:
   int IFMRadio_IsServiceAvailable(IFMRadio *pIFMRadio,
                                     boolean  *pbVal);

Parameters:
   pIFMRadio  [in]: The IFMRadio object.
   bVal      [out]: Pointer to Boolean in which TRUE will returned if service
                    is available otherwise FALSE will be returned

Return Value:
   Returns the AEE return value.

   AEE_SUCCESS:  If succeeds in retrieving the FM-Service availability.
   AEE_EFAILED:  If not successful

Comments:
   None

See Also:
   None


===============================================================================

IFMRadio_SetFidelityMode

Description:
   Set the fidelity Mode of the FM Radio

Prototype:
   int IFMRadio_SetFidelityMode(IFMRadio                 *pIFMRadio,
                                AEEFMRadioFidelityMode   nVal);

Parameters:
   pIFMRadio [in]: The IFMRadio object.
   nVal      [in]: The Fidelity Mode

Return Value:
   Returns the AEE return value.
   AEE_SUCCESS:  If the function was able to set the fidelity mode successfully
   AEE_EFAILED:      Otherwise.

Comments:
   None

See Also:
   None

===============================================================================

IFMRadio_GetFidelityMode

Description:
   Get the fidelity Mode of the FM Radio

Prototype:
   int IFMRadio_GetFidelityMode(IFMRadio                 *pIFMRadio,
                                AEEFMRadioFidelityMode   *pnVal);

Parameters:
   pIFMRadio    [in]: The IFMRadio object.
   pnVal     [in/out]: On success contains the current fidelity mode. 

Return Value:
   Returns the AEE return value.
   AEE_SUCCESS:  If the function was able to get the fidelity mode successfully
   AE_EFAILED:   Otherwise.

Comments:
   None

See Also:
   None

=============================================================================*/
#endif //AEEIFMRADIO_H

