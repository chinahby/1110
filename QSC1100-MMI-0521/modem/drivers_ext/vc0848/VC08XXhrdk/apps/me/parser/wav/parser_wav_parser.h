
#ifndef _ME_PARSER_WAV_PARSER_
#define _ME_PARSER_WAV_PARSER_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "parser_wav.h"

#define WAVE_FORMAT_PCM       (0x0001)     //PCM 
#define WAVE_FORMAT_DVI_ADPCM (0x0011)     //IMA-ADPCM
	
#define me_memcpy memcpy	//by dnn 2008/5/7 for test

typedef struct
{
	ME_U08 ID[4];/* "RIFF" */
	ME_U32 FileLength;/* length of rest of file = Total length - 8 bytes */
	ME_U08 TYPE[4]; /* "WAVE" */
}WAV_RIFF_WAVE_chunk;

typedef struct
{
	ME_U08 ID[4];/* "fmt " */
	ME_U32 Size;/* length of rest of fmt chunk = 16  or 18--have other data*/
	ME_U16 FormatTag; /* MS PCM = 1 */
	ME_U16 Channels;/* channels, mono = 1, stereo = 2 */
	ME_U32 SampleRate; /* samples per second = 44100 */
	ME_U32 AvgBytesPerSec; /* bytes per second = samp_rate * byte_samp = 176400 */
	ME_U16 BlockAlign;/* block align (bytes per sample) = channels * bits_per_sample / 8 = 4 */
	ME_U16 BitPerSample;/* bits per sample = 16 for MS PCM (format specific) */
	//fengyuhong optional data for ima-adpcm 
	ME_U16 cbSize;  
    ME_U16 nSamplesPerBlock;	// sample number per block
}WAV_format_chunk;

typedef struct
{
	ME_U08 ID[4];/*"fact"*/
	ME_U32 Size;/* length of rest of fact chunk = 4*/
	ME_U32 Data; /* uncompressed sample number */
}WAV_fact_chunk; /*This chunk is optional*/

typedef struct
{
	ME_U08 ID[4];/*data*/
	ME_U32 Size;/* data length (bytes) */
}WAV_Data_chunk;

ME_VOID Parser_RIFF_WAVE_Chunk(ME_PS08 buffer,WAV_RIFF_WAVE_chunk *RIFF_WAVE_chunk);
ME_VOID Parser_FORMAT_Chunk(ME_PS08 buffer,WAV_format_chunk *format_chunk);
ME_VOID Parser_FACT_Chunk(ME_PS08 buffer,WAV_fact_chunk *fact_chunk);
ME_VOID Parser_DATA_Chunk(ME_PS08 buffer,WAV_Data_chunk *data_chunk);
//ME_U08 Parser_wave_head(wav_infor_parser *file,WAV_format_chunk *format_chunk,WAV_Data_chunk *data_chunk);
ME_U08 Parser_wave_head(wav_infor_parser *file,WAV_format_chunk *format_chunk,WAV_Data_chunk *data_chunk,WAV_fact_chunk *fact_chunk);
#ifdef __cplusplus
}
#endif

#endif

