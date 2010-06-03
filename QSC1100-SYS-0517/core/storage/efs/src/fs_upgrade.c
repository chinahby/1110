/***********************************************************************
 * fs_upgrade.c
 *
 * In-place upgrade management.
 * Copyright (C) 2006--2007 QUALCOMM, Inc.
 *
 * Code for both tracking what upgrades are needed as well as performing
 * the operations themselves.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_upgrade.c#1 $ $DateTime: 2008/05/29 19:19:11 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2007-05-25   umr   Added support for EFS memory protection feature.
2006-09-06   sh    Lint fixes
2006-08-31   dlb   Add query for all upgrades finished.
2006-08-15   dlb   Create

===========================================================================*/

#include "comdef.h"
#include "assert.h"
#include "fs_upgrade.h"
#include "fs_protected_memory.h"

/* As long as there is only a single filesystem, just track the upgrades
 * needed in this bitmap. */
static uint32 fs_upgrades_needed = 0;

void
fs_upgrade_init (void)
{
  fs_upgrades_needed = 0;
}

void
fs_upgrade_add (unsigned kind)
{
  ASSERT (kind < 32);

  fs_upgrades_needed |= (1 << kind);
}

void
fs_upgrade_finished (unsigned kind)
{
  ASSERT (kind < 32);

  fs_upgrades_needed &= ~(1U << kind);
}

unsigned
fs_upgrade_check (unsigned kind)
{
  ASSERT (kind < 32);

  return (fs_upgrades_needed & (1 << kind)) != 0;
}

unsigned
fs_upgrades_all_finished (void)
{
  return fs_upgrades_needed == 0;
}
