/* =======================================================================
                              QtvMediaADTSBuffer.cpp
DESCRIPTION
  This module contains the implementation for the ADTS Parser State Machine
  class. 

EXTERNALIZED FUNCTIONS
  1. Constructor & Destructor
  2. CanQueue
  3. Queue
  4. Parse


INITIALIZATION AND SEQUENCING REQUIREMENTS
  -NA-
  
Portions copyrighted by QUALCOMM Incorporated; 
Copyright 2007 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/player/audioplayer/main/latest/src/qtvmediaadtsparser.cpp#13 $
$DateTime: 2010/06/01 00:09:10 $
$Change: 1318147 $

========================================================================== */

/*===========================================================================
  Includes
===========================================================================*/

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "comdef.h"
#include "qcplayer.h"
#include "QtvMediaADTSParser.h"
#include "QCUtils.h"

#ifdef FEATURE_QTV_GENERIC_BCAST
// static initialization of a state-table...
QtvCommonStateTable::Transition const QtvMediaADTSParser::myTable[MAX_STATE][MAX_SIG] = 
{
  /* SYNCING_FIRST */
  {
    /* GNS */
    {(QtvCommonStateTable::pMFnAction)(&QtvMediaADTSParser::SyncingFirstHandleGNS), SYNCING_FIRST},  

    /* Sync'd */
    {(QtvCommonStateTable::pMFnAction)(&QtvMediaADTSParser::SyncingFirstHandleSyncd), VALIDATING_FIRST},  

    /* Validated */
    {(QtvCommonStateTable::pMFnAction)NULL, MAX_STATE},

    /* Packet */
    {(QtvCommonStateTable::pMFnAction)NULL, MAX_STATE},

    /* Underrun */
    {(QtvCommonStateTable::pMFnAction)(&QtvMediaADTSParser::SyncingHandleUnderrun), SYNCING_FIRST},  

    /* Discontinuity */
    {(QtvCommonStateTable::pMFnAction)(&QtvMediaADTSParser::HandleDiscontinuity), SYNCING_FIRST},
  },

  /* VALIDATING_FIRST */
  {
    /* GNS */
    {(QtvCommonStateTable::pMFnAction)(&QtvMediaADTSParser::ValidatingFirstHandleGNS), VALIDATING_FIRST},  

    /* Sync'd */
    {(QtvCommonStateTable::pMFnAction)NULL, MAX_STATE},

    /* Validated */
    {(QtvCommonStateTable::pMFnAction)(&QtvMediaADTSParser::ValidatingFirstHandleValidated), FORMING_FIRST},  

    /* Packet */
    {(QtvCommonStateTable::pMFnAction)NULL, MAX_STATE},

    /* Underrun */
    {(QtvCommonStateTable::pMFnAction)(&QtvMediaADTSParser::HandleUnderrun), VALIDATING_FIRST},  

    /* Discontinuity */
    {(QtvCommonStateTable::pMFnAction)(&QtvMediaADTSParser::HandleDiscontinuity), SYNCING_FIRST},
  },

  /* FORMING_FIRST */
  {
    /* GNS */
    {(QtvCommonStateTable::pMFnAction)(&QtvMediaADTSParser::FormingFirstHandleGNS), FORMING_FIRST},  

    /* Sync'd */
    {(QtvCommonStateTable::pMFnAction)NULL, MAX_STATE},

    /* Validated */
    {(QtvCommonStateTable::pMFnAction)NULL, MAX_STATE},

    /* Packet */
    {(QtvCommonStateTable::pMFnAction)(&QtvMediaADTSParser::FormingFirstHandlePacket), SYNCING_SUBSEQUENT},  

    /* Underrun */
    {(QtvCommonStateTable::pMFnAction)(&QtvMediaADTSParser::HandleUnderrun), FORMING_FIRST},  

    /* Discontinuity */
    {(QtvCommonStateTable::pMFnAction)(&QtvMediaADTSParser::HandleDiscontinuity), SYNCING_FIRST},
  },

  /* SYNCING_SUBSEQUENT */
  {
    /* GNS */
    {(QtvCommonStateTable::pMFnAction)(&QtvMediaADTSParser::SyncingSubsequentHandleGNS), SYNCING_SUBSEQUENT},  

    /* Sync'd */
    // There is a difference between Syncd signal handling depending on whether it is 
    // a first or subsequent state. 
    {(QtvCommonStateTable::pMFnAction)(&QtvMediaADTSParser::SyncingSubsequentHandleSyncd), VALIDATING_SUBSEQUENT},  

    /* Validated -- not used */
    {(QtvCommonStateTable::pMFnAction)NULL, MAX_STATE},


    /* Packet -- not used */
    {(QtvCommonStateTable::pMFnAction)NULL, MAX_STATE},

    /* Underrun */
    // All the Sync states have common underrun behavior
    {(QtvCommonStateTable::pMFnAction)(&QtvMediaADTSParser::SyncingHandleUnderrun), SYNCING_FIRST},  

    /* Discontinuity -- not used */
    {(QtvCommonStateTable::pMFnAction)NULL, MAX_STATE},
  },

  /* VALIDATING_SUBSEQUENT */
  {
    /* GNS */
    {(QtvCommonStateTable::pMFnAction)(&QtvMediaADTSParser::ValidatingSubsequentHandleGNS), VALIDATING_SUBSEQUENT},  

    /* Sync'd -- not used */
    {(QtvCommonStateTable::pMFnAction)NULL, MAX_STATE},

    /* Validated */
    {(QtvCommonStateTable::pMFnAction)(&QtvMediaADTSParser::ValidatingSubsequentHandleValidated), FORMING_SUBSEQUENT},  

    /* Packet -- not used */
    {(QtvCommonStateTable::pMFnAction)NULL, MAX_STATE},

    /* Underrun */
    // All the packet/validate  states have common underrun processing. 
    // Also switch to the upper states when underrun happens...
    {(QtvCommonStateTable::pMFnAction)(&QtvMediaADTSParser::HandleUnderrun), VALIDATING_FIRST},  

    /* Discontinuity */
    {(QtvCommonStateTable::pMFnAction)(&QtvMediaADTSParser::HandleDiscontinuity), SYNCING_FIRST},
  },

  /* FORMING_SUBSEQUENT */
  {
    /* GNS */
    // In this transition packets are not fetched from the media source. 
    {(QtvCommonStateTable::pMFnAction)(&QtvMediaADTSParser::FormingSubsequentHandleGNS), FORMING_SUBSEQUENT},  

    /* Sync'd -- not used */
    {(QtvCommonStateTable::pMFnAction)NULL, MAX_STATE},

    /* Validated -- not used */
    {(QtvCommonStateTable::pMFnAction)NULL, MAX_STATE},

    /* Packet */
    // There is a clear difference for packet handling depending on whether 
    // it is first or sub-sequent. 
    {(QtvCommonStateTable::pMFnAction)(&QtvMediaADTSParser::FormingSubsequentHandlePacket), SYNCING_SUBSEQUENT},  

    /* Underrun */
    // All the packet/validate  states have common underrun processing. 
    // Also switch to the upper states when underrun happens...
    {(QtvCommonStateTable::pMFnAction)(&QtvMediaADTSParser::HandleUnderrun), FORMING_FIRST},  

    /* Discontinuity */
    {(QtvCommonStateTable::pMFnAction)(&QtvMediaADTSParser::HandleDiscontinuity), SYNCING_FIRST},
  },

};

/* Transition member functions */
/* First Sync Transitions */
/*===========================================================================
FUNCTION:
  QtvMediaADTSParser::SyncingFirstHandleGNS

DESCRIPTION:
   Handles the GNS signal when syncing for the first sample. 

RETURN VALUE:
   Returns SYNCD, or UNDERRUN or DISCONTINUITY signals. 
          
SIDE EFFECTS:
  None.
===========================================================================*/
unsigned int QtvMediaADTSParser::SyncingFirstHandleGNS()
{
  QTV_MSG_PRIO(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_LOW,
               "ADTS_PARSER_SM: SYNCING_FIRST --GNS_SIG--> SYNCING_FIRST");
  // Input argument indicates that we are working on the first sample
  // Setup the next event member appropriately. 
  return LocateSyncMarker(true);
}

/*===========================================================================
FUNCTION:
  QtvMediaADTSParser::SyncingFirstHandleSyncd

DESCRIPTION:
  Handles the syncd signal at Syncing First state.

RETURN VALUE:
  Returns VALIDATED, DISCONTINUITY, or UNDERRUN
          
SIDE EFFECTS:
  None.
===========================================================================*/
unsigned int QtvMediaADTSParser::SyncingFirstHandleSyncd()
{
  if(m_bPacketPending && (m_nCurTS != QTV_MEDIA_ADTS_INVALID_TIMESTAMP) )
  {
    QTV_MSG_PRIO(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_LOW,
                 "ADTS_PARSER_SM: SYNCING_FIRST (pp=true) --SYNCD_SIG--> VALIDATING_FIRST");
    /* this is a case where sync marker is split across PES boundary, we have to predict Ts */
    PredictTS();
  }
  else
  {
    QTV_MSG_PRIO(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_LOW,
                 "ADTS_PARSER_SM: SYNCING_FIRST (pp=false) --SYNCD_SIG--> VALIDATING_FIRST");
    InitValidTS();
  }
  return ValidateADTSHeader();
}

/*===========================================================================
FUNCTION:
  QtvMediaADTSParser::SyncingHandleUnderrun

DESCRIPTION:
   Handles underrun from any of the syncing states. 

RETURN VALUE:
   MAX_SIG
          
SIDE EFFECTS:
  None.
===========================================================================*/
unsigned int QtvMediaADTSParser::SyncingHandleUnderrun()
{
  unsigned int nRet = MAX_SIG;

  QTV_MSG_PRIO2(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_LOW,
                 "ADTS_PARSER_SM: %d --UNDERRUN_SIG--> %d",m_eState,m_eState);
  m_bOutStatus = true; 
  m_nOutSize = 0;
  // In the syncing state we have either zero bytes or one byte
  // Since when we finally sync we do calculate packet pending flag again. 
  // Hence setting it to false here should not matter. It make sure everything is clean when we hit the 
  // Packet boundary. 
  m_bPacketPending = false;
  return nRet;
}

/*===========================================================================
FUNCTION:
  QtvMediaADTSParser::HandleDiscontinuity

DESCRIPTION:
  Handles Discontinuity for all states except subsequent Syncing

RETURN VALUE:
  MAX_SIG
          
SIDE EFFECTS:
  None.
===========================================================================*/
unsigned int QtvMediaADTSParser::HandleDiscontinuity()
{
  unsigned int nRet = MAX_SIG;
  int nDropped = 0; 

  // Prints the transition 
  QTV_MSG_PRIO1(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_LOW,
                 "ADTS_PARSER_SM: %d --DISCONTINUITY_SIG--> SYNCING_FIRST",m_eState);

  /* Reset Timestamp */
  ResetTS();

  if(m_eState == VALIDATING_FIRST  || m_eState == VALIDATING_SUBSEQUENT)
  {
    // Drops the slice which contains the minimum header offset which is 6
    nDropped = m_objSliceBuf.DropCurrentSlice(QTV_MEDIA_ADTS_MINIMUM_HEADER_SIZE-1);
  }
  else if(m_eState == FORMING_FIRST || m_eState == FORMING_SUBSEQUENT)
  {
    // Drops the slice which contains the last packet offset
    nDropped = m_objSliceBuf.DropCurrentSlice(m_nFrameLength-1);
  }
  else if(m_eState == SYNCING_FIRST)
  {
    // Drops the slice which contains the offset one where the sync 
    // marker ends.
    //nDropped = m_objSliceBuf.DropCurrentSlice(1);
  }
  QTV_MSG_PRIO1(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_LOW,
                 "HandleDiscontinuity: Dropped %d bytes",nDropped);
  // Returns underrun. 
  m_bOutStatus = false;
  m_nOutSize   = 0;
  return nRet;
}

/* First Validate Transitions */
/*===========================================================================
FUNCTION:
  QtvMediaADTSParser::ValidatingFirstHandleGNS

DESCRIPTION:
   Handles GNS Signal in validatingFirst state. 

RETURN VALUE:
  Returns VALIDATED,DISCONTINUITY or UNDERRUN signals. 
          
SIDE EFFECTS:
  None.
===========================================================================*/
unsigned int QtvMediaADTSParser::ValidatingFirstHandleGNS()
{
  QTV_MSG_PRIO(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_LOW,
               "ADTS_PARSER_SM: VALIDATING_FIRST --GNS_SIG--> VALIDATING_FIRST");
  return ValidateADTSHeader();
}

/*===========================================================================
FUNCTION:
  QtvMediaADTSParser::ValidatingFirstHandleValidated

DESCRIPTION:
  Handles the validated signal from Validating First state. 

RETURN VALUE:
  PACKET, UNDERRUN or DISCONTINUITY signals. 
          
SIDE EFFECTS:
  None.
===========================================================================*/
unsigned int QtvMediaADTSParser::ValidatingFirstHandleValidated()
{
  QTV_MSG_PRIO(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_LOW,
               "ADTS_PARSER_SM: VALIDATING_FIRST--VALIDATED--> FORMING_FIRST");
  return FormPacket();
}


/*===========================================================================
FUNCTION:
  QtvMediaADTSParser::HandleUnderrun

DESCRIPTION:
  Handles underrun signal from any of the validating or Forming states. four
  such states exists due to first and sub-sequent variations. 
  
RETURN VALUE:
  MAX_SIG
          
SIDE EFFECTS:
  None.
===========================================================================*/
unsigned int QtvMediaADTSParser::HandleUnderrun()
{
  unsigned int nRet = MAX_SIG;

  QTV_MSG_PRIO2(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_LOW,
                 "ADTS_PARSER_SM: %d --UNDERRUN_SIG--> %d",m_eState,m_eState);
  m_bOutStatus = true; 
  m_nOutSize = 0;
  m_bPacketPending = true; 
  return nRet;
}

/* First Packet Transitions */
/*===========================================================================
FUNCTION:
  QtvMediaADTSParser::FormingFirstHandleGNS

DESCRIPTION:
  Handles GNS signals in forming first state. 

RETURN VALUE:
  Returns PACKET , UNDERRUN or DISCONTINUITY signals. 
          
SIDE EFFECTS:
  None.
===========================================================================*/
unsigned int QtvMediaADTSParser::FormingFirstHandleGNS()
{
  QTV_MSG_PRIO(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_LOW,
               "ADTS_PARSER_SM: FORMING_FIRST --GNS--> FORMING_FIRST");
  return FormPacket();
}
/*===========================================================================
FUNCTION:
  QtvMediaADTSParser::FormingFirstHandlePacket

DESCRIPTION:
  Handles packet signal in Forming first state. 

RETURN VALUE:
  MAX_SIG
          
SIDE EFFECTS:
  None.
===========================================================================*/
unsigned int QtvMediaADTSParser::FormingFirstHandlePacket()
{
  QTV_MSG_PRIO(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_LOW,
               "ADTS_PARSER_SM: FORMING_FIRST --PACKET--> SYNCING_SUBSEQUENT");
  return ReturnSample();
}

/*===========================================================================
FUNCTION:
  QtvMediaADTSParser::SyncingSubsequentHandleGNS

DESCRIPTION:
   Action routine to handle GNS signal while Syncing Subsequent. 
   This is the only complex state processing. It is because sometimes 
   due to Timestamp discontinuity we have to generate two outputs one 
   with underrun and another following OK. Similary if the max size is 
   passed as zero , then also we have to generate a REALLOC followed by 
   OK. After any packet output we will be in subsequent sync state. This 
   fact is heavily used here. 

RETURN VALUE:
  if bWaiting is true then we generate MAX_SIG. Otherwise
  Return SYNCD_SIG or UNDERRUN_SIG 
  In syncing subsequence we should not see any discontinuity. 
          
SIDE EFFECTS:
  None.
===========================================================================*/
unsigned int QtvMediaADTSParser::SyncingSubsequentHandleGNS()
{
  unsigned int nRet = MAX_SIG;
  
  if(m_bWaiting)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_LOW,
                 "ADTS_PARSER_SM: SYNCING_SUBSEQUENT(Waiting=true) --GNS_SIG--> SYNCING_SUBSEQUENT");
    if(m_nMaxSize > 0)
    {
      #ifndef FEATURE_QTV_AUDIO_DISCONTINUITY
      m_bOutStatus = true;
      m_nOutSize   = 0;
      #endif /* FEATURE_QTV_AUDIO_DISCONTINUITY */
      m_bWaiting   = false; 
      m_bRealloc   = false; 
      #ifdef FEATURE_QTV_AUDIO_DISCONTINUITY
#error code not present
      #else /* FEATURE_QTV_AUDIO_DISCONTINUITY */
      if(m_nFrameLength <= m_nMaxSize)
      {
        m_nOutSize = m_nFrameLength;
        if(m_nFrameLength > 0)
	{
          m_objSliceBuf.CopyOrDropTillOffset(m_nFrameLength-1,m_pBuf,true);
	}
      }
      else
      {
        QTV_MSG_PRIO2(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_ERROR,
        "FormingSubsequentHandlePacket: Packet output truncated(%d-->%d)",
        m_nFrameLength,m_nMaxSize);
	m_nOutSize = m_nMaxSize;
        m_objSliceBuf.CopyOrDropTillOffset(m_nMaxSize-1,m_pBuf,true);
        m_objSliceBuf.CopyOrDropTillOffset(m_nFrameLength-m_nMaxSize-1,NULL,false);
      }
      #endif
    }
    else 
    {
      QTV_MSG_PRIO(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_LOW,
                   "Zero Buffer Case: Staying in SyncingSubsequent state");
    }
  }
  else
  {
    QTV_MSG_PRIO(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_LOW,
                 "ADTS_PARSER_SM: SYNCING_SUBSEQUENT(Waiting=false) --GNS_SIG--> SYNCING_SUBSEQUENT");
    nRet = LocateSyncMarker(false);
  }
  return nRet;
  
}
/*===========================================================================
FUNCTION:
  QtvMediaADTSParser::SyncingSubsequentHandleSyncd

DESCRIPTION:
   Handles Syncd signal at SyncingSubsequentHandleSyncd state. 

RETURN VALUE:
   Returns VALIDATED, UNDERRUN or DISCONTINUITY signals. 
          
SIDE EFFECTS:
  None.
===========================================================================*/
unsigned int QtvMediaADTSParser::SyncingSubsequentHandleSyncd()
{
  if(m_bPacketPending)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_LOW,
                 "ADTS_PARSER_SM: SYNCING_SUBSEQUENT(pp=true) --SYNCD--> VALIDATING_SUBSEQUENT");
    InitValidTS();
  }
  else
  {
    QTV_MSG_PRIO(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_LOW,
                 "ADTS_PARSER_SM: SYNCING_SUBSEQUENT(pp=false) --SYNCD--> VALIDATING_SUBSEQUENT");
    PredictTS();
  }
  m_bPacketPending = false; 
  return ValidateADTSHeader();
}

/* Subsequent Validate Transitions */
/*===========================================================================
FUNCTION:
  QtvMediaADTSParser::ValidatingSubsequentHandleGNS

DESCRIPTION:
  Handles GNS signal in Validating Subsequent state. 

RETURN VALUE:
  Returns Validated , Underrun or discontinuity signals. 
          
SIDE EFFECTS:
  None.
===========================================================================*/
unsigned int QtvMediaADTSParser::ValidatingSubsequentHandleGNS()
{
  QTV_MSG_PRIO(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_LOW,
               "ADTS_PARSER_SM: VALIDATING_SUBSEQUENT --GNS_SIG--> VALIDATING_SUBSEQUENT");
  return ValidateADTSHeader();
  
}

/*===========================================================================
FUNCTION:
  QtvMediaADTSParser::ValidatingSubsequentHandleValidated

DESCRIPTION:
  Handles the validated signal from Validating Subsequent state. 

RETURN VALUE:
  PACKET, UNDERRUN or DISCONTINUITY signals. 
          
SIDE EFFECTS:
  None.
===========================================================================*/
unsigned int QtvMediaADTSParser::ValidatingSubsequentHandleValidated()
{
  QTV_MSG_PRIO(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_LOW,
               "ADTS_PARSER_SM: VALIDATING_SUBSEQUENT--VALIDATED--> FORMING_SUBSEQUENT");
  return FormPacket();
}
/* Subsequent Packet Transitions */
/*===========================================================================
FUNCTION:
  QtvMediaADTSParser::FormingSubsequentHandleGNS

DESCRIPTION:
   Handles GNS signal in FormingSubsequent state

RETURN VALUE:
Returns Packet or underrun or discontinuity signals. 
          
SIDE EFFECTS:
  None.
===========================================================================*/
unsigned int QtvMediaADTSParser::FormingSubsequentHandleGNS()
{
  QTV_MSG_PRIO(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_LOW,
               "ADTS_PARSER_SM: FORMING_SUBSEQUENT --GNS_SIG--> FORMING_SUBSEQUENT");
  return FormPacket();
}

/*===========================================================================
FUNCTION:
  QtvMediaADTSParser::FormingSubsequentHandlePacket

DESCRIPTION:
   Handles Packet signal in FormingSubsequent state. 

RETURN VALUE:
  MAX_SIG
          
SIDE EFFECTS:
  None.
===========================================================================*/
unsigned int QtvMediaADTSParser::FormingSubsequentHandlePacket()
{
  QTV_MSG_PRIO(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_LOW,
               "ADTS_PARSER_SM: FORMING_SUBSEQUENT --PACKET_SIG--> SYNCING_SUBSEQUENT");
  return ReturnSample();
}


/*===========================================================================
FUNCTION:
  QtvMediaADTSParser::LocateSyncMarker

DESCRIPTION:
   Locates the sync marker and sets up the signals to SM appropriately. 

RETURN VALUE:
  Return SYNCD_SIG or UNDERRUN_SIG depending on whether sync word matched or not. 
          
SIDE EFFECTS:
  None.
===========================================================================*/
unsigned int QtvMediaADTSParser::LocateSyncMarker(bool bFirst)
{
  unsigned int nRet = MAX_SIG;
  bool bDisc, /* Discontinuous detection flag */
       bRet,  /* Whether Syncd or not */
       bPP;   /* Packet Pending flag  */
  bRet = m_objSliceBuf.FindADTSSyncMarker(bPP,bDisc);
  if(bRet)
  {
    nRet = SYNCD_SIG;
    if(bDisc)
    {
      // Input Timestamp is invalid
      // This can not happen in Subsequent Sync state
      // Because if there is discontinuity it will be 
      // detected as part of the packet signal processing 
      // inside SubsequentFormingHandlePacket()
      if(m_nTS == QTV_MEDIA_ADTS_INVALID_TIMESTAMP && bFirst)
      {
        QTV_MSG_PRIO(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_ERROR,
                     "Resync received with Invalid TS; Raising Discontinuity");
        nRet = DISCONTINUITY_SIG;
      }
    }
    else
    {
      // If the packet pending is received while syncing 
      if(bPP && bFirst)
      {
        QTV_MSG_PRIO(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_ERROR,
             "Syncing received with PP flag set to true");
       m_bPacketPending = true;
      }
      else if(bPP)
      {
        // In the subsequent syncing state this should not happen. 
        QTV_ASSERT(0);
      }
    }
  }
  else
  {
    nRet = UNDERRUN_SIG;
    if(bDisc && bFirst)
    {
      QTV_MSG_PRIO(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_ERROR,
                   "Sync failed and dropped some data; Raising discontinuity");
      nRet = DISCONTINUITY_SIG;
    }
    else if(bDisc)
    {
      QTV_ASSERT(0);
    }
  }

  return nRet;
  
}
/*===========================================================================
FUNCTION:
  QtvMediaADTSParser::ReturnSample

DESCRIPTION:
   Returns the AAC ADTS sample formed. In addition checks for the TS discontinuity. 

RETURN VALUE:
  Returns OK or UNDERRUN QTV media status depending on the , TS discontinuity check.
          
SIDE EFFECTS:
  None.
===========================================================================*/
unsigned int QtvMediaADTSParser::ReturnSample()
{
  unsigned int nRet = MAX_SIG;
  // Return sample
  // If there are no discontinuity copy the output frame to the Audio sample buffer 
  m_nOutTS = m_nCurTS;
  m_bOutStatus = true;
  m_nOutSize   = 0;
  if(DetectTSDiscontinuity())
  {
    m_bWaiting = true; 
    m_bOutStatus = false;
    m_bRealloc   = false; 
  }
  else
  {
    if(m_nMaxSize > 0)
    {
      #ifdef FEATURE_QTV_AUDIO_DISCONTINUITY
#error code not present
      #endif /* FEATURE_QTV_AUDIO_DISCONTINUITY */
      if(m_nFrameLength <= m_nMaxSize)
      {
        m_nOutSize = m_nFrameLength;
        if(m_nFrameLength > 0)
	{
          m_objSliceBuf.CopyOrDropTillOffset(m_nFrameLength-1,m_pBuf,true);
	}
      }
      else
      {
        QTV_MSG_PRIO2(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_ERROR,
          "FormingSubsequentHandlePacket: Packet output truncated(%d-->%d)",
          m_nFrameLength,m_nMaxSize);
        m_nOutSize = m_nMaxSize;
        m_objSliceBuf.CopyOrDropTillOffset(m_nMaxSize-1,m_pBuf,true);
        m_objSliceBuf.CopyOrDropTillOffset(m_nFrameLength-m_nMaxSize-1,NULL,false);
      }
    }
    else
    {
      QTV_MSG_PRIO(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_LOW,
        "FormingSubsequentHandlePacket: Setting Realloc to true");
      m_bOutStatus = false; 
      m_bWaiting   = true; 
      m_bRealloc   = true; 
    }
  }
  return nRet;
}

/*===========================================================================
FUNCTION:
  QtvMediaADTSParser::ValidateADTSHeader

DESCRIPTION:
   Peeks at any valid offset and returns the index and slice values. 

RETURN VALUE:
  true/false depending on the validity of the offset passed.
          
SIDE EFFECTS:
  None.
===========================================================================*/
unsigned int QtvMediaADTSParser::ValidateADTSHeader()
{
  int nRet = MAX_SIG;
  bool bValidated; 
  bool bUnderrun;
  int nSF;
  int nCC;

  bValidated = ValidateHeader(m_nFrameLength,nSF,nCC,bUnderrun);
  if(bValidated)
  {
    nRet = VALIDATED_SIG;
    if(nSF != m_nSamplingFreq)
    {
      QTV_MSG_PRIO2(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_HIGH,
                   "Sampling Frequency switched from %d to %d",
                   m_nSamplingFreq,nSF);
      // Skip the initial change
      if(m_nSamplingFreq != QTV_MEDIA_AAC_ADTS_INVALID_SAMPLING_FREQUENCY)
      {
        if(IsAudioFreqSupported(nSF))
	{
	  // Only if the new frequency supported raise the codec param change
	  m_bCodecParamChanged = true; 
	}
        else
	{
          QTV_MSG_PRIO(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_ERROR,
		       "Received Non-supported frequency; Allowing invalid content!!");
	}
	    
      } 
      #ifdef FEATURE_QTV_AUDIO_DISCONTINUITY
#error code not present
      #endif /* FEATURE_QTV_AUDIO_DISCONTINUITY */
      /* Clear the Previous time stamp in case when the sample frequency switches */
      m_nPrevTS = QTV_MEDIA_ADTS_INVALID_TIMESTAMP;
      m_nSamplingFreq = nSF;
    }
    if(nCC != m_nSpeakers)
    {
      QTV_MSG_PRIO2(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_HIGH,
                    "Number of Speakers switched from %d to %d",
                    m_nSpeakers,nCC);
      // Skip the initial change
      if(m_nSpeakers >= 0)
      {
        if(!IsAudioChannelSupported(nCC))
	{
          QTV_MSG_PRIO(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_ERROR,
		       "Received Non-supported channel; Allowing invalid content!!");
	}	    
	    
      }
      m_nSpeakers = nCC;
    }
  }
  else
  {
    if(bUnderrun)
    {
      nRet = UNDERRUN_SIG;
    }
    else
    {
      nRet = DISCONTINUITY_SIG;
    }
  }
  
  return nRet;
  
}

/*===========================================================================
FUNCTION:
  QtvMediaADTSParser::FormPacket

DESCRIPTION:
   Try to form the packet and sets up UNDERRUN or PACKET or DISCONTINUITY signals for state machine. 

RETURN VALUE:
  Returns state machine signals such as UNDERRUN or PACKET or DISCONTINUITY.
          
SIDE EFFECTS:
  None.
===========================================================================*/
unsigned int QtvMediaADTSParser::FormPacket()
{
  unsigned int nRet=MAX_SIG; 
  
  if(m_objSliceBuf[m_nFrameLength-1] >=0)
  {
    nRet = PACKET_SIG;
    /* Subsequent Packet is formed; hence check for discontinuity */
    if(DetectDiscontinuity())
    {
      nRet = DISCONTINUITY_SIG;
    }
  }
  else
  {
    nRet = UNDERRUN_SIG;
  }
  return nRet;
  
}

/*===========================================================================
FUNCTION:
  QtvMediaADTSParser::PredictTS

DESCRIPTION:
   Predicts the Timestamp based on the previous Timestamp and SF. 

RETURN VALUE:
  None.
          
SIDE EFFECTS:
  None.
===========================================================================*/
void QtvMediaADTSParser::PredictTS()
{
   m_nPrevTS = m_nCurTS;
   if(m_nSamplingFreq != 0)
   {
     m_nCurTS += (QTV_MEDIA_AAC_SAMPLES_PER_FRAME*QTV_MEDIA_NUM_MS_IN_SEC)/m_nSamplingFreq;
   }

   QTV_MSG_PRIO2(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_LOW,
                "Timestamp Predicted (%x-->%x)",
                m_nPrevTS,m_nCurTS);
   return;
}

/*===========================================================================
FUNCTION:
  QtvMediaADTSParser::ResetTS

DESCRIPTION:
   Reset timestamp value after discontinuity.  

RETURN VALUE:
  None.
          
SIDE EFFECTS:
  None.
===========================================================================*/
void QtvMediaADTSParser::ResetTS()
{
   m_nPrevTS = m_nCurTS = m_nOutTS = m_nTS = QTV_MEDIA_ADTS_INVALID_TIMESTAMP;

#ifdef FEATURE_QTV_AUDIO_DISCONTINUITY
#error code not present
#endif //FEATURE_QTV_AUDIO_DISCONTINUITY
   
   QTV_MSG_PRIO(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_LOW,
                "All the Timestamp values are reset to INVALID");
   return;
}
/*===========================================================================
FUNCTION:
  QtvMediaADTSParser::InitValidTS

DESCRIPTION:
   Initializes the current timestamp to the media timestamp if available. 

RETURN VALUE:
  None.
          
SIDE EFFECTS:
  None.
===========================================================================*/
void QtvMediaADTSParser::InitValidTS()
{
   m_nPrevTS = m_nCurTS;
   // Packet pending flag indicates that next possible audio frame constructed 
   // should be initialized with the chunk timestamp. When this routine is called from 
   // SyncingFirstSyncd, it means that following frames that could be syncd -- within the
   // same chunk -- need to be predicted and hence flipping the flag to false. 
   // This also cover the case when the discontinuity occurs and state machine jumps the 
   // syncing-first state. 
   // In other case also when it is called from SyncingSubsequentSyncd state , following frames 
   // -- constructed within the chunk -- need to be predicted.  
   m_bPacketPending = false;
   if(m_nTS == QTV_MEDIA_ADTS_INVALID_TIMESTAMP)
   {
     QTV_MSG_PRIO(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_ERROR,
                  "Continuous stream came with invalid TS even with ADTS header");
     PredictTS();
   }
   else
   {
     m_nCurTS = m_nTS;
   
     QTV_MSG_PRIO1(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_LOW,
                 "Timestamp initialized to %x",m_nCurTS);
     
   }
   return;
}

/*===========================================================================
FUNCTION:
  QtvMediaADTSParser::DetectDiscontinuity

DESCRIPTION:
   Checks for the sync word immediated after a packet is formed. 

RETURN VALUE:
   Returns true if the two additional bytes exist and does nt match.
          
SIDE EFFECTS:
  None.
===========================================================================*/
bool QtvMediaADTSParser::DetectDiscontinuity()
{
   sint31 byte1;

   if((byte1 = m_objSliceBuf[m_nFrameLength]) >= 0)
   {
     if(byte1 != QTV_MEDIA_ADTS_HIGH_SYNC_MARKER_VALUE)
     {
       QTV_MSG_PRIO1(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_LOW,
                  "Discontinuity Detected at TS(%d)!!!",m_nCurTS);
       return true; 
     }
     if((byte1 = m_objSliceBuf[m_nFrameLength+1]) >= 0)
     {
        if((byte1 & QTV_MEDIA_ADTS_LOW_SYNC_MARKER_MASK) != 
               QTV_MEDIA_ADTS_LOW_SYNC_MARKER_VALUE)
        {
          QTV_MSG_PRIO1(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_LOW,
                  "Discontinuity Detected at TS(%d)!!!",m_nCurTS);
          return true; 
        }
     }
 
  
   }
   
   return false; 
}
#ifndef FEATURE_QTV_AUDIO_DISCONTINUITY
/*===========================================================================
FUNCTION:
  QtvMediaADTSParser::DetectTSDiscontinuity

DESCRIPTION:
   Checks for the TS discontinuity. 

RETURN VALUE:
   Returns true if the timestamps are not continuous based on the sampling frequency.
          
SIDE EFFECTS:
  None.
===========================================================================*/
bool QtvMediaADTSParser::DetectTSDiscontinuity()
{
   unsigned int delta;
   unsigned int diffTS; 

   /* If the sampling frequency changes flag the previoius timestamp as invalid */
   /* In this way it will skip the TS Discontinuity check */

   if(m_nPrevTS != QTV_MEDIA_ADTS_INVALID_TIMESTAMP &&
      m_nCurTS  != QTV_MEDIA_ADTS_INVALID_TIMESTAMP && m_nSamplingFreq != 0) 
   {
     delta = (QTV_MEDIA_AAC_SAMPLES_PER_FRAME*QTV_MEDIA_NUM_MS_IN_SEC)/m_nSamplingFreq;
     if(m_nCurTS > m_nPrevTS)
     {
       /* Normal case when the previous timestamp is lesser */
       diffTS = m_nCurTS - m_nPrevTS;
    
     }
     else
     {
       /* wrap around case when previous timestamp is greater */
       diffTS = ~m_nPrevTS + 1 + m_nCurTS;
       QTV_MSG_PRIO3(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_HIGH,
        "Wrap around observed (%d - %d ) = %d",m_nPrevTS,m_nCurTS,diffTS);
     }

     /* If the difference is greater than delta/2 flag the timestamp discontinuity */
     if(diffTS > ((QTV_AUDIO_TS_DELTA_MULT_VALUE *delta)/QTV_AUDIO_TS_DELTA_DIV_VALUE))
     {
       QTV_MSG_PRIO2(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_HIGH,
        "TS discontinuity observed , diffTS(%d), greater than %d",
        diffTS,(delta/2));
       return true;
     }
   }
 
   return false; 
}

#else /* FEATURE_QTV_AUDIO_DISCONTINUITY */
#error code not present
#endif /* FEATURE_QTV_AUDIO_DISCONTINUITY */

/*===========================================================================
FUNCTION:
  QtvMediaADTSParser::Queue

DESCRIPTION:
   Queues the sample into the slice buffer. 

RETURN VALUE:
  None.
          
SIDE EFFECTS:
  None.
===========================================================================*/
void QtvMediaADTSParser::Queue(unsigned char *pBuf, 
                               unsigned int nSize,
                               unsigned int nTS)
{
   bool bRet = false; 
   m_pBuf     = pBuf;
   m_nSize    = nSize; 
   m_nTS      = nTS; 

   if(nSize > 0)
   {
     bRet = m_objSliceBuf.Insert(m_pBuf,m_nTS,m_nSize);
     if(!bRet)
     {
       QTV_MSG_PRIO(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_FATAL,
                    "Insert failed");
       // This should not happen since we have already checked the space
       QTV_ASSERT(0);
       return;
     }
   }
   else
   {
     QTV_MSG_PRIO(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_HIGH,
                  "Queue: Trying to queue zero sample; This should not happpen");
   }
   return;
}


/*===========================================================================
FUNCTION:
  QtvMediaADTSParser::CanQueue

DESCRIPTION:
  Checks if the SM requires a new sample. Also makes sure that the space is 
  available. 
   

RETURN VALUE:
  true/false.
          
SIDE EFFECTS:
  None.
===========================================================================*/
bool QtvMediaADTSParser::CanQueue()
{
   if(m_eState == SYNCING_FIRST ||
      m_eState == VALIDATING_FIRST ||
      m_eState == FORMING_FIRST)
   {
     if(m_objSliceBuf.CanInsert())
     {
       return true;  
     }
     else
     {
       QTV_MSG_PRIO(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_FATAL,
                    "No Space available in the slice buffer ; Major Problem!!");
       // Treat this as discontinuity 
       // Running out of entires is unlikely event. Whenever that happens 
       // due to packet errors, clean up the entire storage
       dispatch(DISCONTINUITY_SIG); 
       while(m_objSliceBuf.DeleteSlice())
       {
         /* Empty Since DeleteSlice returns zero when the slice buffer is empty */
       }
       return false; 
     }
   }
   return false;
}
/*===========================================================================
FUNCTION:
  QtvMediaADTSParser::Parse

DESCRIPTION:
   Attempt to parse the bitstream and generate at the most one ADTS sample. 

RETURN VALUE:
  Size of bytes inside the valid ADTS frame.
          
SIDE EFFECTS:
  None.
===========================================================================*/
unsigned int QtvMediaADTSParser::Parse(unsigned int nMaxSize,bool &bOutStatus,bool &bRealloc)
{
   m_nMaxSize = nMaxSize; 
   nextEv = GNS_SIG;
   while(nextEv != MAX_SIG)
   {
     dispatch(nextEv);
   }
   bOutStatus = m_bOutStatus;
   bRealloc   = m_bRealloc;
   return m_nOutSize;
}
/*===========================================================================
FUNCTION:
  QtvMediaADTSParser::PeekTimestamp

DESCRIPTION:
   Attempt to peek the first timestamp without taking the sample out. This could be done after parse. 

RETURN VALUE:
  Return the current timestamp.
          
SIDE EFFECTS:
  None.
===========================================================================*/
unsigned int QtvMediaADTSParser::PeekTimestamp()
{
   return m_nOutTS;
}


/*===========================================================================
FUNCTION:
  QtvMediaADTSParser::Flush

DESCRIPTION:
   Flush the ADTS Parser State machine. 

RETURN VALUE:
  none.
          
SIDE EFFECTS:
  None.
===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void QtvMediaADTSParser::Flush()
{
   while(m_objSliceBuf.DeleteSlice())
   {
     /* Empty Since DeleteSlice returns zero when the slice buffer is empty */
   }
   // Re-initialize the state machine to the beginning
   init();
   return;
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/*===========================================================================
FUNCTION:
  QtvMediaADTSParser::GetSamplingFreq

DESCRIPTION:
   Fetch the current running sampling frequency from the parser. 

RETURN VALUE:
  none.
          
SIDE EFFECTS:
  None.
===========================================================================*/
unsigned int QtvMediaADTSParser::GetSamplingFrequency()
{

   return m_nSamplingFreq;
}

/*===========================================================================
FUNCTION:
  QtvMediaADTSParser::GetNumberOfChannels

DESCRIPTION:
   Fetch the current running channel/speaker count from the parser. 

RETURN VALUE:
  none.
          
SIDE EFFECTS:
  None.
===========================================================================*/
unsigned int QtvMediaADTSParser::GetNumberOfChannels()
{
   if(m_nSpeakers < 0)
   {
     // Assume by default zero.
     return 0;
   }

   return m_nSpeakers;
}

/*===========================================================================
FUNCTION:
  QtvMediaADTSParser::GetSamplingFreqIndex

DESCRIPTION:
   Fetch the current running sampling frequency Index from the parser. 
   This index is between 0-13 which is used in the ADTS Header. 

RETURN VALUE:
  Returns the sampling frequency index value.
          
SIDE EFFECTS:
  None.
===========================================================================*/
unsigned int QtvMediaADTSParser::GetSamplingFreqIndex()
{

   return MapToSamplingFreqIndex(m_nSamplingFreq);
}





// Helper function to aid in PES Audio Sample Parsing
// ValidateHeader
/*===========================================================================
FUNCTION:
  QtvMediaADTSParser::ValidateHeader
DESCRIPTION:
   Perform sanity checks on the header to make sure that it is well formed. 

RETURN VALUE:
  True - If there are enough bytes in the slice buffer and header content is well formed. 
  false - This could be returned due to two reasons. Either underrun or validation failed. 
          
SIDE EFFECTS:
  None.
===========================================================================*/
bool QtvMediaADTSParser::ValidateHeader(unsigned int  &nFrameLength, 
                                         int  &nSamplingFreq,
                                         int  &nChannelCount,
                                         bool &bUnderrun)
{

  // variables to hold the three bytes (byte2 is MSB and byte0 is LSB) of
  // the 13 bit length field that is split across three bytes
  int lenOffset = QTV_MEDIA_ADTS_MINIMUM_HEADER_SIZE-1;
  sint31 foundVal  = 0x0; 

  // Assume enough bytes are available 
  // Initialize all the output parameters
  bUnderrun     = false; 
  nFrameLength  = 0; 
  nSamplingFreq = 0;
  nChannelCount = 0;

  // Check if complete Header is received!!
  // Otherwise return underrun immediately
  if((foundVal = m_objSliceBuf[QTV_MEDIA_ADTS_PROTECTION_ABSENT_OFFSET]) >= 0)
  {
    foundVal &= QTV_MEDIA_ADTS_PROTECTION_ABSENT_MASK;
    if(!foundVal)
    {
      /* Protection absent is true */
      lenOffset = QTV_MEDIA_ADTS_MAXIMUM_HEADER_SIZE-1;
    }
    bUnderrun = (m_objSliceBuf[lenOffset] < 0);
    if(bUnderrun)
    {
      QTV_MSG_PRIO(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_LOW,
                   "ValidateHeader returning underrun");
      return false; 
    }
  }
  else
  {
    // This case can not happen since buffer has atleast two bytes if we 
    // are in this state. 
    QTV_ASSERT(0); 
    bUnderrun = true; 
    return false; 
  }
 
  int byte2 = 0x0, byte1 = 0x0, byte0 = 0x0;
  // Layer field check 
  foundVal = m_objSliceBuf[QTV_MEDIA_ADTS_LAYER_OFFSET] & QTV_MEDIA_ADTS_LAYER_MASK;
  foundVal >>= QTV_MEDIA_ADTS_LAYER_SHIFT;
  if(foundVal != QTV_MEDIA_ADTS_LAYER_VAL)
  {
    QTV_MSG_PRIO1(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_ERROR,
                 "ValidateHeader: Layer field check failed (%d)",foundVal);
    return false; 
  }

  // Profile ID check 
  foundVal = m_objSliceBuf[QTV_MEDIA_ADTS_PROFILE_ID_OFFSET] & QTV_MEDIA_ADTS_PROFILE_ID_MASK;
  foundVal >>= QTV_MEDIA_ADTS_PROFILE_ID_SHIFT;
  if(foundVal > QTV_MEDIA_ADTS_PROFILE_ID_SCALABLE_SAMPLING_RATE)
  {
    QTV_MSG_PRIO1(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_ERROR,
                 "ValidateHeader: Profile ID field check failed (%d)",foundVal);
    return false; 
  }

  // Sampling Freq Check
  foundVal = m_objSliceBuf[QTV_MEDIA_ADTS_SAMPLING_FREQ_INDEX_OFFSET] & QTV_MEDIA_ADTS_SAMPLING_FREQ_INDEX_MASK;
  foundVal >>= QTV_MEDIA_ADTS_SAMPLING_FREQ_INDEX_SHIFT;
  if(foundVal > QTV_MEDIA_ADTS_SAMPLING_FREQ_ID_8KHZ)
  {
    QTV_MSG_PRIO1(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_ERROR,
                 "ValidateHeader: Sample Frequency field check failed (%d)",foundVal);
    return false; 
  }
  // Convert the Index to its frequency value
  nSamplingFreq = MapToSamplingFrequency((unsigned int)foundVal);


  // Get the  Channel Count
  byte1 = m_objSliceBuf[QTV_MEDIA_ADTS_CHANNEL_COUNT_OFFSET];
  byte0 = m_objSliceBuf[QTV_MEDIA_ADTS_CHANNEL_COUNT_OFFSET+1];
  nChannelCount  = (byte1 & QTV_MEDIA_ADTS_HIGH_CHANNEL_COUNT_MASK) << QTV_MEDIA_ADTS_HIGH_CHANNEL_COUNT_SHIFT;
  nChannelCount |= (byte0 & QTV_MEDIA_ADTS_LOW_CHANNEL_COUNT_MASK)  >> QTV_MEDIA_ADTS_LOW_CHANNEL_COUNT_SHIFT;

  // Get the frame length 
  byte2 = m_objSliceBuf[QTV_MEDIA_ADTS_FRAME_LENGTH_OFFSET];
  byte1 = m_objSliceBuf[QTV_MEDIA_ADTS_FRAME_LENGTH_OFFSET+1];
  byte0 = m_objSliceBuf[QTV_MEDIA_ADTS_FRAME_LENGTH_OFFSET+2];
  nFrameLength  = (byte2 & QTV_MEDIA_ADTS_HIGH_FRAME_LENGTH_MASK) << QTV_MEDIA_ADTS_HIGH_FRAME_LENGTH_SHIFT;
  nFrameLength |= (byte1 & QTV_MEDIA_ADTS_MID_FRAME_LENGTH_MASK)  << QTV_MEDIA_ADTS_MID_FRAME_LENGTH_SHIFT;
  nFrameLength |= (byte0 & QTV_MEDIA_ADTS_LOW_FRAME_LENGTH_MASK)  >> QTV_MEDIA_ADTS_LOW_FRAME_LENGTH_SHIFT;
  if(nFrameLength <= lenOffset)
  {
    QTV_MSG_PRIO1(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_ERROR,
                 "ValidateHeader: AAC Min Frame Length check failed (%d)",nFrameLength);
    return false; 
  }

  QTV_MSG_PRIO3(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_LOW,
               "ValidateHdr Returning success(%d,%d,%d)",
                nFrameLength,nSamplingFreq,nChannelCount);
  return true;
}


// GetADTSSamplingFrequencyFromIndex
/*===========================================================================
FUNCTION:
  QtvMediaADTSParser::GetADTSSamplingFrequencyFromIndex

DESCRIPTION:
   Returns the sample frequency associated with the sample frequency index of 
   ADTS header. Used by Validate header to transform the index to its value. 

RETURN VALUE:
  Returns the sample frequency value in HZ.
          
SIDE EFFECTS:
  None.
===========================================================================*/
unsigned int QtvMediaADTSParser::MapToSamplingFrequency(unsigned int  nFreqIndex) 
{
  unsigned int nSamplingFreq=QTV_MEDIA_ADTS_SAMPLING_FREQ_VALUE_8KHZ;
  /* Transform the sampling frequency index to its value */
  switch(nFreqIndex)
  {
    case QTV_MEDIA_ADTS_SAMPLING_FREQ_ID_8KHZ: 
    {
      nSamplingFreq = QTV_MEDIA_ADTS_SAMPLING_FREQ_VALUE_8KHZ;
      break;
    }
    case QTV_MEDIA_ADTS_SAMPLING_FREQ_ID_11KHZ: 
    {
      nSamplingFreq = QTV_MEDIA_ADTS_SAMPLING_FREQ_VALUE_11KHZ;
      break;
    }
    case QTV_MEDIA_ADTS_SAMPLING_FREQ_ID_12KHZ: 
    {
      nSamplingFreq = QTV_MEDIA_ADTS_SAMPLING_FREQ_VALUE_12KHZ;
      break;
    }
    case QTV_MEDIA_ADTS_SAMPLING_FREQ_ID_16KHZ: 
    {
      nSamplingFreq = QTV_MEDIA_ADTS_SAMPLING_FREQ_VALUE_16KHZ;
      break;
    }
    case QTV_MEDIA_ADTS_SAMPLING_FREQ_ID_22KHZ: 
    {
      nSamplingFreq = QTV_MEDIA_ADTS_SAMPLING_FREQ_VALUE_22KHZ;
      break;
    }
    case QTV_MEDIA_ADTS_SAMPLING_FREQ_ID_24KHZ: 
    {
      nSamplingFreq = QTV_MEDIA_ADTS_SAMPLING_FREQ_VALUE_24KHZ;
      break;
    }
    case QTV_MEDIA_ADTS_SAMPLING_FREQ_ID_32KHZ: 
    {
      nSamplingFreq = QTV_MEDIA_ADTS_SAMPLING_FREQ_VALUE_32KHZ;
      break;
    }
    case QTV_MEDIA_ADTS_SAMPLING_FREQ_ID_44KHZ: 
    {
      nSamplingFreq = QTV_MEDIA_ADTS_SAMPLING_FREQ_VALUE_44KHZ;
      break;
    }
    case QTV_MEDIA_ADTS_SAMPLING_FREQ_ID_48KHZ: 
    {
      nSamplingFreq = QTV_MEDIA_ADTS_SAMPLING_FREQ_VALUE_48KHZ;
      break;
    }
    case QTV_MEDIA_ADTS_SAMPLING_FREQ_ID_64KHZ: 
    {
      nSamplingFreq = QTV_MEDIA_ADTS_SAMPLING_FREQ_VALUE_64KHZ;
      break;
    }
    case QTV_MEDIA_ADTS_SAMPLING_FREQ_ID_88KHZ: 
    {
      nSamplingFreq = QTV_MEDIA_ADTS_SAMPLING_FREQ_VALUE_88KHZ;
      break;
    }
    case QTV_MEDIA_ADTS_SAMPLING_FREQ_ID_96KHZ: 
    {
      nSamplingFreq = QTV_MEDIA_ADTS_SAMPLING_FREQ_VALUE_96KHZ;
      break;
    }
  }
  return nSamplingFreq;
}



// MapToSamplingFreqIndex
/*===========================================================================
FUNCTION:
  QtvMediaADTSParser::MapToSamplingFreqIndex

DESCRIPTION:
   Returns the sample frequency index associated with the sample frequency of 
   ADTS header. 

RETURN VALUE:
  Returns the sample frequency index value between 0x0-0xb.
          
SIDE EFFECTS:
  None.
===========================================================================*/
unsigned int QtvMediaADTSParser::MapToSamplingFreqIndex(unsigned int  nFreq) 
{
  unsigned int nFreqIndex=QTV_MEDIA_ADTS_SAMPLING_FREQ_ID_8KHZ;
  /* Transform the sampling frequency index to its value */
  switch(nFreq)
  {
    case QTV_MEDIA_ADTS_SAMPLING_FREQ_VALUE_8KHZ: 
    {
      nFreqIndex = QTV_MEDIA_ADTS_SAMPLING_FREQ_ID_8KHZ;
      break;
    }
    case QTV_MEDIA_ADTS_SAMPLING_FREQ_VALUE_11KHZ: 
    {
      nFreqIndex = QTV_MEDIA_ADTS_SAMPLING_FREQ_ID_11KHZ;
      break;
    }
    case QTV_MEDIA_ADTS_SAMPLING_FREQ_VALUE_12KHZ: 
    {
      nFreqIndex = QTV_MEDIA_ADTS_SAMPLING_FREQ_ID_12KHZ;
      break;
    }
    case QTV_MEDIA_ADTS_SAMPLING_FREQ_VALUE_16KHZ: 
    {
      nFreqIndex = QTV_MEDIA_ADTS_SAMPLING_FREQ_ID_16KHZ;
      break;
    }
    case QTV_MEDIA_ADTS_SAMPLING_FREQ_VALUE_22KHZ: 
    {
      nFreqIndex = QTV_MEDIA_ADTS_SAMPLING_FREQ_ID_22KHZ;
      break;
    }
    case QTV_MEDIA_ADTS_SAMPLING_FREQ_VALUE_24KHZ: 
    {
      nFreqIndex = QTV_MEDIA_ADTS_SAMPLING_FREQ_ID_24KHZ;
      break;
    }
    case QTV_MEDIA_ADTS_SAMPLING_FREQ_VALUE_32KHZ: 
    {
      nFreqIndex = QTV_MEDIA_ADTS_SAMPLING_FREQ_ID_32KHZ;
      break;
    }
    case QTV_MEDIA_ADTS_SAMPLING_FREQ_VALUE_44KHZ: 
    {
      nFreqIndex = QTV_MEDIA_ADTS_SAMPLING_FREQ_ID_44KHZ;
      break;
    }
    case QTV_MEDIA_ADTS_SAMPLING_FREQ_VALUE_48KHZ: 
    {
      nFreqIndex = QTV_MEDIA_ADTS_SAMPLING_FREQ_ID_48KHZ;
      break;
    }
    case QTV_MEDIA_ADTS_SAMPLING_FREQ_VALUE_64KHZ: 
    {
      nFreqIndex = QTV_MEDIA_ADTS_SAMPLING_FREQ_ID_64KHZ;
      break;
    }
    case QTV_MEDIA_ADTS_SAMPLING_FREQ_VALUE_88KHZ: 
    {
      nFreqIndex = QTV_MEDIA_ADTS_SAMPLING_FREQ_ID_88KHZ;
      break;
    }
    case QTV_MEDIA_ADTS_SAMPLING_FREQ_VALUE_96KHZ: 
    {
      nFreqIndex = QTV_MEDIA_ADTS_SAMPLING_FREQ_ID_96KHZ;
      break;
    }
  }
  return nFreqIndex;
}

#ifdef FEATURE_QTV_AUDIO_DISCONTINUITY
#error code not present
#endif /* FEATURE_QTV_AUDIO_DISCONTINUITY */
#endif
