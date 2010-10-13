/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

          P O R T I B L E    N E T W O R K    G R A P H I C S   (PNG)
                      D E F L A T E

GENERAL DESCRIPTION
   Taken completely from LibPNG's "deflate.c" source file.

EXTERNAL FUNCTIONS

Copyright(c) 2003 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

$Header: //depot/asic/msmshared/services/graph/pngdeflate.c#7 $ 

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/16/05   kln     Fixed lint errors
07/01/04   mvd     Added "pnge" prefix to all functions, struct, local, 
                   and extern global variables.
05/12/04   mvd     Removed code defines: DEFLATE_DEBUG, DEBUG_DONT_INCLUDE, 
                   FASTEST, ASMV, MIN_MATCH, UNALIGNED_OK, MAX_MATCH
11/26/03   mvd     Removed dup header, added cast to fast deflate
                   Removed MVD_NOT_USED
                   Removed MSM_TARGET and PC_TARGET
11/26/03   pdo     Checked into Perforce
11/10/03   mvd     Initial port from LibPNG

===========================================================================*/


/* deflate.c -- compress data using the deflation algorithm
 * Copyright (C) 1995-2002 Jean-loup Gailly.
 * For conditions of distribution and use, see copyright notice in zlib.h 
 */

/*
 *  ALGORITHM
 *
 *      The "deflation" process depends on being able to identify portions
 *      of the input text which are identical to earlier input (within a
 *      sliding window trailing behind the input currently being processed).
 *
 *      The most straightforward technique turns out to be the fastest for
 *      most input files: try all possible matches and select the longest.
 *      The key feature of this algorithm is that insertions into the string
 *      dictionary are very simple and thus fast, and deletions are avoided
 *      completely. Insertions are performed at each input character, whereas
 *      string matches are performed only when the previous match ends. So it
 *      is preferable to spend more time in matches to allow very fast string
 *      insertions and avoid deletions. The matching algorithm for small
 *      strings is inspired from that of Rabin & Karp. A brute force approach
 *      is used to find longer strings when a small match has been found.
 *      A similar algorithm is used in comic (by Jan-Mark Wams) and freeze
 *      (by Leonid Broukhis).
 *         A previous version of this file used a more sophisticated algorithm
 *      (by Fiala and Greene) which is guaranteed to run in linear amortized
 *      time, but has a larger average cost, uses more memory and is patented.
 *      However the F&G algorithm may be faster for some highly redundant
 *      files if the parameter max_chain_length (described below) is too large.
 *
 *  ACKNOWLEDGEMENTS
 *
 *      The idea of lazy evaluation of matches is due to Jan-Mark Wams, and
 *      I found it in 'freeze' written by Leonid Broukhis.
 *      Thanks to many people for bug reports and testing.
 *
 *  REFERENCES
 *
 *      Deutsch, L.P.,"DEFLATE Compressed Data Format Specification".
 *      Available in ftp://ds.internic.net/rfc/rfc1951.txt
 *
 *      A description of the Rabin and Karp algorithm is given in the book
 *         "Algorithms" by R. Sedgewick, Addison-Wesley, p252.
 *
 *      Fiala,E.R., and Greene,D.H.
 *         Data Compression with Finite Windows, Comm.ACM, 32,4 (1989) 490-595
 *
 */



#include <memory.h>
#include <stdlib.h>
#include "comdef.h"
#include "customer.h"
#include "msg.h"

#if defined(FEATURE_PNG_ENCODER)

#include "pngdeflate.h"


extern  pngeEncRetCodeType pnge_mem_allocate( void ** dest, uint32 numBytes );
extern  void pnge_mem_free( void * buf );


const char deflate_copyright[] =
   " deflate 1.1.4 Copyright 1995-2002 Jean-loup Gailly ";
/*
  If you use the zlib library in a product, an acknowledgment is welcome
  in the documentation of your product. If for some reason you cannot
  include such an acknowledgment, I would appreciate that you keep this
  copyright string in the executable of your product.
 */


const char *png_deflate_errmsg[10] = {
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


/* ===========================================================================
 *  Function prototypes.
 */
typedef enum
{
   NEED_MORE,      /* block not completed, need more input or more output */
   BLOCK_DONE,     /* block flush performed */
   FINISH_STARTED, /* finish started, need only more output at next deflate */
   FINISH_DONE     /* finish done, accept no more input or output */
} block_state;

/* Compression function. Returns the block state after the call. */
typedef block_state (*compress_func) (pnge_deflate_state *s, int32 flush);

static void fill_window    (pnge_deflate_state *s);
static block_state deflate_stored (pnge_deflate_state *s, int32 flush);
static block_state deflate_fast   (pnge_deflate_state *s, int32 flush);
static block_state deflate_slow   (pnge_deflate_state *s, int32 flush);
static void lm_init        (pnge_deflate_state *s);
static void putShortMSB    (pnge_deflate_state *s, uint32 b);
static void flush_pending  (pnge_z_stream * strm);
static uint32 read_buf        (pnge_z_stream * strm, uint8 *buf, unsigned size);
static uint32 longest_match  (pnge_deflate_state *s, pnge_IPos cur_match);


#if 0
static  void check_match (pnge_deflate_state *s, pnge_IPos start, pnge_IPos match,
                          int32 length);
#endif

uint32  adler32(uint32 adler, const uint8 *buf, uint32 len);

/* ===========================================================================
 * Local data
 */


/* Tail of hash chains */
#define TOO_FAR 4096


/* Matches of length 3 are discarded if their distance exceeds TOO_FAR */

/* Values for max_lazy_match, good_match and max_chain_length, depending on
 * the desired pack level (0..9). The values given below have been tuned to
 * exclude worst case performance for pathological files. Better values may be
 * found for specific files.
 */
typedef struct config_s
{
   uint16 good_length; /* reduce lazy search above this match length */
   uint16 max_lazy;    /* do not perform lazy search above this match length */
   uint16 nice_length; /* quit search above this match length */
   uint16 max_chain;
   compress_func func;
} config;

static const config configuration_table[10] = {
/*      good lazy nice chain */
/* 0 */ {0,    0,  0,    0, deflate_stored},  /* store only */
/* 1 */ {4,    4,  8,    4, deflate_fast}, /* maximum speed, no lazy matches */
/* 2 */ {4,    5, 16,    8, deflate_fast},
/* 3 */ {4,    6, 32,   32, deflate_fast},

/* 4 */ {4,    4, 16,   16, deflate_slow},  /* lazy matches */
/* 5 */ {8,   16, 32,   32, deflate_slow},
/* 6 */ {8,   16, 128, 128, deflate_slow},
/* 7 */ {8,   32, 128, 256, deflate_slow},
/* 8 */ {32, 128, 258, 1024, deflate_slow},
/* 9 */ {32, 258, 258, 4096, deflate_slow}}; /* maximum compression */

/* Note: the deflate() code requires max_lazy >= MIN_MATCH and max_chain >= 4
 * For deflate_fast() (levels <= 3) good is ignored and lazy has a different
 * meaning.
 */


/* ===========================================================================
 * Update a hash value with the given input byte
 * IN  assertion: all calls to to UPDATE_HASH are made with consecutive
 *    input characters, so that a running hash key can be computed from the
 *    previous key instead of complete recalculation each time.
 */
#define UPDATE_HASH(s,h,c) (h = (((h)<<s->hash_shift) ^ (c)) & s->hash_mask)


/* ===========================================================================
 * Insert string str in the dictionary and set match_head to the previous head
 * of the hash chain (the most recent string with same hash key). Return
 * the previous length of the hash chain.
 * If this file is compiled with -DFASTEST, the compression level is forced
 * to 1, and no hash chains are maintained.
 * IN  assertion: all calls to to INSERT_STRING are made with consecutive
 *    input characters and the first MIN_MATCH bytes of str are valid
 *    (except for the last MIN_MATCH-1 bytes of the input file).
 */
/* #ifdef FASTEST */
#define INSERT_STRING(s, str, match_head) \
   (UPDATE_HASH(s, s->ins_h, s->window[(str) + (MIN_MATCH-1)]), \
    match_head = s->head[s->ins_h], \
    s->head[s->ins_h] = (uint16)(str))

#if 0
/* ifndef FASTEST */

#define INSERT_STRING(s, str, match_head) \
   (UPDATE_HASH(s, s->ins_h, s->window[(str) + (MIN_MATCH-1)]), \
    s->prev[(str) & s->w_mask] = match_head = s->head[s->ins_h], \
    s->head[s->ins_h] = (uint16)(str))
#endif

/* ===========================================================================
 * Initialize the hash table (avoiding 64K overflow for 16 bit systems).
 * prev[] will be initialized on the fly.
 */
#define CLEAR_HASH(s) \
    s->head[s->hash_size-1] = 0; \
    memset((uint8 *)s->head, 0, (unsigned)(s->hash_size-1)*sizeof(*s->head));








/* ========================================================================= */
int32  pnge_deflateInit2_(
                    pnge_z_stream * pngDeflatePtr,
                    int32  level,
                    int32  method,
                    uint32  windowBits,
                    uint32  memLevel,
                    int32  strategy,
                    const char *version,
                    uint32 stream_size
                    )
{
   pnge_deflate_state *s;
   int32 noheader = 0;
   boolean z_stream_error;
   static const char* my_version = ZLIB_VERSION;

   uint16 *overlay;
   /* We overlay pending_buf and d_buf+l_buf. This works since the average
    * output size for (length,distance) codes is <= 24 bits.
    */

   if( version == Z_NULL || version[0] != my_version[0] ||
       stream_size != sizeof(pnge_z_stream) )
   {
      return(Z_VERSION_ERROR);
   }
   if( pngDeflatePtr == Z_NULL ) return(Z_STREAM_ERROR);

   pngDeflatePtr->msg = Z_NULL;

   if( level == Z_DEFAULT_COMPRESSION ) level = 6;

/* #ifdef FASTEST */
   level = 1;
/* #endif */

   if( windowBits > 0x7FFFFFFFUL )
   { /* undocumented feature: suppress zlib header */
      noheader = 1;
      windowBits = 0xFFFFFFFFUL - windowBits;
   }

   z_stream_error = (memLevel < 1) || (memLevel > MAX_MEM_LEVEL) || 
       (method != Z_DEFLATED) || (windowBits < 9) || (windowBits > 15) || 
       (level < 0) || (level > 9) || (strategy < 0) || (strategy > Z_HUFFMAN_ONLY);
   if( z_stream_error ) 
   {
      return(Z_STREAM_ERROR);
   }

   //s = (pnge_deflate_state *) ZALLOC(strm, 1, sizeof(pnge_deflate_state));
   (void)pnge_mem_allocate( (void **) &s, sizeof(pnge_deflate_state) );

   if( s == Z_NULL )
      return(Z_MEM_ERROR);
   //pngDeflatePtr->state = (struct pnge_internal_state FAR *)s;
   pngDeflatePtr->state = (struct pnge_internal_state *)s;

   s->strm = pngDeflatePtr;

   s->noheader = noheader;
   s->w_bits = windowBits;
   s->w_size = 1 << s->w_bits;
   s->w_mask = s->w_size - 1;

   s->hash_bits = memLevel + 7;
   s->hash_size = 1 << s->hash_bits;
   s->hash_mask = s->hash_size - 1;
   s->hash_shift =  ((s->hash_bits+MIN_MATCH-1)/MIN_MATCH);

   (void)pnge_mem_allocate( (void **) &(s->window), 2*sizeof(uint8)*s->w_size );

   (void)pnge_mem_allocate( (void **) &(s->prev), sizeof(uint16)*s->w_size );

   (void)pnge_mem_allocate( (void **) &(s->head), sizeof(uint16)*s->hash_size );

   s->lit_bufsize = 1 << (memLevel + 6); /* 16K elements by default */

   (void)pnge_mem_allocate( (void **) &(overlay), s->lit_bufsize*(sizeof(uint16)+2) );

   s->pending_buf = (uint8 *) overlay;
   s->pending_buf_size = (uint32)s->lit_bufsize * (sizeof(uint16)+2L);

   if( s->window == Z_NULL || s->prev == Z_NULL || s->head == Z_NULL ||
       s->pending_buf == Z_NULL )
   {
      pngDeflatePtr->msg = png_deflate_errmsg[Z_NEED_DICT-Z_MEM_ERROR];
      (void)pnge_deflateEnd (pngDeflatePtr);
      return(Z_MEM_ERROR);
   }
   s->d_buf = overlay + s->lit_bufsize/sizeof(uint16);
   s->l_buf = s->pending_buf + (1+sizeof(uint16))*s->lit_bufsize;

   s->level = level;
   s->strategy = strategy;
   s->method = (uint8)method;

   return(pnge_deflateReset(pngDeflatePtr));
}















/* ========================================================================= */
int32  pnge_deflateReset (pnge_z_stream * strm)
{
   pnge_deflate_state *s;

   if( strm == Z_NULL || strm->state == Z_NULL )
      return(Z_STREAM_ERROR);

   strm->total_in = strm->total_out = 0;
   strm->msg = Z_NULL; /* use zfree if we ever allocate msg dynamically */
   strm->data_type = Z_UNKNOWN;

   s = (pnge_deflate_state *)strm->state;
   s->pending = 0;
   s->pending_out = s->pending_buf;

   if( s->noheader < 0 )
   {
      s->noheader = 0; /* was set to -1 by deflate(..., Z_FINISH); */
   }
   s->status = s->noheader ? BUSY_STATE : INIT_STATE;
   strm->adler = 1;
   s->last_flush = Z_NO_FLUSH;

   pnge_tr_init(s);
   lm_init(s);

   return(Z_OK);
}












/* =========================================================================
 * Put a short in the pending buffer. The 16-bit value is put in MSB order.
 * IN assertion: the stream state is correct and there is enough room in
 * pending_buf.
 */
static void putShortMSB (
                        pnge_deflate_state *s,
                        uint32 b
                        )
{
   put_byte(s, (uint8)(b >> 8));
   put_byte(s, (uint8)(b & 0xff));
}   







/* =========================================================================
 * Flush as much pending output as possible. All deflate() output goes
 * through this function so some applications may wish to modify it
 * to avoid allocating a large strm->next_out buffer and copying into it.
 * (See also read_buf()).
 */
static void flush_pending(pnge_z_stream * strm)
{
   unsigned len = strm->state->pending;

   if( len > strm->avail_out ) len = strm->avail_out;
   if( len == 0 ) return;

   memcpy(strm->next_out, strm->state->pending_out, len);
   strm->next_out  += len;
   strm->state->pending_out  += len;
   strm->total_out += len;
   strm->avail_out  -= len;
   strm->state->pending -= len;
   if( strm->state->pending == 0 )
   {
      strm->state->pending_out = strm->state->pending_buf;
   }
}








/* ========================================================================= */
int32  pnge_deflate (
               pnge_z_stream * strm,
               int32 flush
               )
{
   int32 old_flush; /* value of flush param for previous deflate call */
   pnge_deflate_state *s;

   if( strm == Z_NULL || strm->state == Z_NULL ||
       flush > Z_FINISH || flush < 0 )
   {
      return(Z_STREAM_ERROR);
   }
   s = strm->state;

   if( strm->next_out == Z_NULL ||
       (strm->next_in == Z_NULL && strm->avail_in != 0) ||
       (s->status == FINISH_STATE && flush != Z_FINISH) )
   {
      strm->msg = png_deflate_errmsg[Z_NEED_DICT-Z_STREAM_ERROR];
      return( Z_STREAM_ERROR );
   }
   if( strm->avail_out == 0 )
   {
      strm->msg = png_deflate_errmsg[Z_NEED_DICT-Z_BUF_ERROR];
      return( Z_BUF_ERROR );
   }

   s->strm = strm; /* just in case */
   old_flush = s->last_flush;
   s->last_flush = flush;

   /* Write the zlib header */
   if( s->status == INIT_STATE )
   {

      uint32 header = (Z_DEFLATED + ((s->w_bits-8)<<4)) << 8;
      uint32 level_flags = (uint32)((s->level-1)/2);

      if( level_flags > 3 ) level_flags = 3;
      header |= (level_flags << 6);
      if( s->strstart != 0 ) header |= PRESET_DICT;
      header += 31 - (header % 31);

      s->status = BUSY_STATE;
      putShortMSB(s, header);

      /* Save the adler32 of the preset dictionary: */
      if( s->strstart != 0 )
      {
         putShortMSB(s, (uint32)(strm->adler >> 16));
         putShortMSB(s, (uint32)(strm->adler & 0xffff));
      }
      strm->adler = 1L;
   }

   /* Flush as much pending output as possible */
   if( s->pending != 0 )
   {
      flush_pending(strm);
      if( strm->avail_out == 0 )
      {
         /* Since avail_out is 0, deflate will be called again with
          * more output space, but possibly with both pending and
          * avail_in equal to zero. There won't be anything to do,
          * but this is not an error situation so make sure we
          * return OK instead of BUF_ERROR at next call of deflate:
              */
         s->last_flush = -1;
         return(Z_OK);
      }

      /* Make sure there is something to do and avoid duplicate consecutive
       * flushes. For repeated and useless calls with Z_FINISH, we keep
       * returning Z_STREAM_END instead of Z_BUFF_ERROR.
       */
   }
   else if( strm->avail_in == 0 && flush <= old_flush &&
            flush != Z_FINISH )
   {
      strm->msg = png_deflate_errmsg[Z_NEED_DICT-Z_BUF_ERROR];
      return( Z_BUF_ERROR );
   }

   /* User must not provide more input after the first FINISH: */
   if( s->status == FINISH_STATE && strm->avail_in != 0 )
   {
      strm->msg = png_deflate_errmsg[Z_NEED_DICT-Z_BUF_ERROR];
      return( Z_BUF_ERROR );
   }

   /* Start a new block or continue the current one.
    */
   if( strm->avail_in != 0 || s->lookahead != 0 ||
       (flush != Z_NO_FLUSH && s->status != FINISH_STATE) )
   {
      block_state bstate;

      bstate = (*(configuration_table[s->level].func))(s, flush);

      if( bstate == FINISH_STARTED || bstate == FINISH_DONE )
      {
         s->status = FINISH_STATE;
      }
      if( bstate == NEED_MORE || bstate == FINISH_STARTED )
      {
         if( strm->avail_out == 0 )
         {
            s->last_flush = -1; /* avoid BUF_ERROR next call, see above */
         }
         return(Z_OK);
         /* If flush != Z_NO_FLUSH && avail_out == 0, the next call
          * of deflate should use the same flush parameter to make sure
          * that the flush is complete. So we don't have to output an
          * empty block here, this will be done at next call. This also
          * ensures that for a very small output buffer, we emit at most
          * one empty block.
          */
      }
      if( bstate == BLOCK_DONE )
      {
         if( flush == Z_PARTIAL_FLUSH )
         {
            pnge_tr_align(s);
         }
         else
         { /* FULL_FLUSH or SYNC_FLUSH */
            pnge_tr_stored_block(s, (char*)0, 0L, 0);
            /* For a full flush, this empty block will be recognized
             * as a special marker by inflate_sync().
             */
            if( flush == Z_FULL_FLUSH )
            {
               CLEAR_HASH(s);             /* forget history */
            }
         }
         flush_pending(strm);
         if( strm->avail_out == 0 )
         {
            s->last_flush = -1; /* avoid BUF_ERROR at next call, see above */
            return(Z_OK);
         }
      }
   }

   //Assert(strm->avail_out > 0, "bug2");
   if( strm->avail_out > 0 )
   {
      PNG_DEFLATE_MSG_FATAL( "ASSERTION failed in deflate() %d", strm->avail_out, 0, 0);
   }

   if( flush != Z_FINISH ) return(Z_OK);
   if( s->noheader ) return(Z_STREAM_END);

   /* Write the zlib trailer (adler32) */
   putShortMSB(s, (uint32)(strm->adler >> 16));
   putShortMSB(s, (uint32)(strm->adler & 0xffff));
   flush_pending(strm);
   /* If avail_out is zero, the application will call deflate again
    * to flush the rest.
    */
   s->noheader = -1; /* write the trailer only once! */
   return(s->pending != 0 ? Z_OK : Z_STREAM_END);
}






/* ========================================================================= */
int32  pnge_deflateEnd (pnge_z_stream * strm)
{
   int32 status;

   if( strm == Z_NULL || strm->state == Z_NULL ) return(Z_STREAM_ERROR);

   status = strm->state->status;
   if( status != INIT_STATE && status != BUSY_STATE &&
       status != FINISH_STATE )
   {
      return(Z_STREAM_ERROR);
   }

   /* Deallocate in reverse order of allocations: */
   pnge_mem_free( strm->state->pending_buf );
   pnge_mem_free( strm->state->head );
   pnge_mem_free( strm->state->prev );
   pnge_mem_free( strm->state->window );
   pnge_mem_free( strm->state );
   strm->state = Z_NULL;

   return(Z_OK);
}









/* ===========================================================================
 * Read a new buffer from the current input stream, update the adler32
 * and total number of bytes read.  All deflate() input goes through
 * this function so some applications may wish to modify it to avoid
 * allocating a large strm->next_in buffer and copying from it.
 * (See also flush_pending()).
 */
static uint32 read_buf(
                     pnge_z_stream * strm,
                     uint8 *buf,
                     unsigned size
                     )
{
   uint32 len = strm->avail_in;

   if( len > size ) len = size;
   if( len == 0 ) return(0);

   strm->avail_in  -= len;

   if( !strm->state->noheader )
   {
      strm->adler = adler32(strm->adler, strm->next_in, len);
   }
   memcpy(buf, strm->next_in, len);
   strm->next_in  += len;
   strm->total_in += len;

   return len;
}










/* ===========================================================================
 * Initialize the "longest match" routines for a new zlib stream
 */
static void lm_init (pnge_deflate_state *s)
{
   s->window_size = (uint32)2L*s->w_size;

   CLEAR_HASH(s);

   /* Set the default configuration parameters:
    */
   s->max_lazy_match   = configuration_table[s->level].max_lazy;
   s->good_match       = configuration_table[s->level].good_length;
   s->nice_match       = configuration_table[s->level].nice_length;
   s->max_chain_length = configuration_table[s->level].max_chain;

   s->strstart = 0;
   s->block_start = 0L;
   s->lookahead = 0;
   s->match_length = s->prev_length = MIN_MATCH-1;
   s->match_available = 0;
   s->ins_h = 0;

}






/* ===========================================================================
 * Set match_start to the longest match starting at the given string and
 * return its length. Matches shorter or equal to prev_length are discarded,
 * in which case the result is equal to prev_length and match_start is
 * garbage.
 * IN assertions: cur_match is the head of the hash chain for the current
 *   string (strstart) and its distance is <= MAX_DIST, and prev_length >= 1
 * OUT assertion: the match length is not greater than s->lookahead.
 */
#if 0
/* #ifndef FASTEST */
static uint32 longest_match(
                           pnge_deflate_state *s,
                           pnge_IPos cur_match                             /* current match */
                           )
{
   unsigned chain_length = s->max_chain_length;/* max hash chain length */
   register uint8 *scan = s->window + s->strstart; /* current string */
   register uint8 *match;                       /* matched string */
   register int32 len;                           /* length of current match */
   int32 best_len = s->prev_length;              /* best match length so far */
   int32 nice_match = s->nice_match;             /* stop if match long enough */
   pnge_IPos limit = s->strstart > (pnge_IPos)MAX_DIST(s) ?
                s->strstart - (pnge_IPos)MAX_DIST(s) : 0;
   /* Stop when cur_match becomes <= limit. To simplify the code,
    * we prevent matches with the string of window index 0.
    */
   uint16 *prev = s->prev;
   uint32 wmask = s->w_mask;

   register uint8 *strend = s->window + s->strstart + MAX_MATCH;
   register uint8 scan_end1  = scan[best_len-1];
   register uint8 scan_end   = scan[best_len];

   /* The code is optimized for HASH_BITS >= 8 and MAX_MATCH-2 multiple of 16.
    * It is easy to get rid of this optimization if necessary.
    */
   //Assert(s->hash_bits >= 8 && MAX_MATCH == 258, "Code too clever");
   if( (s->hash_bits >= 8) && (MAX_MATCH == 258) )
   {
      PNG_DEFLATE_MSG_FATAL( "ASSERTION failed in longest_match() %d", s->hash_bits, 0, 0);
   }

   /* Do not waste too much time if we already have a good match: */
   if( s->prev_length >= s->good_match )
   {
      chain_length >>= 2;
   }
   /* Do not look for matches beyond the end of the input. This is necessary
    * to make deflate deterministic.
    */
   if( (uint32)nice_match > s->lookahead )
   {
      nice_match = s->lookahead;
   }

   //Assert((uint32)s->strstart <= s->window_size-MIN_LOOKAHEAD, "need lookahead");
   if( (uint32)s->strstart <= s->window_size-MIN_LOOKAHEAD )
   {
      PNG_DEFLATE_MSG_FATAL( "ASSERTION failed in longest_match() %d %d",
                             (uint32)s->strstart, 
                             s->window_size-MIN_LOOKAHEAD, 0);
   }

   do
   {
      //Assert(cur_match < s->strstart, "no future");
      if( cur_match < s->strstart )
      {
         PNG_DEFLATE_MSG_FATAL( "ASSERTION failed in longest_match() %d %d", cur_match, s->strstart, 0);
      }

      match = s->window + cur_match;

      /* Skip to next match if the match length cannot increase
       * or if the match length is less than 2:
       */

      if( match[best_len]   != scan_end  ||
          match[best_len-1] != scan_end1 ||
          *match            != *scan     ||
          *++match          != scan[1] )      continue;

      /* The check at best_len-1 can be removed because it will be made
       * again later. (This heuristic is not always a win.)
       * It is not necessary to compare scan[2] and match[2] since they
       * are always equal when the other bytes match, given that
       * the hash keys are equal and that HASH_BITS >= 8.
       */
      scan += 2, match++;

      //Assert(*scan == *match, "match[2]?");
      if( *scan == *match )
      {
         PNG_DEFLATE_MSG_FATAL( "ASSERTION failed in longest_match() ", 0, 0, 0);
      }

      /* We check for insufficient lookahead only every 8th comparison;
       * the 256th check will be made at strstart+258.
       */
      do
      {
      } while( *++scan == *++match && *++scan == *++match &&
               *++scan == *++match && *++scan == *++match &&
               *++scan == *++match && *++scan == *++match &&
               *++scan == *++match && *++scan == *++match &&
               scan < strend );

      //Assert(scan <= s->window+(unsigned)(s->window_size-1), "wild scan");
      if( (scan <= s->window+(unsigned)(s->window_size-1) ) )
      {
         PNG_DEFLATE_MSG_FATAL( "ASSERTION failed in longest_match() %d", scan, 0, 0);
      }

      len = MAX_MATCH - (int32)(strend - scan);
      scan = strend - MAX_MATCH;


      if( len > best_len )
      {
         s->match_start = cur_match;
         best_len = len;
         if( len >= nice_match ) break;
         scan_end1  = scan[best_len-1];
         scan_end   = scan[best_len];
      }
   } while( (cur_match = prev[cur_match & wmask]) > limit
            && --chain_length != 0 );

   if( (uint32)best_len <= s->lookahead ) return(uint32)best_len;
   return(s->lookahead);
}


#endif /* NOT FASTEST */










/* FASTEST version */
/* ---------------------------------------------------------------------------
 * Optimized version for level == 1 only
 */
static uint32 longest_match(
                           pnge_deflate_state *s,
                           pnge_IPos cur_match                            /* current match */
                           )
{
   register uint8 *scan = s->window + s->strstart; /* current string */
   register uint8 *match;                       /* matched string */
   register uint32 len;                           /* length of current match */
   register uint8 *strend = s->window + s->strstart + MAX_MATCH;

   /* The code is optimized for HASH_BITS >= 8 and MAX_MATCH-2 multiple of 16.
    * It is easy to get rid of this optimization if necessary.
    */

   //Assert(s->hash_bits >= 8 && MAX_MATCH == 258, "Code too clever");
   if( s->hash_bits >= 8 )
   {
      PNG_DEFLATE_MSG_FATAL( "ASSERTION failed in longest_match 2() %d", s->hash_bits, 0, 0);
   }

   //Assert((uint32)s->strstart <= s->window_size-MIN_LOOKAHEAD, "need lookahead");
   if( (uint32)s->strstart <= s->window_size-MIN_LOOKAHEAD )
   {
      PNG_DEFLATE_MSG_FATAL( "ASSERTION failed in longest_match 2() %d", s->strstart, 0, 0);
   }

   //Assert(cur_match < s->strstart, "no future");
   if( cur_match < s->strstart )
   {
      PNG_DEFLATE_MSG_FATAL( "ASSERTION failed in longest_match 2() %d %d", cur_match, s->strstart, 0);
   }

   match = s->window + cur_match;

   /* Return failure if the match length is less than 2:
    */
   if( match[0] != scan[0] || match[1] != scan[1] ) return(MIN_MATCH-1);

   /* The check at best_len-1 can be removed because it will be made
    * again later. (This heuristic is not always a win.)
    * It is not necessary to compare scan[2] and match[2] since they
    * are always equal when the other bytes match, given that
    * the hash keys are equal and that HASH_BITS >= 8.
    */
   scan += 2, match += 2;

   //Assert(*scan == *match, "match[2]?");
   if( *scan == *match )
   {
      PNG_DEFLATE_MSG_FATAL( "ASSERTION failed in longest_match 2() ", 0, 0, 0);
   }

   /* We check for insufficient lookahead only every 8th comparison;
    * the 256th check will be made at strstart+258.
    */
   do
   {
   } while( *++scan == *++match && *++scan == *++match &&
            *++scan == *++match && *++scan == *++match &&
            *++scan == *++match && *++scan == *++match &&
            *++scan == *++match && *++scan == *++match &&
            scan < strend );

   //Assert(scan <= s->window+(unsigned)(s->window_size-1), "wild scan");
   if( scan <= s->window+(unsigned)(s->window_size-1) )
   {
      PNG_DEFLATE_MSG_FATAL( "ASSERTION failed in longest_match 2()", 0, 0, 0);
   }

   len = MAX_MATCH - (uint32)(strend - scan);

   if( len < MIN_MATCH ) return(MIN_MATCH - 1);

   s->match_start = cur_match;
   return(unsigned)(len <= s->lookahead ? len : s->lookahead);
}






#if 0
/* ===========================================================================
 * Check that the match at match_start is indeed a match.
 */
static void check_match(s, start, match, length)
   pnge_deflate_state *s;
pnge_IPos start, match;
int32 length;
{
   /* check that the match is indeed a match */
   if( memcmp(s->window + match,
              s->window + start, length) != EQUAL )
   {
      fprintf(stderr, " start %u, match %u, length %d\n",
              start, match, length);
      do
      {
         fprintf(stderr, "%c%c", s->window[match++], s->window[start++]);
      } while( --length != 0 );
      z_error("invalid match");
   }
   if( z_verbose > 1 )
   {
      fprintf(stderr,"\\[%d,%d]", start-match, length);
      do
      {
         putc(s->window[start++], stderr);
      } while( --length != 0 );
   }
}
#else
#define check_match(s, start, match, length)
#endif









/* ===========================================================================
 * Fill the window when the lookahead becomes insufficient.
 * Updates strstart and lookahead.
 *
 * IN assertion: lookahead < MIN_LOOKAHEAD
 * OUT assertions: strstart <= window_size-MIN_LOOKAHEAD
 *    At least one byte has been read, or avail_in == 0; reads are
 *    performed for at least two bytes (required for the zip translate_eol
 *    option -- not supported here).
 */
static void fill_window( pnge_deflate_state *s )
{
   unsigned n, m;
   uint16 *p;
   unsigned more;    /* Amount of free space at the end of the window. */
   uint32 wsize = s->w_size;

   do
   {
      more = (unsigned)(s->window_size -(uint32)s->lookahead -(uint32)s->strstart);

      /* Deal with !@#$% 64K limit: */
      if( more == 0 && s->strstart == 0 && s->lookahead == 0 )
      {
         more = wsize;

      }
      else if( more == (unsigned)(-1) )
      {
         /* Very unlikely, but possible on 16 bit machine if strstart == 0
          * and lookahead == 1 (input done one byte at time)
          */
         more--;

         /* If the window is almost full and there is insufficient lookahead,
          * move the upper half to the lower one to make room in the upper half.
          */
      }
      else if( s->strstart >= wsize+MAX_DIST(s) )
      {

         memcpy(s->window, s->window+wsize, (unsigned)wsize);
         s->match_start -= wsize;
         s->strstart    -= wsize; /* we now have strstart >= MAX_DIST */
         s->block_start -= (int32)wsize;

         /* Slide the hash table (could be avoided with 32 bit values
            at the expense of memory usage). We slide even when level == 0
            to keep the hash table consistent if we switch back to level > 0
            later. (Using level 0 permanently is not an optimal usage of
            zlib, so we don't care about this pathological case.)
          */
         n = s->hash_size;
         p = &s->head[n];
         do
         {
            m = *--p;
            *p = (uint16)(m >= wsize ? m-wsize : 0);
         } while( --n );

         n = wsize;
/* #ifndef FASTEST */
         p = &s->prev[n];
         do
         {
            m = *--p;
            *p = (uint16)(m >= wsize ? m-wsize : 0);
            /* If n is not on any hash chain, prev[n] is garbage but
             * its value will never be used.
             */
         } while( --n );
/* endif #ifndef FASTEST */

         more += wsize;
      }
      if( s->strm->avail_in == 0 ) return;

      /* If there was no sliding:
       *    strstart <= WSIZE+MAX_DIST-1 && lookahead <= MIN_LOOKAHEAD - 1 &&
       *    more == window_size - lookahead - strstart
       * => more >= window_size - (MIN_LOOKAHEAD-1 + WSIZE + MAX_DIST-1)
       * => more >= window_size - 2*WSIZE + 2
       * In the BIG_MEM or MMAP case (not yet supported),
       *   window_size == input_size + MIN_LOOKAHEAD  &&
       *   strstart + s->lookahead <= input_size => more >= MIN_LOOKAHEAD.
       * Otherwise, window_size == 2*WSIZE so more >= 2.
       * If there was sliding, more >= WSIZE. So in all cases, more >= 2.
       */
      //Assert(more >= 2, "more < 2");
      if( more >= 2 )
      {
         PNG_DEFLATE_MSG_FATAL( "ASSERTION failed in fill_window() %d", more, 0, 0);
      }

      n = read_buf(s->strm, s->window + s->strstart + s->lookahead, more);
      s->lookahead += n;

      /* Initialize the hash value now that we have some input: */
      if( s->lookahead >= MIN_MATCH )
      {
         s->ins_h = s->window[s->strstart];
         UPDATE_HASH(s, s->ins_h, s->window[s->strstart+1]);
      }
      /* If the whole input has less than MIN_MATCH bytes, ins_h is garbage,
       * but this is not important since only literal bytes will be emitted.
       */

   } while( s->lookahead < MIN_LOOKAHEAD && s->strm->avail_in != 0 );
}





/* ===========================================================================
 * Flush the current block, with given end-of-file flag.
 * IN assertion: strstart is set to the end of the current match.
 */
#define flush_block_only(s, eof) { \
   pnge_tr_flush_block(s, (s->block_start >= 0L ? \
                   (char *)&s->window[(unsigned)s->block_start] : \
                   (char *)Z_NULL), \
        (uint32)((long)s->strstart - s->block_start), \
        (int32)(eof)); \
   s->block_start = (int32)s->strstart; \
   flush_pending(s->strm); \
   Tracev((stderr,"[FLUSH]")); \
}

/* Same but force premature exit if necessary. */
#define flush_block(s, eof) { \
   flush_block_only(s, (eof)); \
   /*lint -e506*/ \
   if (s->strm->avail_out == 0) return ((eof) ? FINISH_STARTED : NEED_MORE);\
}










/* ===========================================================================
 * Copy without compression as much as possible from the input stream, return
 * the current block state.
 * This function does not insert new strings in the dictionary since
 * uncompressible data is probably not useful. This function is used
 * only for the level=0 compression option.
 * NOTE: this function should be optimized to avoid extra copying from
 * window to pending_buf.
 */
static block_state deflate_stored(
                                 pnge_deflate_state *s,
                                 int32 flush
                                 )
{
   /* Stored blocks are limited to 0xffff bytes, pending_buf is limited
    * to pending_buf_size, and each stored block has a 5 byte header:
    */
   uint32 max_block_size = 0xffff;
   uint32 max_start;

   if( max_block_size > s->pending_buf_size - 5 )
   {
      max_block_size = s->pending_buf_size - 5;
   }

   /* Copy as much as possible from input to output: */
   for( ;; )
   {
      /* Fill the window as much as possible: */
      if( s->lookahead <= 1 )
      {

         //Assert(s->strstart < s->w_size+MAX_DIST(s) ||
         //s->block_start >= (long)s->w_size, "slide too late");
         if( (s->strstart < s->w_size+MAX_DIST(s) || (uint32)s->block_start >= s->w_size ) )
         {
            PNG_DEFLATE_MSG_FATAL( "ASSERTION failed in deflate_stored() ", 0, 0, 0);
         }

         fill_window(s);
         if( s->lookahead == 0 && flush == Z_NO_FLUSH ) return(NEED_MORE);

         if( s->lookahead == 0 ) break; /* flush the current block */
      }

      //Assert(s->block_start >= 0L, "block gone");
      if( s->block_start >= 0L )
      {
         PNG_DEFLATE_MSG_FATAL( "ASSERTION failed in deflate_stored() %d", s->block_start, 0, 0);
      }

      s->strstart += s->lookahead;
      s->lookahead = 0;

      /* Emit a stored block if pending_buf will be full: */
      max_start = (uint32)s->block_start + max_block_size;
      if( s->strstart == 0 || (uint32)s->strstart >= max_start )
      {
         /* strstart == 0 is possible when wraparound on 16-bit machine */
         s->lookahead = (uint32)(s->strstart - max_start);
         s->strstart = (uint32)max_start;
         flush_block(s, 0);
      }
      /* Flush if we may have to slide, otherwise block_start may become
           * negative and the data will be gone:
           */
      if( s->strstart - (uint32)s->block_start >= MAX_DIST(s) )
      {
         flush_block(s, 0);
      }
   }
   flush_block(s, flush == Z_FINISH);
   return(flush == Z_FINISH ? FINISH_DONE : BLOCK_DONE);
}






/* ===========================================================================
 * Compress as much as possible from the input stream, return the current
 * block state.
 * This function does not perform lazy evaluation of matches and inserts
 * new strings in the dictionary only for unmatched strings or for short
 * matches. It is used only for the fast compression options.
 */
static block_state deflate_fast(
                               pnge_deflate_state *s,
                               int32 flush
                               )
{
   pnge_IPos hash_head = 0; /* head of the hash chain */
   int32 bflush;           /* set if current block must be flushed */

   for( ;; )
   {
      /* Make sure that we always have enough lookahead, except
       * at the end of the input file. We need MAX_MATCH bytes
       * for the next match, plus MIN_MATCH bytes to insert the
       * string following the next match.
       */
      if( s->lookahead < MIN_LOOKAHEAD )
      {
         fill_window(s);
         if( s->lookahead < MIN_LOOKAHEAD && flush == Z_NO_FLUSH )
         {
            return(NEED_MORE);
         }
         if( s->lookahead == 0 ) break; /* flush the current block */
      }

      /* Insert the string window[strstart .. strstart+2] in the
       * dictionary, and set hash_head to the head of the hash chain:
       */
      if( s->lookahead >= MIN_MATCH )
      {
         INSERT_STRING(s, s->strstart, hash_head);
      }

      /* Find the longest match, discarding those <= prev_length.
       * At this point we have always match_length < MIN_MATCH
       */
      if( hash_head != 0 && s->strstart - hash_head <= MAX_DIST(s) )
      {
         /* To simplify the code, we prevent matches with the string
          * of window index 0 (in particular we have to avoid a match
          * of the string with itself at the start of the input file).
          */
         if( s->strategy != Z_HUFFMAN_ONLY )
         {
            s->match_length = longest_match (s, hash_head);
         }
         /* longest_match() sets match_start */
      }
      if( s->match_length >= MIN_MATCH )
      {
         check_match(s, s->strstart, s->match_start, s->match_length);

         // MVD: uint8 cast added
         pnge_tr_tally_dist(s, (uint16) (s->strstart - s->match_start),
                       (uint8) s->match_length - MIN_MATCH, bflush);

         s->lookahead -= s->match_length;

         /* Insert new strings in the hash table only if the match length
          * is not too large. This saves time but degrades compression.
          */
/* #ifndef FASTEST */
         if( s->match_length <= s->max_insert_length &&
             s->lookahead >= MIN_MATCH )
         {
            s->match_length--; /* string at strstart already in hash table */
            do
            {
               s->strstart++;
               INSERT_STRING(s, s->strstart, hash_head);
               /* strstart never exceeds WSIZE-MAX_MATCH, so there are
                * always MIN_MATCH bytes ahead.
                */
            } while( --s->match_length != 0 );
            s->strstart++; 
         }
         else
/* endif #ifndef FASTEST */
         {
            s->strstart += s->match_length;
            s->match_length = 0;
            s->ins_h = s->window[s->strstart];
            UPDATE_HASH(s, s->ins_h, s->window[s->strstart+1]);
            /* If lookahead < MIN_MATCH, ins_h is garbage, but it does not
             * matter since it will be recomputed at next deflate call.
             */
         }
      }
      else
      {
         /* No match, output a literal byte */
         Tracevv((stderr,"%c", s->window[s->strstart]));
         pnge_tr_tally_lit (s, s->window[s->strstart], bflush);
         s->lookahead--;
         s->strstart++; 
      }
      if( bflush ) flush_block(s, 0);
   }
   flush_block(s, flush == Z_FINISH);
   return(flush == Z_FINISH ? FINISH_DONE : BLOCK_DONE);
}








/* ===========================================================================
 * Same as above, but achieves better compression. We use a lazy
 * evaluation for matches: a match is finally adopted only if there is
 * no better match at the next window position.
 */
static block_state deflate_slow(
                               pnge_deflate_state *s,
                               int32 flush
                               )
{
   pnge_IPos hash_head = 0;    /* head of hash chain */
   int32 bflush;              /* set if current block must be flushed */

   /* Process the input block. */
   for( ;; )
   {
      /* Make sure that we always have enough lookahead, except
       * at the end of the input file. We need MAX_MATCH bytes
       * for the next match, plus MIN_MATCH bytes to insert the
       * string following the next match.
       */
      if( s->lookahead < MIN_LOOKAHEAD )
      {
         fill_window(s);
         if( s->lookahead < MIN_LOOKAHEAD && flush == Z_NO_FLUSH )
         {
            return(NEED_MORE);
         }
         if( s->lookahead == 0 ) break; /* flush the current block */
      }

      /* Insert the string window[strstart .. strstart+2] in the
       * dictionary, and set hash_head to the head of the hash chain:
       */
      if( s->lookahead >= MIN_MATCH )
      {
         INSERT_STRING(s, s->strstart, hash_head);
      }

      /* Find the longest match, discarding those <= prev_length.
       */
      s->prev_length = s->match_length, s->prev_match = s->match_start;
      s->match_length = MIN_MATCH-1;

      if( hash_head != 0 && s->prev_length < s->max_lazy_match &&
          s->strstart - hash_head <= MAX_DIST(s) )
      {
         /* To simplify the code, we prevent matches with the string
          * of window index 0 (in particular we have to avoid a match
          * of the string with itself at the start of the input file).
          */
         if( s->strategy != Z_HUFFMAN_ONLY )
         {
            s->match_length = longest_match (s, hash_head);
         }
         /* longest_match() sets match_start */

         if( s->match_length <= 5 && (s->strategy == Z_FILTERED ||
                                      (s->match_length == MIN_MATCH &&
                                       s->strstart - s->match_start > TOO_FAR)) )
         {

            /* If prev_match is also MIN_MATCH, match_start is garbage
             * but we will ignore the current match anyway.
             */
            s->match_length = MIN_MATCH-1;
         }
      }
      /* If there was a match at the previous step and the current
       * match is not better, output the previous match:
       */
      if( s->prev_length >= MIN_MATCH && s->match_length <= s->prev_length )
      {
         uint32 max_insert = s->strstart + s->lookahead - MIN_MATCH;
         /* Do not insert strings in hash table beyond this. */

         check_match(s, s->strstart-1, s->prev_match, s->prev_length);

         // MVD: uint8 cast added
         pnge_tr_tally_dist(s, (uint16) (s->strstart -1 - s->prev_match),
                       (uint8) s->prev_length - MIN_MATCH, bflush);

         /* Insert in hash table all strings up to the end of the match.
          * strstart-1 and strstart are already inserted. If there is not
          * enough lookahead, the last two strings are not inserted in
          * the hash table.
          */
         s->lookahead -= s->prev_length-1;
         s->prev_length -= 2;
         do
         {
            if( ++s->strstart <= max_insert )
            {
               INSERT_STRING(s, s->strstart, hash_head);
            }
         } while( --s->prev_length != 0 );
         s->match_available = 0;
         s->match_length = MIN_MATCH-1;
         s->strstart++;

         if( bflush ) flush_block(s, 0);
      }
      else if( s->match_available )
      {
         /* If there was no match at the previous position, output a
          * single literal. If there was a match but the current match
          * is longer, truncate the previous match to a single literal.
          */
         Tracevv((stderr,"%c", s->window[s->strstart-1]));
         pnge_tr_tally_lit(s, s->window[s->strstart-1], bflush);
         if( bflush )
         {
            flush_block_only(s, 0);
         }
         s->strstart++;
         s->lookahead--;
         if( s->strm->avail_out == 0 ) return(NEED_MORE);
      }
      else
      {
         /* There is no previous match to compare with, wait for
          * the next step to decide.
          */
         s->match_available = 1;
         s->strstart++;
         s->lookahead--;
      }
   }

   //Assert (flush != Z_NO_FLUSH, "no flush?");
   if( flush != Z_NO_FLUSH )
   {
      PNG_DEFLATE_MSG_FATAL( "ASSERTION failed in deflate_stored() %d", flush, 0, 0);
   }

   if( s->match_available )
   {
      Tracevv((stderr,"%c", s->window[s->strstart-1]));
      pnge_tr_tally_lit(s, s->window[s->strstart-1], bflush);
      s->match_available = 0;
   }
   flush_block(s, flush == Z_FINISH);
   return(flush == Z_FINISH ? FINISH_DONE : BLOCK_DONE);
}






/* adler32.c -- compute the Adler-32 checksum of a data stream
 * Copyright (C) 1995-2002 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h 
 */


#define BASE 65521L /* largest prime smaller than 65536 */
#define NMAX 5552
/* NMAX is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1 */

#define DO1(buf,i)  {s1 += buf[i]; s2 += s1;}
#define DO2(buf,i)  DO1(buf,(i)); DO1(buf,(i)+1);
#define DO4(buf,i)  DO2(buf,(i)); DO2(buf,(i)+2);
#define DO8(buf,i)  DO4(buf,i); DO4(buf,i+4);
#define DO16(buf)   DO8(buf,0); DO8(buf,8);

/* ========================================================================= */
uint32  adler32(uint32 adler, const uint8 *buf, uint32 len)
{
   unsigned long s1 = adler & 0xffff;
   unsigned long s2 = (adler >> 16) & 0xffff;
   uint32 k;

   if( buf == Z_NULL ) return(1L);

   while( len > 0 )
   {
      k = len < NMAX ? len : NMAX;
      len -= k;
      while( k >= 16 )
      {
         DO16(buf);
         buf += 16;
         k -= 16;
      }
      if( k != 0 ) do
         {
            s1 += *buf++;
            s2 += s1;
         } while( --k );
      s1 %= BASE;
      s2 %= BASE;
   }
   return(s2 << 16) | s1;
}

#endif /* FEATURE_PNG_ENCODER   */
