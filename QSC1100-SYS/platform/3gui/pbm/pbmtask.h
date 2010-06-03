#ifndef PBMTASK_H
#define PBMTASK_H

/*===========================================================================

                           PHONEBOOK MANAGER TASK

DESCRIPTION
  Header file for Phonebook Manager Task.

  Copyright (c) 2002 - 2008 by QUALCOMM INCORPORATED  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
$Header: //depot/asic/msmshared/services/pbm/pbmtask.h#13 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/28/08   sg      Featurize MMGSDI seek/search calls based on FEATURE_MMGSDI_ENHANCED_SEARCH
07/10/08   sg      Added PBM_GSDI_SEEK_CB to handle SEEK commands
05/08/08   clm     Queue up pbm notify requests to avoid calling them while in critical sections of pbm.
04/14/08   cvs     Fix compile warning
01/17/08   clm     Add FEATURE_PBM_USIM_SUPPORT to featurize USIM features.
11/02/07   clm     Add refresh okay and init complete support to pbm for mmgsdi.
07/24/06   cvs     Fixes for unequal size multi pb set support
05/03/06   cvs     convert to MMGSDI APIs.  Lint cleanup.
08/12/05   AT      Added support for UCS2 encoding.
03/08/05   PA      Support for PBM notifications.
01/12/05   PA      PBM USIM Support
11/19/04   PA      Support non-GSDI builds
11/18/04   PA      Support for write lock.
09/29/04   PA      Initial revision.
===========================================================================*/
#include "nv.h"
#include "queue.h"
#include "dog.h"
#include "task.h"

/* 
 * PBM Task Sinals
 */
#define PBM_RPT_TIMER_SIG            0x0001  /* PBM timer to kick watchdog */
#define PBM_CLIENT_CMD_SIG           0x0002  /* PBM Client Command Signal */
#define PBM_GSDI_RESPONSE_SIG        0x0004  /* GSDI Response Signal */
#define PBM_NV_SIG                   0x0008  /* NV operation complete signal */
#define PBM_CMD_COMPLETE_SIG         0x0010  /* PBM Client command complete */
#define PBM_MMGSDI_WAIT_SIG          0x0020  /* used for synchronizing MMGSDI accesses */
#define PBM_NOTIFY_EVT_SIG           0x0040  /* PBM notify event */

typedef enum {
  PBM_CMD_WRITE,             /* PBM write command from a client */
  PBM_CMD_WRITE_LOCK,        /* PBM write lock */
  PBM_CMD_WRITE_UNLOCK,      /* PBM write unlock */
  PBM_MMGSDI_WRITE_CB,       /* MMGSDI write callback */
  PBM_MMGSDI_READ_CB,        /* MMGSDI SIM read callback */
  PBM_MMGSDI_FILE_INFO_CB,   /* MMGSDI file info callback */
  PBM_GSDI_SIM_CAP_CB,       /* GSDI SIM capability callback */
  PBM_GSDI_PIN_STATUS_CB,    /* GSDI PIN status callback */
  PBM_MMGSDI_CARD_STATUS_CB  /* MMGSDI Card status callback */
#ifdef FEATURE_PBM_USIM_SUPPORT
  ,PBM_GSDI_READ_PBR_CB      /* GSDI USIM PBR calback */
#endif /* FEATURE_PBM_USIM_SUPPORT */
  ,PBM_CMD_NOTIFY_EVT        /* PBM Notify event */
#ifdef FEATURE_MMGSDI_ENHANCED_SEARCH
  ,PBM_MMGSDI_SEEK_CB        /* MMGSDI SIM seek callback */
#endif /* FEATURE_MMGSDI_ENHANCED_SEARCH */
} pbm_cmd_e_type;

#define PBM_DEFINE_CMD_HEADER() \
  q_link_type         link;     \
  pbm_cmd_e_type      cmd_type; \
  uint16              size;

/* Generic Header Type for PBM commands. 
 * All PBM commands that are queued including client commands and GSDI responses 
 * should have the command header defined at the start of the structure.
 * This should be done using the macro PBM_DEFINE_CMD_HEADER */
typedef struct {
  PBM_DEFINE_CMD_HEADER()
} pbm_cmd_s_type;

/*lint -emacro(740, PBM_CMD_PTR) */
#define PBM_CMD_PTR(x) ((pbm_cmd_s_type *)(x))

/*lint -emacro(740, PBM_CMD_RIGHT_SIZE) */
#define PBM_CMD_RIGHT_SIZE(ptr, type) \
          (((pbm_cmd_s_type *)(ptr))->size == sizeof(type))

/*===========================================================================

FUNCTION PBM_CMD_ALLOC

DESCRIPTION
  Allocate a PBM command buffer. The size of command is provided as input.

DEPENDENCIES
  Heap must have been initialized.

RETURN VALUE
  A pointer to the allocated command buffer.
  If Heap is empty a NULL is returned.

SIDE EFFECTS
  none

===========================================================================*/
void *pbm_cmd_alloc(uint16 size);

/*===========================================================================

FUNCTION PBM_CMD_FREE

DESCRIPTION
  Deallocate a PBM command buffer.

DEPENDENCIES
  Heap must have already been initialized.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void pbm_cmd_free(void *ptr);


/*===========================================================================
FUNCTION PBM_CLIENT_CMD_Q_PUT

DESCRIPTION
  Add an item to the client command queue and send a signal.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void pbm_client_cmd_q_put(pbm_cmd_s_type *cmd_ptr);

/*===========================================================================
FUNCTION PBM_GSDI_RESP_Q_PUT

DESCRIPTION
  Add an item to the GSDI response queue and send a signal.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void pbm_gsdi_resp_q_put(pbm_cmd_s_type *cmd_ptr);

/*===========================================================================
FUNCTION PBM_NOTIFY_EVT_Q_PUT

DESCRIPTION
  Add an item to the NOTIFY event queue and send a signal.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void pbm_notify_evt_q_put(pbm_cmd_s_type *cmd_ptr);

/*===========================================================================
FUNCTION PBM_TASK

DESCRIPTION
  The main PBM task.

DEPENDENCIES
  REX is running.

RETURN VALUE
  Never exits.

SIDE EFFECTS

===========================================================================*/
void pbm_task(dword dummy);

#ifndef _SCL
/*===========================================================================
FUNCTION PBM_GET_NV

DESCRIPTION
  Read an item from the NV memory.

  Note that NV read is done in a synchronous fashion; that is, this
  function waits till the read is done.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
nv_stat_enum_type pbm_get_nv(nv_items_enum_type nv_item, nv_item_type *data_ptr);

/*===========================================================================
FUNCTION PBM_PUT_NV

DESCRIPTION
  Write an item to NV memory.

  Note that NV write is done in a synchronous fashion; that is, this
  function waits till the write is done.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
nv_stat_enum_type pbm_put_nv(nv_items_enum_type nv_item, nv_item_type *data_ptr);
#endif

/*===========================================================================
FUNCTION PBM_WAIT

DESCRIPTION
  Suspend PBM from executing until at least one of the specified signals
  in the input mask is set. 

DEPENDENCIES
  pbm_rpt_timer must have been initialized

RETURN VALUE
  Signals received.

SIDE EFFECTS
  None

===========================================================================*/
rex_sigs_type pbm_wait(rex_sigs_type mask);

/*===========================================================================
FUNCTION PBM_KICK_DOG

DESCRIPTION
  Kick the watchdog and set the watchdog timer.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void pbm_kick_dog(void);
#endif /* PBMTASK_H */
