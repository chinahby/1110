#ifndef SECIOVEC_H
#define SECIOVEC_H
/*===========================================================================

                      S E C U R I T Y    S E R V I C E S
 
                        I O V E C   U T I L I T I E S

                  I N T E R N A L    H E A D E R    F I L E

DESCRIPTION
These are some utilities which are used for sec iovec manipulation.  For
Now there is no dynamic allocation for growing/shrinking of vectors.  This
could change in the future.


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


 Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
  $PVCSPath: $
  $Header: //source/qcom/qct/core/securemsm/crypto/main/latest/src/seciovec.h#1 $ 
  $DateTime: 2007/06/26 13:17:46 $ $Author: jmiera $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/22/06   df      changed fnames to reflect sec instead of dsm
11/28/05   df      Allow trimming to 0 len
08/31/05   df      Initial revision
===========================================================================*/



#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */

#ifdef FEATURE_SEC

#include "memory.h"
#include "dsm.h"

typedef struct {
    void   *pvBase;             // Starting address
    uint32  dwLen;              // Length in bytes
}IOVEC;

typedef struct {
  IOVEC  *iov;                  // pointer to iovec table
  uint32 size;                  // num of iovecs, size of iovec table
  uint32 max_size;              // max num of iovecs that can be allocated
  uint32 tot_byte_cnt;          // total byte count
  uint32 offset;                // starting data byte
  uint32 ref_cnt;               // future use of reference count
}IOVEC_T;


/* forward declarations for iovec functions */
uint32 iovect_len(IOVEC_T *iovt);
uint32 iovect_extract(uint8 *dest, IOVEC_T *iovt, uint32 num);
uint32 iovect_trunc(IOVEC_T *iovt);
int iovect_trim(IOVEC_T *iovt);
uint32 iovect_free(IOVEC_T *iovt);
uint32 iovect_prepend(IOVEC_T* iovt, uint8* src, uint32 num);
uint32 iovect_append(IOVEC_T* iovt, uint8* src, uint32 num);
uint32 iovect_write(IOVEC_T* iovt, uint8* src, uint32 num);
uint32 sec_dsm_item_count(dsm_item_type *dsm_item);
uint32 sec_dsm_item_ncount(dsm_item_type *dsm_item, uint16 dsm_len);
uint16 sec_dsm_2iovect(dsm_item_type *dsm_item, uint16 dsm_len, IOVEC_T *iovt);

#endif // FEATURE_SEC
#endif // SECIOVEC_H
