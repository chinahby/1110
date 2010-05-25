/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

          P O R T I B L E    N E T W O R K    G R A P H I C S   (PNG)
                      T R E E S       

GENERAL DESCRIPTION
trees.c -- output deflated data using Huffman coding
Copyright (C) 1995-2002 Jean-loup Gailly
For conditions of distribution and use, see copyright notice in zlib.h 


EXTERNAL FUNCTIONS

Copyright(c) 2003 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

$Header: //depot/asic/msmshared/services/graph/pngtrees.c#7 $

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/26/06   vma     Added boundary error check in compress_block()
09/16/05   kln     Fixed lint errors
07/01/04   mvd     Added "pnge" prefix to all functions, struct, local,
                   and extern global variables.
05/12/04   mvd     Removed: DUMP_BL_TREE, FORCE_STORED, FORCE_STATIC, 
                   DEFLATE_DEBUG
11/26/03   mvd     Removed MVD_NOT_USED
                   Removed MSM_TARGET and PC_TARGET
11/26/03   pdo     Checked into Perforce
11/10/03   mvd     Initial port from LibPNG

===========================================================================*/



/*
 *  ALGORITHM
 *
 *      The "deflation" process uses several Huffman trees. The more
 *      common source values are represented by shorter bit sequences.
 *
 *      Each code tree is stored in a compressed form which is itself
 * a Huffman encoding of the lengths of all the code strings (in
 * ascending order by source values).  The actual code strings are
 * reconstructed from the lengths in the inflate process, as described
 * in the deflate specification.
 *
 *  REFERENCES
 *
 *      Deutsch, L.P.,"'Deflate' Compressed Data Format Specification".
 *      Available in ftp.uu.net:/pub/archiving/zip/doc/deflate-1.1.doc
 *
 *      Storer, James A.
 *          Data Compression:  Methods and Theory, pp. 49-50.
 *          Computer Science Press, 1988.  ISBN 0-7167-8156-5.
 *
 *      Sedgewick, R.
 *          Algorithms, p290.
 *          Addison-Wesley, 1983. ISBN 0-201-06672-6.
 */


/* #define GEN_TREES_H */

/*===========================================================================

                       INCLUDE FILES FOR MODULE

===========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include "comdef.h"
#include "customer.h"

#if defined(FEATURE_PNG_ENCODER)

#include "pngglobal.h"
#include "pngdeflate.h"
#include "msg.h"






/* ===========================================================================
 * Constants
 */

/* The three kinds of block type */
#define STORED_BLOCK 0
#define STATIC_TREES 1
#define DYN_TREES    2

#define MAX_BL_BITS 7
/* Bit length codes must not exceed MAX_BL_BITS bits */

#define END_BLOCK 256
/* end of block literal code */

#define REP_3_6      16
/* repeat previous bit length 3-6 times (2 bits of repeat count) */

#define REPZ_3_10    17
/* repeat a zero length 3-10 times  (3 bits of repeat count) */

#define REPZ_11_138  18
/* repeat a zero length 11-138 times  (7 bits of repeat count) */

static const uint32 extra_lbits[LENGTH_CODES] /* extra bits for each length code */
   = {0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,0};

static const uint32 extra_dbits[D_CODES] /* extra bits for each distance code */
   = {0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13};

static const uint32 extra_blbits[BL_CODES]/* extra bits for each bit length code */
   = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,3,7};

static const uint8 bl_order[BL_CODES]
   = {16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15};
/* The lengths of the bit length codes are sent in order of decreasing
 * probability, to avoid transmitting the lengths for unused bit length codes.
 */

#define Buf_size (8 * 2*sizeof(char))

/* Number of bits used within bi_buf. (bi_buf might be implemented on
 * more than 16 bits on some systems.)
 */

/* ===========================================================================
 * static data. These are initialized only once.
 */

#define DIST_CODE_LEN  512 /* see definition of array dist_code below */



/*lint -save -e708 */
const pnge_ct_data pnge_static_ltree[L_CODES+2] =  {
   {{ 12},{  8}}, {{140},{  8}}, {{ 76},{  8}}, {{204},{  8}}, {{ 44},{  8}},
   {{172},{  8}}, {{108},{  8}}, {{236},{  8}}, {{ 28},{  8}}, {{156},{  8}},
   {{ 92},{  8}}, {{220},{  8}}, {{ 60},{  8}}, {{188},{  8}}, {{124},{  8}},
   {{252},{  8}}, {{  2},{  8}}, {{130},{  8}}, {{ 66},{  8}}, {{194},{  8}},
   {{ 34},{  8}}, {{162},{  8}}, {{ 98},{  8}}, {{226},{  8}}, {{ 18},{  8}},
   {{146},{  8}}, {{ 82},{  8}}, {{210},{  8}}, {{ 50},{  8}}, {{178},{  8}},
   {{114},{  8}}, {{242},{  8}}, {{ 10},{  8}}, {{138},{  8}}, {{ 74},{  8}},
   {{202},{  8}}, {{ 42},{  8}}, {{170},{  8}}, {{106},{  8}}, {{234},{  8}},
   {{ 26},{  8}}, {{154},{  8}}, {{ 90},{  8}}, {{218},{  8}}, {{ 58},{  8}},
   {{186},{  8}}, {{122},{  8}}, {{250},{  8}}, {{  6},{  8}}, {{134},{  8}},
   {{ 70},{  8}}, {{198},{  8}}, {{ 38},{  8}}, {{166},{  8}}, {{102},{  8}},
   {{230},{  8}}, {{ 22},{  8}}, {{150},{  8}}, {{ 86},{  8}}, {{214},{  8}},
   {{ 54},{  8}}, {{182},{  8}}, {{118},{  8}}, {{246},{  8}}, {{ 14},{  8}},
   {{142},{  8}}, {{ 78},{  8}}, {{206},{  8}}, {{ 46},{  8}}, {{174},{  8}},
   {{110},{  8}}, {{238},{  8}}, {{ 30},{  8}}, {{158},{  8}}, {{ 94},{  8}},
   {{222},{  8}}, {{ 62},{  8}}, {{190},{  8}}, {{126},{  8}}, {{254},{  8}},
   {{  1},{  8}}, {{129},{  8}}, {{ 65},{  8}}, {{193},{  8}}, {{ 33},{  8}},
   {{161},{  8}}, {{ 97},{  8}}, {{225},{  8}}, {{ 17},{  8}}, {{145},{  8}},
   {{ 81},{  8}}, {{209},{  8}}, {{ 49},{  8}}, {{177},{  8}}, {{113},{  8}},
   {{241},{  8}}, {{  9},{  8}}, {{137},{  8}}, {{ 73},{  8}}, {{201},{  8}},
   {{ 41},{  8}}, {{169},{  8}}, {{105},{  8}}, {{233},{  8}}, {{ 25},{  8}},
   {{153},{  8}}, {{ 89},{  8}}, {{217},{  8}}, {{ 57},{  8}}, {{185},{  8}},
   {{121},{  8}}, {{249},{  8}}, {{  5},{  8}}, {{133},{  8}}, {{ 69},{  8}},
   {{197},{  8}}, {{ 37},{  8}}, {{165},{  8}}, {{101},{  8}}, {{229},{  8}},
   {{ 21},{  8}}, {{149},{  8}}, {{ 85},{  8}}, {{213},{  8}}, {{ 53},{  8}},
   {{181},{  8}}, {{117},{  8}}, {{245},{  8}}, {{ 13},{  8}}, {{141},{  8}},
   {{ 77},{  8}}, {{205},{  8}}, {{ 45},{  8}}, {{173},{  8}}, {{109},{  8}},
   {{237},{  8}}, {{ 29},{  8}}, {{157},{  8}}, {{ 93},{  8}}, {{221},{  8}},
   {{ 61},{  8}}, {{189},{  8}}, {{125},{  8}}, {{253},{  8}}, {{ 19},{  9}},
   {{275},{  9}}, {{147},{  9}}, {{403},{  9}}, {{ 83},{  9}}, {{339},{  9}},
   {{211},{  9}}, {{467},{  9}}, {{ 51},{  9}}, {{307},{  9}}, {{179},{  9}},
   {{435},{  9}}, {{115},{  9}}, {{371},{  9}}, {{243},{  9}}, {{499},{  9}},
   {{ 11},{  9}}, {{267},{  9}}, {{139},{  9}}, {{395},{  9}}, {{ 75},{  9}},
   {{331},{  9}}, {{203},{  9}}, {{459},{  9}}, {{ 43},{  9}}, {{299},{  9}},
   {{171},{  9}}, {{427},{  9}}, {{107},{  9}}, {{363},{  9}}, {{235},{  9}},
   {{491},{  9}}, {{ 27},{  9}}, {{283},{  9}}, {{155},{  9}}, {{411},{  9}},
   {{ 91},{  9}}, {{347},{  9}}, {{219},{  9}}, {{475},{  9}}, {{ 59},{  9}},
   {{315},{  9}}, {{187},{  9}}, {{443},{  9}}, {{123},{  9}}, {{379},{  9}},
   {{251},{  9}}, {{507},{  9}}, {{  7},{  9}}, {{263},{  9}}, {{135},{  9}},
   {{391},{  9}}, {{ 71},{  9}}, {{327},{  9}}, {{199},{  9}}, {{455},{  9}},
   {{ 39},{  9}}, {{295},{  9}}, {{167},{  9}}, {{423},{  9}}, {{103},{  9}},
   {{359},{  9}}, {{231},{  9}}, {{487},{  9}}, {{ 23},{  9}}, {{279},{  9}},
   {{151},{  9}}, {{407},{  9}}, {{ 87},{  9}}, {{343},{  9}}, {{215},{  9}},
   {{471},{  9}}, {{ 55},{  9}}, {{311},{  9}}, {{183},{  9}}, {{439},{  9}},
   {{119},{  9}}, {{375},{  9}}, {{247},{  9}}, {{503},{  9}}, {{ 15},{  9}},
   {{271},{  9}}, {{143},{  9}}, {{399},{  9}}, {{ 79},{  9}}, {{335},{  9}},
   {{207},{  9}}, {{463},{  9}}, {{ 47},{  9}}, {{303},{  9}}, {{175},{  9}},
   {{431},{  9}}, {{111},{  9}}, {{367},{  9}}, {{239},{  9}}, {{495},{  9}},
   {{ 31},{  9}}, {{287},{  9}}, {{159},{  9}}, {{415},{  9}}, {{ 95},{  9}},
   {{351},{  9}}, {{223},{  9}}, {{479},{  9}}, {{ 63},{  9}}, {{319},{  9}},
   {{191},{  9}}, {{447},{  9}}, {{127},{  9}}, {{383},{  9}}, {{255},{  9}},
   {{511},{  9}}, {{  0},{  7}}, {{ 64},{  7}}, {{ 32},{  7}}, {{ 96},{  7}},
   {{ 16},{  7}}, {{ 80},{  7}}, {{ 48},{  7}}, {{112},{  7}}, {{  8},{  7}},
   {{ 72},{  7}}, {{ 40},{  7}}, {{104},{  7}}, {{ 24},{  7}}, {{ 88},{  7}},
   {{ 56},{  7}}, {{120},{  7}}, {{  4},{  7}}, {{ 68},{  7}}, {{ 36},{  7}},
   {{100},{  7}}, {{ 20},{  7}}, {{ 84},{  7}}, {{ 52},{  7}}, {{116},{  7}},
   {{  3},{  8}}, {{131},{  8}}, {{ 67},{  8}}, {{195},{  8}}, {{ 35},{  8}},
   {{163},{  8}}, {{ 99},{  8}}, {{227},{  8}}
};

const pnge_ct_data pnge_static_dtree[D_CODES] = {
   {{ 0},{ 5}}, {{16},{ 5}}, {{ 8},{ 5}}, {{24},{ 5}}, {{ 4},{ 5}},
   {{20},{ 5}}, {{12},{ 5}}, {{28},{ 5}}, {{ 2},{ 5}}, {{18},{ 5}},
   {{10},{ 5}}, {{26},{ 5}}, {{ 6},{ 5}}, {{22},{ 5}}, {{14},{ 5}},
   {{30},{ 5}}, {{ 1},{ 5}}, {{17},{ 5}}, {{ 9},{ 5}}, {{25},{ 5}},
   {{ 5},{ 5}}, {{21},{ 5}}, {{13},{ 5}}, {{29},{ 5}}, {{ 3},{ 5}},
   {{19},{ 5}}, {{11},{ 5}}, {{27},{ 5}}, {{ 7},{ 5}}, {{23},{ 5}}
};

/*lint -restore */
const uint8 pnge_dist_code[DIST_CODE_LEN] = {
   0,  1,  2,  3,  4,  4,  5,  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  8,
   8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 10, 10,
   10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
   11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
   12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13,
   13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
   13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
   14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
   14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
   14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15,
   15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
   15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
   15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,  0,  0, 16, 17,
   18, 18, 19, 19, 20, 20, 20, 20, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 22,
   23, 23, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
   24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
   26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
   26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 27,
   27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
   27, 27, 27, 27, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
   28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
   28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
   28, 28, 28, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
   29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
   29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
   29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29
};

const uint8 pnge_length_code[MAX_MATCH-MIN_MATCH+1]= {
   0,  1,  2,  3,  4,  5,  6,  7,  8,  8,  9,  9, 10, 10, 11, 11, 12, 12, 12, 12,
   13, 13, 13, 13, 14, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16,
   17, 17, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19,
   19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
   21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22,
   22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 23, 23,
   23, 23, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
   24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
   25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
   25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 26, 26,
   26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
   26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
   27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 28
};

const int32 pnge_base_length[LENGTH_CODES] = {
   0, 1, 2, 3, 4, 5, 6, 7, 8, 10, 12, 14, 16, 20, 24, 28, 32, 40, 48, 56,
   64, 80, 96, 112, 128, 160, 192, 224, 0
};

const uint32 pnge_base_dist[D_CODES] = {
   0,     1,     2,     3,     4,     6,     8,    12,    16,    24,
   32,    48,    64,    96,   128,   192,   256,   384,   512,   768,
   1024,  1536,  2048,  3072,  4096,  6144,  8192, 12288, 16384, 24576
};




struct pnge_static_tree_desc_s
{
   const pnge_ct_data *static_tree;    /* static tree or NULL */
   const uint32 *extra_bits;       /* extra bits for each code or NULL */
   uint32     extra_base;          /* base index for extra_bits */
   uint32     elems;               /* max number of elements in the tree */
   uint32     max_length;          /* max bit length for the codes */
};

static pnge_static_tree_desc  static_l_desc =
{pnge_static_ltree, extra_lbits, LITERALS+1, L_CODES, MAX_BITS};

static pnge_static_tree_desc  static_d_desc =
{pnge_static_dtree, extra_dbits, 0,          D_CODES, MAX_BITS};

static pnge_static_tree_desc  static_bl_desc =
{(const pnge_ct_data *)0, extra_blbits, 0,   BL_CODES, MAX_BL_BITS};

/* ===========================================================================
 * Local (static) routines in this file.
 */

static void init_block     (pnge_deflate_state *s);
static void pqdownheap     (pnge_deflate_state *s, pnge_ct_data *tree, uint32 k);
static void gen_bitlen     (pnge_deflate_state *s, pnge_tree_desc *desc);
static void gen_codes      (pnge_ct_data *tree, uint32 max_code, uint16 *bl_count);
static void build_tree     (pnge_deflate_state *s, pnge_tree_desc *desc);
static void scan_tree      (pnge_deflate_state *s, pnge_ct_data *tree, uint32 max_code);
static void send_tree      (pnge_deflate_state *s, pnge_ct_data *tree, uint32 max_code);
static uint32  build_bl_tree  (pnge_deflate_state *s);
static void send_all_trees (pnge_deflate_state *s, uint32 lcodes, uint32 dcodes,
                            uint32 blcodes);
static void compress_block (pnge_deflate_state *s, pnge_ct_data *ltree,
                            pnge_ct_data *dtree);
static void set_data_type  (pnge_deflate_state *s);
static uint16 bi_reverse (uint16 value, int32 length);
static void bi_windup      (pnge_deflate_state *s);
static void bi_flush       (pnge_deflate_state *s);
static void copy_block     (pnge_deflate_state *s, char *buf, unsigned len,
                            int32 header);



#define send_code(s, c, tree) send_bits(s, (tree[c].Code), (tree[c].Len))

/* Send a code of the given tree. c and tree must not have side effects */






/* ===========================================================================
 * Output a short LSB first on the stream.
 * IN assertion: there is enough room in pendingBuf.
 */
#define put_short(s, w) { \
    put_byte(s, (uint8)((w) & 0xff)); \
    put_byte(s, (uint8)((uint16)(w) >> 8)); \
}

/* ===========================================================================
 * Send a value on a given number of bits.
 * IN assertion: length <= 16 and value fits in length bits.
 */

#define send_bits(s, value, length) \
{ uint16 len = (uint16)length;\
  if (s->bi_valid > (int32)Buf_size - len) {\
    uint16 val = (uint16)(value);\
    s->bi_buf |= (val << s->bi_valid);\
    put_short(s, s->bi_buf);\
    s->bi_buf = (uint16)val >> (Buf_size - s->bi_valid);\
    s->bi_valid += len - Buf_size;\
  } else {\
    s->bi_buf |= (uint16)(value) << s->bi_valid;\
    s->bi_valid += len;\
  }\
}


#define TREES_MAX(a,b) (a >= b ? a : b)



/* ===========================================================================
 * Initialize the tree data structures for a new zlib stream.
 */
void pnge_tr_init( pnge_deflate_state *s )
{
   // MVD
   //pnge_tr_static_init();

   s->l_desc.dyn_tree = s->dyn_ltree;
   s->l_desc.stat_desc = &static_l_desc;

   s->d_desc.dyn_tree = s->dyn_dtree;
   s->d_desc.stat_desc = &static_d_desc;

   s->bl_desc.dyn_tree = s->bl_tree;
   s->bl_desc.stat_desc = &static_bl_desc;

   s->bi_buf = 0;
   s->bi_valid = 0;
   s->last_eob_len = 8; /* enough lookahead for inflate */

   /* Initialize the first block of the first file: */
   init_block(s);
}

/* ===========================================================================
 * Initialize a new block.
 */
static void init_block( pnge_deflate_state *s )
{
   int32 n; /* iterates over tree elements */

   /* Initialize the trees. */
   for( n = 0; n < L_CODES;  n++ ) s->dyn_ltree[n].Freq = 0;
   for( n = 0; n < D_CODES;  n++ ) s->dyn_dtree[n].Freq = 0;
   for( n = 0; n < BL_CODES; n++ ) s->bl_tree[n].Freq = 0;

   s->dyn_ltree[END_BLOCK].Freq = 1;
   s->opt_len = s->static_len = 0L;
   s->last_lit = s->matches = 0;
}

#define SMALLEST 1
/* Index within the heap array of least frequent node in the Huffman tree */


/* ===========================================================================
 * Remove the smallest element from the heap and recreate the heap with
 * one less element. Updates heap and heap_len.
 */
#define pqremove(s, tree, top) \
{\
    top = s->heap[SMALLEST]; \
    s->heap[SMALLEST] = s->heap[s->heap_len--]; \
    pqdownheap(s, tree, SMALLEST); \
}

/* ===========================================================================
 * Compares to subtrees, using the tree depth as tie breaker when
 * the subtrees have equal frequency. This minimizes the worst case length.
 */
#define smaller(tree, n, m, depth) \
   (tree[n].Freq < tree[m].Freq || \
   (tree[n].Freq == tree[m].Freq && depth[n] <= depth[m]))

/* ===========================================================================
 * Restore the heap property by moving down the tree starting at node k,
 * exchanging a node with the smallest of its two sons if necessary, stopping
 * when the heap property is re-established (each father smaller than its
 * two sons).
 */
static void pqdownheap(
                      pnge_deflate_state *s,
                      pnge_ct_data *tree,  /* the tree to restore */
                      uint32 k               /* node to move down */
                      )
{
   uint32 v = s->heap[k];
   uint32 j = k << 1;  /* left son of k */
   while( j <= s->heap_len )
   {
      /* Set j to the smallest of the two sons: */
      if( j < s->heap_len &&
          smaller(tree, s->heap[j+1], s->heap[j], s->depth) )
      {
         j++;
      }
      /* Exit if v is smaller than both sons */
      if( smaller(tree, v, s->heap[j], s->depth) ) break;

      /* Exchange v with the smallest son */
      s->heap[k] = s->heap[j];  k = j;

      /* And continue down the tree, setting j to the left son of k */
      j <<= 1;
   }
   s->heap[k] = v;
}







/* ===========================================================================
 * Compute the optimal bit lengths for a tree and update the total bit length
 * for the current block.
 * IN assertion: the fields freq and dad are set, heap[heap_max] and
 *    above are the tree nodes sorted by increasing frequency.
 * OUT assertions: the field len is set to the optimal bit length, the
 *     array bl_count contains the frequencies for each bit length.
 *     The length opt_len is updated; static_len is also updated if stree is
 *     not null.
 */
static void gen_bitlen(
                      pnge_deflate_state *s,
                      pnge_tree_desc *desc    /* the tree descriptor */
                      )
{
   pnge_ct_data *tree        = desc->dyn_tree;
   uint32 max_code         = desc->max_code;
   const pnge_ct_data *stree = desc->stat_desc->static_tree;
   const uint32 *extra    = desc->stat_desc->extra_bits;
   uint32 base             = desc->stat_desc->extra_base;
   uint32 max_length       = desc->stat_desc->max_length;
   uint32 h;              /* heap index */
   uint32 n, m;           /* iterate over the tree elements */
   uint32 bits;           /* bit length */
   uint32 xbits;          /* extra bits */
   uint16 f;              /* frequency */
   int32 overflow = 0;   /* number of elements with bit length too large */

   for( bits = 0; bits <= MAX_BITS; bits++ ) s->bl_count[bits] = 0;

   /* In a first pass, compute the optimal bit lengths (which may
    * overflow in the case of the bit length tree).
    */
   tree[s->heap[s->heap_max]].Len = 0; /* root of the heap */

   for( h = s->heap_max+1; h < HEAP_SIZE; h++ )
   {
      n = s->heap[h];
      bits = tree[tree[n].Dad].Len + 1;
      if( bits > max_length ) bits = max_length, overflow++;
      tree[n].Len = (uint16)bits;
      /* We overwrite tree[n].Dad which is no longer needed */

      if( n > max_code ) continue; /* not a leaf node */

      s->bl_count[bits]++;
      xbits = 0;
      if( n >= base ) xbits = extra[n-base];
      f = tree[n].Freq;
      s->opt_len += (uint32)f * (bits + xbits);
      if( stree ) s->static_len += (uint32)f * (stree[n].Len + xbits);
   }
   if( overflow == 0 ) return;

   Trace((stderr,"\nbit length overflow\n"));
   /* This happens for example on obj2 and pic of the Calgary corpus */

   /* Find the first bit length which could increase: */
   do
   {
      bits = max_length-1;
      while( s->bl_count[bits] == 0 ) bits--;
      s->bl_count[bits]--;      /* move one leaf down the tree */
      s->bl_count[bits+1] += 2; /* move one overflow item as its brother */
      s->bl_count[max_length]--;
      /* The brother of the overflow item also moves one step up,
       * but this does not affect bl_count[max_length]
       */
      overflow -= 2;
   } while( overflow > 0 );

   /* Now recompute all bit lengths, scanning in increasing frequency.
    * h is still equal to HEAP_SIZE. (It is simpler to reconstruct all
    * lengths instead of fixing only the wrong ones. This idea is taken
    * from 'ar' written by Haruhiko Okumura.)
    */
   for( bits = max_length; bits != 0; bits-- )
   {
      n = s->bl_count[bits];
      while( n != 0 )
      {
         m = s->heap[--h];
         if( m > max_code ) continue;
         if( tree[m].Len != (unsigned) bits )
         {
            Trace((stderr,"code %d bits %d->%d\n", m, tree[m].Len, bits));
            s->opt_len += (bits - tree[m].Len)*tree[m].Freq;
            tree[m].Len = (uint16)bits;
         }
         n--;
      }
   }
}









/* ===========================================================================
 * Generate the codes for a given tree and bit counts (which need not be
 * optimal).
 * IN assertion: the array bl_count contains the bit length statistics for
 * the given tree and the field len is set for all tree elements.
 * OUT assertion: the field code is set for all tree elements of non
 *     zero code length.
 */
static void gen_codes (
                      pnge_ct_data *tree,             /* the tree to decorate */
                      uint32 max_code,              /* largest code with non zero frequency */
                      uint16 *bl_count             /* number of codes at each bit length */
                      )
{
   uint16 next_code[MAX_BITS+1]; /* next code value for each bit length */
   uint16 code = 0;              /* running code value */
   uint32 bits;                  /* bit index */
   uint32 n;                     /* code index */

   /* The distribution counts are first used to generate the code values
    * without bit reversal.
    */
   for( bits = 1; bits <= MAX_BITS; bits++ )
   {
       /*lint -e734 */
      next_code[bits] = code = (code + bl_count[bits-1]) << 1;
   }
   /* Check that the bit counts in bl_count are consistent. The last code
    * must be all ones.
    */

   //Assert (code + bl_count[MAX_BITS]-1 == (1<<MAX_BITS)-1,
   //        "inconsistent bit counts");
   if( code + bl_count[MAX_BITS]-1 == (1<<MAX_BITS)-1 )
   {
      PNG_DEFLATE_MSG_FATAL( "ASSERTION failed in gen_codes() %d", code, 0, 0);
   }


   Tracev((stderr,"\ngen_codes: max_code %d ", max_code));

   for( n = 0;  n <= max_code; n++ )
   {
      int32 len = tree[n].Len;
      if( len == 0 ) continue;
      /* Now reverse the bits */
      tree[n].Code = bi_reverse(next_code[len]++, len);

      Tracecv(tree != pnge_static_ltree, (stderr,"\nn %3d %c l %2d c %4x (%x) ",
                                     n, (isgraph(n) ? n : ' '), len, tree[n].Code, next_code[len]-1));
   }
}












/* ===========================================================================
 * Construct one Huffman tree and assigns the code bit strings and lengths.
 * Update the total bit length for the current block.
 * IN assertion: the field freq is set for all tree elements.
 * OUT assertions: the fields len and code are set to the optimal bit length
 *     and corresponding code. The length opt_len is updated; static_len is
 *     also updated if stree is not null. The field max_code is set.
 */
static void build_tree(
                      pnge_deflate_state *s,
                      pnge_tree_desc *desc /* the tree descriptor */
                      )
{
   pnge_ct_data *tree         = desc->dyn_tree;
   const pnge_ct_data *stree  = desc->stat_desc->static_tree;
   uint32 elems             = desc->stat_desc->elems;
   uint32 n, m;          /* iterate over heap elements */
   uint32 max_code = 0xFFFFFFFFUL; /* largest code with non zero frequency */
   uint32 node;          /* new node being created */

   /* Construct the initial heap, with least frequent element in
    * heap[SMALLEST]. The sons of heap[n] are heap[2*n] and heap[2*n+1].
    * heap[0] is not used.
    */
   s->heap_len = 0, s->heap_max = HEAP_SIZE;

   for( n = 0; n < elems; n++ )
   {
      if( tree[n].Freq != 0 )
      {
         s->heap[++(s->heap_len)] = max_code = n;
         s->depth[n] = 0;
      }
      else
      {
         tree[n].Len = 0;
      }
   }

   /* The pkzip format requires that at least one distance code exists,
    * and that at least one bit should be sent even if there is only one
    * possible code. So to avoid special checks later on we force at least
    * two codes of non zero frequency.
    */
   while( s->heap_len < 2 )
   {
      node = s->heap[++(s->heap_len)] = (max_code < 2 ? ++max_code : 0);
      tree[node].Freq = 1;
      s->depth[node] = 0;
      s->opt_len--; if( stree ) s->static_len -= stree[node].Len;
      /* node is 0 or 1 so it does not have extra bits */
   }
   desc->max_code = max_code;

   /* The elements heap[heap_len/2+1 .. heap_len] are leaves of the tree,
    * establish sub-heaps of increasing lengths:
    */
   for( n = s->heap_len/2; n >= 1; n-- ) pqdownheap(s, tree, n);

   /* Construct the Huffman tree by repeatedly combining the least two
    * frequent nodes.
    */
   node = elems;              /* next internal node of the tree */
   do
   {
      pqremove(s, tree, n);  /* n = node of least frequency */
      m = s->heap[SMALLEST]; /* m = node of next least frequency */

      s->heap[--(s->heap_max)] = n; /* keep the nodes sorted by frequency */
      s->heap[--(s->heap_max)] = m;

      /* Create a new node father of n and m */
      tree[node].Freq = tree[n].Freq + tree[m].Freq;
      s->depth[node] = (uint8) (TREES_MAX(s->depth[n], s->depth[m]) + 1);
      tree[n].Dad = tree[m].Dad = (uint16)node;

      /* and insert the new node in the heap */
      s->heap[SMALLEST] = node++;
      pqdownheap(s, tree, SMALLEST);

   } while( s->heap_len >= 2 );

   s->heap[--(s->heap_max)] = s->heap[SMALLEST];

   /* At this point, the fields freq and dad are set. We can now
    * generate the bit lengths.
    */
   gen_bitlen(s, (pnge_tree_desc *)desc);

   /* The field len is now set, we can generate the bit codes */
   gen_codes ((pnge_ct_data *)tree, max_code, s->bl_count);
}








/* ===========================================================================
 * Scan a literal or distance tree to determine the frequencies of the codes
 * in the bit length tree.
 */
static void scan_tree (
                      pnge_deflate_state *s,
                      pnge_ct_data *tree,   /* the tree to be scanned */
                      uint32 max_code     /* and its largest code of non zero frequency */
                      )
{
   uint32 n;                     /* iterates over all tree elements */
   int32 prevlen = -1;          /* last emitted length */
   int32 curlen;                /* length of current code */
   int32 nextlen = tree[0].Len; /* length of next code */
   uint16 count = 0;             /* repeat count of the current code */
   int32 max_count = 7;         /* max repeat count */
   int32 min_count = 4;         /* min repeat count */

   if( nextlen == 0 ) max_count = 138, min_count = 3;
   tree[max_code+1].Len = (uint16)0xffff; /* guard */

   for( n = 0; n <= max_code; n++ )
   {
      curlen = nextlen; nextlen = tree[n+1].Len;
      if( ++count < max_count && curlen == nextlen )
      {
         continue;
      }
      else if( count < min_count )
      {
         s->bl_tree[curlen].Freq += count;
      }
      else if( curlen != 0 )
      {
         if( curlen != prevlen ) s->bl_tree[curlen].Freq++;
         s->bl_tree[REP_3_6].Freq++;
      }
      else if( count <= 10 )
      {
         s->bl_tree[REPZ_3_10].Freq++;
      }
      else
      {
         s->bl_tree[REPZ_11_138].Freq++;
      }

      count = 0; 
      prevlen = curlen;

      if( nextlen == 0 )
      {
         max_count = 138, min_count = 3;
      }
      else if( curlen == nextlen )
      {
         max_count = 6, min_count = 3;
      }
      else
      {
         max_count = 7, min_count = 4;
      }
   }
}






/* ===========================================================================
 * Send a literal or distance tree in compressed form, using the codes in
 * bl_tree.
 */
static void send_tree (
                      pnge_deflate_state *s,
                      pnge_ct_data *tree,     /* the tree to be scanned */
                      uint32 max_code       /* and its largest code of non zero frequency */ 
                      )
{
   uint32 n;                     /* iterates over all tree elements */
   int32 prevlen = -1;          /* last emitted length */
   int32 curlen;                /* length of current code */
   int32 nextlen = tree[0].Len; /* length of next code */
   int32 count = 0;             /* repeat count of the current code */
   int32 max_count = 7;         /* max repeat count */
   int32 min_count = 4;         /* min repeat count */

   /* tree[max_code+1].Len = -1; */  /* guard already set */
   if( nextlen == 0 ) max_count = 138, min_count = 3;

   for( n = 0; n <= max_code; n++ )
   {
      curlen = nextlen; nextlen = tree[n+1].Len;
      if( ++count < max_count && curlen == nextlen )
      {
         continue;
      }
      else if( count < min_count )
      {
         do
         {
            send_code(s, curlen, s->bl_tree);
         } while( --count != 0 );

      }
      else if( curlen != 0 )
      {
         if( curlen != prevlen )
         {
            send_code(s, curlen, s->bl_tree); count--;
         }

         //Assert(count >= 3 && count <= 6, " 3_6?");
         if( count >= 3 && count <= 6 )
         {
            PNG_DEFLATE_MSG_FATAL( "ASSERTION failed in send_tree() %d", count, 0, 0);
         }

         send_code(s, REP_3_6, s->bl_tree); send_bits(s, count-3, 2);

      }
      else if( count <= 10 )
      {
         send_code(s, REPZ_3_10, s->bl_tree); send_bits(s, count-3, 3);

      }
      else
      {
         send_code(s, REPZ_11_138, s->bl_tree); send_bits(s, count-11, 7);
      }
      count = 0; prevlen = curlen;
      if( nextlen == 0 )
      {
         max_count = 138, min_count = 3;
      }
      else if( curlen == nextlen )
      {
         max_count = 6, min_count = 3;
      }
      else
      {
         max_count = 7, min_count = 4;
      }
   }
}






/* ===========================================================================
 * Construct the Huffman tree for the bit lengths and return the index in
 * bl_order of the last bit length code to send.
 */
static uint32 build_bl_tree( pnge_deflate_state *s )
{
   uint32 max_blindex;  /* index of last bit length code of non zero freq */

   /* Determine the bit length frequencies for literal and distance trees */
   scan_tree(s, (pnge_ct_data *)s->dyn_ltree, s->l_desc.max_code);
   scan_tree(s, (pnge_ct_data *)s->dyn_dtree, s->d_desc.max_code);

   /* Build the bit length tree: */
   build_tree(s, (pnge_tree_desc *)(&(s->bl_desc)));
   /* opt_len now includes the length of the tree representations, except
    * the lengths of the bit lengths codes and the 5+5+4 bits for the counts.
    */

   /* Determine the number of bit length codes to send. The pkzip format
    * requires that at least 4 bit length codes be sent. (appnote.txt says
    * 3 but the actual value used is 4.)
    */
   for( max_blindex = BL_CODES-1; max_blindex >= 3; max_blindex-- )
   {
      if( s->bl_tree[bl_order[max_blindex]].Len != 0 ) break;
   }
   /* Update opt_len to include the bit length tree and counts */
   s->opt_len += 3*(max_blindex+1) + 5+5+4;
   Tracev((stderr, "\ndyn trees: dyn %ld, stat %ld",
           s->opt_len, s->static_len));

   return(max_blindex);
}





/* ===========================================================================
 * Send the header for a block using dynamic Huffman trees: the counts, the
 * lengths of the bit length codes, the literal tree and the distance tree.
 * IN assertion: lcodes >= 257, dcodes >= 1, blcodes >= 4.
 */
static void send_all_trees(
                          pnge_deflate_state *s,
                          uint32 lcodes, 
                          uint32 dcodes, 
                          uint32 blcodes
                          /* number of codes for each tree */
                          )
{
   uint32 rank;                    /* index in bl_order */

   //Assert (lcodes >= 257 && dcodes >= 1 && blcodes >= 4, "not enough codes");
   if( (lcodes >= 257) && (dcodes >= 1) && (blcodes >= 4) )
   {
      PNG_DEFLATE_MSG_FATAL( "ASSERTION failed in send_all_trees() %d %d %d", lcodes, dcodes, blcodes);
   }

   //Assert (lcodes <= L_CODES && dcodes <= D_CODES && blcodes <= BL_CODES,
   //        "too many codes");
   if( (lcodes <= L_CODES) && (dcodes <= D_CODES) && (blcodes <= BL_CODES) )
   {
      PNG_DEFLATE_MSG_FATAL( "ASSERTION failed in send_all_trees() %d %d %d", lcodes, dcodes, blcodes);
   }

   Tracev((stderr, "\nbl counts: "));
   send_bits(s, lcodes-257, 5); /* not +255 as stated in appnote.txt */
   send_bits(s, dcodes-1,   5);
   send_bits(s, blcodes-4,  4); /* not -3 as stated in appnote.txt */
   for( rank = 0; rank < blcodes; rank++ )
   {
      Tracev((stderr, "\nbl code %2d ", bl_order[rank]));
      send_bits(s, s->bl_tree[bl_order[rank]].Len, 3);
   }
   Tracev((stderr, "\nbl tree: sent %ld", s->bits_sent));

   send_tree(s, (pnge_ct_data *)s->dyn_ltree, lcodes-1); /* literal tree */
   Tracev((stderr, "\nlit tree: sent %ld", s->bits_sent));

   send_tree(s, (pnge_ct_data *)s->dyn_dtree, dcodes-1); /* distance tree */
   Tracev((stderr, "\ndist tree: sent %ld", s->bits_sent));
}






/* ===========================================================================
 * Send a stored block
 */
void pnge_tr_stored_block(
                    pnge_deflate_state *s,
                    char *buf,       /* input block */
                    uint32 stored_len,   /* length of input block */
                    int32 eof           /* true if this is the last block for a file */
                    )
{
   send_bits(s, (STORED_BLOCK<<1)+eof, 3);  /* send block type */
   copy_block(s, buf, (unsigned)stored_len, 1); /* with header */
}







/* ===========================================================================
 * Send one empty static block to give enough lookahead for inflate.
 * This takes 10 bits, of which 7 may remain in the bit buffer.
 * The current inflate code requires 9 bits of lookahead. If the
 * last two codes for the previous block (real code plus EOB) were coded
 * on 5 bits or less, inflate may have only 5+3 bits of lookahead to decode
 * the last real code. In this case we send two empty static blocks instead
 * of one. (There are no problems if the previous block is stored or fixed.)
 * To simplify the code, we assume the worst case of last real code encoded
 * on one bit only.
 */
void pnge_tr_align( pnge_deflate_state *s )
{
   send_bits(s, STATIC_TREES<<1, 3);
   send_code(s, END_BLOCK, pnge_static_ltree);
   bi_flush(s);
   /* Of the 10 bits for the empty block, we have already sent
    * (10 - bi_valid) bits. The lookahead for the last real code (before
    * the EOB of the previous block) was thus at least one plus the length
    * of the EOB plus what we have just sent of the empty static block.
    */
   if( 1 + s->last_eob_len + 10 - s->bi_valid < 9 )
   {
      send_bits(s, STATIC_TREES<<1, 3);
      send_code(s, END_BLOCK, pnge_static_ltree);
      bi_flush(s);
   }
   s->last_eob_len = 7;
}




/* ===========================================================================
 * Determine the best encoding for the current block: dynamic trees, static
 * trees or store, and output the encoded block to the zip file.
 */
void pnge_tr_flush_block(
                   pnge_deflate_state *s,
                   char *buf,       /* input block, or NULL if too old */
                   uint32 stored_len,   /* length of input block */
                   int32 eof          /* true if this is the last block for a file */
                   )
{
   uint32 opt_lenb, static_lenb; /* opt_len and static_len in bytes */
   uint32 max_blindex = 0;  /* index of last bit length code of non zero freq */

   /* Build the Huffman trees unless a stored block is forced */
   if( s->level > 0 )
   {

      /* Check if the file is ascii or binary */
      if( s->data_type == Z_UNKNOWN )
         set_data_type(s);

      /* Construct the literal and distance trees */
      build_tree(s, (pnge_tree_desc *)(&(s->l_desc)));
      Tracev((stderr, "\nlit data: dyn %ld, stat %ld", s->opt_len,
              s->static_len));

      build_tree(s, (pnge_tree_desc *)(&(s->d_desc)));
      Tracev((stderr, "\ndist data: dyn %ld, stat %ld", s->opt_len,
              s->static_len));
      /* At this point, opt_len and static_len are the total bit lengths of
       * the compressed block data, excluding the tree representations.
       */

      /* Build the bit length tree for the above two trees, and get the index
       * in bl_order of the last bit length code to send.
       */
      max_blindex = build_bl_tree(s);

      /* Determine the best encoding. Compute first the block length in bytes*/
      opt_lenb = (s->opt_len+3+7)>>3;
      static_lenb = (s->static_len+3+7)>>3;

      Tracev((stderr, "\nopt %lu(%lu) stat %lu(%lu) stored %lu lit %u ",
              opt_lenb, s->opt_len, static_lenb, s->static_len, stored_len,
              s->last_lit));

      if( static_lenb <= opt_lenb ) opt_lenb = static_lenb;

   }
   else
   {
      //Assert(buf != (char*)0, "lost buf");
      if( buf != (char*)0 )
      {
         PNG_DEFLATE_MSG_FATAL( "ASSERTION failed in pnge_tr_flush_block() ", 0, 0, 0);
      }

      opt_lenb = static_lenb = stored_len + 5; /* force a stored block */
   }

   if( stored_len+4 <= opt_lenb && buf != (char*)0 )
   {
      /* 4: two words for the lengths */

      /* The test buf != NULL is only necessary if LIT_BUFSIZE > WSIZE.
       * Otherwise we can't have processed more than WSIZE input bytes since
       * the last block flush, because compression would have been
       * successful. If LIT_BUFSIZE <= WSIZE, it is never too late to
       * transform a block into a stored block.
       */
      pnge_tr_stored_block(s, buf, stored_len, eof);

   }
   else if( static_lenb == opt_lenb )
   {
      send_bits(s, (STATIC_TREES<<1)+eof, 3);
      compress_block(s, (pnge_ct_data *)pnge_static_ltree, (pnge_ct_data *)pnge_static_dtree);
   }
   else
   {
      send_bits(s, (DYN_TREES<<1)+eof, 3);
      send_all_trees(s, s->l_desc.max_code+1, s->d_desc.max_code+1,
                     max_blindex+1);
      compress_block(s, (pnge_ct_data *)s->dyn_ltree, (pnge_ct_data *)s->dyn_dtree);
   }

   /* The above check is made mod 2^32, for files larger than 512 MB
    * and uint32 implemented on 32 bits.
    */
   init_block(s);

   if( eof )
   {
      bi_windup(s);
   }
   Tracev((stderr,"\ncomprlen %lu(%lu) ", s->compressed_len>>3,
           s->compressed_len-7*eof));
}


/* ===========================================================================
 * Send the block data compressed using the given Huffman trees
 */
static void compress_block(
                          pnge_deflate_state *s,
                          pnge_ct_data *ltree, /* literal tree */
                          pnge_ct_data *dtree  /* distance tree */
                          )
{
   unsigned dist;      /* distance of matched string */
   int32 lc;             /* match length or unmatched char (if dist == 0) */
   unsigned lx = 0;    /* running index in l_buf */
   unsigned code;      /* the code to send */
   uint32 extra;          /* number of extra bits to send */

   if( s->last_lit != 0 ) do
      {
         dist = s->d_buf[lx];
         lc = s->l_buf[lx++];
         if( dist == 0 )
         {
            send_code(s, lc, ltree); /* send a literal byte */
            Tracecv(isgraph(lc), (stderr," '%c' ", lc));
         }
         else
         {
            /* Here, lc is the match length - MIN_MATCH */
            code = pnge_length_code[lc];
            send_code(s, code+LITERALS+1, ltree); /* send the length code */
            extra = extra_lbits[code];
            if( extra != 0 )
            {
               lc -= pnge_base_length[code];
               send_bits(s, lc, extra);       /* send the extra length bits */
            }
            dist--; /* dist is now the match distance - 1 */
            code = d_code(dist);

            if (code >= D_CODES)
            {
              PNG_DEFLATE_MSG_FATAL( "ASSERTION failed in compress_block() %d", code, 0, 0);
              return;
            }
            
            send_code(s, code, dtree);       /* send the distance code */
            
            extra = extra_dbits[code];
            if( extra != 0 )
            {
               dist -= pnge_base_dist[code];
               send_bits(s, dist, extra);   /* send the extra distance bits */
            }
         } /* literal or match pair ? */
            /*lint -restore -e661 */
         /* Check that the overlay between pending_buf and d_buf+l_buf is ok: */

         //Assert(s->pending < s->lit_bufsize + 2*lx, "pendingBuf overflow");
         if( (unsigned) s->pending < (unsigned) s->lit_bufsize + 2*lx )
         {
            PNG_DEFLATE_MSG_FATAL( "ASSERTION failed in compress_block() %d %d", s->pending, s->lit_bufsize + 2*lx, 0);
         }

      } while( lx < s->last_lit );

   send_code(s, END_BLOCK, ltree);
   s->last_eob_len = ltree[END_BLOCK].Len;
}








/* ===========================================================================
 * Set the data type to ASCII or BINARY, using a crude approximation:
 * binary if more than 20% of the bytes are <= 6 or >= 128, ascii otherwise.
 * IN assertion: the fields freq of dyn_ltree are set and the total of all
 * frequencies does not exceed 64K (to fit in an int32 on 16 bit machines).
 */
static void set_data_type( pnge_deflate_state *s )
{
   int32 n = 0;
   unsigned ascii_freq = 0;
   unsigned bin_freq = 0;
   while( n < 7 )        bin_freq += s->dyn_ltree[n++].Freq;
   while( n < 128 )    ascii_freq += s->dyn_ltree[n++].Freq;
   while( n < LITERALS ) bin_freq += s->dyn_ltree[n++].Freq;
   s->data_type = (uint8)(bin_freq > (ascii_freq >> 2) ? Z_BINARY : Z_ASCII);
}









/* ===========================================================================
 * Reverse the first len bits of a code, using straightforward code (a faster
 * method would use a table)
 * IN assertion: 1 <= len <= 15
 */
static uint16 bi_reverse(
                          uint16 code, /* the value to invert */
                          int32 len       /* its bit length */
                          )
{
   register uint16 res = 0;
   do
   {
      res |= code & 1;
      code >>= 1, res <<= 1;
   } while( --len > 0 );
   return(res >> 1);
}






/* ===========================================================================
 * Flush the bit buffer, keeping at most 7 bits in it.
 */
static void bi_flush( pnge_deflate_state *s )
{
   if( s->bi_valid == 16 )
   {
      put_short(s, s->bi_buf);
      s->bi_buf = 0;
      s->bi_valid = 0;
   }
   else if( s->bi_valid >= 8 )
   {
      put_byte(s, (uint8)s->bi_buf);
      s->bi_buf >>= 8;
      s->bi_valid -= 8;
   }
}






/* ===========================================================================
 * Flush the bit buffer and align the output on a byte boundary
 */
static void bi_windup( pnge_deflate_state *s )
{
   if( s->bi_valid > 8 )
   {
      put_short(s, s->bi_buf);
   }
   else if( s->bi_valid > 0 )
   {
      put_byte(s, (uint8)s->bi_buf);
   }
   s->bi_buf = 0;
   s->bi_valid = 0;
}







/* ===========================================================================
 * Copy a stored block, storing first the length and its
 * one's complement if requested.
 */
static void copy_block(
                      pnge_deflate_state *s,
                      char    *buf,    /* the input data */
                      unsigned len,     /* its length */
                      int32      header   /* true if block header must be written */
                      )
{
   bi_windup(s);        /* align on byte boundary */
   s->last_eob_len = 8; /* enough lookahead for inflate */

   if( header )
   {
      put_short(s, (uint16)len);   
      put_short(s, (uint16)~len);
   }
   while( len-- )
   {
      put_byte(s, *buf++);
   }
}



#endif /* FEATURE_PNG_ENCODER   */

