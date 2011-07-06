#ifndef _VDECODER_UTILS_H_
#define _VDECODER_UTILS_H_
/* =======================================================================

                          vdecoder_utils.h

DESCRIPTION
  Definitions of 'utility' functions used on both sides of the VDEC API.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright 2005 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/decoder/core/rel/2.0/inc/vdecoder_utils.h#6 $
$DateTime: 2009/10/13 07:25:38 $
$Change: 1051904 $

when       who      what, where, why
--------   ---      ---------------------------------------------------------
04/08/09    vs     Remove the dependencies of other modules.
--------   ---      ---------------------------------------------------------
========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */


#ifdef __cplusplus
extern "C"
{
#endif
#include <comdef.h>  // common type definitions
#include <rex.h>
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "err.h"
#include "AEEstd.h"
#ifdef __cplusplus
}
#endif


#include "vdecoder_types.h"
#include "TL_Types.h"

//////////////////////////////////////////////////////////////////////
//
// Min, Max macros
//
//////////////////////////////////////////////////////////////////////

#define VMAX(a,b) (((a)>(b)) ? (a) : (b))
#define VMIN(a,b) (((a)<(b)) ? (a) : (b))

//limit a to range (b,c)
#define VLIMIT(a,b,c) (VMIN(VMAX(a,b),c))

/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                        Macro Definitions
** ======================================================================= */
#ifdef __cplusplus
/* =======================================================================
**                        Function Declarations
** ======================================================================= */
/* ======================================================================
FUNCTION:
  ConvertDSPAudioToVdec

DESCRIPTION:
  Converts a MP4QDSP_ audio codec #define (from mp4codec.h) into a
  VDEC_AUDIO_CONFIG (from vdecoder_types.h)

PARAMETERS:
  const int mp4audio
    The MP4QDSP_ #define.

RETURN VALUE:
  VDEC_CONCURRENCY_CONFIG
    The corresponding vdec type.

SIDE EFFECTS:
  None.
===========================================================================*/
VDEC_CONCURRENCY_CONFIG ConvertDSPAudioToVdec( const int mp4audio );

/* ======================================================================
FUNCTION:
  ConvertVdecAudioToDSP

DESCRIPTION:
  Converts a VDEC_AUDIO_CONFIG (from vdecoder_types.h) audio codec
  into a MP4QDSP_ #define (from mp4codec.h)

PARAMETERS:
  const VDEC_CONCURRENCY_CONFIG vdecAudio
    The vdec audio type.

RETURN VALUE:
  int
    The MP4QDSP_ #define corresponding to the vdec type.

SIDE EFFECTS:
  none.
===========================================================================*/
int ConvertVdecAudioToDSP( const VDEC_CONCURRENCY_CONFIG vdecAudio );

/* ======================================================================
FUNCTION:
  ConvertVDLConcurrencyTypeToDSP

DESCRIPTION:
  Converts a VDEC_AUDIO_CONFIG (from vdecoder_types.h) audio codec
  into a VDL_AUDIO_TYPE #define (from VDL_types.h)

PARAMETERS:
  const VDEC_CONCURRENCY_CONFIG vdecAudio
    The vdec audio type.

RETURN VALUE:
  int
    The VDL_CONCURRENCY_TYPE #define corresponding to the vdec type.

SIDE EFFECTS:
  none.
===========================================================================*/
int ConvertVDLConcurrencyTypeToDSP( const VDEC_CONCURRENCY_CONFIG vdecAudio,VDEC_AUDIO_VTYPE audio_vtype);

/* ======================================================================
FUNCTION:
  ConvertStatsToVdecFrame

DESCRIPTION:
  Converts a DecodeStatsType (from mp4codec.h) into a VDEC_FRAME
  (from vdecoder_types.h)

PARAMETERS:
   const DecodeStatsType &stats_in
     The stats structure to convert
   VDEC_FRAME               &frame_out
     The VDEC_FRAME to populate!

RETURN VALUE:
  None.
SIDE EFFECTS:
  none.
===========================================================================*/
void ConvertStatsToVdecFrame
(
  const DecodeStatsType &stats_in,
  VDEC_FRAME               &frame_out
);

#ifndef PLATFORM_LTK
  log_vdec_video_bitstream_type* wait_for_bitstream_log_buffer( void );
#endif /* PLATFORM_LTK */
void log_bitstream_buffer( uint8 *pBitstream, uint32 numBytes );

#endif //__cplusplus

/*-----------------------------------------------------------------------------
      This part of code contain dynamic memory allcation declarations and mem leak tool             
------------------------------------------------------------------------------*/
/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C"
{
#endif

#ifdef FEATURE_QTV_MEMORY_LEAK_DEBUG
#error code not present
#endif /* FEATURE_QTV_MEMORY_LEAK_DEBUG */

/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */
extern int32 vdec_dummy_var;

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                          Macro Definitions
** ======================================================================= */

/*************************************************************************
 * The following macro definitions are the only dynamic memory allocation
 * routines that are to be used in vdec.
 ************************************************************************* */
  
/* =======================================================================
MACRO Vdec_Malloc

ARGS 
  n - the number of bytes to allocate

DESCRIPTION:
  This macro calls vdec_malloc in order to allocate memory from the heap and
  track memory allocations for debugging purposes.  A pointer to the allocated
  memory is returned.
========================================================================== */
#define Vdec_Malloc(n)       vdec_malloc((n), __FILE__, __LINE__)

/* =======================================================================
MACRO Vdec_Realloc

ARGS
  p - original pointer returned by Vdec_Malloc
  n - number of bytes to reallocate

DESCRIPTION:
  This macro calls vdec_realloc in order to allocate memory from the heap and
  track memory allocations for debugging purposes.  A pointer to the allocated
  memory is returned.
========================================================================== */
#define Vdec_Realloc(p,n)    vdec_realloc((p), (n), __FILE__, __LINE__)

/* =======================================================================
MACRO Vdec_Free

ARGS 
  p - pointer to memory which will be free'd

DESCRIPTION:
  This macro calls vdec_free in order to free memory from the heap and
  track memory allocations for debugging purposes.
========================================================================== */
#define Vdec_Free(p)         vdec_free((p), __FILE__, __LINE__)

/* =======================================================================
MACRO Vdec_New

ARGS 
  t - type of object to allocate and construct

DESCRIPTION:
  This macro calls the C++ 'new' operator in order to allocate and construct an
  object of the requested type.  Then vdec_new is called in order to track memory
  allocations for debugging purposes.  A pointer to the newly constructed object
  is returned.
========================================================================== */
#define Vdec_New(t)          ((t*)vdec_new(new t, sizeof(t), __FILE__, __LINE__))
  
/* =======================================================================
MACRO Vdec_New_Args

ARGS 
  t - type of object to allocate and construct
  a - list of arguments to pass to constructor.  MUST BE SURROUNDED BY
      PARENTHESES.

DESCRIPTION:
  This macro calls the C++ 'new' operator in order to allocate and construct an
  object of the requested type passing the arguments specified.  Then vdec_new is
  called in order to track memory allocations for debugging purposes.  A pointer
  to the newly constructed object is returned.
========================================================================== */
#define Vdec_New_Args(t,a)   ((t*)vdec_new(new t##a, sizeof(t), __FILE__, __LINE__))

/* =======================================================================
MACRO Vdec_New_Array

ARGS
  t - type of object to allocate and construct
  n - number of objects to allocate and construct

DESCRIPTION:
  This macro calls the C++ 'new' operator in order to allocate and construct the
  specified number of object of the requested type.  Then vdec_new is called in
  order to track memory allocations for debugging purposes.  A pointer to the
  newly constructed array of objects is returned.
========================================================================== */
#define Vdec_New_Array(t,n)  ((t*)vdec_new(new t[n], (n)*sizeof(t), __FILE__, __LINE__))

/* =======================================================================
MACRO Vdec_New_Array_Args

ARGS
  t - type of object to allocate and construct
  a - list of arguments to pass to constructor.  MUST BE SURROUNDED BY
      PARENTHESES.
  n - number of objects to allocate and construct

DESCRIPTION:
  This macro calls the C++ 'new' operator in order to allocate and construct the
  specified number of object of the requested type with the specified
  parameters.  Then vdec_new is called in order to track memory allocations for
  debugging purposes.  A pointer to the newly constructed array of objects is
  returned.
========================================================================== */
#define Vdec_New_Array_Args(t,a,n)  ((t*)vdec_new(new t##a##[n], (n)*sizeof(t), __FILE__, __LINE__))

/* =======================================================================
MACRO Vdec_Delete

ARGS 
  p - pointer to object which will be destructed and free'd

DESCRIPTION:
  This macro calls the C++ 'delete' operator in order to deconstruct and free
  the specified object.  Then vdec_delete is called in order to track memory
  allocations for debugging purposes.
========================================================================== */
#define Vdec_Delete(p)       vdec_delete((p), __FILE__, __LINE__);delete(p)
  
/* =======================================================================
MACRO Vdec_Delete_Array

ARGS 
  p - pointer to object which will be destructed and free'd

DESCRIPTION:
  This macro calls the C++ 'delete[]' operator in order to deconstruct and free
  the specified object.  Then vdec_delete is called in order to track memory
  allocations for debugging purposes.
========================================================================== */
#define Vdec_Delete_Array(p)  vdec_delete((p), __FILE__, __LINE__);delete[](p)
  
/* =======================================================================
MACRO Vdec_NULL_PTR_CHECK

ARGS
  ptr - the pointer
  retval - return value in case of NULL

DESCRIPTION:
  This macro checks the ptr value against NULL.  If ptr is NULL, then it
  returns retval.  If the function has return type of void, then use RETURN_VOID.
========================================================================== */
#define RETURN_VOID /* null macro */
#define Vdec_NULL_PTR_CHECK(ptr, retval)                          \
  if( NULL == (ptr) )                                            \
  {                                                              \
    VDEC_MSG_PRIO(VDECDIAG_GENERAL, VDECDIAG_PRIO_ERROR,            \
      "Unexpected Null Ptr");                                    \
    return retval;                                               \
  }
/* =======================================================================
MACRO BOUNDS_CHECK_FOR_PTR(

ARGS
  X - Pointer which is going for bound check
  Y - Pointer is maximum limit of X pointer
  Z - value which is going to increment in pointer X
  retval - value assign to retval

DESCRIPTION:
  This macro checks if pointer X going beyond pointer Y + length Z
========================================================================== */

#define BOUNDS_CHECK_FOR_PTR(X, Y, Z, retval)                    \
 if( ((X) >= ((Y) + sizeof(Y) - (Z))))                           \
 {                                                               \
   return retval;                                                \
 } 
  
  /* =======================================================================

MACRO VDEC_USE_ARG1

ARGS 
  arg1 - function argument parameter
  arg2 - function argument parameter
  

DESCRIPTION:
  This macro assign the function formal arguments to dummy variables for just to 
  fix compiler warning.

========================================================================== */
#define VDEC_USE_ARG1(arg1)           vdec_dummy_var = (int32)arg1;

#define VDEC_USE_ARG2(arg1,arg2)      vdec_dummy_var = (int32)arg1;\
                                     vdec_dummy_var = (int32)arg2;

/* =======================================================================
MACRO MYOBJ

ARGS
  xx_obj - this is the xx argument

DESCRIPTION:
  Complete description of what this macro does
========================================================================== */

/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/* ======================================================================
FUNCTION 
  vdec_malloc

DESCRIPTION
  Allocate memory from the heap.

  This function should not be called directly from Qtv code.  Instead, call
  Vdec_Malloc() To be independent of FEATURE_QTV_MEMORY_DEBUG.

DEPENDENCIES
  none

RETURN VALUE
  Returns a pointer to numBytes of memory or NULL if unable to allocate memory.

SIDE EFFECTS
  modifies tables to track memory leaks
  
========================================================================== */
void* vdec_malloc(uint32 numBytes, const char *file, uint16 line);

#ifdef FEATURE_QTV_IMEM 
#error code not present
#endif

/* ======================================================================
FUNCTION 
  vdec_realloc

DESCRIPTION
  Reallocate memory of a different size from the heap.  If possible, the memory
  will be in the same location.  Otherwise the old memory will be free'd and a
  new chunk allocated.

  This function should not be called directly from Qtv code.  Instead, call
  Vdec_Realloc() To be independent of FEATURE_QTV_MEMORY_DEBUG.

DEPENDENCIES
  vdec_realloc will not update any of the byte counters (total, current, or max)
  unless FEATURE_QTV_MEMORY_LEAK_DEBUG is defined.
  
RETURN VALUE
  Returns a pointer to numBytes of memory or NULL if unable to allocate memory.

SIDE EFFECTS
  modifies tables to track memory leaks
  
========================================================================== */
void* vdec_realloc(void *pOldMemory, uint32  numBytes, const char *file, uint16 line);

/* ======================================================================
FUNCTION 
  vdec_free

DESCRIPTION
  Free memory back to the heap to be reused later.

  This function should not be called directly from Qtv code.  Instead, call
  Vdec_Free() To be independent of FEATURE_QTV_MEMORY_DEBUG.

DEPENDENCIES
  This memory must have been allocated by a call to Vdec_Malloc.

RETURN VALUE
  none

SIDE EFFECTS
  modifies tables to track memory leaks
  
========================================================================== */
void  vdec_free(void* pMemory, const char *file, uint16 line);

/* ======================================================================
FUNCTION 
  vdec_new

DESCRIPTION
  Allocate memory from the heap.  This function replaces the standard C++ 'new'
  operator in order to track memory usage by Qtv.

  This function should not be called directly from Qtv code.  Instead, call
  Vdec_New() To be independent of FEATURE_QTV_MEMORY_DEBUG.

DEPENDENCIES
  This memory must have been allocated by a call to Vdec_Malloc.

RETURN VALUE
  none

SIDE EFFECTS
  modifies tables to track memory leaks
  
========================================================================== */
void *vdec_new(void* pMemory, uint32 numBytes, const char *file, uint16 line);

/* ======================================================================
FUNCTION 
  vdec_delete

DESCRIPTION
  Free memory back to the heap to be reused later.  This function replaces the
  standard C++ 'delete' operator in order to track memory usage by vdec.

  This function should not be called directly from Qtv code.  Instead, call
  Vdec_Delete() to be independent of FEATURE_QTV_MEMORY_DEBUG.

DEPENDENCIES
  This memory must have been allocated by a call to Vdec_Malloc.

RETURN VALUE
  none

SIDE EFFECTS
  modifies tables to track memory leaks
  
========================================================================== */
void vdec_delete(void* pMemory, const char *file, uint16 line);
  
#ifdef FEATURE_QTV_MEMORY_LEAK_DEBUG
#error code not present
#endif /* FEATURE_QTV_MEMORY_LEAK_DEBUG */
 
/* ======================================================================
FUNCTION
  Vdec_GetTotalBytesAllocated

DESCRIPTION
  Return the total number of bytes allocated via calls to vdec_malloc and
  vdec_new.  Basically a sum total of all bytes allocated.

DEPENDENCIES
  none 

RETURN VALUE
  Total bytes allocated via Vdec_Malloc and Vdec_New.

SIDE EFFECTS
  none
========================================================================== */
uint32 Vdec_GetTotalBytesAllocated(void);

/* ======================================================================
FUNCTION
  Vdec_ClearBytesAllocated

DESCRIPTION
  Clear the total number of bytes allocated via calls to vdec_malloc and
  vdec_new.

DEPENDENCIES
  none 

RETURN VALUE
  none

SIDE EFFECTS
  none
========================================================================== */
uint32 Vdec_ClearBytesAllocated(void);

#ifdef FEATURE_QTV_MEMORY_LEAK_DEBUG
#error code not present
#endif /* FEATURE_QTV_MEMORY_LEAK_DEBUG */

#ifdef FEATURE_QTV_ASSERT
/* ======================================================================
FUNCTION
  Vdec_AssertFailed

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
void Vdec_AssertFailed(void);
#endif /* FEATURE_QTV_ASSERT */
  
#ifdef __cplusplus
}
#endif /* __cplusplus */

#ifdef FEATURE_QTV_ASSERT
volatile extern int vdec_assert_level;

/* place a breakpoint in Vdec_AssertFailed for easy debugging. */
#define VDEC_ASSERT(a)  \
        { \
          /*lint -save -e506 */ \
          if ((a) == FALSE) \
          /*lint -restore */ \
          { \
            Vdec_AssertFailed(); \              
            if(vdec_assert_level == 0) \
            { \
               ERR_FATAL( "Assertion ", 0, 0, 0 ); \
            } \
            else \
            { \
               MSG_FATAL( "Assertion ", 0, 0, 0 ); \
            } \
          } \
        }
#else  /* FEATURE_QTV_ASSERT */
/* to completly compile the define out */
#define VDEC_ASSERT(a) { }
#endif /* FEATURE_QTV_ASSERT */

#ifndef VDEC_PERF_H
#define VDEC_PERF_H
/*===========================================================================

           VDEC   P E R F O R M A N C E   L I B R A R Y

DESCRIPTION
  Library to measure the performance of the QTV video decoding pipline.
  It can be easily added to new decoders as they are introduced.

NOTES
  The TimeTest port is used to output performance event identifiers.  If you
  see anomalous data appearing on the port it is possible that the wait
  states are too short for the port.

Copyright (c) 2005       by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/
#define QTV_ARMREGISTER_TIMING	//for Arm, Display and DSP timing

#ifdef FEATURE_CMI
#error code not present
#else

#define time_start_calculate(x)  clk_uptime_ms(x)
#define time_end_calculate(x, y, z) \
 clk_uptime_ms( y ); \
 qw_sub( y, y, x ); \
 (z) = ((uint32) qw_lo( y )) \

#endif /* FEATURE_CMI */

/*---------------------------------------------------------------------------
  Performance Event Identifiers are written here.  Use only the upper 8
  bits to make your identifier.  The lower 8 bits are used to create 
  channels under the event.
---------------------------------------------------------------------------*/
#define VDEC_PERF_TIMETEST_LEV_0       0x0000
#define VDEC_PERF_TIMETEST_LEV_1       0x0100

/* h264 oriented */
#define VDEC_PERF_EDEC_START           0x0200
#define VDEC_PERF_EDEC_STOP            0x0300

#define VDEC_PERF_SET_IFRAME           0x0400
#define VDEC_PERF_SET_PFRAME           0x0500
#define VDEC_PERF_SET_BFRAME           0x0600

//#define VDEC_PERF_RENDER_START         0x0700
//#define VDEC_PERF_RENDER_DROP          0x0800
//#define VDEC_PERF_RENDER_STOP          0x0900

#define VDEC_PERF_WATCH_START_ID       0x0a00
#define VDEC_PERF_WATCH_STOP_ID        0x0b00

#define VDEC_PERF_MB_TYPE_START        0x0c00
#define VDEC_PERF_MB_TYPE_STOP         0x0d00

#define VDEC_PERF_RESIDUAL_START       0x0e00
#define VDEC_PERF_RESIDUAL_STOP        0x0f00

/* mp4 oriented */
#define VDEC_PERF_ARM_FRAME_START      0x1000
#define VDEC_PERF_ARM_FRAME_STOP       0x1100

#define VDEC_PERF_ICMB_START           0x1200
#define VDEC_PERF_ICMB_STOP            0x1300
#define VDEC_PERF_PCMB_START           0x1400
#define VDEC_PERF_PCMB_STOP            0x1500
#define VDEC_PERF_IDP_START            0x1600
#define VDEC_PERF_IDP_STOP             0x1700
#define VDEC_PERF_PDP_START            0x1800
#define VDEC_PERF_PDP_STOP             0x1900

#define VDEC_PERF_DSP_FRAME_START      0x1a00
#define VDEC_PERF_DSP_FRAME_STOP       0x1b00

#define VDEC_PERF_DSP_SLICE_START      0x1c00
#define VDEC_PERF_DSP_SLICE_STOP       0x1d00

//#define VDEC_PERF_MDP_START            0x1e00
//#define VDEC_PERF_MDP_STOP             0x1f00
#define VDEC_PERF_MDP_DROP             0x2000

/* real oriented */
#define VDEC_PERF_DECODE_START         0x3000 /* decode input function start */
#define VDEC_PERF_DECODE_STOP          0x3100 /* decode input function return */
#define VDEC_PERF_FRAME_CB_START       0x3200 /* frame callback to app start */
#define VDEC_PERF_FRAME_CB_STOP        0x3300 /* frame callback to app return */


/* Bus Monitor */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
//#include "qtvInternalDefs.h"
#if defined (FEATURE_BUSMON) && defined (FEATURE_QTV_BUSMON)

#ifdef __cplusplus
extern "C" {
#endif
#include "bus_mon.h"
#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif


#define VDEC_PERF_BUS_MON_EVENT(event)                                      \
{                                                                          \
  switch( event & 0xFF00 )                                                 \
  {                                                                        \
    case VDEC_PERF_DSP_FRAME_START:                                         \
      bus_monitor_event_notify( BUS_MON_QTV_DSP_FRAME,                     \
                                BUS_MON_START_TRIGGER );                   \
      break;                                                               \
    case VDEC_PERF_DSP_FRAME_STOP:                                          \
      bus_monitor_event_notify( BUS_MON_QTV_DSP_FRAME,                     \
                                BUS_MON_STOP_TRIGGER );                    \
      break;                                                               \
    case VDEC_PERF_ARM_FRAME_START:                                         \
      bus_monitor_event_notify( BUS_MON_QTV_ARM_FRAME,                     \
                                BUS_MON_START_TRIGGER );                    \
      break;                                                               \
    case VDEC_PERF_ARM_FRAME_STOP:                                          \
      bus_monitor_event_notify( BUS_MON_QTV_ARM_FRAME,                     \
                                BUS_MON_STOP_TRIGGER );                    \
      break;                                                               \
#if 0
    case VDEC_PERF_MDP_START:                                               \
      bus_monitor_event_notify( BUS_MON_QTV_MDP_FRAME,                     \
                                BUS_MON_START_TRIGGER );                   \
      break;                                                               \

    case VDEC_PERF_MDP_STOP:                                                \
      bus_monitor_event_notify( BUS_MON_QTV_MDP_FRAME,                     \
                                BUS_MON_STOP_TRIGGER );                    \
      break;                                                               \
#endif
    default:                                                               \
      break;                                                               \
  }                                                                        \
}

#else /* defined (FEATURE_BUSMON) && defined (FEATURE_QTV_BUSMON) */

#define VDEC_PERF_BUS_MON_EVENT(event)

#endif /* defined (FEATURE_BUSMON) && defined (FEATURE_QTV_BUSMON) */


#ifdef FEATURE_QTV_PERF
#error code not present
#else /* FEATURE_QTV_PERF */

#define VDEC_PERF(event)                                                     \
{                                                                           \
  VDEC_PERF_BUS_MON_EVENT(event);                                            \
}

#endif /* FEATURE_QTV_PERF */

/*---------------------------------------------------------------------------
  Outputs the lower 8 bits of a frame's timestamp and an event identifier 
  to the port.
---------------------------------------------------------------------------*/
#define VDEC_PERF_FRAME_UPDATE(timestamp, event)                             \
{                                                                           \
  VDEC_PERF((uint16)( (event) | (timestamp & 0x00ff) ));         \
}

/*---------------------------------------------------------------------------
  Starts and stops the stopwatch timers.  There are 256 available timers.
---------------------------------------------------------------------------*/
#define VDEC_PERF_WATCH_START(watch_id)                                      \
{                                                                           \
  VDEC_PERF((uint16)(VDEC_PERF_WATCH_START_ID | (watch_id)));     \
}

#define VDEC_PERF_WATCH_STOP(watch_id)                                       \
{                                                                           \
  VDEC_PERF((uint16)(VDEC_PERF_WATCH_STOP_ID | (watch_id)));      \
}

#endif /* VDEC_PERF_H */

#endif /*_VDECODER_UTILS_H_*/

