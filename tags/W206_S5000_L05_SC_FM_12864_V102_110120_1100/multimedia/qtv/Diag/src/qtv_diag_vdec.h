#ifndef QTV_DIAG_VDEC_H
#define QTV_DIAG_VDEC_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

          QTV Diagnostic Packet Service Client Routines

General Description
  This file contains declaration for QTV Diag test commands via the 
  DMSS diagnostics packet service.

Copyright (c) 2002-2004 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "qtv_diag_main.h"
#include "vdecoder_types.h"

/*===========================================================================
*  Type definitions                                                         =
*==========================================================================*/
typedef enum
{
  FRAME=0,TIMESTAMP
} unit_type;

typedef PACKED struct
{
  unit_type units;
  uint32 start;
  uint32 stop;
} record_type;

typedef PACKED struct 
{
  diagpkt_subsys_header_type header;
  uint8                      api_id;
  record_type                rec_info;
  uint8                      bufsize;
  char                       buffer[QTV_MAX_URN_BYTES];
} vdec_diag_cmd_type;

typedef struct
{
  VDEC_CB_STATUS error;
  unsigned char     wr_status;
} vdec_diag_callback_status_type;

/*===========================================================================

FUNCTION QTVDIAG_HANDLE_VDEC_API_CMD

DESCRIPTION
  Function called in the Diag context used to handle packets from diag which
  have the sub command ID set to VDEC_API. This function then looks further into
  the command to see which specific VDEC APIs is to be tested. Results in the 
  direct calling of VDEC APIs.

DEPENDENCIES
  None.

RETURN VALUE
  PACKED void * return to Diag - of no consequence to us, makes diag happy that
  the signal was handled

SIDE EFFECTS
  None
===========================================================================*/
PACKED void * qtvdiag_handle_vdec_api_cmd( PACKED void *, word );

/*===========================================================================

FUNCTION QTVDIAG_VDEC_CLEANUP

DESCRIPTION
  Function called from qtvdiag_end_task, to ensure any memory used by
  qtvdiag_vdec is freed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
VDEC_ERROR qtvdiag_vdec_cleanup( vdec_diag_cmd_type * );

#endif /*QTV_DIAG_VDEC_H*/
