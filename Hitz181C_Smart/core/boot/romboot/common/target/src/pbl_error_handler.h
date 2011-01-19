#ifndef PBL_ERROR_HANDLER_H
#define PBL_ERROR_HANDLER_H

/*===========================================================================

                    P r i m a r y  B o o t   L o a d e r
                         E r r o r  H a n d l e r
                           H e a d e r  F i l e

DESCRIPTION
  This header file contains declarations and definitions for the PBL
  error handler.  It is called in the event a PBL failure is detected
  and configures the MSM for download of software through the JTAG
  interface.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2006, 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/romboot/shared/pbl_error_handler.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/25/07   PS      Initial ULC port.
06/01/06   AMW     Close overwrite stack or vector table security hole.
05/02/06   AMW     Initial SC2X port.
07/12/05   SJD     Incorporated code review comments.
08/27/04   SJD     Initial revision.

============================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

#define PBL_ERROR_MAX_LOG        10  /* Max error logs to log   */

/* Error code definitions for reporting errors in PBL */
typedef enum
{
  PBL_NO_ERR= 0,                        /* 000 output at low ("0")  */
  PBL_FLASH_DET_OR_RW_ERR,              /* 001 output at hclk div 2 */
  PBL_FLASH_TIMEOUT_ERR,                /* 010 output at hclk div 3 */
  PBL_ASSERT_ERR,                       /* 011 output at hclk div 4 */
  PBL_LOAD_DBL_ERR,                     /* 100 output at hclk div 5 */
  PBL_CERT_CHAIN_VERIFY_ERR,            /* 101 output at hclk div 6 */
  PBL_SIGNATURE_VERIFY_ERR,             /* 110 output at hclk div 7 */
  PBL_MISC_OR_EXCEPTION_ABORT_ERR,      /* output at high ("1")     */
  PBL_MAX_ERR_CODE = 0x7FFFFFFF             /* To ensure it's 32 bits wide */
} pbl_err_code_enum_type;


/* Error log structure to store constant data describing error location */
typedef struct
{
  uint32 line_num;
  char *filename;   /* File name pointer is stored in error logs 
                       which points to the const srting in ROM */
  pbl_err_code_enum_type err_code;
}pbl_error_log_location_type;

/* Error log structure that will store error data along with potentially useful
 * data that can help debug PBL issues */
typedef struct
{
  pbl_error_log_location_type location; /* data giving location of error */
  uint32 d1, d2, d3;                    /* Up to 3 uint32 storing values
                                           at time of error              */
}pbl_error_log_type;

/* Valid addr range structure that would store valid addresses to load DBL at */
typedef struct 
{
  uint32 start_address;
  uint32 end_address;
}pbl_valid_address_range_type;

/* Typedef of a function pointer that receives PBL errors */
typedef void (*pbl_error_handler_type)
(
  const char* filename_ptr,         /* File this function was called from -
                                       defined by __FILE__ */
  uint32      line,                 /* Line number this function was called
                                       from - defined by  __LINE__ */
  pbl_err_code_enum_type  err_code, /* Error code to indicate which error
                                       by toggling UART_DP_TX_DATA pin */
  uint32  err_log                   /* Error log to help debug */

);

/* Structure that stores information about an alternate PBL error handler */
typedef struct
{
  uint32 magic1;
  uint32 magic2;
  pbl_error_handler_type err_handler_ptr;
} pbl_error_alternate_handler_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

MACRO  PBL_CHECK_VALID_ADDRESS_RANGE

DESCRIPTION
  Check if provided address range lies in an accepted address range. 

DEPENDENCIES
  None

RETURN VALUE
  TRUE : ADDR VALID, lies within an accepted address range
  FALSE: ADDR INVALID, intersects with address outside all accepted ranges

SIDE EFFECTS
  None

===========================================================================*/

#define PBL_CHECK_VALID_ADDRESS_RANGE( start_addr, size )                     \
(                                                                             \
  (                                                                           \
    ((uint32)start_addr >= PBL_IMEM_UNUSED_START_ADDR) &&                     \
    ((uint32)start_addr < PBL_IMEM_UNUSED_END_ADDR)&&                         \
    (((uint32)start_addr + (uint32)size) >= PBL_IMEM_UNUSED_START_ADDR) &&    \
    (((uint32)start_addr + (uint32)size) <= PBL_IMEM_UNUSED_END_ADDR) &&      \
    ((uint32)size <= (PBL_IMEM_UNUSED_END_ADDR - PBL_IMEM_UNUSED_START_ADDR)) \
  ) ||                                                                        \
  (                                                                           \
    ((uint32)start_addr < EBI1_CS1_BASE) &&                                   \
    (((uint32)start_addr + (uint32)size) <= EBI1_CS1_BASE) &&                 \
    ((uint32)size <= (EBI1_CS1_BASE - EBI1_CS0_BASE))                         \
  )                                                                           \
)

/* Note:  We allow DBL to be loaded from start of UNUSED IMEM to end of 
          UNUSED IMEM and from start of of EBI1 CS0(XIP) to end of EBI1 CS0 
          In case of ULC, EBI1_CS0_BASE is 0x0, so we do not compare load 
          address against it, as any uint32 would be >= zero. Thus we just 
          compare the load address to be less than EBI1_CS1_BASE in this case. 
          The above macro would look different for other chips where EBI1_CS0 
          is not at zero 
 */
/*===========================================================================

MACRO  PBL_VERIFY

DESCRIPTION
  Given a boolean expression, this macro will verify the condition is TRUE
  and do nothing if the condition is TRUE, otherwise it will call the
  PBL error handler.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  This macro never returns if the condition is FALSE.

===========================================================================*/
#define PBL_VERIFY( xx_exp, xx_err_code, xx_err_log )                     \
            do                                                            \
            {                                                             \
              if( !(xx_exp) )                                             \
              {                                                           \
                pbl_error_handler( __FILE__, __LINE__,                    \
                                   (xx_err_code), (uint32)(xx_err_log) ); \
              }                                                           \
            } while( 0 )

/*===========================================================================

MACRO  PBL_ASSERT

DESCRIPTION
  If enabled by not defining PBL_NO_SANITY_CHECK, then:
    Given a boolean expression, this macro will verify the condition is TRUE
    and do nothing if the condition is TRUE, otherwise it will call the PBL
    error handler.
  If disabled using PBL_NO_SANITY_CHECK feature, then:
    Does nothing

  This new assertion macro attempts to separate checks which are absolutely
  necessary compared to checks which help development of the boot rom.
  Keep this on unless absolutely necessary to turn off.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  This macro never returns if the condition is FALSE, and is enabled

===========================================================================*/
#ifndef PBL_NO_SANITY_CHECK
#define PBL_ASSERT( xx_exp )                                        \
  do{                                                               \
    if ( !(xx_exp))                                                 \
    {                                                               \
        pbl_error_handler( __FILE__, __LINE__,                      \
                           PBL_ASSERT_ERR, 0);                      \
    }                                                               \
  }while(FALSE)
#else  /* !PBL_NO_SANITY_CHECK */
  /* Suppress assertions. Assertions are conditions that are expected to be
   * satisfied once the program is finalized.
 */
#define PBL_ASSERT( xx_exp )  
#endif /* PBL_NO_SANITY_CHECK */

/*===========================================================================

MACRO  PBL_ERR_FATAL

DESCRIPTION
  This macro calls the PBL error handler.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  This macro never returns.

===========================================================================*/
#define PBL_ERR_FATAL( xx_err_code, xx_err_log ) \
            PBL_VERIFY( FALSE, xx_err_code, (uint32)xx_err_log )

/*===========================================================================

MACRO  PBL_ADD_ERR

DESCRIPTION
  This macro declares a constant structure saving information about current
  macro location, and adds code to call an error logging function. If
  the error condition occurs, then the logging function will log the
  location of the error, as well as potentially useful for debugging.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Error information is logged into IMEM.

===========================================================================*/
#define PBL_ADD_ERR(xx_err, xx_d1, xx_d2, xx_d3) \
  do{                                                                 \
      static const pbl_error_log_location_type xx_const =             \
         { __LINE__, __FILE__, xx_err};                               \
      (void)pbl_error_add_log(&xx_const,                              \
                              (uint32)(xx_d1),                        \
                              (uint32)(xx_d2),                        \
                              (uint32)(xx_d3));                       \
    }while (FALSE)

/*===========================================================================

FUNCTION  PBL_ERROR_ADD_LOG

DESCRIPTION
  This function allows a software module to log a potentially fatal error, but
  does not force the code to crash as with other error functions. This is
  useful for decoupling control code from modules, and allows control code to
  make the decision of whether error recovery is warranted, or that the failure
  is truly fatal and crashes.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Error is logged to memory

===========================================================================*/
extern boolean pbl_error_add_log
( 
  const pbl_error_log_location_type *log,   /* struct providing error  
                                               location                */
  uint32 d1,                                /* data1 to be saved       */
  uint32 d2,                                /* data2 to be saved       */
  uint32 d3                                 /* data3 to be saved       */
);

/*===========================================================================

FUNCTION  PBL_ERROR_HANDLER

DESCRIPTION
  This function is the error handler for the PBL. It handles errors
  occurring during cold boot and warm boot differently. For cold boot,
  it performs the following to prepare for download of code through the
  JTAG interface at supervisor debug access level:

    1. Flush cache lines and disable instruction and data caches, as well
       as MMU.
    2. Place debug information in IMEM.
    3. Zeroes out memory used for the RW section, ZI section, and stack.
    4. Zeroes out most ARM9 registers.
    5. Enable JTAG interface.
    6. Toggle UART_DP_TX_DATA line.
    7. Loop here forever until a JTAG connection is made or the MSM is reset.

  For errors ocurring during a warm boot, the following is performed:

    1. Flush cache lines and disable instruction and data caches, as well
       as MMU. The caches will not be enabled during a warm boot, so
       this essentially does nothing.
    2. Place debug information in IMEM.
    3. Enable the watchdog timer.
    4. Loop until a JTAG connection is made or the watchdog timer expires,
       forcing a cold boot.

  The warm boot error handling provides some warm boot debugging
  capabilities if the e-fuses are not blown to disable JTAG debug access
  and either the watchdog is disabled by hardware, or a JTAG connection
  can be made before the watchdog expires.

  Note: For security purposes, we are only concerned about concealing the
        hardware key when the JTAG interface is enabled.  This is handled
        by hardware.  The only case where the JTAG interface may be
        enabled without completion of all steps listed above is when this
        function is called to handle an ARM9 exception and a nested
        exception occurs.  A nested exception is terminated immediately
        enabling the JTAG interface.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  This function never returns.

===========================================================================*/
extern void pbl_error_handler
(
  const char* filename_ptr,         /* File this function was called from -
                                       defined by __FILE__ */
  uint32      line,                 /* Line number this function was called
                                       from - defined by  __LINE__ */
  pbl_err_code_enum_type  err_code, /* Error code to indicate which error
                                       by toggling UART_DP_TX_DATA pin */
  uint32  err_log                   /* Error log to help debug */

);

/*===========================================================================

FUNCTION  PBL_SET_ALTERNATE_ERROR_HANDLER

DESCRIPTION
  Sets the alternate PBL error handler to use when calling PBL routines
  from a later stage boot loader.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void pbl_set_alternate_error_handler
( 
  pbl_error_handler_type err_handler_ptr 
);

/* Alternate error handler structure */
extern pbl_error_alternate_handler_type pbl_alt_error_handler;

/* Total number of errors logged */
extern uint32 pbl_total_num_err;

#endif  /* PBL_ERROR_HANDLER_H */
