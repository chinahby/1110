#ifndef _L2CAP_MBUF_H
#define _L2CAP_MBUF_H
/**
@file
MBUF-specific interface to L2CAP
*/
/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include "oi_mbuf.h"
#include "oi_stddefs.h"


/** \addtogroup L2CAP L2CAP APIs */
/**@{*/

/** A callback function of this type is called when the write to the MBUF has completed. */
typedef void (*L2CAP_WRITE_MBUF_CFM)(OI_L2CAP_CID cid,
                                     OI_MBUF *mbuf,
                                     OI_STATUS result);
/** Write to an MBUF. */
#ifndef OI_CODE
OI_STATUS OI_L2CAP_WriteMBUF(L2CAP_WRITE_MBUF_CFM callback,
                             OI_L2CAP_CID cid,
                             OI_MBUF *mbuf,
                             void *frame_handle);
#else
#error code not present
#endif
/**
 * Cancel the write of a specific MBUF on a specific CID.
 *
 * @param cid   the cid to search for
 * @param mbuf  the mbuf to cancel
 */
void OI_L2CAP_CancelWriteMBuf(OI_L2CAP_CID cid,
                              OI_MBUF *mbuf);

#endif /* _L2CAP_MBUF_H */

/**@}*/
