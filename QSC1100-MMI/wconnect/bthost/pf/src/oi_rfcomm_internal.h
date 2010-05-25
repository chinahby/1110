#ifndef _RFCOMM_INTERNAL_H
#define _RFCOMM_INTERNAL_H
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_rfcomm_internal.h#1 $ 

$Log: $
*
*    #4        22 Oct 2005            JH
* Replaced FEATURE_PCAA with FEATURE_PCAA_SIRIUS.
*
*    #3        31 May 2005            RY
* Merged in Sirius changes.
*
===========================================================================*/
/**
@file

Internal interface to RFCOMM exposes APIs that are only available to profiles and other stack components.

*/
/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2003 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include "oi_mbuf.h"
#include "oi_stddefs.h"
#include "oi_rfcomm.h"

/**
 * A callback function of this type is called when the write to the MBUF has completed.
 */
typedef void (*OI_RFCOMM_WRITE_MBUF_CFM)(OI_RFCOMM_LINK_HANDLE link,
                                         OI_MBUF *mbuf,
                                         OI_STATUS result);

/**
 * Write to an MBUF.
 */
OI_STATUS OI_RFCOMM_WriteMBUF(OI_RFCOMM_WRITE_MBUF_CFM callback,
                              OI_RFCOMM_LINK_HANDLE link,
                              OI_MBUF *mbuf);

/**
 * Associates a caller defined context with a registered RFCOMM server channel.
 * This context can then be retrieved by calling OI_RFCOMM_GetServerContext.
 *
 * @param serverChannel   A valid rfcomm channel number for a registered RFCOMM
 *                        server.
 *
 * @param context         A value supplied by the caller.
 *
 * @return                OI_OK if the context was set, OI_STATUS_NOT_FOUND if
 *                        serverChannel is not valid.
 */
OI_STATUS OI_RFCOMM_SetServerContext(OI_UINT8 serverChannel,
                                     void *context);


/**
 * Gets a caller defined context from a registered RFCOMM server channel. This
 * is a value that we previously set by a call to OI_RFCOMM_SetServerContext.
 *
 * @param serverChannel   A valid rfcomm channel number for a registered RFCOMM
 *                        server.
 *
 * @return                A context pointer or NULL if the handle is invalid or
 *                        there is no context associated with this server
 *                        channel.
 */
void* OI_RFCOMM_GetServerContext(OI_UINT8 serverChannel);


/**
 * Associates a caller defined context with an RFCOMM link. This context can
 * then be retrieved by calling OI_RFCOMM_GetLinkContext.
 *
 * @param handle          The rfcomm link handle to associate the context with.
 *
 * @param context         A value supplied by the caller.
 *
 * @return                OI_OK if the context was set, OI_STATUS_NOT_FOUND if
 *                        the link handle is not valid.
 */
OI_STATUS OI_RFCOMM_SetLinkContext(OI_RFCOMM_LINK_HANDLE handle,
                                   void *context);


/**
 * Gets a caller defined context associate with an RFCOMM link. This is a value
 * that we previously set by a call to OI_RFCOMM_SetLinkContext.
 *
 * @param handle          The rfcomm link handle to get the context from.
 *
 * @return                A context pointer or NULL if the handle is invalid or
 *                        there is no context associated with this handle.
 */
void* OI_RFCOMM_GetLinkContext(OI_RFCOMM_LINK_HANDLE handle);

#if defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS)
#error code not present
#endif /* defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS) */

#endif /* _RFCOMM_INTERNAL_H */

