#ifndef VDECODER_PERF_H
#define VDECODER_PERF_H

#include "comdef.h"
#include "qw.h"

//#define FEATURE_QTV_VIDEO_DECODER_PROFILING

#ifdef FEATURE_QTV_VIDEO_DECODER_PROFILING
#error code not present
#else
#define BEGIN_FUNC_PROFILE
#define END_FUNC_PROFILE
#endif

#endif /* VDECODER_PERF_H */

