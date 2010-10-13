/*===========================================================================

              NET   F I L E

DESCRIPTION
  This header file contains definitions that are required for defining NETWORK
  SYSTEM and PROFILES in a NET FILE.

  User's Private Automatic System list is stored at /nvm/net_list.
  User's Manual System is stored at                 /nvm/net_user_manaual_sys


EXTERNALIZED FUNCTIONS

  net_get_sw_ver

  net_file_read
  net_file_write
  net_file_read2
  net_file_write2

  net_validate

  net_parser_make
  net_parser_init
  net_set_file_id


  net_get_max_allowed_file_size
  net_get_file_len

  net_extract_buf_ptr

  net_get_max_allowed_sys
  net_get_max_allowed_profile
  net_get_num_sys
  net_get_num_profile

  net_get_sys_rec
  net_get_profile
  net_get_wlan_sys_rec

  net_insert_sys_rec
  net_insert_profile
  net_insert_sys_rec_profile
  net_insert_wlan_sys_rec
  net_insert_wlan_sys_rec_profile

  net_del_sys_rec
  net_del_profile

  net_modify_sys_rec
  net_modify_profile
  net_modify_wlan_sys_rec

  net_pack_profile
  net_unpack_profile

Copyright (c) 2004 - 2009 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/net.c#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/04/09   mh      Remoted NET APIs
03/10/09   sv      Lint clean up
06/13/08   sv      Memory Reduction for ULC
06/12/08   sv      Resolve Klocwork defects
08/07/07   ic      Lint clean up
06/11/07   tbh     Moved the new fields for WLAN handoff & WLAN security after 
                   SSID field in the system record structure.
06/08/07   tbh     Added new server id field for PEAP, TLS authentication types. 
                   Also modified existing system record structure with new 
                   fields for WLAN handoff & WLAN security.
02/23/07   ic      Initialized several buffers in net_parse_file() and 
                   net_parser_init() 
11/07/06   sk      Memory reduction effort.
10/09/06   sk      Featurized net_list file location in EFS depending on
                   processor count and current processor.
09/11/06   sk      Added support for SIM/AKA authentication types.
04/10/06   pk      Lint fixes
03/24/06   dk      Fixed off-by one error in shift_down function.
03/02/06   pk      Lint fixes
02/06/06   dk      Added support for PAP,CHAP,MsCHAPv2,PEAP,TTLS.
01/12/06   ic      For specific calls / walks Lint warnings, replaced global
                   -fsp comment with -e{error#} comments where needed
01/06/06   ic      Lint cleanup
12/16/05   ic      Fixed RVCT 2.1 compiler warnings "pointless comparison of
                   unsigned inteted with a negative constant"
12/05/05   ic      Fixed ADS1.2 compiler warnings "unsigned comparison with 0
                   is always true: '>=' "
11/18/05   jqi     Repaced cmdbg.h with cmidbg.h file to reduce header file
                   dependency.
11/03/05   ic      Added CM_ASSERT() for ptr function parameters and lint
                   comments for 613 Lint error
10/08/05   ic      Added Header: field
05/25/05   dk      Modified the location of NET files.
04/20/05   ic      Moved unit test to nettest.c file
04/05/05   dk      Fixed issue in net_pack_profile for copy.
03/14/05   dk      Fixed parser_make.
02/02/05   ka      Uncommented WLAN_JAPAN case stmts.
11/02/04   dk      Initial version.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/*lint -esym(766,net.h) */
#include "net.h"

#ifdef FEATURE_WLAN
#error code not present
#endif /* FEATURE_WLAN */

