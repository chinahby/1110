/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Q D S P   S E R V I C E S   M O D U L E
                         T A S K   F U N C T I O N S
                     
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright(c) 2007 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/core/qdspservices/6k/ver1/qdsputils/main/latest/src/qdspdebug.c#1 $ $DateTime: 2008/11/24 02:58:15 $ $Author: lkasam $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/15/07   act     Modified code to allow for QDSP_NOP_ADDRESS to be
                   undefined.
07/24/07   act     Initial revision.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/

#ifdef CUST_H
   #include "customer.h"
#endif

#include "comdef.h"             /* Definitions for byte, word, etc.       */

#ifdef QDSP_WIN32_EMU
#error code not present
#else
#include "assert.h"             /* Definitions for ASSERT                 */
#include "dog.h"                /* Watchdog signal and time definitions   */
#include "err.h"                /* Error logging macro                    */
#include "msg.h"                /* Message logging/reporting services     */
#endif

#include "qdspext.h"

/* <EJECT> */

/*===========================================================================

                    DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed to interface with the QDSP Services module.

===========================================================================*/
//#define QDSP_HALT_ON_INVALID_ACCESS

#ifdef QDSP_HALT_ON_INVALID_ACCESS
#error code not present
#else
boolean qdsp_halt_on_invalid_access = FALSE;
#endif /* QDSP_HALT_ON_INVALID_ACCESS */

const boolean qdsp_debug_mode = FALSE;


#undef XX_MSG_CONST_FMT_VAR
#define XX_MSG_CONST_FMT_VAR(xx_ss_id, xx_ss_mask, xx_fmt) \
    msg_const_type xx_msg_const = { \
      {0, (xx_ss_id), (xx_ss_mask)}, 0, 0}; \
    xx_msg_const.desc.line = line; \
  xx_msg_const.fmt = (xx_fmt);   \
  xx_msg_const.fname = file


/* <EJECT> */
/*===========================================================================

                             FUNCTION DEFINITIONS

===========================================================================*/


/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_debug_read

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void qdsp_debug_read(
  void * address,
  char const * const file,
  char const * const msg,
  uint32 val,
  uint32 line
) {
#ifdef QDSP_NOP_ADDRESS
  if( address == (void *)QDSP_NOP_ADDRESS )
  {
    if( qdsp_halt_on_invalid_access )
    {
      qdsp_fw_ctrl_func ( QDSPEXT_FW_DOWNLOAD );
      ERR_FATAL("Invalid DSP interface variable access",0,0,0);
    }
    else
    {
      MSG_SPRINTF_FMT_VAR_3(MSG_SSID_QDSP, MSG_LEGACY_ERROR, msg, 0xDEADC0DE, 0, 0);
      return;
    }
  }
#endif /* QDSP_NOP_ADDRESS */
  MSG_SPRINTF_FMT_VAR_3(MSG_SSID_QDSP, MSG_LEGACY_LOW, msg, val, 0, 0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_debug_write

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void qdsp_debug_write(
  void * address,
  uint32 val,
  char const * const file,
  char const * const msg,
  uint32 line
) {
#ifdef QDSP_NOP_ADDRESS
  if( address == (void *)QDSP_NOP_ADDRESS )
  {
    if( qdsp_halt_on_invalid_access )
    {
      qdsp_fw_ctrl_func ( QDSPEXT_FW_DOWNLOAD );
      ERR_FATAL("Invalid DSP interface variable access",0,0,0);
    }
    else
    {
      MSG_SPRINTF_FMT_VAR_3(MSG_SSID_QDSP, MSG_LEGACY_ERROR, msg, 0xDEADC0DE, 0, 0);
      return;
    }
  }
#endif /* QDSP_NOP_ADDRESS */
  MSG_SPRINTF_FMT_VAR_3(MSG_SSID_QDSP, MSG_LEGACY_MED, msg, val, 0, 0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_debug_read_offset

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void qdsp_debug_read_offset(
  void * address,
  uint32 offset,
  char const * const file,
  char const * const msg,
  uint32 val,
  uint32 line
) {

#ifdef QDSP_NOP_ADDRESS
  if( address == (void *)QDSP_NOP_ADDRESS )
  {
    if( qdsp_halt_on_invalid_access )
    {
      qdsp_fw_ctrl_func ( QDSPEXT_FW_DOWNLOAD );
      ERR_FATAL("Invalid DSP interface variable access",0,0,0);
    }
    else
    {
      MSG_SPRINTF_FMT_VAR_3(MSG_SSID_QDSP, MSG_LEGACY_ERROR, msg, offset, 0xDEADC0DE, 0);
      return;
    }
  }
#endif /* QDSP_NOP_ADDRESS */
  MSG_SPRINTF_FMT_VAR_3(MSG_SSID_QDSP, MSG_LEGACY_LOW, msg, offset, val, 0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_debug_write_offset

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void qdsp_debug_write_offset(
  void * address,
  uint32 offset,
  uint32 val,
  char const * const file,
  char const * const msg,
  uint32 line
) {
#ifdef QDSP_NOP_ADDRESS
  if( address == (void *)QDSP_NOP_ADDRESS )
  {
    if( qdsp_halt_on_invalid_access )
    {
      qdsp_fw_ctrl_func ( QDSPEXT_FW_DOWNLOAD );
      ERR_FATAL("Invalid DSP interface variable access",0,0,0);
    }
    else
    {
      MSG_SPRINTF_FMT_VAR_3(MSG_SSID_QDSP, MSG_LEGACY_ERROR, msg, 0xDEADC0DE, offset, 0);
      return;
    }
  }
#endif /* QDSP_NOP_ADDRESS */
  MSG_SPRINTF_FMT_VAR_3(MSG_SSID_QDSP, MSG_LEGACY_MED, msg, val, offset, 0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_debug_address

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void qdsp_debug_address(
  void * address,
  uint32 offset,
  char const * const file,
  char const * const msg,
  uint32 val,
  uint32 line
) {

#ifdef QDSP_NOP_ADDRESS
  if( address == (void *)QDSP_NOP_ADDRESS )
  {
    if( qdsp_halt_on_invalid_access )
    {
      qdsp_fw_ctrl_func ( QDSPEXT_FW_DOWNLOAD );
      ERR_FATAL("Invalid DSP interface variable access",0,0,0);
    }
    else
    {
      MSG_SPRINTF_FMT_VAR_3(MSG_SSID_QDSP, MSG_LEGACY_ERROR, msg, offset, val, 0);
      return;
    }
  }
#endif /* QDSP_NOP_ADDRESS */
  MSG_SPRINTF_FMT_VAR_3(MSG_SSID_QDSP, MSG_LEGACY_LOW, msg, offset, val, 0);
}
