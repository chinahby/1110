/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                 M M G S D I   U I M   U I C C   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the UICC protocol processing support for MMGSDI
  before the command is being sent to UIM Module.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2004 - 2009 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/mmgsdi_uim_uicc.c#4 $$ $DateTime: 2009/05/15 02:20:16 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/14/09   kp      Added compiler directive for demand Paging Changes
05/11/09   kp      Demand Paging Changes
04/29/09   js      Fixed compiler warning
09/17/08   ssr     Fixed memory free error from mmgsdi_uim_uicc_search_record
22/08/08   ssr     Removing compilation warning from mmgsdi_uim_uicc_search_record. 
08/08/08   ssr	   Add more detailed messages for dispalyed in the log for 
                   mmgsdi uicc search
07/31/08   ssr     Update search for blocking the search directions belonging 
                   to Record Pointer.
07/16/08   jk      Fixed compile errors
06/12/08   ssr     Support of MMGSDI Search functionality
06/20/08   sp      Add data offset messages for read
06/04/08   tml     Fixed warning on LTK
03/03/08   sun     Added support for OMA Bcast
08/14/07   sun     Added MUK_DEL Context
07/30/07   sun     Return Error for invalid context
07/27/07   sun     Fixed passing of pointer
07/02/07   sun     Fixed USIM Authenticate and implemented odd ins class
05/07/07   sp      Replaced calls to mem_malloc/free with mmgsdi_malloc/free
05/02/07   sun     Added support for Service Available and USIM Auth
03/07/07   sun     Added support for Disable with Replacement
01/13/07   tml     Add path support for Select
12/19/06   tml     Add support for path transparent and record access.  Add 
                   support for cyclic and linear fixed write
12/12/06   tml     Added indication whether the get_file_attribute will result
                   in activation or termination of app
10/26/06   tml     Get Key ref from client ID
08/14/06   tml     Allow select AID to go through on non default channel
08/07/06   sun     Store the PIN on any pin operation
08/03/06   tml     use channel from the client table for pin function
07/26/06   sun     Added support for Multi USIM App and MMGSDI PIN
06/08/06   tml     Fixed featurization
04/18/06   nk      Featurized code
02/14/06   nk      Merged for ARR caching and by path changes
01/17/05   tml     MMGSDI cleanup 
12/14/05   tml     MMGSDI Cleanup
11/03/05   tml     Fixed header
08/30/05   jar     Fix for sending wrong pointer to malloc function
08/30/05   jar     Added support for UICC Status Commands
07/26/05   sst     Added MMGSDIBT support
07/12/05   tml     Session Management and ISIM support
06/16/05   tml     Prototype change for transparent and record access
02/24/05   jk      Initial version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "intconv.h"
#include "mmgsdi_uim_uicc.h"
#include "mmgsdi_uicc_rsp.h"
#include "mmgsdi.h"
#include "uim.h"
#include "gsdi.h"
#include "mmgsdiutil.h"
#include "uimusim.h"

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */
