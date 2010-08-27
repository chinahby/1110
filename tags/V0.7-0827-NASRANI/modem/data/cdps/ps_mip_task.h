#ifndef PS_MIP_TASK_H
#define PS_MIP_TASK_H
/*===========================================================================
                          P S _ M I P _ T A S K . H

DESCRIPTION
  Header file containing the MIP task specific information.  This is intended
  to isolate all of the task specific information to a single module.

  Copyright 2002-2009 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

$PVCSPath: L:/src/asw/MM_DATA/vcs/ps_mip_task.h_v   1.1   16 Sep 2002 14:52:18   jayanthm  $
$Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_mip_task.h#3 $ $DateTime: 2009/04/01 22:30:10 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/18/09    sn     Modified mip_task_rand_data_ready() prototype to register 
                   it as callback function with SEC to be called when 
                   random data is available.
09/19/06    as     Added mip_task_rand_data_ready() API for secutil to
                   indicate when random data is available.
09/10/02    jd/jay created file
===========================================================================*/

#include "ps_svc.h"

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                                   MACROS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*---------------------------------------------------------------------------
  Define the task functions in terms of the equivalent functions for the
  hosting task: in this case PS
---------------------------------------------------------------------------*/
#define MIP_SEND_CMD( cmd, data_ptr )           \
  ps_send_cmd((cmd), (void *) (data_ptr))

#define MIP_ENABLE_SIG( signal )                \
  ps_enable_sig( (signal) )

#define MIP_DISABLE_SIG( signal )               \
  ps_disable_sig( (signal) )

/*---------------------------------------------------------------------------
  define all of the MIP related commands that can be sent in terms of the
  commands for the hosting task: in this case PS
---------------------------------------------------------------------------*/  
#define MIPIO_PPP_OPEN_CMD   PS_MIPIO_PPP_OPEN_CMD
#define MIPIO_PPP_CLOSE_CMD  PS_MIPIO_PPP_CLOSE_CMD

#ifdef FEATURE_DS_MOBILE_IP_DMU
#define DMU_PREENCRYPT_CMD   PS_DMU_PREENCRYPT_CMD
#endif /* FEATURE_DS_MOBILE_IP_DMU */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                             EXTERNAL FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*===========================================================================
FUNCTION MIP_TASK_POWERUP_INIT()

DESCRIPTION
  This function does the powerup initialization for the MIP module.  This
  includes registering handlers with PS (the task executing it) for the MIP
  RX signals.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void mip_task_powerup_init( void );

/*===========================================================================
FUNCTION    MIP_TASK_RAND_DATA_READY

DESCRIPTION     
  This function is called by secutil when random data is available via
  secutil_get_rand().

PARAMETERS
  none

DEPENDENCIES    
  none

RETURN VALUE
  none

SIDE EFFECTS  
  None
===========================================================================*/
void mip_task_rand_data_ready( void *user_data_ptr );

#endif /* PS_MIP_TASK_H */
