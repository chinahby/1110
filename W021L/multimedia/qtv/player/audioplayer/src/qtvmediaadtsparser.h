#ifndef QTV_MEDIA_ADTS_PARSER_H
#define QTV_MEDIA_ADTS_PARSER_H
/* =======================================================================

                          QtvMediaADTSParser.h

DESCRIPTION

  Definition of the QTV Media ADTS parser  class.

INITIALIZATION AND SEQUENCING REQUIREMENTS

  None.

Copyright 2007 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/player/audioplayer/main/latest/src/qtvmediaadtsparser.h#7 $ 
$DateTime: 2008/05/08 08:09:22 $
$Change: 656074 $ 

========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "QtvCommonStateTable.h"
#include "QtvMediaSliceBuffer.h"

/* 
 * Timestamp discontinuity constants. 
 * Every two adjacent AAC Packets should have a constant delta depending on the 
 * sample frequency. We have a timestamp discontinuity features in the state machine
 * to detect discontinuities in the timestamp space. 
 * Right now the state machine signals the discontinuity whenever the difference is 
 * greater than or equal 1.5 times of delta. This could be finetuned later. 
 */
#define QTV_AUDIO_TS_DELTA_MULT_VALUE   3
#define QTV_AUDIO_TS_DELTA_DIV_VALUE    2

#define QTV_AUDIO_MAX_CHANNELS_SUPPORTED 2
#define QTV_AUDIO_MAX_FREQ_SUPPORTED     48000
/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

// Enumeration for QtvMediaADTSParser events
enum QtvMediaADTSParserEvent
{                              
  GNS_SIG,                 // Get Next Sample Signal
  SYNCD_SIG,                // Sync Obtained Signal 
  VALIDATED_SIG,           // Header Validation successful
  PACKET_SIG,              // Successful Sample formed signal
  UNDERRUN_SIG,            // Media Underrun signal 
  DISCONTINUITY_SIG,       // Discontinuity signal
  MAX_SIG
};

// Enumeration for QtvMediaADTSParser states
enum QtvMediaADTSParserState
{                              
  SYNCING_FIRST,           // Syncing First
  VALIDATING_FIRST,        // Validating First
  FORMING_FIRST,           // Forming First 
  SYNCING_SUBSEQUENT,      // Syncing Subsequent
  VALIDATING_SUBSEQUENT,   // Validating Subsequent
  FORMING_SUBSEQUENT,      // Forming Subsequent 
  MAX_STATE
};

/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                        Macro Definitions
** ======================================================================= */

/* =======================================================================
**                        Class & Function Declarations
** ======================================================================= */
// QtvMediaADTSParser state machine ...
/* ======================================================================
CLASS: QtvMediaADTSParser

DESCRIPTION: ADTS Parser State Machine implementation. 

PARAMETERS:

RETURN VALUE:

=========================================================================*/

// QtvMediaADTSParser state machine
class QtvMediaADTSParser : public QtvCommonStateTable
{              
  public:
  /* Parser Constructor */
  QtvMediaADTSParser() : 
    QtvCommonStateTable(&myTable[0][0], 
                        MAX_STATE, 
                        MAX_SIG)
  {
    init();
    #ifndef FEATURE_QTV_AUDIO_DISCONTINUITY
    m_nSamplingFreq  = QTV_MEDIA_AAC_ADTS_INVALID_SAMPLING_FREQUENCY; 
    m_nSpeakers      = -1; 
    m_bCodecParamChanged = false;
    #endif /* FEATURE_QTV_AUDIO_DISCONTINUITY */
  }

  /* Parser Destructor */
  virtual ~QtvMediaADTSParser()
  {
    /* Empty */
  }

  /* Initialize member function */
  void init()
  {
     m_eState        = SYNCING_FIRST;
	 nextEv          = GNS_SIG;
     m_nTS           = QTV_MEDIA_ADTS_INVALID_TIMESTAMP;
	 m_nOutTS        = QTV_MEDIA_ADTS_INVALID_TIMESTAMP;
	 m_nCurTS        = QTV_MEDIA_ADTS_INVALID_TIMESTAMP;
	 m_nPrevTS       = QTV_MEDIA_ADTS_INVALID_TIMESTAMP;
	 m_pBuf          = NULL;
	 m_nSize         = 0;
	 m_nOutSize      = 0;
	 m_nMaxSize      = 0;
	 m_bOutStatus    = false;  
     m_bPacketPending = false; 
	 m_bWaiting     = false;
	 m_bRealloc     = false;
	 m_nFrameLength = 0;
     #ifdef FEATURE_QTV_AUDIO_DISCONTINUITY
#error code not present
     #endif /* FEATURE_QTV_AUDIO_DISCONTINUITY */
  }       // initial tran.
  /* Transition member functions */
  /* First Sync Transitions */
  unsigned int SyncingFirstHandleGNS();
  unsigned int SyncingFirstHandleSyncd();
  unsigned int SyncingHandleUnderrun();
  unsigned int HandleDiscontinuity();

  /* First Validate Transitions */
  unsigned int ValidatingFirstHandleGNS();
  unsigned int ValidatingFirstHandleValidated();
  unsigned int HandleUnderrun();

  /* First Packet Transitions */
  unsigned int FormingFirstHandleGNS();
  unsigned int FormingFirstHandlePacket();

  /* Subsequent Sync Transitions */
  unsigned int SyncingSubsequentHandleGNS();
  unsigned int SyncingSubsequentHandleSyncd();

  /* Subsequent Validate Transitions */
  unsigned int ValidatingSubsequentHandleGNS();
  unsigned int ValidatingSubsequentHandleValidated();

   /* Subsequent Packet Transitions */
  unsigned int FormingSubsequentHandleGNS();
  unsigned int FormingSubsequentHandlePacket();
  
  void Queue(unsigned char *pBuf,unsigned int nSize,unsigned int nTS);
  unsigned int Parse(unsigned int nMaxSize,bool &bOutStatus,bool &bRealloc);
  unsigned int PeekTimestamp();
  bool CanQueue();

  unsigned int LocateSyncMarker(bool bFirst);
  unsigned int ReturnSample();
  unsigned int ValidateADTSHeader();
  unsigned int FormPacket();
  void PredictTS();
  void InitValidTS();
  void ResetTS();
  bool DetectDiscontinuity();
  bool DetectTSDiscontinuity();
  void Flush();
  
  // Validate Header routine: It performs the sanity check on the ADTS Header currently 
  // getting validated.
  bool ValidateHeader(unsigned int &nFrameLength, 
                               int &nSamplingFreq, 
                               int &nChannelCount, 
                               bool &bUnderrun);

  // Get Sampling frequency based on the Index passed as input.
  unsigned int MapToSamplingFrequency(unsigned int  nFreqIndex) ;

  // Get Sampling Frequency value 
  unsigned int GetSamplingFrequency();

  // Get Sampling Frequency index value 
  unsigned int GetSamplingFreqIndex();

  // Get Number of channels played in the current audio stream 
  unsigned int GetNumberOfChannels();

  // Map Sampling Frequency value to its index
  // Get Sampling frequency based on the Index passed as input.
  unsigned int MapToSamplingFreqIndex(unsigned int  nSamplingFreq) ;

  // Check and return if valid sampling frequency received
  bool IsValidSampleFrequencyRcvd()
  {
    return !(m_nSamplingFreq == QTV_MEDIA_AAC_ADTS_INVALID_SAMPLING_FREQUENCY);
  }
  
  // Returns true if the codec parameter changed and restart necessary
  bool IsCodecParamChanged()
  {
    return m_bCodecParamChanged;
  }
  
  // Return the discontinuity status 
  #ifdef FEATURE_QTV_AUDIO_DISCONTINUITY
#error code not present
  #endif /* FEATURE_QTV_AUDIO_DISCONTINUITY */

  // Set the codec Param changed flag
  void SetCodecParamChanged(bool bVal)
  {
    m_bCodecParamChanged = bVal;
    return;
  }
  
  // Returns true if the number of audio channels or speakers supported by DSP
  bool IsAudioChannelSupported(int nChannels)
  {
    if(nChannels <= QTV_AUDIO_MAX_CHANNELS_SUPPORTED)
    {
      return true;
    }
    return false; 
  }

  // Returns true if the number of audio channels or speakers supported by DSP
  bool IsAudioFreqSupported(int nSF)
  {
    if(nSF <= QTV_AUDIO_MAX_FREQ_SUPPORTED)
    {
      return true;
    }
    return false; 
  }
  
  
  

 
  private:
  static QtvCommonStateTable::Transition const myTable[MAX_STATE][MAX_SIG];
  QtvMediaSliceBuffer m_objSliceBuf;  /* Slice Buffer */
  unsigned int m_nTS;       /* Input Timestamp */
  unsigned int m_nOutTS;    /* Output Timestamp */
  unsigned int m_nCurTS;    /* Current Timestamp */
  unsigned int m_nPrevTS;   /* Previous Timestamp */
  unsigned char *m_pBuf;    /* Input Buffer */
  unsigned int  m_nSize;    /* Input Size   */
  unsigned int  m_nOutSize; /* Output Size */
  unsigned int  m_nMaxSize; /* Maximum Size */
  bool m_bOutStatus;          /* true - OK ; false - UNDERRUN */
  unsigned int m_nSamplingFreq;     /* Current Running Sample Frequency */
  int m_nSpeakers;               /* Channel Count */
  bool m_bPacketPending;           /* Packet pending flag */
  bool m_bWaiting;               /* Packet waiting flag */
  bool m_bRealloc;               /* Realloc flag */
  unsigned int m_nFrameLength;   /* Current frame length */
  bool  m_bCodecParamChanged;    /* Codec Param Changed */
  #ifdef FEATURE_QTV_AUDIO_DISCONTINUITY
#error code not present
  #endif /* FEATURE_QTV_AUDIO_DISCONTINUITY */
};
#endif

