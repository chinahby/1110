#ifndef DSM_H
#define DSM_H
/*===========================================================================

                                  D S M . H

DESCRIPTION
  This file contains types and declarations associated with the DMSS Data
  Service Memory pool and services.

Copyright (c) 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

#define DSM_MAJOR_VER_NO 3
#define DSM_MINOR_VER_NO 0

/*===========================================================================
                            EDIT HISTORY FOR FILE
                                      
  $Header: //depot/asic/msmshared/services/dsm/dsm.h#92 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
1/1/06      pjb    Created new dsm.h.  
===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "queue.h"

#include "dsm_item.h"
#include "dsm_init.h"
#include "dsm_kind.h"

#include "dsm_queue.h"

#ifdef DSM_TRACER
  #include "dsm_tracer.h"
#endif


#endif /* DSM_H */
