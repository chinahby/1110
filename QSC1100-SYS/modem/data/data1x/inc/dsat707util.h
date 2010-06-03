#ifndef DSAT707UTIL_H
#define DSAT707UTIL_H
/*===========================================================================

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

               I S - 7 0 7   C O M M A N D   T A B L E S                
                I N T E R N A L   H E A D E R   F I L E


DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants, and function prototypes used by the
  data services AT command processor.

  EXTERNALIZED FUNCTIONS
  dsat707_find_cmd_name
    This function matches the command name that gets passed in with the
    command table entry and returns the corresponding index.

  dsat707_unrec_func_ptr
    This function processess unrecognized commands, and commands with
    parameter errors which will cause a connection to the base station
    if +CXT=1.
  
Copyright (c) 1995,1996,1997 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1998 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1999, 2000, 2001 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/25/03   rsl     Changes relating to Unrec. cmds not working.
07/13/02   atp     Changed signature of dsat707_send_config() to make param
                   passed in a ptr to const byte.
04/03/02   rsl     Initial version of file.

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA
#ifdef FEATURE_DATA_IS707
#include "dsati.h"

/*-------------------------------------------------------------------------
    Definition of strings used for verbose result codes:

    Assignment to the pointer arrays "xxx_codes" must match
    the definition of "ds_atcop_result_enum_type" in dsatcop.h
-------------------------------------------------------------------------*/

extern boolean unrec_cmd_rcvd;
extern const byte res_code_string_ok [];
extern const byte res_code_string_con [];
extern const byte res_code_string_ring [];
extern const byte res_code_string_no_car [];
extern const byte res_code_string_error [];
extern const byte res_code_string_nu [];
extern const byte res_code_string_no_dial [];
extern const byte res_code_string_busy [];
extern const byte res_code_string_no_ans [];

extern const byte res_code_string_rng_amps  [];
extern const byte res_code_string_rng_async [];
extern const byte res_code_string_rng_fax [];
extern const byte res_code_string_rng_pkt   [];

/*20*/
extern const byte res_code_string_no_amps[];
/*21*/
extern const byte res_code_string_no_srv [];
/*22 */
extern const byte res_code_string_no_async [];
/*23*/
extern const byte res_code_string_no_fax [];
/*24*/
extern const byte res_code_string_no_pkt [];
/*25*/
extern const byte res_code_string_no_intr[];
/*26*/
extern const byte res_code_string_paged [];
extern const byte res_code_string_reoder [];
extern const byte res_code_string_page_fail [];
extern const byte res_code_string_link_fail [];

extern const byte res_code_string_released [];

/*  Stand alone */
extern const byte res_code_string_cerror [];

extern const byte *const res_codes [];

extern const byte *const cell_res_codes [];

extern const byte *const cell_error_res_codes [];

/*-------------------------------------------------------------------------
  Dial string for Packet/QNC calls. Defaults to "#777". Note that if the
  default string is changed, the length variable should also be updated.
-------------------------------------------------------------------------*/
extern byte dsat707_pkt_orig_str[];

/*-------------------------------------------------------------------------
  Length of Packet/QNC dial string (including the terminating NULL)
-------------------------------------------------------------------------*/
extern byte dsat707_pkt_orig_str_len;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION DSAT707_FIND_CMD_NAME

DESCRIPTION
  This function matches the command name that gets passed in with the
  command table entry and returns the corresponding index.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/

int dsat707_find_cmd_name (byte *s1);

/*===========================================================================

FUNCTION DSAT707_UNREC_FUNC_PTR

DESCRIPTION
  This function processess unrecognized commands, and commands with parameter
  errors which will cause a connection to the base station if +CXT=1.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

dsat_result_enum_type dsat707_unrec_func_ptr( byte * );

/*===========================================================================

FUNCTION DSAT707_SEND_CONFIG

DESCRIPTION
  This function transmits the mobile configuration to the base station.
  Configuration parameters are those which have the DOWN_LOAD attribute
  in the parse table.  The only DOWN_LOAD paramters transmitted are those
  with the ALWAYS attribute in the parse table, and those which
  have been changed from their default values.  Parameters with
  the FCLASS2P0 attribute are not transmitted unless the current value
  of the FCLASS parameter is 2.0.

  Follow the configuration with the +CFG string (always, even if empty),
  and then the current command.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void dsat707_send_config
(
  const byte *
);

/*===========================================================================

FUNCTION      DSAT707_NV_SYNC

DESCRIPTION   This function reads all the 707-specific NV items out of
              NV.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/

void dsat707_nv_sync(void);

/*=========================================================================*/

#endif /* FEATURE_DATA_IS707 */
#endif /* FEATURE_DATA */
#endif /* DSAT707UTIL_H */
