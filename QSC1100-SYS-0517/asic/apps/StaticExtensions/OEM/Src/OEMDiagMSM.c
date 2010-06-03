/*============================================================================

FILE:  OEMDiag.c

SERVICES: Diagnositic APIs

==============================================================================

               Copyright © 2007 QUALCOMM Incorporated 
                       All Rights Reserved.
                   QUALCOMM Proprietary/GTDR

============================================================================*/

/* Include DMSS headers before BREW headers. */
#include "customer.h"
#include "comdef.h"
#include "diag.h"
#include "queue.h"
#include "log.h"
#include "rbq.h"
#include "fs_public.h"
#include "fs_sys_types.h"
#include "OEMFeatures.h"
#include "BREWVersion.h"

#if defined (AEE_STATIC)
#include "AEE_OEMDispatch.h"
#endif

// This is to be disabled for commercial releases.
#undef DEBUG_OEMDIAGMSM

#if defined (AEE_STATIC) && defined (DEBUG_OEMDIAGMSM)
#error code not present
#else
  #undef MSG
  #undef MSG_1
  #undef MSG_2
  #undef MSG_3
  #undef MSG_4
  #undef MSG_5

  // even though might not use these macros now, we should define them 
  // for future use. Make sure lint doesn't complain about not referenced 
  // symbols
  /*lint -esym(750, MSG) */
  /*lint -esym(750, MSG_1) */
  /*lint -esym(750, MSG_2) */
  /*lint -esym(750, MSG_3) */
  /*lint -esym(750, MSG_4) */
  /*lint -esym(750, MSG_5) */
  #define MSG(xx_ssid, xx_mask, xx_fmt) (void *) 0
  #define MSG_1(xx_ssid, xx_mask, xx_fmt, xx_1) (void *) 0
  #define MSG_2(xx_ssid, xx_mask, xx_fmt, xx_1, xx_2) (void *) 0
  #define MSG_3(xx_ssid, xx_mask, xx_fmt, xx_1, xx_2, xx_3) (void *) 0
  #define MSG_4(xx_ssid, xx_mask, xx_fmt, xx_1, xx_2, xx_3, xx_4) (void *) 0
  #define MSG_5(xx_ssid, xx_mask, xx_fmt, xx_1, xx_2, xx_3, xx_4,xx_5) (void *)0
#endif

#include "AEEComdef.h"
#include "AEE.h"       /* Standard AEE declarations */
#include "AEEError.h"  /* Error codes for return values */
#include "AEEShell.h"  /* AEE Shell services, IShell declarations */
#include "AEEStdLib.h" /* AEE StdLib decls */
#include "AEEFile.h"
#include "AEEX509Chain.h"   /* IX509Chain */

#include "AEE_OEM.h"   /* AEE_GetAppContext */

#include "IDiagMSM.h"
#include "AEESecurity.h"

#ifndef WIN32
  #include "rex.h"
#endif

#define AEECLSID_IDIAGMSM_PRIV      0x01023c44

typedef struct _IDiagMSM DiagMSM;

#define OEMDIAGMSM_MIN_LOG_BUF_SIZE (1024)
#define OEMDIAGMSM_MIN_EVT_BUF_SIZE (256)

/* A random pattern to detect if a given buffer has been initialized */
/*lint -esym(750, OEMDIAGMSM_BUF_INIT_PATTERN) */
#define OEMDIAGMSM_BUF_INIT_PATTERN (0x06101975)

// This should only be used for function parameters, not for local variables
#define PARAM_NOT_REF(param)         /*lint -esym(715, param) */

// This should only be used for function parameters, not for local variables
// Pointer parameter could be declared as pointing to const
#define PARAM_NOT_CONST(param)       /*lint -esym(818, param) */

#undef OEMDIAGMSM_DUMPLOG

// The following is used to track data flow and which queue it is in.
enum 
{
  OEMDIAGMSM_STD_RBQ = 0,
  OEMDIAGMSM_OF_RBQ
};

// This structure is used for servicing a AEECallback when a response is 
// generated from DIAG.
typedef struct
{
  q_link_type mLink;
  DiagMSM *p_mDiagMSM;
  
  void *pUserData;
  AEECallback *pCb;
   
} OEMDiagMSMPktContextType;

// This structure holds a response to a DIAG request packet.
typedef struct
{
  q_link_type mLink;
   
  IDiagMSMRspType mRsp;

} OEMDiagMSMRspType;

// This module uses the RBQ module for buffering/queuing.
typedef struct
{
  void *mRbq;

} OEMDiagMSMQType;

typedef struct
{
  q_link_type mLink; /* Link to the master listener Q */

  DiagMSM *p_mDiagMSM;

  OEMDiagMSMQType mReadyQ;
  OEMDiagMSMQType mOverflowQ; // If unable to place inbound record on the ReadyQ
  
#if defined (DEBUG_OEMDIAGMSM)
#error code not present
#endif

  IDiagMSMListenerType mListenerInfo;

} OEMDiagMSMListenerType;

// Lint Warning 754 "local structure member not referenced"
/*lint -esym(754, pad) */
typedef struct
{
  uint32 dwSize;
  
  uint8 cQ;     //Which Q is it in?
  uint8 pad[3]; // Explicitly align the structure
  
  IDiagMSMRecordType mRec;

} OEMDiagMSMRecType;

////////////////////////////////////////////////////////////////////////////////
typedef struct _DiagMSM_UnlockUD
{
  AEECallback * m_pCallerCB;
  uint32      * m_poutRes;
  uint8         m_bPrivLevel;

  // New for certificates
  uint16        m_wNonce16;
  IX509Chain  * m_pX509Chain;
  WebOpt      * m_pWebOpts;
  X509TrustResult   * m_pX509Result;
  
} DiagMSM_UnlockUD;

struct _IDiagMSM
{
  AEEVTBL(IDiagMSM) *pvt;

   // Standard data members
  IShell        * p_mIShell;
#if !defined (AEE_STATIC)
  IModule       * p_mIModule;
#endif
  uint32        m_nRefs;
//   AEECLSID        clsid;

#ifdef OEMDIAGMSM_DUMPLOG
  IFile         * m_pIFileLog;
  IFile         * m_pIFileEvent;
#endif
  //DiagMSM specific members

  q_type mListenerQ;
  q_type mPendingRspQ;
  q_type mRspQ;

  DiagMSM_UnlockUD * m_pUnlock;  

  ACONTEXT *pAc;               // points to the Context of the "Caller Applet" 

  int                m_nLastFileError;

  // New for certificates
  AEECallback        * m_pVerifyCB;
  uint32             m_dwNonce32;
  boolean            m_bIsUnlocked;
  uint8              m_bPrivLevel;
};

// This macro converts a listener handle to a listener descriptor, as well as 
// perform data verification.
/*lint -esym(750, OEMDIAGMSM_CVT_H2D) */
#define OEMDIAGMSM_CVT_H2D(xx_handle) \
   ((xx_handle != IDIAGMSM_LHANDLE_NULL && \
      ((OEMDiagMSMListenerType *) xx_handle)->pDesc && \
      ((OEMDiagMSMListenerType *) xx_handle)->pMe->dwInitPattern == \
        OEMDIAGMSM_BUF_INIT_PATTERN) \
      ? (((OEMDiagMSMListenerType *) xx_handle)->pDesc) : NULL)
     
// This macro converts a listener handle to a descriptor without 
// verifying validity.
#define OEMDIAGMSM_CVT_H2L(xx_handle) \
  ((xx_handle != IDIAGMSM_LHANDLE_NULL) ? \
     (OEMDiagMSMListenerType *) xx_handle : NULL)

#ifdef OEMDIAGMSM_DUMPLOG
#define OEMDIAGMSM_DUMPLOG_FILE     "dumplog.log"
#define OEMDIAGMSM_DUMPEVENT_FILE   "dumpevent.log"
#endif

// IDiagMSM Root certificate. DO NOT CHANGE.
static const uint8 gabCaCert[] =  {
0x30, 0x82, 0x04, 0x43, 0x30, 0x82, 0x03, 0x2b, 0xa0, 0x03, 0x02, 0x01, 0x02, 
0x02, 0x10, 0x33, 0x45, 0x92, 0xca, 0x4d, 0x5f, 0x81, 0xbb, 0x41, 0x82, 0x64, 
0x2d, 0x05, 0x34, 0x14, 0xf7, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 
0xf7, 0x0d, 0x01, 0x01, 0x05, 0x05, 0x00, 0x30, 0x81, 0xb3, 0x31, 0x37, 0x30, 
0x35, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x09, 0x01, 0x16, 
0x28, 0x63, 0x65, 0x72, 0x74, 0x69, 0x66, 0x69, 0x63, 0x61, 0x74, 0x65, 0x2d, 
0x61, 0x75, 0x74, 0x68, 0x6f, 0x72, 0x69, 0x74, 0x79, 0x2e, 0x61, 0x64, 0x6d, 
0x69, 0x6e, 0x40, 0x71, 0x75, 0x61, 0x6c, 0x63, 0x6f, 0x6d, 0x6d, 0x2e, 0x63, 
0x6f, 0x6d, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 
0x55, 0x53, 0x31, 0x13, 0x30, 0x11, 0x06, 0x03, 0x55, 0x04, 0x08, 0x13, 0x0a, 
0x43, 0x61, 0x6c, 0x69, 0x66, 0x6f, 0x72, 0x6e, 0x69, 0x61, 0x31, 0x12, 0x30, 
0x10, 0x06, 0x03, 0x55, 0x04, 0x07, 0x13, 0x09, 0x53, 0x61, 0x6e, 0x20, 0x44, 
0x69, 0x65, 0x67, 0x6f, 0x31, 0x11, 0x30, 0x0f, 0x06, 0x03, 0x55, 0x04, 0x0a, 
0x13, 0x08, 0x51, 0x55, 0x41, 0x4c, 0x43, 0x4f, 0x4d, 0x4d, 0x31, 0x2f, 0x30, 
0x2d, 0x06, 0x03, 0x55, 0x04, 0x03, 0x13, 0x26, 0x4d, 0x6f, 0x62, 0x69, 0x6c, 
0x65, 0x56, 0x69, 0x65, 0x77, 0x2d, 0x49, 0x44, 0x69, 0x61, 0x67, 0x4d, 0x53, 
0x4d, 0x20, 0x43, 0x65, 0x72, 0x74, 0x69, 0x66, 0x69, 0x63, 0x61, 0x74, 0x65, 
0x20, 0x53, 0x65, 0x72, 0x76, 0x65, 0x72, 0x30, 0x1e, 0x17, 0x0d, 0x30, 0x36, 
0x31, 0x30, 0x32, 0x35, 0x31, 0x30, 0x33, 0x32, 0x30, 0x33, 0x5a, 0x17, 0x0d, 
0x33, 0x31, 0x31, 0x30, 0x32, 0x35, 0x31, 0x30, 0x33, 0x39, 0x30, 0x38, 0x5a, 
0x30, 0x81, 0xb3, 0x31, 0x37, 0x30, 0x35, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 
0xf7, 0x0d, 0x01, 0x09, 0x01, 0x16, 0x28, 0x63, 0x65, 0x72, 0x74, 0x69, 0x66, 
0x69, 0x63, 0x61, 0x74, 0x65, 0x2d, 0x61, 0x75, 0x74, 0x68, 0x6f, 0x72, 0x69, 
0x74, 0x79, 0x2e, 0x61, 0x64, 0x6d, 0x69, 0x6e, 0x40, 0x71, 0x75, 0x61, 0x6c, 
0x63, 0x6f, 0x6d, 0x6d, 0x2e, 0x63, 0x6f, 0x6d, 0x31, 0x0b, 0x30, 0x09, 0x06, 
0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x55, 0x53, 0x31, 0x13, 0x30, 0x11, 0x06, 
0x03, 0x55, 0x04, 0x08, 0x13, 0x0a, 0x43, 0x61, 0x6c, 0x69, 0x66, 0x6f, 0x72, 
0x6e, 0x69, 0x61, 0x31, 0x12, 0x30, 0x10, 0x06, 0x03, 0x55, 0x04, 0x07, 0x13, 
0x09, 0x53, 0x61, 0x6e, 0x20, 0x44, 0x69, 0x65, 0x67, 0x6f, 0x31, 0x11, 0x30, 
0x0f, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x13, 0x08, 0x51, 0x55, 0x41, 0x4c, 0x43, 
0x4f, 0x4d, 0x4d, 0x31, 0x2f, 0x30, 0x2d, 0x06, 0x03, 0x55, 0x04, 0x03, 0x13, 
0x26, 0x4d, 0x6f, 0x62, 0x69, 0x6c, 0x65, 0x56, 0x69, 0x65, 0x77, 0x2d, 0x49, 
0x44, 0x69, 0x61, 0x67, 0x4d, 0x53, 0x4d, 0x20, 0x43, 0x65, 0x72, 0x74, 0x69, 
0x66, 0x69, 0x63, 0x61, 0x74, 0x65, 0x20, 0x53, 0x65, 0x72, 0x76, 0x65, 0x72, 
0x30, 0x82, 0x01, 0x22, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 
0x0d, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x82, 0x01, 0x0f, 0x00, 0x30, 0x82, 
0x01, 0x0a, 0x02, 0x82, 0x01, 0x01, 0x00, 0x91, 0x80, 0xe9, 0x86, 0xc0, 0xa2, 
0x45, 0xf3, 0x12, 0x46, 0x50, 0x69, 0x34, 0x1b, 0x82, 0xf6, 0x0b, 0xaf, 0x01, 
0xc4, 0x53, 0x23, 0x3d, 0x55, 0x3c, 0xa7, 0x5c, 0xc6, 0x22, 0xef, 0x44, 0xdc, 
0x39, 0x8c, 0xca, 0x63, 0x01, 0x31, 0x0f, 0x58, 0x47, 0x40, 0x70, 0x6c, 0x41, 
0x14, 0x1a, 0x1d, 0xce, 0xb9, 0x9e, 0x28, 0x49, 0x79, 0x54, 0x84, 0x08, 0x50, 
0xe2, 0xc0, 0xfc, 0x06, 0xde, 0x27, 0xb9, 0x0b, 0x16, 0x71, 0x79, 0x39, 0xab, 
0x1d, 0x8b, 0x7e, 0x01, 0x62, 0xa8, 0xa7, 0x4f, 0x95, 0x7f, 0x8e, 0x4d, 0x3c, 
0x65, 0x25, 0x84, 0x47, 0x83, 0xb1, 0x93, 0x8e, 0x9e, 0x8e, 0x50, 0x39, 0x13, 
0xbf, 0x7c, 0xc8, 0x5a, 0xbe, 0xed, 0xd5, 0xef, 0x2a, 0x78, 0x8d, 0x5a, 0x50, 
0x8b, 0xd0, 0xc9, 0xad, 0xf9, 0x69, 0x14, 0x7f, 0x19, 0x29, 0x2c, 0xb8, 0x13, 
0x02, 0x64, 0x11, 0x4f, 0x4a, 0xdc, 0x30, 0x72, 0x79, 0xd6, 0xf9, 0x46, 0x55, 
0xa4, 0x95, 0x5a, 0x18, 0xd2, 0xec, 0xde, 0xb4, 0x1e, 0x7b, 0x04, 0x3e, 0xa5, 
0x2c, 0xe1, 0x92, 0x52, 0x1d, 0xa0, 0xea, 0x5a, 0xb7, 0x56, 0x76, 0x05, 0x73, 
0x34, 0x85, 0x37, 0x35, 0x02, 0xb7, 0xd6, 0xe5, 0xb9, 0x6b, 0x14, 0x5f, 0x05, 
0xa2, 0x3f, 0xf4, 0x27, 0x9e, 0x9a, 0xe2, 0x53, 0x2c, 0x4f, 0x5c, 0x7f, 0x7e, 
0x78, 0x38, 0x09, 0xfe, 0xd2, 0xcb, 0xc1, 0x24, 0x18, 0xfd, 0xc9, 0x45, 0x9b, 
0xba, 0xe4, 0x4f, 0xd4, 0x9a, 0xb1, 0x90, 0xa4, 0x80, 0x6c, 0xf7, 0xec, 0x12, 
0x53, 0x8c, 0x3d, 0xfa, 0x55, 0x51, 0x04, 0xae, 0xbf, 0xbf, 0x40, 0x28, 0x28, 
0x94, 0x36, 0xf7, 0x71, 0x9a, 0x7f, 0x76, 0xad, 0x66, 0x9d, 0x50, 0x6e, 0x98, 
0x7f, 0x1a, 0x50, 0x9f, 0xdf, 0x7a, 0xec, 0x19, 0x24, 0x6c, 0x3c, 0x35, 0x33, 
0xa2, 0xb1, 0x99, 0x02, 0x03, 0x01, 0x00, 0x01, 0xa3, 0x51, 0x30, 0x4f, 0x30, 
0x0b, 0x06, 0x03, 0x55, 0x1d, 0x0f, 0x04, 0x04, 0x03, 0x02, 0x01, 0x86, 0x30, 
0x0f, 0x06, 0x03, 0x55, 0x1d, 0x13, 0x01, 0x01, 0xff, 0x04, 0x05, 0x30, 0x03, 
0x01, 0x01, 0xff, 0x30, 0x1d, 0x06, 0x03, 0x55, 0x1d, 0x0e, 0x04, 0x16, 0x04, 
0x14, 0x14, 0x69, 0x06, 0x44, 0x52, 0x6e, 0xd3, 0x5c, 0xd4, 0x86, 0x91, 0xe5, 
0xc1, 0x97, 0x4e, 0xd1, 0x6d, 0xc3, 0x93, 0x17, 0x30, 0x10, 0x06, 0x09, 0x2b, 
0x06, 0x01, 0x04, 0x01, 0x82, 0x37, 0x15, 0x01, 0x04, 0x03, 0x02, 0x01, 0x00, 
0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x05, 
0x05, 0x00, 0x03, 0x82, 0x01, 0x01, 0x00, 0x20, 0x90, 0x91, 0x94, 0x8b, 0x96, 
0xa0, 0xd4, 0xca, 0xaf, 0xc4, 0xab, 0x6c, 0x2e, 0xc4, 0xd7, 0x8d, 0x1d, 0xd1, 
0x57, 0x1f, 0x88, 0x56, 0x14, 0x8d, 0xe6, 0x27, 0x99, 0xd7, 0xcb, 0x06, 0x1a, 
0x96, 0x7d, 0x4d, 0xea, 0x65, 0x7f, 0x0a, 0x82, 0xd3, 0xa9, 0x8c, 0xc7, 0xb1, 
0x18, 0x95, 0x88, 0x9b, 0xdb, 0xd4, 0x5f, 0x29, 0x87, 0x9f, 0xb2, 0x46, 0x44, 
0xac, 0x2e, 0x7a, 0x9e, 0xee, 0x8c, 0x2f, 0x8f, 0x0c, 0xe7, 0x73, 0x37, 0x8f, 
0xfe, 0x22, 0x31, 0x86, 0x9d, 0x2e, 0x0e, 0xd3, 0x96, 0x04, 0xb1, 0xb8, 0x93, 
0x24, 0x46, 0x2f, 0xa0, 0x6f, 0xa2, 0xa7, 0x6b, 0x50, 0xfd, 0x48, 0xb9, 0xf8, 
0x37, 0x31, 0x36, 0x37, 0xfc, 0xcb, 0xcc, 0x3b, 0xf9, 0x73, 0xf3, 0xd0, 0x1d, 
0xf6, 0x39, 0xb9, 0x23, 0x70, 0x89, 0xb7, 0xb5, 0xb4, 0x72, 0x89, 0x52, 0x18, 
0x31, 0xd1, 0x9c, 0xe2, 0xa7, 0x7f, 0x73, 0x8a, 0x63, 0xca, 0x93, 0x85, 0x48, 
0x07, 0xce, 0x99, 0xae, 0xb1, 0xd0, 0xf2, 0x63, 0xe0, 0x7a, 0x17, 0x22, 0x93, 
0xec, 0x9e, 0xbd, 0x28, 0x32, 0x5f, 0x20, 0x42, 0x6d, 0x63, 0xb1, 0x1d, 0xbe, 
0x63, 0x62, 0xb7, 0xa8, 0xd2, 0xc7, 0xaf, 0x0a, 0x72, 0xbf, 0x51, 0x4c, 0x35, 
0x62, 0x7c, 0xff, 0xad, 0x67, 0x3d, 0x65, 0xf8, 0x5c, 0xfc, 0x0e, 0x92, 0x45, 
0x8d, 0x87, 0xb7, 0x63, 0x26, 0xf4, 0xe5, 0x6b, 0x69, 0xb2, 0xdf, 0x09, 0xe6, 
0xbd, 0x70, 0x63, 0x10, 0xfb, 0x27, 0xe8, 0xa8, 0x79, 0x51, 0x7e, 0xfb, 0x4a, 
0xfa, 0xd3, 0x1a, 0x3c, 0x4d, 0x44, 0xb7, 0x1a, 0x5c, 0x8c, 0xb2, 0x8b, 0xfb, 
0x0a, 0x36, 0x34, 0xd3, 0x1a, 0xbd, 0xd4, 0x8f, 0xb6, 0xdf, 0xbb, 0x06, 0xc1, 
0xdb, 0x81, 0x0f, 0x07, 0xb9, 0x3a, 0xb1, 0x45, 0x4e, 0x11, 0x8b, 0x92, 0xe1, 
0x96, 0x02, 0x52};

// Length of IDiagMSM root certificate
#define DIAGMSM_CACERT_LEN        sizeof(gabCaCert)

// Maximum retries to get a non-zero random number
#define DIAGMSM_MAX_NONCE_RETRIES 100

// String to search for in the Leaf CN oid for privilege levels
#define DIAGMSM_PRIV_ALL_TXT      "{P_ALL}"
#define DIAGMSM_PRIV_EFS_TXT      "{P_EFS}"
#define DIAGMSM_PRIV_LOG_TXT      "{P_LOG}"
#define DIAGMSM_PRIV_LAST_TXT     "{P_INVALID}"

// Table of privilege level strings mapped to corresponding enum
// IMPORTANT: This table needs to be updated whenever a privilege level is 
// added to/removed from enum IDiagMSM_PrivLevels
static const char * gaszDiagMSMPrivLevels[] = {
  DIAGMSM_PRIV_ALL_TXT,
  DIAGMSM_PRIV_EFS_TXT,
  DIAGMSM_PRIV_LOG_TXT,
  DIAGMSM_PRIV_LAST_TXT
};

typedef enum {
  IDIAGMSM_BLACKLIST_READ = 0,
  IDIAGMSM_BLACKLIST_WRITE
} BlackListType;

const char * DiagMSM_File_ReadPathBlackList[] = {
  IDIAGMSM_FILE_BADPATH_NV,
  IDIAGMSM_FILE_BADPATH_NV_B,
  IDIAGMSM_FILE_BADPATH_SYSTEM,
  IDIAGMSM_FILE_BADPATH_SYSTEM_B,
  IDIAGMSM_FILE_BADPATH_SYS,
  IDIAGMSM_FILE_BADPATH_SYS_B,
  IDIAGMSM_FILE_BADPATH_DMU,
  IDIAGMSM_FILE_BADPATH_DMU_B
};

const char * DiagMSM_File_WritePathBlackList[] = {
  IDIAGMSM_FILE_BADPATH_NV,
  IDIAGMSM_FILE_BADPATH_NV_B,
  IDIAGMSM_FILE_BADPATH_SYSTEM,
  IDIAGMSM_FILE_BADPATH_SYSTEM_B,
  IDIAGMSM_FILE_BADPATH_SYS,
  IDIAGMSM_FILE_BADPATH_SYS_B,
  IDIAGMSM_FILE_BADPATH_DMU,
  IDIAGMSM_FILE_BADPATH_DMU_B,
  IDIAGMSM_FILE_BADPATH_BREW_SYS,
  IDIAGMSM_FILE_BADPATH_BREW_SYS_B

};


#define IDIAGMSM_READBLACKLIST_LENGTH   \
  (sizeof(DiagMSM_File_ReadPathBlackList)/sizeof(DiagMSM_File_ReadPathBlackList[0]))

#define IDIAGMSM_WRITEBLACKLIST_LENGTH   \
  (sizeof(DiagMSM_File_WritePathBlackList)/sizeof(DiagMSM_File_WritePathBlackList[0]))

/*-------------------------------------------------------------------
Feature mask definitions:
  The feature bit mask buffer has the following structure:
  
    byte 0   byte 1  ....  byte n
  |76543210|76543210|....|76543210|

   11111111 11111111 .... 00000011  

  - A bit value of 1 specifies an IDiagMSM feature is supported.  A bit
    value of 0 denotes a feature not supported or not yet defined.    
  - IDiagMSM features will be mapped in the following order:
    - Bit 0 of byte 0 represents earliest feature supported.
    - Bit 1 of byte n represents latest feature supported.
  - The following algorithms specify how to look up supported features 
    (in c syntax) using the feature bit mask buffer returned by this API
    function:
      
        // Find the bit for IDIAGMSM_FEATURE_XX within the feature_bit_mask
        // array.
        array_index = IDIAGMSM_FEATURE_XX/8; 
        byte_mask  = 0x01 << (IDIAGMSM_FEATURE_XX%8);
        
        // Check if IDIAGMSM_FEATURE_XX is supported.
        if (feature_bit_mask[array_index] & byte_mask)
          feature IDIAGMSM_FEATURE_XX supported
        else
          feature IDIAGMSM_FEATURED_XX not supported

-------------------------------------------------------------------*/
#define IDIAGMSM_SET_BIT_MASK(xx_item)  (0x01 << (xx_item % 8))

static const byte gszFeatBufMask[] = {
  // byte 0
  (IDIAGMSM_SET_BIT_MASK(IDIAGMSM_FEATURE_SEC) |
   IDIAGMSM_SET_BIT_MASK(IDIAGMSM_FEATURE_EFS) |
   IDIAGMSM_SET_BIT_MASK(IDIAGMSM_FEATURE_VER))

};
                   
/*-------------------------------------------------------------------
Function Prototypes
-------------------------------------------------------------------*/
static uint32 DiagMSM_AddRef (DiagMSM *pMe);
static uint32 DiagMSM_Release (DiagMSM *pMe);
static int DiagMSM_QueryInterface(IDiagMSM *me, AEECLSID idReq, void **ppo);
static uint32 DiagMSM_SendReq (DiagMSM *pMe, IDiagMSMReqType *pReq, 
                               void *pUserData);
static uint32 DiagMSM_QueryRsp (DiagMSM *pMe, PFNIDIAGMSM_DLV_RSP pfnPktDeliver, 
                                void *pUserData);

static uint32 DiagMSM_InitListener (DiagMSM *pMe, 
                                    IDiagMSMListenerType *pListener);
static IDiagMSMLHandleType DiagMSM_AddListener (DiagMSM *pMe, 
                                            IDiagMSMListenerType *pListener);
static uint32 DiagMSM_TraverseListeners (IDiagMSM *pDiagMSM, 
                                         PFNIDIAGMSM_LCB pfnDeliver, void *pUD);
static uint32 DiagMSM_RemoveListener (DiagMSM *pMe,IDiagMSMLHandleType mHandle);
   
static uint32 DiagMSM_ServiceListener (IDiagMSM *pDiagMSM, 
                                       IDiagMSMLHandleType mHandle, 
                                       PFNIDIAGMSM_LSVC pfnSvc, 
                                       void *pUd);
static uint32 DiagMSM_ServiceOFlowBuf (IDiagMSM *pDiagMSM, 
                                       IDiagMSMLHandleType mHandle, 
                                       PFNIDIAGMSM_LSVC pfnSvc, 
                                       void *pUd);
static IDiagMSMRecordType * DiagMSM_RecAlloc (IDiagMSM *pDiagMSM, 
                                              IDiagMSMLHandleType mHandle, 
                                              uint32 dwSize);
static uint32 DiagMSM_RecCommit (IDiagMSM *pDiagMSM, 
                                 IDiagMSMLHandleType mHandle, 
                                 IDiagMSMRecordType *pRec);
static uint32 DiagMSM_RecShorten (IDiagMSM *pDiagMSM, 
                                  IDiagMSMLHandleType mHandle, 
                                  IDiagMSMRecordType *pRec, 
                                  uint32 dwNewSize);

static uint32 DiagMSM_GetListenerInfo (IDiagMSM *pDiagMSM, 
                                       IDiagMSMLHandleType mHandle, 
                                       IDiagMSMListenerType *pCfg);
static int32 DiagMSM_BufUsage (IDiagMSM *pDiagMSM, 
                               IDiagMSMLHandleType mHandle);
static uint32 DiagMSM_GetOFlowOpt (IDiagMSM *pDiagMSM, 
                                   IDiagMSMLHandleType mHandle, 
                                   uint8 *pOpt);
static uint32 DiagMSM_GetLThreshold( IDiagMSM *pDiagMSM, 
                                     IDiagMSMLHandleType mHandle, 
                                     int32 *pThreshold);
static uint32 DiagMSM_GetLimit (IDiagMSM *pDiagMSM, 
                                IDiagMSMLHandleType mHandle, 
                                int32 *pLimit);
static uint32 DiagMSM_GetSize (IDiagMSM *pDiagMSM, 
                               IDiagMSMLHandleType mHandle, 
                               uint32 *pSize);
static uint32 DiagMSM_SetOFlowOpt (IDiagMSM *pDiagMSM, 
                                   IDiagMSMLHandleType mHandle, 
                                   uint8 eOpt);
static uint32 DiagMSM_SetLThreshold( IDiagMSM *pDiagMSM, 
                                     IDiagMSMLHandleType mHandle, 
                                     int32 dwThreshold);
static uint32 DiagMSM_SetLimit (IDiagMSM *pDiagMSM, 
                                IDiagMSMLHandleType mHandle, 
                                int32 dwLimit);      
static uint32 DiagMSM_CreateHMAC (DiagMSM *pMe, byte *pMsg, uint16 nMsgLen,
                                  byte *pKey, uint16 nKeyLen, byte *pDigest,
                                  int *nDigestSz);
static boolean DiagMSM_Verify(IDiagMSM *pMe, const void *pCert, int nCertSz, 
                              AEECallback *pcb, uint32 *poutRes, uint16 wRand, 
                              uint8 bPrivLevel);
static uint16  DiagMSM_Unlock(IDiagMSM *pMe, uint32 dwRand);
static uint16  DiagMSM_VerifyCert (IDiagMSM *pMe, const void *pCert, 
                                   int nCertSz);
static void    DiagMSM_HandleVerifyCB (void *pD);
static void    DiagMSM_ReleaseVerifyTCB (void *pUD);
static uint32  DiagMSM_GetNonce (IDiagMSM *pMe);
static uint32  DiagMSM_GetVersion(IDiagMSM *pDiagMSM, 
                                  AECHAR *pBuf, uint32 dwBufLen);
static const byte *DiagMSM_GetFeatures(IDiagMSM *pDiagMSM, 
                                       uint32 *pBufLen);

//-----------------------------------------------------------------------------
// IDiagMSM EFS access methods
//-----------------------------------------------------------------------------
static IDiagMSMFile DiagMSM_File_Open(IDiagMSM *pDiagMSM, 
                                      const char * pPath, 
                                      int oFlag, uint16 nMode);
static uint32 DiagMSM_File_Close(IDiagMSM *pDiagMSM, IDiagMSMFile handle);
static int32 DiagMSM_File_Read(IDiagMSM *pDiagMSM, 
                               IDiagMSMFile handle, 
                               void * pBuf, 
                               int32 nBufSz);
static int32 DiagMSM_File_Seek(IDiagMSM *pDiagMSM, 
                               IDiagMSMFile handle, int32 nOffset,
                               int nWhence); 
static uint32 DiagMSM_File_Truncate(IDiagMSM *pDiagMSM, 
                                    const char * pPath, 
                                    int32 nOffset);
static int32 DiagMSM_File_Write(IDiagMSM *pDiagMSM, 
                                IDiagMSMFile handle, 
                                const void * pBuf,
                                int32 nBufSz);
static uint32 DiagMSM_File_MkDir(IDiagMSM *pDiagMSM, 
                                 const char * pPath, 
                                 uint16 nMode);
static uint32 DiagMSM_File_Rename(IDiagMSM *pDiagMSM, 
                                  const char * pOldPath, 
                                  const char * pNewPath);
static uint32 DiagMSM_File_RmDir(IDiagMSM *pDiagMSM, const char * pPath);
static uint32 DiagMSM_File_Stat(IDiagMSM *pDiagMSM, 
                                const char *pPath, 
                                IDiagMSMFileStat * pBuf); 
static uint32 DiagMSM_File_Remove(IDiagMSM *pDiagMSM, const char *pPath);
static IDiagMSM_EFSDIR * DiagMSM_File_OpenDir(IDiagMSM *pDiagMSM, 
                                              const char *pPath);
static IDiagMSMDirEnt * DiagMSM_File_ReadDir(IDiagMSM *pDiagMSM, 
                                             const IDiagMSM_EFSDIR *pDir);
static uint32 DiagMSM_File_CloseDir(IDiagMSM *pDiagMSM, 
                                    const IDiagMSM_EFSDIR *pDir);
static int DiagMSM_File_GetLastError(IDiagMSM *pDiagMSM);
static boolean DiagMSM_File_ValidatePath(IDiagMSM *pDiagMSM, const char *pPath, 
                                         BlackListType nBlackList); 

//-----------------------------------------------------------------------------
// Locked function prototypes
// These prototypes correspond to the functions declared above and are 
// used when the interface has to be locked by default
//-----------------------------------------------------------------------------
#if (MIN_BREW_VERSION(3,0) && defined(FEATURE_BNMAPP_STRIPPED))
static uint32 DiagMSM_SendReq_Locked (DiagMSM *pMe, IDiagMSMReqType *pReq, 
                                      void *pUserData);
static uint32 DiagMSM_QueryRsp_Locked (DiagMSM *pMe, 
                                       PFNIDIAGMSM_DLV_RSP pfnPktDeliver, 
                                       void *pUserData);

static uint32 DiagMSM_InitListener_Locked (DiagMSM *pMe, 
                                           IDiagMSMListenerType *pListener);
static IDiagMSMLHandleType DiagMSM_AddListener_Locked (DiagMSM *pMe, 
                                            IDiagMSMListenerType *pListener);
static uint32 DiagMSM_TraverseListeners_Locked (IDiagMSM *pDiagMSM, 
                                                PFNIDIAGMSM_LCB pfnDeliver, 
                                                void *pUD);
static uint32 DiagMSM_RemoveListener_Locked (DiagMSM *pMe,
                                             IDiagMSMLHandleType mHandle);
   
static uint32 DiagMSM_ServiceListener_Locked (IDiagMSM *pDiagMSM, 
                                              IDiagMSMLHandleType mHandle, 
                                              PFNIDIAGMSM_LSVC pfnSvc, 
                                              void *pUd);
static uint32 DiagMSM_ServiceOFlowBuf_Locked (IDiagMSM *pDiagMSM, 
                                              IDiagMSMLHandleType mHandle, 
                                              PFNIDIAGMSM_LSVC pfnSvc, 
                                              void *pUd);
static IDiagMSMRecordType * DiagMSM_RecAlloc_Locked (IDiagMSM *pDiagMSM, 
                                                    IDiagMSMLHandleType mHandle, 
                                                    uint32 dwSize);
static uint32 DiagMSM_RecCommit_Locked (IDiagMSM *pDiagMSM, 
                                        IDiagMSMLHandleType mHandle, 
                                        IDiagMSMRecordType *pRec);
static uint32 DiagMSM_RecShorten_Locked (IDiagMSM *pDiagMSM, 
                                         IDiagMSMLHandleType mHandle, 
                                         IDiagMSMRecordType *pRec, 
                                         uint32 dwNewSize);

static uint32 DiagMSM_GetListenerInfo_Locked (IDiagMSM *pDiagMSM, 
                                              IDiagMSMLHandleType mHandle, 
                                              IDiagMSMListenerType *pCfg);
static int32 DiagMSM_BufUsage_Locked (IDiagMSM *pDiagMSM, 
                                      IDiagMSMLHandleType mHandle);
static uint32 DiagMSM_GetOFlowOpt_Locked (IDiagMSM *pDiagMSM, 
                                          IDiagMSMLHandleType mHandle, 
                                          uint8 *pOpt);
static uint32 DiagMSM_GetLThreshold_Locked ( IDiagMSM *pDiagMSM, 
                                             IDiagMSMLHandleType mHandle, 
                                             int32 *pThreshold);
static uint32 DiagMSM_GetLimit_Locked (IDiagMSM *pDiagMSM, 
                                       IDiagMSMLHandleType mHandle, 
                                       int32 *pLimit);
static uint32 DiagMSM_GetSize_Locked (IDiagMSM *pDiagMSM, 
                                      IDiagMSMLHandleType mHandle, 
                                      uint32 *pSize);
static uint32 DiagMSM_SetOFlowOpt_Locked (IDiagMSM *pDiagMSM, 
                                          IDiagMSMLHandleType mHandle, 
                                          uint8 eOpt);
static uint32 DiagMSM_SetLThreshold_Locked (IDiagMSM *pDiagMSM, 
                                            IDiagMSMLHandleType mHandle, 
                                            int32 dwThreshold);
static uint32 DiagMSM_SetLimit_Locked (IDiagMSM *pDiagMSM, 
                                       IDiagMSMLHandleType mHandle, 
                                       int32 dwLimit);
static uint32 DiagMSM_CreateHMAC_Locked (DiagMSM *pMe, byte *pMsg, 
                                         uint16 nMsgLen, byte *pKey, 
                                         uint16 nKeyLen, byte *pDigest, 
                                         int *nDigestSz);
static IDiagMSMFile DiagMSM_File_Open_Locked (IDiagMSM *pDiagMSM, 
                                              const char * pPath, 
                                              int oFlag, uint16 nMode);
static uint32 DiagMSM_File_Close_Locked (IDiagMSM *pDiagMSM, 
                                         IDiagMSMFile handle);
static int32 DiagMSM_File_Read_Locked (IDiagMSM *pDiagMSM, 
                                       IDiagMSMFile handle, 
                                       void * pBuf, 
                                       int32 nBufSz);
static int32 DiagMSM_File_Seek_Locked (IDiagMSM *pDiagMSM, 
                                       IDiagMSMFile handle, int32 nOffset,
                                       int nWhence); 
static uint32 DiagMSM_File_Truncate_Locked (IDiagMSM *pDiagMSM, 
                                            const char * pPath, 
                                            int32 nOffset);
static int32 DiagMSM_File_Write_Locked (IDiagMSM *pDiagMSM, 
                                        IDiagMSMFile handle, 
                                        const void * pBuf,
                                        int32 nBufSz);
static uint32 DiagMSM_File_MkDir_Locked (IDiagMSM *pDiagMSM, 
                                         const char * pPath, 
                                         uint16 nMode);
static uint32 DiagMSM_File_Rename_Locked (IDiagMSM *pDiagMSM, 
                                          const char * pOldPath, 
                                          const char * pNewPath);
static uint32 DiagMSM_File_RmDir_Locked (IDiagMSM *pDiagMSM, 
                                         const char * pPath);
static uint32 DiagMSM_File_Stat_Locked (IDiagMSM *pDiagMSM, 
                                        const char *pPath, 
                                        IDiagMSMFileStat * pBuf); 
static uint32 DiagMSM_File_Remove_Locked (IDiagMSM *pDiagMSM, 
                                          const char *pPath);
static IDiagMSM_EFSDIR * DiagMSM_File_OpenDir_Locked(IDiagMSM *pDiagMSM, 
                                                     const char *pPath);
static IDiagMSMDirEnt * DiagMSM_File_ReadDir_Locked(IDiagMSM *pDiagMSM, 
                                                   const IDiagMSM_EFSDIR *pDir);
static uint32 DiagMSM_File_CloseDir_Locked(IDiagMSM *pDiagMSM, 
                                           const IDiagMSM_EFSDIR *pDir);
static int DiagMSM_File_GetLastError_Locked(IDiagMSM *pDiagMSM);
#endif

//Internal (non-API) prototypes
#if !defined (AEE_STATIC)
static
#endif
int AEE01013fd6_New (IShell * pIShell, AEECLSID clsid, void ** ppif);
static void DiagMSM_Delete (DiagMSM *pMe);
static void OEMDiagMSM_InitThreshold (IDiagMSMThresholdType *pThresh, 
                                      boolean bActive);
static boolean OEMDiagMSM_ServiceThreshold (DiagMSM *pMe, 
                                            IDiagMSMThresholdType *pThresh, 
                                            int32 dwCriteria);
static void OEMDiagMSM_LogListener (uint32 dwSeqNum, 
                                    const byte *pLog, 
                                    unsigned int dwLength, 
                                    void *pParam);
static void OEMDiagMSM_EventListener (uint32 dwSeqNum, 
                                      const diag_event_type * pEvent, 
                                      void *pParam);

#if defined (AEE_STATIC)
/*lint -esym(714, gvtIDiagMSM) */
IDiagMSMVtbl gvtIDiagMSM = {
   DiagMSM_AddRef,
   DiagMSM_Release,
   DiagMSM_QueryInterface,
   DiagMSM_SendReq,
   DiagMSM_QueryRsp,
   DiagMSM_InitListener,
   DiagMSM_AddListener,
   DiagMSM_TraverseListeners,
   DiagMSM_RemoveListener,
   DiagMSM_ServiceListener,
   DiagMSM_ServiceOFlowBuf,
   DiagMSM_RecAlloc,
   DiagMSM_RecCommit,
   DiagMSM_RecShorten,
   DiagMSM_GetListenerInfo,
   DiagMSM_BufUsage,
   DiagMSM_GetOFlowOpt,
   DiagMSM_GetLThreshold,
   DiagMSM_GetLimit,
   DiagMSM_GetSize,
   DiagMSM_SetOFlowOpt,
   DiagMSM_SetLThreshold,
   DiagMSM_SetLimit,
   DiagMSM_CreateHMAC,
   DiagMSM_Verify,
   DiagMSM_Unlock,
   DiagMSM_File_Open,
   DiagMSM_File_Close,
   DiagMSM_File_Read,
   DiagMSM_File_Seek,
   DiagMSM_File_Truncate,
   DiagMSM_File_Write,
   DiagMSM_File_MkDir,
   DiagMSM_File_Rename,
   DiagMSM_File_RmDir,
   DiagMSM_File_Stat,
   DiagMSM_File_Remove,
   DiagMSM_File_OpenDir,
   DiagMSM_File_ReadDir,
   DiagMSM_File_CloseDir,
   DiagMSM_File_GetLastError, 
   DiagMSM_GetVersion,
   DiagMSM_GetFeatures
};
#endif


/*==============================================================================
FUNCTION DEFINITIONS
==============================================================================*/
/*===========================================================================

FUNCTION: AEE01013fd6_New

DESCRIPTION
  Instantiates an instance of IDiagMSM.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
/*lint -esym(714,AEE01013fd6_New) AEE01013fd6_New is not referenced */
#if !defined (AEE_STATIC)
static
#endif
int AEE01013fd6_New (IShell * pIShell, AEECLSID clsid, void ** ppif)
{
  DiagMSM           *pMe = NULL;
  AEEVTBL(IDiagMSM) *pModMethods = NULL;
#ifdef OEMDIAGMSM_DUMPLOG
  IFileMgr      * pIFileMgr = NULL;
  uint32        nWriteSize = 0;
#endif

  if (ppif == NULL || AEECLSID_IDIAGMSM != clsid)
  {
    return EUNSUPPORTED;
  }
  *ppif = 0;

#if MIN_BREW_VERSION(3,0)
  if ( !ISHELL_CheckPrivLevel(pIShell,AEECLSID_IDIAGMSM_PRIV,TRUE) ) {
	  return EPRIVLEVEL; 
  }
#endif

 
  pMe = (DiagMSM *) MALLOC (sizeof(DiagMSM) + sizeof(VTBL(IDiagMSM)));
  
  if (pMe == NULL)
  {
    return ENOMEMORY;
  }

#if MIN_BREW_VERSION(3,0)  
  // Point the Mod functions to the memory location
  pModMethods = (VTBL(IDiagMSM) *)(void *)((byte *)pMe + sizeof(DiagMSM));
  pModMethods->AddRef = DiagMSM_AddRef;
  pModMethods->Release =  DiagMSM_Release;
  pModMethods->QueryInterface = DiagMSM_QueryInterface;     
  pModMethods->Verify = DiagMSM_Verify;
  pModMethods->Unlock = DiagMSM_Unlock;
  pModMethods->GetVersion = DiagMSM_GetVersion;
  pModMethods->GetFeatures = DiagMSM_GetFeatures;



#if defined(FEATURE_BNMAPP_STRIPPED)
    pModMethods->SendReq = DiagMSM_SendReq_Locked;
    pModMethods->QueryRsp = DiagMSM_QueryRsp_Locked;
    pModMethods->InitListener = DiagMSM_InitListener_Locked;
    pModMethods->AddListener = DiagMSM_AddListener_Locked;
    pModMethods->TraverseListeners = DiagMSM_TraverseListeners_Locked;
    pModMethods->RemoveListener = DiagMSM_RemoveListener_Locked;
    pModMethods->ServiceListener = DiagMSM_ServiceListener_Locked;
    pModMethods->ServiceOFlowBuf = DiagMSM_ServiceOFlowBuf_Locked;
    pModMethods->RecAlloc = DiagMSM_RecAlloc_Locked;
    pModMethods->RecCommit = DiagMSM_RecCommit_Locked;
    pModMethods->RecShorten = DiagMSM_RecShorten_Locked;
    pModMethods->GetListenerInfo = DiagMSM_GetListenerInfo_Locked;
    pModMethods->BufUsage = DiagMSM_BufUsage_Locked;
    pModMethods->GetOFlowOpt = DiagMSM_GetOFlowOpt_Locked;
    pModMethods->GetLThreshold = DiagMSM_GetLThreshold_Locked;
    pModMethods->GetLimit = DiagMSM_GetLimit_Locked;
    pModMethods->GetSize = DiagMSM_GetSize_Locked;
    pModMethods->SetOFlowOpt = DiagMSM_SetOFlowOpt_Locked;
    pModMethods->SetLThreshold = DiagMSM_SetLThreshold_Locked;
    pModMethods->SetLimit = DiagMSM_SetLimit_Locked;
    pModMethods->CreateHMAC = DiagMSM_CreateHMAC_Locked;
    pModMethods->File_Open = DiagMSM_File_Open_Locked;
    pModMethods->File_Close = DiagMSM_File_Close_Locked;
    pModMethods->File_Read = DiagMSM_File_Read_Locked;
    pModMethods->File_Seek = DiagMSM_File_Seek_Locked;
    pModMethods->File_Truncate = DiagMSM_File_Truncate_Locked;
    pModMethods->File_Write = DiagMSM_File_Write_Locked;
    pModMethods->File_MkDir = DiagMSM_File_MkDir_Locked;
    pModMethods->File_Rename = DiagMSM_File_Rename_Locked;
    pModMethods->File_RmDir = DiagMSM_File_RmDir_Locked;
    pModMethods->File_Stat = DiagMSM_File_Stat_Locked;
    pModMethods->File_Remove = DiagMSM_File_Remove_Locked;
    pModMethods->File_OpenDir = DiagMSM_File_OpenDir_Locked;
    pModMethods->File_ReadDir = DiagMSM_File_ReadDir_Locked;
    pModMethods->File_CloseDir = DiagMSM_File_CloseDir_Locked;
    pModMethods->File_GetLastError = DiagMSM_File_GetLastError_Locked;
#else
    pModMethods->SendReq = DiagMSM_SendReq;
    pModMethods->QueryRsp = DiagMSM_QueryRsp;
    pModMethods->InitListener = DiagMSM_InitListener;
    pModMethods->AddListener = DiagMSM_AddListener;
    pModMethods->TraverseListeners = DiagMSM_TraverseListeners;
    pModMethods->RemoveListener = DiagMSM_RemoveListener;
    pModMethods->ServiceListener = DiagMSM_ServiceListener;
    pModMethods->ServiceOFlowBuf = DiagMSM_ServiceOFlowBuf;
    pModMethods->RecAlloc = DiagMSM_RecAlloc;
    pModMethods->RecCommit = DiagMSM_RecCommit;
    pModMethods->RecShorten = DiagMSM_RecShorten;
    pModMethods->GetListenerInfo = DiagMSM_GetListenerInfo;
    pModMethods->BufUsage = DiagMSM_BufUsage;
    pModMethods->GetOFlowOpt = DiagMSM_GetOFlowOpt;
    pModMethods->GetLThreshold = DiagMSM_GetLThreshold;
    pModMethods->GetLimit = DiagMSM_GetLimit;
    pModMethods->GetSize = DiagMSM_GetSize;
    pModMethods->SetOFlowOpt = DiagMSM_SetOFlowOpt;
    pModMethods->SetLThreshold = DiagMSM_SetLThreshold;
    pModMethods->SetLimit = DiagMSM_SetLimit;
    pModMethods->CreateHMAC = DiagMSM_CreateHMAC;
    pModMethods->File_Open = DiagMSM_File_Open;
    pModMethods->File_Close = DiagMSM_File_Close;
    pModMethods->File_Read = DiagMSM_File_Read;
    pModMethods->File_Seek = DiagMSM_File_Seek;
    pModMethods->File_Truncate = DiagMSM_File_Truncate;
    pModMethods->File_Write = DiagMSM_File_Write;
    pModMethods->File_MkDir = DiagMSM_File_MkDir;
    pModMethods->File_Rename = DiagMSM_File_Rename;
    pModMethods->File_RmDir = DiagMSM_File_RmDir;
    pModMethods->File_Stat = DiagMSM_File_Stat;
    pModMethods->File_Remove = DiagMSM_File_Remove;
    pModMethods->File_OpenDir = DiagMSM_File_OpenDir;
    pModMethods->File_ReadDir = DiagMSM_File_ReadDir;
    pModMethods->File_CloseDir = DiagMSM_File_CloseDir;
    pModMethods->File_GetLastError = DiagMSM_File_GetLastError;
#endif // FEATURE_BNMAPP_STRIPPED
#else 
  pModMethods = (VTBL(IDiagMSM) *)(void *)((byte *)pMe + sizeof(DiagMSM));
  pModMethods->AddRef = DiagMSM_AddRef;
  pModMethods->Release =  DiagMSM_Release;
  pModMethods->QueryInterface = DiagMSM_QueryInterface;
  pModMethods->Unlock = DiagMSM_Unlock; 
  pModMethods->Verify = DiagMSM_Verify;
  pModMethods->SendReq = DiagMSM_SendReq;
  pModMethods->QueryRsp = DiagMSM_QueryRsp;
  pModMethods->InitListener = DiagMSM_InitListener;
  pModMethods->AddListener = DiagMSM_AddListener;
  pModMethods->TraverseListeners = DiagMSM_TraverseListeners;
  pModMethods->RemoveListener = DiagMSM_RemoveListener;
  pModMethods->ServiceListener = DiagMSM_ServiceListener;
  pModMethods->ServiceOFlowBuf = DiagMSM_ServiceOFlowBuf;
  pModMethods->RecAlloc = DiagMSM_RecAlloc;
  pModMethods->RecCommit = DiagMSM_RecCommit;
  pModMethods->RecShorten = DiagMSM_RecShorten;
  pModMethods->GetListenerInfo = DiagMSM_GetListenerInfo;
  pModMethods->BufUsage = DiagMSM_BufUsage;
  pModMethods->GetOFlowOpt = DiagMSM_GetOFlowOpt;
  pModMethods->GetLThreshold = DiagMSM_GetLThreshold;
  pModMethods->GetLimit = DiagMSM_GetLimit;
  pModMethods->GetSize = DiagMSM_GetSize;
  pModMethods->SetOFlowOpt = DiagMSM_SetOFlowOpt;
  pModMethods->SetLThreshold = DiagMSM_SetLThreshold;
  pModMethods->SetLimit = DiagMSM_SetLimit; 
  pModMethods->CreateHMAC = DiagMSM_CreateHMAC; 
  pModMethods->File_Open = DiagMSM_File_Open;
  pModMethods->File_Close = DiagMSM_File_Close;
  pModMethods->File_Read = DiagMSM_File_Read;
  pModMethods->File_Seek = DiagMSM_File_Seek;
  pModMethods->File_Truncate = DiagMSM_File_Truncate;
  pModMethods->File_Write = DiagMSM_File_Write;
  pModMethods->File_MkDir = DiagMSM_File_MkDir;
  pModMethods->File_Rename = DiagMSM_File_Rename;
  pModMethods->File_RmDir = DiagMSM_File_RmDir;
  pModMethods->File_Stat = DiagMSM_File_Stat;
  pModMethods->File_Remove = DiagMSM_File_Remove;
  pModMethods->File_OpenDir = DiagMSM_File_OpenDir;
  pModMethods->File_ReadDir = DiagMSM_File_ReadDir;
  pModMethods->File_CloseDir = DiagMSM_File_CloseDir;
  pModMethods->File_GetLastError = DiagMSM_File_GetLastError;
  pModMethods->GetVersion = DiagMSM_GetVersion;
  pModMethods->GetFeatures = DiagMSM_GetFeatures;
#endif // MIN_BREW_VERSION(3,0)    
  
  INIT_VTBL(pMe, IDiagMSM, *pModMethods);
//TODO  pMe->pvt = &gvtIDiagMSM;
  
  pMe->m_nRefs = 1;
  pMe->p_mIShell = pIShell;
  pMe->m_nLastFileError = SUCCESS;
  pMe->m_pVerifyCB = NULL;
  pMe->m_bIsUnlocked = FALSE;
  pMe->m_dwNonce32   = 0;
  pMe->m_bPrivLevel = (uint8)DIAGMSM_PRIV_LAST;

  (void)ISHELL_AddRef(pIShell);
  
  *ppif = (void *)pMe;

   // IDiagMSM Specific stuff   
  (void)q_init (&pMe->mListenerQ);
  (void)q_init (&pMe->mPendingRspQ);
  (void)q_init (&pMe->mRspQ);
   
  // Save the app context for use with AEE_ResumeEx()
  pMe->pAc  = AEE_GetAppContext();

#ifdef OEMDIAGMSM_DUMPLOG

  ISHELL_CreateInstance(pMe->p_mIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr);
  if ( pIFileMgr == NULL) {
    return FALSE;
  }
  pMe->m_pIFileLog = IFILEMGR_OpenFile( pIFileMgr, OEMDIAGMSM_DUMPLOG_FILE, 
                                        _OFM_CREATE );
  if ( pMe->m_pIFileLog == NULL ) {
    pMe->m_pIFileLog = IFILEMGR_OpenFile( pIFileMgr, OEMDIAGMSM_DUMPLOG_FILE, 
                                          _OFM_READWRITE );
  }

  pMe->m_pIFileEvent = IFILEMGR_OpenFile( pIFileMgr, OEMDIAGMSM_DUMPEVENT_FILE, 
                                          _OFM_CREATE );
  if ( pMe->m_pIFileEvent == NULL ) {
    pMe->m_pIFileEvent = IFILEMGR_OpenFile( pIFileMgr,OEMDIAGMSM_DUMPEVENT_FILE, 
                                            _OFM_READWRITE );
  }
  IFILEMGR_Release( pIFileMgr );
#endif

  return AEE_SUCCESS;

} // AEE01013fd6_New

/*===========================================================================

FUNCTION: DiagMSM_ListenerRemoveCB

DESCRIPTION
  This routine is used as a callback for listener remove. 

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void
DiagMSM_ListenerRemoveCB 
(
  IDiagMSMLHandleType mHandle, 
  void *pLUd, 
  void *pUd
)
{
  PARAM_NOT_REF(pLUd)
  PARAM_NOT_CONST(pLUd)
  DiagMSM *pMe = (DiagMSM *) pUd;

  if (pMe)
  {
    // Nothing to do with the return value
    (void)DiagMSM_RemoveListener (pMe, mHandle);
  }
}
/*===========================================================================

FUNCTION: DiagMSM_Delete

DESCRIPTION
  This routine frees all resources (memory, hooks to lower layers, etc).

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void
DiagMSM_Delete (DiagMSM *pMe)
{
  //Free the buffer, if allocated
  if (pMe)
  {
    //Release all listeners, if applicable
    (void)DiagMSM_TraverseListeners (pMe, DiagMSM_ListenerRemoveCB, 
                                     (void *) pMe);
      
    (void)ISHELL_Release(pMe->p_mIShell);
#if !defined (AEE_STATIC)
    (void)IMODULE_Release(pMe->p_mIModule);
#endif

    if ( pMe->m_pUnlock != NULL ) {
      if ( pMe->m_pUnlock->m_pX509Chain != NULL ) {
        (void)IX509CHAIN_Release(pMe->m_pUnlock->m_pX509Chain);
        pMe->m_pUnlock->m_pX509Chain = NULL;
      }
      if ( pMe->m_pVerifyCB != NULL ) {
        CALLBACK_Cancel(pMe->m_pVerifyCB);
      }
      
      FREEIF(pMe->m_pUnlock->m_pX509Result);
      FREEIF(pMe->m_pUnlock->m_pWebOpts);
      FREEIF(pMe->m_pUnlock->m_pCallerCB);
    }
    FREEIF(pMe->m_pUnlock);
    FREEIF(pMe->m_pVerifyCB);
    FREE(pMe);
  }
}
#ifndef AEE_STATIC
/*===========================================================================

FUNCTION: AEEClsCreateInstance

DESCRIPTION
    This function is invoked while the app is being loaded. All Modules must 
    provide this 
    function. Ensure to retain the same name and parameters for this function.
    In here, the module must verify the ClassID and then invoke the 
    AEEApplet_New() function
    that has been provided in AEEAppGen.c. 

   After invoking AEEApplet_New(), this function can do app specific 
   initialization. In this
   example, a generic structure is provided so that app developers need not 
   change app specific initialization section every time except 
   for a call to IDisplay_InitAppData(). 
   This is done as follows: InitAppData() is called to initialize AppletData 
   instance. It is app developers responsibility to fill-in app data 
   initialization 
   code of InitAppData(). App developer is also responsible to release memory 
   allocated for data contained in AppletData -- this can be done in 
   IDisplay_FreeAppData().

PROTOTYPE:
   int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,
                            void ** ppObj)

PARAMETERS:
    clsID: [in]: Specifies the ClassID of the applet which is being loaded

    pIShell: [in]: Contains pointer to the IShell object. 

    pIModule: pin]: Contains pointer to the IModule object to the current 
    module to which this app belongs

    ppObj: [out]: On return, *ppObj must point to a valid IApplet structure. 
    Allocation of memory for this structure and initializing the base data 
    members is done by AEEApplet_New().

DEPENDENCIES
  none

RETURN VALUE
  AEE_SUCCESS: If the app needs to be loaded and if AEEApplet_New() invocation 
  was successful
  EFAILED: If the app does not need to be loaded or if errors occurred in 
  AEEApplet_New(). If this function returns FALSE, the app will not be loaded.

SIDE EFFECTS
  none
===========================================================================*/
int
AEEClsCreateInstance (AEECLSID ClsId,IShell * pIShell, IModule * po,
                      void ** ppObj)
{
   DiagMSM     *pMe;

   if ((pIShell == NULL) || (po == NULL) || (ppObj == NULL)) {
     return EFAILED;
   }
    
   *ppObj = NULL;

   if(ClsId == AEECLSID_IDIAGMSM){
      if(AEE01013fd6_New (pIShell, ClsId, ppObj) == AEE_SUCCESS) 
      {

         // Add your code here .....
         pMe = (DiagMSM *)(*ppObj);

         pMe->p_mIModule = po;
         (void)IMODULE_AddRef(po);
         return (AEE_SUCCESS);

      }
   }
   return (EFAILED);
}
#endif /* !AEE_STATIC */
/*=========================================================================*/
static uint32
DiagMSM_AddRef (IDiagMSM * po)
{
  DiagMSM          *pMe = (DiagMSM *) po;

  if (pMe == NULL) {
    return 0;
  }

  return (++pMe->m_nRefs);
    
}
/*=========================================================================*/
static uint32
DiagMSM_CreateHMAC (DiagMSM *pMe, byte *pMsg, uint16 nMsgLen,
                    byte *pKey, uint16 nKeyLen, byte *pDigest, 
                    int *nDigestSz)
{
  PARAM_NOT_CONST(pMe)
  PARAM_NOT_CONST(pKey)

// HMAC is only supported in BREW 3.x
#if MIN_BREW_VERSION(3,0)
  IHash *pIHash;
  uint32 nRet = AEE_SUCCESS;

  if (!pMe || !pMsg || !pKey || !pDigest || !nDigestSz) {
    return EBADPARM;
  }

  // Create an instance of SHA1 HMAC HASH
  nRet = (uint32)ISHELL_CreateInstance(pMe->p_mIShell, AEECLSID_HMAC_SHA1,
                                       (void **)&pIHash);
  if ( nRet != AEE_SUCCESS ) {
    return nRet;
  }

  // Set the key
  nRet = (uint32)IHASH_SetKey( pIHash, pKey, nKeyLen );

  if ( nRet != AEE_SUCCESS ) {
    (void)IHASH_Release(pIHash);
    pIHash = NULL;
    return nRet;
  }

  // Create HMAC signature
  IHASH_Update( pIHash, (void *)pMsg, 
                (int)nMsgLen );

  // Get HMAC results
  nRet = (uint32)IHASH_GetResult( pIHash, pDigest, nDigestSz );

  (void)IHASH_Release(pIHash);
  pIHash = NULL;

  return nRet;
#else 
  return EFAILED;
#endif

}
/*=========================================================================*/
static uint32
DiagMSM_Release (IDiagMSM *po)
{
  DiagMSM          *pMe = (DiagMSM *) po;
  unsigned int m_nRefs;

  if (pMe == NULL) {
    return 0;
  }

  m_nRefs = --pMe->m_nRefs;

  if (m_nRefs == 0)
  {
#ifdef OEMDIAGMSM_DUMPLOG
    (void)IFILE_Release( pMe->m_pIFileLog );
    (void)IFILE_Release( pMe->m_pIFileEvent );
#endif
    DiagMSM_Delete(pMe);

  }

  return (uint32) m_nRefs;
}
/*=========================================================================*/
static 
int DiagMSM_QueryInterface(IDiagMSM *me, AEECLSID idReq, void **ppo)
{
  PARAM_NOT_REF(me)
  if ( (ppo == NULL) || (*ppo == NULL) || (me == NULL) ) {
    return EFAILED;
  }

  if (NULL != IQI_SELF (me,idReq,ppo,AEECLSID_IDIAGMSM))
  {
    (void)IDiagMSM_AddRef (*ppo);
    return AEE_SUCCESS;
  } else if ( (idReq == AEECLSID_IDIAGMSMEX) ||
              (idReq == AEECLSID_IDIAGMSMEFS)||
              (idReq == AEECLSID_IDIAGMSMVER) ) {
    *ppo = (void *)me;
    (void)IDiagMSM_AddRef (*ppo);
    return AEE_SUCCESS;
  }
  else
  {
    return ECLASSNOTSUPPORT;
  }
}
/*=========================================================================*/
static boolean
DiagMSM_Verify (IDiagMSM *pMe, const void *pCert, int nCertSz, 
                AEECallback *pcb, uint32 *poutRes, uint16 wRand, 
                uint8 bPrivLevel)
{
  // Certificate based unlock
  uint16    wRet    = SUCCESS;
  int       nRet    = SUCCESS;

  // Sanity check. DIAGMSM_PRIV_LAST is invalid priv level too!
  if ( (pMe == NULL) || (pCert == NULL) || (nCertSz <= 0) || 
       (pcb == NULL) || (poutRes == NULL) || (wRand == 0) || 
       (bPrivLevel >= (uint8)DIAGMSM_PRIV_LAST) ) {
    (void)MSG (MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
               "DiagMSM_Verify: SC F");  
    return FALSE;
  }

  // Cancel any existing certificate verifications (occurs when unlock is 
  // called too often)
  if ( pMe->m_pVerifyCB != NULL ) {
    CALLBACK_Cancel(pMe->m_pVerifyCB);
    (void)ISHELL_CancelTimer(pMe->p_mIShell, DiagMSM_ReleaseVerifyTCB,
                             (void *)pMe);
    // Notify that unlock has been re-entered for the same instance
    if ( pMe->m_pUnlock != NULL ) {
      if ( pMe->m_pUnlock->m_poutRes != NULL ) {
        *pMe->m_pUnlock->m_poutRes  = EREENTERED;
      }
      if ( pMe->m_pUnlock->m_pCallerCB != NULL ) {
        ISHELL_Resume(pMe->p_mIShell, pMe->m_pUnlock->m_pCallerCB);
      }
    }
    FREEIF(pMe->m_pVerifyCB);
  }
  
  // Create unlock descriptor if none present or re-use old copy if avail.
  if ( pMe->m_pUnlock != NULL ) {
    // Clear existing unlock UD
    if ( pMe->m_pUnlock->m_pX509Result != NULL ) {
      FREEIF(pMe->m_pUnlock->m_pX509Result);
    }
    if ( pMe->m_pUnlock->m_pX509Chain != NULL ) {
      (void)IX509CHAIN_Release(pMe->m_pUnlock->m_pX509Chain);
    }
    if ( pMe->m_pUnlock->m_pWebOpts != NULL ) {
      FREEIF(pMe->m_pUnlock->m_pWebOpts);
    }
  } else {
    pMe->m_pUnlock = (DiagMSM_UnlockUD *)MALLOC(sizeof(DiagMSM_UnlockUD));
    if (pMe->m_pUnlock == NULL) { 
      (void)MSG(MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
                "DiagMSM_Verify: MALLOC F.(1)");
      return FALSE;
    }
  } 
  (void)MEMSET(pMe->m_pUnlock, 0, sizeof(DiagMSM_UnlockUD));

  pMe->m_pUnlock->m_bPrivLevel  = bPrivLevel;
  pMe->m_pUnlock->m_pCallerCB   = pcb;
  pMe->m_pUnlock->m_poutRes     = poutRes;
  pMe->m_pUnlock->m_wNonce16    = wRand;

  wRet = DiagMSM_VerifyCert (pMe, pCert, nCertSz);
  if ( wRet != SUCCESS ) {
    FREEIF(pMe->m_pUnlock);
    (void)MSG(MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_Verify:Verify Cert failure.");
    return FALSE;
  }

  nRet = ISHELL_SetTimer(pMe->p_mIShell, 
                        10000,  // TODO: Make it a constant
                        DiagMSM_ReleaseVerifyTCB,
                        (void *)pMe);
  if ( nRet != SUCCESS ) {
    (void)MSG(MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_Verify:SetTimer F for:DiagMSM_ReleaseVerifyTCB");
  }

  return TRUE;
}


// Builds the cert chain and sets up cert verification
static uint16
DiagMSM_VerifyCert (IDiagMSM * pMe, const void * pCert, int nCertSz)
{
  int               nRet          = 0;
  // override default trust settings: 0=>Apply to all
  // 1. disable host check as this is not an https cert
  // Note: Date checks are enabled at this time. To disable all date checks,
  //       adds opts X509CHAIN_CERT_EXIPRED and X509CHAIN_BAD_DATE
  X509TrustOverride trustOverRide = {0, X509CHAIN_HOST_MISMATCH  | 
                                        X509CHAIN_NO_TARGET_HOST }; 


  // Sanity checks
  if ( (pMe == NULL)   ||
       (pCert == NULL) ||
       (nCertSz <= 0)  ||
       (pMe->p_mIShell == NULL) ) {
    (void)MSG(MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_VerifyCert: S C F");
    return EBADPARM;
  }

  // Check if unlock UD is allocated and valid
  if ( (pMe->m_pUnlock == NULL) ||
       (pMe->m_pUnlock->m_bPrivLevel >= (uint8)DIAGMSM_PRIV_LAST) ||
       (pMe->m_pUnlock->m_pCallerCB == NULL) ||
       (pMe->m_pUnlock->m_poutRes == NULL) ||
       (pMe->m_pUnlock->m_wNonce16 == 0) ) {
    (void)MSG(MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_VerifyCert: F (UnlockUD invalid)");
    return EBADITEM;
  }

  nRet = ISHELL_CreateInstance(pMe->p_mIShell, AEECLSID_X509CHAIN, 
                               (void **)&pMe->m_pUnlock->m_pX509Chain);
  if ( nRet != SUCCESS ) {
    (void)MSG(MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_VerifyCert: IX509Chain creat F");
    return EFAILED;
  }

  // Add Root certificate
  nRet = IX509CHAIN_AddCert(pMe->m_pUnlock->m_pX509Chain, 
                            WEBOPT_X509_ROOT_CERTS, gabCaCert, 
                            DIAGMSM_CACERT_LEN);
  if ( nRet != SUCCESS ) {
    (void)MSG(MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_VerifyCert: IX509Chain Add Cert F (root)");
    (void)IX509CHAIN_Release(pMe->m_pUnlock->m_pX509Chain);
    pMe->m_pUnlock->m_pX509Chain = NULL;
    return EFAILED;
  }
  
  // Add Leaf certificate
  nRet = IX509CHAIN_AddCert(pMe->m_pUnlock->m_pX509Chain, 
                            WEBOPT_X509_LEAF_CERT, pCert, 
                            nCertSz);
  if ( nRet != SUCCESS ) {
    (void)MSG(MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_VerifyCert: IX509Chain Add Cert F (leaf)");
    (void)IX509CHAIN_Release(pMe->m_pUnlock->m_pX509Chain);
    pMe->m_pUnlock->m_pX509Chain = NULL;
    return EFAILED;
  }

  // Allocate space for verification result
  pMe->m_pUnlock->m_pX509Result = 
    (X509TrustResult *)MALLOC(sizeof(X509TrustResult));
  if ( pMe->m_pUnlock->m_pX509Result == NULL ) {
    (void)MSG(MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_VerifyCert:MALLOC F (X509Res)");
    (void)IX509CHAIN_Release(pMe->m_pUnlock->m_pX509Chain);
    pMe->m_pUnlock->m_pX509Chain = NULL;
    return ENOMEMORY;
  }
  (void)MEMSET(pMe->m_pUnlock->m_pX509Result, 0, sizeof(X509TrustResult));

  // Allocate space for Callback
  if ( pMe->m_pVerifyCB != NULL ) {
    CALLBACK_Cancel(pMe->m_pVerifyCB);
  } else {
    pMe->m_pVerifyCB = (AEECallback *)MALLOC(sizeof(AEECallback));
    if ( pMe->m_pVerifyCB == NULL ) {
      (void)MSG(MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
                "DiagMSM_VerifyCert: MALLOC F (pVerifyCB)");
      (void)IX509CHAIN_Release(pMe->m_pUnlock->m_pX509Chain);
      pMe->m_pUnlock->m_pX509Chain = NULL;
      FREEIF(pMe->m_pUnlock->m_pX509Result);
      return ENOMEMORY;
    }
  }
  (void)MEMSET(pMe->m_pVerifyCB, 0, sizeof(AEECallback));
  
  // Add override options and initiate verification
  pMe->m_pUnlock->m_pWebOpts = (WebOpt *) MALLOC(2*sizeof(WebOpt));
  if ( pMe->m_pUnlock->m_pWebOpts == NULL ) {
    (void)MSG(MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_VerifyCert: MALLOC F (pWebOpts)");
    (void)IX509CHAIN_Release(pMe->m_pUnlock->m_pX509Chain);
    pMe->m_pUnlock->m_pX509Chain = NULL;
    FREEIF(pMe->m_pVerifyCB);
    FREEIF(pMe->m_pUnlock->m_pX509Result);
    return ENOMEMORY;
  }
  (void)MEMSET(pMe->m_pUnlock->m_pWebOpts, 0, (2*sizeof(WebOpt)));

  pMe->m_pUnlock->m_pWebOpts->nId  = (int)WEBOPT_X509_OVERRIDE;
  pMe->m_pUnlock->m_pWebOpts->pVal = &trustOverRide;
  (pMe->m_pUnlock->m_pWebOpts+1)->nId = WEBOPT_END;
  
  CALLBACK_Init(pMe->m_pVerifyCB, DiagMSM_HandleVerifyCB, pMe); 
  IX509CHAIN_VerifyV(pMe->m_pUnlock->m_pX509Chain, 
                     pMe->m_pUnlock->m_pX509Result,
                     pMe->m_pVerifyCB,
                     pMe->m_pUnlock->m_pWebOpts);

  return SUCCESS;
}

static void 
DiagMSM_HandleVerifyCB (void *pD) 
{
  PARAM_NOT_CONST (pD)

  IDiagMSM            * pMe         = (IDiagMSM *)pD;
  X509PartRequest     * pPartReq    = NULL;
  X509CertPart        * pCertPart   = NULL;
  AECHAR              * pwSubject   = NULL;
  AEECLSID              dwClsId     = 0;
  DiagMSM_UnlockUD    * pMeUnlock   = NULL;
  char                * pszSubject  = NULL;
  int                   nIdx        = 0;
  int                   nRet        = 0;
  uint32                nStatus     = SUCCESS;

  // Sanity check. We can't really do anything if this fails.
  if ( pMe == NULL ) {
    (void)MSG(MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_HndlVerifyCB: S C F (pMe)");
    return;
  }

   // Cancel ReleaseVerify timer
  (void)ISHELL_CancelTimer(pMe->p_mIShell, 
                           DiagMSM_ReleaseVerifyTCB, 
                           (void *)pMe);

  if ( pMe->m_pUnlock == NULL ) {
    (void)MSG(MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_HndlVerifyCB: Missing UnlockUD");
    FREEIF(pMe->m_pVerifyCB);
    return;
  }
  pMeUnlock = pMe->m_pUnlock;

  // Validate unlock struct and check for verify results
  if ( (pMeUnlock->m_pX509Result == NULL) ||
       (pMeUnlock->m_bPrivLevel  >= (int)DIAGMSM_PRIV_LAST) ) {
    (void)MSG(MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_HndlVerifyCB: Missing Result");
    nStatus = EBADITEM;
  } else if ( (pMeUnlock->m_pX509Result->nResult != SUCCESS) ||
              (pMeUnlock->m_pX509Result->uErrorSummary != 
                X509CHAIN_TRUST_OK) ) {
    // Certificate failed verification checks
    (void)MSG(MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_HndlVerifyCB: Cert trust F");

    nStatus = EPRIVLEVEL;
  } else {
    for (nIdx=0; nIdx < pMeUnlock->m_pX509Result->nChainLen; nIdx++) {
      // The certificates are OK, lets check for any chain errors now
      if ( pMeUnlock->m_pX509Result->auChainErrors[nIdx] != 
             X509CHAIN_TRUST_OK ) {
        // Certificate failed verification checks
        (void)MSG(MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
                  "DiagMSM_HndlVerifyCB: Cert Chain trust F");
        nStatus = EPRIVLEVEL;
        break;
      } // end - if (..X509Result->auChainErrors..)
    } // end - for (nIdx...)
  } // end if-elseif-else


  // Done basic checks. Now check classIds, PrivLevels etc.
  if ( pMeUnlock->m_pX509Chain == NULL ) {
    (void)MSG(MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_HndlVerifyCB: Missing Cert Chain");
    nStatus = EBADITEM;
  }

  if ( nStatus == SUCCESS ) {
    // Sentinel failure value. Success occurs only when all following succeed.
    nStatus = EFAILED;

    pPartReq   = (X509PartRequest *)MALLOC(sizeof(X509PartRequest));
    pCertPart  = (X509CertPart *)MALLOC(sizeof(X509CertPart));
    if ( (pPartReq  == NULL)  ||
         (pCertPart == NULL) ) {
      (void)MSG(MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
                "DiagMSM_HndlVerifyCB: MALLOC F (pPartReq, pCertPart)");
      nStatus = ENOMEMORY;
    } else {
      // Check for PrivLevel requested
      pPartReq->nCertIndex  = 0;
      pPartReq->nCertWebOpt = WEBOPT_X509_LEAF_CERT;
      pPartReq->uCertPart   = X509CHAIN_FIELD_SUBJECT;
      pPartReq->pcRawOID    = NULL;
      pPartReq->nInstance   = 0;
      pPartReq->uASNOID     = ASNOID_X500_CN;
    
      // Get the classid and priv. levels from CN field
      nRet = IX509CHAIN_GetFieldPart(pMeUnlock->m_pX509Chain,
                                     pPartReq,
                                     pCertPart);
      if ( (nRet != SUCCESS) ||
           ((int)((pCertPart->uLength + 1)*sizeof(AECHAR)) <= 0 ) ) {
        (void)MSG(MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
                  "DiagMSM_HndlVerifyCB:EFAILED:(nRet, pCertPart->uLength)");

        nStatus = EFAILED;
      } else {
        pwSubject  = (AECHAR *)MALLOC((pCertPart->uLength + 1)*sizeof(AECHAR));
        pszSubject = (char *)MALLOC((pCertPart->uLength + 1)*sizeof(char));
        if ( (pwSubject == NULL) ||
             (pszSubject ==NULL) ) {
          (void)MSG(MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
                    "DiagMSM_HndlVerifyCB:MALLOC F (pwSubject, pszSubject)");

          nStatus = EFAILED;
        } else {
          // Lint uint to int. Checked in if condition above (nret...)
          /*lint -e713*/
          (void)UTF8TOWSTR(pCertPart->pcValue, pCertPart->uLength, pwSubject, 
                           ((pCertPart->uLength + 1)*sizeof(AECHAR)));
          pwSubject[pCertPart->uLength] = '\0';
          (void)WSTRTOSTR(pwSubject, pszSubject, (int)(pCertPart->uLength+1));
          /*lint +e713*/
          // Check for Priv-Level in the cert
          if ( STRSTR(pszSubject, 
                      gaszDiagMSMPrivLevels[(uint8)pMeUnlock->m_bPrivLevel]) ==
               NULL) {
            // Priv level not found
            nStatus = EPRIVLEVEL;
          } else {
            // Note:10 is base for conversion
            dwClsId = (AEECLSID)STRTOUL(pszSubject, NULL, 16);
            // Cert and priv. verified, now generate random numbers
            pMe->m_dwNonce32 = DiagMSM_GetNonce(pMe);
            if ( pMe->m_dwNonce32 == 0 ) {
              (void)MSG(MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
                        "DiagMSM_HndlVerifyCB:RNG Failure");
              nStatus = EFAILED;              
            } else {
              pMe->m_bIsUnlocked = TRUE;
              pMe->m_bPrivLevel = (uint8)pMeUnlock->m_bPrivLevel;
              (void)ISHELL_SendEvent(pMe->p_mIShell, dwClsId, EVT_IDIAGMSM, 
                                     pMeUnlock->m_wNonce16, pMe->m_dwNonce32);
              
              (void)MSG(MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
                        "DiagMSM_HndlVerifyCB:Verify SUCCESS");
              nStatus = SUCCESS;
            } // if-else (dwNonce32)
          } // if-else Priv-Level(STRSTR..)
        } // if-else (pwSubject, pszSubject)
      } // if-else (nRet)
    } // if-else (pPartReq, pCertPart)
  } // if - nstatus

  if ( pMeUnlock->m_poutRes != NULL ) {
    *(pMeUnlock->m_poutRes) = nStatus;
  }
  if ( pMeUnlock->m_pCallerCB != NULL ) {
    ISHELL_Resume(pMe->p_mIShell, pMeUnlock->m_pCallerCB);
  }

  // Free mem.
  FREEIF(pCertPart);
  FREEIF(pPartReq);
  FREEIF(pszSubject);
  FREEIF(pwSubject);
  if ( pMeUnlock->m_pX509Chain != NULL ) {
    (void)IX509CHAIN_Release(pMeUnlock->m_pX509Chain);
    pMeUnlock->m_pX509Chain = NULL;
  }
  FREEIF(pMeUnlock->m_pX509Result);
  FREEIF(pMeUnlock->m_pWebOpts);
  FREEIF(pMe->m_pVerifyCB);
  FREEIF(pMe->m_pUnlock);
}

static void 
DiagMSM_ReleaseVerifyTCB (void *pUD) 
{
  IDiagMSM *pMe = (IDiagMSM *)pUD;
  // Sanity check.
  if(pMe == NULL) {
    (void)MSG(MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_ReleaseVerifyTCB: S C F");
    return;
  }
  
  // Cancel callback if it hasn't happened and notify caller.
  if(pMe->m_pVerifyCB != NULL) {
    CALLBACK_Cancel(pMe->m_pVerifyCB);
    FREEIF(pMe->m_pVerifyCB);
  }
  if ( pMe->m_pUnlock != NULL ) {
    if((pMe->m_pUnlock->m_poutRes != NULL) &&
       (pMe->m_pUnlock->m_pCallerCB != NULL)) {
      // return invalid operation status
      *(pMe->m_pUnlock->m_poutRes) = EEXPIRED;
      ISHELL_Resume(pMe->p_mIShell, pMe->m_pUnlock->m_pCallerCB);
    }
    // Free userCert as a double check. (It would freed anyway before any 
    if ( pMe->m_pUnlock->m_pX509Chain != NULL ) {
      (void)IX509CHAIN_Release(pMe->m_pUnlock->m_pX509Chain);
      pMe->m_pUnlock->m_pX509Chain = NULL;
    }
    FREEIF(pMe->m_pUnlock->m_pX509Result);
    FREEIF(pMe->m_pUnlock->m_pWebOpts);
  }
  FREEIF(pMe->m_pUnlock);
  FREEIF(pMe->m_pVerifyCB);
} // end - AppLogExt_ReleaseVerifyTCB

static uint16
DiagMSM_Unlock (IDiagMSM *pMe, uint32 dwRand)
{
  AEEVTBL(IDiagMSM) *pModMethods = NULL;

  if ( (pMe == NULL) ||
       (dwRand == 0) ) {
    (void)MSG(MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_FinishUnlock: S C F");
    return EBADPARM;
  }

  // Check if verification was done before calling FinishUnlock
  if ( pMe->m_bIsUnlocked == FALSE ) {
    (void)MSG(MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_FinishUnlock:pMe Unverified.Can't finish Unlock.");
    return EBADITEM;
  } 
  // Reset the flag so FinishUnlock cannot be called again
  pMe->m_bIsUnlocked = FALSE;

  // Check Random number
  if ( pMe->m_dwNonce32 != dwRand ) {
    (void)MSG(MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_FinishUnlock:Rand# mismatch.");
    return EBADITEM;
  }
  // Reset the random number because its no longer needed
  pMe->m_dwNonce32  = 0;

  // Check for valid priv level
  if ( pMe->m_bPrivLevel >= (uint8)DIAGMSM_PRIV_LAST )  {
    (void)MSG(MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_FinishUnlock:Invalid PrivLevel");
    return EPRIVLEVEL;
  }

  // Everything looks ok, lets unlock based on privlevel
  pModMethods = ((DiagMSM *)pMe)->pvt;
  switch (pMe->m_bPrivLevel) {
    case DIAGMSM_PRIV_ALL:
    {
      pModMethods->SendReq = DiagMSM_SendReq;
      pModMethods->QueryRsp = DiagMSM_QueryRsp;
      pModMethods->InitListener = DiagMSM_InitListener;
      pModMethods->AddListener = DiagMSM_AddListener;
      pModMethods->TraverseListeners = DiagMSM_TraverseListeners;
      pModMethods->RemoveListener = DiagMSM_RemoveListener;
      pModMethods->ServiceListener = DiagMSM_ServiceListener;
      pModMethods->ServiceOFlowBuf = DiagMSM_ServiceOFlowBuf;
      pModMethods->RecAlloc = DiagMSM_RecAlloc;
      pModMethods->RecCommit = DiagMSM_RecCommit;
      pModMethods->RecShorten = DiagMSM_RecShorten;
      pModMethods->GetListenerInfo = DiagMSM_GetListenerInfo;
      pModMethods->BufUsage = DiagMSM_BufUsage;
      pModMethods->GetOFlowOpt = DiagMSM_GetOFlowOpt;
      pModMethods->GetLThreshold = DiagMSM_GetLThreshold;
      pModMethods->GetLimit = DiagMSM_GetLimit;
      pModMethods->GetSize = DiagMSM_GetSize;
      pModMethods->SetOFlowOpt = DiagMSM_SetOFlowOpt;
      pModMethods->SetLThreshold = DiagMSM_SetLThreshold;
      pModMethods->SetLimit = DiagMSM_SetLimit;
      pModMethods->CreateHMAC = DiagMSM_CreateHMAC;
      pModMethods->File_Open = DiagMSM_File_Open;
      pModMethods->File_Close = DiagMSM_File_Close;
      pModMethods->File_Read = DiagMSM_File_Read;
      pModMethods->File_Seek = DiagMSM_File_Seek;
      pModMethods->File_Truncate = DiagMSM_File_Truncate;
      pModMethods->File_Write = DiagMSM_File_Write;
      pModMethods->File_MkDir = DiagMSM_File_MkDir;
      pModMethods->File_Rename = DiagMSM_File_Rename;
      pModMethods->File_RmDir = DiagMSM_File_RmDir;
      pModMethods->File_Stat = DiagMSM_File_Stat;
      pModMethods->File_Remove = DiagMSM_File_Remove;
      pModMethods->File_OpenDir = DiagMSM_File_OpenDir;
      pModMethods->File_ReadDir = DiagMSM_File_ReadDir;
      pModMethods->File_CloseDir = DiagMSM_File_CloseDir;
      pModMethods->File_GetLastError = DiagMSM_File_GetLastError;
      break;
    }
    case DIAGMSM_PRIV_LOG:
    {
      pModMethods->SendReq = DiagMSM_SendReq;
      pModMethods->QueryRsp = DiagMSM_QueryRsp;
      pModMethods->InitListener = DiagMSM_InitListener;
      pModMethods->AddListener = DiagMSM_AddListener;
      pModMethods->TraverseListeners = DiagMSM_TraverseListeners;
      pModMethods->RemoveListener = DiagMSM_RemoveListener;
      pModMethods->ServiceListener = DiagMSM_ServiceListener;
      pModMethods->ServiceOFlowBuf = DiagMSM_ServiceOFlowBuf;
      pModMethods->RecAlloc = DiagMSM_RecAlloc;
      pModMethods->RecCommit = DiagMSM_RecCommit;
      pModMethods->RecShorten = DiagMSM_RecShorten;
      pModMethods->GetListenerInfo = DiagMSM_GetListenerInfo;
      pModMethods->BufUsage = DiagMSM_BufUsage;
      pModMethods->GetOFlowOpt = DiagMSM_GetOFlowOpt;
      pModMethods->GetLThreshold = DiagMSM_GetLThreshold;
      pModMethods->GetLimit = DiagMSM_GetLimit;
      pModMethods->GetSize = DiagMSM_GetSize;
      pModMethods->SetOFlowOpt = DiagMSM_SetOFlowOpt;
      pModMethods->SetLThreshold = DiagMSM_SetLThreshold;
      pModMethods->SetLimit = DiagMSM_SetLimit;
      pModMethods->CreateHMAC = DiagMSM_CreateHMAC;
      break;
    }
    case DIAGMSM_PRIV_EFS:
    {
      pModMethods->File_Open = DiagMSM_File_Open;
      pModMethods->File_Close = DiagMSM_File_Close;
      pModMethods->File_Read = DiagMSM_File_Read;
      pModMethods->File_Seek = DiagMSM_File_Seek;
      pModMethods->File_Truncate = DiagMSM_File_Truncate;
      pModMethods->File_Write = DiagMSM_File_Write;
      pModMethods->File_MkDir = DiagMSM_File_MkDir;
      pModMethods->File_Rename = DiagMSM_File_Rename;
      pModMethods->File_RmDir = DiagMSM_File_RmDir;
      pModMethods->File_Stat = DiagMSM_File_Stat;
      pModMethods->File_Remove = DiagMSM_File_Remove;
      pModMethods->File_OpenDir = DiagMSM_File_OpenDir;
      pModMethods->File_ReadDir = DiagMSM_File_ReadDir;
      pModMethods->File_CloseDir = DiagMSM_File_CloseDir;
      pModMethods->File_GetLastError = DiagMSM_File_GetLastError;
      break;
    }
    default:
      return EFAILED;
  }
  return AEE_SUCCESS; 
} // end - DiagMSM_Unlock

static uint32 
DiagMSM_GetNonce (IDiagMSM *pMe) 
{
  PARAM_NOT_CONST(pMe)

  ISource *pRandSrc = NULL;
  uint32   dwRand   = 0;
  int      nRetVal  = 0;
  uint16   nRetries = 0;

  if( (pMe == NULL) ||
      (pMe->p_mIShell == NULL)) {
    (void)MSG(MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
          "DiagMSM_GetNonce: S C F");
    return 0;
  }

  nRetVal = ISHELL_CreateInstance(pMe->p_mIShell, 
                                  AEECLSID_RANDOM,
                                  (void*)&pRandSrc);
  if(nRetVal != SUCCESS) {
    // Use GetRand if ISource creation fails
    // if RNG returns zero, get a different random #.
    while((dwRand == 0) &&
          (nRetries <= DIAGMSM_MAX_NONCE_RETRIES)) {
      GETRAND((byte *)&dwRand, sizeof(uint32));
      nRetries++;
    }
    return dwRand;
  }

  // if RNG returns zero, get a different random #.
  nRetries = 0;
  while((dwRand == 0) &&
        (nRetries <= DIAGMSM_MAX_NONCE_RETRIES)) {
    nRetVal = ISOURCE_Read(pRandSrc, (char *)&dwRand, sizeof(uint32));
    nRetries++;
  }
  (void)ISOURCE_Release(pRandSrc);
  return dwRand;
} // end - DiagMSM_GetNonce
/*=========================================================================*/
static uint32
DiagMSM_InitListener (DiagMSM *pMe, IDiagMSMListenerType *pListener)
{
  PARAM_NOT_CONST(pMe)
  if (pMe && pListener)
  {
    pListener->pUserData = NULL;
    
    pListener->dwSize = 1024;
    pListener->dwId = 0;
    pListener->eType = (uint8)IDIAGMSM_ID_COUNT;
  
    pListener->eOFlowOpt = (uint8)IDIAGMSM_OF_DROP;
    
    OEMDiagMSM_InitThreshold (&pListener->mListenerThresh, FALSE);
    
    pListener->dwLimit = -1;
  
    pListener->dwOFlowBufSize = 0;
    pListener->cOflowStateThreshold = 0;

    return AEE_SUCCESS;
  }

  // HACK:
  // This parameter is changed in IDiagMSM to give applications an ability
  // to differentiate between IDiagMSM that supports Security features and 
  // IDiagMSM that doesn't
  //
  // Codes used in versions:
  // Version     |  Code
  // --------------------
  // no security | EFAILED
  // security    | EBADPARM
  //
  // Application may do the following check to determine the version of IBnmRm:
  // if ( IDiagMSM_InitListener(pMe->pIBnmRm, NULL) == EBADPARM ) {
  //   The version of IDiagMSM with supported security features
  // } else {
  //   The version of IDiagMSM with no security features
  // }
  // In future, this hack should not be used. IDiagMSM_QueryInterface()
  // should be used instead
  return EBADPARM;
}
/*=========================================================================*/
static IDiagMSMLHandleType
DiagMSM_AddListener (DiagMSM *pMe, IDiagMSMListenerType *pListener)
{
  PARAM_NOT_CONST(pListener)
  OEMDiagMSMListenerType *pOListener = NULL;
  uint32 dwSize = 0;
  boolean bRegSuccess = FALSE;
  
  if (pMe && pListener)
  {
    pOListener = (OEMDiagMSMListenerType *) 
      MALLOC (sizeof(OEMDiagMSMListenerType));

    if (pOListener)
    {
      pOListener->p_mDiagMSM = pMe;

#if defined (DEBUG_OEMDIAGMSM)
#error code not present
#endif
      
      (void)MEMCPY (&pOListener->mListenerInfo, pListener, sizeof (*pListener));

      pOListener->mReadyQ.mRbq = NULL;
      pOListener->mOverflowQ.mRbq = NULL;

      //Enforce minimums to be safe
      if (pListener->eType == (uint8)IDIAGMSM_ID_LOG)
      {
        if (pListener->dwSize > (uint32)OEMDIAGMSM_MIN_LOG_BUF_SIZE)
        {
          dwSize = pOListener->mListenerInfo.dwSize;
        }
        else
        {
          dwSize = (uint32)OEMDIAGMSM_MIN_LOG_BUF_SIZE;
          
          pOListener->mListenerInfo.dwSize = dwSize;
        }

      }
      else
      {
        if (pListener->dwSize > (uint32)OEMDIAGMSM_MIN_EVT_BUF_SIZE)
        {
          dwSize = pOListener->mListenerInfo.dwSize;
        }
        else
        {
          dwSize = OEMDIAGMSM_MIN_EVT_BUF_SIZE;
          
          pOListener->mListenerInfo.dwSize = dwSize;
        }
      }
      
      if (pListener->dwOFlowBufSize > 0)
      {
        pOListener->mOverflowQ.mRbq = MALLOC (pListener->dwOFlowBufSize);
        
        if (pOListener->mOverflowQ.mRbq)
        {
          // Compare with 0 to check for an integer overflow
          if ( ((int)pListener->dwOFlowBufSize < 0) ||
               (!rbq_init (pOListener->mOverflowQ.mRbq, 
                         (int)pListener->dwOFlowBufSize)) )
          {
            FREE (pOListener->mOverflowQ.mRbq);
            pOListener->mOverflowQ.mRbq = NULL;
          }
        } else {
          (void)MSG_2 (MSG_SSID_OEMDIAG, OEMDIAG_MSG_CONFIG,
                     "Add Listener fail %d %d.", 
                     pListener->eType, pListener->dwId);
          return FALSE;
        }

      }
      else
      {
        pOListener->mOverflowQ.mRbq = NULL;
      }

      pOListener->mReadyQ.mRbq = (void *)MALLOC ((uint32)dwSize);

      /* Place on the listener Q in the DiagMSM structure to track it */
      q_put (&pMe->mListenerQ, q_link (pOListener, &pOListener->mLink));

      if ( ((int)dwSize >= 0) && pOListener->mReadyQ.mRbq &&
           (rbq_init (pOListener->mReadyQ.mRbq, (int)dwSize) == TRUE) )
      {
        switch (pListener->eType)
        {
          case IDIAGMSM_ID_LOG:
            bRegSuccess = diag_add_log_listener (pListener->dwId, 
                                                 OEMDiagMSM_LogListener,
                                                 (void *) pOListener);
            break;

          case IDIAGMSM_ID_EVENT:
            bRegSuccess = diag_add_event_listener (pListener->dwId, 
                                                   OEMDiagMSM_EventListener,
                                                   (void *) pOListener);
            break;

          default:
            break;
        }
      }

      if (bRegSuccess)
      {
        (void)MSG_2 (MSG_SSID_OEMDIAG, OEMDIAG_MSG_CONFIG,
                     "Add Listener %d %d", pListener->eType, pListener->dwId);
      }
      else
      {
        (void)MSG_2 (MSG_SSID_OEMDIAG, OEMDIAG_MSG_CONFIG,
                     "Add Listener fail %d %d.  Cleaning up", 
                     pListener->eType, pListener->dwId);

        q_delete (&pMe->mListenerQ, &pOListener->mLink);

        if (!pOListener->mReadyQ.mRbq)
        {
          FREE (pOListener->mReadyQ.mRbq);
        }

        if (!pOListener->mOverflowQ.mRbq)
        {
          FREE (pOListener->mOverflowQ.mRbq);
        }

        FREE (pOListener);
        pOListener = NULL;
      }
    }

    return (pOListener) ? (IDiagMSMLHandleType)pOListener:IDIAGMSM_LHANDLE_NULL;
  }
  else
  {
    return IDIAGMSM_LHANDLE_NULL;
  }
}
/*=========================================================================*/
static uint32
DiagMSM_RemoveListener (DiagMSM *pMe, IDiagMSMLHandleType mHandle)
{
  OEMDiagMSMListenerType *pOListener = OEMDIAGMSM_CVT_H2L (mHandle);
  boolean nRet = AEE_SUCCESS;
  
  if ( (pMe) && (pOListener) && (pOListener->p_mDiagMSM == pMe) )
  {
    q_delete (&pMe->mListenerQ, &pOListener->mLink);

    switch (pOListener->mListenerInfo.eType)
    {
      case IDIAGMSM_ID_LOG:
        nRet = diag_remove_log_listener (pOListener->mListenerInfo.dwId, 
                                        OEMDiagMSM_LogListener,
                                        (void *) pOListener);
        break;

      case IDIAGMSM_ID_EVENT:
        nRet = diag_remove_event_listener (pOListener->mListenerInfo.dwId, 
                                          OEMDiagMSM_EventListener,
                                          (void *) pOListener);
        break;

      default:
        break;
    }

    OEMDiagMSM_InitThreshold (&pOListener->mListenerInfo.mListenerThresh, TRUE);

    if (pOListener->mReadyQ.mRbq)
    {
      FREE (pOListener->mReadyQ.mRbq);
    }

    if (pOListener->mOverflowQ.mRbq)
    {
      FREE (pOListener->mOverflowQ.mRbq);
    }
    
    (void)MSG_3 (MSG_SSID_DIAG, OEMDIAG_MSG_STATS | OEMDIAG_MSG_CONFIG, 
                 "Remove Stats ID:0x%X S:%d of %d", 
                 pOListener->mListenerInfo.dwId, 
                 pOListener->dwClobberUnfilterCnt, 
                 pOListener->dwClobberCnt);
    
    FREE (pOListener);

    if (nRet == FALSE) {
      return EBADITEM;
    }

    return AEE_SUCCESS;
  }
  else
  {
    return EBADITEM;
  }
}
/*=========================================================================*/
static uint32
DiagMSM_TraverseListeners(IDiagMSM *pDiagMSM, PFNIDIAGMSM_LCB pfnDeliver, 
                          void *pUd)
{
  DiagMSM *pMe = (DiagMSM *) pDiagMSM;
  OEMDiagMSMListenerType *pOListener = NULL;
  OEMDiagMSMListenerType *pLast = NULL;

  if ( (pMe) && (pfnDeliver) )
  {
    /* To traverse the list, first capture the address of the last listener
       in the list.  Then, dequeue the item and enqueue it to the end of the 
       queue.  Re-queueing the item makes sure it is in the list in case 
       pfn removes the listener. */
    pLast = q_last_check (&pMe->mListenerQ);

    pOListener = (OEMDiagMSMListenerType *) q_get (&pMe->mListenerQ);

    while (pOListener)
    {
      q_put (&pMe->mListenerQ, q_link (pOListener, &pOListener->mLink));

      pfnDeliver ((IDiagMSMLHandleType) pOListener, 
                  pOListener->mListenerInfo.pUserData, pUd);

      pOListener = (pOListener == pLast) ? NULL : 
        (OEMDiagMSMListenerType *) q_get (&pMe->mListenerQ);
    }

    /* If pLast is set, there was at least one item in the queue */
    if (pLast)
    {
      return (pLast) ? AEE_SUCCESS : EUNABLETOLOAD;
    }
  }

  return EFAILED;
}
/*===========================================================================

FUNCTION: 

DESCRIPTION
   

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
/* lint -esym(715, *) */
static void
OEMDiagMSM_HandleRsp (const byte *pRsp, unsigned int dwRspLen, void *pParam)
{
  OEMDiagMSMPktContextType *pPktInfo = (OEMDiagMSMPktContextType *) pParam;
  DiagMSM *pMe = NULL;
  OEMDiagMSMRspType *pRspQItem = NULL;

  if (pPktInfo)
  {
    pMe = pPktInfo->p_mDiagMSM;

    if (pMe)
    {
      q_delete (&pMe->mPendingRspQ, &pPktInfo->mLink);
      
      pRspQItem = (OEMDiagMSMRspType *) 
        MALLOC (dwRspLen + FPOS (OEMDiagMSMRspType, mRsp.mRsp.aPkt));
      
      if (pRspQItem)
      {
        pRspQItem->mRsp.pUserData = pPktInfo->pUserData;
        pRspQItem->mRsp.mRsp.dwLen = dwRspLen;
        memcpy (pRspQItem->mRsp.mRsp.aPkt, pRsp, dwRspLen);
      
        q_put (&pMe->mRspQ, q_link (pRspQItem, &pRspQItem->mLink));
      
        if (!CALLBACK_IsQueued (pPktInfo->pCb))
        {
#if MIN_BREW_VERSION(3,0)
          AEE_ResumeCallback (pPktInfo->pCb, pMe->pAc);
#else
          AEE_ResumeEx (pPktInfo->pCb, 0, pMe->pAc);
#endif
        }
      }
        
      FREE (pPktInfo);
    }
  } else {
    (void)MSG (MSG_SSID_DIAG, OEMDIAG_MSG_NOTIFY,
               "OEMDiagMSM_HandleRsp: SC F"); 
  }

}
/*=========================================================================*/
static uint32
DiagMSM_SendReq (IDiagMSM *pDiagMSM, IDiagMSMReqType *pReq, void *pUserData)
{
  PARAM_NOT_CONST(pReq)
  PARAM_NOT_CONST(pUserData)
  PARAM_NOT_REF(pUserData)
  DiagMSM *pMe = (DiagMSM *) pDiagMSM;
  OEMDiagMSMPktContextType *pPktInfo = NULL;
  
  if (pMe && pReq)
  {
    pPktInfo = MALLOC (sizeof (OEMDiagMSMPktContextType));

    if (pPktInfo)
    {
      pPktInfo->pUserData = pReq->pUserData;
      pPktInfo->p_mDiagMSM = (DiagMSM *) pMe;
      pPktInfo->pCb = pReq->pCb;

      q_put (&pMe->mPendingRspQ, q_link (pPktInfo, &pPktInfo->mLink));
        
      if (diag_cmd_req (pReq->mReq.aPkt, pReq->mReq.dwLen, 
                        OEMDiagMSM_HandleRsp, (void *) pPktInfo) == TRUE)
      {
        return AEE_SUCCESS;
      }
      else
      {
        q_delete (&pMe->mPendingRspQ, &pPktInfo->mLink);
        FREE (pPktInfo);
         
        return EBADPARM;
      }
    }
  }

  return EFAILED;
}
/*=========================================================================*/
static uint32
DiagMSM_QueryRsp (IDiagMSM *pDiagMSM, PFNIDIAGMSM_DLV_RSP pfnPktDeliver, 
                  void *pUd)
{
  DiagMSM *pMe = (DiagMSM *) pDiagMSM;
  OEMDiagMSMRspType *pRspQItem = NULL;
  
  if (pMe)
  {
    pRspQItem = (OEMDiagMSMRspType *) q_get (&pMe->mRspQ);
  
    if (pRspQItem)
    {
      if (pfnPktDeliver)
      {
        pfnPktDeliver (&pRspQItem->mRsp, pUd);
      }
      
      //Regardless of delivery status, free the item.
      
      FREE (pRspQItem);
      pRspQItem = NULL;
      
      return AEE_SUCCESS;
    }
    else
    {
      return ERESOURCENOTFOUND;
    }
  }

  return EFAILED;
}
/*=========================================================================*/
static uint32
DiagMSM_ServiceListener (IDiagMSM *pDiagMSM, IDiagMSMLHandleType mHandle, 
                         PFNIDIAGMSM_LSVC pfnSvc, void *pUd)
{
  DiagMSM *pMe = (DiagMSM *) pDiagMSM;
  OEMDiagMSMListenerType *pOListener = OEMDIAGMSM_CVT_H2L (mHandle);
  OEMDiagMSMRecType *pORec = NULL;
  int32 dwRecLength = 0;
  boolean bFreeRec = TRUE;
  
  if (pMe && pOListener && (pOListener->p_mDiagMSM == pMe))
  {
    pORec = rbq_dequeue (pOListener->mReadyQ.mRbq, &dwRecLength);
  
    if (pORec)
    {
      if (pfnSvc)
      {
        bFreeRec=pfnSvc(&pORec->mRec,
                        (uint32)(dwRecLength - 
                                 (int32)FPOS(OEMDiagMSMRecType,mRec)),
                        pOListener->mListenerInfo.pUserData, 
                        pUd);
      }
      
      (void)MSG_3 (MSG_SSID_OEMDIAG, OEMDIAG_MSG_LDATAOUT, 
                   "LSVC pfn:%d del:%d SN:%d",
                   (pfnSvc) ? TRUE : FALSE, bFreeRec, pORec->mRec.dwSeqNum);

      (void)rbq_dequeue_return (pOListener->mReadyQ.mRbq, pORec, bFreeRec);
      
      return (bFreeRec) ? AEE_SUCCESS : EBADITEM;
    }
    else
    {
      return ERESOURCENOTFOUND;
    }
  }
  
  return EFAILED;
}
/*=========================================================================*/
static uint32
DiagMSM_ServiceOFlowBuf (IDiagMSM *pDiagMSM, IDiagMSMLHandleType mHandle, 
                         PFNIDIAGMSM_LSVC pfnSvc, void *pUd)
{
  DiagMSM *pMe = (DiagMSM *) pDiagMSM;
  OEMDiagMSMListenerType *pOListener = OEMDIAGMSM_CVT_H2L (mHandle);
  OEMDiagMSMRecType *pORec = NULL;
  int32 dwRecLength = 0;
  boolean bFreeRec = TRUE;
  
  if ( (pMe) && (pOListener) && (pOListener->p_mDiagMSM == pMe) && 
       (pOListener->mOverflowQ.mRbq) )
  {
    pORec = rbq_dequeue (pOListener->mOverflowQ.mRbq, &dwRecLength);
  
    if (pORec)
    {
      if (pfnSvc)
      {
        bFreeRec = pfnSvc (&pORec->mRec, 
                           (uint32)dwRecLength - FPOS (OEMDiagMSMRecType, mRec),
                           pOListener->mListenerInfo.pUserData, 
                           pUd);
      }
      
      (void)MSG_3 (MSG_SSID_OEMDIAG, OEMDIAG_MSG_LDATAOUT,
                   "LOFSVC pfn:%d del:%d SN:%d",
                   (pfnSvc) ? TRUE : FALSE, bFreeRec, pORec->mRec.dwSeqNum);

      (void)rbq_dequeue_return (pOListener->mOverflowQ.mRbq, pORec, bFreeRec);
      
      return (bFreeRec) ? AEE_SUCCESS : EBADITEM;
    }
    else
    {
      return ERESOURCENOTFOUND;
    }
  }
  
  return EFAILED;
}
/*=========================================================================*/
static IDiagMSMRecordType *
DiagMSM_RecAlloc (IDiagMSM *pDiagMSM, IDiagMSMLHandleType mHandle, 
                  uint32 dwSize)
{
  DiagMSM *pMe = (DiagMSM *) pDiagMSM;
  OEMDiagMSMListenerType *pOListener = OEMDIAGMSM_CVT_H2L (mHandle);
  OEMDiagMSMRecType *pORec = NULL;
  int32 dwRecLength = (int32) dwSize + (int32) FPOS (OEMDiagMSMRecType, mRec);
  
  if (pMe && pOListener && (pOListener->p_mDiagMSM == pMe))
  {
    pORec = rbq_alloc (pOListener->mReadyQ.mRbq, dwRecLength);

    if (pORec)
    {
      (void)MSG_3 (MSG_SSID_OEMDIAG, OEMDIAG_MSG_LDATAIN, 
                   "RecAlloc ID:0x%X, sz:%d SN:%d",
                   pOListener->mListenerInfo.dwId, dwSize, pORec->mRec.dwSeqNum);

      pORec->cQ = (uint8)OEMDIAGMSM_STD_RBQ;
      pORec->dwSize = (uint32)dwRecLength;

      return &pORec->mRec;
    }
    else
    {
      (void)MSG_3 (MSG_SSID_OEMDIAG, OEMDIAG_MSG_LDATAIN, 
                   "RecAlloc failed ID:0x%X, sz:%d SN:%d",
                   pOListener->mListenerInfo.dwId, dwSize, pORec->mRec.dwSeqNum);
    }
  }

  return NULL;

}
/*=========================================================================*/
static uint32
DiagMSM_RecCommit (IDiagMSM *pDiagMSM, IDiagMSMLHandleType mHandle, 
                   IDiagMSMRecordType *pRec)
{
  DiagMSM *pMe = (DiagMSM *) pDiagMSM;
  OEMDiagMSMListenerType *pOListener = OEMDIAGMSM_CVT_H2L (mHandle);
  OEMDiagMSMRecType *pORec = NULL;
  
  //TODO: Verify rec
  if (pMe && pRec && pOListener && (pOListener->p_mDiagMSM == pMe))
  {
    pORec = (OEMDiagMSMRecType *) 
      (void *)((uint8 *) pRec - FPOS (OEMDiagMSMRecType, mRec));
    /*lint -e{774}*/
    if (pORec == NULL) {
      (void)MSG (MSG_SSID_OEMDIAG, OEMDIAG_MSG_LDATAIN,
                     "RecCommit STD Inv Rec ptr");
      return EFAILED;
    }
    switch (pORec->cQ)
    {
      case OEMDIAGMSM_STD_RBQ:
        (void)rbq_commit (pOListener->mReadyQ.mRbq, pORec);
        
        (void)MSG_3 (MSG_SSID_OEMDIAG, OEMDIAG_MSG_LDATAIN,
                     "RecCommit STD SN:%d RQ:%d OFQ:%d",
                     pORec->mRec.dwSeqNum,
                     rbq_used (pOListener->mReadyQ.mRbq),
                     rbq_used (pOListener->mOverflowQ.mRbq));
        break;
      
      case OEMDIAGMSM_OF_RBQ:
        (void)rbq_commit (pOListener->mOverflowQ.mRbq, pORec);
        
        (void)MSG_3 (MSG_SSID_OEMDIAG, OEMDIAG_MSG_LDATAIN,
                     "RecCommit OF SN:%d RQ:%d OFQ:%d",
                     pORec->mRec.dwSeqNum,
                     rbq_used (pOListener->mReadyQ.mRbq),
                     rbq_used (pOListener->mOverflowQ.mRbq));
        break;
      default:
        return EFAILED;
    }

    return AEE_SUCCESS;
  }

  return EFAILED;
}
/*=========================================================================*/
static uint32
DiagMSM_RecShorten (IDiagMSM *pDiagMSM, IDiagMSMLHandleType mHandle, 
                    IDiagMSMRecordType *pRec, uint32 dwNewSize)
{
  DiagMSM *pMe = (DiagMSM *) pDiagMSM;
  OEMDiagMSMListenerType *pOListener = OEMDIAGMSM_CVT_H2L (mHandle);
  OEMDiagMSMRecType *pORec = NULL;
  
  if (pMe && pRec && pOListener && (pOListener->p_mDiagMSM == pMe) )
  {
    pORec = (OEMDiagMSMRecType *) 
      (void *)((uint8 *) pRec - FPOS (OEMDiagMSMRecType, mRec));

    /*lint -e{774}*/
    if (pORec) {
      switch (pORec->cQ)
      {
        case OEMDIAGMSM_STD_RBQ:
          rbq_shorten (pOListener->mReadyQ.mRbq, pORec, dwNewSize);
          break;
        
        case OEMDIAGMSM_OF_RBQ:
          rbq_shorten (pOListener->mOverflowQ.mRbq, pORec, dwNewSize);
          break;
  
        default:
          return EBADITEM;
      }
    }

    return AEE_SUCCESS;
  }

  return EFAILED;
}
/*===========================================================================

FUNCTION: OEMDIAGMSM_CLOBBER

DESCRIPTION
  This routine attempts to allocate a record in a listener Q that does not
  have sufficient space for it.  This is only used when the overflow option
  is set to "clobber".  In this case, the tail record of the RBQ, is the state
  has reached the threshold, will be discarded to make room for the requested
  allocation.
  
RETURN VALUE
  A pointer to a new record buffer, if available.  NULL otherwise.

===========================================================================*/
static OEMDiagMSMRecType * 
OEMDiagMSM_Clobber (DiagMSM *pMe, OEMDiagMSMListenerType *pOListener, 
                    uint32 dwLength)
{
  PARAM_NOT_CONST(pOListener)
  OEMDiagMSMRecType *pORec = NULL;
  void *rbq;
  OEMDiagMSMRecType *pOFlowRec = NULL;
  
  if (pMe && pOListener)
  {
    rbq = pOListener->mReadyQ.mRbq;

    if (rbq_used (pOListener->mOverflowQ.mRbq) > 0)
    {
      //Once the overflow Q is non-empty, all inbound records must be queued in
      //the oflow Q to maintain order.  If that failes, clobber failed.
      pORec = rbq_alloc (pOListener->mOverflowQ.mRbq, (int)dwLength);

      if (pORec)
      {
        (void)MSG_3 (MSG_SSID_OEMDIAG, OEMDIAG_MSG_LDATAIN,
                     "OF Persist 0x%X RQ:%d OFQ:%d", 
                     pOListener->mListenerInfo.dwId,
                     rbq_used (pOListener->mReadyQ.mRbq),
                     rbq_used (pOListener->mOverflowQ.mRbq));
        
        pORec->cQ = (uint8)OEMDIAGMSM_OF_RBQ;
      }

      return pORec;
    }

    //OFlow Q is empty - try the ready Q
    while (!pORec)
    {
      pORec = rbq_dequeue (rbq, NULL);

      if (pORec)
      {
        if (pORec->mRec.cState < pOListener->mListenerInfo.cOflowStateThreshold)
        {
          // State transition has not occurred, which is done by caller.
          // Allocate the record from the overflow Q.
          pOFlowRec = rbq_alloc (pOListener->mOverflowQ.mRbq, (int)dwLength);
          
          if (pOFlowRec)
          {
            pOFlowRec->cQ = OEMDIAGMSM_OF_RBQ;

            (void)MSG_3 (MSG_SSID_OEMDIAG, OEMDIAG_MSG_LDATAIN,
                         "OF Start 0x%X RQ:%d OFQ:%d", 
                         pOListener->mListenerInfo.dwId,
                         rbq_used (pOListener->mReadyQ.mRbq),
                         rbq_used (pOListener->mOverflowQ.mRbq));
            
            // Do not clobber this record.  The oflow Q has space...
            (void)rbq_dequeue_return (rbq, pORec, FALSE);
            pORec = NULL;
          }
          else
          {
#if defined (DEBUG_OEMDIAGMSM)
#error code not present
#endif

            /*lint -e{774} */
            // pOFlowRec is modified above, so this if does not always 
            // evaluate to false
            if (pOFlowRec)
            {
              (void)rbq_dequeue_return(pOListener->mOverflowQ.mRbq, 
                                       pOFlowRec, 
                                       FALSE);
              pOFlowRec = NULL;
            }
          }
        }
        
        if (pORec)
        {
          // This record has reached state threshold.  OK to clobber.
          (void)MSG_3 (MSG_SSID_OEMDIAG, OEMDIAG_MSG_LDATAIN,
                       "Clobber SN:%d RQ:%d OFQ:%d",
                       pORec->mRec.dwSeqNum,
                       rbq_used (pOListener->mReadyQ.mRbq),
                       rbq_used (pOListener->mOverflowQ.mRbq));

          (void)rbq_dequeue_return (rbq, pORec, TRUE);
          pORec = NULL;

#if defined (DEBUG_OEMDIAGMSM)
#error code not present
#endif
        }
      }
      else
      {
        (void)MSG_2 (MSG_SSID_OEMDIAG, OEMDIAG_MSG_LDATAIN,
                     "RQDQ fail RQ:%d OFQ:%d",
                     rbq_used (pOListener->mReadyQ.mRbq),
                     rbq_used (pOListener->mOverflowQ.mRbq));

        break;
      }

      // If this pointer is valid, the overflow queue is to be used.
      if (pOFlowRec)
      {
        pORec = pOFlowRec;
        pOFlowRec = NULL;

      }
      else if ((pOListener->mListenerInfo.dwLimit < 0 || 
        rbq_used (rbq) + (int32) dwLength <= pOListener->mListenerInfo.dwLimit))
      {
        pORec = rbq_alloc (rbq, (int)dwLength);
        
        if (pORec)
        {
          pORec->cQ = OEMDIAGMSM_STD_RBQ;
        }
      }
    }
  }

  return pORec;
}
/*===========================================================================

FUNCTION: OEMDIAGMSM_INITTHRESHOLD

DESCRIPTION
  This routine initializes the threshold for a given listener.
  
  If bActive == TRUE, any pending callback is cancelled.
    
===========================================================================*/
static void 
OEMDiagMSM_InitThreshold (IDiagMSMThresholdType *pThresh, boolean bActive)
{
  if (pThresh)
  {
    pThresh->dwThreshold = -1;

    if (bActive && pThresh->pCb)
    {
      CALLBACK_Cancel (pThresh->pCb);
    }

    pThresh->pCb = NULL;
  }
}
/*===========================================================================

FUNCTION: OEMDIAGMSM_SERVICETHRESHOLD

DESCRIPTION
  This routine compares the threshold to the given criteria, queueing the 
  associated AEECallback if the threshold is exceeded.
      
RETURN VALUE
  TRUE if the callback was queued.
  FALSE if no action is taken.
  
===========================================================================*/
static boolean
OEMDiagMSM_ServiceThreshold (DiagMSM *pMe, IDiagMSMThresholdType *pThresh, 
                             int32 dwCriteria)
{
  PARAM_NOT_CONST(pThresh)
  boolean bNotify = FALSE;

  if (pMe && pMe->pAc && pThresh && pThresh->pCb)
  {
    if ( ((pThresh->dwThreshold > 0) && 
          (dwCriteria >= pThresh->dwThreshold)) ||
         (pThresh->dwThreshold == 0) )
    {
      //Passed specified threshold; notify caller.
      if (!CALLBACK_IsQueued (pThresh->pCb))
      {
#if MIN_BREW_VERSION(3,0)
        AEE_ResumeCallback (pThresh->pCb, pMe->pAc);
#else
        AEE_ResumeEx (pThresh->pCb, 0, pMe->pAc);
#endif
        bNotify = TRUE;
      }
    }
  }

  return bNotify;
}
/*===========================================================================

FUNCTION: OEMDIAGMSM_RECMALLOC

DESCRIPTION
  This routine attempts to allocate a buffer for an inbound listener record.
  It takeks into account all settings associated with a listener, and services
  the listener threshold if successful.
      
RETURN VALUE
  Pointer to the allocated record.
  NULL if not successful.
    
===========================================================================*/
static OEMDiagMSMRecType *
OEMDiagMSM_RecMalloc (DiagMSM *pMe, OEMDiagMSMListenerType *pOListener, 
                      uint32 dwRecSize)
{
  OEMDiagMSMRecType *pORec = NULL;
  int32 dwUsage = 0; //For calculating thresholds
  int32 dwLimit = 0;

  if (pMe && pOListener && dwRecSize > 0)
  {
    dwUsage = (int32)dwRecSize + (int32)rbq_used (pOListener->mReadyQ.mRbq);
    dwLimit = pOListener->mListenerInfo.dwLimit;

    if (rbq_used(pOListener->mReadyQ.mRbq) == -1) {
      //assume that limit is exceeded if there was an error while 
      //determining the amount of space used in rbq
      (void)MSG (MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
                 "RBQ Usage Error");
    }
    else if ( (dwLimit >= 0) && (dwUsage > dwLimit) )
    {
      //Unless the clobber option is specified, flag this failure as 
      //being induced by the listener limit being exceeded.  This is
      //used to differentiate this from running out of overall buffer.
      //Limit will be exceeded.  If clobber option, start clobbering.
      if (pOListener->mListenerInfo.eOFlowOpt == (uint8)IDIAGMSM_OF_CLOBBER)
      {
        pORec = OEMDiagMSM_Clobber (pMe, pOListener, dwRecSize);
      }
    }
    else
    {
      pORec = rbq_alloc (pOListener->mReadyQ.mRbq, (int)dwRecSize);

      if (pORec)
      {
        (void)MSG_1 (MSG_SSID_OEMDIAG, OEMDIAG_MSG_LDATAIN, 
                     "Rec Alloc Success 0x%X", 
                     pOListener->mListenerInfo.dwId);

        pORec->cQ = (uint8)OEMDIAGMSM_STD_RBQ;
      }

      if (!pORec)
      {
        if (pOListener->mListenerInfo.eOFlowOpt == (uint8)IDIAGMSM_OF_CLOBBER)
        {
          //Attempt to alloc at the expense of older records.
          pORec = OEMDiagMSM_Clobber (pMe, pOListener, dwRecSize);
        }
      }
    }
    
    if (OEMDiagMSM_ServiceThreshold (pMe, 
          &pOListener->mListenerInfo.mListenerThresh, dwUsage) == TRUE)
    {
      (void)MSG_3 (MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY, 
                   "Listener Notify 0x%X Th:%d U:%d", 
                   pOListener->mListenerInfo.dwId,
                   pOListener->mListenerInfo.mListenerThresh.dwThreshold, 
                   dwUsage);

    }
  }

  return pORec;
}
#ifdef OEMDIAGMSM_DUMPLOG
static boolean 
OEMDiagMSM_DumpLog (DiagMSM  * pMe, const void * pLog, unsigned int dwLen)
{
  IFile         * pFile = NULL;

  if (pMe && pLog) {
    pFile = pMe->m_pIFileLog; 
  
    // Now we are ready to write the log into the file
    IFILE_Write( pFile, (void *)pLog, dwLen ); 
  }

  return TRUE;
}
#endif

/*===========================================================================

FUNCTION: OEMDIAGMSM_LOGLISTENER

DESCRIPTION
  This routine is called via function pointer by the DIAG task to deliver a
  log for the given listener.
                 
===========================================================================*/
static void OEMDiagMSM_LogListener (
  uint32 dwSeqNum, 
  const byte *pLog, 
  unsigned int dwLength, 
  void *pParam 
) {
  OEMDiagMSMListenerType *pOListener = (OEMDiagMSMListenerType *) pParam;
  DiagMSM *pMe = NULL;
  OEMDiagMSMRecType *pORec = NULL;
  uint32 dwRecSize = dwLength + FPOS (OEMDiagMSMRecType, mRec.uRec.mLog.aLog);
  
  if (pLog && pOListener)
  {
    pMe = pOListener->p_mDiagMSM;
  
    if (pMe)
    {
      // Everything looks good.  Proceed with buffering.
      pORec = OEMDiagMSM_RecMalloc (pMe, pOListener, dwRecSize);

#ifdef OEMDIAGMSM_DUMPLOG
      OEMDiagMSM_DumpLog(pMe, pLog, dwLength);
#endif
      if (pORec)
      {
        pORec->mRec.dwSeqNum = dwSeqNum;
        pORec->mRec.mHandle = (IDiagMSMLHandleType) pOListener;
        pORec->dwSize = dwRecSize;
        pORec->mRec.cState = 0;
        pORec->mRec.eType = (uint8)IDIAGMSM_ID_LOG;
        pORec->mRec.uRec.mLog.dwLogId=(uint32)log_get_code((PACKED void *)pLog);
        pORec->mRec.uRec.mLog.dwLength = dwLength;
      
        (void)MEMCPY ((void *)pORec->mRec.uRec.mLog.aLog, pLog, dwLength);
      
        switch (pORec->cQ)
        {
          case OEMDIAGMSM_STD_RBQ:
            (void)MSG_2 (MSG_SSID_OEMDIAG, OEMDIAG_MSG_LDATAIN,
                         "Commit Log RQ ID:0x%X SN:%d",
                         pORec->mRec.uRec.mLog.dwLogId, pORec->mRec.dwSeqNum);

            (void)rbq_commit (pOListener->mReadyQ.mRbq, pORec);
            break;

          case OEMDIAGMSM_OF_RBQ:
            (void)MSG_2 (MSG_SSID_OEMDIAG, OEMDIAG_MSG_LDATAIN,
                         "Commit Log OFQ ID:0x%X SN:%d",
                         pORec->mRec.uRec.mLog.dwLogId, pORec->mRec.dwSeqNum);

            (void)rbq_commit (pOListener->mOverflowQ.mRbq, pORec);
            break;
          default:
            break;
        }
      }
    }
  }
}

#ifdef OEMDIAGMSM_DUMPLOG
static boolean OEMDiagMSM_DumpEvent(
  DiagMSM  * pMe, 
  const diag_event_type * pEvent
) {

  IFile         * pFile = NULL;

  if (pMe && pEvent) {
    pFile = pMe->m_pIFileEvent; 
    // Now we are ready to write the log into the file
    IFILE_Write( pFile, (void *)pEvent, sizeof(diag_event_type) ); 
  }
  return TRUE;

}
#endif
/*===========================================================================

FUNCTION: OEMDiagMSM_EventListener

DESCRIPTION
  This routine is called via function pointer by the DIAG task to deliver an
  event for the given listener.
      
===========================================================================*/
static void OEMDiagMSM_EventListener (
  uint32 dwSeqNum, 
  const diag_event_type * pEvent, 
  void *pParam
) {
  OEMDiagMSMListenerType *pOListener = (OEMDiagMSMListenerType *) pParam;
  DiagMSM *pMe = NULL;
  OEMDiagMSMRecType *pORec = NULL;
  uint32 dwRecSize = 0;
  
  if (pEvent && pOListener)
  {
    dwRecSize = FPOS (OEMDiagMSMRecType, mRec.uRec.mEvent.aPayload) +
      pEvent->length;

    pMe = pOListener->p_mDiagMSM;
  
    if (pMe)
    {
      // Everything looks good.  Proceed with buffering.
      pORec = OEMDiagMSM_RecMalloc (pMe, pOListener, dwRecSize);

#ifdef OEMDIAGMSM_DUMPLOG
      OEMDiagMSM_DumpEvent(pMe, pEvent);
#endif
      if (pORec)
      {
        pORec->mRec.dwSeqNum = dwSeqNum;
        pORec->mRec.mHandle = (IDiagMSMLHandleType) pOListener;
        pORec->dwSize = (uint32)dwRecSize;
        pORec->mRec.cState = 0;
        pORec->mRec.eType = (uint8)IDIAGMSM_ID_EVENT;
        
        pORec->mRec.uRec.mEvent.dwEventId = pEvent->event_id;
        pORec->mRec.uRec.mEvent.qwTs.qw[0] = qw_lo ((uint32 *) pEvent->ts);
        pORec->mRec.uRec.mEvent.qwTs.qw[1] = qw_hi ((uint32 *) pEvent->ts);
        pORec->mRec.uRec.mEvent.bLength = pEvent->length;

        // Return value is not checked because there isn't much we can do 
        // in case of the failure
        (void)MEMCPY ((void *)pORec->mRec.uRec.mEvent.aPayload, 
                      pEvent->payload, 
                      pEvent->length);
            
        switch (pORec->cQ)
        {
          //TODO: Make a func
          case OEMDIAGMSM_STD_RBQ:
            (void)MSG_2 (MSG_SSID_OEMDIAG, OEMDIAG_MSG_LDATAIN,
                         "Commit Evt RQ ID:0x%X SN:%d",
                         pORec->mRec.uRec.mEvent.dwEventId, 
                         pORec->mRec.dwSeqNum);
            
            (void)rbq_commit (pOListener->mReadyQ.mRbq, pORec);
            break;

          case OEMDIAGMSM_OF_RBQ:
            (void)MSG_2 (MSG_SSID_OEMDIAG, OEMDIAG_MSG_LDATAIN,
                         "Commit Evt OFQ ID:0x%X SN:%d",
                         pORec->mRec.uRec.mEvent.dwEventId, 
                         pORec->mRec.dwSeqNum);
            
            (void)rbq_commit (pOListener->mOverflowQ.mRbq, pORec);
            break;
          default:
            break;
        }
      }
    }
  }
}
/*=========================================================================*/
static uint32
DiagMSM_GetListenerInfo (IDiagMSM *pDiagMSM, IDiagMSMLHandleType mHandle, 
                         IDiagMSMListenerType *pDestLCopy)
{
  DiagMSM *pMe = (DiagMSM *) pDiagMSM;
  OEMDiagMSMListenerType *pOListener = OEMDIAGMSM_CVT_H2L (mHandle);

  if ( (pMe != NULL) && (pOListener != NULL) && 
       (pOListener->p_mDiagMSM == pMe) && 
       (pDestLCopy != NULL) )
  {
    *pDestLCopy = pOListener->mListenerInfo;

    return AEE_SUCCESS;
  }

  return EFAILED;
}
/*=========================================================================*/
static int32
DiagMSM_BufUsage (IDiagMSM *pDiagMSM, IDiagMSMLHandleType mHandle)
{
  DiagMSM *pMe = (DiagMSM *) pDiagMSM;
  OEMDiagMSMListenerType *pOListener = OEMDIAGMSM_CVT_H2L (mHandle);
  int32 dwUsage = -1;

  if ( (pMe) && (pOListener) && (pOListener->p_mDiagMSM == pMe) )
  {
    dwUsage = (int32) rbq_used (pOListener->mReadyQ.mRbq);
  }

  return dwUsage;
}
/*=========================================================================*/
static uint32
DiagMSM_GetOFlowOpt (IDiagMSM *pDiagMSM, IDiagMSMLHandleType mHandle, 
                     uint8 *pOpt)
{
  DiagMSM *pMe = (DiagMSM *) pDiagMSM;
  OEMDiagMSMListenerType *pOListener = OEMDIAGMSM_CVT_H2L (mHandle);

  if ( (pMe) && (pOListener) && (pOListener->p_mDiagMSM == pMe) && (pOpt) )
  {
    *pOpt = pOListener->mListenerInfo.eOFlowOpt;
    return AEE_SUCCESS;
  }

  return EFAILED;
}
/*=========================================================================*/
static uint32 
DiagMSM_GetLThreshold (IDiagMSM *pDiagMSM, IDiagMSMLHandleType mHandle, 
                       int32 *pThreshold)
{
  DiagMSM *pMe = (DiagMSM *) pDiagMSM;
  OEMDiagMSMListenerType *pOListener = OEMDIAGMSM_CVT_H2L (mHandle);

  if ( (pMe) && (pOListener) && (pOListener->p_mDiagMSM == pMe) && 
       (pThreshold) )
  {
    *pThreshold = pOListener->mListenerInfo.mListenerThresh.dwThreshold;

    return AEE_SUCCESS;
  }

  return EFAILED;
}
/*=========================================================================*/
static uint32
DiagMSM_GetLimit (IDiagMSM *pDiagMSM, IDiagMSMLHandleType mHandle, 
                  int32 *pLimit)
{
  OEMDiagMSMListenerType *pOListener = OEMDIAGMSM_CVT_H2L (mHandle);
  DiagMSM *pMe = (DiagMSM *) pDiagMSM;

  if ( (pMe) && (pOListener) && (pOListener->p_mDiagMSM == pMe) && (pLimit) )
  {
    *pLimit = pOListener->mListenerInfo.dwLimit;
    
    return AEE_SUCCESS;
  }

  return EFAILED;
}
/*=========================================================================*/
static uint32
DiagMSM_GetSize (IDiagMSM *pDiagMSM, IDiagMSMLHandleType mHandle, 
                 uint32 *pSize)
{
  OEMDiagMSMListenerType *pOListener = OEMDIAGMSM_CVT_H2L (mHandle);
  DiagMSM *pMe = (DiagMSM *) pDiagMSM;

  if ( (pMe) && (pOListener) && (pOListener->p_mDiagMSM == pMe) && (pSize) )
  {
    *pSize = pOListener->mListenerInfo.dwSize;
    
    return AEE_SUCCESS;
  }

  return EFAILED;
}
/*=========================================================================*/
static uint32
DiagMSM_SetOFlowOpt (IDiagMSM *pDiagMSM, IDiagMSMLHandleType mHandle, 
                     uint8 eOpt)
{
  DiagMSM *pMe = (DiagMSM *) pDiagMSM;
  OEMDiagMSMListenerType *pOListener = OEMDIAGMSM_CVT_H2L (mHandle);

  if ( (pMe) && (pOListener) && (pOListener->p_mDiagMSM == pMe) )
  {
    if (eOpt < (uint8)IDIAGMSM_OF_COUNT)
    {
      (void)MSG_3 (MSG_SSID_OEMDIAG, OEMDIAG_MSG_CONFIG, 
                   "Changing OF Opt ID:0x%X old:%d new:%d",
                   pOListener->mListenerInfo.dwId, 
                   pOListener->mListenerInfo.eOFlowOpt,
                   eOpt);

      pOListener->mListenerInfo.eOFlowOpt = eOpt;
      return AEE_SUCCESS;
    }
    else
    {
      return EBADITEM;
    }
  }

  return EFAILED;
}
/*=========================================================================*/
static uint32 
DiagMSM_SetLThreshold (IDiagMSM *pDiagMSM, IDiagMSMLHandleType mHandle, 
                       int32 dwThreshold)
{
  DiagMSM *pMe = (DiagMSM *) pDiagMSM;
  OEMDiagMSMListenerType *pOListener = OEMDIAGMSM_CVT_H2L (mHandle);
  int32 dwUsage;

  if ( (pMe) && (pOListener) && (pOListener->p_mDiagMSM == pMe) )
  {
    (void)MSG_3 (MSG_SSID_OEMDIAG, OEMDIAG_MSG_CONFIG, 
                 "Changing Thresh ID:0x%X old:%d new:%d",
                 pOListener->mListenerInfo.dwId, 
                 pOListener->mListenerInfo.mListenerThresh.dwThreshold,
                 dwThreshold);
    
    pOListener->mListenerInfo.mListenerThresh.dwThreshold = 
      (dwThreshold >= 0) ? dwThreshold : -1;

    dwUsage = DiagMSM_BufUsage (pMe, mHandle);
    
    if (OEMDiagMSM_ServiceThreshold (pMe, 
          &pOListener->mListenerInfo.mListenerThresh, dwUsage) == TRUE)
    {
      (void)MSG_3 (MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY | OEMDIAG_MSG_CONFIG, 
                   "Listener Notify 0x%X Th:%d U:%d", 
                   pOListener->mListenerInfo.dwId,
                   pOListener->mListenerInfo.mListenerThresh.dwThreshold,
                   dwUsage);
    }

    return AEE_SUCCESS;
  }

  return EFAILED;
}
/*=========================================================================*/
static uint32
DiagMSM_SetLimit (IDiagMSM *pDiagMSM, IDiagMSMLHandleType mHandle, 
                  int32 dwLimit)
{
  OEMDiagMSMListenerType *pOListener = OEMDIAGMSM_CVT_H2L (mHandle);
  DiagMSM *pMe = (DiagMSM *) pDiagMSM;

  if ( (pMe) && (pOListener) && (pOListener->p_mDiagMSM == pMe) )
  {
    (void)MSG_4 (MSG_SSID_OEMDIAG, OEMDIAG_MSG_CONFIG, 
                 "Changing Limit ID:0x%X old:%d new:%d U:%d",
                 pOListener->mListenerInfo.dwId, 
                 pOListener->mListenerInfo.dwLimit,
                 (dwLimit >= 0) ? dwLimit: -1, 
                 DiagMSM_BufUsage (pMe, mHandle));
    
    pOListener->mListenerInfo.dwLimit =
      (dwLimit >= 0) ? dwLimit: -1;
    
    return AEE_SUCCESS;
  }

  return EFAILED;
}

/*=========================================================================
Function: DiagMSM_GetVersion()

DESCRIPTION
  This is an interface function which is used by an applications to obtain
  the version of IDiagMSM

RETURN VALUE
  AEE_SUCCESS              :  Get version operation is successful
  IDIAGMSM_VER_EFAILED     :  General Error
  IDIAGMSM_VER_EINVALIDLEN :  Invalid buffer length
  IDIAGMSM_VER_EINSBUFMEM  :  Insufficient buffer memory

===========================================================================*/
static uint32 
DiagMSM_GetVersion(IDiagMSM *pDiagMSM, AECHAR *pBuf, uint32 dwBufLen) 
{
  PARAM_NOT_REF(pDiagMSM) 
  PARAM_NOT_CONST(pDiagMSM)

  if (pBuf && pDiagMSM)
  {
    if (dwBufLen < (STRLEN(IDIAGMSM_MODULE_VERSION) + 1) * sizeof(AECHAR)) 
    {
      (void) MSG ( MSG_SSID_OEMDIAG, OEMDIAG_MSG_LDATAIN,
                   "DiagMSM_GetVersion: Buffer too small" );
      return IDIAGMSM_VER_EINSBUFMEM;
    }

    if ((int)dwBufLen < 0)
    {
      (void) MSG ( MSG_SSID_OEMDIAG, OEMDIAG_MSG_LDATAIN,
                   "DiagMSM_GetVersion: error, dwBufLen < 0." );
      return IDIAGMSM_VER_EINVALIDLEN;
    }

    (void) STRTOWSTR( IDIAGMSM_MODULE_VERSION, pBuf, (int)dwBufLen );
    // Insert a NULL terminator in the end of the string
    pBuf[STRLEN(IDIAGMSM_MODULE_VERSION)] = (AECHAR)'\0';
    return AEE_SUCCESS;
  }
  return IDIAGMSM_VER_EFAILED;
}

/*=========================================================================
Function: DiagMSM_GetFeatures()

DESCRIPTION
  This is an interface function intended to be used by applications to obtain
  features supported by IDiagMSM module.

PARAMETERS:
  pBufLen : IDiagMSM provides the size in bytes of the feature bit mask buffer. 

RETURN VALUE
  pointer to feature bit mask buffer of bytes.  Each bit denotes a feature
  supported by IDiagMSM module.
  The pointer will be set to NULL if an error is encountered.

NOTE
  This function may be used as a future method for determining the various 
  interface versions supported by IDiagMSM module.

===========================================================================*/
static const byte *
DiagMSM_GetFeatures(IDiagMSM *pDiagMSM, uint32 *pBufLen)
{
  PARAM_NOT_REF(pDiagMSM) 
  PARAM_NOT_CONST(pDiagMSM)
  
  if (!pDiagMSM || !pBufLen)
  {
    (void) MSG ( MSG_SSID_OEMDIAG, OEMDIAG_MSG_LDATAIN,
                 "IDiagMSM: DiagMSM_GetFeatures: SC F" );
    return NULL;
  }

  *pBufLen = sizeof(gszFeatBufMask);

  return gszFeatBufMask;
}

//===========================================================================
//
// IDiagMSM File Interface
// ==========================================================================
static IDiagMSMFile
DiagMSM_File_Open(IDiagMSM *pDiagMSM, const char * pPath, int oFlag, 
                  uint16 nMode)
{
  PARAM_NOT_CONST(pDiagMSM) 
  IDiagMSMFile   nFile;

  if (pDiagMSM == NULL) {
    return IDIAGMSM_FILE_ERROR;
  }

  pDiagMSM->m_nLastFileError = SUCCESS;

  if ( pPath == NULL ) {
    pDiagMSM->m_nLastFileError = IDIAGMSM_FILE_BADPARM;
    return IDIAGMSM_FILE_ERROR;
  }

  // validate the path from read black list
  if ( (oFlag == IDIAGMSM_FILE_RDONLY) &&
       (DiagMSM_File_ValidatePath(pDiagMSM, pPath, 
                                  IDIAGMSM_BLACKLIST_READ) == FALSE)) {
    pDiagMSM->m_nLastFileError = IDIAGMSM_FILE_BADPATH;
    return IDIAGMSM_FILE_ERROR;
  }

  // validate the path from write black list if the file is create
  if ( ((oFlag & 
        (IDIAGMSM_FILE_CREAT|IDIAGMSM_FILE_WRONLY|IDIAGMSM_FILE_TRUNC|
         IDIAGMSM_FILE_APPEND|IDIAGMSM_FILE_RDWR|IDIAGMSM_FILE_EXCL)) != 0) &&
       (DiagMSM_File_ValidatePath(pDiagMSM, pPath, 
                                  IDIAGMSM_BLACKLIST_WRITE) == FALSE)) {
    pDiagMSM->m_nLastFileError = IDIAGMSM_FILE_BADPATH;
    return IDIAGMSM_FILE_ERROR;
  }

  // If the file is create, we need an extra parameter
  if ( (oFlag & IDIAGMSM_FILE_CREAT) != 0 ) {    
    nFile = (IDiagMSMFile)efs_open(pPath, oFlag, nMode);
    if (nFile < 0) {
      pDiagMSM->m_nLastFileError = efs_errno;
      nFile = IDIAGMSM_FILE_ERROR;
    }            
    return nFile;
  }

  nFile = (IDiagMSMFile)efs_open(pPath, oFlag);
  if (nFile < 0) {
    pDiagMSM->m_nLastFileError = efs_errno;
    nFile = IDIAGMSM_FILE_ERROR;
  }
  return nFile;
}
/*=========================================================================*/
static uint32
DiagMSM_File_Close(IDiagMSM *pDiagMSM, IDiagMSMFile pHandle)
{
  PARAM_NOT_CONST(pDiagMSM)

  if (pDiagMSM == NULL) {
    return EFAILED;
  }

  pDiagMSM->m_nLastFileError = SUCCESS;
  if (efs_close(pHandle) == IDIAGMSM_FS_SUCCESS) {
    return AEE_SUCCESS;
  }

  pDiagMSM->m_nLastFileError = efs_errno;
  return EFAILED;
}
/*=========================================================================*/
static int32
DiagMSM_File_Read(IDiagMSM *pDiagMSM, IDiagMSMFile handle, void * pBuf, 
                  int32 nBufSz)
{ 
  PARAM_NOT_CONST(pDiagMSM)
  int32 nRet;

  if (pDiagMSM == NULL) { 
    return IDIAGMSM_FILE_ERROR;
  }

  pDiagMSM->m_nLastFileError = SUCCESS;
  if ( (pBuf == NULL) || (nBufSz < 0) ) {
    pDiagMSM->m_nLastFileError = IDIAGMSM_FILE_BADPARM;
    return IDIAGMSM_FILE_ERROR;
  }

  nRet = efs_read(handle, pBuf, (uint32)nBufSz);
  if (nRet < 0) {
    pDiagMSM->m_nLastFileError = efs_errno;
    nRet = IDIAGMSM_FILE_ERROR;
  }           

  return nRet;
}
/*=========================================================================*/
static int32
DiagMSM_File_Seek(IDiagMSM *pDiagMSM, IDiagMSMFile handle, int32 nOffset,
                  int nWhence)
{
  PARAM_NOT_CONST(pDiagMSM)
  int32 nRet;

  if (pDiagMSM == NULL) {
    return IDIAGMSM_FILE_ERROR;
  }

  pDiagMSM->m_nLastFileError = SUCCESS;
  nRet = efs_lseek(handle, nOffset, nWhence);
  if (nRet < 0) {
    pDiagMSM->m_nLastFileError = efs_errno;
    nRet = IDIAGMSM_FILE_ERROR;
  }

  return nRet;
}
/*=========================================================================*/
static uint32
DiagMSM_File_Truncate(IDiagMSM *pDiagMSM, const char * pPath, int32 nOffset)
{
  PARAM_NOT_CONST(pDiagMSM)

  if (pDiagMSM == NULL) {
    return EFAILED;
  }

  pDiagMSM->m_nLastFileError = SUCCESS;
  if ( pPath == NULL ) {
    pDiagMSM->m_nLastFileError = IDIAGMSM_FILE_BADPARM;
    return EFAILED;
  }

  // validate the path
  if (DiagMSM_File_ValidatePath(pDiagMSM, pPath, 
                                IDIAGMSM_BLACKLIST_WRITE) == FALSE) {
    pDiagMSM->m_nLastFileError = IDIAGMSM_FILE_BADPATH;
    return EFAILED;
  }

  if (efs_truncate(pPath, nOffset) == IDIAGMSM_FS_SUCCESS) {
    return AEE_SUCCESS;
  }

  pDiagMSM->m_nLastFileError = efs_errno;
  return EFAILED;
}
/*=========================================================================*/
static int32
DiagMSM_File_Write(IDiagMSM *pDiagMSM, IDiagMSMFile handle, const void * pBuf,
                   int32 nBufSz)
{
  PARAM_NOT_CONST(pDiagMSM)
  int32 nRet;

  if ( pDiagMSM == NULL ) {
    return IDIAGMSM_FILE_ERROR;
  }

  pDiagMSM->m_nLastFileError = SUCCESS;
  if ( (pBuf == NULL) || (nBufSz < 0) ) {
    pDiagMSM->m_nLastFileError = IDIAGMSM_FILE_BADPARM;
    return IDIAGMSM_FILE_ERROR;
  }

  nRet = efs_write(handle, pBuf, (uint32)nBufSz);
  if (nRet < 0) {
    pDiagMSM->m_nLastFileError = efs_errno;
    nRet = IDIAGMSM_FILE_ERROR;
  }
  return nRet;
}
/*=========================================================================*/
static uint32
DiagMSM_File_MkDir(IDiagMSM *pDiagMSM, const char * pPath, uint16 nMode)
{
  PARAM_NOT_CONST(pDiagMSM)

  if ( pDiagMSM == NULL ) {
    return EFAILED;
  }

  pDiagMSM->m_nLastFileError = SUCCESS;
  if ( pPath == NULL ) {
    pDiagMSM->m_nLastFileError = IDIAGMSM_FILE_BADPARM;
    return EFAILED;
  }

  // validate the path
  if (DiagMSM_File_ValidatePath(pDiagMSM, pPath, 
                                IDIAGMSM_BLACKLIST_WRITE) == FALSE) {
    pDiagMSM->m_nLastFileError = IDIAGMSM_FILE_BADPATH;
    return EFAILED;
  }

  if (efs_mkdir(pPath, nMode) == IDIAGMSM_FS_SUCCESS) {
    return AEE_SUCCESS;
  }

  pDiagMSM->m_nLastFileError = efs_errno;
  return EFAILED;
}
/*=========================================================================*/
static uint32
DiagMSM_File_Rename(IDiagMSM *pDiagMSM, const char * pOldPath, 
                    const char * pNewPath)
{ 
  PARAM_NOT_CONST(pDiagMSM)

  if (pDiagMSM == NULL) {
    return EFAILED;
  }

  pDiagMSM->m_nLastFileError = SUCCESS;

  if ( (pOldPath == NULL) || (pNewPath == NULL) ) {
    pDiagMSM->m_nLastFileError = IDIAGMSM_FILE_BADPARM;
    return EFAILED;
  }

  if ( (DiagMSM_File_ValidatePath(pDiagMSM, pOldPath, 
                                  IDIAGMSM_BLACKLIST_WRITE) == FALSE) ||
       (DiagMSM_File_ValidatePath(pDiagMSM, pNewPath, 
                                  IDIAGMSM_BLACKLIST_WRITE) == FALSE) ) {
    pDiagMSM->m_nLastFileError = IDIAGMSM_FILE_BADPATH;
    return EFAILED;
  }


  if (efs_rename(pOldPath, pNewPath) == IDIAGMSM_FS_SUCCESS) {
    return AEE_SUCCESS;
  }

  pDiagMSM->m_nLastFileError = efs_errno;
  return EFAILED;
}
/*=========================================================================*/
static uint32
DiagMSM_File_RmDir(IDiagMSM *pDiagMSM, const char * pPath)
{
  PARAM_NOT_CONST(pDiagMSM)

  if (pDiagMSM == NULL) {
    return EFAILED;
  }

  pDiagMSM->m_nLastFileError = SUCCESS;

  if ( pPath == NULL ) {
    pDiagMSM->m_nLastFileError = IDIAGMSM_FILE_BADPARM;
    return EFAILED;
  }

  if (DiagMSM_File_ValidatePath(pDiagMSM, pPath, 
                                IDIAGMSM_BLACKLIST_WRITE) == FALSE) {
    pDiagMSM->m_nLastFileError = IDIAGMSM_FILE_BADPATH;
    return EFAILED;
  }

  if (efs_rmdir(pPath) == IDIAGMSM_FS_SUCCESS) {
    return AEE_SUCCESS;
  }

  pDiagMSM->m_nLastFileError = efs_errno;
  return EFAILED;
}
/*=========================================================================*/
static uint32
DiagMSM_File_Stat(IDiagMSM *pDiagMSM, const char *pPath, 
                  IDiagMSMFileStat * pBuf)
{
  PARAM_NOT_CONST(pDiagMSM)

  struct fs_stat * pStat;

  if (pDiagMSM == NULL) {
    return EFAILED;
  }

  pDiagMSM->m_nLastFileError = SUCCESS;

  if ( (pPath == NULL) || (pBuf == NULL) ) {
    pDiagMSM->m_nLastFileError = IDIAGMSM_FILE_BADPARM;
    return EFAILED;
  }

  if (DiagMSM_File_ValidatePath(pDiagMSM, pPath, 
                                IDIAGMSM_BLACKLIST_READ) == FALSE) {
    pDiagMSM->m_nLastFileError = IDIAGMSM_FILE_BADPATH;
    return EFAILED;
  }

  pStat = (struct fs_stat *)MALLOC(sizeof(struct fs_stat));
  if ( pStat == NULL ) {
    pDiagMSM->m_nLastFileError = IDIAGMSM_FILE_EFAILED;
    return EFAILED;
  }

  if (efs_stat(pPath, pStat) == IDIAGMSM_FS_SUCCESS) {
    // Copy the result to the output buffer
    (void)MEMCPY((void*)pBuf, (void*)pStat, sizeof(IDiagMSMFileStat));
    FREE((void *)pStat);
    pStat = NULL;  
    return AEE_SUCCESS;
  }

  pDiagMSM->m_nLastFileError = efs_errno;
  FREE((void *)pStat);
  pStat = NULL;
  return EFAILED;
}
/*=========================================================================*/
static uint32
DiagMSM_File_Remove(IDiagMSM *pDiagMSM, const char *pPath)
{
  PARAM_NOT_CONST(pDiagMSM)

  if (pDiagMSM == NULL) {
    return EFAILED;
  }

  pDiagMSM->m_nLastFileError = SUCCESS;

  if (pPath == NULL) {
    pDiagMSM->m_nLastFileError = IDIAGMSM_FILE_BADPARM;
    return EFAILED;
  }

  if (DiagMSM_File_ValidatePath(pDiagMSM, pPath, 
                                IDIAGMSM_BLACKLIST_WRITE) == FALSE) {
    pDiagMSM->m_nLastFileError = IDIAGMSM_FILE_BADPATH;
    return EFAILED;
  }

  if (efs_unlink(pPath) == IDIAGMSM_FS_SUCCESS) {
    return AEE_SUCCESS;
  }

  pDiagMSM->m_nLastFileError = efs_errno;
  return EFAILED;
}
/*=========================================================================*/
static IDiagMSM_EFSDIR *
DiagMSM_File_OpenDir(IDiagMSM *pDiagMSM, const char *pPath)
{
  PARAM_NOT_CONST(pDiagMSM)
  IDiagMSM_EFSDIR * pRet = NULL;

  if (pDiagMSM == NULL) {
    return NULL;
  }

  pDiagMSM->m_nLastFileError = SUCCESS;

  if (pPath == NULL) {
    pDiagMSM->m_nLastFileError = IDIAGMSM_FILE_BADPARM;
    return NULL;
  }

  if (DiagMSM_File_ValidatePath(pDiagMSM, pPath, 
                                IDIAGMSM_BLACKLIST_READ) == FALSE) {
    pDiagMSM->m_nLastFileError = IDIAGMSM_FILE_BADPATH;
    return NULL;
  }

  pRet = (IDiagMSM_EFSDIR *)(void *)efs_opendir( pPath );
  if (pRet == NULL) {
    pDiagMSM->m_nLastFileError = efs_errno;
  }
  return pRet;
}
/*=========================================================================*/
static IDiagMSMDirEnt *
DiagMSM_File_ReadDir(IDiagMSM *pDiagMSM, const IDiagMSM_EFSDIR *pDir)
{
  IDiagMSMDirEnt *pRet = NULL;

  if (pDiagMSM == NULL) {
    return NULL;
  }

  pDiagMSM->m_nLastFileError = SUCCESS;

  if (pDir == NULL) {
    pDiagMSM->m_nLastFileError = IDIAGMSM_FILE_BADPARM;
    return NULL;
  }

  pRet = (IDiagMSMDirEnt *)(void *)efs_readdir( (EFSDIR *)(void *)pDir );
  if (pRet == NULL) {
    pDiagMSM->m_nLastFileError = efs_errno;
  }
  return pRet;
}
/*=========================================================================*/
static uint32
DiagMSM_File_CloseDir(IDiagMSM *pDiagMSM, const IDiagMSM_EFSDIR *pDir)
{
  if (pDiagMSM == NULL) {
    return 0;
  }

  pDiagMSM->m_nLastFileError = SUCCESS;

  if (pDir == NULL) {
    pDiagMSM->m_nLastFileError = IDIAGMSM_FILE_BADPARM;
    return 0;
  }

  if ( efs_closedir((EFSDIR *)(void *)pDir) == IDIAGMSM_FS_SUCCESS ) {
    return AEE_SUCCESS;
  } 
  pDiagMSM->m_nLastFileError = efs_errno;
  return EFAILED;
}
/*=========================================================================*/
static int
DiagMSM_File_GetLastError(IDiagMSM *pDiagMSM) 
{
  if (pDiagMSM == NULL) {
    return EFAILED;
  }
  return pDiagMSM->m_nLastFileError;
}
/*=========================================================================
Function: DiagMSM_File_ValidatePath()

Description:
  This function validates the path. Path is considered invalid if it is a 
  relative path or if it is in a black list.

Prototype:
  boolean DiagMSM_File_ValidatePath(IDiagMSM *pDiagMSM, const char *pPath)

Parameters:
  pDiagMSM : Pointer to IDiagMSM Interface object
  pPath :    Pointer to path to be validated.
  nBlackList : Contains info on which black list to use: Read or Write

Return Value:
  TRUE if path validation succeeded
  FALSE if path validation failed

Comments: 
  None

Side Effects: 
  None

See Also:
   DiagMSM_File_ReadPathBlackList and DiagMSM_File_WritePathBlackList

==============================================================================*/
static boolean 
DiagMSM_File_ValidatePath(IDiagMSM *pDiagMSM, const char *pPath,
                          BlackListType nBlackList) 
{
  uint32 nIdx;
  char * pPathCpy;

  if ( (pDiagMSM == NULL) || (pPath == NULL) ) {
    return FALSE;
  }

  if ( (STRSTR(pPath, "..\\") != NULL) ||
       (STRSTR(pPath, "../")  != NULL) || 
       (STRSTR(pPath, ".\\") != NULL) ||
       (STRSTR(pPath, "./")  != NULL) ||
       (STRCMP(pPath, ".") == 0) ) {
    return FALSE;
  }

  // Create a copy of the path to convert it to lower case
  pPathCpy = MALLOC(STRLEN(pPath) + 1);
  if (pPathCpy == NULL) {
    return FALSE;
  }
  (void) MEMCPY((void*)pPathCpy, (void*)pPath, (STRLEN(pPath)+1));

  // Convert Path to lower case letters
  pPathCpy = STRLOWER(pPathCpy);
  if (nBlackList == IDIAGMSM_BLACKLIST_READ) {
    for (nIdx = 0; nIdx < IDIAGMSM_READBLACKLIST_LENGTH; nIdx++) {
      if (STRBEGINS(DiagMSM_File_ReadPathBlackList[nIdx], pPathCpy) == TRUE) {
        FREEIF(pPathCpy);
        return FALSE;
      }
    }
  }

  if (nBlackList == IDIAGMSM_BLACKLIST_WRITE) {
    for (nIdx = 0; nIdx < IDIAGMSM_WRITEBLACKLIST_LENGTH; nIdx++) {
      if (STRBEGINS(DiagMSM_File_WritePathBlackList[nIdx], pPathCpy) == TRUE) {
        FREEIF(pPathCpy);
        return FALSE;
      }
    }
  }

  FREEIF(pPathCpy);
  return TRUE;
}

// Locked functionality will only be supported in BREW 3.x
#if (MIN_BREW_VERSION(3,0) && defined(FEATURE_BNMAPP_STRIPPED))
/*=========================================================================*/
static uint32
DiagMSM_QueryRsp_Locked (IDiagMSM *pDiagMSM, PFNIDIAGMSM_DLV_RSP pfnPktDeliver, 
                  void *pUd)
{
  PARAM_NOT_REF(pDiagMSM)
  PARAM_NOT_REF(pfnPktDeliver)
  PARAM_NOT_REF(pUd)
  PARAM_NOT_CONST(pDiagMSM)
  PARAM_NOT_CONST(pfnPktDeliver)
  PARAM_NOT_CONST(pUd)

  (void) MSG (MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_QueryRsp: This interface is locked");

  return EPRIVLEVEL;
}
/*=========================================================================*/
static uint32
DiagMSM_SendReq_Locked (IDiagMSM *pDiagMSM, IDiagMSMReqType *pReq, 
                        void *pUserData)
{
  PARAM_NOT_REF(pDiagMSM)
  PARAM_NOT_REF(pReq)
  PARAM_NOT_REF(pUserData)
  PARAM_NOT_CONST(pDiagMSM)
  PARAM_NOT_CONST(pReq)
  PARAM_NOT_CONST(pUserData)

  (void) MSG (MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_SendReq: This interface is locked");

  return EPRIVLEVEL;
}
/*=========================================================================*/
static uint32 
DiagMSM_InitListener_Locked (DiagMSM *pMe, 
                             IDiagMSMListenerType *pListener)
{
  PARAM_NOT_REF(pMe)
  PARAM_NOT_REF(pListener)
  PARAM_NOT_CONST(pMe)
  PARAM_NOT_CONST(pListener)

  (void) MSG (MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_InitListener: This interface is locked");

  return EPRIVLEVEL;
}
/*=========================================================================*/

static IDiagMSMLHandleType 
DiagMSM_AddListener_Locked (DiagMSM *pMe, 
                            IDiagMSMListenerType *pListener)
{
  PARAM_NOT_REF(pMe)
  PARAM_NOT_REF(pListener)
  PARAM_NOT_CONST(pMe)
  PARAM_NOT_CONST(pListener)

  (void) MSG (MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_AddListener: This interface is locked");

  return IDIAGMSM_LHANDLE_NULL;
}
/*=========================================================================*/
static uint32 
DiagMSM_TraverseListeners_Locked (IDiagMSM *pDiagMSM, 
                                  PFNIDIAGMSM_LCB pfnDeliver, 
                                  void *pUD)
{
  PARAM_NOT_REF(pDiagMSM)
  PARAM_NOT_REF(pfnDeliver)
  PARAM_NOT_REF(pUD)
  PARAM_NOT_CONST(pDiagMSM)
  PARAM_NOT_CONST(pfnDeliver)
  PARAM_NOT_CONST(pUD)

  (void) MSG (MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_TraverseListeners: This interface is locked");

  return EPRIVLEVEL;
}
/*=========================================================================*/
static uint32 
DiagMSM_RemoveListener_Locked (DiagMSM *pMe,
                               IDiagMSMLHandleType mHandle)
{
  PARAM_NOT_REF(pMe)
  PARAM_NOT_REF(mHandle)
  PARAM_NOT_CONST(pMe)
  PARAM_NOT_CONST(mHandle)

  (void) MSG (MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_RemoveListener: This interface is locked");

  return EPRIVLEVEL;
}
/*=========================================================================*/
static uint32 
DiagMSM_ServiceListener_Locked (IDiagMSM *pDiagMSM, 
                                IDiagMSMLHandleType mHandle, 
                                PFNIDIAGMSM_LSVC pfnSvc, 
                                void *pUd)
{
  PARAM_NOT_REF(pDiagMSM)
  PARAM_NOT_REF(mHandle)
  PARAM_NOT_REF(pfnSvc)
  PARAM_NOT_REF(pUd)
  PARAM_NOT_CONST(pDiagMSM)
  PARAM_NOT_CONST(mHandle)
  PARAM_NOT_CONST(pfnSvc)
  PARAM_NOT_CONST(pUd)

  (void) MSG (MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_ServiceListener: This interface is locked");

  return EPRIVLEVEL;
}
/*=========================================================================*/
static uint32 
DiagMSM_ServiceOFlowBuf_Locked (IDiagMSM *pDiagMSM, 
                                IDiagMSMLHandleType mHandle, 
                                PFNIDIAGMSM_LSVC pfnSvc, 
                                void *pUd)
{
  PARAM_NOT_REF(pDiagMSM)
  PARAM_NOT_REF(mHandle)
  PARAM_NOT_REF(pfnSvc)
  PARAM_NOT_REF(pUd)
  PARAM_NOT_CONST(pDiagMSM)
  PARAM_NOT_CONST(mHandle)
  PARAM_NOT_CONST(pfnSvc)
  PARAM_NOT_CONST(pUd)

  (void) MSG (MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_ServiceOFlowBuf: This interface is locked");

  return EPRIVLEVEL;
}
/*=========================================================================*/

static IDiagMSMRecordType * 
DiagMSM_RecAlloc_Locked (IDiagMSM *pDiagMSM, 
                         IDiagMSMLHandleType mHandle, 
                         uint32 dwSize)
{
  PARAM_NOT_REF(pDiagMSM)
  PARAM_NOT_REF(mHandle)
  PARAM_NOT_REF(dwSize)
  PARAM_NOT_CONST(pDiagMSM)
  PARAM_NOT_CONST(mHandle)
  PARAM_NOT_CONST(dwSize)

  (void) MSG (MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_RecAlloc: This interface is locked");

  return NULL;
}
/*=========================================================================*/
static uint32 
DiagMSM_RecCommit_Locked (IDiagMSM *pDiagMSM, 
                          IDiagMSMLHandleType mHandle, 
                          IDiagMSMRecordType *pRec)
{
  PARAM_NOT_REF(pDiagMSM)
  PARAM_NOT_REF(mHandle)
  PARAM_NOT_REF(pRec)
  PARAM_NOT_CONST(pDiagMSM)
  PARAM_NOT_CONST(mHandle)
  PARAM_NOT_CONST(pRec)

  (void) MSG (MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_RecCommit: This interface is locked");

  return EPRIVLEVEL;
}
/*=========================================================================*/
static uint32 
DiagMSM_RecShorten_Locked (IDiagMSM *pDiagMSM, 
                           IDiagMSMLHandleType mHandle, 
                           IDiagMSMRecordType *pRec, 
                           uint32 dwNewSize)
{
  PARAM_NOT_REF(pDiagMSM)
  PARAM_NOT_REF(mHandle)
  PARAM_NOT_REF(pRec)
  PARAM_NOT_REF(dwNewSize)
  PARAM_NOT_CONST(pDiagMSM)
  PARAM_NOT_CONST(mHandle)
  PARAM_NOT_CONST(pRec)
  PARAM_NOT_CONST(dwNewSize)

  (void) MSG (MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_RecShorten: This interface is locked");

  return EPRIVLEVEL;
}
/*=========================================================================*/

static uint32 
DiagMSM_GetListenerInfo_Locked (IDiagMSM *pDiagMSM, 
                                IDiagMSMLHandleType mHandle, 
                                IDiagMSMListenerType *pCfg)
{
  PARAM_NOT_REF(pDiagMSM)
  PARAM_NOT_REF(mHandle)
  PARAM_NOT_REF(pCfg)
  PARAM_NOT_CONST(pDiagMSM)
  PARAM_NOT_CONST(mHandle)
  PARAM_NOT_CONST(pCfg)

  (void) MSG (MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_GetListenerInfo: This interface is locked");

  return EPRIVLEVEL;
}
/*=========================================================================*/
static int32 
DiagMSM_BufUsage_Locked (IDiagMSM *pDiagMSM, 
                                   IDiagMSMLHandleType mHandle)
{
  PARAM_NOT_REF(pDiagMSM)
  PARAM_NOT_REF(mHandle)
  PARAM_NOT_REF(pCfg)
  PARAM_NOT_CONST(pDiagMSM)
  PARAM_NOT_CONST(mHandle)
  PARAM_NOT_CONST(pCfg)

  (void) MSG (MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_BufUsage: This interface is locked");

  return EPRIVLEVEL;
}
/*=========================================================================*/
static uint32 
DiagMSM_GetOFlowOpt_Locked (IDiagMSM *pDiagMSM, 
                            IDiagMSMLHandleType mHandle, 
                            uint8 *pOpt)
{
  PARAM_NOT_REF(pDiagMSM)
  PARAM_NOT_REF(mHandle)
  PARAM_NOT_REF(pOpt)
  PARAM_NOT_CONST(pDiagMSM)
  PARAM_NOT_CONST(mHandle)
  PARAM_NOT_CONST(pOpt)

  (void) MSG (MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_GetOFlowOpt: This interface is locked");

  return EPRIVLEVEL;
}
/*=========================================================================*/
static uint32 
DiagMSM_GetLThreshold_Locked ( IDiagMSM *pDiagMSM, 
                               IDiagMSMLHandleType mHandle, 
                               int32 *pThreshold)
{
  PARAM_NOT_REF(pDiagMSM)
  PARAM_NOT_REF(mHandle)
  PARAM_NOT_REF(pThreshold)
  PARAM_NOT_CONST(pDiagMSM)
  PARAM_NOT_CONST(mHandle)
  PARAM_NOT_CONST(pThreshold)

  (void) MSG (MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_GetLThreshold: This interface is locked");

  return EPRIVLEVEL;
}
/*=========================================================================*/
static uint32 
DiagMSM_GetLimit_Locked (IDiagMSM *pDiagMSM, 
                         IDiagMSMLHandleType mHandle, 
                         int32 *pLimit)
{
  PARAM_NOT_REF(pDiagMSM)
  PARAM_NOT_REF(mHandle)
  PARAM_NOT_REF(pLimit)
  PARAM_NOT_CONST(pDiagMSM)
  PARAM_NOT_CONST(mHandle)
  PARAM_NOT_CONST(pLimit)

  (void) MSG (MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_GetLimit: This interface is locked");

  return EPRIVLEVEL;
}
/*=========================================================================*/
static uint32 
DiagMSM_GetSize_Locked (IDiagMSM *pDiagMSM, 
                        IDiagMSMLHandleType mHandle, 
                        uint32 *pSize)
{
  PARAM_NOT_REF(pDiagMSM)
  PARAM_NOT_REF(mHandle)
  PARAM_NOT_REF(pSize)
  PARAM_NOT_CONST(pDiagMSM)
  PARAM_NOT_CONST(mHandle)
  PARAM_NOT_CONST(pSize)

  (void) MSG (MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_GetSize: This interface is locked");

  return EPRIVLEVEL;
}
/*=========================================================================*/
static uint32 
DiagMSM_SetOFlowOpt_Locked (IDiagMSM *pDiagMSM, 
                            IDiagMSMLHandleType mHandle, 
                            uint8 eOpt)
{
  PARAM_NOT_REF(pDiagMSM)
  PARAM_NOT_REF(mHandle)
  PARAM_NOT_REF(eOpt)
  PARAM_NOT_CONST(pDiagMSM)
  PARAM_NOT_CONST(mHandle)
  PARAM_NOT_CONST(eOpt)

  (void) MSG (MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_SetOFlowOpt: This interface is locked");

  return EPRIVLEVEL;
}
/*=========================================================================*/
static uint32 
DiagMSM_SetLThreshold_Locked (IDiagMSM *pDiagMSM, 
                              IDiagMSMLHandleType mHandle, 
                              int32 dwThreshold)
{
  PARAM_NOT_REF(pDiagMSM)
  PARAM_NOT_REF(mHandle)
  PARAM_NOT_REF(dwThreshold)
  PARAM_NOT_CONST(pDiagMSM)
  PARAM_NOT_CONST(mHandle)
  PARAM_NOT_CONST(dwThreshold)

  (void) MSG (MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_SetLThreshold: This interface is locked");

  return EPRIVLEVEL;
}
/*=========================================================================*/
static uint32 
DiagMSM_SetLimit_Locked (IDiagMSM *pDiagMSM, 
                         IDiagMSMLHandleType mHandle, 
                         int32 dwLimit)
{
  PARAM_NOT_REF(pDiagMSM)
  PARAM_NOT_REF(mHandle)
  PARAM_NOT_REF(dwLimit)
  PARAM_NOT_CONST(pDiagMSM)
  PARAM_NOT_CONST(mHandle)
  PARAM_NOT_CONST(dwLimit)

  (void) MSG (MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_SetLimit: This interface is locked");

  return EPRIVLEVEL;
}
/*=========================================================================*/
static uint32
DiagMSM_CreateHMAC_Locked (DiagMSM *pMe, byte *pMsg, uint16 nMsgLen,
                           byte *pKey, uint16 nKeyLen, byte *pDigest, 
                           int *nDigestSz)
{
  PARAM_NOT_REF(pMe)
  PARAM_NOT_REF(pMsg)
  PARAM_NOT_REF(nMsgLen)
  PARAM_NOT_REF(pKey)
  PARAM_NOT_REF(nKeyLen)
  PARAM_NOT_REF(pDigest)
  PARAM_NOT_REF(nDigestSz)
  PARAM_NOT_CONST(pMe)
  PARAM_NOT_CONST(pMsg)
  PARAM_NOT_CONST(nMsgLen)
  PARAM_NOT_CONST(pKey)
  PARAM_NOT_CONST(nKeyLen)
  PARAM_NOT_CONST(pDigest)
  PARAM_NOT_CONST(nDigestSz)

  (void) MSG (MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_CreateHMAC: This interface is locked");

  return EPRIVLEVEL;
}
/*=========================================================================*/
static IDiagMSMFile
DiagMSM_File_Open_Locked(IDiagMSM *pDiagMSM, const char * pPath, int oFlag, 
                         uint16 nMode)
{
  PARAM_NOT_REF(pDiagMSM)
  PARAM_NOT_REF(pPath)
  PARAM_NOT_REF(oFlag)
  PARAM_NOT_REF(nMode)
  PARAM_NOT_CONST(pDiagMSM)
  PARAM_NOT_CONST(pPath)
  PARAM_NOT_CONST(oFlag)
  PARAM_NOT_CONST(nMode)

  (void) MSG (MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_File_Open: This interface is locked");

  return IDIAGMSM_FILE_ERROR;
}
/*=========================================================================*/
static uint32
DiagMSM_File_Close_Locked(IDiagMSM *pDiagMSM, IDiagMSMFile pHandle)
{
  PARAM_NOT_REF(pDiagMSM)
  PARAM_NOT_REF(pHandle)
  PARAM_NOT_CONST(pDiagMSM)
  PARAM_NOT_CONST(pHandle)

  (void) MSG (MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_File_Close: This interface is locked");

  return EPRIVLEVEL;
}
/*=========================================================================*/
static int32
DiagMSM_File_Read_Locked(IDiagMSM *pDiagMSM, IDiagMSMFile handle, void * pBuf, 
                         int32 nBufSz)
{
  PARAM_NOT_REF(pDiagMSM)
  PARAM_NOT_REF(handle)
  PARAM_NOT_REF(pBuf)
  PARAM_NOT_REF(nBufSz)
  PARAM_NOT_CONST(pDiagMSM)
  PARAM_NOT_CONST(handle)
  PARAM_NOT_CONST(pBuf)
  PARAM_NOT_CONST(nBufSz)

  (void) MSG (MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_File_Read: This interface is locked");

  return IDIAGMSM_FILE_ERROR;
}
/*=========================================================================*/
static int32
DiagMSM_File_Seek_Locked(IDiagMSM *pDiagMSM, IDiagMSMFile handle, int32 nOffset,
                         int nWhence)
{
  PARAM_NOT_REF(pDiagMSM)
  PARAM_NOT_REF(handle)
  PARAM_NOT_REF(nOffset)
  PARAM_NOT_REF(nWhence)
  PARAM_NOT_CONST(pDiagMSM)
  PARAM_NOT_CONST(handle)
  PARAM_NOT_CONST(nOffset)
  PARAM_NOT_CONST(nWhence)

  (void) MSG (MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_File_Seek: This interface is locked");

  return IDIAGMSM_FILE_ERROR;
}
/*=========================================================================*/
static uint32
DiagMSM_File_Truncate_Locked(IDiagMSM *pDiagMSM, const char * pPath, 
                             int32 nOffset)
{
  PARAM_NOT_REF(pDiagMSM)
  PARAM_NOT_REF(pPath)
  PARAM_NOT_REF(nOffset)
  PARAM_NOT_CONST(pDiagMSM)
  PARAM_NOT_CONST(pPath)
  PARAM_NOT_CONST(nOffset)

  (void) MSG (MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_File_Truncate: This interface is locked");

  return EPRIVLEVEL;
}
/*=========================================================================*/
static int32
DiagMSM_File_Write_Locked(IDiagMSM *pDiagMSM, IDiagMSMFile handle, 
                          const void * pBuf, int32 nBufSz)
{
  PARAM_NOT_REF(pDiagMSM)
  PARAM_NOT_REF(handle)
  PARAM_NOT_REF(pBuf)
  PARAM_NOT_REF(nBufSz)
  PARAM_NOT_CONST(pDiagMSM)
  PARAM_NOT_CONST(handle)
  PARAM_NOT_CONST(pBuf)
  PARAM_NOT_CONST(nBufSz)

  (void) MSG (MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_File_Write: This interface is locked");

  return IDIAGMSM_FILE_ERROR;
}
/*=========================================================================*/
static uint32
DiagMSM_File_MkDir_Locked(IDiagMSM *pDiagMSM, const char * pPath, uint16 nMode)
{
  PARAM_NOT_REF(pDiagMSM)
  PARAM_NOT_REF(pPath)
  PARAM_NOT_REF(nMode)
  PARAM_NOT_CONST(pDiagMSM)
  PARAM_NOT_CONST(pPath)
  PARAM_NOT_CONST(nMode)

  (void) MSG (MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_File_MkDir: This interface is locked"); 
  return EPRIVLEVEL;
}
/*=========================================================================*/
static uint32
DiagMSM_File_Rename_Locked(IDiagMSM *pDiagMSM, const char * pOldPath, 
                    const char * pNewPath)
{ 
  PARAM_NOT_REF(pDiagMSM)
  PARAM_NOT_REF(pOldPath)
  PARAM_NOT_REF(pNewPath)
  PARAM_NOT_CONST(pDiagMSM)
  PARAM_NOT_CONST(pOldPath)
  PARAM_NOT_CONST(pNewPath)

  (void) MSG (MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_File_Rename: This interface is locked"); 
  return EPRIVLEVEL;
} 
/*=========================================================================*/
static uint32
DiagMSM_File_RmDir_Locked(IDiagMSM *pDiagMSM, const char * pPath)
{
  PARAM_NOT_REF(pDiagMSM)
  PARAM_NOT_REF(pPath)
  PARAM_NOT_CONST(pDiagMSM)
  PARAM_NOT_CONST(pPath)

  (void) MSG (MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_File_RmDir: This interface is locked"); 
  return EPRIVLEVEL;
}
/*=========================================================================*/
static uint32
DiagMSM_File_Stat_Locked(IDiagMSM *pDiagMSM, const char *pPath, 
                         IDiagMSMFileStat * pBuf)
{
  PARAM_NOT_REF(pDiagMSM)
  PARAM_NOT_REF(pPath)
  PARAM_NOT_REF(pBuf)
  PARAM_NOT_CONST(pDiagMSM)
  PARAM_NOT_CONST(pPath)
  PARAM_NOT_CONST(pBuf)

  (void) MSG (MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_File_Stat: This interface is locked"); 
  return EPRIVLEVEL;
}
/*=========================================================================*/
static uint32
DiagMSM_File_Remove_Locked(IDiagMSM *pDiagMSM, const char *pPath)
{
  PARAM_NOT_REF(pDiagMSM)
  PARAM_NOT_REF(pPath)
  PARAM_NOT_CONST(pDiagMSM)
  PARAM_NOT_CONST(pPath)

  (void) MSG (MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_File_Remove: This interface is locked");
  return EPRIVLEVEL;
}
/*=========================================================================*/
static IDiagMSM_EFSDIR *
DiagMSM_File_OpenDir_Locked(IDiagMSM *pDiagMSM, const char *pPath)
{
  PARAM_NOT_REF(pDiagMSM)
  PARAM_NOT_REF(pPath)
  PARAM_NOT_CONST(pDiagMSM)
  PARAM_NOT_CONST(pPath)

  (void) MSG (MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_File_OpenDir: This interface is locked");
  return NULL;
} 
/*=========================================================================*/
static IDiagMSMDirEnt *
DiagMSM_File_ReadDir_Locked(IDiagMSM *pDiagMSM, const IDiagMSM_EFSDIR *pDir)
{
  PARAM_NOT_REF(pDiagMSM)
  PARAM_NOT_REF(pDir)
  PARAM_NOT_CONST(pDiagMSM)
  PARAM_NOT_CONST(pDir)

  (void) MSG (MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_File_OpenDir: This interface is locked");
  return NULL;
}
/*=========================================================================*/
static uint32
DiagMSM_File_CloseDir_Locked(IDiagMSM *pDiagMSM, const IDiagMSM_EFSDIR *pDir)
{
  PARAM_NOT_REF(pDiagMSM)
  PARAM_NOT_REF(pDir)
  PARAM_NOT_CONST(pDiagMSM)
  PARAM_NOT_CONST(pDir)

  (void) MSG (MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_File_CloseDir: This interface is locked");
  return EPRIVLEVEL;
}
/*=========================================================================*/
static int
DiagMSM_File_GetLastError_Locked(IDiagMSM *pDiagMSM) 
{
  PARAM_NOT_REF(pDiagMSM)
  PARAM_NOT_CONST(pDiagMSM)

  (void) MSG (MSG_SSID_OEMDIAG, OEMDIAG_MSG_NOTIFY,
              "DiagMSM_File_GetLastError: This interface is locked");
  return EPRIVLEVEL;
}
#endif // MIN_BREW_VERSION(3,0) && FEATURE_BNMAPP_STRIPPED
