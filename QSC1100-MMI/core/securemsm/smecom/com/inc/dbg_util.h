#ifndef DBG_UTIL_H
#define DBG_UTIL_H
/*============================================================================
dbg_util

ABSTRACT
   debug utilities.

DESCRIPTION
   Provides debug functions


Copyright (c) 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
=========================================================================== */

/*=========================================================================
                              Edit History

  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/com/inc/dbg_util.h#2 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
06/26/06   jay    Added a few more utility functions
06/21/06   ssm    Enable DEBUG_PRINT_ENABLE
03/02/06   gr     Initial Revision

=========================================================================== */

#include "customer.h"
#include "comdef.h"
#include "msg.h"
#ifdef UIONE_SDK
#include "AEEStdlib.h"
#endif
#ifndef DEBUG_PRINT_ENABLE
#define DEBUG_PRINT_ENABLE
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DEBUG_PRINT_ENABLE
  #define DBGPRINT_METADATA_FORMAT  "dbgprint %s:%d"
  #ifdef UIONE_SDK
    #define DBGPRINT DBGPRINTF
  #else
   #define DBGPRINT\
      dbgprint_format( msg_file, __LINE__), dbgprint
  #endif /* UIONE_SDK */
#else
  #define DBGPRINT dbgprint_dummy
#endif

#ifdef FEATURE_DRM_NO_BREW
  #define DBGPRINTF DBGPRINT
#endif

extern void dbgprint(const char * pFormat, ...);

extern void dbgprint_dummy(const char * pszFormat, ...);

extern void dbgprint_format( const char * pFilePath, int nLineNum);

extern void PrintBinary( uint8*  data, uint32 len  );

extern void PrintLargeData( uint8* data, uint32 len );

#ifdef __cplusplus
}
#endif

#endif /* DBG_UTIL_H */
