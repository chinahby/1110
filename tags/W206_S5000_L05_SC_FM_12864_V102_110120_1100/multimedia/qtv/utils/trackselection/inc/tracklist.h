#ifndef _TRACKLIST_H_ 
#define _TRACKLIST_H_ 
/* =======================================================================
                              TrackList.h
DESCRIPTION
  This file declares the default implementation of ITrackList. It uses an array of
  Media::MediaInfo to store information about each track. It uses a map
  to store attributes
  .
Copyright 2006 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/trackselection/main/latest/inc/tracklist.h#8 $
$DateTime: 2008/09/29 02:48:55 $
$Change: 752638 $


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
#include "oscl_string.h"
#include "media.h"
#include "qcplayer_misc.h"
#include "IReferenceCountable.h"
#include "ITrackList.h"
#include "ReferenceCountedPointer.h"
#include "DeepMap.h"


/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                          Macro Definitions
** ======================================================================= */
/* =======================================================================
**                        Class Declarations
** ======================================================================= */
class TrackList : public ITrackList
{
  IMPLEMENT_IREFERENCECOUNTABLE_MT(TrackList) 

public:
  
  TrackList();

  // This just copies the pointer value, it does not copy the array!
  // The argument value is set to NULL to prevent the caller from using it 
  // again.
  TrackList(Media::TrackInfo *&trackInfoArray, int32 numTracks);

  TrackList(const TrackList &b);

  virtual ~TrackList();
 
  virtual TrackList &operator = (const TrackList &b);

  virtual int32 GetNumTracksAvailable() const;

  virtual bool Clone(ITrackList **pTrackList /* out */) const;

  virtual uint32 GetTrackID(int32 trackIndex) const;
  virtual Media::CodecType GetCodecType(int32 trackIndex) const;
  virtual int32 GetBitrateBPS(int32 trackIndex) const;
  virtual int32 GetNumAudioChannels(int32 trackIndex) const;
  virtual int32 GetDependsOnID(int32 trackIndex) const;
  virtual uint16 GetContentVersion(int32 trackIndex) const;
  virtual int32 GetFrameWidth(int32 trackIndex) const;
  virtual int32 GetFrameHeight(int32 trackIndex) const;
  virtual bool IsPlayable(int32 trackIndex) const;

  virtual bool IsTrackSelected(int32 trackIndex) const;
  virtual bool SelectTrack(int32 trackIndex, bool select = true);

  virtual void ResetTrackSelection();

  virtual int32 GetNumTracksSelected() const;

  virtual int32 GetNextSelectedAudioTrackIndex(
    int32 prevSelectedAudioTrackIndex = -1) const;

  virtual int32 GetNextSelectedVideoTrackIndex(
    int32 prevSelectedVideoTrackIndex = -1) const;

  virtual int32 GetNextSelectedTextTrackIndex(
    int32 prevSelectedTextTrackIndex = -1) const;

  virtual bool GetAttribute(const char *name, 
                            IReferenceCountable **pValue /* out */);
  virtual void PutAttribute(const char *name, 
                            IReferenceCountable *value /* in */);
  virtual QtvPlayer::EncryptionTypeT GetEncryptionType(int32 trackIndex) const;
  virtual void GetAudioTrackLanguage( char *pLanguage, int32 trackIndex);

#ifdef FEATURE_QTV_WM_SWITCHING_THINNING
  virtual bool AreSameTracksSelected(const ITrackList &b) const;

  virtual bool IsMultiBitRateWMClip() const;
#endif /* FEATURE_QTV_WM_SWITCHING_THINNING */

private:

  static Media::TrackInfo *_CloneTrackInfoArray(
    Media::TrackInfo *trackInfoArray, int32 numTracks);

  void _ComputeCachedData() const;
  void _InvalidateCachedData() const;

  int32 m_numTracks;
  Media::TrackInfo *m_trackInfoArray;

  mutable int32 m_cachedNumTracksSelected;

  DeepMap< OSCL_STRING, 
           ReferenceCountedPointer< IReferenceCountable > > m_attributeMap;
   
};
#endif /* _TRACKLIST_H_ */
