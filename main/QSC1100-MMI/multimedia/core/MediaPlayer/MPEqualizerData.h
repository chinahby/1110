/*=============================================================================
  FILE: MPEqualizerData.h

  SERVICES: defines CMPEQData class
 
  GENERAL DESCRIPTION: provides service for equalizer data


  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPEqualizerData.h#5 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/

#ifndef MPEQUALIZERDATA_H
#define MPEQUALIZERDATA_H

#include "MPDefines.h"

#ifdef FEATURE_MP_EQ
#include "AEESoundCtl.h"

typedef struct {
  AEESoundCtlEqualizerFilter *filter; 
  int32                       band;
  AECHAR*                     name;
} equalizer;

typedef enum {
   MP_EQ_preset_flat = 0,
   MP_EQ_preset_band,
   MP_EQ_preset_dance,
   MP_EQ_preset_fullbass,
   MP_EQ_preset_fullbass_treble,
   MP_EQ_preset_full_treble,
   MP_EQ_preset_laptop,
   MP_EQ_preset_large_hall,
   MP_EQ_preset_live,
   MP_EQ_preset_party,
   MP_EQ_preset_pop,
   MP_EQ_preset_Reggae,
   MP_EQ_preset_Rock,
   MP_EQ_preset_ska,
   MP_EQ_preset_soft,
   MP_EQ_preset_soft_rock,
   MP_EQ_preset_techno,
   MP_EQ_preset_num_presets
} MPEqualizerPreset;

#ifdef __cplusplus
  extern "C" {
#endif

extern AEESoundCtlEqualizerFilter* MPGetEqualizerFilter(uint16 filterNumber);

#ifdef __cplusplus
  }
#endif

#ifdef __cplusplus

class CMPEQData
{
public:
  CMPEQData();
  ~CMPEQData();

  // delete current if there is any and re-initialize filters
  void Init();
  // return Equalizer ptr
  equalizer* GetEqualizer(int16 idx);
  // return current selected equalizer
  equalizer* GetEqualizer();
  // total num of filters
  int16      GetCount();
  // set index for the selected filter
  void       SetIdx(int16 idx);
  // get index for the selected filter
  int16      GetIdx();

  void       Save(equalizer* peq);
private:
  equalizer* m_pFilter;
  int16      m_nCount;
  int16      m_nIdx;
  AECHAR* GetName(const char* pName);
};

#endif //#ifdef __cplusplus

#endif //FEATURE_MP_EQ
#endif // MPEQUALIZERDATA_H
