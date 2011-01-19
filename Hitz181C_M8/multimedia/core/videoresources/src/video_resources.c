/*==============================================================================

            V I D E O  C A M E R A   R E S O U R C E S   S O U R C E   F I L E

                             COPYRIGHT INFORMATION

Copyright (c) 2005-2007 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/core/videoresources/6k/main/latest/src/video_resources.c#1 $ $DateTime: 2008/05/09 16:34:47 $ $Author: karenp $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
04/17/08   chai    Fixed Some critical lint Warnings
10/18/07   dle     Added call stack tracking.
11/28/06   ven     Wrapped the unsupported clock part for SC2X
02/16/06   dle     Linted.
10/21/05   dl      Resolved lint issues.
05/02/05   kwa     Initial version.
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE

==============================================================================*/

#include "customer.h"
#include "video_resources.h"
#include "msg.h"

extern signed long MALLOCMGR_UNWIND_STACK(void **buffer, unsigned long size);

/*==============================================================================

                                TYPE DEFINITIONS

==============================================================================*/

/*==============================================================================

                          STATIC FUNCTION DECLARATIONS

==============================================================================*/

/*==============================================================================

                               DATA DECLARATIONS

==============================================================================*/

/*
**  Used to manage the array for storing the bitmasks of clients for each clock.
*/
typedef enum video_resources_clk_rgm_e
{
  VIDEO_RESOURCES_CLK_RGM_VIDEO_M,
  VIDEO_RESOURCES_CLK_RGM_VIDEO_VFE_M,
  VIDEO_RESOURCES_CLK_RGM_CAMCLK_M,
  VIDEO_RESOURCES_CLK_RGM_MDDI_CLIENT_IO_M,
  VIDEO_RESOURCES_CLK_RGM_MDDI_CLIENT_VFE_M,
  VIDEO_RESOURCES_CLK_RGM_MDDI_CLIENT_DIV4_M,
  VIDEO_RESOURCES_CLK_RGM_NUM_REGIMES
} video_resources_clk_rgm_e_type;

/*
**  Array for storing the bitmask of the clients using any given clock.
*/
static uint32 video_resources_clock_clients[VIDEO_RESOURCES_CLK_RGM_NUM_REGIMES];

/*
**  Flag used to determine whether or not video_resources_clock_clients has
**  been initialized.
*/
static boolean video_resources_clock_clients_initialized = FALSE;


#ifdef MALLOC_LOGGING
/*
**  Array of buffers which store the last call stack when _clock_on
**  is called.
*/
#define  CALL_STACK_BUFSIZE                24
typedef struct
{
  int32  lens;
  void * buffers[CALL_STACK_BUFSIZE];
} call_stack_t;

static call_stack_t video_resources_call_stack[VIDEO_RESOURCES_CLK_RGM_NUM_REGIMES];
#endif
/*==============================================================================

                          STATIC FUNCTION DEFINITIONS

==============================================================================*/

/*==============================================================================

FUNCTION NAME

  video_resources_valid_client

==============================================================================*/

static boolean video_resources_valid_client
(
  uint32 client
)
{
  switch (client)
  {
    case VIDEO_RESOURCES_CLIENT_DTV:        return TRUE;
    case VIDEO_RESOURCES_CLIENT_JPEGD:      return TRUE;
    case VIDEO_RESOURCES_CLIENT_JPEGE:      return TRUE;
    case VIDEO_RESOURCES_CLIENT_QCAMCORDER: return TRUE;
    case VIDEO_RESOURCES_CLIENT_QCAMERA:    return TRUE;
    case VIDEO_RESOURCES_CLIENT_QTV:        return TRUE;
    case VIDEO_RESOURCES_CLIENT_QVP:        return TRUE;
    default:                                return FALSE;
  }
}


/*==============================================================================

FUNCTION NAME

  video_resources_convert_clk_rgm

==============================================================================*/

static video_resources_clk_rgm_e_type video_resources_convert_clk_rgm
(
  clk_regime_type                      clock
)
{
  switch (clock)
  {
    case CLK_RGM_VIDEO_M:            return VIDEO_RESOURCES_CLK_RGM_VIDEO_M;
    case CLK_RGM_VIDEO_VFE_M:        return VIDEO_RESOURCES_CLK_RGM_VIDEO_VFE_M;
    case CLK_RGM_CAMCLK_M:           return VIDEO_RESOURCES_CLK_RGM_CAMCLK_M;
    case CLK_RGM_MDDI_CLIENT_VFE_M:  return VIDEO_RESOURCES_CLK_RGM_MDDI_CLIENT_VFE_M;
#ifndef T_QSC6055
#ifdef FEATURE_MDDI_CAMERA
    case CLK_RGM_MDDI_CLIENT_DIV4_M: return VIDEO_RESOURCES_CLK_RGM_MDDI_CLIENT_DIV4_M;
    case CLK_RGM_MDDI_CLIENT_IO_M:   return VIDEO_RESOURCES_CLK_RGM_MDDI_CLIENT_IO_M;
#endif
#endif /* T_QSC6055 */
    default:                         return VIDEO_RESOURCES_CLK_RGM_NUM_REGIMES;
  }
}

/*==============================================================================

                              FUNCTION DEFINITIONS

==============================================================================*/

/*==============================================================================

FUNCTION NAME

  video_resources_clock_on

==============================================================================*/

void video_resources_clock_on
(
  uint32                               client,
  clk_regime_type                      clock
)
{
  int32 i;

  video_resources_clk_rgm_e_type local_clock;

  local_clock = video_resources_convert_clk_rgm(clock);

  /*
  ** Initialize the array of clock clients the first time any clock is
  ** turned on.
  */
  if (!video_resources_clock_clients_initialized)
  {
    MSG_MED("Initializing camera resources clock client array", 0, 0, 0);

    /*lint -save -e641 enum to int */
    for (i = 0; i < VIDEO_RESOURCES_CLK_RGM_NUM_REGIMES; i++)
    /*lint -restore */
    {
      video_resources_clock_clients[i] = 0;
    }

    video_resources_clock_clients_initialized = TRUE;
  }

  /*
  ** Verify that the client is a defined client.
  */
  if (!video_resources_valid_client(client))
  {
    MSG_ERROR("Client UNDEFINED (0x%0.8x)", client, 0, 0);
    return;
  }

  /*
  ** Verify that the clock is within the range of defined clocks.
  */
  if (local_clock < VIDEO_RESOURCES_CLK_RGM_NUM_REGIMES)
  {
    /*
    ** Turn on the clock if it is not already turned on.
    */
    if (!video_resources_clock_clients[local_clock])
    {
#ifdef FEATURE_DUAL_CLK_RGM
      clk_regime_msm_enable(clock);
#else
      clk_regime_enable(clock);
#endif
      MSG_MED("Clock ON (%2d)", clock, 0, 0);
    }

    /*
    ** Add the client to the bitmask of active clients for the clock.
    */
    /*lint -save -e737 assign to unsigned long */
    video_resources_clock_clients[local_clock] |= client;
    /*lint -restore */

    MSG_MED("Clock ON (%2d - 0x%0.8x - 0x%0.8x)",
            clock, client, video_resources_clock_clients[local_clock]);
            
#ifdef MALLOC_LOGGING            
    for (i = 0; i < video_resources_call_stack[local_clock].lens; i++)
    {
      video_resources_call_stack[local_clock].buffers[i] = NULL;
    }
    i = MALLOCMGR_UNWIND_STACK(video_resources_call_stack[local_clock].buffers, CALL_STACK_BUFSIZE);
    /* Return is in bytes. Convert to DW. Make sure possitive and in bound */
    i /= 4;
    if (i < 0)
    {
      i *= -1;
    }
    if (i > CALL_STACK_BUFSIZE)
    {
      i = CALL_STACK_BUFSIZE;
    }
    video_resources_call_stack[local_clock].lens = i;
#endif    
  }
  else
  {
    MSG_FATAL("Clock UNDEFINED (%2d - 0x%0.8x)", clock, client, 0);
  }
}


/*==============================================================================

FUNCTION NAME

  video_resources_clock_off

==============================================================================*/

void video_resources_clock_off
(
  uint32                               client,
  clk_regime_type                      clock
)
{
  video_resources_clk_rgm_e_type local_clock;

  local_clock = video_resources_convert_clk_rgm(clock);

  /*
  ** Verify that the client is a defined client.
  */
  if (!video_resources_valid_client(client))
  {
    MSG_ERROR("Client UNDEFINED (0x%0.8x)", client, 0, 0);
    return;
  }

  /*
  ** Verify that the clock is within the range of defined clocks.
  */
  if (local_clock < VIDEO_RESOURCES_CLK_RGM_NUM_REGIMES)
  {
    /*
    ** Remove the client from the bitmask of active clients for the clock.
    */
    /*lint -save -e502 unsigned type */
    video_resources_clock_clients[local_clock] &= ~client;
    /*lint -restore */

    MSG_MED("Clock OFF (%2d - 0x%0.8x - 0x%0.8x)",
            clock, client, video_resources_clock_clients[local_clock]);

    /*
    ** Turn off the clock if no other clients are using the clock.
    */
    if (!video_resources_clock_clients[local_clock])
    {
#ifdef FEATURE_DUAL_CLK_RGM
      clk_regime_msm_disable(clock);
#else
      clk_regime_disable(clock);
#endif
      MSG_MED("Clock OFF (%2d)", clock, 0, 0);
    }
  }
  else
  {
    MSG_ERROR("Clock UNDEFINED (%2d - 0x%0.8x)", clock, client, 0);
  }
}
