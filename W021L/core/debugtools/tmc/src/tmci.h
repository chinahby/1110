#ifndef TMCI_H
#define TMCI_H
/*===========================================================================

               T A S K  M A I N  C O N T R O L L E R
               
               I N T E R N A L   H E A D E R   F I L E

DESCRIPTION
  This header file contains definitions used internally by the Task Main
  Controller, as well as, providing internal interface to the TMC.C file.
   
  This header file should NOT be included by any source file outside the
  Task Main Controller module.
  
Copyright (c) 2000 by QUALCOMM, Incorporated.  All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/services/tmc/tmci.h#12 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/19/04   jqi     Added support for FEATURE_CHG_INIT_NO_PARA to resolve
                   compilation issue for MSM6250 target.
10/31/03    sr     Removed parameters to SBI API's
10/29/03   jqi     Consolidate MSM6200 target related features.
10/07/03   jqi     Add tmcdiag_init() prototype.
03/18/03   sb      Undefined TMC_DEBUG
08/16/02   jqi     Removed temporary UIM features.
                   Added temporary WMS, SLEEP feature.
07/29/02   jqi     Remove redundant features of TMC supported task.
07/24/02   jqi     TMC Reorganization.
07/08/02   jqi     Initial release.

===========================================================================*/


#include "msg.h"                                    /* Message header file */
#include "err.h"                                      /* Error header file */
#include "customer.h"                       /* Customer configuration file */
#include "comdef.h"               /* Definition for basic types and macros */
#ifdef FEATURE_BATTERY_CHARGER
  #include "charger.h"                              /* Charger header file */
#endif


/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
================================== COMMON ===================================
=============================================================================
=============================================================================
===========================================================================*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Macro to correctly align function definitions (cosmetic).
*/
#define EXTERN


/* <EJECT> */
/*---------------------------------------------------------------------------
                         MISCELLANEOUS TYPES AND MACROS
---------------------------------------------------------------------------*/

/* Macro to check whether target is 1x mode.
*/
#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
  #define TMC_CDMA
#endif

#ifdef FEATURE_ACP
#error code not present
#endif

#ifdef FEATURE_HDR
#error code not present
#endif

#ifdef FEATURE_GSM
#error code not present
#endif

#ifdef FEATURE_WCDMA
#error code not present
#endif

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif

#if defined(TMC_CDMA) || defined(TMC_ACP) || defined(TMC_HDR)
  #define TMC_1X
#endif

#ifdef TMC_1X
  #ifdef FEATURE_CDSMS
    #define TMC_WMS
  #endif
  
  #ifdef T_SLEEP
    #define TMC_SLEEP
  #endif  
#endif

#ifdef TMC_GSMWCDMA
#error code not present
#endif

#if defined(T_MSM6200) || defined(FEATURE_SET_RAN_SEED)
  #define TMC_SET_RAN_SEED
#endif

#if ( defined(T_MSM6200) || defined(FEATURE_HW_IMPLEMENT_LPM)) && \
    defined(FEATURE_LPM)
  #define TMC_HW_IMPLEMENT_LPM
#endif

#if defined(T_MSM6200) && !defined(FEATURE_BIO_INIT_DURING_BOOT)
#error code not present
#endif

#if defined(T_MSM6275)
#error code not present
#endif

#if defined(T_MSM6200) || defined(FEATURE_HW_INIT)
  #define TMC_HW_INIT
#endif

#ifdef FEATURE_BATTERY_CHARGER
  #if defined(T_MSM6200) && !defined(FEATURE_CHG_INIT_NO_PARA)
#error code not present
  #else
    #define tmc_chg_init()   chg_init()
  #endif
#endif

#if (defined(FEATURE_MULTIMODE_RFAPI) && defined(T_MSM6100) && \
     (!defined(T_MSM6300))) || (defined(FEATURE_GSM_QUADBAND_1X_DUALBAND))
  #define TMC_RFM_ENTER_MODE
#endif

#if (defined(T_MSM6050) || defined(T_MSM6250)) && defined(FEATURE_I2C)
  #define TMC_I2C_INIT
#endif


/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
===================== TMC DEBUG RELATED STUFF ===============================
=============================================================================
=============================================================================
===========================================================================*/

/*---------------------------------------------------------------------------
      Compile TMC with debug information, asserts and messages.
---------------------------------------------------------------------------*/

#undef TMC_DEBUG


/* <EJECT> */
/*---------------------------------------------------------------------------
                  TMC_DEBUG/T_PC independent macros.
---------------------------------------------------------------------------*/
#define TMC_MSG_LOW( str, a, b, c )       MSG_LOW( "=TMC= " str, a, b, c)
#define TMC_MSG_MED( str, a, b, c )       MSG_MED( "=TMC= " str, a, b, c)
#define TMC_MSG_HIGH( str, a, b, c )      MSG_HIGH( "=TMC= " str, a, b, c)
#define TMC_MSG_ERROR( str, a, b, c )     MSG_ERROR( "=TMC= " str, a, b, c)


/* <EJECT> */
/*---------------------------------------------------------------------------
                   TMC_DEBUG/T_PC dependent macros.
---------------------------------------------------------------------------*/

#ifdef TMC_DEBUG
#error code not present
#elif( TG==T_PC )
#error code not present
#else
  
  #define TMC_ERR_FATAL(str,a,b,c )      ERR_FATAL( "",a,b,c )
  
  #define TMC_ERR( str,a,b,c )           ERR( "",a,b,c )
  #define TMC_CASE_ERR( str,a,b,c )      ERR( "",a,b,c )
  #define TMC_SWITCH_ERR( str,a,b,c )    ERR( "",a,b,c )
  #define TMC_ELSE_ERR( str,a,b,c )      ERR( "",a,b,c )
                        
  #define TMC_FUNC_START( str,a,b,c )
  #define TMC_FUNC_END( str,a,b,c )

  #define TMC_FUNC_NOT_IMP( func )       ERR( "",0,0,0 )
  #define TMC_CASE_NOT_IMP( _case )      ERR( "",0,0,0 )

#endif

/*---------------------------------------------------------------------------
                  Asserts and verifications macros
---------------------------------------------------------------------------*/

#ifdef TMC_DEBUG
#error code not present
#else
  #define TMC_ASSERT( exp )
#endif


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


#ifdef TMC_DEBUG
#error code not present
#else
  #define TMC_ASSERT_ENUM_IS_INRANGE( val, max )
#endif


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


#ifdef TMC_DEBUG
#error code not present
#else
  #define TMC_ASSERT_ENUM_IS_INRANGE_INC( val, max )
#endif


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


#ifdef TMC_DEBUG
#error code not present
#else
  #define TMC_ASSERT_ENUM_IS_INRANGE_OR( val, max, equal )
#endif

/*---------------------------------------------------------------------------
                         EXTERNAL FUNCTION PROTOTYPE
---------------------------------------------------------------------------*/
extern void tmcdiag_init (void);

#endif /* TMCI_H */

