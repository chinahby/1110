#ifndef AEEVOCODER_H
#define AEEVOCODER_H

/*===========================================================================
FILE:      AEEVocoder.h

SERVICES:
   Provide frame-level access to the vocoder (voice encoder/decoder) device

DESCRIPTION:
   The IVocoder class provides streaming access to output from the voice
   encoder and supports streaming of frames to the voice decoder for playout

PUBLIC CLASSES:  IVocoder

INITIALIZATION AND SEQUENCING REQUIREMENTS: NA

        Copyright © 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===============================================================================
    INCLUDES AND VARIABLE DEFINITIONS
===============================================================================*/

/*-------------------------------------------------------------------
            Defines
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
      Include Files
-------------------------------------------------------------------*/

#include "AEE.h"
#include "AEEShell.h"

/*-------------------------------------------------------------------
      Type Declarations
-------------------------------------------------------------------*/
typedef struct _IVocoder IVocoder;

enum {
   BLANK_RATE,          // Indicates data was blanked
   EIGHTH_RATE,         // Indicates rate 1/8 data
   QUARTER_RATE,        // Indicates rate 1/4 data
   HALF_RATE,           // Indicates rate 1/2 data
   FULL_RATE,           // Indicates rate 1   data
   ERASURE,             // Indicates erasure frame
   MAX_RATE             // last enum
};

enum {
  AMR_MODE_0475,        // 4.75 kbit /s
  AMR_MODE_0515,        // 5.15 kbit /s
  AMR_MODE_0590,        // 5.90 kbit /s
  AMR_MODE_0670,        // 6.70 kbit /s
  AMR_MODE_0740,        // 7.40 kbit /s
  AMR_MODE_0795,        // 7.95 kbit /s
  AMR_MODE_1020,        // 10.2 kbit /s
  AMR_MODE_1220,        // 12.2 kbit /s
  AMR_MODE_MAX          // last enum
};

enum {
  IVOC_G711_MODE_MULAW,  // G.711, mulaw format
  IVOC_G711_MODE_ALAW,   // G.711, alaw format
  IVOC_G711_MODE_MAX     // last enum
};

#ifdef WIN32
typedef unsigned DataRateType;
#else
typedef int8 DataRateType;
#endif


enum {
   VOC_NONE,
   VOC_IS96,            // IS-96  - QCELP-8k
   VOC_IS96A,           // IS-96A - QCELP-8k
   VOC_IS733,           // IS-733 - QCELP-13k
   VOC_IS127,           // IS-127 - EVRC-8K
   VOC_SMV,             // SMV for QChat Demo
   VOC_PCM,             // PCM
   VOC_AMR,             // AMR
   VOC_G711,            // G.711
   VOC_4GV_NB,          // 4GV narrow band
   VOC_MAX              // Used to determine size of enum
};
#ifdef WIN32
typedef unsigned VocoderType;
#else
typedef int8 VocoderType;
#endif


typedef void (*PFNREADY)(void * usrPtr);
typedef void (*PFNDATA)(uint16 numFrames,
                        void * usrPtr);

#define IVOC_AMR_SCRMODE 2
#define IVOC_AMR_DTXMODE 4

typedef struct
{
   void * usrPtr;
   PFNREADY readyCB;
   PFNDATA haveDataCB;
   PFNDATA needDataCB;
   PFNDATA playedDataCB;
   uint32 watermark;
   uint16 txReduction;
   DataRateType max;
   DataRateType min;
   VocoderType vocoder;
   boolean overwrite;
} IVocoderConfigType;

typedef struct
{
   uint16 frameDuration;
   uint16 maxFrameSize;
} IVocoderInfoType;

//******************************************************************************
//
// IVocoder Interface
//
//******************************************************************************

QINTERFACE(IVocoder)
{
   DECLARE_IBASE(IVocoder)

   int16   (*VocConfigure)(IVocoder * pVoc,
                           IVocoderConfigType config,
                           IVocoderInfoType * pInfo);

   int16   (*VocInStart)(IVocoder * pVoc);
   int16   (*VocInStop)(IVocoder * pVoc);
   int16   (*VocInReset)(IVocoder * pVoc);
   int16   (*VocInRead)(IVocoder * pVoc,
                        DataRateType * pRate,
                        uint16 * pLength,
                        uint8 * pData);

   int16   (*VocOutStart)(IVocoder * pVoc);
   int16   (*VocOutStop)(IVocoder * pVoc);
   int16   (*VocOutReset)(IVocoder * pVoc);
   int16   (*VocOutWrite)(IVocoder * pVoc,
                          DataRateType rate,
                          uint16 length,
                          const uint8 * pData);

   int16   (*GetOutDepth)(IVocoder * pVoc,
                          uint16 * pDepth);
   int16   (*GetInDepth)(IVocoder * pVoc,
                         uint16 * pDepth);
   int16   (*GetFrameLength)(IVocoder * pVoc,
                             VocoderType vocoder,
                             DataRateType rate,
                             uint16 * pLength);
   int16   (*PlayTone)(IVocoder * pVoc,
                       uint16 tone1,
                       uint16 tone2,
                       int16 volume,
                       uint16 duration);
};

#define IVOCODER_AddRef(p)               GET_PVTBL(p, IVocoder)->AddRef(p)
#define IVOCODER_Release(p)              GET_PVTBL(p, IVocoder)->Release(p)
#define IVOCODER_VocConfigure(p,c,i)     GET_PVTBL(p, IVocoder)->VocConfigure(p,c,i)
#define IVOCODER_VocInStart(p)           GET_PVTBL(p, IVocoder)->VocInStart(p)
#define IVOCODER_VocInStop(p)            GET_PVTBL(p, IVocoder)->VocInStop(p)
#define IVOCODER_VocInReset(p)           GET_PVTBL(p, IVocoder)->VocInReset(p)
#define IVOCODER_VocInRead(p,r,l,d)      GET_PVTBL(p, IVocoder)->VocInRead(p,r,l,d)
#define IVOCODER_VocOutStart(p)          GET_PVTBL(p, IVocoder)->VocOutStart(p)
#define IVOCODER_VocOutStop(p)           GET_PVTBL(p, IVocoder)->VocOutStop(p)
#define IVOCODER_VocOutReset(p)          GET_PVTBL(p, IVocoder)->VocOutReset(p)
#define IVOCODER_VocOutWrite(p,r,l,d)    GET_PVTBL(p, IVocoder)->VocOutWrite(p,r,l,d)
#define IVOCODER_GetInDepth(p,d)         GET_PVTBL(p, IVocoder)->GetInDepth(p,d)
#define IVOCODER_GetOutDepth(p,d)        GET_PVTBL(p, IVocoder)->GetOutDepth(p,d)
#define IVOCODER_GetFrameLength(p,v,r,l) GET_PVTBL(p, IVocoder)->GetFrameLength(p,v,r,l)
#define IVOCODER_PlayTone(p,t1,t2,v,d)   GET_PVTBL(p, IVocoder)->PlayTone(p,t1,t2,v,d)

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================

IVocoderConfigType

Description:

   The IVocoderConfigType structure supplies the information
   required to operate the vocoder device and supply/obtain
   frames

Definition:

   typedef struct
   {
      void * usrPtr;
      PFNREADY ReadyCB;
      PFNDATA haveDataCB;
      PFNDATA needDataCB;
      PFNDATA playedDataCB;
      uint32 watermark;
      uint16 txReduction;
      DataRateType max;
      DataRateType min;
      VocoderType vocoder;
      boolean overwrite;
   } IVocoderConfigType;

Members:
   usrPtr:        User data to be passed back to application in callbacks
   readyCB:       Callback function invoked when vocoder is ready for use
   haveDataCB:    Callback function invoked when encoder has frames available
   needDataCB:    Callback function invoked when decoder requires more frames
   playedDataCB:  Callback function invoked when decoder has played one+ frames
   watermark:     Frame count to be accumulated before starting decoder sound
                  path, the size of both the in and out buffers, and non-real
                  time operation compensation.
                    (* more below)
   txReduction:   Mode of vocoder operation (** more below)
   max:           Maximum encoder data rate, 
                  For VOC_AMR vocoder it specifies the mode to use and 
                  For VOC_G711 vocoder it specifies which companding law to use.
   min:           Minimum encoder data rate
   vocoder:       Vocoder algorithm to use
   overwrite:     Encoder overflow to discard newest (FALSE) or oldest (TRUE)
                  frames. When AMR vocoder is used the field is also used to 
		  set the bit flags to IVOC_AMR_SCRMODE and IVOC_AMR_DTXMODE
		  to indicate SCR on and DTX on for AMR Vocoder.

   * The watermark value is used in many ways.  As its name implies, playout of
     frames will not start until enough frames to meet the watermark have been
     queued for playout.  For example, if the watermark value is 30, playout of
     frames will not start until VocOutWrite() has been called 30 times.

     The watermark value is also used to determine the size of the encode and
     decode queues.  An attempt to queue another frame will fail and return a
     BUFFER_FULL error code.  When encoding, a new frame will either overwrite
     the oldest frame or be thrown away.  This is dependent upon the 'overwrite'
     field. The overwrite field is used as a bit mask of flags when AMR vocoder
     is used. In addition to bit 0 used to indicate "frame overwrite", the 
     IVOC_AMR_SCRMODE and IVOC_AMR_DTXMODE can be used as bit 1 and 2 flags
     for specifying SCR and DTX on for AMR vocoder.

     Another definition of the watermark value is a compensation for operating in
     non real time.  Voice quality may be impacted if the watermark value is set
     too low, and media latency grows as watermark value is increased.  Some tuning
     on this parameter will be necessary.
     
  ** txReduction is vocoder specific.  It really means the mode the vocoder is in.
     For instance, the SMV vocoder has six modes available(values of 0 - 5).
     
     For EVRC, tx reduction results in a fraction that is used to limit the
     average number of data the vocoder will produce.  The units are in
     sixteenths and represent the fraction to reduce the data by.  The only
     acceptable values(for the service options currently available) for tx
     reduction are 0,2,4,6, and 8.  So you can really only reduce the data by
     eighths, not sixteenths.   Here is a table that illustrates what the values
     mean:

        0 = 0/16 reduce vocoder tx data by 0, or full rate 14.4 kbps.  (DEFAULT)
        2 = 2/16 reduce vocoder tx data by 1/8, or 7/8 rate 12.2 kbps.
        4 = 4/16 reduce vocoder tx data by 1/4 or 3/4 rate 11.2 kbps.
        6 = 6/16 reduce vocoder tx data by 3/8, or 5/8 rate 9.0 kbps.
        8 = 8/16 reduce vocoder tx data by 1/2, or 1/2 rate 7.2 kbps.


     If using a non-zero value for tx reduction, the DataRateType value for max
     must by set to FULL_RATE and EIGHTH_RATE for min.  Anything else will cause
     IVocoder to return an error.

     The effect of a non-zero tx reduction value is that IVocoder will produce
     less FULL_RATE frames than a non-zero value, producing more and more
     HALF_RATE, QUARTER_RATE, and EIGHTH_RATE frames as the value of tx reduction
     gets higher.  This achieves limiting the average maximum data production by
     the vocoder.

Comments:
    None

See Also:
    DataRateType

=============================================================================

IVocoderInfoType

Description:

   The IVocoderInfoType structure supplies information to the
   application about the selected vocoder mode

Definition:

   typedef struct
   {
      uint16 frameDuration;
      uint16 maxFrameSize;
   } IVocoderInfoType;

Members:

   frameDuration  Indicates the duration in milliseconds of a vocoder frame
   maxFrameSize   Indicates the size in bytes of the largest vocoder frame

Comments:
    None

See Also:
    None

=============================================================================

PFNDATA

Description:
    This data type provides a frame count to the have data or need data callback
function. For the haveDataCB, numFrames is the number of encoded frames that are
queued. For the needDataCB, numFrames is the number of erasures that have been collected
while data was not available.

Definition:
    typedef void (*PFNDATA)(uint16 numFrames, void * usrPtr);

Members
    None

Comments:
Be aware that due to BREW latency these numbers may not reflect current
conditions by the time the application callback is invoked. In particular,
the haveDataCB may wish to continue reading until it receives
AEE_VOC_EBUFFER_EMPTY return value.

See Also:
    None

=============================================================================

DataRateType

Description:
    This enum specifies the various data rates a vocoder frame may have. Frames
rates correspond to frame lengths and are vocoder specific. 

For VOC_IS127, VOC_IS733 and 4GV vocoders the following definition is used.

Definition:
    typedef enum {
    BLANK_RATE,
    EIGHTH_RATE,
    QUARTER_RATE,
    HALF_RATE,
    FULL_RATE,
    ERASURE,
    MAX_RATE
    } DataRateType;

Members:
    BLANK_RATE,   :  Indicates data was blanked
    EIGHTH_RATE,  :  Indicates rate 1:8 data
    QUARTER_RATE, :  Indicates rate 1:4 data
    HALF_RATE,    :  Indicates rate 1:2 data
    FULL_RATE,    :  Indicates rate 1 data
    ERASURE,      :  Indicates erasure frame
    MAX_RATE      :  last enum


For AMR vododer following enum is used.

Definition:
    enum {
    AMR_MODE_0475,        
    AMR_MODE_0515, 
    AMR_MODE_0590,
    AMR_MODE_0670,
    AMR_MODE_0740,
    AMR_MODE_0795,
    AMR_MODE_1020,
    AMR_MODE_1220,
    AMR_MODE_MAX
    };

Members:
    AMR_MODE_0475  :  Indicates data at 4.75 kbit /s
    AMR_MODE_0515  :  Indicates data at 5.15 kbit /s
    AMR_MODE_0590  :  Indicates data at 5.90 kbit /s
    AMR_MODE_0670  :  Indicates data at 6.70 kbit /s
    AMR_MODE_0740  :  Indicates data at 7.40 kbit /s
    AMR_MODE_0795  :  Indicates data at 7.95 kbit /s
    AMR_MODE_1020  :  Indicates data at 10.2 kbit /s
    AMR_MODE_1220  :  Indicates data at 12.2 kbit /s
    AMR_MODE_MAX   :  last enum

For G.711 Vocoder following enum is used.

Definition:
   enum {
   IVOC_G711_MODE_MULAW,  
   IVOC_G711_MODE_ALAW,   
   IVOC_G711_MODE_MAX     
   };

Members:
   IVOC_G711_MODE_MULAW   :  Indicates use G.711 mulaw format
   IVOC_G711_MODE_ALAW    :  Indicates use G.711 alaw format
   IVOC_G711_MODE_MAX     :  lase enum


Comments:
    None

See Also:
    None

=====================================================================
  INTERFACES   DOCUMENTATION
=====================================================================

IVocoder Interface


Description:
The IVocoder interface uses the default sound path to capture and play vocoder frames.
IVocoder uses the same device which is used to perform a voice call, so applications
wishing to use IVocoder must be aware of and integrated with TAPI notifications
so that they properly stop using the vocoder when a voice call might be started.
Carriers should be careful to test this integration thoroughly before allowing applications
to be deployed with IVocoder.

IVocoder has near-realtime requirements for good operation. For example, CDMA vocoder
frames are 20ms in duration. Intensive use of the CPU by the application or other
applications while IVocoder is streaming will have adverse affects on voice quality,
but will not cause problems for the handset.

IVocoder is a protected class. Dynamic apps can specify the dependency of IVocoder
in the MIF.
===H2>
Usage
===/H2>

1.   Make sure the mic/earpiece are turned on.

2.   Watermark value must be positive, and will function very poorly if set to low.
It is recommended to start with a value of at least 20. In addition, playout of
frames will not start until enough frames are queued for playout to match the watermark.

3.   Min rate should be at least EIGHTH_RATE, and max rate should be at least HALF_RATE.
BLANK_RATE is not an option for min rate as it blanks out every frame. QUARTER_RATE
should be avoided for max rate as EVRC does not produce quarter rate frames.

4.   Set tx reduction value to zero. Here is an expanded blurb of what it does:

The value for tx reduction results in a fraction that is used to limit the average
number of data the vocoder will produce. The units are in sixteenths and represent
the fraction to reduce the data by. The only acceptable values(for the service options
currently available) for tx reduction are 0,2,4,6, and 8. So you can really only
reduce the data by eighths, not sixteenths. Here is a table that illustrates what
the values mean:
===pre>
0 = 0/16 reduce vocoder tx data by 0, or full rate 14.4 kbps. (DEFAULT)

2 = 2/16 reduce vocoder tx data by 1/8, or 7/8 rate 12.2 kbps.

4 = 4/16 reduce vocoder tx data by 1/4 or 3/4 rate 11.2 kbps.

6 = 6/16 reduce vocoder tx data by 3/8, or 5/8 rate 9.0 kbps.

8 = 8/16 reduce vocoder tx data by 1/2, or 1/2 rate 7.2 kbps.
===/pre>

If using a non-zero value for tx reduction, the DataRateType value for max must
by set to FULL_RATE and EIGHTH_RATE for min. Anything else will cause IVocoder to
return an error.

The effect of a non-zero tx reduction value is that IVocoder will produce less FULL_RATE
frames than a non-zero value, producing more and more HALF_RATE, QUARTER_RATE, and
EIGHTH_RATE frames as the value of tx reduction gets higher. This achieves limiting
the average maximum data production by the vocoder.

The following header file is required:

AEEVocoder.h

=============================================================================

IVOCODER_AddRef()

Description:
    This function is inherited from IBASE_AddRef(). 

See Also:
    IVOCODER_Release()

=============================================================================

IVOCODER_Release()

Description:
    This function is inherited from IBASE_Release(). 

See Also:
    IVOCODER_AddRef()

=============================================================================

IVOCODER_VocConfigure()

Description:
   This function configures the vocoder object to use a specific vocoder. Attempting to re-configure an already configured
   vocoder may first result in the previous configuration to be shutdown before attempting the new
   configuration. 
   To shutdown the vocoder, invoke this function with configuration set to VOC_NONE (i.e. with config.config set to VOC_NONE)

Prototype:
   int16 IVOCODER_VocConfigure(IVocoder* pVoc,
                               IVocoderConfigType config,
                               IVocoderInfoType* pInfo);

Parameters:
   pVoc:     Pointer to an IVocoder object
   config:   Configuration parameters for the vocoder object
   pInfo :   Configuration information about the selected vocoder

Return Value:
   SUCCESS                    :Operation succeeded
   EBADPARM                   :zero watermark value. Must be positive
   ENOMEMORY                  :Not able to allocate memory for vocoder data buffers
   AEE_VOC_ENOT_AVAIL         :Specified vocoder is not available
   AEE_VOC_ERATE_OUT_OF_RANGE :Specified min or max rate is not supported by the
                              vocoder
   AEE_VOC_EBAD_TX_REDUCTION  :Inappropriate tx reduction value
   AEE_VOC_IN_USE             :Vocoder is currently being used by a different instance of IVocoder. 
   ENOMEMORY:                 :Insufficient memory in the system. The previous configuration of the vocoder will be 
                              shutdown.
   EFAILED:                   :Configuration Failed.The previous configuration of the vocoder will be 
                              shutdown.

Comments:
   Specifying vocoder type to VOC_NONE will shut down use of vocoder device.

Side Effects:
   Powers up the vocoder device.
   When switching vocoders, the vocoder must be powered down before it can be
   powered up with a new vocoder.  This will require configuring with VOC_NONE,
   before configuring with the new vocoder.

Version:
   Introduced BREW Client 2.0

See Also:
   IVocoderConfigType

=============================================================================

IVOCODER_VocInStart()

Description:
   This function starts encoding voice frames

Prototype:
   int16 IVOCODER_VocInStart(IVocoder * pVoc);

Parameters:
   pVoc:     Pointer to an IVocoder object

Return Value:
   SUCCESS                    :Operation succeeded
   EBADCLASS                  :NULL IVocoder ptr
   AEE_VOC_EIN_USE            :IVocoder in use by other application
   AEE_VOC_ENOT_CONFIGED      :IVocoder not configured

Comments:
   Have data callback will be invoked when frames are available

Side Effects:
   Begins accepting and queueing encoded frames from the vocoder

Version:
   Introduced BREW Client 2.0

See Also:
   haveDataCB

=============================================================================

IVOCODER_VocInStop()

Description:
   This function stops encoding voice frames

Prototype:
   int16 IVOCODER_VocInStop(IVocoder * pVoc);

Parameters:
   pVoc:     Pointer to an IVocoder object

Return Value:
   SUCCESS                    :Operation succeeded
   EBADCLASS                  :NULL IVocoder ptr
   AEE_VOC_EIN_USE            :IVocoder in use by other application
   AEE_VOC_ENOT_CONFIGED      :IVocoder not configured

Comments:
   Does not clear the queue of encoded frames

Side Effects:
   Ceases accepting and queueing encoded frames from the vocoder

Version:
   Introduced BREW Client 2.0

See Also:
   haveDataCB

=============================================================================

IVOCODER_VocInReset()

Description:
   This function clears the queue of encoded vocoder frames

Prototype:
   int16 IVOCODER_VocInReset(IVocoder * pVoc);

Parameters:
   pVoc:     Pointer to an IVocoder object

Return Value:
   SUCCESS                    :Operation succeeded
   EBADCLASS                  :NULL IVocoder ptr
   AEE_VOC_EIN_USE            :IVocoder in use by other application
   AEE_VOC_ENOT_CONFIGED      :IVocoder not configured

Comments:
   This operation will resume prior operation

Side Effects:
    Ceases accepting and queueing encoded frames from the vocoder and frees queued
frames

Version:
   Introduced BREW Client 2.0

See Also:
   IVOCODER_VocInStop()

=============================================================================

IVOCODER_VocInRead()

Description:
   This functions obtains the oldest encoded voice frame from the queue

Prototype:
   int16 IVOCODER_VocInRead(IVocoder* pVoc,
                            DataRateType* pRate,
                            uint16* pLength,
                            uint8* pData);

Parameters:
   pVoc:     Pointer to an IVocoder object
   pRate:    Pointer to storage to accept data rate of the frame

   pLength:  Pointer to storage to accept size in bytes of the frame. This parameter must be non-NULL. On input, *pLength
   must specify the size of the buffer pData. On return, *pLength will contain the actual size of the data filled in the buffer
   pData by this function.
   If the size of pData is lesser than the actual size of the frame, *pLength will be set to the actual size of the frame on return
   but there will be a data loss i.e. pData will be filled only with as much space as the size of pData 
   (i.e value of *pLength on input).

   pData:    Pointer to storage to accept the encoded frame bytes

Return Value:
   SUCCESS                    :Operation succeeded
   EBADCLASS                  :NULL IVocoder ptr
   AEE_VOC_EIN_USE            :IVocoder in use by other application
   AEE_VOC_ENOT_CONFIGED      :IVocoder not configured
   AEE_VOC_EBUFFER_EMPTY      :No encoded frame is queued

Comments:
   The buffer must be large enough to support the maximum frame rate for
   the vocoder, not merely the maximum used by the current rate limits

Side Effects:
   None

Version:
   Introduced BREW Client 2.0

See Also:
   haveDataCB

=============================================================================

IVOCODER_VocOutStart()

Description:
   This function starts the decoding and playout of queued frames

Prototype:
   int16 IVOCODER_VocOutStart(IVocoder * pVoc);

Parameters:
   pVoc:     Pointer to an IVocoder object

Return Value:
   SUCCESS                    :Operation succeeded
   EBADCLASS                  :NULL IVocoder ptr
   AEE_VOC_EIN_USE            :IVocoder in use by other application
   AEE_VOC_ENOT_CONFIGED      :IVocoder not configured

Comments:
   The vocoder will not be started until the watermark of queued
   frames for decoding has been reached.  It may be necessary to append enough
   ERASURE frames to the queue in order to meet the watermark and start playout

Side Effects:
   None

Version:
   Introduced BREW Client 2.0

See Also:
   IVOCODER_VocOutWrite()
   needDataCB

=============================================================================

IVOCODER_VocOutStop()

Description:
   This function stops the decoding and playout of queued frames

Prototype:
   int16 IVOCODER_VocOutStop(IVocoder * pVoc);

Parameters:
   pVoc:     Pointer to an IVocoder object

Return Value:
   SUCCESS                    Operation succeeded
   EBADCLASS                  NULL IVocoder ptr
   AEE_VOC_EIN_USE            IVocoder in use by other application
   AEE_VOC_ENOT_CONFIGED      IVocoder not configured

Comments:
   Does not clear the already queued frames

Side Effects:
   None

Version:
   Introduced BREW Client 2.0

See Also:
   None

=============================================================================

IVOCODER_VocOutReset()

Description:
   This function clears the queue of frames available for decoding

Prototype:
   int16 IVOCODER_VocOutReset(IVocoder * pVoc);

Parameters:
   pVoc:     Pointer to an IVocoder object

Return Value:
   SUCCESS                    :Operation succeeded
   EBADCLASS                  :NULL IVocoder ptr
   AEE_VOC_EIN_USE            :IVocoder in use by other application
   AEE_VOC_ENOT_CONFIGED      :IVocoder not configured

Comments:
   If the vocoder was previously playing, it will wait for frames until the
   watermark is reached again and then will resume playout

Side Effects:
   frees queued frames

Version:
   Introduced BREW Client 2.0

See Also:
   needDataCB

=============================================================================

IVOCODER_VocOutWrite()

Description:
   This function provides a new frame of data for the decoder

Prototype:
   int16 IVOCODER_VocOutWrite(IVocoder* pVoc,
                              DataRateType rate,
                              uint16 length,
                              const uint8* pData);

Parameters:
   pVoc:     Pointer to an IVocoder object
   rate:     Data rate of the frame
   length:   Size in bytes of the frame
   pData:    Pointer to the frame data bytes

Return Value:
   SUCCESS                    :Operation succeeded
   EBADCLASS                  :NULL IVocoder ptr
   EBADPARM                   :Frame length is not appropriate for the specified
                              rate
   AEE_VOC_EIN_USE            :IVocoder in use by other application
   AEE_VOC_ENOT_CONFIGED      :IVocoder not configured
   AEE_VOC_EBUFFER_FULL       :No more frames can be accepted by the output queue
   AEE_VOC_ENOT_STARTED       :Frame queued for playout when playout op not started

Comments:
   Will accept frames even if playout(IVOCODER_VocOutStart) has not been started

Side Effects:
   May cause playout of frames to start if this frame is the first to satisfy
   the watermark

Version:
   Introduced BREW Client 2.0

See Also:
   IVOCODER_VocOutStart()
   needDataCB

=============================================================================

IVOCODER_GetOutDepth()

Description:
   This function discovers how many frames are queued for playout

Prototype:
   int16 IVOCODER_GetOutDepth(IVocoder* pVoc, uint16* pDepth);

Parameters:
   pVoc:     Pointer to an IVocoder object
   pDepth:   Pointer to storage to accept count of queued frames

Return Value:
   SUCCESS                    :Operation succeeded
   EBADCLASS                  :NULL IVocoder ptr
   EBADPARM                   :Bad input parameter pDepth
   AEE_VOC_EIN_USE            :IVocoder in use by other application
   AEE_VOC_ENOT_CONFIGED      ;IVocoder not configured

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 2.0

See Also:
   None

=============================================================================

IVOCODER_GetInDepth()

Description:
   This function discovers how many encoded frames are queued

Prototype:
   int16 IVOCODER_GetInDepth(IVocoder* pVoc, uint16* pDepth);

Parameters:
   pVoc:     Pointer to an IVocoder object
   pDepth:   Pointer to storage to accept count of queued frames

Return Value:
   SUCCESS                    :Operation succeeded
   EBADCLASS                  :NULL IVocoder ptr
   EBADPARM                   :Bad input parameter pDepth
   AEE_VOC_EIN_USE            :IVocoder in use by other application
   AEE_VOC_ENOT_CONFIGED      :IVocoder not configured

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 2.0

See Also:
   None

=============================================================================

IVOCODER_GetFrameLength()

Description:
   This function provides the size of a specified vocoder frame at a given rate

Prototype:
   int16 IVOCODER_GetFrameLength(IVocoder * pVoc,
                                   VocoderType vocoder,
                                   DataRateType rate,
                                   uint16 * pLength);

Parameters:
   pVoc:     Pointer to an IVocoder object
   vocoder:  Vocoder type of the frame
   rate:     Data rate of the frame
   pLength:  Pointer to storage to accept size of the frame

Return Value:
   size in bytes of a frame for given vocoder/rate, 
   or the value -1 if bad parameters are provided.

Comments:
   Vocoder need not be configured to utilize this method

Side Effects:
   None

Version:
   Introduced BREW Client 2.0

See Also:
   None

=============================================================================

IVOCODER_PlayTone()

Description:
   This function plays a tone through the vocoder

Prototype:
   int16 IVOCODER_PlayTone(IVocoder * pVoc,
                             uint16 tone1,
                             uint16 tone2,
                             int16 volume,
                             uint16 duration);

Parameters:
   pVoc:     Pointer to an IVocoder object
   tone1:    frequency of first tone
   tone2:    frequency of second tone
   volume:   volume of tones
   duration: duration of tones

Return Value:
   SUCCESS                    :Operation succeeded
   EBADCLASS                  :NULL IVocoder ptr
   AEE_VOC_EIN_USE            :IVocoder in use by other application
   AEE_VOC_ENOT_CONFIGED      :IVocoder not configured
   AEE_VOC_EACTIVE            :IVocoder currently playing or acquiring

Comments:
   Can only play tones while vocoder is configured and silent

Side Effects:
   None

Version:
   Introduced BREW Client 2.0

See Also:
   None

=============================================================================
*/

#endif /* AEEVOCODER_H */
