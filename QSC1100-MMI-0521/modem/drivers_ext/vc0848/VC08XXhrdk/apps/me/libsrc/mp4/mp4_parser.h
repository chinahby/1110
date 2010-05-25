/*=============================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	mp4_parser.h  
* User for : 	mp4 demux mux, 
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2006/10/04
* Last modify : Amanda Deng 2006-xx-xx     
*
* relase history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=============================================================================*/

#ifndef QUICKTIME_H
#define QUICKTIME_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>

#include "mp4_basedef.h"
#include "mp4_private.h"
#include "mp4_user.h"


extern uint32 endduration;

typedef enum _MP4_BIU_STATUS_INDEX
{
	MP4_IDX_ERROR_CODE=0,
	MP4_IDX_AUD_STATUS=1,	
	MP4_IDX_AUD_DMA_COUNT=2,	
	MP4_IDX_VID_STATUS = 4,
	
	MP4_BIU_STATUS_INDEX_RESERVED=0x7FFFFFFF		
}MP4_BIU_STATUS_INDEX;


void *mp4_calloc( unsigned int n,unsigned int size);
void *mp4_malloc( unsigned int size );
void mp4_free( void *memblock );
void mp4_memset(char* buf, uint8 val, uint32 size);
char* mp4_memcopy(char* str_dst, const char* str_src, uint32 len);

uint32 mp4_fileread(void* file, char* buffer, uint32 count);
uint32 mp4_filewrite(void* file, char* buffer, uint32 count);
uint32 mp4_fileseek(void* file, uint32 offset, uint32 mode);
uint32 mp4_filetell(void* file);
void*mp4_fileopen(const char *filename,const char *mode);
uint32 mp4_fileclose(void* file);
uint32 mp4_filedelete(const char *filename);

uint32  mp4_swap_uint32(uint32  value);
uint32  mp4_match_char32(char *input, char *output);
uint32 mp4_current_time(void);
uint32 mp4_log2bin(uint32 value);
uint32 mp4_bitstream_get_bits(unsigned char* bitsstream, uint32 n, uint32 offset);
uint32 mp4_bitstream_set_bits(unsigned char* bitsstream, uint32 value, uint32 n, uint32 offset);
uint32 mp4_bitstream_delete_bits(unsigned char* bitsstream, uint32 *datalength, uint32 n, uint32 offset);

uint32 mp4_test_position(mp4_infor_t *file);
uint32  mp4_get_file_position(mp4_infor_t *file) ;
uint32 mp4_set_file_position(mp4_infor_t *file, uint32  position) ;

/* utilities for reading data types */
#if MP4LIB_VERSION2_FOREHANDLE
uint32 mp4_read_videodata(mp4_infor_t *file, char *data, uint32 size, uint32 reg, uint32 bit);
uint32 mp4_read_audiodata(mp4_infor_t *file, char *data, uint32 size, uint32 reg, uint32 bit);
uint32 mp4_read_videoindx(mp4_infor_t *file, char *data, uint32 size);
uint32 mp4_read_audioindx(mp4_infor_t *file, char *data, uint32 size);
#endif
uint32 mp4_read_data(mp4_infor_t *file, char *data, uint32 size);
uint32 mp4_write_data(mp4_infor_t *file, char *data, uint32 size);

uint32 mp4_read_pascal(mp4_infor_t *file, char *data);
uint32 mp4_write_pascal(mp4_infor_t *file, char *data);

uint32 mp4_read_fixed32(mp4_infor_t *file, float *value);
uint32 mp4_write_fixed32(mp4_infor_t *file, float number);

uint32 mp4_read_fixed16(mp4_infor_t *file, float *value);
uint32 mp4_write_fixed16(mp4_infor_t *file, float number);

uint32 mp4_read_uint64(mp4_infor_t *file, uint64 *value);
uint32 mp4_write_uint64(mp4_infor_t *file, uint64 number);

uint32  mp4_read_uint32(mp4_infor_t *file, uint32 *value);
uint32 mp4_write_uint32(mp4_infor_t *file, uint32  number);

uint32  mp4_read_uint24(mp4_infor_t *file, uint32 *value);
uint32 mp4_write_uint24(mp4_infor_t *file, uint32  number);

uint32 mp4_read_uint16(mp4_infor_t *file, uint32 *value);
uint32 mp4_write_uint16(mp4_infor_t *file, uint32 number);

uint32 mp4_read_char(mp4_infor_t *file, uint32 *value);
uint32 mp4_write_char(mp4_infor_t *file, uint32 x);

uint32 mp4_read_char32(mp4_infor_t *file, char *string);
uint32 mp4_write_char32(mp4_infor_t *file, char *string);
void	   mp4_copy_char32(char *output, char *input);

uint32 mp4_read_descr_length(mp4_infor_t *file, uint32* length);
uint32 mp4_write_descr_length(mp4_infor_t *file, uint32 length, uint32 compact);

/* atom handling routines */
uint32 mp4_atom_is(mp4_atom_t *atom, char *type);
uint32 mp4_atom_skip(mp4_infor_t *file, mp4_atom_t *atom);
uint32 mp4_atom_read_header(mp4_infor_t *file, mp4_atom_t *atom);
uint32 mp4_atom_write_header(mp4_infor_t *file, mp4_atom_t *atom, char *text);
uint32 mp4_atom_write_footer(mp4_infor_t *file, mp4_atom_t *atom);

/* dinf handling routines */
void mp4_dinf_init(mp4_dinf_t *dinf);
void mp4_dinf_init_all(mp4_dinf_t *dinf);
uint32 mp4_read_dinf(mp4_infor_t *file, mp4_dinf_t *dinf, mp4_atom_t *dinf_atom);
uint32 mp4_write_dinf(mp4_infor_t *file, mp4_dinf_t *dinf);
void mp4_dinf_dump(mp4_dinf_t *dinf);
void mp4_dinf_delete(mp4_dinf_t *dinf);

/* dref handling routines */
void mp4_dref_table_init(mp4_dref_table_t *table);
void mp4_dref_init_all(mp4_dref_t *dref);
void mp4_dref_init(mp4_dref_t *dref);

uint32 mp4_read_dref_table(mp4_infor_t *file, mp4_dref_table_t *table);
uint32 mp4_read_dref(mp4_infor_t *file, mp4_dref_t *dref);
uint32 mp4_write_dref_table(mp4_infor_t *file, mp4_dref_table_t *table);
uint32 mp4_write_dref(mp4_infor_t *file, mp4_dref_t *dref);

void mp4_dref_table_dump(mp4_dref_table_t *table);
void mp4_dref_dump(mp4_dref_t *dref);

void mp4_dref_table_delete(mp4_dref_table_t *table);
void mp4_dref_delete(mp4_dref_t *dref);

/* esds / h263 decode config handling routines */
void mp4_esds_init(mp4_esds_t *esds);
void mp4_esds_delete(mp4_esds_t *esds);
void mp4_esds_dump(mp4_esds_t *esds);

uint32 mp4_read_esds(mp4_infor_t *file, mp4_esds_t *esds);
uint32 mp4_write_esds_common(mp4_infor_t *file, mp4_esds_t *esds, uint32 esid, uint8 objectType, uint8 streamType);
uint32 mp4_write_esds_audio(mp4_infor_t *file, mp4_esds_t *esds, uint32 esid);
uint32 mp4_write_esds_video(mp4_infor_t *file, mp4_esds_t *esds, uint32 esid);
uint32 mp4_esds_get_decoder_config(mp4_esds_t* esds, char** ppBuf, uint32* pBufSize);
uint32 mp4_esds_set_decoder_config(mp4_esds_t* esds, char* pBuf, uint32 bufSize);
uint32 mp4_analysize_mp4v_decoderconfig(unsigned char * decoderConfig, uint32 *decoderConfigLen, mp4_mp4v_info* info);
uint32 mp4_analysize_mp4v_streamheader(unsigned char * streamdata, uint32* datalength, mp4_mp4v_info* info);
uint32 mp4_analysize_h263_streamheader(unsigned char * streamdata, uint32* datalength, mp4_h263_info* info);


void mp4_damr_init(mp4_damr_t *damr);
void mp4_damr_delete(mp4_damr_t *damr);
void mp4_damr_dump(mp4_damr_t *damr);
uint32 mp4_read_damr_audio(mp4_infor_t *file, mp4_damr_t *damr);
uint32 mp4_write_damr_audio(mp4_infor_t *file, mp4_damr_t *damr);

void mp4_d263_init(mp4_d263_t *d263);
void mp4_d263_delete(mp4_d263_t *d263);
void mp4_d263_dump(mp4_d263_t *d263);
uint32 mp4_read_d263_video(mp4_infor_t *file, mp4_d263_t *d263);
uint32 mp4_write_d263_video(mp4_infor_t *file, mp4_d263_t *d263);

/* hdlr  handling routines */
void mp4_hdlr_init(mp4_hdlr_t *hdlr);
void mp4_hdlr_init_video(mp4_hdlr_t *hdlr);
void mp4_hdlr_init_audio(mp4_hdlr_t *hdlr);
void mp4_hdlr_init_data(mp4_hdlr_t *hdlr);
uint32 mp4_read_hdlr(mp4_infor_t *file, mp4_hdlr_t *hdlr);
uint32 mp4_write_hdlr(mp4_infor_t *file, mp4_hdlr_t *hdlr);

void mp4_hdlr_delete(mp4_hdlr_t *hdlr);
void mp4_hdlr_dump(mp4_hdlr_t *hdlr);

/* matrix handling routines */
void mp4_matrix_init(mp4_matrix_t *matrix);
void mp4_matrix_delete(mp4_matrix_t *matrix);
void mp4_matrix_dump(mp4_matrix_t *matrix);
uint32 mp4_read_matrix(mp4_infor_t *file, mp4_matrix_t *matrix);
uint32 mp4_write_matrix(mp4_infor_t *file, mp4_matrix_t *matrix);

/* mdat handling routines */
void mp4_mdat_init(mp4_mdat_t *mdat);
void mp4_mdat_delete(mp4_mdat_t *mdat);
uint32 mp4_read_mdat(mp4_infor_t *file, mp4_mdat_t *mdat, mp4_atom_t *parent_atom);
uint32 mp4_write_mdat(mp4_infor_t *file, mp4_mdat_t *mdat);

/* mdhd handling routines */
void mp4_mdhd_init(mp4_mdhd_t *mdhd);
void mp4_mdhd_init_video(mp4_infor_t *file, mp4_mdhd_t *mdhd, uint32  time_scale);
void mp4_mdhd_init_audio(mp4_infor_t *file, mp4_mdhd_t *mdhd, uint32  time_scale);
uint32 mp4_read_mdhd(mp4_infor_t *file, mp4_mdhd_t *mdhd);
uint32 mp4_write_mdhd(mp4_infor_t *file, mp4_mdhd_t *mdhd);
void mp4_mdhd_delete(mp4_mdhd_t *mdhd);
void mp4_mdhd_dump(mp4_mdhd_t *mdhd);

/* mdia handling routines */
void mp4_mdia_init(mp4_mdia_t *mdia);
void mp4_mdia_init_video(mp4_infor_t *file, mp4_mdia_t *mdia, uint32 frame_w, uint32 frame_h, uint32  sample_duration, uint32  time_scale, char *compressor);
void mp4_mdia_init_audio(mp4_infor_t *file, mp4_mdia_t *mdia, uint32 channels, uint32 bits, uint32  sample_rate, uint32  sample_size, uint32  time_scale, uint32  sample_duration, char *compressor);
uint32 mp4_read_mdia(mp4_infor_t *file, mp4_mdia_t *mdia, mp4_atom_t *trak_atom);
uint32 mp4_write_mdia(mp4_infor_t *file, mp4_mdia_t *mdia);
//void mp4_mdia_delete(mp4_mdia_t *mdia);
void mp4_mdia_delete(mp4_infor_t *file,mp4_mdia_t *mdia);
void mp4_mdia_dump(mp4_mdia_t *mdia);

/* minf handling routines */
void mp4_minf_init(mp4_minf_t *minf);
void mp4_minf_init_video(mp4_infor_t *file, mp4_minf_t *minf, uint32 frame_w, uint32 frame_h, uint32  time_scale, uint32  sample_duration, char *compressor);
void mp4_minf_init_audio(mp4_infor_t *file, mp4_minf_t *minf, uint32 channels, uint32 bits, uint32  sample_rate, uint32  sample_size, uint32  time_scale, uint32  sample_duration, char *compressor);
uint32 mp4_read_minf(mp4_infor_t *file, mp4_minf_t *minf, mp4_atom_t *parent_atom);
uint32 mp4_write_minf(mp4_infor_t *file, mp4_minf_t *minf);
//void mp4_minf_delete(mp4_minf_t *minf);
void mp4_minf_delete(mp4_infor_t *file,mp4_minf_t *minf);
void mp4_minf_dump(mp4_minf_t *minf);

/* moov handling routines */
void mp4_moov_init(mp4_moov_t *moov);
uint32 mp4_read_moov(mp4_infor_t *file, mp4_moov_t *moov, mp4_atom_t *parent_atom);
uint32 mp4_write_moov(mp4_infor_t *file, mp4_moov_t *moov);
//void mp4_moov_delete(mp4_moov_t *moov);
void mp4_moov_delete(mp4_infor_t *file,mp4_moov_t *moov);
void mp4_moov_dump(mp4_moov_t *moov);

void mp4_mvhd_init(mp4_mvhd_t *mvhd);
uint32 mp4_read_mvhd(mp4_infor_t *file, mp4_mvhd_t *mvhd);
uint32 mp4_write_mvhd(mp4_infor_t *file, mp4_mvhd_t *mvhd);
void mp4_mvhd_delete(mp4_mvhd_t *mvhd);
void mp4_mvhd_dump(mp4_mvhd_t *mvhd);

/* smhd handling routines */
void mp4_smhd_init(mp4_smhd_t *smhd);
uint32 mp4_read_smhd(mp4_infor_t *file, mp4_smhd_t *smhd);
uint32 mp4_write_smhd(mp4_infor_t *file, mp4_smhd_t *smhd);
void mp4_smhd_delete(mp4_smhd_t *smhd);
void mp4_smhd_dump(mp4_smhd_t *smhd);

/* stbl handling routines */
void mp4_stbl_init(mp4_stbl_t *tkhd);
void mp4_stbl_init_video(mp4_infor_t *file, mp4_stbl_t *stbl, uint32 frame_w, uint32 frame_h, uint32  sample_duration, char *compressor);
void mp4_stbl_init_audio(mp4_infor_t *file, mp4_stbl_t *stbl, uint32 channels, uint32 bits, uint32  sample_rate, uint32  sample_size, uint32  sample_duration, char *compressor);
uint32 mp4_read_stbl(mp4_infor_t *file, mp4_minf_t *minf, mp4_stbl_t *stbl, mp4_atom_t *parent_atom);
uint32 mp4_write_stbl(mp4_infor_t *file, mp4_minf_t *minf, mp4_stbl_t *stbl);
//void mp4_stbl_delete(mp4_stbl_t *stbl);
void mp4_stbl_delete(mp4_infor_t *file, mp4_stbl_t *stbl);
void mp4_stbl_dump(void *minf_ptr, mp4_stbl_t *stbl);

/* stco handling routines */
void mp4_stco_init(mp4_stco_t *stco);
void mp4_stco_init_common(mp4_infor_t *file, mp4_stco_t *stco);
void mp4_stco_init_common_video(mp4_infor_t *file, mp4_stco_t *stco);
void mp4_stco_init_common_audio(mp4_infor_t *file, mp4_stco_t *stco);
uint32 mp4_read_stco_table(mp4_infor_t *file, mp4_stco_t *stco, uint32 restart);
uint32 mp4_read_stco(mp4_infor_t *file, mp4_stco_t *stco);
uint32 mp4_write_stco(mp4_infor_t *file, mp4_stco_t *stco);
uint32 mp4_update_stco(mp4_infor_t *file,mp4_stco_t *stco, uint32  chunk, uint32  offset);
//void mp4_stco_delete(mp4_stco_t *stco);
void mp4_stco_delete(mp4_infor_t *file, mp4_stco_t *stco);
void mp4_stco_dump(mp4_stco_t *stco);
#if MP4LIB_VERSION2_FOREHANDLE 
uint32 mp4_read_video_stco_table(mp4_infor_t *file, mp4_stco_t *stco, uint32 restart);
uint32 mp4_read_audio_stco_table(mp4_infor_t *file, mp4_stco_t *stco, uint32 restart);
#endif

/* stsc handling routines */
void mp4_stsc_init(mp4_stsc_t *stsc);
void mp4_stsc_init_common(mp4_infor_t *file, mp4_stsc_t *stsc);
uint32 mp4_read_stsc_table(mp4_infor_t *file, mp4_stsc_t *stsc, uint32 restart);
uint32 mp4_read_stsc(mp4_infor_t *file, mp4_stsc_t *stsc);
uint32 mp4_write_stsc(mp4_infor_t *file, mp4_stsc_t *stsc);
uint32 mp4_update_stsc(mp4_infor_t *file,mp4_stsc_t *stsc, uint32  chunk, uint32  samples);
//void mp4_stsc_delete(mp4_stsc_t *stsc);
void mp4_stsc_delete(mp4_infor_t *file, mp4_stsc_t *stsc);
void mp4_stsc_dump(mp4_stsc_t *stsc);
#if MP4LIB_VERSION2_FOREHANDLE
uint32 mp4_read_video_stsc_table(mp4_infor_t *file, mp4_stsc_t *stsc, uint32 restart);
uint32 mp4_read_audio_stsc_table(mp4_infor_t *file, mp4_stsc_t *stsc, uint32 restart);
#endif

/*use when init stsd table before mux, and befor read stsd table*/
void mp4_stsd_table_init(mp4_stsd_table_t *table);
uint32 mp4_read_stsd_audio(mp4_infor_t *file, mp4_stsd_table_t *table, mp4_atom_t *parent_atom);
uint32 mp4_read_stsd_video(mp4_infor_t *file, mp4_stsd_table_t *table, mp4_atom_t *parent_atom);
uint32 mp4_read_stsd_table(mp4_infor_t *file, mp4_minf_t *minf, mp4_stsd_table_t *table);
uint32 mp4_write_stsd_audio(mp4_infor_t *file, mp4_stsd_table_t *table);
uint32 mp4_write_stsd_video(mp4_infor_t *file, mp4_stsd_table_t *table);
uint32 mp4_write_stsd_table(mp4_infor_t *file, mp4_minf_t *minf, mp4_stsd_table_t *table);
void mp4_stsd_table_delete(mp4_stsd_table_t *table);
void mp4_stsd_audio_dump(mp4_stsd_table_t *table);
void mp4_stsd_video_dump(mp4_stsd_table_t *table);
void mp4_stsd_table_dump(void *minf_ptr, mp4_stsd_table_t *table);

/* use when add trak atom, init trak */
void mp4_stsd_init(mp4_stsd_t *stsd);
/* use before mux initialize stsd parameter*/
void mp4_stsd_init_table(mp4_stsd_t *stsd);
void mp4_stsd_init_video(mp4_infor_t *file, mp4_stsd_t *stsd, uint32 frame_w, uint32 frame_h, char *compression);
void mp4_stsd_init_audio(mp4_infor_t *file, mp4_stsd_t *stsd, uint32 channels, uint32 bits, uint32  sample_rate, char *compressor);
uint32 mp4_read_stsd(mp4_infor_t *file, mp4_minf_t *minf, mp4_stsd_t *stsd);
uint32 mp4_write_stsd(mp4_infor_t *file, mp4_minf_t *minf, mp4_stsd_t *stsd);
//void mp4_stsd_delete(mp4_stsd_t *stsd);
void mp4_stsd_delete(mp4_infor_t *file, mp4_stsd_t *stsd);
void mp4_stsd_dump(void *minf_ptr, mp4_stsd_t *stsd);

/* stss handling routines */
void mp4_stss_init(mp4_stss_t *stss);
void mp4_stss_init_common(mp4_infor_t *file, mp4_stss_t *stss);
uint32 mp4_read_stss_table(mp4_infor_t *file, mp4_stss_t *stss, uint32 restart);
uint32 mp4_read_stss(mp4_infor_t *file, mp4_stss_t *stss);
uint32 mp4_write_stss(mp4_infor_t *file, mp4_stss_t *stss);
uint32 mp4_update_stss(mp4_infor_t *file,mp4_stss_t *stss, uint32  sample);
//void mp4_stss_delete(mp4_stss_t *stss);
void mp4_stss_delete(mp4_infor_t *file, mp4_stss_t *stss);
void mp4_stss_dump(mp4_stss_t *stss);
#if MP4LIB_VERSION2_FOREHANDLE
uint32 mp4_read_video_stss_table(mp4_infor_t *file, mp4_stss_t *stss, uint32 restart);
uint32 mp4_read_audio_stss_table(mp4_infor_t *file, mp4_stss_t *stss, uint32 restart);
#endif

/* stsz handling routines */
void mp4_stsz_init(mp4_stsz_t *stsz);
void mp4_stsz_init_video(mp4_infor_t *file, mp4_stsz_t *stsz);
void mp4_stsz_init_common(mp4_infor_t *file, mp4_stsz_t *stsz, uint32  sample_size);
void mp4_stsz_init_common_video(mp4_infor_t *file, mp4_stsz_t *stsz, uint32  sample_size);
void mp4_stsz_init_common_audio(mp4_infor_t *file, mp4_stsz_t *stsz, uint32  sample_size);
uint32 mp4_read_stsz_table(mp4_infor_t *file, mp4_stsz_t *stsz, uint32 restart);
uint32 mp4_read_stsz(mp4_infor_t *file, mp4_stsz_t *stsz);
uint32 mp4_write_stsz(mp4_infor_t *file, mp4_stsz_t *stsz);
uint32 mp4_update_stsz(mp4_infor_t *file,mp4_stsz_t *stsz, uint32  sample, uint32  sample_size);
//void mp4_stsz_delete(mp4_stsz_t *stsz);
void mp4_stsz_delete(mp4_infor_t *file, mp4_stsz_t *stsz);
void mp4_stsz_dump(mp4_stsz_t *stsz);
#if MP4LIB_VERSION2_FOREHANDLE
uint32 mp4_read_video_stsz_table(mp4_infor_t *file, mp4_stsz_t *stsz, uint32 restart);
uint32 mp4_read_audio_stsz_table(mp4_infor_t *file, mp4_stsz_t *stsz, uint32 restart);
#endif

/* stts handling routines */
void mp4_stts_init(mp4_stts_t *stts);
void mp4_stts_init_common(mp4_infor_t *file, mp4_stts_t *stts, uint32  sample_duration);
void mp4_stts_init_common_video(mp4_infor_t *file, mp4_stts_t *stts, uint32  sample_duration);
void mp4_stts_init_common_audio(mp4_infor_t *file, mp4_stts_t *stts, uint32  sample_duration);
uint32 mp4_read_stts_table(mp4_infor_t *file, mp4_stts_t *stts, uint32 restart);
uint32 mp4_read_stts(mp4_infor_t *file, mp4_stts_t *stts);
uint32 mp4_write_stts(mp4_infor_t *file, mp4_stts_t *stts);
uint32 mp4_update_stts(mp4_infor_t *file,mp4_stts_t *stts, uint32  sample_duration);
//void mp4_stts_delete(mp4_stts_t *stts);
void mp4_stts_delete(mp4_infor_t *file, mp4_stts_t *stts);
void mp4_stts_dump(mp4_stts_t *stts);
#if MP4LIB_VERSION2_FOREHANDLE
uint32 mp4_read_video_stts_table(mp4_infor_t *file, mp4_stts_t *stts, uint32 restart);
uint32 mp4_read_audio_stts_table(mp4_infor_t *file, mp4_stts_t *stts, uint32 restart);
#endif

void mp4_tkhd_init(mp4_tkhd_t *tkhd);
void mp4_tkhd_init_audio(mp4_infor_t *file, mp4_tkhd_t *tkhd);
void mp4_tkhd_init_video(mp4_infor_t *file, mp4_tkhd_t *tkhd, uint32 frame_w, uint32 frame_h);
uint32 mp4_read_tkhd(mp4_infor_t *file, mp4_tkhd_t *tkhd);
uint32 mp4_write_tkhd(mp4_infor_t *file, mp4_tkhd_t *tkhd);
void mp4_tkhd_delete(mp4_tkhd_t *tkhd);
void mp4_tkhd_dump(mp4_tkhd_t *tkhd);


void mp4_vmhd_init(mp4_vmhd_t *vmhd);
void mp4_vmhd_init_video(mp4_infor_t *file, mp4_vmhd_t *vmhd, uint32 frame_w, uint32 frame_h, uint32  sample_duration);
uint32 mp4_read_vmhd(mp4_infor_t *file, mp4_vmhd_t *vmhd);
uint32 mp4_write_vmhd(mp4_infor_t *file, mp4_vmhd_t *vmhd);
void mp4_vmhd_delete(mp4_vmhd_t *vmhd);
void mp4_vmhd_dump(mp4_vmhd_t *vmhd);

mp4_trak_t* mp4_add_trak(mp4_moov_t *moov);
mp4_trak_t* mp4_add_track(mp4_moov_t *moov);

void mp4_trak_init(mp4_trak_t *trak);
void mp4_trak_init_video(mp4_infor_t *file, mp4_trak_t *trak, uint32 frame_w, uint32 frame_h, uint32  sample_duration, uint32  time_scale, char *compressor);
void mp4_trak_init_audio(mp4_infor_t *file, mp4_trak_t *trak, uint32 channels, uint32 bits, uint32  sample_rate, uint32  sample_size, uint32  time_scale, uint32  sample_duration, char *compressor);

uint32 mp4_read_trak(mp4_infor_t *file, mp4_trak_t *trak, mp4_atom_t *trak_atom);
uint32 mp4_write_trak(mp4_infor_t *file, mp4_trak_t *trak, uint32  moov_time_scale);

//uint32 mp4_delete_trak(mp4_moov_t *moov, mp4_trak_t *trak);
uint32 mp4_delete_trak(mp4_infor_t *file,mp4_moov_t *moov, mp4_trak_t *trak);
void mp4_trak_dump(mp4_trak_t *trak);


/* the total number of samples in the track depending on the access mode */
uint32  mp4_track_samples(mp4_infor_t *file, mp4_trak_t *trak);
uint32  mp4_track_times(mp4_infor_t *file, mp4_trak_t *trak);
void   mp4_trak_duration(mp4_trak_t *trak, uint32  *duration, uint32  *timescale);
#if MP4LIB_VERSION2_FOREHANDLE 
uint32  mp4_video_track_samples(mp4_infor_t *file, mp4_trak_t *trak);
uint32  mp4_video_track_times(mp4_infor_t *file, mp4_trak_t *trak);
uint32  mp4_audio_track_samples(mp4_infor_t *file, mp4_trak_t *trak);
uint32  mp4_audio_track_times(mp4_infor_t *file, mp4_trak_t *trak);
#endif
/* chunks always start on 1 */
/* samples start on 0 */

/* total bytes between the two samples */
uint32  mp4_sample_range_size(mp4_infor_t *file, mp4_trak_t *trak, uint32  chunk_sample, uint32  sample);
#if MP4LIB_VERSION2_FOREHANDLE
uint32  mp4_video_sample_range_size(mp4_infor_t *file, mp4_trak_t *trak, uint32  chunk_sample, uint32  sample);
uint32  mp4_audio_sample_range_size(mp4_infor_t *file, mp4_trak_t *trak, uint32  chunk_sample, uint32  sample);
#endif
/* the byte offset from mdat start of the chunk */
uint32  mp4_chunk_to_offset(mp4_infor_t *file, mp4_trak_t *trak, uint32  chunk);
#if MP4LIB_VERSION2_FOREHANDLE
uint32  mp4_video_chunk_to_offset(mp4_infor_t *file, mp4_trak_t *trak, uint32  chunk);
uint32  mp4_audio_chunk_to_offset(mp4_infor_t *file, mp4_trak_t *trak, uint32  chunk);
#endif
/* converting between mdat offsets to samples */
uint32  mp4_sample_to_offset(mp4_infor_t *file, mp4_trak_t *trak, uint32  sample);
#if MP4LIB_VERSION2_FOREHANDLE
uint32  mp4_video_sample_to_offset(mp4_infor_t *file, mp4_trak_t *trak, uint32  sample);
uint32  mp4_audio_sample_to_offset(mp4_infor_t *file, mp4_trak_t *trak, uint32  sample);
#endif

uint32 mp4_chunk_of_sample(mp4_infor_t *file,uint32  *chunk_sample, uint32  *chunk, mp4_trak_t *trak, uint32  sample);
#if MP4LIB_VERSION2_FOREHANDLE
uint32 mp4_video_chunk_of_sample(mp4_infor_t *file,uint32  *chunk_sample, uint32  *chunk, mp4_trak_t *trak, uint32  sample);
uint32 mp4_audio_chunk_of_sample(mp4_infor_t *file,uint32  *chunk_sample, uint32  *chunk, mp4_trak_t *trak, uint32  sample);
#endif

/* update all the tables after writing a buffer */
/* set sample_size to 0 if no sample size should be set */
uint32 mp4_update_tables(mp4_infor_t *file, 
							mp4_trak_t *trak, 
							uint32  offset, 
							uint32  chunk, 
							uint32  sample, 
							uint32  sample_size,
							uint32  sample_duration,
							uint32  isSyncSample);

uint32  mp4_track_data_end_position(mp4_trak_t *trak);

/* call this first to open the file and create all the objects */
mp4_infor_t* mp4_open(char *filename, uint32 rw_mode);
/* close the file and delete all the objects */
uint32 mp4_close(mp4_infor_t *file);
/* get audio parameter information */
uint32 mp4_audio_channels(mp4_infor_t *file, uint32  *channels);
uint32 mp4_audio_bits(mp4_infor_t *file, uint32  *bits);
uint32 mp4_audio_sample_rate(mp4_infor_t *file, uint32  *samplerate);
uint32 mp4_audio_time_scale(mp4_infor_t *file, uint32  *timescale);
uint32 mp4_audio_sample_duration(mp4_infor_t *file, uint32  sample, uint32  *duration);
uint32 mp4_audio_compressor(mp4_infor_t *file, char**compressor);

uint32 mp4_audio_frame_size(mp4_infor_t *file, uint32  frame, uint32  *framesize);
uint32 mp4_audio_stsz_frame_size(mp4_infor_t *file, uint32  *framesize);
uint32 mp4_audio_max_frame_size(mp4_infor_t *file, uint32  *framesize);

/* get video parameter information */
uint32 mp4_video_width(mp4_infor_t *file, uint32  *width);
uint32 mp4_video_height(mp4_infor_t *file, uint32  *height);
uint32 mp4_video_depth(mp4_infor_t *file, uint32  *depth);
uint32 mp4_video_time_scale(mp4_infor_t *file, uint32  *timescale);
uint32 mp4_video_sample_duration(mp4_infor_t *file, uint32  sample, uint32  *duration);
uint32 mp4_video_frame_rate(mp4_infor_t *file, uint32  *framerate);
uint32 mp4_video_compressor(mp4_infor_t *file, char**compressor);

uint32 mp4_video_frame_size(mp4_infor_t *file, uint32  frame, uint32  *framesize);
uint32 mp4_video_max_frame_size(mp4_infor_t *file, uint32  *framesize);
/* make the quicktime file streamable */
uint32 mp4_set_time_scale(mp4_infor_t *file, uint32  time_scale);
uint32  mp4_get_time_scale(mp4_infor_t *file, uint32  *timescale); 
int mp4_get_timescale(float frame_rate);
uint32  mp4_set_audio_position(mp4_infor_t *file, uint32  sample);
uint32  mp4_set_video_position(mp4_infor_t *file, uint32  frame);
/* =========================== private interface ========================= // */

void mp4_init_video_map(mp4_video_map_t *vtrack, mp4_trak_t *trak);
void mp4_init_audio_map(mp4_audio_map_t *atrack, mp4_trak_t *trak);
void mp4_delete_video_map(mp4_video_map_t *vtrack);
void mp4_delete_audio_map(mp4_audio_map_t *atrack);

mp4_trak_t* mp4_add_track(mp4_moov_t *moov);

uint32 mp4_delete(mp4_infor_t *file);
uint32 mp4_dump(mp4_infor_t *file);

uint32 mp4_set_preload(mp4_infor_t *file, uint32  preload);
uint32 mp4_free_preload(mp4_infor_t *file);

uint32 mp4_seek_data_end(mp4_infor_t *file);
uint32 mp4_seek_data_start(mp4_infor_t *file);

uint32  mp4_audio_position(mp4_infor_t *file);
uint32  mp4_video_position(mp4_infor_t *file);

FILE* mp4_get_fd(mp4_infor_t *file);

uint32 mp4_is_syn_video_sample(mp4_infor_t *file, uint32  sample);


#ifdef __cplusplus
}
#endif

#endif
