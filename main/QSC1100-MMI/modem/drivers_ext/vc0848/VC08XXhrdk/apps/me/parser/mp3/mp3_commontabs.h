#ifndef _MP3V1_COMMONTABS_H_
#define _MP3V1_COMMONTABS_H_

#include "mp3_parser.h"

extern const unsigned char vesionIDTab[MPEG_VERSION_IDS+1];
extern const int samplerateTab[MPEG_VERSION_IDS][SAMPL_FREQS];
extern const short bitrateTab[MPEG_VERSION_IDS][LAYERS][BITRATES-1];
extern const short samplesPerFrameTab[MPEG_VERSION_IDS][LAYERS];
extern const unsigned char abl2[BITRATES];	
extern const unsigned char k_nSlots[MPEG_VERSION_IDS][LAYERS];

#endif//_MP3V1_COMMONTABS_H_