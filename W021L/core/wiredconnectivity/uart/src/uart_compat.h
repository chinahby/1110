#ifndef UART_COMPAT_H
#define UART_COMPAT_H     1
/*============================================================================

GENERAL DESCRIPTION

  This module contains code for compatability between different versions
  of the UART driver and the code it integrates with.

EXTERNALIZED FUNCTIONS
    
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 1999-2000, 2007-2008 QUALCOMM Incorporated.  All Rights Reserved.
Qualcomm Confidential and Proprietary
============================================================================*/

/*============================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //depot/asic/msmshared/drivers/uart/02.00.XX/uart_compat.h#2 $

when         user      what, where, why
-----------  --------  -------------------------------------------------------
15-Jan-2009  cmihalik  Initial revision.

============================================================================*/

#include "target.h"
#include "comdef.h"
#include "customer.h"
#include "sio.h"

#include "siors_lib.h"
#include "tramp.h"

//----------------------------------------------------------------------------
//  uart_device_id_type
//----------------------------------------------------------------------------

#define UART_FORCE_SIZE_TO_32_BITS     0x12345678

#ifdef FEATURE_THIRD_UART
  #define UART_NUM_DEVICES    3
#elif defined FEATURE_SECOND_UART
  #define UART_NUM_DEVICES    2
#elif defined FEATURE_FIRST_UART
  #define UART_NUM_DEVICES    1
#else
  #define UART_NUM_DEVICES    0
#endif

typedef enum 
{
  //  UART devices.

  UART_DEVICE_INVALID = -1,
  UART_DEVICE_1 = 0,
  UART_DEVICE_2,
  UART_DEVICE_3,

  //  Aliases used for enumeration.
  //  Enumeration is ( i = FIRST; i < LAST; i++ ), i.e. LAST is not included.

  UART_DEVICE_FIRST = UART_DEVICE_1,
  UART_DEVICE_LAST  = UART_NUM_DEVICES,

  //  Force size. 

  UART_DEVICE_FORCE_SIZE = UART_FORCE_SIZE_TO_32_BITS

} uart_device_id_type;

//----------------------------------------------------------------------------
//  This is the latest version of tramp and the preferred mapping.
//----------------------------------------------------------------------------

#ifndef SIORS_TRAMP_VERSION
#define SIORS_TRAMP_VERSION    TRAMP_VERSION_INVALID
#endif

#if( SIORS_TRAMP_VERSION == TRAMP_VERSION_2_0 )

  #define UART_NULL_IRQ         TRAMP_NULL_IRQ

  #define UART1_IRQ             TRAMP_UART1_IRQ
  #define UART2_IRQ             TRAMP_UART2_IRQ
  #define UART3_IRQ             TRAMP_UART3_IRQ
  #define UART1DM_IRQ           TRAMP_UART1DM_IRQ
  #define UART2DM_IRQ           TRAMP_UART2DM_IRQ
  #define UART1DM_LT_IRQ        TRAMP_UART1DM_IRQ
  #define UART2DM_LT_IRQ        TRAMP_UART2DM_IRQ

  #define UART1_RX_IRQ          TRAMP_UART1_RX_IRQ
  #define UART2_RX_IRQ          TRAMP_UART2_RX_IRQ
  #define UART3_RX_IRQ          TRAMP_UART3_RX_IRQ
  #define UART1DM_RX_IRQ        TRAMP_UART1DM_RX_IRQ
  #define UART2DM_RX_IRQ        TRAMP_UART2DM_RX_IRQ
  #define UART1DM_LT_RX_IRQ     TRAMP_UART1DM_RX_IRQ
  #define UART2DM_LT_RX_IRQ     TRAMP_UART2DM_RX_IRQ

//----------------------------------------------------------------------------
//  1100
//----------------------------------------------------------------------------
//  The legacy UART uses the legacy IRQ.  The DM UART uses the DM IRQs.  
//  What does DM lite use?  On the 1100, there is only 1 UART, DM lite.  It 
//  uses the legacy IRQs.  Is this an error, or does it matter?
//  Shouldn't it be using the DM IRQs?
//----------------------------------------------------------------------------

#elif( SIORS_TRAMP_VERSION == TRAMP_VERSION_1100 )

  #define UART_NULL_IRQ         TRAMP_UN_SUPPORTED_ISR

  #define UART1_IRQ             TRAMP_UART1_ISR
  #define UART2_IRQ             TRAMP_UART2_ISR
  #define UART3_IRQ             UART_NULL_IRQ
  #define UART1DM_IRQ           TRAMP_DM_UART_ISR
  #define UART2DM_IRQ           UART_NULL_IRQ
  #define UART1DM_LT_IRQ        TRAMP_UART1DM_ISR
  #define UART2DM_LT_IRQ        UART_NULL_IRQ

  #define UART1_RX_IRQ          UART_NULL_IRQ
  #define UART2_RX_IRQ          UART_NULL_IRQ
  #define UART3_RX_IRQ          UART_NULL_IRQ
  #define UART1DM_RX_IRQ        UART_NULL_IRQ
  #define UART2DM_RX_IRQ        UART_NULL_IRQ
  #define UART1DM_LT_RX_IRQ     TRAMP_UART1DM_DP_RX_DATA_ISR
  #define UART2DM_LT_RX_IRQ     UART_NULL_IRQ

//----------------------------------------------------------------------------
//  6055, 6085
//----------------------------------------------------------------------------

#elif( SIORS_TRAMP_VERSION == TRAMP_VERSION_6055 )

  #define UART_NULL_IRQ         TRAMP_UN_SUPPORTED_ISR

  #define UART1_IRQ             TRAMP_UART1_ISR
  #define UART2_IRQ             TRAMP_UART2_ISR
  #define UART3_IRQ             TRAMP_UART3_ISR
  #define UART1DM_IRQ           TRAMP_DM_UART_ISR
  #define UART2DM_IRQ           UART_NULL_IRQ
  #define UART1DM_LT_IRQ        UART_NULL_IRQ
  #define UART2DM_LT_IRQ        UART_NULL_IRQ

  #define UART1_RX_IRQ          TRAMP_DP_RX_DATA_ISR
  #define UART2_RX_IRQ          TRAMP_DP_RX_DATA2_ISR
  #define UART3_RX_IRQ          TRAMP_DP_RX_DATA3_ISR
  #define UART1DM_RX_IRQ        TRAMP_DM_DP_RX_DATA_ISR
  #define UART2DM_RX_IRQ        UART_NULL_IRQ
  #define UART1DM_LT_RX_IRQ     UART_NULL_IRQ
  #define UART2DM_LT_RX_IRQ     UART_NULL_IRQ

//----------------------------------------------------------------------------
//  Compile time error if this is not set up correctly.
//----------------------------------------------------------------------------

#else

  #error Must define SIORS_TRAMP_VERSION in custsio.h to one of the following values: TRAMP_VERSION_2_0; TRAMP_VERSION_1100; TRAMP_VERSION_6055.

#endif

#ifdef IMAGE_QDSP6_PROC
  extern rex_crit_sect_type uart_crit_sect;
  #define UART_INIT_LOCK() rex_init_crit_sect(&uart_crit_sect)
  #define UART_LOCK()   rex_enter_crit_sect(&uart_crit_sect)
  #define UART_UNLOCK() rex_leave_crit_sect(&uart_crit_sect)
  #define UART_LOCK_SAV(sav_var)   rex_enter_crit_sect(&uart_crit_sect)
  #define UART_UNLOCK_SAV(sav_var) rex_leave_crit_sect(&uart_crit_sect)
#else
  #define UART_INIT_LOCK() 
  #define UART_LOCK()   INTLOCK()
  #define UART_UNLOCK() INTFREE()
  #define UART_LOCK_SAV(sav_var)   INTLOCK_SAV(sav_var)
  #define UART_UNLOCK_SAV(sav_var) INTFREE_SAV(sav_var)
#endif

//----------------------------------------------------------------------------
//  FUNCTION 
//    uart_sio_port_id_to_device_id
//
//  DESCRIPTION
//    This function converts the input SIO port id to the corresponding 
//    device id.
//
//  DEPENDENCIES
//    None
//
//  RETURN VALUE
//    device_id if input port_id is valid.
//    UART_DEVICE_INVALID otherwise.
//
//  SIDE EFFECTS
//    None
//----------------------------------------------------------------------------

uart_device_id_type      uart_sio_port_id_to_device_id( sio_port_id_type port_id );

#endif

