#ifndef _OI_MBUF_H
#define _OI_MBUF_H
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_mbuf.h#1 $ 

$Log: $
*
*    #5       18 Oct  2006           PR
*  Merged OI Code v3.0.14rev1revQ
*

===========================================================================*/
/** @file   
 This module implements a non-linear buffering scheme called "mbufs" that allows
 data to be passed down the stack layers without requiring data copying at each
 layer. The simple concept is that with an mbuf, a data packet is stored in
 multiple buffers rather than in a single contiguous data buffer. As each stack
 layer proceses data from the higher layers, data can be segmented and
 packetized without the need to actually copy any of the data. This improves
 performance by eliminating memory-to-memory copying and reduces total memory
 usage by elminating the need for each stack layer to allocate buffers to
 accomodate payloads passed in from the upper layers.
*/
/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include "oi_stddefs.h"
#include "oi_debug.h"


#define MBUF_INLINE_BUF_SIZE  8

typedef enum {
    MBUF_COPY,   /**< Data buffer is to be copied by mbuf (implies will free) */
    MBUF_FREE,   /**< Data buffer will be freed by mbuf when mbuf is freed */
    MBUF_KEEP    /**< Data buffer must not be freed by mbuf */
} OI_MBUF_DISPOSITION;

/**
 * Type definition for an mbuf cell. An mbuf cell has a pointer to a buffer and
 * a length.
 */

typedef struct {
    #ifdef  BROKEN_BITFIELD_SUPPORT
#error code not present
    #else
    OI_MBUF_DISPOSITION disposition : 8;
    #endif
    OI_UINT16 len;
    union {
        const OI_BYTE *ptr;
        OI_BYTE  data[MBUF_INLINE_BUF_SIZE];
    } buf;
} OI_MBUF_CELL;


#define DEFAULT_MBUF_CELLS    4

/**
 * Type definition for an mbuf.
 */
#ifdef OI_CODE
#error code not present
#else
#include "dsm.h"

typedef enum
{
  BT_PF_MBUF_OK_TO_FREE, /* default state, DSM item can be freed when MBUF is freed */
  BT_PF_MBUF_DSM_SENT    /* DSM item has been passed onto stack for transmission; do not free */
} bt_pf_mbuf_state_type;

typedef struct _OI_MBUF 
{
  dsm_item_type*        dsm_ptr;     /* pointer to underlying DSM item */
  void*                 context_ptr; /* pointer to data context */
  OI_BOOL               free;        /* true if entry is free */
  bt_pf_mbuf_state_type dsm_state;   /* state of underlying DSM item */
  struct _OI_MBUF *     parent;      /*< mbuf from upper layer, if any */
  struct _OI_MBUF *     next;        /*< pointer to next mbuf, if in a linked list */
  boolean               IsPristine;  /* True if no bytes pulled */
} OI_MBUF;
#endif /* OI_CODE */

/**
 * Initializes an mbuf that was declared as an automatic variable. The disposition
 * on the buffer is MBUF_KEEP.
 *
 * @param  mbuf  A pointer to an unitialized mbuf. This can be a pointer to an
 *               mbuf allocated as an automatic variable.
 *
 * @param  data  A data buffer to initialize the mbuf. This can be NULL in which
 *               case the mbuf will be initially empty.
 *
 * @param  len   The size of the data buffer.
 *
 * @param numCells  The number of cells in the mbuf. This must less than
 *                  DEFAULT_MBUF_CELLS.
 */

OI_STATUS OI_MBUF_Automatic(OI_MBUF *mbuf,
                            const OI_BYTE *data,
                            OI_UINT16 len,
                            OI_UINT8 numCells);


/**
 * Allocate and initialize an mbuf sized to accomodate the requested number of
 * cells.
 *
 * @param numCells the number of cells the mbuf can hold.
 *
 * @returns a pointer to a dynmically allocted mbuf or NULL if the mbuf could
 * not be allocated.
 */

OI_MBUF* OI_MBUF_Alloc(OI_UINT8 numCells);


/**
 * Helper function that wraps a buffer in an mbuf. Allocates a one cell mbuf initializes it to
 * reference a buffer. The mbuf must be freed by calling OI_MBUF_Free().
 *
 * @param  data  A data buffer to initialize the mbuf with.
 *
 * @param  len   The size of the data buffer.
 *
 * @param disposition specifies what mbuf should do with the data buffer when
 *                    the mbuf is freed.
 *
 * @return a pointer to a dynamically allocated mbuf and initialized mbuf 
 *         or NULL if the mbuf could not be allocated.
 */

OI_MBUF* OI_MBUF_Wrap(const OI_BYTE *data,
                      OI_UINT16 len,
                      OI_MBUF_DISPOSITION disposition);


/**
 * Free an mbuf and any data that is not flagged to be kept.
 *
 * @param mbuf a pointer to a dynamically allocated mbuf.
 */

void OI_MBUF_Free(OI_MBUF *mbuf);

/**
 * Add a cell to the front of an mbuf. This function will typically be used to
 * add a packet header to a payload passed from a higher layer. Prepend is only
 * allowed on a pristine mbuf, that is an mbuf that has not had data pulled from
 * it.
 *
 * @param mbuf a pointer to an initialized mbuf.
 *
 * @param buf a pointer to a data buffer.
 *
 * @param len the length of the data buffer.
 *
 * @param disposition specifies what mbuf should do with the data buffer. There
 * are three possibilities:
 *
 * - MBUF_COPY indicates that mbuf must copy the buffer. Use this if the
 *   data buffer is an automatic variable. Mbuf will free any allocated memory
 *   when the mbuf itself is freed.
 *
 * - MBUF_FREE indicates that mbuf that caller want mbuf to eventually free
 *   the buffer. Obviously the buffer must not also be freed by the caller.
 *
 * - MBUF_KEEP indicates that mbuf must not free or otherwise modify the
 *   buffer.
 * 
 * @return OI_OK is the buffer was succesfully added or and
 * OI_STATUS_MBUF_OVERFLOW if there are no free cells in the mbuf.
 */

OI_STATUS OI_MBUF_Prepend(OI_MBUF* mbuf,
                          const OI_BYTE* buf,
                          OI_UINT16 len,
                          OI_MBUF_DISPOSITION disposition);

/**
 * Add a cell to the end of an mbuf. This function will typically be used to add
 * a packet trailer to a payload passed from a higher layer. Append is only
 * allowed on a pristine mbuf, that is an mbuf that has not had data pulled from
 * it.
 *
 * @param mbuf a pointer to an initialized mbuf.
 *
 * @param buf a pointer to a data buffer.
 *
 * @param len the length of the data buffer.
 *
 * @param disposition specifies what mbuf should do with the data buffer. There
 * are three possibilities:
 *
 * - MBUF_COPY indicates that mbuf must copy the buffer. Use this if the data
 *   buffer is an automatic variable. Mbuf will free any allocated memory when
 *   the mbuf itself is freed.
 *
 * - MBUF_FREE indicates that mbuf that caller want mbuf to eventually free the
 *   buffer. Obviously the buffer must not also be freed by the caller.
 *
 * - MBUF_KEEP indicates that mbuf must not free or otherwise modify the buffer.
 * 
 * @return OI_OK is the buffer was succesfully added or and
 * OI_STATUS_MBUF_OVERFLOW if there are no free cells in the mbuf.
 */

OI_STATUS OI_MBUF_Append(OI_MBUF* mbuf,
                         const OI_BYTE* buf,
                         OI_UINT16 len,
                         OI_MBUF_DISPOSITION disposition);


/**
 * Create a new mbuf from an existing mbuf creating references in the new mbuf
 * to the data buffers in the cells from the parent mbuf. Additional cells can
 * be added to the clone to allow new cells to be appended or prepended. The
 * parent mbuf must not be freed until the all mbufs cloned from it are no
 * longer in use.  The mbuf clone keeps a pointer to the parent mbuf that can be
 * retrieved by calling OI_MBUF_GetParent().
 *
 * @param parent a pointer to an initialized mbuf.
 *
 * @param headroom how many free cells will be available in the new mbuf.
 *
 * @returns a new mbuf if the operation succeeds, or NULL if memory could not be
 * alloced for the new mbuf.
 */

OI_MBUF* OI_MBUF_Clone(OI_MBUF *parent,
                       OI_UINT8 headroom);

/**
 * Transfers data from one mbuf to another. The operation does not destroy data
 * in the source mbuf, instead adjusting internal pointers that keep track of
 * the current position in the source data buffers. No data buffers are copied
 * either, the destination mbuf has pointers to data in the source mbuf.  The
 * source mbuf must not be freed until the destination mbuf is no longer in use.
 * Sucessive pulls will advance the internal state as if the data pulled were
 * removed from the source mbuf. The source mbuf state can be restored
 * OI_MBUF_Reset().
 *
 * @param dest A pointer to an initialized mbuf. After the operation this mbuf
 *             will contain numBytes of data.  This destination mbuf must
 *             contain sufficient cells to allow the desired number of bytes to
 *             be pulled.  In the absence of special knowledge about the source
 *             mbuf, the destination mbuf should have at least as many cells as
 *             the source mbuf.
 *
 * @param src  A pointer to a mbuf.
 *
 * @param numBytes  The maximum number of bytes to be pulled from the src
 *                  mbuf.
 *
 * @returns  The number of bytes actuall pulled.
 */

OI_UINT16 OI_MBUF_PullMBuf(OI_MBUF *dest,
                           OI_MBUF *src,
                           OI_UINT16 numBytes);


/**
 * Copies bytes from an mbuf info a contiguous buffer. The data in the source
 * mbuf is not modified by this operation but the state of the source mbuf is
 * updated to reflect the data that has been pulled. Sucessive pulls will
 * advance the internal state as if the data pulled were removed from the source
 * mbuf. The source mbuf state can be restored OI_MBUF_Reset().
 *
 * @param dest a pointer to a buffer at least numBytes long.
 *
 * @param src is a pointer to a mbuf.
 *
 * @param numBytes is the maximum number of bytes to be pulled from the mbuf.
 *
 * @returns  The number of bytes pulled.
 */

OI_UINT16 OI_MBUF_PullBytes(OI_BYTE *dest,
                            OI_MBUF *src,
                            OI_UINT16 numBytes);


/**
 * Projects a "window" on an mbuf defined by a start position and a length. A
 * window sets the internal state on an mbuf so that the calls to pull data from
 * the mbuf will pull data from the range defined by the window.  If the window
 * start position + the window length is beyond the actual end of the mbuf, the
 * length is set to its maximum possible value. If the start position is beyond
 * the end of the mbuf the window will be empty. After a window has been set,
 * OI_MBUF_NumBytes() will return the size of the window. To reset the mbuf call
 * OI_MBUF_Reset().
 *
 * @param mbuf A pointer to an mbuf.
 *
 * @param  winStart  The offset in the mbuf to the start of the window.
 *
 * @param  winSize   The size of the window.
 *
 * @returns  The actual window size, which may be smaller than the window size
 *           specified if there are fewer than winStart + winSize bytes in the
 *           mbuf.
 */

OI_UINT16 OI_MBUF_SetWindow(OI_MBUF *mbuf,
                            OI_UINT16 winSize);


/**
 * This function projects a new window on an mbuf that starts where the previous window ended.
 * Prior to this call, OI_MBUF_SetWindow() must have been called to set an
 * initial window. To segment a packet into fixed-size frames, repeatedly call
 * this function until it returns 0. 
 *
 * @param mbuf     the mbuf 
 * @param winSize  the size of the new window
 *
 * @returns  The actual window size, which may be smaller than the window size
 *           specified if there are fewer than winSize bytes between the current
 *           position and the end of the mbuf.
 */

OI_UINT16 OI_MBUF_AdvanceWindow(OI_MBUF *mbuf,
                                OI_UINT16 winSize);


/**
 * Undoes the effect of "pulling" from or "windowing" an mbuf. All data in
 * the mbuf is available again and OI_MBUF_NumBytes() will return the total size
 * of all data in all cells.
 *
 * @param mbuf the mbuf to be reset.
 */

void OI_MBUF_Reset(OI_MBUF *mbuf);


/**
 * Coalesces all of the data from the mbuf into one contiguous buffer. If only
 * one cell exists within the buffer, operation is optimized by returning a
 * byte pointer to the existing location of the data. Otherwise, a contiguous
 * buffer is allocated, and the data is pulled into the new buffer. This new
 * buffer is owned by the mbuf, and must be freed by calling OI_MBUF_CoalesceFree.
 *
 * Pointer returned is to data exactly as if it had been constructed by OI_MBUF_PullBytes.
 *
 * @param mbuf the mbuf to be coalesced.
 *
 * @returns  the OI_BYTE pointer to the coalesced data
 */

OI_BYTE * OI_MBUF_Coalesce(OI_MBUF *mbuf);

/**
 * Frees any dynamic memory allocated by OI_MBUF_Coalesce. As the coalesced
 * buffer is owned by the mbuf, the owning mbuf must be the first parameter.
 *
 * @param mbuf the mbuf which owns the coalesced buffer
 * @param buf  the buffer allocated by OI_MBUF_Coalesce
 */

void OI_MBUF_CoalesceFree(OI_MBUF *mbuf, OI_BYTE *buf);


/**
 * Obtain a pointer to a contiguous representaiton of the mbuf.  This
 * pointer must be freed when it is no longer in use.
 *
 */
OI_BYTE *OI_MBUF_ToBytes(OI_MBUF *src);


/**
 * Returns the parent mbuf of a child mbuf.
 */

#define OI_MBUF_Parent(mb)   (((mb) == NULL) ? NULL : (mb)->parent)

/**
 * Returns the aggregate number of bytes in the cells of an mbuf.
 */
#ifdef OI_CODE
#error code not present
#else
extern OI_UINT16 _OI_MBUF_NumBytes(OI_MBUF*);
#define OI_MBUF_NumBytes(mb) ( _OI_MBUF_NumBytes(mb) )
#endif /* OI_CODE */
/**
 * Returns the maximum cell capacity of an mbuf.
 */

#define OI_MBUF_NumAllocCells(mb) ((OI_UINT8) (((mb) == NULL) ? 0 : (mb)->allocCells))

/**
 * Returns the number of avaiable cells in an mbuf.
 */

#define OI_MBUF_NumFreeCells(mb) ((OI_UINT8) (((mb) == NULL) ? 0 : ((mb)->allocCells - ((mb)->usedCells))))

/**
 * Returns the number of used cells in an mbuf.
 */

#define OI_MBUF_NumUsedCells(mb) ((OI_UINT8) (((mb) == NULL) ? 0 : (mb)->usedCells))

/**
 * Determines if the mbuf is pristine (i.e. has not had anything pulled from it).
 */
#ifdef OI_CODE
#error code not present
#else /* OI_CODE */
#define OI_MBUF_IsPristine(mb)  ( (mb)->IsPristine )
#endif /* OI_CODE */

#ifdef OI_DEBUG
#error code not present
#endif /* OI_DEBUG */


/*****************************************************************************/
#endif /* _OI_MBUF_H */

