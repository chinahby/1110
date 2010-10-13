/***********************************************************************
 * fs_osal.h
 *
 * This is a thin OS abstraction layer.
 * Copyright (C) 2009 QUALCOMM, Inc.
 *
 * Using this file allows us to compile EFS/HFAT code without any sort
 * of underlying operating system (REX in this case). This situation
 * arises when we are compiling code into qcsbl or oemsbl.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/inc/fs_osal.h#1 $ $DateTime: 2009/07/06 18:30:05 $ $Author: ebuller $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2009-06-12   ebb   Create

===========================================================================*/

#ifndef __FS_OSAL_H__
#define __FS_OSAL_H__

#include "customer.h"
#include "fs_config.h"
#include "comdef.h"

/* REX not defined, stub everything out */
#ifdef FEATURE_EFS_COMPILE_WITHOUT_REX

  /* These two fields must be present because they are getting dereferenced
   * in our code */
  typedef struct
  {
    uint32 err_num;
    uint32 dog_report_val;
  } fs_os_tcb_type;

  typedef uint32               fs_os_timer_type;
  typedef uint32               fs_os_timer_cnt_type;
  typedef uint32               fs_os_sigs_type;
  typedef uint32               fs_os_crit_sect_type;
  typedef uint32               fs_os_stack_size_type;
  typedef uint32               fs_os_priority_type;
  typedef void                 (*fs_os_task_func_type) (unsigned long);

  /* Needed to maintain the notion of time for unit tests */
  #ifdef FS_UNIT_TEST
#error code not present
  #endif

  /* Function Stubs */
  static inline fs_os_sigs_type fs_os_clr_sigs (fs_os_tcb_type *tcb,
                                              fs_os_sigs_type sigs)
  {
    (void)tcb; (void)sigs;
    return 0;
  }

  static inline fs_os_sigs_type fs_os_set_sigs (fs_os_tcb_type *tcb,
                                                fs_os_sigs_type sigs)
  {
    (void)tcb; (void)sigs;
    return 0;
  }

  static inline void fs_os_def_timer (fs_os_timer_type *tmr,
                                      fs_os_tcb_type *tcb,
                                      fs_os_sigs_type sigs)
  {
    (void)tmr; (void)tcb; (void)sigs;
  }

  static inline fs_os_timer_cnt_type fs_os_set_timer (fs_os_timer_type *tmr,
                                                      fs_os_timer_cnt_type ms)
  {
    (void)tmr; (void)ms;
    return 0;
  }

  static inline fs_os_timer_cnt_type fs_os_clr_timer (fs_os_timer_type *tmr)
  {
    (void)tmr;
    return 0;
  }

  static inline fs_os_sigs_type fs_os_wait (fs_os_sigs_type sigs)
  {
    (void)sigs;
    return 0;
  }

  static inline fs_os_sigs_type fs_os_timed_wait(fs_os_sigs_type sigs,
                                                 fs_os_timer_type *tmr,
                                                 fs_os_timer_cnt_type cnt)
  {
    (void)sigs; (void)tmr; (void)cnt;

    #ifdef FS_UNIT_TEST
#error code not present
    #endif
    return 0;
  }

  static inline fs_os_tcb_type *fs_os_self (void)
  {
    return (fs_os_tcb_type *)0;
  }

  static inline int fs_os_autodog_enable (int val)
  {
    (void)val;
    return 0;
  }

  static inline int fs_os_autodog_disable (void)
  {
    return 0;
  }

  static inline void fs_os_init_crit_sect (fs_os_crit_sect_type *cs)
  {
    (void)cs;
  }

  static inline void fs_os_enter_crit_sect (fs_os_crit_sect_type *cs)
  {
    (void)cs;
  }

  static inline void fs_os_leave_crit_sect (fs_os_crit_sect_type *cs)
  {
    (void)cs;
  }

  static inline void fs_os_set_dac (fs_os_tcb_type *tcb, unsigned int dac)
  {
    (void)tcb; (void)dac;
  }

  static inline void fs_os_def_task_ext(fs_os_tcb_type *tcb,
                                        unsigned char *stk,
                                        fs_os_stack_size_type stksz,
                                        fs_os_priority_type pri,
                                        fs_os_task_func_type tsk,
                                        dword p,
                                        char *nme,
                                        boolean s)
  {
    (void)tcb; (void)stk; (void)stksz; (void)tsk;
    (void)pri; (void)p; (void)nme; (void)s;
  }

#else /* REX is defined, redirect all calls to the REX API */

  /* Include rex header */
  #include "rex.h"

  /* Data types */
  typedef rex_tcb_type         fs_os_tcb_type;
  typedef rex_timer_type       fs_os_timer_type;
  typedef rex_timer_cnt_type   fs_os_timer_cnt_type;
  typedef rex_sigs_type        fs_os_sigs_type;
  typedef rex_crit_sect_type   fs_os_crit_sect_type;

  /* Functions */
  #define fs_os_clr_sigs(tcb,sigs)       rex_clr_sigs(tcb,sigs)
  #define fs_os_set_sigs(tcb,sigs)       rex_set_sigs(tcb,sigs)
  #define fs_os_def_timer(tmr,tcb,sigs)  rex_def_timer(tmr,tcb,sigs)
  #define fs_os_set_timer(tmr,msecs)     rex_set_timer(tmr,msecs)
  #define fs_os_clr_timer(tmr)           rex_clr_timer(tmr)
  #define fs_os_wait(sigs)               rex_wait(sigs)
  #define fs_os_timed_wait(sigs,tmr,cnt) rex_timed_wait(sigs,tmr,cnt)
  #define fs_os_self()                   rex_self()
  #define fs_os_autodog_enable(val)      rex_autodog_enable(val)
  #define fs_os_autodog_disable()        rex_autodog_disable()
  #define fs_os_init_crit_sect(cs)       rex_init_crit_sect(cs)
  #define fs_os_enter_crit_sect(cs)      rex_enter_crit_sect(cs)
  #define fs_os_leave_crit_sect(cs)      rex_leave_crit_sect(cs)
  #define fs_os_set_dac(tcb,dac)         rex_set_dac(tcb,dac)
  #define fs_os_def_task_ext(tcb,stk,stksz,pri,tsk,p,nme,s) \
          rex_def_task_ext(tcb,stk,stksz,pri,tsk,p,nme,s)

#endif /* FEATURE_EFS_COMPILE_WITHOUT_REX */

#endif /* not __FS_OSAL_H__ */
