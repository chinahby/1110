#ifndef GSDI_PERSO_ENGINE_RUIM_H
#define GSDI_PERSO_ENGINE_RUIM_H
/*===========================================================================


        G S D I   P E R S O N A L I Z A T I O N   E N G I N E

     S T R U C T U R E S  A N D  F U N C T I O N  P R O T O T Y P E S


DESCRIPTION
  Structure and Function prototype definitions for the personalization
  engine.  The gsdi_perso.c functions are the entry point into the
  engine.  The engine is going to retrieve all of the information required
  to allow the security library to perform the security procedures.


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2001-2008 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/gsdi_perso_engine_ruim.h#1 $$ $DateTime: 2008/12/15 12:00:08 $ $Author: nkukreja $

when       who     what, where, why
--------   ---     --------------------------------------------------------
11/12/08   ssr     Removed compiler warnings
05/15/06   jk      Added definition for gsdi_perso_engine_get_esn
04/06/06   sun     Initial Revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#include "gsdi_exp.h"

#ifdef FEATURE_MMGSDI_PERSONALIZATION_ENGINE

/* Offsets used when taking data_p information and populating the cnf rsp */
#ifdef FEATURE_PERSO_RUIM

/*===========================================================================
FUNCTION GSDI_PERSO_ENGINE_CREATE_RUIM_DATA

DESCRIPTION
  This function is called to calcuate the total memory required for the
  SIM Items to be retrieved.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_perso_engine_create_ruim_data(int32       * returned_data_len_p,
                                                  const uint8 * data_p);

#ifndef FEATURE_MMGSDI_PERSO_SFS
/*===========================================================================
FUNCTION GSDI_PERSO_ENGINE_RUIM_FILE_TEST

DESCRIPTION
  This function is used to create the directories and files.  This function
  should only be called once during the personalization phase.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
gsdi_returns_T gsdi_perso_engine_ruim_fs_file_test(void);
#endif /*FEATURE_MMGSDI_PERSO_SFS */


/*===========================================================================
FUNCTION GSDI_PERSO_ENGINE_HANDLE_RUIM_REQUEST

DESCRIPTION
  This function is the main handler for all the perso requests.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
gsdi_returns_T gsdi_perso_engine_handle_ruim_request(
                                      gsdi_perso_engine_proc_enum_type feature_type,
                                      gsdi_perso_sec_client_req_type  *client_data,
                                      const void *                     req_p,
                                      uint8 *                          ret_data_p,
                                      int32*                           ret_data_len_p,
                                      gsdi_perso_event_enum_type       *perso_event_p);

/* ----------------------------------------------------------------------------
FUNCTION: GSDI_PERSO_ENGINE_GET_ESN

DESCRIPTION:
  Will retrieve the ESN number from NV.

DEPENDENCIES:
  None

LIMITATIONS:
  The memory allocated in this function will need to be released by the
  caller of this function.

RETURN TYPE:
  gsdi_returns_T

-----------------------------------------------------------------------------*/
gsdi_returns_T gsdi_perso_engine_get_esn (
                                      uint8 *  esn_data_p,
                                      uint32 * esn_len_p);

#endif /*FEATURE_PERSO_RUIM*/
#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */
#endif /* GSDI_PERSO_ENGINE_RUIM_H */
