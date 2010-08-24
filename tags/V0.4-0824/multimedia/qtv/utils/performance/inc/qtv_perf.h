#ifndef QTV_PERF_H
#define QTV_PERF_H
/*===========================================================================

           Q T V   P E R F O R M A N C E   L I B R A R Y

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
#define time_start_calculate(x) \
 clk_uptime_ms(x) \


#define time_end_calculate(x, y, z) \
 clk_uptime_ms( y ); \
 qw_sub( y, y, x ); \
 (z) = ((uint32) qw_lo( y )) \

/*---------------------------------------------------------------------------
  Performance Event Identifiers are written here.  Use only the upper 8
  bits to make your identifier.  The lower 8 bits are used to create 
  channels under the event.
---------------------------------------------------------------------------*/
#define QTV_PERF_TIMETEST_LEV_0       0x0000
#define QTV_PERF_TIMETEST_LEV_1       0x0100

/* h264 oriented */
#define QTV_PERF_EDEC_START           0x0200
#define QTV_PERF_EDEC_STOP            0x0300

#define QTV_PERF_SET_IFRAME           0x0400
#define QTV_PERF_SET_PFRAME           0x0500
#define QTV_PERF_SET_BFRAME           0x0600

#define QTV_PERF_RENDER_START         0x0700
#define QTV_PERF_RENDER_DROP          0x0800
#define QTV_PERF_RENDER_STOP          0x0900

#define QTV_PERF_WATCH_START_ID       0x0a00
#define QTV_PERF_WATCH_STOP_ID        0x0b00

#define QTV_PERF_MB_TYPE_START        0x0c00
#define QTV_PERF_MB_TYPE_STOP         0x0d00

#define QTV_PERF_RESIDUAL_START       0x0e00
#define QTV_PERF_RESIDUAL_STOP        0x0f00

/* mp4 oriented */
#define QTV_PERF_ARM_FRAME_START      0x1000
#define QTV_PERF_ARM_FRAME_STOP       0x1100

#define QTV_PERF_ICMB_START           0x1200
#define QTV_PERF_ICMB_STOP            0x1300
#define QTV_PERF_PCMB_START           0x1400
#define QTV_PERF_PCMB_STOP            0x1500
#define QTV_PERF_IDP_START            0x1600
#define QTV_PERF_IDP_STOP             0x1700
#define QTV_PERF_PDP_START            0x1800
#define QTV_PERF_PDP_STOP             0x1900

#define QTV_PERF_DSP_FRAME_START      0x1a00
#define QTV_PERF_DSP_FRAME_STOP       0x1b00

#define QTV_PERF_DSP_SLICE_START      0x1c00
#define QTV_PERF_DSP_SLICE_STOP       0x1d00

#define QTV_PERF_MDP_START            0x1e00
#define QTV_PERF_MDP_STOP             0x1f00
#define QTV_PERF_MDP_DROP             0x2000

/* real oriented */
#define QTV_PERF_DECODE_START         0x3000 /* decode input function start */
#define QTV_PERF_DECODE_STOP          0x3100 /* decode input function return */
#define QTV_PERF_FRAME_CB_START       0x3200 /* frame callback to app start */
#define QTV_PERF_FRAME_CB_STOP        0x3300 /* frame callback to app return */


/* Bus Monitor */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#if defined (FEATURE_BUSMON) && defined (FEATURE_QTV_BUSMON)

#ifdef __cplusplus
extern "C" {
#endif
#include "bus_mon.h"
#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif


#define QTV_PERF_BUS_MON_EVENT(event)                                      \
{                                                                          \
  switch( event & 0xFF00 )                                                 \
  {                                                                        \
    case QTV_PERF_DSP_FRAME_START:                                         \
      bus_monitor_event_notify( BUS_MON_QTV_DSP_FRAME,                     \
                                BUS_MON_START_TRIGGER );                   \
      break;                                                               \
    case QTV_PERF_DSP_FRAME_STOP:                                          \
      bus_monitor_event_notify( BUS_MON_QTV_DSP_FRAME,                     \
                                BUS_MON_STOP_TRIGGER );                    \
      break;                                                               \
    case QTV_PERF_ARM_FRAME_START:                                         \
      bus_monitor_event_notify( BUS_MON_QTV_ARM_FRAME,                     \
                                BUS_MON_START_TRIGGER );                    \
      break;                                                               \
    case QTV_PERF_ARM_FRAME_STOP:                                          \
      bus_monitor_event_notify( BUS_MON_QTV_ARM_FRAME,                     \
                                BUS_MON_STOP_TRIGGER );                    \
      break;                                                               \
    case QTV_PERF_MDP_START:                                               \
      bus_monitor_event_notify( BUS_MON_QTV_MDP_FRAME,                     \
                                BUS_MON_START_TRIGGER );                   \
      break;                                                               \
    case QTV_PERF_MDP_STOP:                                                \
      bus_monitor_event_notify( BUS_MON_QTV_MDP_FRAME,                     \
                                BUS_MON_STOP_TRIGGER );                    \
      break;                                                               \
    default:                                                               \
      break;                                                               \
  }                                                                        \
}

#else /* defined (FEATURE_BUSMON) && defined (FEATURE_QTV_BUSMON) */

#define QTV_PERF_BUS_MON_EVENT(event)

#endif /* defined (FEATURE_BUSMON) && defined (FEATURE_QTV_BUSMON) */


#ifdef FEATURE_QTV_PERF
#error code not present
#else /* FEATURE_QTV_PERF */

#define QTV_PERF(event)                                                     \
{                                                                           \
  QTV_PERF_BUS_MON_EVENT(event);                                            \
}

#endif /* FEATURE_QTV_PERF */

/*---------------------------------------------------------------------------
  Outputs the lower 8 bits of a frame's timestamp and an event identifier 
  to the port.
---------------------------------------------------------------------------*/
#define QTV_PERF_FRAME_UPDATE(timestamp, event)                             \
{                                                                           \
  QTV_PERF((uint16)( (event) | (timestamp & 0x00ff) ));         \
}

/*---------------------------------------------------------------------------
  Starts and stops the stopwatch timers.  There are 256 available timers.
---------------------------------------------------------------------------*/
#define QTV_PERF_WATCH_START(watch_id)                                      \
{                                                                           \
  QTV_PERF((uint16)(QTV_PERF_WATCH_START_ID | (watch_id)));     \
}

#define QTV_PERF_WATCH_STOP(watch_id)                                       \
{                                                                           \
  QTV_PERF((uint16)(QTV_PERF_WATCH_STOP_ID | (watch_id)));      \
}

#endif /* QTV_PERF_H */

