#ifdef FEATURE_UIM_JCDMA_RUIM_SUPPORT
#ifndef MMGSDI_RUIM_JCDMA_H
#define MMGSDI_RUIM_JCDMA_H
/*===========================================================================


           M M G S D I   U I M   H E A D E R S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2007 QUALCOMM, Incorporated and its licensors.  All
Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/mmgsdi_ruim_jcdma.h#1 $$ $DateTime: 2008/12/15 12:00:08 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
05/17/07   nk      First revision

=============================================================================*/

#include "mmgsdi.h"
#include "mmgsdilib.h"

/*===========================================================================
FUNCTION MMGSDI_PROCESS_JCDMA_CARD_GET_INFO_CMD

DESCRIPTION
  This function, called in mgmsdi_process_command.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE:  
  MMGSDI_SUCCESS: upon succssesful processing of command.
  MMGSDI_ERROR: upon error

SIDE EFFECTS
  NONE
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_process_jcdma_card_get_info_cmd (
  const mmgsdi_jcdma_get_card_info_req_type *req_ptr
);


/*===========================================================================
FUNCTION MMGSDI_JCDMA_RSP_GET_CARD_INFO

DESCRIPTION
  This function sends response 

DEPENDENCIES
  None
  
LIMITATIONS
  NONE
  
RETURN VALUE:  
   MMGSDI_SUCCESS: upon successful queueing of the conf.
   MMGSDI_ERROR: upon error
  
SIDE EFFECTS
  NONE
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_jcdma_rsp_get_card_info (
  const mmgsdi_uim_report_rsp_type*   uim_rsp_ptr
);


/*===========================================================================
FUNCTION:      MMGSDI_JCDMA_CARD_GET_COLOR_AND_LOCKS

DESCRIPTION:
  This function reads files from card and decides card color. It also reads 
  UIM lock indicator to decide if lock checks are needed. 
  Files read from card: MMGSDI_CDMA_IMSI_M 	 
			MMGSDI_CDMA_IMSI_STATUS
                        MMGSDI_CDMA_UIM_VERSION 
		        MMGSDI_CDMA_RUIM_ID
			MMGSDI_CDMA_GID1
			MMGSDI_CDMA_GID2

DEPENDENCIES:
	 
LIMITATIONS:

RETURN VALUE:  
  MMGSDI_SUCCESS:  Upon successfuly parsing read file data
  MMGSDI_ERROR:    Upon error 
     
SIDE EFFECTS:
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_jcdma_card_get_color_and_locks(
  mmgsdi_jcdma_card_attr_type *card_attr_ptr);


/*===========================================================================
FUNCTION:      MMGSDI_JCDMA_CARD_GET_ATTR

DESCRIPTION:
  This function checks card color (white/gray/black), UIM lock staus, and card 
  insertion history. 
                        
DEPENDENCIES:
   PIN1 verification is done before calling this function.

LIMITATIONS:

RETURN VALUE:  
  MMGSDI_SUCCESS: If succssesful in getting card attributes
  MMGSDI_ERROR : If a error in getting card attributes
     
SIDE EFFECTS:
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_jcdma_card_get_attr(void);
                        
   
#endif /* MMGSDI_RUIM_JCDMA_H */
#endif /* FEATURE_UIM_JCDMA_RUIM_SUPPORT  */
