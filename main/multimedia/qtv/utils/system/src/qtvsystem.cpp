/* =======================================================================
                               qtvsystem.c
DESCRIPTION
   This file provides definitions for the QTV Tasks.

EXTERNALIZED FUNCTIONS
  List functions and a brief description that are exported from this file

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Detail how to initialize and use this service.  The sequencing aspect
  is only needed if the order of operations is important.

Portions copyrighted by PacketVideo Corporation;
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated;
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/system/main/latest/src/qtvsystem.cpp#5 $
$DateTime: 2009/02/11 04:24:59 $
$Change: 838655 $

========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
#include <comdef.h>  // common type definitions
#include <stdlib.h>
#include <rex.h>
#include <task.h>
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#ifndef FEATURE_WINCE
#include "OEMFeatures.h"
#endif
#include "qtvsystem.h"


#ifdef FEATURE_QTV_IMEM 
#error code not present
#endif

#if( defined(FEATURE_QTV_MEMORY_LEAK_DEBUG) || defined (FEATURE_QTV_MEMORY_DEBUG) )
# include "qtv_msg.h"
#endif /* FEATURE_QTV_MEMORY_LEAK_DEBUG */

#ifdef FEATURE_BREW_3_0
# include "AEE_OEMHeap.h"
# include "OEMHeap.h"
#else
#ifndef FEATURE_WINCE
# include "AEE_OEM.h"
#endif
#endif /* FEATURE_BREW_3_0 */

#ifdef FEATURE_QTV_IMEM 
#error code not present
#endif

/* ==========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/*********************************
*
* Task Priorities
*
**********************************/
#ifdef TIMETEST
// Minimum QTV Priority (not including the decoder thread)
// This value is used by timetest builds to determine
// which LEDs to use for each QTV thread
#define QTV_BASE_PRIORITY QTVParserPriority
#endif

//  The player thread should be highest so it can interrupt playback.
//  Any decode threads should be lowest since they can hog the CPU.
/*
** MPEG4_PV_TASK1_PRI_ORDER is used by vdec decoders which need a high-priority
** task to feed (and otherwise interact with) the DSP.  Check headers in
** services/qtv/dec for this task definition.
*/
rex_priority_type QTVDSPTaskPriority    = TASK_PRIORITY( MPEG4_PV_TASK1_PRI_ORDER );
rex_priority_type QTVPlayerPriority     = TASK_PRIORITY( MPEG4_PV_TASK2_PRI_ORDER );
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
rex_priority_type QTVTimedTextPriority  = TASK_PRIORITY( MPEG4_PV_TASK3_PRI_ORDER );
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
/***
** renderer task is now created/destroyed from services/task.
**
** rex_priority_type QTVRendererPriority   = TASK_PRIORITY( MPEG4_PV_TASK4_PRI_ORDER );
***/
rex_priority_type QTVAudioPriority      = TASK_PRIORITY( MPEG4_PV_TASK5_PRI_ORDER );
rex_priority_type QTVStreamerPriority   = TASK_PRIORITY( MPEG4_PV_TASK6_PRI_ORDER );

#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)||defined (FEATURE_QTV_HTTP_DOWNLOAD)
rex_priority_type QTVDownloaderPriority = TASK_PRIORITY( MPEG4_PV_TASK6_PRI_ORDER );
#endif
//  For the broadcast task to guarantee delivery of events in a timely fashion
//  its priority should be higher than all tasks it delivers events to -
//  potentially all Qtv tasks. However, since the use of broadcasts is currently
//  limited (to MediaFLO) the safer option is to place it at the lowest priority at
//  which it gives acceptable performance for its current clients. The priority of
//  the broadcast task should be revisited if/when its use is expanded.
rex_priority_type QTVBcastXmitPriority  = TASK_PRIORITY( MPEG4_PV_TASK7_PRI_ORDER );
rex_priority_type QTVDecoderPriority    = TASK_PRIORITY( MPEG4_PV_TASK8_PRI_ORDER );
rex_priority_type QTVParserPriority     = TASK_PRIORITY( MPEG4_PV_TASK9_PRI_ORDER );

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */
#ifdef FEATURE_QTV_MEMORY_LEAK_DEBUG
#error code not present
#endif /* FEATURE_QTV_MEMORY_LEAK_DEBUG */

/* Keep track of the total bytes allocated by Qtv.  This does not take into
 * account qtv_realloc calls unless FEATURE_QTV_MEMORY_LEAK_DEBUG is defined.
 */
static uint32        qtv_malloc_total_bytes = 0; 
int32 qtv_dummy_var;

#ifdef FEATURE_QTV_IMEM 
#error code not present
#endif


/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                            Function Definitions
** ======================================================================= */
#ifdef FEATURE_QTV_MEMORY_LEAK_DEBUG
#error code not present
#endif /* FEATURE_QTV_MEMORY_LEAK_DEBUG */


/* ======================================================================
FUNCTION 
  qtv_malloc

DESCRIPTION
  Allocate memory from the heap.

  This function should not be called directly from Qtv code.  Instead, call
  QTV_Malloc() To be independent of FEATURE_QTV_MEMORY_DEBUG.

DEPENDENCIES
  none

RETURN VALUE
  Returns a pointer to numBytes of memory or NULL if unable to allocate memory.

SIDE EFFECTS
  modifies tables to track memory leaks
  
========================================================================== */
void* qtv_malloc(uint32 numBytes, const char *file, uint16 line)
{
  void *pMemory;
#if( defined(FEATURE_QTV_MEMORY_DEBUG) || defined(FEATURE_QTV_MEMORY_LEAK_DEBUG) )
  const char *pFile = strrchr( file, '\\' );
  if(pFile == NULL)
  {
    pFile = strrchr(file, '/'); 
  } /* in case of linux, use forward slash */
  if(pFile == NULL) 
  { // if still null, print error msg, page fault about to ensue
    pFile = file; 
  }
  else
  {
    pFile++; 
  } /* Just past the last '\' */
#endif /* FEATURE_QTV_MEMORY_DEBUG || FEATURE_QTV_MEMORY_LEAK_DEBUG */
  
#ifdef T_WINNT
#error code not present
#endif

  /* Allocate the memory from the heap */
  pMemory = malloc(numBytes);

#ifdef T_WINNT
#error code not present
#endif

  if( pMemory != NULL )
  {
#ifdef FEATURE_QTV_MEMORY_LEAK_DEBUG
#error code not present
#endif /* FEATURE_QTV_MEMORY_LEAK_DEBUG */

#ifdef FEATURE_QTV_MEMORY_DEBUG
    QTV_MSG_SPRINTF_4( QTVDIAG_DEBUG, "[%s:%d] qtv_malloc(%d), %x",
                       pFile, line, numBytes, pMemory );
#endif /* FEATURE_QTV_MEMORY_DEBUG */
    
    /* Increment the number of bytes allocated by Qtv */
    qtv_malloc_total_bytes += numBytes;
  }
  
  return(pMemory);
}


#ifdef FEATURE_QTV_IMEM 
#error code not present
#endif 


/* ======================================================================
FUNCTION 
  qtv_realloc

DESCRIPTION
  Reallocate memory of a different size from the heap.  If possible, the memory
  will be in the same location.  Otherwise the old memory will be free'd and a
  new chunk allocated.

  This function should not be called directly from Qtv code.  Instead, call
  QTV_Realloc() To be independent of FEATURE_QTV_MEMORY_DEBUG.

DEPENDENCIES
  qtv_realloc will not update any of the byte counters (total, current, or max)
  unless FEATURE_QTV_MEMORY_LEAK_DEBUG is defined.
  
RETURN VALUE
  Returns a pointer to numBytes of memory or NULL if unable to allocate memory.

SIDE EFFECTS
  modifies tables to track memory leaks
  
========================================================================== */
void* qtv_realloc(void *pOldMemory, uint32 numBytes, const char *file, uint16 line)
{
  void *pMemory;
#if( defined(FEATURE_QTV_MEMORY_DEBUG) || defined(FEATURE_QTV_MEMORY_LEAK_DEBUG) )
  const char *pFile = strrchr( file, '\\' );
  if(pFile == NULL) 
  {
    pFile = strrchr(file, '/'); 
  } /* in case of linux, use forward slash */
  if(pFile == NULL) 
  { // if still null, print error msg, page fault about to ensue
    pFile = file;
  }
  else 
  {
    pFile++; 
  } /* Just past the last '\' */
#endif /* FEATURE_QTV_MEMORY_DEBUG || FEATURE_QTV_MEMORY_LEAK_DEBUG */

#ifdef FEATURE_QTV_MEMORY_LEAK_DEBUG
#error code not present
#endif /* FEATURE_QTV_MEMORY_LEAK_DEBUG */

#ifdef T_WINNT
#error code not present
#endif

  /* Reallocate the memory */
  pMemory = realloc( pOldMemory, numBytes );
  
#ifdef T_WINNT
#error code not present
#endif

#ifdef FEATURE_QTV_MEMORY_LEAK_DEBUG
#error code not present
#endif /* FEATURE_QTV_MEMORY_LEAK_DEBUG */

#ifdef FEATURE_QTV_MEMORY_DEBUG
  QTV_MSG_SPRINTF_5( QTVDIAG_DEBUG, "[%s:%d], qtv_realloc(%x,%d), %x",
                     pFile, line, pMemory, numBytes, pMemory ); /*lint!e449 */
#endif /* FEATURE_QTV_MEMORY_DEBUG */

  return(pMemory);
}

/* ======================================================================
FUNCTION 
  qtv_free

DESCRIPTION
  Free memory back to the heap to be reused later.

  This function should not be called directly from Qtv code.  Instead, call
  QTV_Free() To be independent of FEATURE_QTV_MEMORY_DEBUG.

DEPENDENCIES
  This memory must have been allocated by a call to QTV_Malloc.

RETURN VALUE
  none

SIDE EFFECTS
  modifies tables to track memory leaks
  
========================================================================== */
void qtv_free(void* pMemory, const char* file, uint16 line)
{
#ifdef FEATURE_QTV_MEMORY_DEBUG
  const char *pFile = strrchr( file, '\\' );
  if(pFile == NULL) 
  {
    pFile = strrchr(file, '/'); 
  } /* in case of linux, use forward slash */
  if(pFile == NULL) 
  { // if still null, print error msg, page fault about to ensue
    pFile = file;
  }
  else 
  {
    pFile++; 
  } /* Just past the last '\' */
#endif /* FEATURE_QTV_MEMORY_DEBUG */

#ifdef FEATURE_QTV_MEMORY_LEAK_DEBUG
#error code not present
#endif /* FEATURE_QTV_MEMORY_LEAK_DEBUG */

#ifdef T_WINNT
#error code not present
#endif

  /* Free the memory */
  free(pMemory);

#ifdef T_WINNT
#error code not present
#endif
  QTV_USE_ARG1(line);

}

/* ======================================================================
FUNCTION 
  qtv_new

DESCRIPTION
  Allocate memory from the heap.  This function replaces the standard C++ 'new'
  operator in order to track memory usage by Qtv.

  This function should not be called directly from Qtv code.  Instead, call
  QTV_New() To be independent of FEATURE_QTV_MEMORY_DEBUG.

DEPENDENCIES
  This memory must have been allocated by a call to QTV_Malloc.

RETURN VALUE
  none

SIDE EFFECTS
  modifies tables to track memory leaks
  
========================================================================== */
void *qtv_new(void* pMemory, uint32 numBytes, const char *file, uint16 line)
{
#if( defined(FEATURE_QTV_MEMORY_DEBUG) || defined(FEATURE_QTV_MEMORY_LEAK_DEBUG) )
  const char *pFile = strrchr( file, '\\' );
  if(pFile == NULL) 
  {
    pFile = strrchr(file, '/'); 
  } /* in case of linux, use forward slash */
  if(pFile == NULL)
  { // if still null, print error msg, page fault about to ensue
    pFile = file;
  }
  else 
  {
    pFile++; 
  } /* Just past the last '\' */
#endif /* FEATURE_QTV_MEMORY_DEBUG || FEATURE_QTV_MEMORY_LEAK_DEBUG */

  if( pMemory != NULL )
  {
#ifdef FEATURE_QTV_MEMORY_LEAK_DEBUG
#error code not present
#endif /* FEATURE_QTV_MEMORY_LEAK_DEBUG */

#ifdef FEATURE_QTV_MEMORY_DEBUG
    QTV_MSG_SPRINTF_4( QTVDIAG_DEBUG, "[%s:%d] qtv_new(%d), %x",
                       pFile, line, numBytes, pMemory );
#endif /* FEATURE_QTV_MEMORY_DEBUG */

    /* Increment the number of bytes allocated by Qtv */
    qtv_malloc_total_bytes += numBytes;
  }
    
  return(pMemory);
}

/* ======================================================================
FUNCTION 
  qtv_delete

DESCRIPTION
  Free memory back to the heap to be reused later.  This function assumes that
  the memory has already been deleted.  This function just keeps track of
  information for debugging memory errors.

  This function should not be called directly from Qtv code.  Instead, call
  QTV_Delete() to be independent of FEATURE_QTV_MEMORY_DEBUG.

DEPENDENCIES
  This memory must have been allocated by a call to QTV_Malloc.

RETURN VALUE
  none

SIDE EFFECTS
  modifies tables to track memory leaks
  
========================================================================== */
void qtv_delete(void* pMemory, const char *file, uint16 line)
{
#ifdef FEATURE_QTV_MEMORY_DEBUG
  const char *pFile = strrchr( file, '\\' );
  if(pFile == NULL)
  {
    pFile = strrchr(file, '/');
  } /* in case of linux, use forward slash */
  if(pFile == NULL)
  { // if still null, print error msg, page fault about to ensue
    pFile = file;
  }
  else
  {
    pFile++;
  } /* Just past the last '\' */
#endif /* FEATURE_QTV_MEMORY_DEBUG */

#ifdef FEATURE_QTV_MEMORY_LEAK_DEBUG
#error code not present
#endif /* FEATURE_QTV_MEMORY_LEAK_DEBUG */

#ifdef FEATURE_QTV_MEMORY_DEBUG
  QTV_MSG_SPRINTF_3( QTVDIAG_DEBUG, "[%s:%d] qtv_delete(%x)", pFile, line, pMemory );
#endif /* FEATURE_QTV_MEMORY_DEBUG */
}

/* ======================================================================
FUNCTION
  QTV_GetTotalBytesAllocated

DESCRIPTION
  Return the total number of bytes allocated via calls to qtv_malloc and
  qtv_new.  Basically a sum total of all bytes allocated.

DEPENDENCIES
  none 

RETURN VALUE
  Total bytes allocated via QTV_Malloc and QTV_New.

SIDE EFFECTS
  none
========================================================================== */
uint32 QTV_GetTotalBytesAllocated(void)
{
  return qtv_malloc_total_bytes;
}

/* ======================================================================
FUNCTION
  QTV_ClearBytesAllocated

DESCRIPTION
  Clear the total number of bytes allocated via calls to qtv_malloc and
  qtv_new.

DEPENDENCIES
  none 

RETURN VALUE
  none

SIDE EFFECTS
  none
========================================================================== */
uint32 QTV_ClearBytesAllocated(void)
{
  qtv_malloc_total_bytes = 0;
#ifdef FEATURE_QTV_MEMORY_LEAK_DEBUG
#error code not present
#endif /* FEATURE_QTV_MEMORY_LEAK_DEBUG*/
  return 0;
}

#ifdef FEATURE_QTV_MEMORY_LEAK_DEBUG
#error code not present
#endif /* FEATURE_QTV_MEMORY_LEAK_DEBUG */

#ifdef FEATURE_QTV_ASSERT
/* ======================================================================
FUNCTION
  QTV_AssertFailed

DESCRIPTION
  Empty function to place breakpoints in during de-bugging. Function is
  called before the program is halted / fatal message logged.

DEPENDENCIES
  none 

RETURN VALUE
  none

SIDE EFFECTS
  none
========================================================================== */
void QTV_AssertFailed(void) {
  return;
}

int qtv_assert_level = 0; /* 0 causes error fatal */
                          /* 1 causes mesg fatal  */
#endif /* FEATURE_QTV_ASSERT */

