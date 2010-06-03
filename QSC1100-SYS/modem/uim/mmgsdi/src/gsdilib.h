#ifndef GSDILIB_H
#define GSDILIB_H
/*===========================================================================


            G S D I   L I B R A R Y   I N T E R F A C E


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2001 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/gsdilib.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/15/07   tml     Remove extra client registration signal
09/13/05   sun     A different approach to serialization is required for L4.
05/19/03   tml     linted
07/23/02   jar     Added a function prototype for gsdi_lib_utility_sync_sim_and_cache
                   to be called from within the GSDI Library Function 
                   gsdi_sim_write to match contents written into the cache
                   to the contents in the SIM/USIM.
10/11/01   jc      Update library cmd q signal value.


===========================================================================*/
#include "gsdi_exp.h"
#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_MMGSDI_UMTS)
#error code not present
#endif
  
/* Signal defs for the GSDI library */
#define GSDI_LIB_CMD_Q_SIG         0x1000

#ifdef FEATURE_L4
#error code not present
#endif

/* Command structures */
typedef union
{
    gsdi_cnf_T        gsdi_cnf_cmd;
    timer_expiry_T        timer_expiry;

} gsdi_lib_cmd_data_type;


/* The queue command structures for GSDI Library */
typedef struct
{
        q_link_type               link;
        gsdi_lib_cmd_data_type    cmd;

} gsdi_lib_cmd_type;

extern q_type gsdi_lib_cmd_q;

/* Used by GSDI task to allocate a buffer from the GSDI library */
extern void gsdi_lib_put_cmd_buf(gsdi_lib_cmd_type *cmd_ptr);

/* Used by GSDI task to send a command to the GSDI library */
extern gsdi_lib_cmd_type *gsdi_lib_get_cmd_buf(void);

/* Call by rex for timer expiration */
extern void reg_timer_expiry(unsigned long param);

#ifdef FEATURE_GSDI_OPTIMIZATION
/*===========================================================================
FUNCTION GSDI_LIB_UTILITY_SYNC_SIM_AND_CACHE

DESCRIPTION
	This function when called will be used to access the cach for read and 
    writes.  On a successful access, a corresponding Async Message is 
    generated to also update the SIM. 

DEPENDENCIES
  None

RETURN VALUE
  gs_returns_T.
===========================================================================*/
extern gsdi_returns_T gsdi_lib_utility_sync_sim_and_cache(
  uim_items_enum_type    sim_filename,
  gsdi_index_modes_T     index_mode,
  int                    rec_num,
  byte *                 data_p,
  int                    data_len,
  int                    data_offset );

#endif /* FEATURE_GSDI_OPTIMIZATION */

#endif /* GSDILIB_H */

