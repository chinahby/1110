/* linesize(132)
** pagesize(60)
** title("Dual Mode Subscriber Station")
** subtitle("PNG Encoder")
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

          P O R T I B L E    N E T W O R K    G R A P H I C S   (PNG)
                      D E F L A T E 

GENERAL DESCRIPTION

EXTERNAL FUNCTIONS

Copyright(c) 2003 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/* deflate.h -- internal compression state
 * Copyright (C) 1995-2002 Jean-loup Gailly
 * For conditions of distribution and use, see copyright notice in zlib.h 
 */

/* WARNING: this file should *not* be used by applications. It is
   part of the implementation of the compression library and is
   subject to change. Applications should only use zlib.h.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/services/graph/pngdeflate.h#5 $



when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/16/05   kln     Fixed lint errors
07/01/04   mvd     Added "pnge" prefix to all functions, struct, local,
                   and extern global variables.
05/12/04   mvd     Removed code defines: DEFLATE_DEBUG, _ZLIB_H
11/07/03   mvd     Initial version

===========================================================================*/

#ifndef PNGDEFLATE_H
#define PNGDEFLATE_H
#if defined(FEATURE_PNG_ENCODER)

#include "pngglobal.h"




/* ===========================================================================
 * Internal compression state.
 */

#define LENGTH_CODES 29
/* number of length codes, not counting the special END_BLOCK code */

#define LITERALS  256
/* number of literal bytes 0..255 */

#define L_CODES (LITERALS+1+LENGTH_CODES)
/* number of Literal or Length codes, including the END_BLOCK code */

#define D_CODES   30
/* number of distance codes */

#define BL_CODES  19
/* number of codes used to transfer the bit lengths */

#define HEAP_SIZE (2*L_CODES+1)
/* maximum heap size */

#define MAX_BITS 15
/* All codes must not exceed MAX_BITS bits */

#define INIT_STATE    42
#define BUSY_STATE   113
#define FINISH_STATE 666
/* Stream status */


/* ===========================================================================
 * Internal compression config.
 */

/* Maximum value for memLevel in deflateInit2 */
#define MAX_MEM_LEVEL 9

/* The minimum and maximum match lengths */
#define MIN_MATCH  3
#define MAX_MATCH  258

#define PRESET_DICT 0x20 /* preset dictionary flag in zlib header */


/* Data structure describing a single value and its code string. */
typedef struct pnge_ct_data_s
{
   union
   {
      uint16  freq;       /* frequency count */
      uint16  code;       /* bit string */
   } fc;
   union
   {
      uint16  dad;        /* father node in Huffman tree */
      uint16  len;        /* length of bit string */
   } dl;
}  pnge_ct_data;




#define Freq fc.freq
#define Code fc.code
#define Dad  dl.dad
#define Len  dl.len

typedef struct pnge_static_tree_desc_s  pnge_static_tree_desc;

typedef struct pnge_tree_desc_s
{
   pnge_ct_data *dyn_tree;           /* the dynamic tree */
   uint32     max_code;            /* largest code with non zero frequency */
   pnge_static_tree_desc *stat_desc; /* the corresponding static tree */
}  pnge_tree_desc;

typedef unsigned pnge_IPos;



/*
   The application must update next_in and avail_in when avail_in has
   dropped to zero. It must update next_out and avail_out when avail_out
   has dropped to zero. The application must initialize zalloc, zfree and
   opaque before calling the init function. All other fields are set by the
   compression library and must not be updated by the application.

   The opaque value provided by the application will be passed as the first
   parameter for calls of zalloc and zfree. This can be useful for custom
   memory management. The compression library attaches no meaning to the
   opaque value.

   zalloc must return Z_NULL if there is not enough memory for the object.
   If zlib is used in a multi-threaded application, zalloc and zfree must be
   thread safe.

   On 16-bit systems, the functions zalloc and zfree must be able to allocate
   exactly 65536 bytes, but will not be required to allocate more than this
   if the symbol MAXSEG_64K is defined (see zconf.h). WARNING: On MSDOS,
   pointers returned by zalloc for objects of exactly 65536 bytes *must*
   have their offset normalized to zero. The default allocation function
   provided by this library ensures this (see zutil.c). To reduce memory
   requirements and avoid any allocation of 64K objects, at the expense of
   compression ratio, compile the library with -DMAX_WBITS=14 (see zconf.h).

   The fields total_in and total_out can be used for statistics or
   progress reports. After compression, total_in holds the total size of
   the uncompressed data and may be saved for use in the decompressor
   (particularly if the decompressor wants to decompress everything in
   a single step).
*/

typedef struct pnge_z_stream_s
{
   uint8    *next_in;  /* next input byte */
   uint32     avail_in;  /* number of bytes available at next_in */
   uint32    total_in;  /* total nb of input bytes read so far */

   uint8    *next_out; /* next output byte should be put there */
   uint32     avail_out; /* remaining free space at next_out */
   uint32    total_out; /* total nb of bytes output so far */

   const char     *msg;      /* last error message, NULL if no error */
   struct pnge_internal_state *state; /* not visible by applications */
   //struct pnge_internal_state FAR *state; /* not visible by applications */

   int32     data_type;  /* best guess about the data type: ascii or binary */
   uint32   adler;      /* adler32 value of the uncompressed data */
   uint32   reserved;   /* reserved for future use */
} pnge_z_stream;



/* A Pos (uint16) is an index in the character window. We use short instead of int32 to
 * save space in the various tables. pnge_IPos is used only for parameter passing.
 */

typedef struct pnge_internal_state
{
   pnge_z_stream * strm;      /* pointer back to this zlib stream */
   int32   status;        /* as the name implies */
   uint8 *pending_buf;  /* output still pending */
   uint32   pending_buf_size; /* size of pending_buf */
   uint8 *pending_out;  /* next pending byte to output to the stream */
   uint32   pending;       /* nb of bytes in the pending buffer */
   int32   noheader;      /* suppress zlib header and adler32 */
   uint8  data_type;     /* UNKNOWN, BINARY or ASCII */
   uint8  method;        /* STORED (for zip only) or DEFLATED */
   int32   last_flush;    /* value of flush param for previous deflate call */

   /* used by deflate.c: */

   uint32  w_size;        /* LZ77 window size (32K by default) */
   uint32  w_bits;        /* log2(w_size)  (8..16) */
   uint32  w_mask;        /* w_size - 1 */

   uint8 *window;
   /* Sliding window. Input bytes are read into the second half of the window,
    * and move to the first half later to keep a dictionary of at least wSize
    * bytes. With this organization, matches are limited to a distance of
    * wSize-MAX_MATCH bytes, but this ensures that IO is always
    * performed with a length multiple of the block size. Also, it limits
    * the window size to 64K, which is quite useful on MSDOS.
    * To do: use the user input buffer as sliding window.
    */

   uint32 window_size;
   /* Actual size of window: 2*wSize, except when the user input buffer
    * is directly used as sliding window.
    */

   uint16 *prev;
   /* Link to older string with same hash index. To limit the size of this
    * array to 64K, this link is maintained only for the last 32K strings.
    * An index in this array is thus a window index modulo 32K.
    */

   uint16 *head; /* Heads of the hash chains or 0. */

   uint32  ins_h;          /* hash index of string to be inserted */
   uint32  hash_size;      /* number of elements in hash table */
   uint32  hash_bits;      /* log2(hash_size) */
   uint32  hash_mask;      /* hash_size-1 */

   uint32  hash_shift;
   /* Number of bits by which ins_h must be shifted at each input
    * step. It must be such that after MIN_MATCH steps, the oldest
    * byte no longer takes part in the hash key, that is:
    *   hash_shift * MIN_MATCH >= hash_bits
    */

   long block_start;
   /* Window position at the beginning of the current output block. Gets
    * negative when the window is moved backwards.
    */

   uint32 match_length;           /* length of best match */

   pnge_IPos prev_match;          /* previous match */
   int32 match_available;         /* set if previous match exists */
   uint32 strstart;               /* start of string to insert */
   uint32 match_start;            /* start of matching string */
   uint32 lookahead;              /* number of valid bytes ahead in window */

   uint32 prev_length;
   /* Length of the best match at previous step. Matches not greater than this
    * are discarded. This is used in the lazy match evaluation.
    */

   uint32 max_chain_length;
   /* To speed up deflation, hash chains are never searched beyond this
    * length.  A higher limit improves compression ratio but degrades the
    * speed.
    */

   uint32 max_lazy_match;
   /* Attempt to find a better match only when the current match is strictly
    * smaller than this value. This mechanism is used only for compression
    * levels >= 4.
    */
#define max_insert_length  max_lazy_match

   /* Insert new strings in the hash table only if the match length is not
    * greater than this length. This saves time but degrades compression.
    * max_insert_length is used only for compression levels <= 3.
    */

   int32 level;    /* compression level (1..9) */
   int32 strategy; /* favor or force Huffman coding*/

   uint32 good_match;
   /* Use a faster search when the previous match is longer than this */

   int32 nice_match; /* Stop searching when current match exceeds this */

   /* used by trees.c: */
   /* Didn't use ct_data typedef below to supress compiler warning */
   struct pnge_ct_data_s dyn_ltree[HEAP_SIZE];   /* literal and length tree */
   struct pnge_ct_data_s dyn_dtree[2*D_CODES+1]; /* distance tree */
   struct pnge_ct_data_s bl_tree[2*BL_CODES+1];  /* Huffman tree for bit lengths */

   struct pnge_tree_desc_s l_desc;               /* desc. for literal tree */
   struct pnge_tree_desc_s d_desc;               /* desc. for distance tree */
   struct pnge_tree_desc_s bl_desc;              /* desc. for bit length tree */

   uint16 bl_count[MAX_BITS+1];
   /* number of codes at each bit length for an optimal tree */

   uint32 heap[2*L_CODES+1];      /* heap used to build the Huffman trees */
   uint32 heap_len;               /* number of elements in the heap */
   uint32 heap_max;               /* element of largest frequency */
   /* The sons of heap[n] are heap[2*n] and heap[2*n+1]. heap[0] is not used.
    * The same heap array is used to build all trees.
    */

   uint8 depth[2*L_CODES+1];
   /* Depth of each subtree used as tie breaker for trees of equal frequency
    */

   uint8 *l_buf;          /* buffer for literals or lengths */

   uint32  lit_bufsize;
   /* Size of match buffer for literals/lengths.  There are 4 reasons for
    * limiting lit_bufsize to 64K:
    *   - frequencies can be kept in 16 bit counters
    *   - if compression is not successful for the first block, all input
    *     data is still in the window so we can still emit a stored block even
    *     when input comes from standard input.  (This can also be done for
    *     all blocks if lit_bufsize is not greater than 32K.)
    *   - if compression is not successful for a file smaller than 64K, we can
    *     even emit a stored file instead of a stored block (saving 5 bytes).
    *     This is applicable only for zip (not gzip or zlib).
    *   - creating new Huffman trees less frequently may not provide fast
    *     adaptation to changes in the input data statistics. (Take for
    *     example a binary file with poorly compressible code followed by
    *     a highly compressible string table.) Smaller buffer sizes give
    *     fast adaptation but have of course the overhead of transmitting
    *     trees more frequently.
    *   - I can't count above 4
    */

   uint32 last_lit;      /* running index in l_buf */

   uint16 *d_buf;
   /* Buffer for distances. To simplify the code, d_buf and l_buf have
    * the same number of elements. To use different lengths, an extra flag
    * array would be necessary.
    */

   uint32 opt_len;        /* bit length of current block with optimal trees */
   uint32 static_len;     /* bit length of current block with static trees */
   uint32 matches;       /* number of string matches in current block */
   int32 last_eob_len;   /* bit length of EOB code for last block */

   uint16 bi_buf;
   /* Output buffer. bits are inserted starting at the bottom (least
    * significant bits).
    */
   uint16 bi_valid;
   /* Number of valid bits in bi_buf.  All bits above the last valid bit
    * are always zero.
    */

} pnge_deflate_state;



/* Output a byte on the stream.
 * IN assertion: there is enough room in pending_buf.
 */
#define put_byte(s, c) {s->pending_buf[s->pending++] = (c);}


#define MIN_LOOKAHEAD (MAX_MATCH+MIN_MATCH+1)

/* Minimum amount of lookahead, except at the end of the input file.
 * See deflate.c for comments about the MIN_MATCH+1.
 */

#define MAX_DIST(s)  (uint32)((s)->w_size-MIN_LOOKAHEAD)

/* In order to simplify the code, particularly on 16 bit machines, match
 * distances are limited to MAX_DIST instead of WSIZE.
 */

/* in trees.c */
void pnge_tr_init( pnge_deflate_state *s);
int32  pnge_tr_tally( pnge_deflate_state *s, unsigned dist, unsigned lc);
void pnge_tr_flush_block(pnge_deflate_state *s, char *buf, uint32 stored_len,
                    int32 eof);
void pnge_tr_align(pnge_deflate_state *s);
void pnge_tr_stored_block(pnge_deflate_state *s, char *buf, uint32 stored_len,
                     int32 eof);

#define d_code(dist) \
   ((dist) < 256 ? pnge_dist_code[dist] : pnge_dist_code[256+((dist)>>7)])

/* Mapping from a distance to a distance code. dist is the distance - 1 and
 * must not have side effects. pnge_dist_code[256] and pnge_dist_code[257] are never
 * used.
 */

/* Inline versions of _tr_tally for speed: */

extern const uint8 pnge_length_code[];
extern const uint8 pnge_dist_code[];

#define pnge_tr_tally_lit(s, c, flush) \
  { uint8 cc = (c); \
    s->d_buf[s->last_lit] = 0; \
    s->l_buf[s->last_lit++] = cc; \
    s->dyn_ltree[cc].Freq++; \
    flush = (s->last_lit == s->lit_bufsize-1); \
   }
#define pnge_tr_tally_dist(s, distance, length, flush) \
  { uint8 len = (length); \
    uint16 dist = (distance); \
    s->d_buf[s->last_lit] = dist; \
    s->l_buf[s->last_lit++] = len; \
    dist--; \
    s->dyn_ltree[pnge_length_code[len]+LITERALS+1].Freq++; \
    s->dyn_dtree[d_code(dist)].Freq++; \
    flush = (s->last_lit == s->lit_bufsize-1); \
  }




/* zlib.h -- interface of the 'zlib' general purpose compression library
version 1.1.4, March 11th, 2002

Copyright (C) 1995-2002 Jean-loup Gailly and Mark Adler

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
(zlib format), rfc1951.txt (deflate format) and rfc1952.txt (gzip format).
*/




#define ZLIB_VERSION "1.1.4"

/* 
     The 'zlib' compression library provides in-memory compression and
  decompression functions, including integrity checks of the uncompressed
  data.  This version of the library supports only one compression method
  (deflation) but other algorithms will be added later and will have the same
  stream interface.

     Compression can be done in a single step if the buffers are large
  enough (for example if an input file is mmap'ed), or can be done by
  repeated calls of the compression function.  In the latter case, the
  application must provide more input and/or consume the output
  (providing more output space) before each call.

     The library also supports reading and writing files in gzip (.gz) format
  with an interface similar to that of stdio.

     The library does not install any signal handler. The decoder checks
  the consistency of the compressed data, so the library should never
  crash even in case of corrupted input.
*/







/* constants */

#define Z_NO_FLUSH      0
#define Z_PARTIAL_FLUSH 1 /* will be removed, use Z_SYNC_FLUSH instead */
#define Z_SYNC_FLUSH    2
#define Z_FULL_FLUSH    3
#define Z_FINISH        4

/* Allowed flush values; see deflate() below for details */

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

/* compression strategy; see deflateInit2() below for details */

#define Z_BINARY   0
#define Z_ASCII    1
#define Z_UNKNOWN  2

/* Possible values of the data_type field */

#define Z_DEFLATED   8

/* The deflate compression method (the only one supported in this version) */

#define Z_NULL  0  /* for initializing zalloc, zfree, opaque */

#define zlib_version pnge_zlibVersion()

/* for compatibility with versions < 1.0.2 */

/* basic functions */

/* The application can compare zlibVersion and ZLIB_VERSION for consistency.
   If the first character differs, the library code actually used is
   not compatible with the zlib.h header file used by the application.
   This check is automatically made by deflateInit and inflateInit.
 */
extern const char *  pnge_zlibVersion (void);




/* 
     Initializes the internal stream state for compression. The fields
   zalloc, zfree and opaque must be initialized before by the caller.
   If zalloc and zfree are set to Z_NULL, deflateInit updates them to
   use default allocation functions.

     The compression level must be Z_DEFAULT_COMPRESSION, or between 0 and 9:
   1 gives best speed, 9 gives best compression, 0 gives no compression at
   all (the input data is simply copied a block at a time).
   Z_DEFAULT_COMPRESSION requests a default compromise between speed and
   compression (currently equivalent to level 6).

     deflateInit returns Z_OK if success, Z_MEM_ERROR if there was not
   enough memory, Z_STREAM_ERROR if level is not a valid compression level,
   Z_VERSION_ERROR if the zlib library version (zlib_version) is incompatible
   with the version assumed by the caller (ZLIB_VERSION).
   msg is set to null if there is no error message.  deflateInit does not
   perform any compression: this will be done by deflate().
extern int32  pnge_deflateInit (pnge_z_stream * strm, int32 level);
*/


/*
    deflate compresses as much data as possible, and stops when the input
  buffer becomes empty or the output buffer becomes full. It may introduce some
  output latency (reading input without producing any output) except when
  forced to flush.

    The detailed semantics are as follows. deflate performs one or both of the
  following actions:

  - Compress more input starting at next_in and update next_in and avail_in
    accordingly. If not all input can be processed (because there is not
    enough room in the output buffer), next_in and avail_in are updated and
    processing will resume at this point for the next call of deflate().

  - Provide more output starting at next_out and update next_out and avail_out
    accordingly. This action is forced if the parameter flush is non zero.
    Forcing flush frequently degrades the compression ratio, so this parameter
    should be set only when necessary (in interactive applications).
    Some output may be provided even if flush is not set.

  Before the call of deflate(), the application should ensure that at least
  one of the actions is possible, by providing more input and/or consuming
  more output, and updating avail_in or avail_out accordingly; avail_out
  should never be zero before the call. The application can consume the
  compressed output when it wants, for example when the output buffer is full
  (avail_out == 0), or after each call of deflate(). If deflate returns Z_OK
  and with zero avail_out, it must be called again after making room in the
  output buffer because there might be more output pending.

    If the parameter flush is set to Z_SYNC_FLUSH, all pending output is
  flushed to the output buffer and the output is aligned on a byte boundary, so
  that the decompressor can get all input data available so far. (In particular
  avail_in is zero after the call if enough output space has been provided
  before the call.)  Flushing may degrade compression for some compression
  algorithms and so it should be used only when necessary.

    If flush is set to Z_FULL_FLUSH, all output is flushed as with
  Z_SYNC_FLUSH, and the compression state is reset so that decompression can
  restart from this point if previous compressed data has been damaged or if
  random access is desired. Using Z_FULL_FLUSH too often can seriously degrade
  the compression.

    If deflate returns with avail_out == 0, this function must be called again
  with the same value of the flush parameter and more output space (updated
  avail_out), until the flush is complete (deflate returns with non-zero
  avail_out).

    If the parameter flush is set to Z_FINISH, pending input is processed,
  pending output is flushed and deflate returns with Z_STREAM_END if there
  was enough output space; if deflate returns with Z_OK, this function must be
  called again with Z_FINISH and more output space (updated avail_out) but no
  more input data, until it returns with Z_STREAM_END or an error. After
  deflate has returned Z_STREAM_END, the only possible operations on the
  stream are deflateReset or deflateEnd.
  
    Z_FINISH can be used immediately after deflateInit if all the compression
  is to be done in a single step. In this case, avail_out must be at least
  0.1% larger than avail_in plus 12 bytes.  If deflate does not return
  Z_STREAM_END, then it must be called again as described above.

    deflate() sets strm->adler to the adler32 checksum of all input read
  so far (that is, total_in bytes).

    deflate() may update data_type if it can make a good guess about
  the input data type (Z_ASCII or Z_BINARY). In doubt, the data is considered
  binary. This field is only for information purposes and does not affect
  the compression algorithm in any manner.

    deflate() returns Z_OK if some progress has been made (more input
  processed or more output produced), Z_STREAM_END if all input has been
  consumed and all output has been produced (only when flush is set to
  Z_FINISH), Z_STREAM_ERROR if the stream state was inconsistent (for example
  if next_in or next_out was NULL), Z_BUF_ERROR if no progress is possible
  (for example avail_in or avail_out was zero).
*/
extern int32  pnge_deflate (pnge_z_stream * strm, int32 flush);





/*
     All dynamically allocated data structures for this stream are freed.
   This function discards any unprocessed input and does not flush any
   pending output.

     deflateEnd returns Z_OK if success, Z_STREAM_ERROR if the
   stream state was inconsistent, Z_DATA_ERROR if the stream was freed
   prematurely (some input or output was discarded). In the error case,
   msg may be set but then points to a static string (which must not be
   deallocated).
*/
extern int32  pnge_deflateEnd (pnge_z_stream * strm);



/*
     All dynamically allocated data structures for this stream are freed.
   This function discards any unprocessed input and does not flush any
   pending output.

     inflateEnd returns Z_OK if success, Z_STREAM_ERROR if the stream state
   was inconsistent. In the error case, msg may be set but then points to a
   static string (which must not be deallocated).
*/
extern int32  pnge_inflateEnd (pnge_z_stream * strm);

/* Advanced functions */

/*
    The following functions are needed only in some special applications.
*/

/*   
extern int32  pnge_deflateInit2 (pnge_z_stream * strm,
                                     int32  level,
                                     int32  method,
                                     int32  windowBits,
                                     int32  memLevel,
                                     int32  strategy);

     This is another version of deflateInit with more compression options. The
   fields next_in, zalloc, zfree and opaque must be initialized before by
   the caller.

     The method parameter is the compression method. It must be Z_DEFLATED in
   this version of the library.

     The windowBits parameter is the base two logarithm of the window size
   (the size of the history buffer).  It should be in the range 8..15 for this
   version of the library. Larger values of this parameter result in better
   compression at the expense of memory usage. The default value is 15 if
   deflateInit is used instead.

     The memLevel parameter specifies how much memory should be allocated
   for the internal compression state. memLevel=1 uses minimum memory but
   is slow and reduces compression ratio; memLevel=9 uses maximum memory
   for optimal speed. The default value is 8. See zconf.h for total memory
   usage as a function of windowBits and memLevel.

     The strategy parameter is used to tune the compression algorithm. Use the
   value Z_DEFAULT_STRATEGY for normal data, Z_FILTERED for data produced by a
   filter (or predictor), or Z_HUFFMAN_ONLY to force Huffman encoding only (no
   string match).  Filtered data consists mostly of small values with a
   somewhat random distribution. In this case, the compression algorithm is
   tuned to compress them better. The effect of Z_FILTERED is to force more
   Huffman coding and less string matching; it is somewhat intermediate
   between Z_DEFAULT and Z_HUFFMAN_ONLY. The strategy parameter only affects
   the compression ratio but not the correctness of the compressed output even
   if it is not set appropriately.

      deflateInit2 returns Z_OK if success, Z_MEM_ERROR if there was not enough
   memory, Z_STREAM_ERROR if a parameter is invalid (such as an invalid
   method). msg is set to null if there is no error message.  deflateInit2 does
   not perform any compression: this will be done by deflate().
*/

extern int32  pnge_deflateSetDictionary (pnge_z_stream * strm,
                                    const uint8 *dictionary,
                                    uint32  dictLength);
/*
     Initializes the compression dictionary from the given byte sequence
   without producing any compressed output. This function must be called
   immediately after deflateInit, deflateInit2 or deflateReset, before any
   call of deflate. The compressor and decompressor must use exactly the same
   dictionary (see inflateSetDictionary).

     The dictionary should consist of strings (byte sequences) that are likely
   to be encountered later in the data to be compressed, with the most commonly
   used strings preferably put towards the end of the dictionary. Using a
   dictionary is most useful when the data to be compressed is short and can be
   predicted with good accuracy; the data can then be compressed better than
   with the default empty dictionary.

     Depending on the size of the compression data structures selected by
   deflateInit or deflateInit2, a part of the dictionary may in effect be
   discarded, for example if the dictionary is larger than the window size in
   deflate or deflate2. Thus the strings most likely to be useful should be
   put at the end of the dictionary, not at the front.

     Upon return of this function, strm->adler is set to the Adler32 value
   of the dictionary; the decompressor may later use this value to determine
   which dictionary has been used by the compressor. (The Adler32 value
   applies to the whole dictionary even if only a subset of the dictionary is
   actually used by the compressor.)

     deflateSetDictionary returns Z_OK if success, or Z_STREAM_ERROR if a
   parameter is invalid (such as NULL dictionary) or the stream state is
   inconsistent (for example if deflate has already been called for this stream
   or if the compression method is bsort). deflateSetDictionary does not
   perform any compression: this will be done by deflate().
*/

extern int32  pnge_deflateCopy (pnge_z_stream * dest,
                           pnge_z_stream * source);
/*
     Sets the destination stream as a complete copy of the source stream.

     This function can be useful when several compression strategies will be
   tried, for example when there are several ways of pre-processing the input
   data with a filter. The streams that will be discarded should then be freed
   by calling deflateEnd.  Note that deflateCopy duplicates the internal
   compression state which can be quite large, so this strategy is slow and
   can consume lots of memory.

     deflateCopy returns Z_OK if success, Z_MEM_ERROR if there was not
   enough memory, Z_STREAM_ERROR if the source stream state was inconsistent
   (such as zalloc being NULL). msg is left unchanged in both source and
   destination.
*/

extern int32  pnge_deflateReset (pnge_z_stream * strm);
/*
     This function is equivalent to deflateEnd followed by deflateInit,
   but does not free and reallocate all the internal compression state.
   The stream will keep the same compression level and any other attributes
   that may have been set by deflateInit2.

      deflateReset returns Z_OK if success, or Z_STREAM_ERROR if the source
   stream state was inconsistent (such as zalloc or state being NULL).
*/

extern int32  pnge_deflateParams (pnge_z_stream * strm,
                             int32 level,
                             int32 strategy);
/*
     Dynamically update the compression level and compression strategy.  The
   interpretation of level and strategy is as in deflateInit2.  This can be
   used to switch between compression and straight copy of the input data, or
   to switch to a different kind of input data requiring a different
   strategy. If the compression level is changed, the input available so far
   is compressed with the old level (and may be flushed); the new level will
   take effect only at the next call of deflate().

     Before the call of deflateParams, the stream state must be set as for
   a call of deflate(), since the currently available input may have to
   be compressed and flushed. In particular, strm->avail_out must be non-zero.

     deflateParams returns Z_OK if success, Z_STREAM_ERROR if the source
   stream state was inconsistent or if a parameter was invalid, Z_BUF_ERROR
   if strm->avail_out was zero.
*/




/* utility functions */

/*
     The following utility functions are implemented on top of the
   basic stream-oriented functions. To simplify the interface, some
   default options are assumed (compression level and memory usage,
   standard memory allocation functions). The source code of these
   utility functions can easily be modified if you need special options.
*/

extern int32  pnge_compress (uint8 *dest,   uint32 *destLen,
                        const uint8 *source, uint32 sourceLen);
/*
     Compresses the source buffer into the destination buffer.  sourceLen is
   the byte length of the source buffer. Upon entry, destLen is the total
   size of the destination buffer, which must be at least 0.1% larger than
   sourceLen plus 12 bytes. Upon exit, destLen is the actual size of the
   compressed buffer.
     This function can be used to compress a whole file at once if the
   input file is mmap'ed.
     compress returns Z_OK if success, Z_MEM_ERROR if there was not
   enough memory, Z_BUF_ERROR if there was not enough room in the output
   buffer.
*/

extern int32  pnge_compress2 (uint8 *dest,   uint32 *destLen,
                         const uint8 *source, uint32 sourceLen,
                         int32 level);
/*
     Compresses the source buffer into the destination buffer. The level
   parameter has the same meaning as in deflateInit.  sourceLen is the byte
   length of the source buffer. Upon entry, destLen is the total size of the
   destination buffer, which must be at least 0.1% larger than sourceLen plus
   12 bytes. Upon exit, destLen is the actual size of the compressed buffer.

     compress2 returns Z_OK if success, Z_MEM_ERROR if there was not enough
   memory, Z_BUF_ERROR if there was not enough room in the output buffer,
   Z_STREAM_ERROR if the level parameter is invalid.
*/

extern int32  pnge_uncompress (uint8 *dest,   uint32 *destLen,
                          const uint8 *source, uint32 sourceLen);
/*
     Decompresses the source buffer into the destination buffer.  sourceLen is
   the byte length of the source buffer. Upon entry, destLen is the total
   size of the destination buffer, which must be large enough to hold the
   entire uncompressed data. (The size of the uncompressed data must have
   been saved previously by the compressor and transmitted to the decompressor
   by some mechanism outside the scope of this compression library.)
   Upon exit, destLen is the actual size of the compressed buffer.
     This function can be used to decompress a whole file at once if the
   input file is mmap'ed.

     uncompress returns Z_OK if success, Z_MEM_ERROR if there was not
   enough memory, Z_BUF_ERROR if there was not enough room in the output
   buffer, or Z_DATA_ERROR if the input data was corrupted.
*/






/*
     These functions are not related to compression but are exported
   anyway because they might be useful in applications using the
   compression library.
*/

extern uint32  pnge_adler32 (uint32 adler, const uint8 *buf, uint32 len);

/*
     Update a running Adler-32 checksum with the bytes buf[0..len-1] and
   return the updated checksum. If buf is NULL, this function returns
   the required initial value for the checksum.
   An Adler-32 checksum is almost as reliable as a CRC32 but can be computed
   much faster. Usage example:

     uint32 adler = adler32(0L, Z_NULL, 0);

     while (read_buffer(buffer, length) != EOF) {
       adler = adler32(adler, buffer, length);
     }
     if (adler != original_adler) error();
*/


/* deflateInit and inflateInit are macros to allow checking the zlib version
 * and the compiler's view of pnge_z_stream:
 */
//extern int32  deflateInit_ (pnge_z_stream * strm, int32 level,
//const char *version, int32 stream_size);
extern int32  pnge_deflateInit2_ (pnge_z_stream * strm, int32  level, int32  method,
                             uint32 windowBits, uint32 memLevel,
                             int32 strategy, const char *version,
                             uint32 stream_size);
#define pnge_deflateInit2(strm, level, method, windowBits, memLevel, strategy) \
        pnge_deflateInit2_((strm),(level),(method),(windowBits),(memLevel),\
                      (strategy),           ZLIB_VERSION, sizeof(pnge_z_stream))





#endif /* FEATURE_PNG_ENCODER   */
#endif /* PNGDEFLATE_H */

