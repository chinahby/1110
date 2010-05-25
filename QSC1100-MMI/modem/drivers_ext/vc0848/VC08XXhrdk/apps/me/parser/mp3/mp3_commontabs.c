/**************************************************************************************
 * Fixed-point MP3 decoder
 * mp3_commontabs.c - platform-independent tables for MP3 decoder (global, read-only)
 **************************************************************************************/

#include "mp3_parser.h"
#include "mp3_commontabs.h"

/*map mpeg ids on table order in following tables
id 1 should not happen, but is mapped on 3 by default*/
const unsigned char vesionIDTab[MPEG_VERSION_IDS+1] = {2,3,1,0};	


/* indexing = [version][samplerate index]
 * sample rate of frame (Hz)
*/
const int samplerateTab[MPEG_VERSION_IDS][SAMPL_FREQS] = 
{
  {44100, 48000, 32000},		/* MPEG-1 */
  {22050, 24000, 16000},		/* MPEG-2 */
  {11025, 12000,  8000},		/* MPEG-2.5 */
};

/* indexing = [version][layer][bitrate index]
 * bitrate (kbps) of frame
 *   - bitrate index == 0 is "free" mode (bitrate determined on the fly by
 *       counting bits between successive sync words)
 */
const short bitrateTab[MPEG_VERSION_IDS][LAYERS][BITRATES-1] = {
	{
		/* MPEG-1 */
		{  0, 32, 64, 96,128,160,192,224,256,288,320,352,384,416,448}, /* Layer 1 */
		{  0, 32, 48, 56, 64, 80, 96,112,128,160,192,224,256,320,384}, /* Layer 2 */
		{  0, 32, 40, 48, 56, 64, 80, 96,112,128,160,192,224,256,320}, /* Layer 3 */
	},
	{
		/* MPEG-2 */
		{  0, 32, 48, 56, 64, 80, 96,112,128,144,160,176,192,224,256}, /* Layer 1 */
		{  0,  8, 16, 24, 32, 40, 48, 56, 64, 80, 96,112,128,144,160}, /* Layer 2 */
		{  0,  8, 16, 24, 32, 40, 48, 56, 64, 80, 96,112,128,144,160}, /* Layer 3 */
	},
	{
		/*
			 MPEG-2.5
			 In fact, for MPEG2.5, no layer 1 or layer 2.(just keep it!) 
		*/

		{  0, 32, 48, 56, 64, 80, 96,112,128,144,160,176,192,224,256}, /* Layer 1 */
		{  0,  8, 16, 24, 32, 40, 48, 56, 64, 80, 96,112,128,144,160}, /* Layer 2 */
		{  0,  8, 16, 24, 32, 40, 48, 56, 64, 80, 96,112,128,144,160}, /* Layer 3 */
	},
};

/* indexing = [version][layer]
 * number of samples in one frame (per channel)
 */
const short samplesPerFrameTab[MPEG_VERSION_IDS][LAYERS] = {
	{384, 1152, 1152 }, /* MPEG1 */
	{384, 1152,  576 }, /* MPEG2 */
	{384, 1152,  576 }, /* MPEG2.5 */
};




/*
table used to check the MPEG1 Layer2 header
unsigned char type used to save memory
*/
const unsigned char abl2[BITRATES] =
{ 0, 7, 7, 7, 0, 7, 0, 0, 0, 0, 0, 8, 8, 8, 8, 8 };


/*
* Multiplier used to compute number of slots in frame (see ISO/IEC 11172-3
* and ISO/IEC 13818-3, section 2.4.3.1).
*/
const unsigned char k_nSlots[MPEG_VERSION_IDS][LAYERS] =
{   
	/*  Layer: I, II,  III  */
	{ 12,	 144,  144 },	/* mpeg 1   ( ID = 11 ) */
	{ 12,	 144,	72 },   /* mpeg 2   ( ID = 10 ) */
    { 0,	   0,	72 }    /* mpeg 2.5 ( ID = 00 ) */
};


