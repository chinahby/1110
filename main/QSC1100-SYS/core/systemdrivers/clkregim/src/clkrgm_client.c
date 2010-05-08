/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

    C L O C K    R E G I M E    S O U R C E    M A N A G E R    C L I E N T


GENERAL DESCRIPTION
  Clock regime clock sources are voted on and managed via these client interfaces.
  Clock sources are enabled when any client requests them, and disabled only when
  all clients have released them.

EXTERNALIZED FUNCTIONS
  
  void clk_regime_client_init(void)
    Initialize the client services.

  void clk_regime_enable_client (clkrgm_modem_clk_type clk, clkrgm_clients_type client)
    Request a modem clock resource.

  void clk_regime_disable_client (clkrgm_modem_clk_type clk, clkrgm_clients_type client)
    Release a modem clock resource.

  void clk_regime_msm_enable_client (clkrgm_msm_clk_type clk, clkrgm_clients_type client)
    Request a non-modem clock resource.

  void clk_regime_msm_disable_client (clkrgm_msm_clk_type clk, clkrgm_clients_type client)
    Release a non-modem clock resource.

INITIALIZATION AND SEQUENCING REQUIREMENTS

  clk_regime_client_init() should be called before any other client functions are used.

  Copyright (c) 2004-2008      by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/clkregim/clkrgm_client.c#4 $
$DateTime: 2008/01/16 11:20:14 $
$Author: bcalder $

when       who     what, where, why
10/11/2007  bc     Added comment block.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "clkrgm_client.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/
#ifdef CLKRGM_CLIENT_DBG
#error code not present
#else
  #define CLKRGM_CLIENT_TRACE(mod_clk, msm_clk, cli, mask, direction)
#endif

static uint16 clkrgm_clock_state[CLKRGM_MODEM_NUM_OF_CLOCKS];
/* The above definition/allocation is a bit of a memory waister.
 * It can be made smaller by limiting number of client such that a 
 * uint8 or uint16 could hold all clients.
 */


static boolean clkrgm_client_access=FALSE;


void clk_regime_client_init(void)
{
  memset(clkrgm_clock_state, 0, sizeof(uint16)*(CLKRGM_MODEM_NUM_OF_CLOCKS-1));
}


void clk_regime_enable_client (clkrgm_modem_clk_type clk, clkrgm_clients_type client)
{
  boolean is_off;

  INTLOCK();
  is_off = CLKRGM_RESOURCE_IS_OFF(client, clk);
  CLKRGM_RESOURCE_ON(client, clk);
  CLKRGM_CLIENT_TRACE(clk, 0xff, client, clkrgm_clock_state[clk], TRUE);
  if (is_off)
  {
    CLKRGM_RESOURCE_ACCESS(TRUE);
    clk_regime_enable(clk);
    CLKRGM_RESOURCE_ACCESS(FALSE);
  }
  INTFREE();
} /* clk_regime_enable_client */

void clk_regime_disable_client (clkrgm_modem_clk_type clk, clkrgm_clients_type client)
{
  INTLOCK();
  CLKRGM_RESOURCE_OFF(client, clk);
  CLKRGM_CLIENT_TRACE(clk, 0xff, client, clkrgm_clock_state[clk], FALSE);
  if (CLKRGM_RESOURCE_IS_OFF(client, clk))
  {
    CLKRGM_RESOURCE_ACCESS(TRUE);
    clk_regime_disable(clk);
    CLKRGM_RESOURCE_ACCESS(FALSE);
  }
  INTFREE();
} /* clk_regime_disable_client */

void clk_regime_msm_enable_client (clkrgm_msm_clk_type clk, clkrgm_clients_type client)
{
  boolean is_off;

  INTLOCK();
  is_off = CLKRGM_RESOURCE_IS_OFF(client, clk);
  CLKRGM_RESOURCE_ON(client, clk);
  CLKRGM_CLIENT_TRACE(0xff, clk, client, clkrgm_clock_state[clk], TRUE);
  if (is_off)
  {
    CLKRGM_RESOURCE_ACCESS(TRUE);
    clk_regime_msm_enable(clk);
    CLKRGM_RESOURCE_ACCESS(FALSE);
  }
  INTFREE();
}

void clk_regime_msm_disable_client (clkrgm_msm_clk_type clk, clkrgm_clients_type client)
{
  INTLOCK();
  CLKRGM_RESOURCE_OFF(client, clk);
  CLKRGM_CLIENT_TRACE(0xff, clk, client, clkrgm_clock_state[clk], FALSE);
  if (CLKRGM_RESOURCE_IS_OFF(client, clk))
  {
    CLKRGM_RESOURCE_ACCESS(TRUE);
    clk_regime_msm_disable(clk);
    CLKRGM_RESOURCE_ACCESS(FALSE);
  }
  INTFREE();
}

boolean clk_regime_client_access(void)
{
  boolean r;
  INTLOCK();
  r = clkrgm_client_access;
  INTFREE();
  return r;
}
