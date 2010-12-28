/*=============================================================================
  FILE: MPEqualizerData.cpp

  SERVICES: CMPEQData class object Implementation
 
  GENERAL DESCRIPTION:

  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPEqualizerData.cpp#6 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/
#include "MPEqualizerData.h"
#include "AEEStdLib.h"

#ifdef FEATURE_MP_EQ

// flat filter that uses 8 bands 
AEESoundCtlEqualizerFilter flat[] = {
  0,  60,   1, 362, sizeof(AEESoundCtlEqualizerFilter),
  0,  170,  1, 362, sizeof(AEESoundCtlEqualizerFilter),
  0,  310,  3, 362, sizeof(AEESoundCtlEqualizerFilter),
  0,  600,  3, 362, sizeof(AEESoundCtlEqualizerFilter),
  0, 1000,  3, 362, sizeof(AEESoundCtlEqualizerFilter),
  0, 3000,  3, 362, sizeof(AEESoundCtlEqualizerFilter),
  0, 6000,  2, 362, sizeof(AEESoundCtlEqualizerFilter),
  0, 12000, 2, 362, sizeof(AEESoundCtlEqualizerFilter),
};
// club filter that uses 8 bands 
AEESoundCtlEqualizerFilter club[] = {
  0,  60,   1, 362, sizeof(AEESoundCtlEqualizerFilter),
  0,  170,  1, 362, sizeof(AEESoundCtlEqualizerFilter),
  3,  310,  3, 362, sizeof(AEESoundCtlEqualizerFilter),
  4,  600,  3, 362, sizeof(AEESoundCtlEqualizerFilter),
  4, 1000,  3, 362, sizeof(AEESoundCtlEqualizerFilter),
  4, 3000,  3, 362, sizeof(AEESoundCtlEqualizerFilter),
  3, 6000,  2, 362, sizeof(AEESoundCtlEqualizerFilter),
  0, 12000, 2, 362, sizeof(AEESoundCtlEqualizerFilter),
};


// dance filter that uses 8 bands 
AEESoundCtlEqualizerFilter dance[] = {
   6,    60, 1, 362, sizeof(AEESoundCtlEqualizerFilter),
   4,   170, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   1,   310, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   0,   600, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   0,  1000, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
  -3,  3000, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
  -4,  6000, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
  -4, 12000, 2, 362, sizeof(AEESoundCtlEqualizerFilter),
};

// boosts the base and cuts the treble 
AEESoundCtlEqualizerFilter fullbass[] = {
   6,    60, 1, 362, sizeof(AEESoundCtlEqualizerFilter),
   6,   170, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   6,   310, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   3,   600, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   0,  1000, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
  -2,  3000, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
  -3,  6000, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
  -6, 12000, 2, 362, sizeof(AEESoundCtlEqualizerFilter),
};

// boosts the base and cuts the treble 
AEESoundCtlEqualizerFilter fullbass_treble[] = {
   4,    60, 1, 362, sizeof(AEESoundCtlEqualizerFilter),
   3,   170, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   0,   310, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
  -4,   600, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
  -3,  1000, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   1,  3000, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   4,  6000, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   6, 12000, 2, 362, sizeof(AEESoundCtlEqualizerFilter),
};

// boosts the base and cuts the treble 
AEESoundCtlEqualizerFilter full_treble[] = {
  -6,    60, 1, 362, sizeof(AEESoundCtlEqualizerFilter),
  -6 ,   170, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
  -6,   310, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
  -3,   600, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   2,  1000, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   6,  3000, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   9,  6000, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   9, 12000, 2, 362, sizeof(AEESoundCtlEqualizerFilter),
};

// boosts the base and cuts the treble 
AEESoundCtlEqualizerFilter laptop[] = {
   3,    60, 1, 362, sizeof(AEESoundCtlEqualizerFilter),
   6,   170, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   3,   310, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   3,   600, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   0,  1000, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
  -3,  3000, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
  -3,  6000, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
  -3, 12000, 2, 362, sizeof(AEESoundCtlEqualizerFilter),
};

// boosts the base and cuts the treble 
AEESoundCtlEqualizerFilter large_hall[] = {
   6,    60, 1, 362, sizeof(AEESoundCtlEqualizerFilter),
   6,   170, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   3,   310, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   3,   600, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   0,  1000, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
  -3,  3000, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
  -3,  6000, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
  -3, 12000, 2, 362, sizeof(AEESoundCtlEqualizerFilter),
};

// boosts the base and cuts the treble 
AEESoundCtlEqualizerFilter live[] = {
  -3,    60, 1, 362, sizeof(AEESoundCtlEqualizerFilter),
   0,   170, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   2,   310, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   2,   600, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   3,  1000, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   3,  3000, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   3,  6000, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   2, 12000, 2, 362, sizeof(AEESoundCtlEqualizerFilter),
};

// boosts the base and cuts the treble 
AEESoundCtlEqualizerFilter party[] = {
   4,    60, 1, 362, sizeof(AEESoundCtlEqualizerFilter),
   4,   170, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   0,   310, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   0,   600, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   0,  1000, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   0,  3000, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   0,  6000, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   4, 12000, 2, 362, sizeof(AEESoundCtlEqualizerFilter),
};

// boosts the base and cuts the treble 
AEESoundCtlEqualizerFilter pop[] = {
   -1,    60, 1, 362, sizeof(AEESoundCtlEqualizerFilter),
   3,   170, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   4,   310, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   5,   600, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   3,  1000, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   0,  3000, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
  -1,  6000, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
  -1, 12000, 2, 362, sizeof(AEESoundCtlEqualizerFilter),
};

// boosts the base and cuts the treble 
AEESoundCtlEqualizerFilter Reggae[] = {
   0,    60, 1, 362, sizeof(AEESoundCtlEqualizerFilter),
   0,   170, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
  -1,   310, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
  -3,   600, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   0,  1000, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   4,  3000, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   4,  6000, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   0, 12000, 2, 362, sizeof(AEESoundCtlEqualizerFilter),
};

// boosts the base and cuts the treble 
AEESoundCtlEqualizerFilter Rock[] = {
   5,    60, 1, 362, sizeof(AEESoundCtlEqualizerFilter),
   3,   170, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
  -3,   310, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
  -5,   600, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
  -2,  1000, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   2,  3000, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   6,  6000, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   6, 12000, 2, 362, sizeof(AEESoundCtlEqualizerFilter),
};

// boosts the base and cuts the treble 
AEESoundCtlEqualizerFilter ska[] = {
  -2,    60, 1, 362, sizeof(AEESoundCtlEqualizerFilter),
  -3,   170, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
  -3,   310, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
  -1,   600, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   3,  1000, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   4,  3000, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   5,  6000, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   5, 12000, 2, 362, sizeof(AEESoundCtlEqualizerFilter),
};

// boosts the base and cuts the treble 
AEESoundCtlEqualizerFilter soft[] = {
   3,    60, 1, 362, sizeof(AEESoundCtlEqualizerFilter),
   1,   170, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
  -1,   310, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
  -2,   600, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
  -1,  1000, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   2,  3000, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   4,  6000, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   6, 12000, 2, 362, sizeof(AEESoundCtlEqualizerFilter),
};

// boosts the base and cuts the treble 
AEESoundCtlEqualizerFilter soft_rock[] = {
   3,    60, 1, 362, sizeof(AEESoundCtlEqualizerFilter),
   3,   170, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   1,   310, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
  -1,   600, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
  -2,  1000, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
  -3,  3000, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
  -2,  6000, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   3, 12000, 2, 362, sizeof(AEESoundCtlEqualizerFilter),
};

// boosts the base and cuts the treble 
AEESoundCtlEqualizerFilter techno[] = {
   5,    60, 1, 362, sizeof(AEESoundCtlEqualizerFilter),
   3,   170, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   0,   310, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
  -3,   600, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
  -3,  1000, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   0,  3000, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   5,  6000, 3, 362, sizeof(AEESoundCtlEqualizerFilter),
   6, 12000, 2, 362, sizeof(AEESoundCtlEqualizerFilter),
};

#ifdef __cplusplus
  extern "C" {
#endif

AEESoundCtlEqualizerFilter* MPGetEqualizerFilter(uint16 filterNumber)
{
  AEESoundCtlEqualizerFilter* retFilter = NULL;
  switch(filterNumber)
  {
  case MP_EQ_preset_flat:
    retFilter = flat;
    break;
  case MP_EQ_preset_band:
    retFilter = club;
    break;
  case MP_EQ_preset_dance:
    retFilter = dance;
    break;
  case MP_EQ_preset_fullbass:
    retFilter = fullbass;
    break;
  case MP_EQ_preset_fullbass_treble:
    retFilter = fullbass_treble;
    break;
  case MP_EQ_preset_full_treble:
    retFilter = full_treble;
    break;
  case MP_EQ_preset_laptop:
    retFilter = laptop;
    break;
  case MP_EQ_preset_large_hall:
    retFilter = large_hall;
    break;
  case MP_EQ_preset_live:
    retFilter = live;
    break;
  case MP_EQ_preset_party:
    retFilter = party;
    break;
  case MP_EQ_preset_pop:
    retFilter = pop;
    break;
  case MP_EQ_preset_Reggae:
    retFilter = Reggae;
    break;
  case MP_EQ_preset_Rock:
    retFilter = Rock;
    break;
  case MP_EQ_preset_ska:
    retFilter = ska;
    break;
  case MP_EQ_preset_soft:
    retFilter = soft;
    break;
  case MP_EQ_preset_soft_rock:
    retFilter = soft_rock;
    break;
  case MP_EQ_preset_techno:
    retFilter = techno;
    break;
  default:
    return NULL;
  }

  return retFilter;
}

#ifdef __cplusplus
  }
#endif


CMPEQData::CMPEQData():m_pFilter(NULL), m_nCount(0),m_nIdx(0)
{
  Init();
}

CMPEQData::~CMPEQData()
{ 
  int i;
  if(m_pFilter)
  {
    for(i=0; i < m_nCount; i++)
    {
      if(m_pFilter[i].name)
        delete m_pFilter[i].name;
    }
    delete[] m_pFilter;
  }
  DBGPRINTF_LOW("MP: end deleting CMPEQData.");
}
void CMPEQData::Init()
{
  if(m_pFilter)
    delete[] m_pFilter;
  m_nCount = 0;
  m_nIdx = 0;
  
  m_nCount = MP_EQ_preset_num_presets;
  m_pFilter = new equalizer[m_nCount];
  if(m_pFilter)
  {
    m_pFilter[MP_EQ_preset_flat].filter = flat;
    m_pFilter[MP_EQ_preset_flat].band   = 8;
    m_pFilter[MP_EQ_preset_flat].name   = GetName("flat");

    m_pFilter[MP_EQ_preset_band].filter = club;
    m_pFilter[MP_EQ_preset_band].band   = 8;
    m_pFilter[MP_EQ_preset_band].name   = GetName("band");

    m_pFilter[MP_EQ_preset_dance].filter = dance;
    m_pFilter[MP_EQ_preset_dance].band   = 8;
    m_pFilter[MP_EQ_preset_dance].name   = GetName("dance");
    
    m_pFilter[MP_EQ_preset_fullbass].filter = fullbass;
    m_pFilter[MP_EQ_preset_fullbass].band   = 8;
    m_pFilter[MP_EQ_preset_fullbass].name   = GetName("fullbass");

//-
    m_pFilter[MP_EQ_preset_fullbass_treble].filter = fullbass_treble;
    m_pFilter[MP_EQ_preset_fullbass_treble].band   = 8;
    m_pFilter[MP_EQ_preset_fullbass_treble].name   = GetName("fullbass_treble");

    m_pFilter[MP_EQ_preset_full_treble].filter = full_treble;
    m_pFilter[MP_EQ_preset_full_treble].band   = 8;
    m_pFilter[MP_EQ_preset_full_treble].name   = GetName("full_treble");

    m_pFilter[MP_EQ_preset_laptop].filter = laptop;
    m_pFilter[MP_EQ_preset_laptop].band   = 8;
    m_pFilter[MP_EQ_preset_laptop].name   = GetName("laptop");
    
    m_pFilter[MP_EQ_preset_large_hall].filter = fullbass;
    m_pFilter[MP_EQ_preset_large_hall].band   = 8;
    m_pFilter[MP_EQ_preset_large_hall].name   = GetName("large_hall");

//-
    m_pFilter[MP_EQ_preset_live].filter = live;
    m_pFilter[MP_EQ_preset_live].band   = 8;
    m_pFilter[MP_EQ_preset_live].name   = GetName("live");

    m_pFilter[MP_EQ_preset_party].filter = party;
    m_pFilter[MP_EQ_preset_party].band   = 8;
    m_pFilter[MP_EQ_preset_party].name   = GetName("party");

    m_pFilter[MP_EQ_preset_pop].filter = pop;
    m_pFilter[MP_EQ_preset_pop].band   = 8;
    m_pFilter[MP_EQ_preset_pop].name   = GetName("pop");
    
    m_pFilter[MP_EQ_preset_Reggae].filter = Reggae;
    m_pFilter[MP_EQ_preset_Reggae].band   = 8;
    m_pFilter[MP_EQ_preset_Reggae].name   = GetName("Reggae");

//-
    m_pFilter[MP_EQ_preset_Rock].filter = Rock;
    m_pFilter[MP_EQ_preset_Rock].band   = 8;
    m_pFilter[MP_EQ_preset_Rock].name   = GetName("Rock");

    m_pFilter[MP_EQ_preset_ska].filter = ska;
    m_pFilter[MP_EQ_preset_ska].band   = 8;
    m_pFilter[MP_EQ_preset_ska].name   = GetName("ska");

    m_pFilter[MP_EQ_preset_soft].filter = soft;
    m_pFilter[MP_EQ_preset_soft].band   = 8;
    m_pFilter[MP_EQ_preset_soft].name   = GetName("soft");
    
    m_pFilter[MP_EQ_preset_soft_rock].filter = soft_rock;
    m_pFilter[MP_EQ_preset_soft_rock].band   = 8;
    m_pFilter[MP_EQ_preset_soft_rock].name   = GetName("soft_rock");

    m_pFilter[MP_EQ_preset_techno].filter = techno;
    m_pFilter[MP_EQ_preset_techno].band   = 8;
    m_pFilter[MP_EQ_preset_techno].name   = GetName("techno");
  }
  else
    m_nCount = 0;
}

AECHAR* CMPEQData::GetName(const char* pName)
{
  AECHAR * pstr;
  pstr = (AECHAR *) new AECHAR[(STRLEN(pName)+1)*sizeof(AECHAR)];
  if(pstr)
    (void)STRTOWSTR(pName, pstr, (STRLEN(pName)+1)*sizeof(AECHAR));
  return pstr;
}
equalizer* CMPEQData::GetEqualizer(int16 idx)
{
  if(idx < m_nCount)
    return &m_pFilter[idx];
  else
    return NULL;
}
equalizer* CMPEQData::GetEqualizer()
{
  if(m_pFilter)
    return &m_pFilter[m_nIdx];
  else
    return NULL;
}

// total num of filters
int16 CMPEQData::GetCount()
{ 
  return m_nCount;
}
// set index for the selected filter
void CMPEQData::SetIdx(int16 idx)
{
  if(m_nIdx < m_nCount)
    m_nIdx = idx;
}
// get index for the selected filter
int16 CMPEQData::GetIdx()
{
  return m_nIdx;
}
void CMPEQData::Save(equalizer* /*peq*/)
{

}
#endif
