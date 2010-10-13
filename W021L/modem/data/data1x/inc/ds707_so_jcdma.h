#ifndef DS707_SO_JCDMA_H
#define DS707_SO_JCDMA_H
/*===========================================================================

                          D S 7 0 7 _ J C D M A _ S O
GENERAL DESCRIPTION

 Copyright (c) 2002 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_so_jcdma.h_v   1.4   23 Oct 2002 10:09:26   sramacha  $
  $Header: //source/qcom/qct/modem/data/1x/jcdma/main/lite/inc/ds707_so_jcdma.h#1 $ $DateTime: 2008/04/11 07:02:00 $ $Author: nsivakum $
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/28/03   ak      Added extern for func to set page_rsp
10/22/02   rsl    Updated FEATURE_JCDMA_DS_1X to FEATURE_JCDMA_1X
10/15/02   ak      Updated FEATURE_JCDMA_DS to FEATURE_JCDMA_DS_1X
10/10/02   ak      Updated for JCDMA M51/1X.
02/20/02   ak      First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_JCDMA_1X
#include "ds707.h"

/*===========================================================================
                        FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_SO_JCDMA_PKT_SET_PAGE_RSP

DESCRIPTION   JCDMA Pkt SO's

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_so_jcdma_pkt_set_page_rsp(void);

/*===========================================================================
FUNCTION      DS707_SO_JCDMA_PKT_SET__STATE

DESCRIPTION   Enables/disables packet service options in SNM, based on
              current settings.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_so_jcdma_pkt_set_state(void);

/*===========================================================================
FUNCTION      DS707_SO_JCDMA_PKT_GET_ORIG

DESCRIPTION   Returns the packet service option to originate with.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
ds707_so_type ds707_so_jcdma_pkt_get_orig
(
  ds707_rateset_type         rateset
);

/*===========================================================================
FUNCTION      DS707_SO_JCDMA_ASYNC_SET_STATE

DESCRIPTION   Lookup the state of each async data service option based on the
              current service option set in use. Enable or disable each async
              data service option as per the table.

DEPENDENCIES  Uses the global variable ds_atcop_qcso_val to determine the
              current setting of the service option set.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_so_jcdma_async_set_state(void);

/*===========================================================================
FUNCTION      DS707_SO_JCDMA_ASYNC_GET_ORIG

DESCRIPTION   Returns the async service option to originate with.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
ds707_so_type ds707_so_jcdma_async_get_orig
(
  ds707_rateset_type         rateset
);

/*===========================================================================
FUNCTION      DS707_SO_JCDMA_G3FAX_SET_STATE

DESCRIPTION   Lookup the state of each g3 fax service option based on the
              current Service Option Set in use. Enable or disable each G3
              fax service option as per the table.

DEPENDENCIES  Uses the global variable ds_atcop_qcso_val to determine the
              current setting of the service option set.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_so_jcdma_g3fax_set_state(void);

/*===========================================================================
FUNCTION      DS707_SO_JCDMA_G3FAX_GET_ORIG

DESCRIPTION   Returns the G3/FAX service option to originate with.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
ds707_so_type ds707_so_jcdma_g3fax_get_orig
(
  ds707_rateset_type         rateset
);

/*===========================================================================
FUNCTION      DS707_SO_JCDMA_VERIFY_INC_PKT_SO

DESCRIPTION   Given an SO, returns TRUE or FALSE, indicating if the SO
              should be accepted or rejected.  

DEPENDENCIES  None.

RETURN VALUE  TRUE - accept SO.   FALSE - Reject SO

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_so_jcdma_verify_inc_pkt_so
(
  ds707_so_type     inc_so
);

/*===========================================================================
FUNCTION      DS707_SO_JCDMA_VERIFY_INC_ASYNC_FAX_SO

DESCRIPTION   Given an SO, returns TRUE or FALSE, indicating if the SO
              should be accepted or rejected.  

DEPENDENCIES  None.

RETURN VALUE  TRUE - accept SO.   FALSE - Reject SO

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_so_jcdma_verify_inc_async_fax_so
(
  ds707_so_type     inc_so
);

#endif /* FEATURE_JCDMA_1X */
#endif /* DS707_SO_JCDMA_H */

