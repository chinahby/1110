/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                    M M G S D I   U I M   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the UICC protocol processing support for MMGSDI.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2004 - 2006, 2008 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/mmgsdi_sap.c#1 $$ $DateTime: 2008/12/15 12:00:08 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/05/08   ssr     Resolved compiler error for RUIM enablement
09/22/08   nb      Resolved compiler warning
06/23/08   tml     Added featurization for BT in WM
09/10/07   tml     Used generic get ATR and card reader status
05/07/07   sp      Replaced calls to mem_malloc/free with mmgsdi_malloc/free
07/26/06   sun     Lint Fixes and Clean up
07/10/06   tml     compilation fix
07/07/06   sun     Lint Fixes
06/27/06   sun     Send UIM_RESET_SWITCH_UIM_PASSIVE_F during SAP_RESET
                   Fixed Alignment
02/22/06   tml     lint fix
12/20/05   sun     Populated extra_param for Sap_Disconnect and return proper
                   errors on invalid state
12/14/05   tml     MMGSDI Cleanup
12/05/05   sun     Added support for MMGSDI_SAP_CARD_READER_STATUS_REQ and
                   MMGSDI_SAP_RESET_REQ
11/09/05   sun     Fixed Lint Errors
11/03/05   tml     Fixed header
11/03/05   sun     On Disconnect, do not do a refresh immediately
10/28/05   sun     Fixed Sap Disconnect
10/20/05   sun     Added support for BT Deregisteration
08/29/05   tml     Fixed compilation
08/26/05   tml     Fixed compilation issue
08/26/05   sun     Added support for BT SAP
08/25/05   pv      Moved functions and data types relating to refresh to the
                   mmgsdi_gen module
07/28/05   sst     Fixed compile errors when SAP feature is not defined
07/28/05   sst     Fixed compile errors when SAP feature is not defined
06/06/05   sst     Initial version

===========================================================================*/


#include "comdef.h"
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */
