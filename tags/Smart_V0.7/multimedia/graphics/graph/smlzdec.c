/* linesize(132)
** pagesize(60)
** title("Dual Mode Subscriber Station")
** subtitle("MLZ Decoder")
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  M O D I F I E D    L I M P L E - Z I V  (MLZ)   D E C O D E R

GENERAL DESCRIPTION
  This module contains functions to decode MLZ compressed images for SAF and
  CMX content.

EXTERNALIZED FUNCTIONS
  LZ_InitDecoder
  SMLZ_DecodeTable
  SMLZ_DecodeBlock
  ZLIB_DecodeBlock
  LZ_KillDecoder

INITIALIZATION AND SEQUENCING REQUIREMENTS
  NONE

Copyright(c) 2002 by NeoMtel Co., Ltd.   All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/services/graph/smlzdec.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/19/08    kk     Fixed lint errors.
11/06/02    sa     Clean up code by removing unneeded #ifdefs. 
09/24/02    sa     Initial version
===========================================================================*/

/*===========================================================================

                       INCLUDE FILES FOR MODULE


===========================================================================*/
#include "target.h"             /* Target specific definitions             */
#include "customer.h"           /* Customer specific definitions           */
#include "comdef.h"             /* Common definitions                      */

#ifdef FEATURE_MLZ_DECODER
#include "smlzdec.h"
#include "smlzdeci.h"

/* <EJECT> */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
/* macros */
#define NEEDBYTE2 {if(z->avail_in==0)return r;r=f;}
#define NEXTBYTE2 (z->avail_in--,z->total_in++,*z->next_in++)
/* simplify the use of the inflate_huft type with some defines */
#define exop word.what.Exop
#define bits word.what.Bits
/* If BMAX needs to be larger than 16, then h and x[] should be uLong. */
#define BMAX 15         /* maximum bit length of any code */
/* macros for bit input with no checking and for returning unused bytes */
#define GRABBITS(j) {while(k<(j)){b|=((uLong)NEXTBYTE)<<k;k+=8;}}
#define UNGRAB {c=z->avail_in-n;c=(k>>3)<c?k>>3:c;n+=c;p-=c;k-=c<<3;}
#define BASE 65521L /* largest prime smaller than 65536 */
#define NMAX 5552
/* NMAX is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1 */
#define DO1(buf,i)  {s1 += buf[i]; s2 += s1;}
#define DO2(buf,i)  DO1(buf,i); DO1(buf,(i+1));
#define DO4(buf,i)  DO2(buf,i); DO2(buf,(i+2));
#define DO8(buf,i)  DO4(buf,i); DO4(buf,(i+4));
#define DO16(buf)   DO8(buf,0); DO8(buf,8);

/* constants */
/* And'ing with mask[n] masks the lower n bits */
uInt LZinflate_mask[17] = {
    0x0000,
    0x0001, 0x0003, 0x0007, 0x000f, 0x001f, 0x003f, 0x007f, 0x00ff,
    0x01ff, 0x03ff, 0x07ff, 0x0fff, 0x1fff, 0x3fff, 0x7fff, 0xffff
};

local const uInt border[] = { /* Order of the bit length code lengths */
        16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};

/* Tables for LZdeflate */
local const uInt cplens[31] = { /* Copy lengths for literal codes 257..285 */
        3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
        35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0, 0};
        /* see note #13 above about 258 */
local const uInt cplext[31] = { /* Extra bits for literal codes 257..285 */
        0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
        3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0, 112, 112}; /* 112==invalid */
local const uInt cpdist[30] = { /* Copy offsets for distance codes 0..29 */
        1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
        257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145,
        8193, 12289, 16385, 24577};
local const uInt cpdext[30] = { /* Extra bits for distance codes */
        0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6,
        7, 7, 8, 8, 9, 9, 10, 10, 11, 11,
        12, 12, 13, 13};

const char *LZdec__errmsg[10] = {
"need dictionary",     /* Z_NEED_DICT       2  */
"stream end",          /* Z_STREAM_END      1  */
"",                    /* Z_OK              0  */
"file error",          /* Z_ERRNO         (-1) */
"stream error",        /* Z_STREAM_ERROR  (-2) */
"data error",          /* Z_DATA_ERROR    (-3) */
"insufficient memory", /* Z_MEM_ERROR     (-4) */
"buffer error",        /* Z_BUF_ERROR     (-5) */
"incompatible version",/* Z_VERSION_ERROR (-6) */
""};

/* globals */
/* build fixed tables only once--keep them here */
local int fixed_built = 0;
#define FIXEDH 544      /* number of hufts used by fixed tables */
local inflate_huft fixed_mem[FIXEDH];
local uInt fixed_bl;
local uInt fixed_bd;
local inflate_huft *fixed_tl;
local inflate_huft *fixed_td;

z_stream* d_stream;
huff_tree_s gl_htree;	/* global dynamic huffman tree structure - kobj */

inflate_codes_statef* smlz_ptr_gl_inflate_codes_state;
inflate_blocks_statef* smlz_ptr_gl_inflate_block_state;
struct internal_state* smlz_ptr_gl_internal_state;
z_stream*	smlz_ptr_gl_lzko_stream;
uInt* smlz_ptr_gl_code_table;  /* 258+29+29 entry */
uInt* smlz_ptr_work_area_blen_tree;   /* 19 entry */
uInt* smlz_ptr_work_area_tree1;      /* 288 entry */ 
uInt* smlz_ptr_work_area_tree2;  /* 288 entry */


/* local functions */
int LZinflateHuff(z_streamp z);
local int LZhuft_build OF((
    uInt *, uInt, uInt,/* code lengths in bits, number of codes, number of simple codes */ 
    const uInt *,      /* list of base values for non-simple codes */
    const uInt *,      /* list of extra bits for non-simple codes */
    inflate_huft * *,  /* result: starting table */
    uInt *,            /* maximum lookup bits (returns actual) */
    inflate_huft *,    /* space for trees */
    uInt *,            /* hufts used in space */
    uInt * ));         /* space for values */

/* <EJECT> */
/*===========================================================================

FUNCTION SMLZ_InitDecoder_static 

DESCRIPTION
  This function initialize a MLZ Decoder.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
int SMLZ_InitDecoder_static(unsigned char* inbuf,unsigned short size)
{
  int result;
  unsigned short tmp;

  tmp = sizeof(inflate_huft)*MANY+sizeof(inflate_codes_statef)+sizeof(inflate_blocks_statef)
    +sizeof(struct internal_state)+sizeof(z_stream)+sizeof(uInt)*(288+288+20+316);
  if(size >= tmp)
  {
    smlz_ptr_gl_inflate_codes_state = (inflate_codes_statef*)inbuf;
	tmp = sizeof(inflate_codes_statef);
    smlz_ptr_gl_inflate_block_state = (inflate_blocks_statef*)&(inbuf[tmp]);
	tmp += sizeof(inflate_blocks_statef);
    smlz_ptr_gl_internal_state = (struct internal_state*)&(inbuf[tmp]);
	tmp += sizeof(struct internal_state);
    smlz_ptr_gl_lzko_stream = (z_stream*)&(inbuf[tmp]);
	tmp += sizeof(z_stream);
	smlz_ptr_work_area_tree2 = (uInt*)&(inbuf[tmp]);
	tmp += sizeof(uInt)*288;
    smlz_ptr_work_area_tree1 = (uInt*)&(inbuf[tmp]);
	tmp += sizeof(uInt)*288;
    smlz_ptr_work_area_blen_tree = (uInt*)&(inbuf[tmp]);
	tmp += sizeof(uInt)*20;
    smlz_ptr_gl_code_table = (uInt*)&(inbuf[tmp]);
	tmp += sizeof(uInt)*316;
    gl_htree.hufts = (inflate_huft*)&(inbuf[tmp]);
    result = 1;
  }
  else
    result = 0;
  return result;
}

/* <EJECT> */
/*===========================================================================

FUNCTION SMLZ_DecodeTable 

DESCRIPTION
  This function decode a huffman table.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
int SMLZ_DecodeTable(unsigned char* htable, unsigned short htable_size)
{
	int err;
	z_stream* t_stream;

	t_stream	= smlz_ptr_gl_lzko_stream;

	t_stream->next_in	= htable;
	t_stream->avail_in	= (uLong)htable_size;
	t_stream->next_out	= Z_NULL;
	t_stream->avail_out	= 0;

    err = LZinflateInit(t_stream);
	if(err != Z_OK)
	{
		switch(err)
		{
		case Z_MEM_ERROR:	return DECODE_LZ_MEM_ERROR;
		default:	return DECODE_LZ_INTERNAL_ERROR;
		}
	}

    err = LZinflateHuff(t_stream);
	if(err != Z_STREAM_END)
	{
		switch(err)
		{
		case Z_OK: return DECODE_LZ_BUF_ERROR;
		case Z_BUF_ERROR:	return DECODE_LZ_BUF_ERROR;
		case Z_DATA_ERROR:	return DECODE_LZ_DATA_ERROR;
		case Z_MEM_ERROR:	return DECODE_LZ_MEM_ERROR;
		default:	return DECODE_LZ_INTERNAL_ERROR;
		}
	}

	err = LZinflateEnd(t_stream);

	return err;
}

/* <EJECT> */
/*===========================================================================

FUNCTION SMLZ_DecodeBlock 

DESCRIPTION
  This function decode a mlz data block.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
int SMLZ_DecodeBlock(TS_DECODE_STREAM* strm)
{
	int err;
	int ret;

	if(strm->state == DECODE_INIT)
	{
		/* Initialize z_stream structure */
		d_stream = smlz_ptr_gl_lzko_stream;
		d_stream->next_in	= strm->buf_in;
		d_stream->avail_in	= strm->size_in;
		d_stream->next_out	= strm->buf_out;
		d_stream->avail_out	= strm->size_out;

		err = LZinflateInit(d_stream);
		if (err != Z_OK)
		{
			switch(err)
			{
			case Z_MEM_ERROR:	return DECODE_LZ_MEM_ERROR;
			default:	return DECODE_LZ_INTERNAL_ERROR;
			}
		}
	}
	else
	{
		/* Update input buffer only */
		d_stream->next_in	= strm->buf_in;
		d_stream->avail_in	= strm->size_in;
	}

	err	= LZinflate(d_stream, Z_NO_FLUSH, COMPRESS_SCHEME_SMLZ);
	switch(err)
	{
		case Z_STREAM_END:
			ret = DECODE_LZ_FINISH;
			break;
		case Z_OK:
		case Z_BUF_ERROR:
			if(strm->state == DECODE_INIT || strm->state == DECODE_CONTINUE)
			{
				strm->total_out	= d_stream->total_out;
				strm->total_in	= d_stream->total_in;
				return DECODE_LZ_CONTINUE;
			}
			else
			{
				ret = DECODE_LZ_BUF_ERROR;
				break;
			}
		case Z_DATA_ERROR:	
			ret = DECODE_LZ_DATA_ERROR;
			break;
		case Z_MEM_ERROR:	
			ret = DECODE_LZ_MEM_ERROR;
			break;
		default:	
			ret = DECODE_LZ_INTERNAL_ERROR;
			break;
	}

	strm->total_in	= d_stream->total_in;
	strm->total_out	= d_stream->total_out;
	err = LZinflateEnd(d_stream);
	return ret;
}

/* <EJECT> */
/*===========================================================================

FUNCTION ZLIB_DecodeBlock 

DESCRIPTION
  This function decode a zlib data block.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
int ZLIB_DecodeBlock(TS_DECODE_STREAM* strm)
{
	int err;
	int ret;

	if(strm->state == DECODE_INIT)
	{
		/* Initialize z_stream structure */
		d_stream = smlz_ptr_gl_lzko_stream;

		d_stream->next_in	= strm->buf_in;
		d_stream->avail_in	= strm->size_in;
		d_stream->next_out	= strm->buf_out;
		d_stream->avail_out	= strm->size_out;

		err = LZinflateInit(d_stream);
		if (err != Z_OK)
		{
			switch(err)
			{
			case Z_MEM_ERROR:	return DECODE_LZ_MEM_ERROR;
			default:	return DECODE_LZ_INTERNAL_ERROR;
			}
		}
	}
	else
	{
		/* Update input buffer only */
		d_stream->next_in	= strm->buf_in;
		d_stream->avail_in	= strm->size_in;
	}

	err	= LZinflate(d_stream, Z_NO_FLUSH, COMPRESS_SCHEME_ZLIB);
	switch(err)
	{
		case Z_STREAM_END:
			ret = DECODE_LZ_FINISH;
			break;
		case Z_OK:
		case Z_BUF_ERROR:
			if(strm->state == DECODE_INIT || strm->state == DECODE_CONTINUE)
			{
				strm->total_out	= d_stream->total_out;
				strm->total_in	= d_stream->total_in;
				return DECODE_LZ_CONTINUE;
			}
			else
			{
				ret = DECODE_LZ_BUF_ERROR;
				break;
			}
		case Z_DATA_ERROR:	
			ret = DECODE_LZ_DATA_ERROR;
			break;
		case Z_MEM_ERROR:	
			ret = DECODE_LZ_MEM_ERROR;
			break;
		default:	
			ret = DECODE_LZ_INTERNAL_ERROR;
			break;
	}

	strm->total_in	= d_stream->total_in;
	strm->total_out	= d_stream->total_out;
	err = LZinflateEnd(d_stream);

	return ret;
}

/* <EJECT> */
/*===========================================================================

FUNCTION LZinflate_blocks_reset 

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void LZinflate_blocks_reset(inflate_blocks_statef *s, z_streamp z, uLong *c)
{
	if (c != Z_NULL)
		*c = s->check;
	if (s->mode == CODES)
		LZinflate_codes_free(s->sub.decode.codes, z);
	s->mode = TYPE;
	s->bitk = 0;
	s->bitb = 0;
	s->read = s->write = s->window;
	if (s->checkfn != Z_NULL)
		z->adler = s->check = (*s->checkfn)(0L, (const Byte *)Z_NULL, 0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION LZinflate_blocks_new 

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
inflate_blocks_statef *LZinflate_blocks_new(z_streamp z, LZcheck_func c, uInt w)
{
  inflate_blocks_statef *s;

  s = smlz_ptr_gl_inflate_block_state;

  /* hufmann tree points to global tree - kobj */
  s->hufts = gl_htree.hufts;

  /* use output buffer as LZinflate window buffer - kobj */
  s->window	= z->next_out;

  /* kobj - set window end to min of window size and output buffer size */
  s->checkfn = c;
  s->mode = TYPE;
  LZinflate_blocks_reset(s, z, Z_NULL);
  return s;
}


/* <EJECT> */
/*===========================================================================

FUNCTION LZinflate_blocks 

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/* LZinflate_blocks() - modified by kobj 
 *	(exclude routines related to decoding dynamic huffman table)
 */
int LZinflate_blocks(inflate_blocks_statef *s, z_streamp z, int r, int scheme)
{
	uLong t;               /* temporary storage */
	uLong b;              /* bit buffer */
	uInt k;               /* bits in bit buffer */
	Byte *p;             /* input data pointer */
	uLong n;               /* bytes available there */
	Byte *q;             /* output window write pointer */
	uLong m;               /* bytes to end of window or read pointer */
	
	/* copy input/output information to locals (UPDATE macro restores) */
	LOAD
		
	/* process input based on current state */
	while (1) switch (s->mode)
	{
    case TYPE:
		NEEDBITS(3)
		t = b & 7;
		s->last = (uInt)t & 1;
		switch (t >> 1)
		{
        case 0:                         /* stored */
			DUMPBITS(3)
			t = (uLong)k & 7;                    /* go to byte boundary */
			DUMPBITS(((uInt)t))
			s->mode = LENS;               /* get length of stored block */
			break;
        case 1:                         /* fixed */
			{
				LZinflate_trees_fixed(&gl_htree.bl, &gl_htree.bd, &gl_htree.tl, &gl_htree.td, z);

				s->sub.decode.codes = LZinflate_codes_new(gl_htree.bl, gl_htree.bd, 
														  gl_htree.tl, gl_htree.td, z);

				if (s->sub.decode.codes == Z_NULL)
				{
					r = Z_MEM_ERROR;
					LEAVE
				}
			}
			DUMPBITS(3)
			s->mode = CODES;
			break;
        case 2:                         /* dynamic - modified by kobj */
			/* If zlib, decode dynamic huffman tree here - kobj 
			 * else if smlz, go to decoding code block */
			DUMPBITS(3)
			if(scheme == COMPRESS_SCHEME_ZLIB)
			{
				UPDATE
				r = LZinflate_huff_block(s, z);
				LOAD
				if(r != Z_STREAM_END)
				{
					LEAVE
				}
			}

			{
				inflate_codes_statef *c;

				if ((c = LZinflate_codes_new(gl_htree.bl, gl_htree.bd,
											 gl_htree.tl, gl_htree.td, z)) == Z_NULL)
				{
					r = Z_MEM_ERROR;
					LEAVE
				}
				s->sub.decode.codes = c;
			}
			s->mode = CODES;				
			break;
        case 3:                         /* illegal */
			DUMPBITS(3)
			s->mode = BAD;
			z->msg = (char*)"invalid block type";
			r = Z_DATA_ERROR;
			LEAVE
		}
		break;
		case LENS:
			NEEDBITS(32)
			if ((((~b) >> 16) & 0xffff) != (b & 0xffff))
			{
				s->mode = BAD;
				z->msg = (char*)"invalid stored block lengths";
				r = Z_DATA_ERROR;
				LEAVE
			}
			s->sub.left = (uInt)b & 0xffff;
			b = k = 0;                      /* dump bits */
			s->mode = s->sub.left ? STORED : (s->last ? DRY : TYPE);
			break;
		case STORED:
			if (n == 0)
				LEAVE
			NEEDOUT
			t = s->sub.left;
			if (t > n) t = n;
			if (t > m) t = m;
			zmemcpy_dec(q, p, t);
			p += t;   n -= t;
			q += t;  m -= t;
			if ((s->sub.left -= (uInt)t) != 0)
				break;
			s->mode = s->last ? DRY : TYPE;
			break;
		case CODES:
			s->mode = CODES;
			UPDATE
			if ((r = LZinflate_codes(s, z, r)) != Z_STREAM_END)
				return LZinflate_flush(s, z, r);
			r = Z_OK;
			LZinflate_codes_free(s->sub.decode.codes, z);
			LOAD
			if (!s->last)
			{
				s->mode = TYPE;
				break;
			}
			s->mode = DRY;
		case DRY:
			FLUSH
			s->mode = DONE;
		case DONE:
			r = Z_STREAM_END;
			LEAVE
		case BAD:
			r = Z_DATA_ERROR;
			LEAVE
		default:
			r = Z_STREAM_ERROR;
			LEAVE
  }
}


/* <EJECT> */
/*===========================================================================

FUNCTION LZinflate_blocks_free 

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
int LZinflate_blocks_free(inflate_blocks_statef *s, z_streamp z)
{
  LZinflate_blocks_reset(s, z, Z_NULL);

  return Z_OK;
}

/* <EJECT> */
/*===========================================================================

FUNCTION LZinflate_huff_block 

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/* Functions Added by kobj */
int LZinflate_huff_block(inflate_blocks_statef* s, z_streamp z)
{
	uInt t;               /* temporary storage */
	uLong b;              /* bit buffer */
	uInt k;               /* bits in bit buffer */
	Byte *p;             /* input data pointer */
	uLong n;               /* bytes available there */
	Byte *q;             /* output window write pointer */
	uLong m;               /* bytes to end of window or read pointer */

	int r	= Z_BUF_ERROR;
	
	/* copy input/output information to locals (UPDATE macro restores) */
	LOAD

	s->mode	= TABLE;

	while(1)
	{
		switch(s->mode)
		{
		case TABLE:
			NEEDBITS(14)
			s->sub.trees.table = t = (uInt)b & 0x3fff;
			if ((t & 0x1f) > 29 || ((t >> 5) & 0x1f) > 29)
			{
				z->msg = (char*)"too many length or distance symbols";
				r = Z_DATA_ERROR;
				LEAVE
			}
			t = 258 + (t & 0x1f) + ((t >> 5) & 0x1f);
			s->sub.trees.blens = smlz_ptr_gl_code_table;
			DUMPBITS(14)
			s->sub.trees.index = 0;
			s->mode = BTREE;
		case BTREE:
			while (s->sub.trees.index < 4 + (s->sub.trees.table >> 10))
			{
				NEEDBITS(3)
				s->sub.trees.blens[border[s->sub.trees.index]] = (uInt)b & 7;
				s->sub.trees.index++;
				DUMPBITS(3)
			}
			while (s->sub.trees.index < 19)
				s->sub.trees.blens[border[s->sub.trees.index++]] = 0;
			s->sub.trees.bb = 7;
			r = LZinflate_trees_bits(s->sub.trees.blens, &s->sub.trees.bb,
				&s->sub.trees.tb, s->hufts, z);
			if (r != Z_OK)
			{
				if (r == Z_DATA_ERROR)
					s->mode = BAD;
				LEAVE
			}
			s->sub.trees.index = 0;
			s->mode = DTREE;
		case DTREE:
			while (t = s->sub.trees.table,
				s->sub.trees.index < 258 + (t & 0x1f) + ((t >> 5) & 0x1f))
			{
				inflate_huft *h;
				uInt i, j, c;
				
				t = s->sub.trees.bb;
				NEEDBITS(t)
				h = s->sub.trees.tb + ((uInt)b & LZinflate_mask[t]);
				t = h->bits;
				c = h->base;
				if (c < 16)
				{
					DUMPBITS(t)
					s->sub.trees.blens[s->sub.trees.index++] = c;
				}
				else /* c == 16..18 */
				{
					i = c == 18 ? 7 : c - 14;
					j = c == 18 ? 11 : 3;
					NEEDBITS(t + i)
					DUMPBITS(t)
					j += (uInt)b & LZinflate_mask[i];
					DUMPBITS(i)
					i = s->sub.trees.index;
					t = s->sub.trees.table;
					if (i + j > 258 + (t & 0x1f) + ((t >> 5) & 0x1f) ||
						(c == 16 && i < 1))
					{
						s->mode	= BAD;
						z->msg = (char*)"invalid bit length repeat";
						r = Z_DATA_ERROR;
						LEAVE
					}
					c = c == 16 ? s->sub.trees.blens[i - 1] : 0;
					do {
						s->sub.trees.blens[i++] = c;
					} while (--j);
					s->sub.trees.index = i;
				}
			}
			s->sub.trees.tb = Z_NULL;
			{
				gl_htree.bl = 9;         /* must be <= 9 for lookahead assumptions */
				gl_htree.bd = 6;         /* must be <= 9 for lookahead assumptions */
				t = s->sub.trees.table;
				t = LZinflate_trees_dynamic(257 + (t & 0x1f), 1 + ((t >> 5) & 0x1f),
					s->sub.trees.blens, &gl_htree.bl, &gl_htree.bd, &gl_htree.tl, &gl_htree.td,
					s->hufts, z);
				if (t != Z_OK)
				{
					if (t == (uInt)Z_DATA_ERROR)
						s->mode = BAD;
					r = t;
					LEAVE
				}
				s->mode	= DONE;
				r = Z_STREAM_END;
				LEAVE
			}
		default: 
			r = Z_STREAM_ERROR;
			LEAVE
		}
	}
}



/* <EJECT> */
/*===========================================================================

FUNCTION LZinflateReset 

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
int LZinflateReset(z_streamp z)
{
  if (z == Z_NULL || z->state == Z_NULL)
    return Z_STREAM_ERROR;
  z->total_in = z->total_out = 0;
  z->msg = Z_NULL;
  z->state->mode = z->state->nowrap ? BLOCKS : METHOD;
  LZinflate_blocks_reset(z->state->blocks, z, Z_NULL);
  return Z_OK;
}

/* <EJECT> */
/*===========================================================================

FUNCTION LZinflateEnd 

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
int LZinflateEnd(z_streamp z)
{
  if (z == Z_NULL || z->state == Z_NULL)
    return Z_STREAM_ERROR;
  if (z->state->blocks != Z_NULL)
    LZinflate_blocks_free(z->state->blocks, z);

  z->state = Z_NULL;
  return Z_OK;
}

/* <EJECT> */
/*===========================================================================

FUNCTION LZinflateInit2_ 

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
int LZinflateInit2_(z_streamp z, int w, const char *version, int stream_size)
{
  if (version == Z_NULL || version[0] != ZLIB_VERSION[0] ||
      stream_size != sizeof(z_stream))
      return Z_VERSION_ERROR;

  /* initialize state */
  if (z == Z_NULL)
    return Z_STREAM_ERROR;
  z->msg = Z_NULL;

  z->state = smlz_ptr_gl_internal_state;

  z->state->blocks = Z_NULL;

  /* handle undocumented nowrap option (no zlib header or check) */
  z->state->nowrap = 0;
  if (w < 0)
  {
    w = - w;
    z->state->nowrap = 1;
  }

  /* set window size */
  if (w < 8 || w > 15)
  {
    LZinflateEnd(z);
    return Z_STREAM_ERROR;
  }
  z->state->wbits = (uInt)w;

  /* create LZinflate_blocks state */
  if ((z->state->blocks =
      LZinflate_blocks_new(z, z->state->nowrap ? Z_NULL : LZdec_adler32, (uInt)1 << w))
      == Z_NULL)
  {
    LZinflateEnd(z);
    return Z_MEM_ERROR;
  }

  /* reset state */
  LZinflateReset(z);
  return Z_OK;
}

/* <EJECT> */
/*===========================================================================

FUNCTION LZinflateInit_ 

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
int LZinflateInit_(z_streamp z, const char *version, int stream_size)
{
  return LZinflateInit2_(z, DEF_WBITS, version, stream_size);
}

/* <EJECT> */
/*===========================================================================

FUNCTION LZinflate 

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/* LZinflate() - modified by kobj 
 *	(exclude routines related to decoding dynamic huffman table)
 */
int LZinflate(z_streamp z, int f, int scheme)
{
  int r;
  uInt b;

  if (z == Z_NULL || z->state == Z_NULL || z->next_in == Z_NULL)
    return Z_STREAM_ERROR;
  f = f == Z_FINISH ? Z_BUF_ERROR : Z_OK;
  r = Z_BUF_ERROR;

  while (1) switch (z->state->mode)
  {
    case METHOD:
      NEEDBYTE2
      if (((z->state->sub.method = NEXTBYTE2) & 0xf) != Z_DEFLATED)
      {
        z->state->mode = BAD2;
        z->msg = (char*)"unknown compression method";
        break;
      }
      if ((z->state->sub.method >> 4) + 8 > z->state->wbits)
      {
        z->state->mode = BAD2;
        z->msg = (char*)"invalid window size";
        break;
      }
      z->state->mode = FLAG;
    case FLAG:
      NEEDBYTE2
      b = NEXTBYTE2;
      if (((z->state->sub.method << 8) + b) % 31)
      {
        z->state->mode = BAD2;
        z->msg = (char*)"incorrect header check";
        break;
      }
	  z->state->mode = BLOCKS;
    case BLOCKS:
      r = LZinflate_blocks(z->state->blocks, z, r, scheme);
      if (r == Z_DATA_ERROR)
      {
        z->state->mode = BAD2;
        break;
      }
      if (r == Z_OK)
        r = f;
      if (r != Z_STREAM_END)
        return r;
      r = f;
      LZinflate_blocks_reset(z->state->blocks, z, &z->state->sub.check.was);
      if (z->state->nowrap)
      {
        z->state->mode = DONE2;
        break;
      }
      z->state->mode = CHECK4;
    case CHECK4:
      NEEDBYTE2
      z->state->sub.check.need = (uLong)NEXTBYTE2 << 24;
      z->state->mode = CHECK3;
    case CHECK3:
      NEEDBYTE2
      z->state->sub.check.need += (uLong)NEXTBYTE2 << 16;
      z->state->mode = CHECK2;
    case CHECK2:
      NEEDBYTE2
      z->state->sub.check.need += (uLong)NEXTBYTE2 << 8;
      z->state->mode = CHECK1;
    case CHECK1:
      NEEDBYTE2
      z->state->sub.check.need += (uLong)NEXTBYTE2;

      if (z->state->sub.check.was != z->state->sub.check.need)
      {
        z->state->mode = BAD2;
        z->msg = (char*)"incorrect data check";
        break;
      }
      z->state->mode = DONE2;
    case DONE2:
      return Z_STREAM_END;
    case BAD2:
      return Z_DATA_ERROR;
    default:
      return Z_STREAM_ERROR;
  }
}

/* <EJECT> */
/*   Functions Added by kobj    */
/*===========================================================================

FUNCTION LZinflateHuff 

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
int LZinflateHuff(z_streamp z)
{
	return LZinflate_huff_block(z->state->blocks, z);
}

/* <EJECT> */
/*===========================================================================

FUNCTION LZhuft_build 

DESCRIPTION
  This function build huffman tree.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
local int LZhuft_build(
	uInt *b,               /* code lengths in bits (all assumed <= BMAX) */
	uInt n,                 /* number of codes (assumed <= 288) */
	uInt s,                 /* number of simple-valued codes (0..s-1) */
	const uInt *d,         /* list of base values for non-simple codes */
	const uInt *e,         /* list of extra bits for non-simple codes */
	inflate_huft * *t,  /* result: starting table */
	uInt *m,               /* maximum lookup bits, returns actual */
	inflate_huft *hp,       /* space for trees */
	uInt *hn,               /* hufts used in space */
	uInt *v                /* working area: values in order of bit length */
)
/* Given a list of code lengths and a maximum table size, make a set of
   tables to decode that set of codes.  Return Z_OK on success, Z_BUF_ERROR
   if the given code set is incomplete (the tables are still built in this
   case), Z_DATA_ERROR if the input is invalid (an over-subscribed set of
   lengths), or Z_MEM_ERROR if not enough memory. */
{

  uInt a;                       /* counter for codes of length k */
  uInt c[BMAX+1];               /* bit length count table */
  uInt f;                       /* i repeats in table every f entries */
  int g;                        /* maximum code length */
  int h;                        /* table level */
  register uInt i;              /* counter, current code */
  register uInt j;              /* counter */
  register int k;               /* number of bits in current code */
  int l;                        /* bits per table (returned in m) */
  uInt mask;                    /* (1 << w) - 1, to avoid cc -O bug on HP */
  register uInt *p;            /* pointer into c[], b[], or v[] */
  inflate_huft *q;              /* points to current table */
  struct inflate_huft_s r;      /* table entry for structure assignment */
  inflate_huft *u[BMAX];        /* table stack */
  register int w;               /* bits before this table == (l * h) */
  uInt x[BMAX+1];               /* bit offsets, then code stack */
  uInt *xp;                    /* pointer into x */
  int y;                        /* number of dummy codes added */
  uInt z;                       /* number of entries in current table */


  /* Generate counts for each bit length */
  p = c;
#define C0 *p++ = 0;
#define C2 C0 C0 C0 C0
#define C4 C2 C2 C2 C2
  C4                            /* clear c[]--assume BMAX+1 is 16 */
  p = b;  i = n;
  do {
    c[*p++]++;                  /* assume all entries <= BMAX */
  } while (--i);
  if (c[0] == n)                /* null input--all zero length codes */
  {
    *t = (inflate_huft *)Z_NULL;
    *m = 0;
    return Z_OK;
  }


  /* Find minimum and maximum length, bound *m by those */
  l = *m;
  for (j = 1; j <= BMAX; j++)
    if (c[j])
      break;
  k = j;                        /* minimum code length */
  if ((uInt)l < j)
    l = j;
  for (i = BMAX; i; i--)
    if (c[i])
      break;
  g = i;                        /* maximum code length */
  if ((uInt)l > i)
    l = i;
  *m = l;


  /* Adjust last length count to fill out codes, if needed */
  for (y = 1 << j; j < i; j++, y <<= 1)
    if ((y -= c[j]) < 0)
      return Z_DATA_ERROR;
  if ((y -= c[i]) < 0)
    return Z_DATA_ERROR;
  c[i] += y;


  /* Generate starting offsets into the value table for each length */
  x[1] = j = 0;
  p = c + 1;  xp = x + 2;
  while (--i) {                 /* note that i == g from above */
    *xp++ = (j += *p++);
  }


  /* Make a table of values in order of bit lengths */
  p = b;  i = 0;
  do {
    if ((j = *p++) != 0)
      v[x[j]++] = i;
  } while (++i < n);
  n = x[g];                     /* set n to length of v */


  /* Generate the Huffman codes and for each, make the table entries */
  x[0] = i = 0;                 /* first Huffman code is zero */
  p = v;                        /* grab values in bit order */
  h = -1;                       /* no tables yet--level -1 */
  w = -l;                       /* bits decoded == (l * h) */
  u[0] = (inflate_huft *)Z_NULL;        /* just to keep compilers happy */
  q = (inflate_huft *)Z_NULL;   /* ditto */
  z = 0;                        /* ditto */

  /* go through the bit lengths (k already is bits in shortest code) */
  for (; k <= g; k++)
  {
    a = c[k];
    while (a--)
    {
      /* here i is the Huffman code of length k bits for value *p */
      /* make tables up to required level */
      while (k > w + l)
      {
        h++;
        w += l;                 /* previous table always l bits */

        /* compute minimum size table less than or equal to l bits */
        z = g - w;
        z = z > (uInt)l ? l : z;        /* table size upper limit */
        if ((f = 1 << (j = k - w)) > a + 1)     /* try a k-w bit table */
        {                       /* too few codes for k-w bit table */
          f -= a + 1;           /* deduct codes from patterns left */
          xp = c + k;
          if (j < z)
            while (++j < z)     /* try smaller tables up to z bits */
            {
              if ((f <<= 1) <= *++xp)
                break;          /* enough codes to use up j bits */
              f -= *xp;         /* else deduct codes from patterns */
            }
        }
        z = 1 << j;             /* table entries for j-bit table */

        /* allocate new table */
        if (*hn + z > MANY)     /* (note: doesn't matter for fixed) */
          return Z_MEM_ERROR;   /* not enough memory */
        u[h] = q = hp + *hn;
        *hn += z;

        /* connect to last table, if there is one */
        if (h)
        {
          x[h] = i;             /* save pattern for backing up */
          r.bits = (Byte)l;     /* bits to dump before this table */
          r.exop = (Byte)j;     /* bits in this table */
          j = i >> (w - l);
          r.base = (uInt)(q - u[h-1] - j);   /* offset to this table */
          u[h-1][j] = r;        /* connect to last table */
        }
        else
          *t = q;               /* first table is returned result */
      }

      /* set up table entry in r */
      r.bits = (Byte)(k - w);
      if (p >= v + n)
        r.exop = 128 + 64;      /* out of values--invalid code */
      else if (*p < s)
      {
        r.exop = (Byte)(*p < 256 ? 0 : 32 + 64);     /* 256 is end-of-block */
        r.base = *p++;          /* simple code is just the value */
      }
      else
      {
        r.exop = (Byte)(e[*p - s] + 16 + 64);/* non-simple--look up in lists */
        r.base = d[*p++ - s];
      }

      /* fill code-like entries with r */
      f = 1 << (k - w);
      for (j = i >> w; j < z; j += f)
        q[j] = r;

      /* backwards increment the k-bit code i */
      for (j = 1 << (k - 1); i & j; j >>= 1)
        i ^= j;
      i ^= j;

      /* backup over finished tables */
      mask = (1 << w) - 1;      /* needed on HP, cc -O bug */
      while ((i & mask) != x[h])
      {
        h--;                    /* don't need to update q */
        w -= l;
        mask = (1 << w) - 1;
      }
    }
  }


  /* Return Z_BUF_ERROR if we were given an incomplete table */
  return y != 0 && g != 1 ? Z_BUF_ERROR : Z_OK;
}

/* <EJECT> */
/*===========================================================================

FUNCTION LZinflate_trees_bits 

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
int LZinflate_trees_bits(
	uInt *c,               /* 19 code lengths */
	uInt *bb,              /* bits tree desired/actual depth */
	inflate_huft * *tb, /* bits tree result */
	inflate_huft *hp,       /* space for trees */
	z_streamp z             /* for messages */
)
{
  int r;
  uInt hn = 0;          /* hufts used in space */
  uInt *v;             /* work area for LZhuft_build */

  v = smlz_ptr_work_area_blen_tree;

  r = LZhuft_build(c, 19, 19, (uInt*)Z_NULL, (uInt*)Z_NULL,
                 tb, bb, hp, &hn, v);
  if (r == Z_DATA_ERROR)
    z->msg = (char*)"oversubscribed dynamic bit lengths tree";
  else if (r == Z_BUF_ERROR || *bb == 0)
  {
    z->msg = (char*)"incomplete dynamic bit lengths tree";
    r = Z_DATA_ERROR;
  }

  return r;
}

/* <EJECT> */
/*===========================================================================

FUNCTION LZinflate_trees_dynamic 

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
int LZinflate_trees_dynamic(
	uInt nl,                /* number of literal/length codes */
	uInt nd,                /* number of distance codes */
	uInt *c,               /* that many (total) code lengths */
	uInt *bl,              /* literal desired/actual bit depth */
	uInt *bd,              /* distance desired/actual bit depth */
	inflate_huft * *tl, /* literal/length tree result */
	inflate_huft * *td, /* distance tree result */
	inflate_huft *hp,       /* space for trees */
	z_streamp z            /* for messages */
)
{
  int r;
  uInt hn = 0;          /* hufts used in space */
  uInt *v;             /* work area for LZhuft_build */

  /* allocate work area */
  v = smlz_ptr_work_area_tree1;

  /* build literal/length tree */
  r = LZhuft_build(c, nl, 257, cplens, cplext, tl, bl, hp, &hn, v);
  if (r != Z_OK || *bl == 0)
  {
    if (r == Z_DATA_ERROR)
      z->msg = (char*)"oversubscribed literal/length tree";
    else if (r != Z_MEM_ERROR)
    {
      z->msg = (char*)"incomplete literal/length tree";
      r = Z_DATA_ERROR;
    }
    return r;
  }

  /* build distance tree */
  r = LZhuft_build(c + nl, nd, 0, cpdist, cpdext, td, bd, hp, &hn, v);
  if (r != Z_OK || (*bd == 0 && nl > 257))
  {
    if (r == Z_DATA_ERROR)
      z->msg = (char*)"oversubscribed distance tree";
    else if (r == Z_BUF_ERROR) {
      z->msg = (char*)"incomplete distance tree";
      r = Z_DATA_ERROR;
    }
    else if (r != Z_MEM_ERROR)
    {
      z->msg = (char*)"empty distance tree with lengths";
      r = Z_DATA_ERROR;
    }
    return r;
  }

  /* done */
  return Z_OK;
}


/* <EJECT> */
/*===========================================================================

FUNCTION LZinflate_trees_fixed 

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
int LZinflate_trees_fixed(
	uInt *bl,               /* literal desired/actual bit depth */
	uInt *bd,               /* distance desired/actual bit depth */
	inflate_huft * *tl,  /* literal/length tree result */
	inflate_huft * *td,  /* distance tree result */
	z_streamp z              /* for memory allocation */
)
{
  /* build fixed tables if not already */
  if (!fixed_built)
  {
    int k;              /* temporary variable */
    uInt f = 0;         /* number of hufts used in fixed_mem */
    uInt *c;           /* length list for LZhuft_build */
    uInt *v;           /* work area for LZhuft_build */

	c = smlz_ptr_work_area_tree1;
	v = smlz_ptr_work_area_tree2;

    /* literal table */
    for (k = 0; k < 144; k++)
      c[k] = 8;
    for (; k < 256; k++)
      c[k] = 9;
    for (; k < 280; k++)
      c[k] = 7;
    for (; k < 288; k++)
      c[k] = 8;
    fixed_bl = 9;

    LZhuft_build(c, 288, 257, cplens, cplext, &fixed_tl, &fixed_bl,
               fixed_mem, &f, v);

    /* distance table */
    for (k = 0; k < 30; k++)
      c[k] = 5;
    fixed_bd = 5;
    LZhuft_build(c, 30, 0, cpdist, cpdext, &fixed_td, &fixed_bd,
               fixed_mem, &f, v);

    /* done */
    fixed_built = 1;
  }
  *bl = fixed_bl;
  *bd = fixed_bd;
  *tl = fixed_tl;
  *td = fixed_td;
  return Z_OK;
}

/*===========================================================================

FUNCTION zmemcpy_dec 

DESCRIPTION
  This function is memcpy function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void zmemcpy_dec(Byte* dest, const Byte* source, uInt len)
{
    if (len == 0) return;
    do {
        *dest++ = *source++; /* ??? to be unrolled */
    } while (--len != 0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION zmemcmp_dec 

DESCRIPTION
  This function is memcmp function

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
int zmemcmp_dec(const Byte* s1, const Byte* s2, uInt len)
{
    uInt j;

    for (j = 0; j < len; j++) {
        if (s1[j] != s2[j]) return 2*(s1[j] > s2[j])-1;
    }
    return 0;
}

/* <EJECT> */
/*===========================================================================

FUNCTION zmemzero_dec 

DESCRIPTION
  This function is memset to 0.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void zmemzero_dec(Byte* dest, uInt len)
{
    if (len == 0) return;
    do {
        *dest++ = 0;  /* ??? to be unrolled */
    } while (--len != 0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION inflate_codes_statef 

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
inflate_codes_statef *LZinflate_codes_new(uInt bl, uInt bd, 
										inflate_huft *tl, inflate_huft *td,
										z_streamp z)
{
	inflate_codes_statef *c;
	
	c = smlz_ptr_gl_inflate_codes_state;
    c->mode = START;
    c->lbits = (Byte)bl;
    c->dbits = (Byte)bd;
    c->ltree = tl;
    c->dtree = td;
	
	return c;
}

/* <EJECT> */
/*===========================================================================

FUNCTION LZinflate_codes 

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
int LZinflate_codes(inflate_blocks_statef *s, z_streamp z, int r)
{
  uInt j;               /* temporary storage */
  inflate_huft *t;      /* temporary pointer */
  uInt e;               /* extra bits or operation */
  uLong b;              /* bit buffer */
  uInt k;               /* bits in bit buffer */
  Byte *p;             /* input data pointer */
  uLong n;               /* bytes available there */
  Byte *q;             /* output window write pointer */
  uLong m;               /* bytes to end of window or read pointer */
  Byte *f;             /* pointer to copy strings from */
  inflate_codes_statef *c = s->sub.decode.codes;  /* codes state */

  /* copy input/output information to locals (UPDATE macro restores) */
  LOAD

  /* process input and output based on current state */
  while (1) switch (c->mode)
  {             /* waiting for "i:"=input, "o:"=output, "x:"=nothing */
    case START:         /* x: set up for LEN */
      if (m >= 258 && n >= 10)
      {
        UPDATE
        r = LZinflate_fast(c->lbits, c->dbits, c->ltree, c->dtree, s, z);
        LOAD
        if (r != Z_OK)
        {
          c->mode = r == Z_STREAM_END ? WASH : BADCODE;
          break;
        }
      }
      c->sub.code.need = c->lbits;
      c->sub.code.tree = c->ltree;
      c->mode = LEN;
    case LEN:           /* i: get length/literal/eob next */
      j = c->sub.code.need;
      NEEDBITS(j)
      t = c->sub.code.tree + ((uInt)b & LZinflate_mask[j]);
      DUMPBITS(t->bits)
      e = (uInt)(t->exop);
      if (e == 0)               /* literal */
      {
        c->sub.lit = t->base;
        c->mode = LIT;
        break;
      }
      if (e & 16)               /* length */
      {
        c->sub.copy.get = e & 15;
        c->len = t->base;
        c->mode = LENEXT;
        break;
      }
      if ((e & 64) == 0)        /* next table */
      {
        c->sub.code.need = e;
        c->sub.code.tree = t + t->base;
        break;
      }
      if (e & 32)               /* end of block */
      {
        c->mode = WASH;
        break;
      }
      c->mode = BADCODE;        /* invalid code */
      z->msg = (char*)"invalid literal/length code";
      r = Z_DATA_ERROR;
      LEAVE
    case LENEXT:        /* i: getting length extra (have base) */
      j = c->sub.copy.get;
      NEEDBITS(j)
      c->len += (uInt)b & LZinflate_mask[j];
      DUMPBITS(j)
      c->sub.code.need = c->dbits;
      c->sub.code.tree = c->dtree;
      c->mode = DIST;
    case DIST:          /* i: get distance next */
      j = c->sub.code.need;
      NEEDBITS(j)
      t = c->sub.code.tree + ((uInt)b & LZinflate_mask[j]);
      DUMPBITS(t->bits)
      e = (uInt)(t->exop);
      if (e & 16)               /* distance */
      {
        c->sub.copy.get = e & 15;
        c->sub.copy.dist = t->base;
        c->mode = DISTEXT;
        break;
      }
      if ((e & 64) == 0)        /* next table */
      {
        c->sub.code.need = e;
        c->sub.code.tree = t + t->base;
        break;
      }
      c->mode = BADCODE;        /* invalid code */
      z->msg = (char*)"invalid distance code";
      r = Z_DATA_ERROR;
      LEAVE
    case DISTEXT:       /* i: getting distance extra */
      j = c->sub.copy.get;
      NEEDBITS(j)
      c->sub.copy.dist += (uInt)b & LZinflate_mask[j];
      DUMPBITS(j)
      c->mode = COPY;
    case COPY:          /* o: copying bytes in window, waiting for space */
      f = q - c->sub.copy.dist;
      while (c->len)
      {
        NEEDOUT
        OUTBYTE(*f++)
        c->len--;
      }
      c->mode = START;
      break;
    case LIT:           /* o: got literal, waiting for output space */
      NEEDOUT
      OUTBYTE(c->sub.lit)
      c->mode = START;
      break;
    case WASH:          /* o: got eob, possibly more output */
      if (k > 7)        /* return unused byte, if any */
      {
        k -= 8;
        n++;
        p--;            /* can always return one */
      }
      FLUSH
      c->mode = END;
    case END:
      r = Z_STREAM_END;
      LEAVE
    case BADCODE:       /* x: got error */
      r = Z_DATA_ERROR;
      LEAVE
    default:
      r = Z_STREAM_ERROR;
      LEAVE
  }
}


/* <EJECT> */
/*===========================================================================

FUNCTION LZinflate_codes_free 

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void LZinflate_codes_free(inflate_codes_statef *c, z_streamp z)
{
}

/* Modified by kobj - move output pointer not flushing window */
/*===========================================================================

FUNCTION LZinflate_flush 

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
int LZinflate_flush(inflate_blocks_statef *s, z_streamp z, int r)
{
  uLong n;

  n = z->next_out - s->read;
  if (n && r == Z_BUF_ERROR) r = Z_OK;
  if (s->checkfn != Z_NULL)
    z->adler = s->check = (*s->checkfn)(s->check, s->read, n);
  s->read += n;
  return r;
}

/* <EJECT> */
/*===========================================================================

FUNCTION LZinflate_fast 

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
int LZinflate_fast(uInt bl, uInt bd,
				 inflate_huft *tl, inflate_huft *td,
				 inflate_blocks_statef *s, z_streamp z)
{
  inflate_huft *t;      /* temporary pointer */
  uInt e;               /* extra bits or operation */
  uLong b;              /* bit buffer */
  uInt k;               /* bits in bit buffer */
  Byte *p;             /* input data pointer */
  uLong n;               /* bytes available there */
  Byte *q;             /* output window write pointer */
  uLong m;               /* bytes to end of window or read pointer */
  uInt ml;              /* mask for literal/length tree */
  uInt md;              /* mask for distance tree */
  uLong c;               /* bytes to copy */
  uInt d;               /* distance back to copy from */
  Byte *r;             /* copy source pointer */

  /* load input, output, bit values */
  LOAD

  /* initialize masks */
  ml = LZinflate_mask[bl];
  md = LZinflate_mask[bd];

  /* do until not enough input or output space for fast loop */
  do {                          /* assume called with m >= 258 && n >= 10 */
    /* get literal/length code */
    GRABBITS(20)                /* max bits for literal/length code */
    if ((e = (t = tl + ((uInt)b & ml))->exop) == 0)
    {
      DUMPBITS(t->bits)
      *q++ = (Byte)t->base;
      m--;
      continue;
    }
    do {
      DUMPBITS(t->bits)
      if (e & 16)
      {
        /* get extra bits for length */
        e &= 15;
        c = t->base + ((uInt)b & LZinflate_mask[e]);
        DUMPBITS(e)

        /* decode distance base of block to copy */
        GRABBITS(15);           /* max bits for distance code */
        e = (t = td + ((uInt)b & md))->exop;
        do {
          DUMPBITS(t->bits)
          if (e & 16)
          {
            /* get extra bits to add to distance base */
            e &= 15;
            GRABBITS(e)         /* get extra bits (up to 13) */
            d = t->base + ((uInt)b & LZinflate_mask[e]);
            DUMPBITS(e)

            /* do the copy */
            m -= c;
            r = q - d;
            do {                        /* copy all or what's left */
              *q++ = *r++;
            } while (--c);
            break;
          }
          else if ((e & 64) == 0)
          {
            t += t->base;
            e = (t += ((uInt)b & LZinflate_mask[e]))->exop;
          }
          else
          {
            z->msg = (char*)"invalid distance code";
            UNGRAB
            UPDATE
            return Z_DATA_ERROR;
          }
        } while (1);
        break;
      }
      if ((e & 64) == 0)
      {
        t += t->base;
        if ((e = (t += ((uInt)b & LZinflate_mask[e]))->exop) == 0)
        {
          DUMPBITS(t->bits)
          *q++ = (Byte)t->base;
          m--;
          break;
        }
      }
      else if (e & 32)
      {
        UNGRAB
        UPDATE
        return Z_STREAM_END;
      }
      else
      {
        z->msg = (char*)"invalid literal/length code";
        UNGRAB
        UPDATE
        return Z_DATA_ERROR;
      }
    } while (1);
  } while (m >= 258 && n >= 10);

  /* not enough input or output--restore pointers and return */
  UNGRAB
  UPDATE
  return Z_OK;
}


/* <EJECT> */
/*===========================================================================

FUNCTION LZdec_adler32 

DESCRIPTION
  This function controls a checksum.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
uLong LZdec_adler32(uLong adler, const Byte *buf, uLong len)
{
    unsigned long s1 = adler & 0xffff;
    unsigned long s2 = (adler >> 16) & 0xffff;
    unsigned long k;

    if (buf == Z_NULL) return 1L;

    while (len > 0) {
        k = len < NMAX ? len : NMAX;
        len -= k;
        while (k >= 16) {
            DO16(buf);
	    buf += 16;
            k -= 16;
        }
        if (k != 0) do {
            s1 += *buf++;
	    s2 += s1;
        } while (--k);
        s1 %= BASE;
        s2 %= BASE;
    }
    return (s2 << 16) | s1;
}

#endif /* FEATURE_MLZ_DECODER */
