#ifndef SMLZDECI_H
#define SMLZDECI_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               M L Z   I N T E R N A L  H E A D E R   F I L E

GENERAL DESCRIPTION
  This header file contains all the definitions necessary to interface to the
  MLZ image decoder.


Copyright(c) 2002 by NeoMtel, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/services/graph/smlzdeci.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/06/02    sa     Cleaned up code by removing unneeded #ifdefs.
09/24/02    sa     Initial version

===========================================================================*/

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  interface of the 'zlib' general purpose compression library
  version 1.1.3, July 9th, 1998

  Copyright (C) 1995-1998 Jean-loup Gailly and Mark Adler

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  Jean-loup Gailly        Mark Adler
  jloup@gzip.org          madler@alumni.caltech.edu


  The data format used by the zlib library is described by RFCs (Request for
  Comments) 1950 to 1952 in the files ftp://ds.internic.net/rfc/rfc1950.txt
  (zlib format), rfc1951.txt (LZdeflate format) and rfc1952.txt (gzip format).
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#ifdef FEATURE_MLZ_DECODER

/* Maximum value for memLevel in LZdeflateInit2 */
#define MAX_MEM_LEVEL 9

/* Maximum value for windowBits in LZdeflateInit2 and LZinflateInit2.
 * WARNING: reducing MAX_WBITS makes minigzip unable to extract .gz files
 * created by gzip. (Files created by minigzip can still be extracted by
 * gzip.)
 */
#define MAX_WBITS   15 /* 32K LZ77 window */

/* The memory requirements for LZdeflate are (in bytes):
            (1 << (windowBits+2)) +  (1 << (memLevel+9))
 that is: 128K for windowBits=15  +  128K for memLevel = 8  (default values)
 plus a few kilobytes for small objects. For example, if you want to reduce
 the default memory requirements from 256K to 128K, compile with
     make CFLAGS="-O -DMAX_WBITS=14 -DMAX_MEM_LEVEL=7"
 Of course this will generally degrade compression (there's no free lunch).

   The memory requirements for LZinflate are (in bytes) 1 << windowBits
 that is, 32K for windowBits=15 (default value) plus a few kilobytes
 for small objects.
*/
              /* Type declarations */

#define OF(args)  args

/* The following definitions for FAR are needed only for MSDOS mixed
 * model programming (small or medium model with some far allocations).
 * This was tested only with MSC; for other MSDOS compilers you may have
 * to define NO_MEMCPY in zutil.h.  If you don't need the mixed model,
 * just define FAR to be empty.
 */


typedef unsigned char  Byte;  /* 8 bits */
typedef unsigned short uInt;  /* 16 bits or more */
typedef unsigned long  uLong; /* 32 bits or more */

typedef void     *voidp;
#define  z_off_t long


#define ZLIB_VERSION "1.1.3"

typedef voidp (*alloc_func) OF((voidp opaque, uInt items, uInt size));
typedef void   (*free_func)  OF((voidp opaque, voidp address));

typedef struct z_stream_s {
    Byte    *next_in;  /* next input byte */
    uLong     avail_in;  /* number of bytes available at next_in */
    uLong    total_in;  /* total nb of input bytes read so far */

    Byte    *next_out; /* next output byte should be put there */
    uLong     avail_out; /* remaining free space at next_out */
    uLong    total_out; /* total nb of bytes output so far */

    char     *msg;      /* last error message, NULL if no error */
    struct internal_state *state; /* not visible by applications */

    uLong   adler;      /* LZdec_adler32 value of the uncompressed data */
} z_stream;

typedef z_stream *z_streamp;

                          /* constants */

#define Z_NO_FLUSH      0
#define Z_PARTIAL_FLUSH 1 /* will be removed, use Z_SYNC_FLUSH instead */
#define Z_SYNC_FLUSH    2
#define Z_FULL_FLUSH    3
#define Z_FINISH        4
/* Allowed flush values; see LZdeflate() below for details */

#define Z_OK            0
#define Z_STREAM_END    1
#define Z_NEED_DICT     2
#define Z_ERRNO        (-1)
#define Z_STREAM_ERROR (-2)
#define Z_DATA_ERROR   (-3)
#define Z_MEM_ERROR    (-4)
#define Z_BUF_ERROR    (-5)
#define Z_VERSION_ERROR (-6)
/* Return codes for the compression/decompression functions. Negative
 * values are errors, positive values are used for special but normal events.
 */

#define Z_NO_COMPRESSION         0
#define Z_BEST_SPEED             1
#define Z_BEST_COMPRESSION       9
#define Z_DEFAULT_COMPRESSION  (-1)
/* compression levels */

#define Z_FILTERED            1
#define Z_HUFFMAN_ONLY        2
#define Z_DEFAULT_STRATEGY    0
/* compression strategy; see LZdeflateInit2() below for details */

#define Z_BINARY   0
#define Z_ASCII    1
#define Z_UNKNOWN  2
/* Possible values of the data_type field */

#define Z_DEFLATED   8
/* The LZdeflate compression method (the only one supported in this version) */

#define Z_NULL  0  /* for initializing zalloc, zfree, opaque */

#define zlib_version LZzlibVersion_dec()
/* for compatibility with versions < 1.0.2 */

typedef voidp gzFile;

/* Compression scheme - defined by kobj */
#define COMPRESS_SCHEME_ZLIB	0x01
#define COMPRESS_SCHEME_SMLZ	0x02
#define COMPRESS_SCHEME_RESERVED	0x03

typedef struct inflate_huft_s inflate_huft;

#define _PACKED __packed

struct inflate_huft_s {
  _PACKED union {
    _PACKED struct {
      Byte Exop;        /* number of extra bits or operation */
      Byte Bits;        /* number of bits in this code or subcode */
    } what;
    uInt pad;           /* pad structure to a power of 2 (4 bytes for */
  } word;               /*  16-bit, 8 bytes for 32-bit int's) */
  uInt base;            /* literal, length base, distance base,
                           or table offset */
};

/* Maximum size of dynamic tree.  The maximum found in a long but non-
   exhaustive search was 1004 huft structures (850 for length/literals
   and 154 for distances, the latter actually the result of an
   exhaustive search).  The actual maximum is not known, but the
   value below is more than safe. */
#define MANY 1440

#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#define local static
/* compile with -Dlocal if your debugger can't find static symbols */

typedef unsigned char  uch;
typedef unsigned short ush;
typedef unsigned long  ulg;

extern const char *LZdec__errmsg[10]; /* indexed by 2-zlib_error */
/* (size given to avoid silly warnings with Visual C++) */

#define ERR_MSG(err) LZdec__errmsg[Z_NEED_DICT-(err)]

#define ERR_RETURN(strm,err) \
  return (strm->msg = (char*)ERR_MSG(err), (err))
/* To be used only when the state is known to be valid */

        /* common constants */

#define DEF_WBITS MAX_WBITS
/* default windowBits for decompression. MAX_WBITS is for compression only */

#define DEF_MEM_LEVEL 8

#define STORED_BLOCK 0
#define STATIC_TREES 1
#define DYN_TREES    2
/* The three kinds of block type */

#define MIN_MATCH  3
#define MAX_MATCH  258
/* The minimum and maximum match lengths */

#define PRESET_DICT 0x20 /* preset dictionary flag in zlib header */


extern void zmemcpy_dec  OF((Byte* dest, const Byte* source, uInt len));
extern int  zmemcmp_dec  OF((const Byte* s1, const Byte* s2, uInt len));
extern void zmemzero_dec OF((Byte* dest, uInt len));

typedef uLong ( *LZcheck_func) OF((uLong check, const Byte *buf,
				       uLong len));
struct inflate_codes_state;
typedef struct inflate_codes_state inflate_codes_statef;
struct inflate_blocks_state;
typedef struct inflate_blocks_state inflate_blocks_statef;

typedef enum {
      TYPE,     /* get type bits (3, including end bit) */
      LENS,     /* get lengths for stored */
      STORED,   /* processing stored block */
      TABLE,    /* get table lengths */
      BTREE,    /* get bit lengths tree for a dynamic block */
      DTREE,    /* get length, distance trees for a dynamic block */
      CODES,    /* processing fixed or dynamic block */
      DRY,      /* output remaining window bytes */
      DONE,     /* finished last block, done */
      BAD}      /* got a data error--stuck here */
inflate_block_mode;

/* LZinflate blocks semi-private state */
struct inflate_blocks_state {

  /* mode */
  inflate_block_mode  mode;     /* current inflate_block mode */

  /* mode dependent information */
  union {
    uInt left;          /* if STORED, bytes left to copy */
    struct {
      uInt table;               /* table lengths (14 bits) */
      uInt index;               /* index into blens (or border) */
      uInt *blens;             /* bit lengths of codes */
      uInt bb;                  /* bit length tree depth */
      inflate_huft *tb;         /* bit length decoding tree */
    } trees;            /* if DTREE, decoding info for trees */
    struct {
      inflate_codes_statef 
         *codes;
    } decode;           /* if CODES, current state */
  } sub;                /* submode */
  uInt last;            /* true if this block is the last block */

  /* mode independent information */
  uInt bitk;            /* bits in bit buffer */
  uLong bitb;           /* bit buffer */
  inflate_huft *hufts;  /* single malloc for tree space */
  Byte *window;        /* sliding window */
//  Byte *end;           /* one byte after sliding window */
  Byte *read;          /* window read pointer */
  Byte *write;         /* window write pointer */
  LZcheck_func checkfn;   /* check function */
  uLong check;          /* check on output */

};


/* defines for LZinflate input/output - modified by kobj */

/*   update pointers and return */
#define UPDBITS {s->bitb=b;s->bitk=k;}
#define UPDIN {z->avail_in=n;z->total_in+=p-z->next_in;z->next_in=p;}
#define UPDOUT {z->avail_out=m;z->total_out+=q-z->next_out;z->next_out=q;}
#define UPDATE {UPDBITS UPDIN UPDOUT}
#define LEAVE {UPDATE return r;}
/*   load local pointers */
#define LOADBITS {b=s->bitb;k=s->bitk;}
#define LOADIN {p=z->next_in;n=z->avail_in;}
#define LOADOUT {q=z->next_out;m=z->avail_out;}
#define LOAD {LOADBITS LOADIN LOADOUT}

#define NEEDBYTE {if(n)r=Z_OK;else LEAVE}
#define NEXTBYTE (n--,*p++)
#define NEEDBITS(j) {while(k<(j)){NEEDBYTE;b|=((uLong)NEXTBYTE)<<k;k+=8;}}
#define DUMPBITS(j) {b>>=(j);k-=(j);}

/*   output bytes */
#define NEEDOUT {if(m)r=Z_OK;else LEAVE}
#define OUTBYTE(a) {*q++=(Byte)(a);m--;}

#define FLUSH {UPDOUT r=LZinflate_flush(s,z,r); LOADOUT}





                        /* basic functions */

extern const char *  LZzlibVersion_dec OF((void));

extern int  LZdeflate OF((z_streamp strm, int flush));

extern int  LZdeflateEnd OF((z_streamp strm));

extern int  LZinflate OF((z_streamp strm, int flush, int scheme));

extern int  LZinflateEnd OF((z_streamp strm));

                        /* Advanced functions */

extern int  LZdeflateSetDictionary OF((z_streamp strm,
                                             const Byte *dictionary,
                                             uInt  dictLength));
extern int  LZdeflateCopy OF((z_streamp dest,
                                    z_streamp source));

extern int  LZdeflateReset OF((z_streamp strm));

extern int  LZdeflateParams OF((z_streamp strm,
				      int level,
				      int strategy));

extern int  LZinflateSetDictionary OF((z_streamp strm,
                                             const Byte *dictionary,
                                             uInt  dictLength));

extern int  LZinflateSync OF((z_streamp strm));

extern int  LZinflateReset OF((z_streamp strm));

                        /* utility functions */

extern int  LZcompress OF((Byte *dest,   uLong *destLen,
                                 const Byte *source, uLong sourceLen));

extern int  LZcompress2 OF((Byte *dest,   uLong *destLen,
                                  const Byte *source, uLong sourceLen,
                                  int level));

extern int  LZuncompress OF((Byte *dest,   uLong *destLen,
                                   const Byte *source, uLong sourceLen));

extern gzFile  gzopen  OF((const char *path, const char *mode));

extern gzFile  gzdopen  OF((int fd, const char *mode));

extern int  gzsetparams OF((gzFile file, int level, int strategy));

extern int     gzread  OF((gzFile file, voidp buf, unsigned len));

extern int     gzwrite OF((gzFile file, 
				   const voidp buf, unsigned len));

extern int    gzprintf OF((gzFile file, const char *format, ...));

extern int  gzputs OF((gzFile file, const char *s));

extern char *  gzgets OF((gzFile file, char *buf, int len));

extern int     gzputc OF((gzFile file, int c));

extern int     gzgetc OF((gzFile file));

extern int     gzflush OF((gzFile file, int flush));

extern z_off_t     gzseek OF((gzFile file,
				      z_off_t offset, int whence));

extern int     gzrewind OF((gzFile file));

extern z_off_t     gztell OF((gzFile file));

extern int  gzeof OF((gzFile file));

extern int     gzclose OF((gzFile file));

extern const char *  gzerror OF((gzFile file, int *errnum));

                        /* checksum functions */

extern uLong  LZdec_adler32 OF((uLong adler, const Byte *buf, uLong len));

extern uLong  LZdec_crc32   OF((uLong crc, const Byte *buf, uInt len));

extern int  LZdeflateInit_ OF((z_streamp strm, int level,
                                     const char *version, int stream_size));
extern int  LZinflateInit_ OF((z_streamp strm,
                                     const char *version, int stream_size));
extern int  LZdeflateInit2_ OF((z_streamp strm, int  level, int  method,
                                      int windowBits, int memLevel,
                                      int strategy, const char *version,
                                      int stream_size));
extern int  LZinflateInit2_ OF((z_streamp strm, int  windowBits,
                                      const char *version, int stream_size));
#define LZdeflateInit(strm, level) \
        LZdeflateInit_((strm), (level),       ZLIB_VERSION, sizeof(z_stream))
#define LZinflateInit(strm) \
        LZinflateInit_((strm),                ZLIB_VERSION, sizeof(z_stream))
#define LZdeflateInit2(strm, level, method, windowBits, memLevel, strategy) \
        LZdeflateInit2_((strm),(level),(method),(windowBits),(memLevel),\
                      (strategy),           ZLIB_VERSION, sizeof(z_stream))
#define LZinflateInit2(strm, windowBits) \
        LZinflateInit2_((strm), (windowBits), ZLIB_VERSION, sizeof(z_stream))


extern const char   *  LZdec_zError           OF((int err));
extern const uLong *  LZdec_get_crc_table    OF((void));

extern int LZinflate_trees_bits OF((
    uInt *,                    /* 19 code lengths */
    uInt *,                    /* bits tree desired/actual depth */
    inflate_huft * *,       /* bits tree result */
    inflate_huft *,             /* space for trees */
    z_streamp));                /* for messages */

extern int LZinflate_trees_dynamic OF((
    uInt,                       /* number of literal/length codes */
    uInt,                       /* number of distance codes */
    uInt *,                    /* that many (total) code lengths */
    uInt *,                    /* literal desired/actual bit depth */
    uInt *,                    /* distance desired/actual bit depth */
    inflate_huft * *,       /* literal/length tree result */
    inflate_huft * *,       /* distance tree result */
    inflate_huft *,             /* space for trees */
    z_streamp));                /* for messages */

extern int LZinflate_trees_fixed OF((
    uInt *,                    /* literal desired/actual bit depth */
    uInt *,                    /* distance desired/actual bit depth */
    inflate_huft * *,       /* literal/length tree result */
    inflate_huft * *,       /* distance tree result */
    z_streamp));                /* for memory allocation */


extern inflate_blocks_statef * LZinflate_blocks_new OF((
    z_streamp z,
    LZcheck_func c,               /* check function */
    uInt w));                   /* window size */

extern int LZinflate_blocks OF((
    inflate_blocks_statef *,
    z_streamp ,
    int,
	int));                      /* initial return code */

extern void LZinflate_blocks_reset OF((
    inflate_blocks_statef *,
    z_streamp ,
    uLong *));                  /* check value on output */

extern int LZinflate_blocks_free OF((
    inflate_blocks_statef *,
    z_streamp));


/* Functions Added by kobj */
extern LZinflate_huff_block OF((
	inflate_blocks_statef *,
	z_streamp));

extern int LZinflate_fast OF((
    uInt,
    uInt,
    inflate_huft *,
    inflate_huft *,
    inflate_blocks_statef *,
    z_streamp ));


extern inflate_codes_statef *LZinflate_codes_new OF((
    uInt, uInt,
    inflate_huft *, inflate_huft *,
    z_streamp ));

extern int LZinflate_codes OF((
    inflate_blocks_statef *,
    z_streamp ,
    int));

extern void LZinflate_codes_free OF((
    inflate_codes_statef *,
    z_streamp ));

extern int LZinflate_flush OF((
    inflate_blocks_statef *,
    z_streamp ,
    int));

extern int LZinflate_fast OF((
    uInt,
    uInt,
    inflate_huft *,
    inflate_huft *,
    inflate_blocks_statef *,
    z_streamp ));

typedef enum {
      METHOD,   /* waiting for method byte */
      FLAG,     /* waiting for flag byte */
      DICT4,    /* four dictionary check bytes to go */
      DICT3,    /* three dictionary check bytes to go */
      DICT2,    /* two dictionary check bytes to go */
      DICT1,    /* one dictionary check byte to go */
      DICT0,    /* waiting for LZinflateSetDictionary */
      BLOCKS,   /* decompressing blocks */
      CHECK4,   /* four check bytes to go */
      CHECK3,   /* three check bytes to go */
      CHECK2,   /* two check bytes to go */
      CHECK1,   /* one check byte to go */
      DONE2,     /* finished check, done */
      BAD2}      /* got an error--stay here */
inflate_mode;

/* LZinflate private state */
struct internal_state {

  /* mode */
  inflate_mode  mode;   /* current LZinflate mode */

  /* mode dependent information */
  union {
    uInt method;        /* if FLAGS, method byte */
    struct {
      uLong was;                /* computed check value */
      uLong need;               /* stream check value */
    } check;            /* if CHECK, check values to compare */
  } sub;        /* submode */

  /* mode independent information */
  int  nowrap;          /* flag for no wrapper */
  uInt wbits;           /* log2(window size)  (8..15, defaults to 15) */
  inflate_blocks_statef 
    *blocks;            /* current LZinflate_blocks state */

};
/* Global dynamic huffman tree - kobj */
typedef struct{
	inflate_huft* hufts;  /* MANY entry */	
	//inflate_huft hufts[MANY];  /* MANY entry */	
	uInt bl;
	uInt bd;
	inflate_huft *tl;
	inflate_huft *td;
}huff_tree_s;

typedef enum {        /* waiting for "i:"=input, "o:"=output, "x:"=nothing */
      START,    /* x: set up for LEN */
      LEN,      /* i: get length/literal/eob next */
      LENEXT,   /* i: getting length extra (have base) */
      DIST,     /* i: get distance next */
      DISTEXT,  /* i: getting distance extra */
      COPY,     /* o: copying bytes in window, waiting for space */
      LIT,      /* o: got literal, waiting for output space */
      WASH,     /* o: got eob, possibly still output waiting */
      END,      /* x: got eob and all data flushed */
      BADCODE}  /* x: got error */
inflate_codes_mode;

/* LZinflate codes private state */
struct inflate_codes_state {

  /* mode */
  inflate_codes_mode mode;      /* current LZinflate_codes mode */

  /* mode dependent information */
  uInt len;
  union {
    struct {
      inflate_huft *tree;       /* pointer into tree */
      uInt need;                /* bits needed */
    } code;             /* if LEN or DIST, where in tree */
    uInt lit;           /* if LIT, literal */
    struct {
      uInt get;                 /* bits to get for extra */
      uInt dist;                /* distance back to copy from */
    } copy;             /* if EXT or COPY, where and how much */
  } sub;                /* submode */

  /* mode independent information */
  Byte lbits;           /* ltree bits decoded per branch */
  Byte dbits;           /* dtree bits decoder per branch */
  inflate_huft *ltree;          /* literal/length/eob tree */
  inflate_huft *dtree;          /* distance tree */

};

#endif /* FEATURE_MLZ_DECODER */
#endif /* SMLZDECI_H          */
