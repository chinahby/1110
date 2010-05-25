#ifndef SMLZDEC_H
#define SMLZDEC_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

M O D I F I E D  L I M P L E - Z I V  (MLZ)  H E A D E R   F I L E

GENERAL DESCRIPTION
  This header file contains all the definitions necessary to interface to the
  MLZ decoder.


Copyright(c) 2002 by NeoMtel Co., Ltd.   All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/services/graph/smlzdec.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/24/02    sa     Initial version

===========================================================================*/
#ifdef FEATURE_MLZ_DECODER

/* Decode LZ77 return values */
#define DECODE_LZ_FINISH	0	/* Finish decode (success) */
#define DECODE_LZ_CONTINUE	1	/* Still going on decoding - more input buf needed */
#define DECODE_LZ_BUF_ERROR	(-1)	/* More buffer(input or output) needed */
#define DECODE_LZ_DATA_ERROR	(-2)	/* Block data error */
#define DECODE_LZ_MEM_ERROR	(-3)	/* Memory allocation error */
#define DECODE_LZ_INTERNAL_ERROR	(-4)

typedef enum{
	DECODE_INIT,
	DECODE_CONTINUE,
	DECODE_FINISH
}TE_DECODE_STATE;

typedef struct{
	unsigned char *buf_in;	/* Input buffer pointer - input parameter */
	unsigned long size_in;	/* Input buffer size available - input parameter */
	unsigned long total_in;	/* Total input size for this block stream so far - output parameter */

	unsigned char *buf_out;	/* Output buffer pointer - input parameter (at DECODE_INIT only) */
	unsigned long size_out;	/* Output buffer size available - input parameter (at DECODE_INIT only) */
	unsigned long total_out;/* Total input size for this block stream so far - output parameter*/

	TE_DECODE_STATE state;
}TS_DECODE_STREAM;

int SMLZ_InitDecoder_static(unsigned char* inbuf,unsigned short size);
extern int SMLZ_DecodeTable(unsigned char* htable, unsigned short htable_size);
extern int SMLZ_DecodeBlock(TS_DECODE_STREAM*);
extern int ZLIB_DecodeBlock(TS_DECODE_STREAM*);

#endif /* FEATURE_MLZ_DECODER */
#endif /* SMLZDEC_H */
