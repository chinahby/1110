#ifndef SIORS232_MDD_DEFS_H    //{
#define SIORS232_MDD_DEFS_H    1  

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                             S I O R S 2 3 2  M D D  D E F S

GENERAL DESCRIPTION


INITIALIZATION AND SEQUENCING REQUIREMENTS

  sio_rs232_dev_init() must be called prior to any of the other functions of
  this module.

Copyright (c) 1999-2000, 2007-2008 QUALCOMM Incorporated.  All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //depot/asic/msmshared/drivers/uart/02.00.XX/siors232_mdd_defs.h#4 $

when         who        what, where, why
-----------  ---------  -----------------------------------------------------
21-Jul-2008  cmihalik   Initial revision.
===========================================================================*/

/*===========================================================================

                              INCLUDE FILES

===========================================================================*/
#include "target.h"                /* Target specific definitions          */
#include "comdef.h"                /* Definitions for byte, word, etc.     */
#include "customer.h"              /* Customer Specific Definitions        */
#include "processor.h"             /* Definition of CPU registers          */
#include "memory.h"                /* Memory copy/set routine prototypes   */
#include "msm.h"                   /* MSM3000 Register Aliases             */

#include "queue.h"                 /* Queue declaration package            */
#include "rex.h"                   /* REX multitasking package             */
#include "sio.h"                   /* Serial I/O service definitions       */

//  If there is is at least one UART.
#if( defined(FEATURE_FIRST_UART)  || \
     defined(FEATURE_SECOND_UART) || \
     defined(FEATURE_THIRD_UART))              //{

#include "err.h"                   /* Error services definitions           */
#include "tramp.h"                 /* Trampoline typedefs and prototypes   */
#include "clk.h"                   /* Clock services definitions           */
#include "dsm.h"                   /* DS Memory Pool (Rawdata only)        */
#include "assert.h"                /* ASSERT macro unitilities             */
#include "siors_lib.h"
#include "siors232_pd_api.h"
#include "siors232_mdd.h"
#include "siors232_trace.h"        /* For the debug trace feature */

#ifdef FEATURE_DS
#ifdef FEATURE_DATA_MM
    #include "dsat.h"
#else
    #include "dsatcop.h"
#endif /* FEATURE_DATA_MM */
#endif

#if (defined(FEATURE_TCXO_SHUTDOWN) || defined(FEATURE_UP_CLOCK_SWITCHING))
#include "sleep.h"
#endif

#ifdef SIORS_USES_DIAGDSM
#include "diagdsm.h"               /* Spinoff of the DSM.h file            */
#endif

#ifdef IMAGE_APPS_PROC
#include "oncrpc.h"
#include "oncrpc_proxy.h"
#include "remote_apis.h"
#endif


/*===========================================================================

                FEATURE FLAG RESOLUTION - Keep the mess here

===========================================================================*/
#if (defined(FEATURE_DS_IS2000) || defined(FEATURE_HDR) || defined(FEATURE_DATA) ) 
#define SIORS_HAS_PACKET_MODE
#endif

#if defined (FEATURE_DS) || defined (FEATURE_DATA)
#define SIORS_HAS_AUTODETECT
#endif

/* Compile time error generation */
#if defined(FEATURE_AUTOBAUD)
#error "AUTOBAUD is no longer supported"
#endif

#if defined(FEATURE_USE_UART1DM) && !defined(FEATURE_UART_TCXO_CLK_FREQ)
#error "UART1DM should be used with TCXO clock frequency"
#endif

/* Remap the quick open variable for easy reading */
#define QUICK TRUE
#define SLOW  FALSE

/* If Sleep is not in use, change the code to NOP */
#ifndef FEATURE_UART_POWERDOWN
#define sleep_register(a, b)      NULL
#endif

  /* UART1 */
#ifdef FEATURE_FIRST_UART
#if (SIORS_FIRST_UART_TYPE == HW_UART)
#define SIORS_UART1_INITIALIZE(a, b, c, d, e)    siors_pd_initialize(a, b, c, d, e)
#endif
#if (SIORS_FIRST_UART_TYPE == HW_UARTDM)
#define SIORS_UART1_INITIALIZE(a, b, c, d, e)    siors_dm_initialize(a, b, c, d, e)
#endif
#if (SIORS_FIRST_UART_TYPE == HW_UARTDM_LT)
#define SIORS_UART1_INITIALIZE(a, b, c, d, e)    siors_dml_initialize(a, b, c, d, e)
#endif
#if (SIORS_FIRST_UART_PD_CABLE_DETECT == TRUE)
#define SIORS_UART1_PD_CHECK_CABLE_STATE
#endif
#endif /* FEATURE_FIRST_UART */

  /* UART2 */
#ifdef FEATURE_SECOND_UART
#if (SIORS_SECOND_UART_TYPE == HW_UART)
#define SIORS_UART2_INITIALIZE(a, b, c, d, e)    siors_pd_initialize(a, b, c, d, e)
#endif
#if (SIORS_SECOND_UART_TYPE == HW_UARTDM)
#define SIORS_UART2_INITIALIZE(a, b, c, d, e)    siors_dm_initialize(a, b, c, d, e)
#endif
#if (SIORS_SECOND_UART_TYPE == HW_UARTDM_LT)
#define SIORS_UART2_INITIALIZE(a, b, c, d, e)    siors_dml_initialize(a, b, c, d, e)
#endif
#if (SIORS_SECOND_UART_PD_CABLE_DETECT == TRUE)
#define SIORS_UART2_PD_CHECK_CABLE_STATE
#endif
#endif /* FEATURE_SECOND_UART */

  /* UART3 */
#ifdef FEATURE_THIRD_UART
#if (SIORS_THIRD_UART_TYPE == HW_UART)
#define SIORS_UART3_INITIALIZE(a, b, c, d, e)    siors_pd_initialize(a, b, c, d, e)
#endif
#if (SIORS_THIRD_UART_TYPE == HW_UARTDM)
#define SIORS_UART3_INITIALIZE(a, b, c, d, e)    siors_dm_initialize(a, b, c, d, e)
#endif
#if (SIORS_THIRD_UART_TYPE == HW_UARTDM_LT)
#define SIORS_UART3_INITIALIZE(a, b, c, d, e)    siors_dml_initialize(a, b, c, d, e)
#endif
#if (SIORS_THIRD_UART_PD_CABLE_DETECT == TRUE)
#define SIORS_UART3_PD_CHECK_CABLE_STATE
#endif
#endif /* FEATURE_THIRD_UART */

/* Map out the sleep control if power control is not in use */
#ifndef FEATURE_UART_POWERDOWN
  #define sleep_assert_okts(a)
  #define sleep_negate_okts(a)
  #define sleep_set_hclk_restriction(a, b)
  #define sleep_set_mclk_restriction(a, b)
#endif

/* Map out MCLK control code if ARM HALT limitation is not in use */
#if (!SIORS_FIRST_UART_NEGATE_ARM_HALT && !SIORS_SECOND_UART_NEGATE_ARM_HALT && \
     !SIORS_THIRD_UART_NEGATE_ARM_HALT)
  #define sleep_set_hclk_restriction(a, b)
  #define sleep_set_mclk_restriction(a, b)
#endif
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*===========================================================================
 General constant and variable definitions.
===========================================================================*/

/* Set a upper bound for number of bytes TX_ISR can transit per interrupt. 
   the value is adjust to tx_isr during < 1 ms.  */
#define RS232_TX_UPPER_BOUND        130

/* Guard time that is needed before and after the Escape Characters in order
   for the escape characters to be detected as escape sequence.  This time
   is also an upper bound on the amount of time that can pass between the
   escape characters.  Typically guard-time is 1s, but expire in .9s so that
   if application's timing is off (and/or phone's timing is off) escape 
   detection still works.  Phone's timing should not be off by more than 5ms,
   unless there are some extreme performance problems.                     */
#define  SIO_ESC_GUARD_TIME         900

/* The escape character.                                                   */
#define  SIO_ESC_CHAR                               '+'

/* PPP Flag character value.                                               */
#define SIO_ASYNC_FLAG        0x7E 

/* Mask to transform lower to upper case                                   */
#define TO_UPPERCASE_MASK  0xDF   

/* A default value for a single character to leave the UART.  This time 
 * is actually bitrate dependent.  The idea is to fire a timer for a preset
 * period of time, than check the hardware to see if all the data has left
 * the FIFO, if not, register the timer again for a recheck                */
#define  SIO_DEFAULT_SINGLE_CHAR_TIME              30

#define DISCARD_CHECK_INTERVAL                      20   /* 20 milli-seconds */
#define SIO_PACKET_DETECTION_TIMEOUT                2    /* 2 milli-seconds */

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

/*===========================================================================

                       MACROs

===========================================================================*/

/*===========================================================================

MACRO SIO_RS232_CHECK_PORT_EXIST                         MACRO

DESCRIPTION
  Check to make sure the hardware is available, cause a fatel error 
  if no hardware is defined.  Possible combination is
  * UART1 ONLY
  * UART1 and UART2
  * UART1 and UART3
  * UART1,2,3

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#if defined(FEATURE_SECOND_UART) && defined(FEATURE_THIRD_UART)
#define SIO_RS232_CHECK_PORT_EXIST(port_id)    \
    if((port_id != SIO_PORT_UART_AUX) &&       \
            (port_id != SIO_PORT_UART_MAIN) && \
            (port_id != SIO_PORT_UART_THIRD))  \
      ERR_FATAL( "Invalid Port ID",0,0,0)    
#elif defined(FEATURE_SECOND_UART) 
#define SIO_RS232_CHECK_PORT_EXIST(port_id)    \
    if((port_id != SIO_PORT_UART_AUX) &&       \
            (port_id != SIO_PORT_UART_MAIN))   \
      ERR_FATAL( "Invalid Port ID",0,0,0)    
#elif defined(FEATURE_THIRD_UART)
#define SIO_RS232_CHECK_PORT_EXIST(port_id)    \
    if((port_id != SIO_PORT_UART_THIRD) &&     \
            (port_id != SIO_PORT_UART_MAIN))   \
      ERR_FATAL( "Invalid Port ID",0,0,0)    
#else                                        
#define SIO_RS232_CHECK_PORT_EXIST(port_id)    \
    if((port_id != SIO_PORT_UART_MAIN))        \
      ERR_FATAL( "Invalid Port ID",0,0,0)    
#endif

#endif //}

#endif //}

