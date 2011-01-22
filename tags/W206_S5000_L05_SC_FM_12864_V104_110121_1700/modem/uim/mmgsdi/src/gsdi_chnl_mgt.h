#ifndef GSDI_CHNL_MGT_H
#define GSDI_CHNL_MGT_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            G E N E R I C   S I M   D R I V E R   I N T E R F A C E

            C H A N N E L  M A N A G E M E N T  P R O C E D U R E S

                              H E A D E R  F I L E


GENERAL DESCRIPTION

  This source file contains the operating procedures that pertain to the
  channel management procedures as defined in the following specifications:
  7816-4.

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Requrires that following events are generated prior to processing of the
  commands.
  GSDI_SIM_INIT_COMPLETED           (SLOT1)

  If the Target is a dual slotted target, then the following events, in addition
  to the event mentioned above will need to be generated before processing of
  these functions can occur on the respective slot.
  GSDI_SIM_INIT_COMPLETED_NO_PROV   (SLOT1)
  GSDI_SIM_INIT_COMPLETED_2         (SLOT2)
  GSDI_SIM_INIT_COMPLETED_NO_PROV_2 (SLOT2)

                        COPYRIGHT INFORMATION

Copyright (c) 2001 - 2005 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/gsdi_chnl_mgt.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/11/05   sst     Lint fixes
04/28/04   jar     Created File

===========================================================================*/
/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "gsdi_exp.h"

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/



#ifdef FEATURE_MMGSDI_CHANNEL_MANAGEMENT
/* ==========================================================================
TYPE ENUM:    GSDI_CHNL_MGT_ID_ENUM_TYPE

DESCRIPTION:  Used internally within the GSDI Channel Management procedures
              to distinguish the different request types in general
              when using general functions.

 VALUES:      GSDI_CHNL_MGT_ID_NONE      = 0x00
              GSDI_CHNL_MGT_ID_OPEN      = 0x01
              GSDI_CHNL_MGT_ID_CLOSE     = 0x02
              GSDI_CHNL_MGT_ID_SEND_DATA = 0x03
              GSDI_CHNL_MGT_ID_GET_ATR   = 0x04
============================================================================*/
typedef enum {
  GSDI_CHNL_MGT_ID_NONE      = 0x00,
  GSDI_CHNL_MGT_ID_OPEN      = 0x01,
  GSDI_CHNL_MGT_ID_CLOSE     = 0x02,
  GSDI_CHNL_MGT_ID_SEND_DATA = 0x03,
  GSDI_CHNL_MGT_ID_GET_ATR   = 0x04
}gsdi_chnl_mgt_id_enum_type;


/*===========================================================================
FUNCTION GSDI_PROC_CHNL_MGT_OPEN_CHANNEL

DESCRIPTION
  This is the function called from the main procedure handling function to
  open a communication channel with the Card Inserted.  This funciton will
  validate the important parameters.  Once the parameters pass the checking,
  it will call the GSDI - UIM Function to send it a Manage Channel
  Request - Open.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T:

SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T gsdi_proc_chnl_mgt_open_channel(
    gsdi_open_channel_req_T *  req_p,
    int32 *                    returned_data_len_p,
    byte *                     data_p
);

/*===========================================================================
FUNCTION GSDI_PROC_CHNL_MGT_CLOSE_CHANNEL

DESCRIPTION
  This is the function called from the main procedure handling function to
  close a channel previously opened.  This funciton will validate the
  important parameters.  Once the parameters pass the checking, it will
  call the GSDI - UIM Function to send the UIM a Manage Channel - Close
  for the channel id provided.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T:

SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T gsdi_proc_chnl_mgt_close_channel(
    gsdi_close_channel_req_T *  req_p,
    int32 *                     returned_data_len_p,
    byte *                      data_p
);

/*===========================================================================
FUNCTION GSDI_PROC_CHNL_MGT_SEND_CHANNEL_DATA

DESCRIPTION
  This is the function called from the main procedure handling function.
  This funciton will validate the important parameters.  Once the
  parameters pass the checking, it will call the GSDI - UIM Function
  to send the UIM a Manage Channel - Close for the channel id provided.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T:

SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T gsdi_proc_chnl_mgt_send_channel_data(
    gsdi_send_channel_data_req_T *  req_p,
    int32 *                         returned_data_len_p,
    byte *                          data_p
);


/*===========================================================================
FUNCTION GSDI_PROC_CHNL_MGT_GET_ATR

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T gsdi_proc_chnl_mgt_get_atr(
    gsdi_get_atr_req_T *  req_p,
    int32 *               returned_data_len_p,
    byte *                data_p
);


/* ==========================================================================
                    GSDI  CHANNEL  MANAGEMENT
                               AND
                    UIM  INTERFACE  FUNCTIONS
===========================================================================*/

/*===========================================================================
FUNCTION GSDI_UIM_OPEN_CHANNEL

DESCRIPTION
  This function is called from the Channel Management Open Channel
  function to build a UIM Manage Channel Request to open a new
  communication channel with the Card presently inserted.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T

SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T gsdi_uim_server_open_channel(
  gsdi_open_channel_req_T * req_p,
  int32 *                   returned_data_len_p,
  byte *                    data_p
);

/*===========================================================================
FUNCTION GSDI_UIM_SERVER_CLOSE_CHANNEL

DESCRIPTION
  This function is called from the Channel Management Close Channel
  function to build a UIM Manage Channel Request to close the
  communication channel with the Card.  The Channel to be closed will be the
  channel provided in the request.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T

SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T gsdi_uim_server_close_channel(
  gsdi_close_channel_req_T * req_p,
  int32 *                    returned_data_len_p,
  byte *                     data_p
);

/*===========================================================================
FUNCTION GSDI_UIM_SERVER_SEND_CHANNEL_DATA

DESCRIPTION
  This function is called from the Channel Management Send Channel
  Data function to build a UIM Request to send an APDU on the
  communication channel with the Card.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T

SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T gsdi_uim_server_send_channel_data(
  gsdi_send_channel_data_req_T * req_p,
  int32 *                        returned_data_len_p,
  byte *                         data_p
);
#endif /* FEATURE_MMGSDI_CHANNEL_MANAGEMENT */
#endif /* GSDI_CHNL_MGT_H */

