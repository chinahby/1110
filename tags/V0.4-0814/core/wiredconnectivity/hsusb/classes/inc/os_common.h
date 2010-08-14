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
#ifndef _OS_COMMON_H_
#define _OS_COMMON_H_

/* 
 * Functions and prototypes common for the USBWare stack and the porting layer 
 */

#include <compiler.h>
#include <jdebug.h>
#include <jtypes.h>

#ifdef JDEBUG 
# define jmalloc(a,b)           jmalloc_d(a, b, __FILE__, __LINE__)
# define jfree(a)               jfree_d(a, __FILE__, __LINE__)
void *jmalloc_d(juint32_t size, juint16_t flags, char *file, jint32_t line);
void jfree_d(void *addr, char *file, jint32_t line);
#elif defined(CONFIG_MEMPOOL)
# define jmalloc               jmemp_alloc
# define jfree                 jmemp_free
void *jmemp_alloc(juint32_t, juint16_t);
void jmemp_free(void *);
#else
# define jmalloc               jos_malloc
# define jfree                 jos_free
#endif

#ifdef JDEBUG
# ifdef CONFIG_PORT_NO_GET_CONTEXT
/* HSU Fix:
** For now, we deliberately disable synchronization debugging,
** and thus we do not need the warnings.
*/
#if 0
#  ifndef T_REXNT
#   warning "Cannot debug sychronization"
#   warning "No 'os_get_context()' configure flag is set in the config file."
#  else
#error code not present
#  endif
#endif /* 0 */
/* End HSU Fix */
#  undef  DEBUG_SYNC
# else
#  define DEBUG_SYNC
# endif
#endif

#ifndef SINGLE_THREADED
#ifdef DEBUG_SYNC
# define jsafe_enter()          jsafe_enter_d(__FILE__, __LINE__)
# define jsafe_leave()          jsafe_leave_d(__FILE__, __LINE__)
void jsafe_enter_d(char *file, int line);
void jsafe_leave_d(char *file, int line);
jbool_t jsafe_check(char *file, int line);
# define LOCK_CHECK             jsafe_check(__FILE__, __LINE__)
#else
# define jsafe_enter            os_safe_enter
# define jsafe_leave            os_safe_leave
# define LOCK_CHECK             do { } while (0)
#endif
#else
# define jsafe_enter()
# define jsafe_leave()
# define LOCK_CHECK             do { } while (0)
#endif

#endif

