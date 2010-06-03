#ifndef __IXCONTENTINIT_H__
#define __IXCONTENTINIT_H__

/*===========================================================================

                         I X  C O N T E N T  I N I T

                             H E A D E R  F I L E

FILE:  IxContentInit.h


GENERAL DESCRIPTION

  IxContet Initialization

PUBLIC CLASSES AND STATIC FUNCTIONS
  
INITIALIZATION & SEQUENCING REQUIREMENTS
  
  Copyright (c) 2006 - 2007 by QUALCOMM, Incorporated.  All Rights Reserved.  
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
                            
  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/com/inc/IxContentInit.h#5 $
  $DateTime: 2008/03/14 18:44:16 $
  $Author: rajeevg $

===========================================================================*/


/*===========================================================================
                           
               INCLUDE FILES

============================================================================*/

#include "customer.h"
#include "comdef.h"

/*
** DUAL_PD is defined for dual PD builds.
** SEC_PD is defined on all single-PD builds, and also on dual PD builds
** if this piece of code is being built as part of the SEC PD.
** AMSS_PD is defined on all single PD builds, and also on dual PD builds
** if this piece of code is being built as part of the AMSS PD.
** MINIMAL_DUAL_PD is defined on builds where two PDs exist, but there is
** minimal functionality in the second PD.
*/
#ifdef SEC_PD
  #define THIS_PD_NAME    "SEC PD"
  #define OTHER_PD_NAME   "AMSS PD"
  #define THIS_PD_ID      5
  #ifdef MINIMAL_DUAL_PD
    #define THIS_PD_IPC_SVC  IPCSVC_IXCONTENT_AMSS
    #define OTHER_PD_IPC_SVC IPCSVC_IXCONTENT
  #else
    #define THIS_PD_IPC_SVC  IPCSVC_IXCONTENT
    #define OTHER_PD_IPC_SVC IPCSVC_IXCONTENT_AMSS
  #endif
#else
  #define THIS_PD_NAME    "AMSS PD"
  #define OTHER_PD_NAME   "SEC PD"
  #define THIS_PD_ID      6
  #ifdef MINIMAL_DUAL_PD
    #define THIS_PD_IPC_SVC  IPCSVC_IXCONTENT
    #define OTHER_PD_IPC_SVC IPCSVC_IXCONTENT_AMSS
  #else
    #define THIS_PD_IPC_SVC  IPCSVC_IXCONTENT_AMSS
    #define OTHER_PD_IPC_SVC IPCSVC_IXCONTENT
  #endif
#endif /* SEC_PD */

#if defined(DUAL_PD) && defined(SEC_PD)
  #define FEATURE_AMSSAGENT_TASK
#endif

#if defined(DUAL_PD) && defined(SEC_PD)
  #define FEATURE_SECPD_REXTIMER_TASK
#endif

#if defined(DUAL_PD) && defined(AMSS_PD)
  #define FEATURE_SECAGENT_TASK
#endif

#if defined(AMSS_PD) && defined(FEATURE_IXFILESVC)
  #define FEATURE_IX_FILESVC_TASK
#endif

#if defined(DUAL_PD) && defined(AMSS_PD)
  #define FEATURE_IX_DIAGSVC_TASK
#endif

#if defined(SEC_PD) && !defined(IMAGE_MODEM_PROC) && defined(FEATURE_IXSQLSVC)
  #define FEATURE_IX_SQLSVC_TASK
#endif

#if defined(SEC_PD) && !defined(IMAGE_MODEM_PROC) && defined(FEATURE_IXSQLSVC)
  #define FEATURE_IX_SQLCLNT_TASK
#endif

#if defined(AMSS_PD) && !defined(IMAGE_MODEM_PROC) && defined(FEATURE_IXHTTPSVC)
  #define FEATURE_IX_HTTPSVC_TASK
#endif

#if defined(AMSS_PD) && defined(FEATURE_IXTIMESVC)
  #define FEATURE_IX_TIMESVC_TASK
#endif

#if defined(AMSS_PD) && defined(FEATURE_ICONTENT) && \
    !defined(IMAGE_MODEM_PROC)
  #define FEATURE_IX_CONTENTCLNT_TASK
#endif

#if defined(AMSS_PD) && defined(FEATURE_IXBCASTCONTENTSVC) && \
   !defined(IMAGE_MODEM_PROC)
  #define FEATURE_IX_BCASTCONTENTCLNT_TASK
  #define FEATURE_IX_BCASTKEYTPORTSVC_TASK
#endif

#if defined(SEC_PD) && defined(FEATURE_IXBCASTCONTENTSVC) && \
   !defined(IMAGE_MODEM_PROC)
  #define FEATURE_IX_BCASTCONTENTSVC_TASK
#endif

#if defined(FEATURE_SEC)
  #define FEATURE_SEC_TASK
#endif

#if defined(FEATURE_SEC_ARM_CRYPTO)
  #define FEATURE_SEC_ARM_CRYPTO_TASK
#endif

#if defined(FEATURE_SECRND) && \
    defined(SEC_PD) && !defined(IMAGE_APPS_PROC)
  #define FEATURE_SECRND_TASK
#endif

#if defined(FEATURE_SEC_USFS) && defined(SEC_PD) && \
   !defined(IMAGE_APPS_PROC)
  #define FEATURE_SEC_USFS_TASK
#endif

#if defined(FEATURE_SEC_CONTENT_AGENT) && defined(SEC_PD) && \
   !defined(IMAGE_MODEM_PROC)
  #define FEATURE_SEC_CONTENT_AGENT_TASK
#endif

#if defined(FEATURE_SEC) && defined(FEATURE_IPC_SECTEST) && defined(SEC_PD)
  #define FEATURE_IPC_SECTEST_TASK
#endif

/*
** IPC signal for the Content Service task.
*/
#define IXCONTENTSERVER_IPC_SIG    0x100

/*
** Signal used by the IxContent initialization timer
*/
#define IXCONTENT_INIT_TIMER_SIG   0x200 

/*
** Dog report signal for the Content Service task
*/
#define IXCONTENT_RPT_TIMER_SIG    0x300

/*
** Signal used by Content-related tasks to ack the Content Service task
*/
#define IXCONTENT_ACK_SIG          0x400

/*
** Signal used by Content Task to signal completion of NV commands
*/
#define IXCONTENT_NV_SIG          0x800

/* Enumeration of AMSS functions that need to be accessed from the
** SEC PD.
**
** NOTE: Please make sure that the symbols in the enumeration below are in
** alphabetical order. This is to avoid mismatching functions and their
** addresses in the table of exported functions.
*/
typedef enum
{
  CLK_PAUSE_E,
  DIAG_FS_REGISTER_ACCESS_CHECK_E,
  DOG_MONITOR_PAUSE_E,
  DOG_MONITOR_RESUME_E,
  DOG_REPORT_E,
  FLASH_NAND_DEVICE_PROBE_E,
  QDSP_CURRENT_DMA_TABLE_E,
  AMSS_MAX_FUNC_E
} exportedAMSSFuncE;

/* Enumeration of SEC functions that need to be accessed from the
** AMSS PD.
*/
typedef enum
{
  SEC_HEAP_PTR_E,
  SEC_HEAP_BUF_PTR_E,
  SEC_MAX_FUNC_E
} exportedSECFuncE;

#ifdef DUAL_PD
  #ifdef SEC_PD
    #define NUM_EXPORTED_FUNCS SEC_MAX_FUNC_E
    #define NUM_IMPORTED_FUNCS AMSS_MAX_FUNC_E
  #else
    #define NUM_EXPORTED_FUNCS AMSS_MAX_FUNC_E
    #define NUM_IMPORTED_FUNCS SEC_MAX_FUNC_E
  #endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================
* FUNCTION:  IxContentIPCSync ()
*
* This function blocks until the IxContent main task is done
* with its IPC initialization.
==============================================================*/
void IxContentIPCSync (void);

/*==============================================================
* FUNCTION:  IxContentMain()
*
* Entry point for the main IxContent task.
==============================================================*/
void IxContentMain (dword);

#ifdef __cplusplus
}
#endif

#endif /* IXCONTENTINIT_H */

