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
#ifndef _OS_PORT_H_
#define _OS_PORT_H_

/* 
 * Functions and prototypes for use by the USBWare stack only 
 */

#include <os_common.h>

#define jbus_resource_h         os_bus_resource_h
#define jbus_h                  os_bus_h
#define jspinlock_h             os_spinlock_h
#define jtime_t                 os_time_t
#define jmsleep_h               os_msleep_h

#define jget_context            os_get_context

#define jspinlock_init          os_spinlock_init
#define jspinlock_uninit        os_spinlock_uninit
#define jspinlock_lock          os_spinlock_lock
#define jspinlock_unlock        os_spinlock_unlock

#ifdef SINGLE_THREADED
# define jthread_create(a,b,c,d)  0
# define jmutex_init(a)         0
# define jmutex_uninit(a)       do { } while (0)
# define jmutex_lock(a)         do { } while (0)
# define jmutex_unlock(a)       do { } while (0)

# define jdelay_us              os_delay_us
# define jdelay_ms(a)           os_delay_us((a) * 1000)

# define jmsleep_init           0
# define jmsleep_uninit         0
# define jmsleep(a,b)           0
# define jwakeup(a)             do { } while (0)

# define jtask_h                os_timer_h
# define jtask_init(a,b)        os_timer_init(a)
# define jtask_uninit           os_timer_uninit
# define jtask_schedule         os_timer_reset
# define jtask_stop             os_timer_stop
# define jtask_pending          os_timer_pending

#else /* Multi threaded */

# define jthread_create         os_thread_create

#ifdef DEBUG_SYNC
# define jmutex_init(a)         jmutex_init_d(a, __FILE__, __LINE__)
# define jmutex_uninit(a)       jmutex_uninit_d(a, __FILE__, __LINE__)
# define jmutex_lock(a)         jmutex_lock_d(a, __FILE__, __LINE__)
# define jmutex_unlock(a)       jmutex_unlock_d(a, __FILE__, __LINE__)
typedef void *jmutex_h;
jresult_t jmutex_init_d(jmutex_h *mtx_h, char *file, int line);
void jmutex_uninit_d(jmutex_h mtx_h, char *file, int line);
void jmutex_lock_d(jmutex_h mtx_h, char *file, int line);
void jmutex_unlock_d(jmutex_h mtx_h, char *file, int line);
#else
# define jmutex_h               os_mutex_h
# define jmutex_init            os_mutex_init
# define jmutex_uninit          os_mutex_uninit
# define jmutex_lock            os_mutex_lock
# define jmutex_unlock          os_mutex_unlock
#endif

# define jdelay_us              os_delay_us
# define jdelay_ms              os_delay_ms

# define jmsleep_init           os_msleep_init
# define jmsleep_uninit         os_msleep_uninit
# define jmsleep                os_msleep
# define jwakeup                os_wakeup
#endif

#define jget_time               os_get_time
#define jget_hz                 os_get_hz

/* HSU modification - Make jprintf use AMSS logging mechanism. (Start) */
#ifndef T_REXNT
  #ifdef JDEBUG
    #define jprintf                 MSG_HIGH_VAR_ARGS
  #else
    #define jprintf
  #endif /* JDEBUG */
#else   /* !T_REXNT */
#error code not present
#endif  /* !T_REXNT */
/* HSU modification - Make jprintf use AMSS logging mechanism. (End) */

#define j_panic                 os_panic
#define jdprintf                os_dprintf
#define jprintstack             os_print_stack

#define j_memcmp                os_memcmp
#define j_strlcpy               os_strlcpy
#define j_snprintf              os_snprintf
#define j_strcmp                jos_strcmp
#define j_strncmp               jos_strncmp
#define j_memset                jos_memset
#define j_memcpy                jos_memcpy
#define j_strlen                jos_strlen

#define jhw_init                os_hw_init
#define jhw_uninit              os_hw_uninit

#define j_bus_alloc_resource    os_bus_alloc_resource
#define jbus_release_resource   os_bus_release_resource

#define j_bus_space_read_1      os_bus_space_read_1
#define j_bus_space_read_2      os_bus_space_read_2
#define j_bus_space_read_4      os_bus_space_read_4
#define j_bus_space_write_1     os_bus_space_write_1
#define j_bus_space_write_2     os_bus_space_write_2
#define j_bus_space_write_4     os_bus_space_write_4

#ifdef CONFIG_PORT_INTERCHIP
#define j_power_request         os_power_request
#define j_get_port_info         os_get_port_info
#endif

#define j_bus_get_transceiver   os_bus_get_transceiver
#define j_bus_get_controller_id os_bus_get_controller_id

#ifdef CONFIG_BUS_PCI
#define j_pci_read_config       os_pci_read_config
#define j_pci_write_config      os_pci_write_config
#define j_pci_enable_busmaster  os_pci_enable_busmaster
#define j_pci_get_function      os_pci_get_function
#define j_pci_get_slot          os_pci_get_slot
#endif

#ifdef CONFIG_CPU_LOAD_SUPPORT
#define j_cpuload_get_100ms     os_cpuload_get_100ms
#define j_cpuload_get_1s        os_cpuload_get_1s
#define j_cpuload_get_10s       os_cpuload_get_10s
#endif

#define j_cache_invalidate      os_cache_invalidate
#define j_cache_flush           os_cache_flush

#define jnotify_os              os_notify
#include <port.h>

#endif

