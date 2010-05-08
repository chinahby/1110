/************************************************************************** 
*************************   Restricted access   *************************** 
*************************************************************************** 












This file must only be used for the development of the HSUSB 
driver for the AMSS / BREW SW baselines using the Jungo USB Stack.
This file must not be used in any way for the development of any
functionally equivalent USB driver not using the Jungo USB stack.

For any questions please contact: Sergio Kolor, Liron Manor,
Yoram Rimoni, Dedy Lansky.











************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */
#ifndef __UTIL_H__
#define __UTIL_H__

#include "jdebug.h"
#include "lists.h"

/* HSU change - Use HSU_ASSERT in place of j_panic in the KASSERT macro. */
#include <hsu_common.h>

#define MAGIC_NUMBER    0xB0B0BABA

#ifdef JDEBUG
/* Used to catch invalid pointer dereferences */
# define INVALID_HANDLE ((void *)MAGIC_NUMBER)
#else
# define INVALID_HANDLE         ((void*)-1)
#endif

#ifdef DEBUG_PERF_LOG
extern jbool_t jstart_perf;
void jdump_time(void);
void jmark_time_v(char *file, jint32_t line);
# define DEBUG_PERF_LOG_ON jstart_perf = 1;
# define DEBUG_PERF_LOG_OFF do { jdump_time(); jstart_perf = 0; } while(0);
# define DEBUG_PERF_LOG_MARK do { jmark_time_v(__FILE__, __LINE__); } while(0);
#else
# define DEBUG_PERF_LOG_ON 
# define DEBUG_PERF_LOG_OFF 
# define DEBUG_PERF_LOG_MARK 
#endif

void mem_log_print(void);

#ifdef JDEBUG
/* HSU change - Use HSU_ASSERT in place of j_panic in the KASSERT macro. */
#define KASSERT(expr,msg) \
  if (!(expr)) \
  { \
    HSU_ASSERT("KASSERT failed" == NULL); \
    /* should never reach here. this is only to resolve compilation warning */ \
    /* about the msg argument */ \
    os_printf msg; \
  }
#else
#define KASSERT(expr, msg)                             
#endif

#define __CONCAT(a,b)   a ## b

#endif

