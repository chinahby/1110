#ifndef AEEIRADIO_H
#define AEEIRADIO_H
/*==============================================================================
FILE: AEEIRadio.h

SERVICES: Radio related Services

GENERAL DESCRIPTION:
   IRadio implements all the APIs that will used by the Radio application,
   to control the Radio.

PUBLIC CLASSES AND STATIC FUNCTIONS:
   IRadio

INITIALIZATION AND SEQUENCING REQUIREMENTS:
      
          COPYRIGHT © 2007 QUALCOMM Incorporated.
                   All Rights Reserved.
                   QUALCOMM Proprietary
==============================================================================*/
#include "AEEIQI.h"
#include "AEECallback.h"

#define AEEIID_IRadio          0x0106cb2a

/* type definition for Radio band */
typedef uint32 AEERadioBand;

/* type definition for Radio events */
typedef uint32 AEERadioEvents;

/* Search direction for scan */
#define AEERADIO_STN_SEARCH_UP         1
#define AEERADIO_STN_SEARCH_DOWN       2
#define AEERADIO_STN_SEARCH_ALL        3

typedef uint32 AEERadioSearchDirection;


/*=============================================================================
IRadio INTERFACE
=============================================================================*/
#define INHERIT_IRadio(iname)                                                 \
   INHERIT_IQI(iname);                                                        \
   void (*OnEventNotify)(iname *po, AEECallback *pcb);                        \
   int  (*GetEvents)(iname           *po,                                     \
                    AEERadioEvents  *pnRadioEvents,                           \
                    int             nSize,                                    \
                    int             *pnReqSize);                              \
   void (*SetStation)(iname       *po,                                        \
                      int         nFrequency,                                 \
                      int         *pnResult,                                  \
                      AEECallback *pcb);                                      \
   int  (*GetTunedStationFrequency)(iname *po, int *pnStationFreq);           \
   void (*SetBand)(iname             *po,                                     \
                   AEERadioBand      nBand,                                   \
                   int               *pnResult,                               \
                   AEECallback       *pcb);                                   \
   int (*EnablePlay) (iname *po, boolean bVal);                               \
   int (*GetValidFreqRange)(iname *po,int *pnFreqMin, int *pnFreqMax);        \
   void (*GetStrongestChannels)(iname                      *po,               \
                                int                        *pChannels,        \
                                int                        nChannels,         \
                                int                        *pnReqChannels,    \
                                int                        *pnResult,         \
                                AEECallback                *pcb,              \
                                AEERadioSearchDirection    nSearchVal);       \
   int (*GetSignalStrength)(iname *po,int *pnSignalStrength);                 \
   int (*GetMaxSignalStrength)(iname *po,int *pnMaxSignalStrength);           \
   int (*IsServiceAvailable)(iname *po,boolean *pbService)


AEEINTERFACE_DEFINE(IRadio);

static __inline uint32 IRadio_AddRef(IRadio *pIRadio)
{
   return AEEGETPVTBL( pIRadio, IRadio)->AddRef(pIRadio);
}

static __inline uint32 IRadio_Release(IRadio *pIRadio)
{
   return AEEGETPVTBL( pIRadio, IRadio)->Release(pIRadio);
}

static __inline int IRadio_QueryInterface(IRadio      *pIRadio,
                                          AEEIID      id,
                                          void        **ppObj)
{
   return AEEGETPVTBL( pIRadio, IRadio)->QueryInterface(pIRadio,
                                                        id,
                                                        ppObj);
}

static __inline void IRadio_OnEventNotify(IRadio *pIRadio,AEECallback *pcb)
{
   AEEGETPVTBL( pIRadio, IRadio )->OnEventNotify(pIRadio, pcb);
}

static __inline int IRadio_GetEvents(IRadio          *pIRadio,
                                     AEERadioEvents  *pnRadioEvents,
                                     int             nSize,
                                     int             *pnReqSize)
{
   return AEEGETPVTBL( pIRadio, IRadio )->GetEvents(pIRadio,
                                                    pnRadioEvents,
                                                    nSize,
                                                    pnReqSize);
}

static __inline void IRadio_SetStation(IRadio       *pIRadio,
                                       int          nFrequency,
                                       int          *pnResult,
                                       AEECallback  *pcb)
{
   AEEGETPVTBL( pIRadio, IRadio)->SetStation(pIRadio,
                                             nFrequency,
                                             pnResult,
                                             pcb);
}

static __inline int IRadio_GetTunedStationFrequency(IRadio  *pIRadio,
                                                    int     *pnStationFreq)
{
   return AEEGETPVTBL( pIRadio, IRadio )->GetTunedStationFrequency(pIRadio,
                                                                   pnStationFreq);
}
                                                    

static __inline void IRadio_SetBand(IRadio        *pIRadio,
                                    AEERadioBand  nBand,
                                    int           *pnResult,
                                    AEECallback   *pcb)
{
   AEEGETPVTBL( pIRadio, IRadio)->SetBand(pIRadio,
                                          nBand,
                                          pnResult,
                                          pcb);
}

static __inline int IRadio_EnablePlay(IRadio *pIRadio,boolean bVal)
{
   return AEEGETPVTBL( pIRadio, IRadio)->EnablePlay(pIRadio,bVal);
}

static __inline int IRadio_GetValidFreqRange(IRadio   *pIRadio,
                                             int      *pnFreqMin,
                                             int      *pnFreqMax)
{
   return AEEGETPVTBL( pIRadio, IRadio)->GetValidFreqRange(pIRadio,
                                                           pnFreqMin,
                                                           pnFreqMax);
}

static __inline void IRadio_GetStrongestChannels(
      IRadio                     *pIRadio,
      int                        *pChannels,
      int                         nChannels,
      int                        *pnReqChannels,
      int                        *pnResult,
      AEECallback                *pcb,
      AEERadioSearchDirection    nSearchVal
      )
{
   AEEGETPVTBL( pIRadio, IRadio)->GetStrongestChannels(pIRadio,
                                                       pChannels,
                                                       nChannels,
                                                       pnReqChannels,
                                                       pnResult,
                                                       pcb,
                                                       nSearchVal);
}
   
static __inline int IRadio_GetSignalStrength(IRadio *pIRadio,
                                             int    *pnSignalStrength)
{
   return AEEGETPVTBL( pIRadio, IRadio)->GetSignalStrength(pIRadio,
                                                           pnSignalStrength);
}

static __inline int IRadio_GetMaxSignalStrength(IRadio *pIRadio,
                                                int    *pnMaxSignalStrength)
{
   return AEEGETPVTBL( pIRadio,IRadio)->GetMaxSignalStrength(pIRadio,
                                                             pnMaxSignalStrength);
}

static __inline int IRadio_IsServiceAvailable(IRadio    *pIRadio,
                                              boolean   *pbService)
{
   return AEEGETPVTBL( pIRadio,IRadio)->IsServiceAvailable(pIRadio,pbService);
}

/*=============================================================================
  DATA STRUCTURE DOCUMENTATION
===============================================================================
AEERadioSearchDirection

Description:
   AEERadioSearchDirection is used to tell Radio in which direction to 
   search for stations when doing a band scan

Definition:
   #define AEERADIO_STN_SEARCH_UP         1
   #define AEERADIO_STN_SEARCH_DOWN       2
   #define AEERADIO_STN_SEARCH_ALL        3

   typedef uint32 AEERadioSearchDirection;

Members:
   AEERADIO_STN_SEARCH_UP   :  Scan upwards in the band from the current
                               frequency
   AEERADIO_STN_SEARCH_DOWN :  Search downwards in the band from the current
                               frequency
   AEERADIO_STN_SEARCH_ALL  :  Scan the entire band for the best channels

Comments:
   None

See Also:
   None

===============================================================================
   INTERFACES DOCUMENTATION
===============================================================================

IRadio Interface

   This interface supports the ability to interact with the device Radio
   tuner. 

   The IRadio interface provides access to the lower layer Radio tuner.
   Some of the features are,
   - Tune to a station
   - Scan for strongest channels in a band
   - Get notifications about any known changes
   
===============================================================================

IRadio_AddRef

Description:
   This function is inherited from IQI_AddRef().

See Also:
   IRadio_Release

===============================================================================

IRadio_Release

Description:
   This function is inherited from IQI_Release().

See Also:
   IRadio_AddRef
   
===============================================================================

IRadio_QueryInterface

Description:
   This function is inherited from IQI_QueryInterface(). 

===============================================================================

IRadio_OnEventNotify

Description:
   Users of IRadio can register to get notified about any events

Prototype:
   void IRadio_OnEventNotify(IRadio *pIRadio,AEECallback *pcb);

Parameters:
   pIRadio     [in]: The IRadio interface object
   pcb         [in]: Pointer to the CB that would be fired once we have any
                     events

Return Value:
   None

Comments:
   Users will have to re-register their callback after every event notification
   
See Also:
   IRadio_GetEvents
===============================================================================

IRadio_GetEvents

Description:
   Once the user is notified about any event he can retrieve the event through
   this call

Prototype:
   int IRadio_GetEvents(IRadio          *pIRadio,
                        AEERadioEvents  *pnRadioEvents,
                        int             nSize,
                        int             *pnReqSize);

Parameters:
   pIRadio           [in]: Pointer to the IRadio interface object
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

IRadio_SetStation

Description:
   Changes the current station frequency to the new station and the Radio
   will be tuned to the new station.

Prototype:
      void IRadio_SetStation (IRadio      *pIRadio,
                              int         nFrequency,
                              int         *pnResult,
                              AEECallback *pcb);

Parameters:      
   pIRadio          [in]: The IRadio object.
   nFrequency       [in]: Frequency of the new radio station 
   pnResult         [out]: Would contain the status of call back function, if 
                          the station is set successfully then would say
                          AEE_SUCCESS else AEE_EFAILED.                          
   pcb              [in]: AEECallback that will be notified when the station
                          is set.

Return Value:
   None

Comments:
   The user should check for the status of pnResult only after the callback is
   called and pnResult should be valid till the callback is fired

See Also:
   None
===============================================================================

IRadio_GetTunedStationFrequency

Description:
   Returns the currently tuned station frequency if the radio is tuned to any
   station

Prototype:
   int IRadio_GetTunedStationFrequency(IRadio *pIRadio,int *pnStationFreq);

Parameters:
   pIRadio        [in]: The IRadio object interface
   pnStationFreq [out]: On return holds the currently tuned station frequency

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

IRadio_SetBand

Description:
   Set's the band of the Radio to which it has tune to

Prototype:
   void IRadio_SetBand(IRadio          *pIRadio,
                       AEERadioBand    nBand,
                       int             *pnResult,
                       AEECallback     *pcb);

Parameters:
   pIRadio   [in]  :  The IRadio object
   nBand     [out] :  The band which we have to set to
   pnResult  [out] :  Would contain the status of call back function, if 
                      the station is set successfully then would say
                      AEE_SUCCESS else AEE_EFAILED.                          
   pcb       [in]  :  AEECallback that will be notified when the station
                      is set.

Return Value:
   None

Comments:
   The user should check for the status of pnResult only after the callback is
   called and pnResult should be valid till the callback is fired

See Also:
   None

===============================================================================

IRadio_EnablePlay

Description:
   Enables or disables the playing of radio audio based on the second parameter

Prototype:
   int IRadio_EnablePlay(IRadio *pIRadio, boolean bVal);

Parameters:
   pIRadio    [in]: The IRadio object.
   bVal       [in]: Boolean which will decide to enable or disable the
                    playing of radio

Return Value:
   Returns the AEE return value.
   
   AEE_SUCCESS:  If function succeeds.
   AEE_EFAILED:  If not successful

Comments:
   None

See Also:
   None

===============================================================================

IRadio_GetValidFreqRange

Description:
   Gets the valid frequency for the Radio in that particular geographical
   location

Prototype:
   int IRadio_GetValidFreqRange(IRadio *pIRadio,
                                int    *pnFreqMin,
                                int    *pnFreqMax);


Parameters:
   pIRadio       [in]: The IRadio object.
   pnFreqMin    [out]: pointer to frequency, where the minimum of the
                       frequency range will be returned.
   pnFreqMax    [out]: pointer to frequency, where the maximum of the
                       frequency range will be returned.

Return Value:
   Returns the AEE return value.

   AEE_SUCCESS:  If succeeds in retrieving the valid range.
   AEE_EFAILED:  If not successful

Comments:
   The value returned would be in kHz

See Also:
   None

===============================================================================

IRadio_GetStrongestChannels

Description:
   Get the given number of strongest channels. Caller should allocate the
   memory for the number of channels required

Prototype:
   void IRadio_GetStrongestChannels(IRadio                     *pIRadio,
                                    int                        *pChannels,
                                    int                         nChannels,
                                    int                        *pnReqChannels,
                                    int                        *pnResult,
                                    AEECallback                *pcb,
                                    AEERadioSearchDirection    nSearchVal)

Parameters:
   pIRadio            [in]: The IRadio object.
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

IRadio_GetSignalStrength

Description:
   Gets the signal strength of the currently tuned station

Prototype:
   int IRadio_GetSignalStrength(IRadio  *pIRadio,
                                int     *pnSignalStrength);
Parameters:
   pIRadio           [in]: The IRadio object.
   pnSignalStrength [out]: Signal strength of the station

Return Value:
   Returns the AEE return value.
   AEE_SUCCESS :  If the signal strength was retrieved successfully
   AEE_EFAILED :  Otherwise.

Comments: 
   None

See Also:
   None

===============================================================================

IRadio_GetMaxSignalStrength

Description:
   Gets the Maximum signal strength available for any station

Prototype:
   int IRadio_GetMaxSignalStrength(IRadio *pIRadio,
                                   int    *pnMaxSignalStrength);

Parameters:
   pIRadio              [in]: The IRadio object.
   pnMaxSignalStrength  [out]: Maximum Signal strength for any station

Return Value:
   Returns the AEE return value.
   
   AEE_SUCCESS :  If the signal strength was retrieved successfully
   AEE_EFAILED :  Otherwise

Comments:
   None

See Also:
   None

===============================================================================

IRadio_IsServiceAvailable

Description:
   Find if the Radio Service is Available.

Prototype:
   int IRadio_IsServiceAvailable(IRadio   *pIRadio,
                                 boolean  *pbService);

Parameters:
   pIRadio      [in]: The IRadio object.
   pbService   [out]: Pointer to Boolean in which TRUE will returned if service
                      is available otherwise FALSE will be returned

Return Value:
   Returns the AEE return value.

   AEE_SUCCESS :  If succeeds in retrieving the Service availability.
   AEE_EFAILED :  If not successful

Comments:
   None

See Also:
   None

=============================================================================*/
#endif //AEEIRADIO_H

