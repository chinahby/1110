/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                    M M G S D I   U I M   R S P   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the UIM response processing support for MMGSDI.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2004 - 2008 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/mmgsdi_sap_rsp.c#3 $$ $DateTime: 2009/05/15 02:20:16 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/14/09   kp      Added compiler directive for demand Paging Changes
05/11/09   kp      Demand Paging Changes
11/05/08   ssr     Resolved compiler error for RUIM enablement
06/23/08   tml     Added featurization for BT in WM
06/04/08   tml     Added sw to cnf header and messages cleanup
11/15/07   tml     Use regular rex function for critical sections for 
                   mmgsdi_task_rsp_q to prevent gsdi task stuck when synchronous
                   commands were issued
09/23/07   sk      Put a lock around mmgsdi_task_rsp
09/10/07   tml     Used generic get ATR and card reader functions and removed
                   channel atr function from the file.
03/19/07   sun     Fixed Lint Error
07/26/06   sun     Lint Fix
07/07/06   sun     Lint Fixes
06/12/06   sun     Set mmgsdi_sap_state  on Power Off and Power On
12/14/05   tml     MMGSDI Cleanup
12/05/05   sun     Added support for MMGSDI_SAP_CARD_READER_STATUS_REQ and 
                   MMGSDI_SAP_RESET_REQ
11/16/05   sun     Propagated mmgsdi_status back to the clients
11/09/05   sun     Fixed Lint Errors
11/03/05   tml     Fixed header
10/28/05   sun     Fixed Sap Disconnect
10/20/05   sun     Added support for BT Deregisteration
08/26/05   sun     Added support for BT SAP
07/28/05   sst     Fixed compile errors when SAP feature is not defined
06/06/05   sst     Initial version

===========================================================================*/


#include "comdef.h"
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */
