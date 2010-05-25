/***********************************************************************
 * fs_global_lock.c
 *
 * FS_GLOBAL_LOCK critical section
 * Copyright (C) 2007-2008 QUALCOMM, Inc.
 *
 * The FS_GLOBAL_LOCK is used to protect the file system data from
 * concurrent access.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_global_lock.c#3 $ $DateTime: 2008/12/17 12:09:37 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2008-12-15   yg    Do not include pd.h unless memory protection is defined.
2008-07-14   np    Added support for enabling l4 memory protection.
2007-12-27   yg    Added memory protection related header file
2007-12-05   rp    Moved FS_PARANOIA_CHECKS feature to fs_paranoia.c
2007-11-16   sh    Rename TIMETEST2 calls to Babylon calls
2007-10-26   rp    Corrected featurization of FS_PARANOIA_CHECKS.
2007-08-19   rp    Create

===========================================================================*/

#include "fs_global_lock.h"
#include "rex.h"
#include "fs_babylon.h"
#include "fs_efs2.h"
#include "fs_paranoia.h"

#ifdef FEATURE_DAC_MEMORY_PROTECTION
  #ifndef HAVE_DAC_MEMORY_PROTECTION
    #error "Rex version mismatch with EFS"
  #endif /* HAVE_DAC_MEMORY_PROTECTION */

  #include "armmmu.h"
#endif /* FEATURE_DAC_MEMORY_PROTECTION */

#ifdef FS_STANDALONE
#include "ut_utils.h"
#endif

/* FS_GLOBAL_LOCK critical section definition memory allocation */
static rex_crit_sect_type fs_crit_sect;

#if (defined(FEATURE_L4) && \
     defined(FEATURE_ENABLE_EFS_MEMORY_PROTECTION))
  #include <iguana/pd.h>
#endif /* FEATURE_L4 && FEATURE_ENABLE_EFS_MEMORY_PROTECTION */

static void fs_efs2_protect_memory (void);

void
fs_global_lock_init(void)
{
  rex_init_crit_sect (&fs_crit_sect);
}

/* This function replaces the FS_GLOBAL_LOCK macro */
void
fs_global_lock (void)
{
  /* Enter critical section and Unlock the EFS RW and ZI */
  /* In the current implementation any running task calling
   * unprotect/activate_pd() will enter the special protection domain that has
   * access to the EFS protected segment and only one thread is allowed to run
   * in the protected domain at a time.
   *
   * Thus we must gate/protect calling unprotect/activate_pd() with a critical
   * section to ensure only one task at a time accesses the protected area.
   */
  rex_enter_crit_sect (&fs_crit_sect);
  fs_efs2_unprotect_memory ();

  fs_paranoia_check ();

#ifdef FS_STANDALONE
/* Call simulator specific global lock */
  ut_fs_global_lock ();
#endif

  EFS_BAB_START (EFS_BAB_GLOBAL_LOCK); /* TIMETEST2 marker */
}

/* Freeze data structure hash */
void
fs_global_unlock (void)
{
  fs_paranoia_record ();

  rex_leave_crit_sect (&fs_crit_sect);

  /* Lock the EFS critical RW and ZI */
  fs_efs2_protect_memory ();

#ifdef FS_STANDALONE
  /* Call simulator specific global unlock */
  ut_fs_global_unlock ();
#endif

  EFS_BAB_END (EFS_BAB_GLOBAL_LOCK); /* TIMETEST2 marker */
}

#if (defined(FEATURE_ENABLE_EFS_MEMORY_PROTECTION) && \
     defined(FEATURE_DAC_MEMORY_PROTECTION))

/* Lock the EFS RW and ZI memory block for any read or write access. */
static void
fs_efs2_protect_memory (void)
{
  rex_tcb_type *rex_task;
  rex_task = rex_self ();

  /* Lock down the EFS WR and ZI section */
  if (rex_task != fs_crit_sect.owner)
  {
    /* Current task does not own the critical section any more. This means no
     * more nesting of locks. We are free to lock memory */
    rex_set_dac (rex_task, MMU_PROTECT_EFS_MEMORY);
  }
  /* ELSE - There is a nesting of entering critical sections.. Just skip */
}

/* UnLock the EFS RW and ZI memory block for all read or write access. */
void
fs_efs2_unprotect_memory (void)
{
  rex_tcb_type *rex_task;

  rex_task = rex_self ();
  rex_set_dac (rex_task, MMU_UNPROTECT_EFS_MEMORY);
}

#elif (defined(FEATURE_ENABLE_EFS_MEMORY_PROTECTION) && \
       defined(HAVE_L4_PD_EXTENSION_ACTIVATE))

/* Lock the EFS RW and ZI memory block for any read or write access. */
static void
fs_efs2_protect_memory (void)
{
  rex_tcb_type *rex_task;
  rex_task = rex_self ();

  /* In the case of nested enter_critical_section, we have to make sure that
   * only the last instance of the leave_critical_section should protect the
   * memory. All the intermediate leave_critical_section instances should not
   * protect it, since the task might still be changing the memory after this.
   */
  if (rex_task != fs_crit_sect.owner)
  {
    /* Current task does not own the critical section any more. This means no
     * more nesting of locks. We are free to lock memory */
    pd_extension_deactivate(NULL);
  }
  /* ELSE - There is a nesting of entering critical sections.. Just skip */
}

/* UnLock the EFS RW and ZI memory block for all read or write access. */
void
fs_efs2_unprotect_memory (void)
{
  pd_extension_activate(NULL);
}

#else

/* If memory protection is not available, the protect and unprotect functions
 * become NO-op.
 */
static void
fs_efs2_protect_memory (void)
{
  return;
}

void
fs_efs2_unprotect_memory (void)
{
  return;
}

#endif

