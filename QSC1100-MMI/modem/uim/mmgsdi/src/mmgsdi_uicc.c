/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                    M M G S D I   U I C C   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the UICC protocol processing support for MMGSDI.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2004 - 2009 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/mmgsdi_uicc.c#5 $$ $DateTime: 2009/05/12 04:14:02 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/12/09   kk      Fixed PIN cmd not sent to card in case UPIN is disabled
04/29/09   js      Fixed compiler warnings
04/20/09   kk      Removed featurization which lead to path thrashing
01/21/09   kk      Featurize function call which checks for EF init in
                   gsdi_usim_path_lookup_table for EF access
09/25/08   js      Fixed Klockwork Error
09/19/08   js      Fixed mmgsdi_uicc_read logic
09/06/08   kk      Changed pkcs15 command handling
08/28/08   sun     If command fails in the original state, then return original
                   error mmgsdi status to client
08/20/08   kk      Calls in create_pkcs15_table made to async
08/06/08   tml     Fixed klocwork issue
07/31/08   kk      Fixing issues related to path length and path in pkcs table
07/20/08   sp      Update the cache properly for read on record based files
07/17/08   kk      Added support for PKCS 15
06/12/08   ssr     Support of MMGSDI Search functionality
06/23/08   sp      Don't clean up the global that was populated as a result
                   of EF-DIR aid read during the reselection of an app.
06/20/08   sp      Add data offset messages for read
03/24/08   sun     Added support for OMA Bcast
03/13/08   tml     fixed memory leak
10/02/07   tml     Added proprietary app session support
07/27/07   sun     Fixed Service Available Calculation
05/07/07   sp      Replaced calls to mem_malloc/free with mmgsdi_malloc/free
05/02/07   sun     Added support for Service Available and USIM Auth
03/19/07   sun     Fixed Lint Error
03/07/07   sun     Added support for Disable with Replacement
03/02/07   sun     Check for file Attributes before proceeding to read/write
02/01/07   tml     Null pointer check
01/16/07   tml     Reset mmgsdi status to MMGSDI_SUCCESS after
                   mmgsdi_util_access_path_lookup_table
01/16/07   sk      Return MMGSDI_NOT_FOUND if file is not init in path lookup
                   table.
01/13/07   tml     Add path support for Select
12/19/06   tml     Add support for path transparent and record access
12/12/06   tml     Added indication whether the get_file_attribute will result
                   in activation or termination of app
10/20/06   tml     Check if default application before accessing the cache
08/20/06   tml     Fixed Select AID issue
08/07/06   tml     Added ACL support
08/03/06   tml     Passed in generic_data client ID for get pin info
07/26/06   sun     Added support for Multi USIM App and MMGSDI PIN
06/07/06   tml     Fixed featurization issue
05/23/06   tml     bdn support
04/18/06   nk      Featurized code
02/15/06   tml     Used read_cache_len to get the cached item len in case
                   user passed in len of 0
12/14/05   tml     MMGSDI Cleanup
11/03/05   tml     Fixed header
09/29/05   tml     Greater than 512 byte Cache Read
08/30/05   jar     Added support to send a UICC Status Command
07/26/05   sst     Fixed lint errors
07/18/05   jk      Fixed Compile Issues for 1x Targets
06/16/05   tml     Prototype change for transparent and record access
02/24/05   tml     Initial version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "intconv.h"
#include "mmgsdi_uicc.h"
#include "mmgsdi_uim_uicc.h"
#include "mmgsdi.h"
#include "mmgsdiutil.h"

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /*FEATURE_MMGSDI_UMTS*/

