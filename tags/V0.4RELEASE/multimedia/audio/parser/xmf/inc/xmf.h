#ifndef XMF_H
#define XMF_H
/*===========================================================================

             X M F   S E R V I C E S    H E A D E R    F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface 
  with XMF Services.

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
   
  $Header: //source/qcom/qct/multimedia/audio/parser/xmf/main/latest/inc/xmf.h#1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/11/07   pl/st     Added xmf_terminate() function prototype.
02/06/05    st     Created file.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#ifdef FEATURE_XMF
#include "comdef.h"     /* Definitions for byte, word, etc.                */

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* <EJECT> */
/*---------------------------------------------------------------------------
** XMF Public Function Prototypes
**---------------------------------------------------------------------------
*/
/*===========================================================================

FUNCTION xmf_terminate

DESCRIPTION
  This function terminates XMF parsing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void xmf_terminate (
  void *xmf_state
);

/* <EJECT> */
/*===========================================================================

FUNCTION xmf_load_and_play_cont

DESCRIPTION
  This function continues loading and playing the XMF file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void xmf_load_and_play_cont (
  void *xmf_state
);

/* <EJECT> */
/*===========================================================================

FUNCTION xmf_process_continue

DESCRIPTION
  This function continues processing the indicated XMF file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void xmf_process_continue (
  void *xmf_state
);

/* <EJECT> */
/*===========================================================================

FUNCTION xmf_data_req_cb

DESCRIPTION
  This function handles data request callbacks.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void xmf_data_req_cb (
  cmx_xmf_status_type status,
  void                *xmf_state,
  uint32              num_bytes
);

/* <EJECT> */
/*===========================================================================

FUNCTION xmf_verify

DESCRIPTION
  This function checks if the indicated file is an XMF file and, if so,
  parses and plays the file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void xmf_verify (
  void *handle_ptr
);
#endif /* FEATURE_XMF */
#endif /* XMF_H */
