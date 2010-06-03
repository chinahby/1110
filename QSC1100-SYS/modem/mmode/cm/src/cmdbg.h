#ifndef CMDBG_H
#define CMDBG_H
/*===========================================================================

     C A L L   M A N A G E R   D E B U G   H E A D E R   F I L E

DESCRIPTION
  This header file contains debug macros and definitions necessary to
  interface with CMDBG.C


Copyright (c) 1991 - 2006 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmdbg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/10/06   pk      Lint fixes
03/15/06   sk      Fixed debug messages to print client type instead of
                   client id.
03/09/06   pk      Added support for generic report
01/06/06   ic      Lint cleanup  
11/18/05   jqi     Moved CM_DEBUG to cmidbg.h file to reduce header file 
                   dependency.
05/26/05   ic      Removed references to CM_STRING_MAPS_ACTIVE
11/15/04   ic      Lint cleanup with CM_DEBUG on 
04/13/04   ic      Featurization changes
04/01/04   ic      Changed cmdbg_print_fav_mode_pref_for_origination() to take
                   5 parameters instead of a pointer to a structure that
                   contains those parameters.
                   cmdbg.c is shared across targets and the structure
                   cm_call_cmd_info_s_type became different for each target.
03/30/04   ws      Made print_fav_mode_pref mode independent.
03/24/04   ic      Changed definition of CM_VERIFY to not return a value
                   (compiler warning fix).
                   Made print_message() global.
02/06/04   ws      Initial jaguar/mainline merge.
11/03/03   ic      Added cmdbg_print_fav_mode_pref_for_origination()
10/23/03   ic      Added cm_ss_cmd_prt(), cm_ss_event_prt(), cm_ss_cmd_err_prt(),
                   cmdbg_ss_info_print()
08/27/03   ic      Changes to support HDR hybrid operation on Cougar.
07/11/03   ic      Baseline for HDR hybrid development for Jaguar.
                   Restored CMDBG_PRT_FUNC so it does not evaluate to no op
                   when CM_DEBUG is defined.
01/21/03   ws      Updated copyright information for 2003
05/05/02   vt      Fixed compile problem with new mm cm.h
03/25/02   ic      Added declaration of cm_call_orig_fail_e_type_string_map
03/06/02   ic      Replaced
                   extern const char* cm_call_cmd_e_string_map[];
                   with
                   extern const char* cm_call_cmd_e_type_string_map[];
                   Added
                   extern const char* cm_inband_cmd_e_type_string_map [];
                   extern const char* cm_inband_cmd_err_e_type_string_map [ ];
02/15/02   ic      Added declaration of cm_call_cmd_e_string_map
02/11/02   ic      Made print_message() global
02/07/02   ic      Under CM_STRING_MAPS_ACTIVE & FEATURE_IS2000_REL_A, added
                   declaration of cm_flash_type_e_type_string_map
01/18/02   ic      Merged MSM_CM.05.03.33 on top of tip CCS CM
                   Diffed MSM_CM.05.03.32 with MSM_CM.05.03.33 and only the changes
                   between those two were merged with CCS CM
12/13/01   ic      Merged MSM_CM.05.03.32 onto SVD CM tip.
                   CCS CM code started from CM VU MSM_CM.05.03.25 + WCDMA/GSM
                   stuff from SIMBA team merged on top of it.
                   We diff'ed MSM_CM.05.03.25 with MSM_CM.05.03.32 (CM VU in N*2165)
                   that we needed to merge with and changes between those two VUs
                   were merged onto CCS CM tip.
10/23/01   VT/IC   Baseline for CCS
05/31/01   RC      Compile CM_CASE_ERR() to ERR() when CM_DEBUG is not
                   defined.
11/17/00   RC      Added support for System Determination 2.0
08/31/99   SH      Corrected some comments
06/21/99   SH      Enabled CM_DEBUG
01/07/99   RC      Changes per ARM compiler.
12/30/98   RC      Changes per 4th week of I&T.
12/08/98   RC      Changes per second week of I&T.
11/24/98   RC      Initial release.



===========================================================================*/

#include "comdef.h"    /* Definition for basic types and macros */
#include "target.h"    /* Target configuration file */
#include "err.h"       /* Interface to error log services */
#include "msg.h"       /* Interface to F3 messages services */

#include "cm.h"        /* External interface to cm.c */
#include "cmi.h"
#include "cmcall.h"
#include "cmph.h"
#include "cminband.h"
#include "cmss.h"
#include "cmidbg.h"



/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/


/*lint -save -e656 -e641 
** Turn off enum to int / operationuses compatible enums
*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

extern void cm_call_state_prt( dword enum_val );
#define CM_CALL_STATE_PRT( enum_val )  \
    CMDBG_PRT_FUNC( cm_call_state_prt, enum_val )


extern void cm_call_type_prt( dword  enum_val );
#define CM_CALL_TYPE_PRT( enum_val )  \
    CMDBG_PRT_FUNC( cm_call_type_prt, enum_val )


extern void cm_call_end_prt( cm_call_end_e_type  enum_val );
#define CM_CALL_END_PRT( enum_val )  \
    CMDBG_PRT_FUNC( cm_call_end_prt, enum_val )


extern void cm_digit_mode_prt( dword enum_val );
#define CM_DIGIT_MODE_PRT( enum_val )  \
    CMDBG_PRT_FUNC( cm_digit_mode_prt, enum_val )


extern void cm_call_cmd_prt( dword enum_val );
#define CM_CALL_CMD_PRT( enum_val )  \
    CMDBG_PRT_FUNC( cm_call_cmd_prt, enum_val )


extern void cm_call_event_prt( dword enum_val );
#define CM_CALL_EVENT_PRT( enum_val )  \
    CMDBG_PRT_FUNC( cm_call_event_prt, enum_val )


extern void cm_call_cmd_err_prt( dword enum_val );
#define CM_CALL_CMD_ERR_PRT( enum_val )  \
    CMDBG_PRT_FUNC( cm_call_cmd_err_prt, enum_val )


extern void cm_srv_state_prt( dword enum_val );
#define CM_SRV_STATE_PRT( enum_val )  \
    CMDBG_PRT_FUNC( cm_srv_state_prt, enum_val )


extern void cm_oprt_mode_prt( dword enum_val );
#define CM_OPRT_MODE_PRT( enum_val )  \
    CMDBG_PRT_FUNC( cm_oprt_mode_prt, enum_val )


extern void cm_mode_pref_prt( dword enum_val );
#define CM_MODE_PREF_PRT( enum_val )  \
    CMDBG_PRT_FUNC( cm_mode_pref_prt, enum_val )


extern void cm_mode_term_prt( dword enum_val );
#define CM_MODE_TERM_PRT( enum_val )  \
    CMDBG_PRT_FUNC( cm_mode_term_prt, enum_val )

extern void cm_band_pref_prt( dword enum_val );
#define CM_BAND_PREF_PRT( enum_val )  \
    CMDBG_PRT_FUNC( cm_band_pref_prt, enum_val )

extern void cm_roam_pref_prt( dword enum_val );
#define CM_ROAM_PREF_PRT( enum_val )  \
    CMDBG_PRT_FUNC( cm_roam_pref_prt, enum_val )

extern void cm_answer_voice_prt( dword enum_val );
#define CM_ANSWER_VOICE_PRT( enum_val )  \
    CMDBG_PRT_FUNC( cm_answer_voice_prt, enum_val )


extern void cm_nam_prt( dword enum_val );
#define CM_NAM_PRT( enum_val )  \
    CMDBG_PRT_FUNC( cm_nam_prt, enum_val )


extern void cm_cdma_lock_mode_prt( dword enum_val );
#define CM_CDMA_LOCK_MODE_PRT( enum_val )  \
    CMDBG_PRT_FUNC( cm_cdma_lock_mode_prt, enum_val )


extern void cm_ph_cmd_prt( dword enum_val );
#define CM_PH_CMD_PRT( enum_val )  \
    CMDBG_PRT_FUNC( cm_ph_cmd_prt, enum_val )

extern void cm_ss_cmd_prt( dword enum_val );
#define CM_SS_CMD_PRT( enum_val )  \
    CMDBG_PRT_FUNC( cm_ss_cmd_prt, enum_val );

extern void cm_ph_event_prt( dword enum_val );
#define CM_PH_EVENT_PRT( enum_val )  \
    CMDBG_PRT_FUNC( cm_ph_event_prt, enum_val )

extern void cm_ss_event_prt( dword enum_val );
#define CM_SS_EVENT_PRT( enum_val )  \
    CMDBG_PRT_FUNC( cm_ss_event_prt, enum_val )

extern void cm_ph_cmd_err_prt( dword enum_val );
#define CM_PH_CMD_ERR_PRT( enum_val )  \
    CMDBG_PRT_FUNC( cm_ph_cmd_err_prt, enum_val )

extern void cm_ss_cmd_err_prt( dword enum_val );
#define CM_SS_CMD_ERR_PRT( enum_val )  \
    CMDBG_PRT_FUNC( cm_ss_cmd_err_prt, enum_val )

extern void cm_in_use_prt( dword enum_val );
#define CM_IN_USE_PRT( enum_val )  \
    CMDBG_PRT_FUNC( cm_in_use_prt, enum_val )


extern void cm_client_type_prt( cm_client_type_e_type  client_id );
#define CM_CLIENT_TYPE_PRT( client )  \
    CMDBG_PRT_FUNC( cm_client_type_prt, client )

extern void  mc_cmd_prt( dword enum_val );
#define CM_MC_CMD_PRT( enum_val )  \
    CMDBG_PRT_FUNC( mc_cmd_prt, enum_val )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to create a bit array of arbitrary size. The first argument
** (bit_arr) is the name of the array. The second argument (bit_size)
** is the bit size of the array.
*/
#define BIT_ARR_T( bit_arr, bit_size )             \
    byte _##bit_arr[1 + (bit_size)/sizeof(byte)];  \
    bit_arr_s_type bit_arr = { _##bit_arr, bit_size }


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/**--------------------------------------------------------------------------
** Externs
** --------------------------------------------------------------------------
*/



/**--------------------------------------------------------------------------
** Functions
** --------------------------------------------------------------------------
*/

extern void cmdbg_print_message( const char *string, ... );
extern void cmdbg_print_call(const cmcall_s_type *call_ptr);
extern void cmdbg_print_cm_cmd(const cm_cmd_s_type  *cmd_ptr );

extern void cmdbg_call_info_print(const cm_mm_call_info_s_type *info_ptr);
extern void cmdbg_ph_info_print(const cm_ph_info_s_type  *ph_info_ptr);
extern void cmdbg_ss_info_print( const cm_mm_ss_info_s_type   *ss_info_ptr);
extern void cmdbg_print_cm_event( cm_cmd_type_e_type  cmd_type, word event,
  const void *info_ptr);
extern void cmdbg_print_cm_cmderr( cm_cmd_type_e_type  cmd_type,
  word cmd_err, const void *cmd_ptr);
extern void cmdbg_print_sd_rpt(const cm_sd_rpt_u_type *sd_rpt_ptr );
extern void cmdbg_print_generic_rpt(const cm_generic_rpt_s_type *generic_rpt_ptr );


#define CMDBG_PRINT_CM_CMD_FUNC( cmd_ptr ) \
  cmdbg_print_cm_cmd( cmd_ptr )
#define CMDBG_PRINT_CM_EVENT_FUNC( cmd_type, event, info_ptr ) \
  cmdbg_print_cm_event( cmd_type, event, info_ptr)
#define CMDBG_PRINT_CM_CMDERR_FUNC( cmd_type, cmd_err, cmd_ptr ) \
  cmdbg_print_cm_cmderr( cmd_type, cmd_err, cmd_ptr )
#define CMDBG_PRINT_SD_RPT_FUNC( sd_rpt_ptr ) \
  cmdbg_print_sd_rpt( sd_rpt_ptr )
#define CMDBG_PRINT_GENERIC_RPT_FUNC( generic_rpt_ptr ) \
  cmdbg_print_generic_rpt( generic_rpt_ptr )

#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))

extern void cmdbg_print_mc_cmd(const mc_msg_type *mc_cmd_ptr );
extern void cmdbg_print_mc_rpt(const cm_mc_rpt_type *mc_rpt_ptr );

#define CMDBG_PRINT_MC_CMD_FUNC( mc_cmd_ptr ) \
  cmdbg_print_mc_cmd( mc_cmd_ptr )
#define CMDBG_PRINT_MC_RPT_FUNC( mc_rpt_ptr ) \
  cmdbg_print_mc_rpt( mc_rpt_ptr )

#endif /* FEATURE_CDMA_800 || FEATURE_CDMA_1900 */

#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#endif /* FEATURE_WCDMA || FEATURE_GSM */


extern void cmdbg_print_fav_mode_pref_for_origination(

  cm_call_type_e_type            call_type,
    /* requested call type
    */

  cm_srv_type_e_type             srv_type,
    /* requested service type
    */

  cm_mode_pref_e_type            mode_pref,
    /* mode preference at the time call origination request was made
    */

  cm_mode_pref_e_type            favored_mode_pref,
    /* computed favored mode preference for the call
    */

  cm_hybr_pref_e_type            favored_hybr_pref
    /* computed favored hybrid preference for the call
    */

);

/*lint -restore */
#endif /* CMDBG_H */





