#ifndef OEMVOCODER_H
#define OEMVOCODER_H
/*****************************************************************************
                    IMPORTANT NOTICE

                  OEM DISTRIBUTION ONLY

     O E M       V O C O D E R     I N T E R F A C E

  This file contains the definition of the functions needed by the IVocoder
  implementation in the AEE Layer.

        Copyright 1999-2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
*****************************************************************************/

#include "AEE_OEMComdef.h"
#include "AEEVocoder.h"

OEMINSTANCE OEMVocoder_New(AEECLSID classId, int *nErr);

int    OEMVocoder_Delete(OEMINSTANCE handle);

int16  OEMVocoder_Configure(OEMINSTANCE handle,
                            const IVocoderConfigType* config,
                            IVocoderInfoType * pInfo);

int16  OEMVocoder_InStart(OEMINSTANCE handle);

int16  OEMVocoder_InStop(OEMINSTANCE handle);

int16  OEMVocoder_InReset(OEMINSTANCE handle);

int16  OEMVocoder_OutStart(OEMINSTANCE handle);

int16  OEMVocoder_OutStop(OEMINSTANCE handle);

int16  OEMVocoder_OutReset(OEMINSTANCE handle);


int16  OEMVocoder_InRead(OEMINSTANCE handle,
                         DataRateType * pRate,
                         uint16 * pLength,
                         uint8 * pData);

int16  OEMVocoder_OutWrite(OEMINSTANCE handle,
                           DataRateType rate,
                           uint16 length,
                           const uint8 * pData);

int16  OEMVocoder_GetInDepth(OEMINSTANCE handle,
                             uint16 * pDepth);

int16  OEMVocoder_GetOutDepth(OEMINSTANCE handle,
                              uint16 * pDepth);

int16  OEMVocoder_GetFrameLength(OEMINSTANCE handle,
                                 VocoderType vocoder,
                                 DataRateType rate,
                                 uint16 * pLength);

int16  OEMVocoder_PlayTone(OEMINSTANCE handle,
                           uint16 tone1,
                           uint16 tone2,
                           int16 volume,
                           uint16 duration);


int    OEMVocoder_RegisterCB(AEECallback *cbReady,
                            AEECallback *cbHaveData,
                            AEECallback *cbNeedData,
                            AEECallback *cbPlayedData);

uint16 OEMVocoder_GetDataAvailCount( void);

uint16 OEMVocoder_GetErasureCount(void );

uint16 OEMVocoder_GetDataPlayedCount(void);



/*========================================================================
  INTERFACES DOCUMENTATION
=======================================================================
OEMVocoder Interface
===================================================================

Function: OEMVocoder_New()
Description:
   This function is invoked when a new instance of IVocoder is to be created.
   Any per-instance initialization of the OEMVocoder must be done here.
   When this instance needs to be cleaned up, the function OEMVocoder_Delete() is invoked
   by the AEE layer.

Prototype:
   OEMINSTANCE OEMVocoder_New(AEECLSID classId, int *nErr);

Parameters:
   classiD:ClassID of the Vocoder interface that must be created
   nErr:   Pointer to contain the error code on return

Return Value:
   On success, returns a valid OEMINSTANCE. This is used by the AEE layer for further
   communication w.r.t. this instance of the OEM Vocoder.
   On faiure, returns NULL

Comments:
   None

Side Effects:
   None

See Also:
   OEMVocoder_Delete()

=======================================================================
Function: OEMVocoder_Delete()

Description:
   This function is invoked when the OEMVocoder instance needs to be deleted. Any per-instance
   cleanup must be done here.

Prototype:
   int    OEMVocoder_Delete(OEMINSTANCE handle);

Parameters:
   handle: OEMINSTANCE corresponding to the OEMVocoder instance that must be deleted.


Return Value:
   AEE_SUCCESS: On Success
   EFAILED: On Failure


Comments:
   None

Side Effects:
   None

See Also:
   OEMVocoder_New()

=======================================================================
Function: OEMVocoder_Configure()

Description:
   This function is invoked when the OEMVocoder instance needs to be configured to use a specifiv vocoder suppported
   by the device. Attempting to re-configure an already configured vocoder can first result in the
   previous configuration to be shutdown before attempting the new configuration.
   When this function is invoked with configuration set to VOC_NONE (i.e. with config.config set to VOC_NONE),
   the vocoder must be shutdown.


Prototype:
   int16  OEMVocoder_Configure(OEMINSTANCE handle,const IVocoderConfigType* config, IVocoderInfoType * pInfo);

Parameters:
   handle: OEMINSTANCE corresponding to the OEMVocoder instance that must be configured.
   config:[in]:   Configuration parameters for the vocoder object
   pInfo:[out]:    Configuration information about the selected vocoder


Return Value:
   SUCCESS                    Operation succeeded
   EBADPARM                   zero watermark value. Must be positive
   ENOMEMORY                  Not able to allocate memory for vocoder data buffers
   AEE_VOC_ENOT_AVAIL         Specified vocoder is not available
   AEE_VOC_ERATE_OUT_OF_RANGE Specified min or max rate is not supported by the
                              vocoder
   AEE_VOC_EBAD_TX_REDUCTION  Inappropriate tx reduction value
   AEE_VOC_IN_USE             Vocoder is currently being used by a different instance of IVocoder.
   ENOMEMORY:                 Insufficient memory in the system. The previous configuration of the vocoder will be
                              shutdown.
   EFAILED:                   Configuration Failed.The previous configuration of the vocoder will be
                              shutdown


Comments:
   None

Side Effects:
   None

See Also:
   OEMVocoder_New()

=======================================================================
Function: OEMVocoder_InStart()

Description:
   This function is invoked to start encoding voice frames


Prototype:
   int16  OEMVocoder_InStart(OEMINSTANCE handle);

Parameters:
   handle: OEMINSTANCE corresponding to the OEMVocoder instance that must be used to encode voice frames.

Return Value:
   SUCCESS                    Operation succeeded
   EBADCLASS                  Invalid handle
   AEE_VOC_EIN_USE            IVOCODER in use by other application or other instance of IVocoder
   AEE_VOC_ENOT_CONFIGED      IVOCODER not configured

Comments:
   Have data callback will be invoked when frames are available

Side Effects:
   Begins accepting and queueing encoded frames from the vocoder

See Also:
   OEMVocoder_Configure()
   OEMVocoder_InStop()

=======================================================================
Function: OEMVocoder_InStop()

Description:
   This function is invoked to stop encoding voice frames


Prototype:
   int16  OEMVocoder_InStop(OEMINSTANCE handle);

Parameters:
   handle: OEMINSTANCE corresponding to the OEMVocoder instance that must be used to
   stop encoding voice frames.

Return Value:
   SUCCESS                    Operation succeeded
   EBADCLASS                  Invalid handle
   AEE_VOC_EIN_USE            IVOCODER in use by other application or other instance of IVocoder
   AEE_VOC_ENOT_CONFIGED      IVOCODER not configured

Comments:
   Does not clear the queue of encoded frames

Side Effects:
   Ceases accepting and queueing encoded frames from the vocoder

See Also:
   OEMVocoder_Configure()
   OEMVocoder_InStart()

=======================================================================
Function: OEMVocoder_InReset()

Description:
   This function is invoked to Clear the queue of encoded vocoder frames


Prototype:
   int16  OEMVocoder_InReset(OEMINSTANCE handle);

Parameters:
   handle: OEMINSTANCE corresponding to the OEMVocoder instance that must be used

Return Value:
   SUCCESS                    Operation succeeded
   EBADCLASS                  Invalid handle
   AEE_VOC_EIN_USE            IVOCODER in use by other application or other instance of IVocoder
   AEE_VOC_ENOT_CONFIGED      IVOCODER not configured

Comments:
   This operation will resume prior operation

Side Effects:
   frees queued frames

See Also:
   OEMVocoder_Configure()
   OEMVocoder_InStop()

=======================================================================
Function: OEMVocoder_OutStart()

Description:
   This function is invoked to Start the decoding and playout of queued frames


Prototype:
   int16  OEMVocoder_OutStart(OEMINSTANCE handle);

Parameters:
   handle: OEMINSTANCE corresponding to the OEMVocoder instance that must be used

Return Value:
   SUCCESS                    Operation succeeded
   EBADCLASS                  Invalid handle
   AEE_VOC_EIN_USE            IVOCODER in use by other application or other instance of IVocoder
   AEE_VOC_ENOT_CONFIGED      IVOCODER not configured

Comments:
   The vocoder will not be started until the watermark of queued
   frames for decoding has been reached.  It may be necessary to append enough
   ERASURE frames to the queue in order to meet the watermark and start playout

Side Effects:


See Also:
   OEMVocoder_OutStop()

=======================================================================
Function: OEMVocoder_OutStop()

Description:
   This function is invoked to stop the decoding and playout of queued frames


Prototype:
   int16  OEMVocoder_OutStop(OEMINSTANCE handle);

Parameters:
   handle: OEMINSTANCE corresponding to the OEMVocoder instance that must be used

Return Value:
   SUCCESS                    Operation succeeded
   EBADCLASS                  Invalid handle
   AEE_VOC_EIN_USE            IVOCODER in use by other application or other instance of IVocoder
   AEE_VOC_ENOT_CONFIGED      IVOCODER not configured

Comments:
   Does not clear the already queued frames

Side Effects:


See Also:
   OEMVocoder_OutStart()

=======================================================================
Function: OEMVocoder_OutReset()

Description:
   This function is invoked to Clear the queue of frames available for decoding


Prototype:
   int16  OEMVocoder_OutReset(OEMINSTANCE handle);

Parameters:
   handle: OEMINSTANCE corresponding to the OEMVocoder instance that must be used

Return Value:
   SUCCESS                    Operation succeeded
   EBADCLASS                  Invalid handle
   AEE_VOC_EIN_USE            IVOCODER in use by other application or other instance of IVocoder
   AEE_VOC_ENOT_CONFIGED      IVOCODER not configured

Comments:
   This operation will resume prior operation

Side Effects:
   If the vocoder was previously playing, it will wait for frames until the
   watermark is reached again and then will resume playout

See Also:
   OEMVocoder_OutStop()

=======================================================================

Function: OEMVocoder_InRead()

Description:
   This function is invoked to Obtain the oldest encoded voice frame from the queue

Prototype:
   int16  OEMVocoder_InRead(OEMINSTANCE handle,
                            DataRateType * pRate,
                            uint16 * pLength,
                            uint8 * pData);
Parameters:
   handle:     OEMINSTANCE corresponding to the OEMVocoder instance that must be used

   pRate:[in]: Pointer to storage to accept data rate of the frame

   pLength:[in/out]:  Pointer to storage to accept size in bytes of the frame. This parameter must be
   non-NULL. On input, *pLength must specify the size of the buffer pData. On return, *pLength
   will contain the actual size of the data filled in the buffer pData by this function.
   If the size of pData is lesser than the actual size of the frame, *pLength will be set to the actual size of the frame on return
   but there will be a data loss i.e. pData will be filled only with as much space as the size of pData
   (i.e value of *pLength on input).

   pData:[in/out]:    Pointer to storage to accept the encoded frame bytes

Return Value:
   SUCCESS                    Operation succeeded
   EBADCLASS                  Invalid handle
   AEE_VOC_EIN_USE            IVOCODER in use by other application or other instance of IVocoder
   AEE_VOC_ENOT_CONFIGED      IVOCODER not configured
   AEE_VOC_EBUFFER_EMPTY      No encoded frame is queued

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMVocoder_OutWrite()

Description:
   This function provides a new frame of data for the decoder

Prototype:
   int16  OEMVocoder_OutWrite(OEMINSTANCE handle,
                              DataRateType rate,
                              uint16 length,
                              const uint8 * pData);

Parameters:
   handle:     OEMINSTANCE corresponding to the OEMVocoder instance that must be used

   rate:[in]:     Data rate of the frame
   length:[in]:   Size in bytes of the frame
   pData:[out]:    Pointer to the frame data bytes

Return Value:
   SUCCESS                    Operation succeeded
   EBADCLASS                  Invalid handle
   EBADPARM                   Frame length is not appropriate for the specified
                              rate
   AEE_VOC_EIN_USE            IVOCODER in use by other application
   AEE_VOC_ENOT_CONFIGED      IVOCODER not configured
   AEE_VOC_EBUFFER_FULL       No more frames can be accepted by the output queue
   AEE_VOC_ENOT_STARTED       Frame queued for playout when playout op not started

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================
Function: OEMVocoder_GetInDepth()

Description:
   This function returns the number of encoded frames available in the queue.

Prototype:
   int16  OEMVocoder_GetInDepth(OEMINSTANCE handle,
                                uint16 * pDepth);

Parameters:
   handle:     OEMINSTANCE corresponding to the OEMVocoder instance that must be used
   pDepth:[in/out]: Pointer to storage to accept count of queued frames


Return Value:
   SUCCESS                    Operation succeeded
   EBADCLASS                  Invalid handle
   EBADPARM                   Bad input parameter pDepth
   AEE_VOC_EIN_USE            IVOCODER in use by other application
   AEE_VOC_ENOT_CONFIGED      IVOCODER not configured

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================
Function: OEMVocoder_GetOutDepth()

Description:
   This function returns the number of frames that are currently queued for playout (decoding).

Prototype:
   int16  OEMVocoder_GetOutDepth(OEMINSTANCE handle,
                                 uint16 * pDepth);

Parameters:
   handle:     OEMINSTANCE corresponding to the OEMVocoder instance that must be used
   pDepth:[in/out]: Pointer to storage to accept count of queued frames


Return Value:
   SUCCESS                    Operation succeeded
   EBADCLASS                  Invalid handle
   EBADPARM                   Frame length is not appropriate for the specified
                              rate
   AEE_VOC_EIN_USE            IVOCODER in use by other application
   AEE_VOC_ENOT_CONFIGED      IVOCODER not configured
   AEE_VOC_EBUFFER_FULL       No more frames can be accepted by the output queue
   AEE_VOC_ENOT_STARTED       Frame queued for playout when playout op not started

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================
Function: OEMVocoder_GetFrameLength()

Description:
   This function provides the size of a specified vocoder frame at a given rate

Prototype:
   int16  OEMVocoder_GetFrameLength(OEMINSTANCE handle,
                                    VocoderType vocoder,
                                    DataRateType rate,
                                    uint16 * pLength);

Parameters:
   handle:     OEMINSTANCE corresponding to the OEMVocoder instance that must be used
   vocoder:[in]:  Vocoder type of the frame
   rate:[in]:     Data rate of the frame
   pLength:[in/out]:  Pointer to storage to accept size of the frame


Return Value:
   SUCCESS        Operation succeeded
   EBADCLASS      Invalid handle or invalid vocoder or rate

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================
Function: OEMVocoder_PlayTone()

Description:
   This function plays a tone through the vocoder

Prototype:
   int16  OEMVocoder_PlayTone(OEMINSTANCE handle,
                              uint16 tone1,
                              uint16 tone2,
                              int16 volume,
                              uint16 duration);
Parameters:
   handle:     OEMINSTANCE corresponding to the OEMVocoder instance that must be used
   tone1    frequency of first tone
   tone2    frequency of second tone
   volume   volume of tones
   duration duration of tones



Return Value:
   SUCCESS           Operation succeeded
   AEE_VOC_EACTIVE   IVOCODER currently playing or acquiring
   EFAILED:          Operation failed

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================
Function: OEMVocoder_RegisterCB()

Description:
   This function registers a set of callbacks that must be invoked by the OEMVocoder. Each callback
   is used for a specific reason as indicated below.
   These callbacks must be invoked using BREW's resume functionality (AEE_ResumeCallback)

Prototype:
   int    OEMVocoder_RegisterCB(AEECallback *cbReady,
                                AEECallback *cbHaveData,
                                AEECallback *cbNeedData,
                                AEECallback *cbPlayedData);

Parameters:
   cbReady:[in]:       If Non-NULL, this callback must be invoked when the Vocoder is Ready for use
   cvHaveData:[in]:    If Non-NULL, this Callback function invoked when encoder has frames available
   cbNeedData:[in]:    If Non-NULL, this Callback function invoked when decoder requires more frames
   cbPlayedData:[in]:  If Non-NULL, this Callback function invoked when decoder has played one or more
                       frames


Return Value:
   SUCCESS           Registration of Callbacks succeeded
   EFAILED:          Registration of Callbacks failed

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================
Function: OEMVocoder_GetDataAvailCount()

Description:
   This function returns the number of decoded frames currently available in the vocoder queue

Prototype:
   uint16 OEMVocoder_GetDataAvailCount( void);

Parameters:
   None

Return Value:
   Number of decoded frames available

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================
Function: OEMVocoder_GetErasureCount()

Description:
   This function returns the number of erasure frames that have been stuffed while data was not
   available since last invocation of this function

Prototype:
   uint16 OEMVocoder_GetErasureCount(void );

Parameters:
   None

Return Value:
   Number of erasure frames played

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================
Function: OEMVocoder_GetDataPlayedCount()

Description:
   This function returns the number of encoded frames played since last invocation of this function.

Prototype:
   uint16 OEMVocoder_GetDataPlayedCount(void);

Parameters:
   None

Return Value:
   Number of erasure frames played

Comments:
   None

Side Effects:
   None

See Also:
   None

==============================================================
AEEVocoder

See the IVocoder Interface in the BREW API Reference.

=======================================================================
*/

#endif //OEMVOCODER_H
