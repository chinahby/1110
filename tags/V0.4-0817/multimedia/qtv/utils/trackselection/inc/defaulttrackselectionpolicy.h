/* =======================================================================
                      DefaultTrackSelectionPolicy.h
DESCRIPTION
  This module contains the declaration of the DefaultTrackSelectionPolicy 
  class.
  
EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright 2006 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/trackselection/main/latest/inc/defaulttrackselectionpolicy.h#7 $
$DateTime: 2008/05/12 23:03:34 $
$Change: 658428 $

========================================================================== */

#ifndef DEFAULT_TRACK_SELECTION_POLICY_H
#define DEFAULT_TRACK_SELECTION_POLICY_H

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "media.h"
#include "QtvPlayer.h"
#include "ITrackList.h"
#include "ITrackSelectionPolicy.h"
#include "qcplayer_misc.h"

class DefaultTrackSelectionPolicy : public ITrackSelectionPolicy 
{
  IMPLEMENT_IREFERENCECOUNTABLE_MT(DefaultTrackSelectionPolicy)

public:

  DefaultTrackSelectionPolicy();
  DefaultTrackSelectionPolicy(streaming_codec_select_criteria selectionCriteria);
  virtual ~DefaultTrackSelectionPolicy();

  virtual void SelectTracks(ITrackList *trackList /* in */, bool reselect);

  virtual bool Notify(int eventType, void *value);

  void SetCodecSelectCriteria(streaming_codec_select_criteria selectionCriteria);

  void BlockH264(bool doBlock);

protected:

  streaming_codec_select_criteria m_selectionCriteria;
  bool m_doBlockH264;
};

#endif /* DEFAULT_TRACK_SELECTION_POLICY_H */
