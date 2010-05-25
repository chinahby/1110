#ifndef SECDIAG_H
#define SECDIAG_H

/*===========================================================================
*
*                               S E C  D I A G
*             
*                            H E A D E R  F I L E
*
* FILE:  secdiag.h
*
* DESCRIPTION :
*  Base class from which all the Ix classes are derived.
*  Contains just pure virtual functions for AddRef and Release.
*
* Copyright (c) 2006 by QUALCOMM, Incorporated.  All Rights Reserved.  
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
                            
  $Header: //source/qcom/qct/core/securemsm/crypto/main/latest/inc/secdiag.h#1 $
  $DateTime: 2009/02/27 17:37:08 $
  $Author: clavians $


when       who      what, where, why
--------   ---      ------------------------------------
08/09/07    sm      Added SECDIAG_BCASTAUTH definition. Mapping to an existing SSID, till a new one is assigned for this
06/22/07    sb      Added SECDIAG_BCASTCNTAGENT definition
07/05/06    sl      Created
      
===========================================================================*/

#include "msg.h"

/*
 * Security Messages SSIDs. These are defined by DIAG in msgcfg.h, and centralized
 * here to be used by all security modules. 
 */

#define SECDIAG_CRYPTO                 (MSG_SSID_SEC_CRYPTO)
#define SECDIAG_SSL                    (MSG_SSID_SEC_SSL)
#define SECDIAG_IPSEC                  (MSG_SSID_SEC_IPSEC)
#define SECDIAG_SFS                    (MSG_SSID_SEC_SFS)
#define SECDIAG_TEST                   (MSG_SSID_SEC_TEST)
#define SECDIAG_CNTAGENT               (MSG_SSID_SEC_CNTAGENT)
#define SECDIAG_RIGHTSMGR              (MSG_SSID_SEC_RIGHTSMGR)
#define SECDIAG_ROAP                   (MSG_SSID_SEC_ROAP)
#define SECDIAG_MEDIAMGR               (MSG_SSID_SEC_MEDIAMGR)
#define SECDIAG_IDSTORE                (MSG_SSID_SEC_IDSTORE)
#define SECDIAG_IXFILE                 (MSG_SSID_SEC_IXFILE)
#define SECDIAG_IXSQL                  (MSG_SSID_SEC_IXSQL)
#define SECDIAG_IXCOMMON               (MSG_SSID_SEC_IXCOMMON)
#define SECDIAG_BCASTCNTAGENT          (MSG_SSID_SEC_BCASTCNTAGENT)
#define SECDIAG_BCASTAUTH              (MSG_SSID_SEC_BCASTCNTAGENT) // TODO request appropriate SSID into msgcfg.h

/*
 * Security Message masks. DIAG MSG 2.0 allows masks (beyond 5 legacy masks) to be 
 * defined within a subsystem, or across common subsystems.
 */

#define MSG_SEC_LOW                 (MSG_MASK_0)
#define MSG_SEC_MED                 (MSG_MASK_1)
#define MSG_SEC_HIGH                (MSG_MASK_2)
#define MSG_SEC_ERROR               (MSG_MASK_3)
#define MSG_SEC_FATAL               (MSG_MASK_4) 
#define MSG_SEC_DEBUG               (MSG_MASK_5)


/*
 * Message Macros to send debug message to DIAG.
 */

/*---------------------------------------------------------------------------
  Macro for terse messages with no parameters.
---------------------------------------------------------------------------*/
#define SEC_DIAG_MSG(xx_ss_mask, xx_prio, xx_fmt) \
  do { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_mask, xx_prio, xx_fmt); \
      /*lint -e571 */ \
      msg_send (&xx_msg_const); \
  /*lint -e717 */ \
  } while (0) \
  /* lint +e717 */

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameter.
---------------------------------------------------------------------------*/
#define SEC_DIAG_MSG1(xx_ss_mask, xx_prio, xx_fmt, xx_arg1) \
  do { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_mask, xx_prio, xx_fmt); \
      /*lint -e571 */ \
      msg_send_1 (&xx_msg_const, (uint32) (xx_arg1)); \
  /*lint -e717 */ \
  } while (0) \
  /* lint +e717 */

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameter.
---------------------------------------------------------------------------*/
#define SEC_DIAG_MSG2(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2) \
  do { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_mask, xx_prio, xx_fmt); \
      /*lint -e571 */ \
      msg_send_2 (&xx_msg_const, (uint32)(xx_arg1), (uint32)(xx_arg2)); \
  /*lint -e717 */ \
  } while (0) \
  /* lint +e717 */

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameter.
---------------------------------------------------------------------------*/
#define SEC_DIAG_MSG3(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  do { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_mask, xx_prio, xx_fmt); \
      /*lint -e571 */ \
      msg_send_3 (&xx_msg_const, (uint32) (xx_arg1), (uint32) (xx_arg2), \
                                 (uint32) (xx_arg3)); \
  /*lint -e717 */ \
  } while (0) \
  /* lint +e717 */

/*---------------------------------------------------------------------------
  Macro for messages with 4 parameter.
---------------------------------------------------------------------------*/
#define SEC_DIAG_MSG4(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4) \
  do { \
      XX_MSG_CONST (xx_ss_mask, xx_prio, xx_fmt); \
      /*lint -e571 */ \
      msg_send_var (&xx_msg_const, (uint32)(4), (uint32) (xx_arg1), \
               (uint32) (xx_arg2), (uint32) (xx_arg3), (uint32) (xx_arg4)); \
      /*lint +e571 */ \
  /*lint -e717 */ \
  } while (0) \
  /* lint +e717 */

/*---------------------------------------------------------------------------
  Macro for messages with 5 parameter.
---------------------------------------------------------------------------*/
#define SEC_DIAG_MSG5(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5) \
  do { \
      XX_MSG_CONST (xx_ss_mask, xx_prio, xx_fmt); \
      /*lint -e571 */ \
      msg_send_var(&xx_msg_const, (uint32)(5), (uint32)(xx_arg1), \
               (uint32)(xx_arg2), (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                  (uint32)(xx_arg5)); \
      /*lint +e571 */ \
  /*lint -e717 */ \
  } while (0) \
  /* lint +e717 */

/*---------------------------------------------------------------------------
  Macro for messages with 6 parameter.
---------------------------------------------------------------------------*/
#define SEC_DIAG_MSG6(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6) \
  do { \
      XX_MSG_CONST (xx_ss_mask, xx_prio, xx_fmt); \
      /*lint -e571 */ \
      msg_send_var (&xx_msg_const, (uint32)(6), (uint32)(xx_arg1), \
                (uint32)(xx_arg2), (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                   (uint32)(xx_arg5), (uint32)(xx_arg6)); \
      /*lint +e571 */ \
  /*lint -e717 */ \
  } while (0) \
  /* lint +e717 */

/*---------------------------------------------------------------------------
  Macro for messages with 7 parameter.
---------------------------------------------------------------------------*/
#define SEC_DIAG_MSG7(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6, \
                                            xx_arg7) \
  do { \
      XX_MSG_CONST (xx_ss_mask, xx_prio, xx_fmt); \
      /*lint -e571 */ \
      msg_send_var (&xx_msg_const, (uint32)(7), (uint32)(xx_arg1), \
                (uint32)(xx_arg2), (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                   (uint32)(xx_arg5), (uint32)(xx_arg6), \
                                   (uint32)(xx_arg7)); \
      /*lint +e571 */ \
  /*lint -e717 */ \
  } while (0) \
  /* lint +e717 */

/*---------------------------------------------------------------------------
  Macro for messages with 8 parameter.
---------------------------------------------------------------------------*/
#define SEC_DIAG_MSG8(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6, \
                                            xx_arg7, xx_arg8) \
  do { \
      XX_MSG_CONST (xx_ss_mask, xx_prio, xx_fmt); \
      /*lint -e571 */ \
      msg_send_var (&xx_msg_const, (uint32)(8), (uint32)(xx_arg1), \
                (uint32)(xx_arg2), (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                   (uint32)(xx_arg5), (uint32)(xx_arg6), \
                                   (uint32)(xx_arg7), (uint32)(xx_arg8)); \
      /*lint +e571 */ \
  /*lint -e717 */ \
  } while (0) \
  /* lint +e717 */
  
/*---------------------------------------------------------------------------
  Macro for messages with 9 parameter.
---------------------------------------------------------------------------*/
#define SEC_DIAG_MSG9(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6, \
                                            xx_arg7, xx_arg8, xx_arg9) \
  do { \
      XX_MSG_CONST (xx_ss_mask, xx_prio, xx_fmt); \
      /*lint -e571 */ \
      msg_send_var (&xx_msg_const, (uint32)(9), (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                   (uint32)(xx_arg5), (uint32)(xx_arg6), \
                                   (uint32)(xx_arg7), (uint32)(xx_arg8), \
                                   (uint32)(xx_arg9)); \
      /*lint +e571 */ \
  /*lint -e717 */ \
  } while (0) \
  /* lint +e717 */


/*
 * Message Macros to send debug message in Extended Format to DIAG.
 */

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 1 parameters. msg_sprintf() 
  uses var arg list supported by the compiler.This Macro is used when xx_fmt
  is passed as a literal.
---------------------------------------------------------------------------*/
#define SEC_DIAG_SPRINTF1(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
				       /* lint +e717 */

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 2 parameters. msg_sprintf() 
  uses var arg list supported by the compiler.This Macro is used when xx_fmt
  is passed as a literal.
---------------------------------------------------------------------------*/
#define SEC_DIAG_SPRINTF2(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
				       /* lint +e717 */

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters. msg_sprintf() 
  uses var arg list supported by the compiler.This Macro is used when xx_fmt
  is passed as a literal.
---------------------------------------------------------------------------*/
#define SEC_DIAG_SPRINTF3(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
				       /* lint +e717 */

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 4 parameters. msg_sprintf() 
  uses var arg list supported by the compiler.This Macro is used when xx_fmt
  is passed as a literal.
---------------------------------------------------------------------------*/
#define SEC_DIAG_SPRINTF4(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                                    xx_arg4 ); \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3), \
               (uint32)(xx_arg4)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
				       /* lint +e717 */

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 5 parameters. msg_sprintf() 
  uses var arg list supported by the compiler.This Macro is used when xx_fmt
  is passed as a literal.
---------------------------------------------------------------------------*/
#define SEC_DIAG_SPRINTF5(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                                    xx_arg4, xx_arg5 ); \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3), \
               (uint32)(xx_arg4),  (uint32)(xx_arg5)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
				       /* lint +e717 */

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 6 parameters. msg_sprintf() 
  uses var arg list supported by the compiler.This Macro is used when xx_fmt
  is passed as a literal.
---------------------------------------------------------------------------*/
#define SEC_DIAG_SPRINTF6(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                                    xx_arg4, xx_arg5, xx_arg6 ); \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3), \
               (uint32)(xx_arg4),  (uint32)(xx_arg5), \
               (uint32)(xx_arg6)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
				       /* lint +e717 */

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 7 parameters. msg_sprintf() 
  uses var arg list supported by the compiler.This Macro is used when xx_fmt
  is passed as a literal.
---------------------------------------------------------------------------*/
#define SEC_DIAG_SPRINTF7(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                                    xx_arg4, xx_arg5, xx_arg6, \
                                                    xx_arg7 ); \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3), \
               (uint32)(xx_arg4),  (uint32)(xx_arg5), \
               (uint32)(xx_arg6),  (uint32)(xx_arg7)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
				       /* lint +e717 */

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 8 parameters. msg_sprintf() 
  uses var arg list supported by the compiler.This Macro is used when xx_fmt
  is passed as a literal.
---------------------------------------------------------------------------*/
#define SEC_DIAG_SPRINTF8(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                                    xx_arg4, xx_arg5, xx_arg6, \
                                                    xx_arg7, xx_arg8 ); \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3), \
               (uint32)(xx_arg4),  (uint32)(xx_arg5), \
               (uint32)(xx_arg6),  (uint32)(xx_arg7), \
               (uint32)(xx_arg8)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
				       /* lint +e717 */

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters. This Macro is 
  used when xx_fmt is passed at runtime.
---------------------------------------------------------------------------*/
#define SEC_DIAG_SPRINTF_FMT_VAR_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_MSG_SSID_APPS_QTV)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST_FMT_VAR (MSG_SSID_APPS_QTV, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const, (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0)
                       

#endif /* SECDIAG_H */
