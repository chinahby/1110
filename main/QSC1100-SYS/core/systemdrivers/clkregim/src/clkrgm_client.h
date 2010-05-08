#ifndef _CLKRGM_RESOURCE_CLK_H_
#define _CLKRGM_RESOURCE_CLK_H_

/*=========================================================================

            CLOCK REGIME RESOURCE CLIENT DECLARATIONS


GENERAL DESCRIPTION
   The interface to the clock resource manager is provided in this header file.

EXTERNALIZED FUNCTIONS

  clk_regime_enable_client(clk, clt)
  clk_regime_disable_client(clk, clt)
  clk_regime_msm_enable_client(clk, clt)
  clk_regime_msm_disable_client(clk, clt)

  Copyright (c) 2006-2007 by QUALCOMM, Inc.  All Rights Reserved.

  $Header: //depot/asic/qsc1100/drivers/clkregim/clkrgm_client.h#4 $
=========================================================================*/
#include "clkrgm_msm.h"
#include "clkrgm_modem.h"

#define CLKRGM_RESOURCE_ON(client, clock) \
   clkrgm_clock_state[clock] |= (1<<client)

#define CLKRGM_RESOURCE_IS_OFF(client, clock) \
   (clkrgm_clock_state[clock] == 0x0)

#define CLKRGM_RESOURCE_OFF(client, clock) \
   clkrgm_clock_state[clock] &= ~(1<<client)

#define CLKRGM_RESOURCE_ACCESS(b) \
  clkrgm_client_access=b

typedef enum {
  CLKRGM_RF_0 = 0,
  CLKRGM_RF_1,
  CLKRGM_HDR,
  CLKRGM_GPS,
  CLKRGM_SRCH,
  CLKRGM_MDSP,
  CLKRGM_ADSP,
  CLKRGM_USB,
  CLKRGM_AUDIO,
  CLKRGM_BT,
  CLKRGM_CLKREGIM,
  CLKRGM_NUM_CLIENTS
} clkrgm_clients_type;

#ifndef BUILD_BOOT_CHAIN_OEMSBL
extern void clk_regime_client_init(void);

extern void clk_regime_enable_client
(
  clkrgm_modem_clk_type regime, 
  clkrgm_clients_type client
);
extern void clk_regime_disable_client
(
  clkrgm_modem_clk_type regime, 
  clkrgm_clients_type client
);
extern void clk_regime_msm_enable_client
(
  clkrgm_msm_clk_type regime, 
  clkrgm_clients_type client
);
extern void clk_regime_msm_disable_client
(
  clkrgm_msm_clk_type regime, 
  clkrgm_clients_type client
);
extern boolean clk_regime_client_access(void);
#else
  #define clk_regime_enable_client(clk, clt) clk_regime_enable(clk)
  #define clk_regime_disable_client(clk, clt) clk_regime_disable(clk)
  #define clk_regime_msm_enable_client(clk, clt) clk_regime_msm_enable(clk)
  #define clk_regime_msm_disable_client(clk, clt) clk_regime_msm_disable(clk)
  #define clk_regime_client_access() TRUE
#endif /*  BUILD_BOOT_CHAIN_OEMSBL */

#endif /* _CLKRGM_RESOURCE_CLK_H_ */
