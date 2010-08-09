#ifndef AP_ARMPRG_H
#define AP_ARMPRG_H
/*===========================================================================

     D M S S   F L A S H   P R O G R A M M E R   H E A D E R   F I L E

DESCRIPTION
   This file contains common definitions for the DMSS flash programmer
   utility.
   
   Some things are MSM specific and must be ported to a new architecture.

   Copyright (c) 1997-2006 by QUALCOMM Incorporated.  
   All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*============================================================================
                          EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/tools/hostdl/HOSTDL.14.00/common/ap_armprg.h#2 $ $DateTime: 2008/05/20 14:34:57 $ $Author: mmiranda $
  Integrated from P402:  //depot/common-sw/armprg/armprg-9.0/common/ap_armprg.h#3 
  
 when       who     what, where, why
 --------   ---     --------------------------------------------------------
 04/09/08   mm      Added support for programming factory image.
 05/25/06   rt      Get rid of unused MSG macros
 05/11/06   rt      Add support for CEFS image for modem efs partition
 12/05/05   dp      NOR flash driver unification 
 03/20/05   drh     Remove MSG_HIGH define
 12/08/04   drh     Support for multi-image boot
 07/12/04   drh     Support buffered IO for printf.
                    Add enums for new MSM types
 05/31/04   drh     Add enum for MSM6550
 09/24/03   drh     Corrected order of boot_mode_type enum to match ICD
                    Remove obsolete prototypes
 07/23/03   drh     Added some additional error enums
 07/04/03   drh     Added new MSM enums
 03/25/03   drh     Add simple profiling printf macro
 02/27/03   drh     Add selective print macros
 02/25/03   drh     Add enumerated type for boot mode parameter in open
 11/07/02   drh     Moved two public interface prototypes from ap_msm.h
                    to here.
 11/05/02   drh     Added values to enum response_code_type and added several
                    new entries to account for new protocol about to be
                    implemented.
                    Moved prototypes of flash layer functions to ap_flash.h
 10/30/02   drh     Added debug macros
                    Added prototype for flash_device_sectors()
                    Added prototype for do_read(), do_simple_write() and
                    do_stream_write()
 10/23/02   drh     Add prototype for msm_reset, new function that is now
                    moved to hardware specific ap_msm.c
 09/18/02   drh     Added enum for MSM6100
 06/07/02   drh     Added prototype for flash_device_init_data
 04/28/02   drh     Added enums for WCDMA targets
                    Moved extern for target_id to here.
 03/11/02   drh     Removed unused typedef
 02/11/02   drh     Added support for dispatch of transport functions
                    from table of function pointers.
 01/16/02   drh     Initial version of armprg to support the software 
                    downloads over UART on a single MSM target built in
                    the DMSS directory using DMSS header files.  Based on
                    code cleanup effort by lnr 11/2001.

============================================================================*/

//--------------------------------------------------------------------------
// Include Files
//--------------------------------------------------------------------------

#include <string.h>
#include "comdef.h"


//--------------------------------------------------------------------------
// Defines
//--------------------------------------------------------------------------

/* For transport layer */
#define CHECK_FOR_DATA() (*disptbl->check)()
#define DRAIN_TRANSMIT_FIFO() (*disptbl->drain)()
#define TRANSMIT_BYTE(c) (*disptbl->xmit)(c)

/* For printing */

/* When compiled with NOISY, these statements always print */
#if defined(NOISY) && defined(COMPILE_PRINTF)
#error code not present
#else
#define DPRINTF(x) /**/
#endif /* NOISY */

/* When compiled with TNOISY, these statements will print depending
 * on level of TNOISY.  Only changeable at compile time
 */
#if defined(TNOISY) && defined(COMPILE_PRINTF)
#error code not present
#else
#define TPRINTF(level, x) /**/
#endif /* NOISY */

/* These statements will always print no matter what compilation
 * options are not given
 */
#if defined(COMPILE_PRINTF)
#error code not present
#else
#define PRINTF(x) /**/
#define SPRINTF(x) /**/
#endif


/* These macros scattered in strategic places in code are a very crude
 * way of profiling.  Each statement needs its own number or letter
 * and if all instances end up in the log file, all sections of code
 * were executed.
 */
#if defined(NOISY)  && defined(PROFILING)
#error code not present
#else
#define PROF(x) /**/
#endif

#ifdef NOISY
#error code not present
#else
  #define DPRINTF(x) /**/
#endif /* NOISY */


//--------------------------------------------------------------------------
// Type Declarations
//--------------------------------------------------------------------------

/* List to include all the supported interfaces for the software download */
typedef enum
{
  INTERFACE_INVALID,
  INTERFACE_UART,
  INTERFACE_USB
} interface_id_type;



/* List to include all the return types for partition table command */
typedef enum
{
  PARTI_TBL_ACCEPTED,
  PARTI_TBL_DIFFERS,
  PARTI_TBL_BAD_FORMAT,
  PARTI_TBL_ERASE_FAIL,
  PARTI_TBL_UNKNOWN_ERROR,
} parti_tbl_type;


/* Defines for Open Multi command */
#define OPEN_MULTI_SUCCESS             0x00
#define OPEN_MULTI_LENGTH_EXCEEDED     0x01
#define OPEN_MULTI_PAYLOAD_NOT_ALLOWED 0x02
#define OPEN_MULTI_PAYLOAD_REQUIRED    0x03
#define OPEN_MULTI_UNKNOWN_ERROR       0x04

/* List to use for which image to open for */
typedef enum
{
  OPEN_MULTI_MODE_NONE        = 0x00,    /* Not opened yet                    */
  OPEN_MULTI_MODE_PBL         = 0x01,    /* Primary Boot Loader               */
  OPEN_MULTI_MODE_QCSBLHDCFG  = 0x02,    /* QC 2ndary Boot Loader Header and  */
                                   /*      Config Data                  */
  OPEN_MULTI_MODE_QCSBL       = 0x03,    /* QC 2ndary Boot Loader             */
  OPEN_MULTI_MODE_OEMSBL      = 0x04,    /* OEM 2ndary Boot Loader            */
  OPEN_MULTI_MODE_AMSS        = 0x05,    /* AMSS modem executable             */
  OPEN_MULTI_MODE_AMSS_APPS   = 0x06,    /* AMSS apps executable              */
  OPEN_MULTI_MODE_OBL         = 0x07,    /* OBL boot Loader                   */
  OPEN_MULTI_MODE_FOTAUI      = 0x08,    /* FOTA UI Data                      */
  OPEN_MULTI_MODE_CEFS_MODEM  = 0x09,    /* CEFS image for Modem EFS partition*/
} open_multi_mode_type;


/* List to use for which image to open in Open mode. Modes 
 * Bootloader download (0x01), Bootable image download (0x02), 
 * CEFS Image Download (0x03) are not supported in OPEN Mode. 
 */

typedef enum
{
  OPEN_MODE_NONE          = 0x00,    /* Not opened yet                    */
  OPEN_BOOTLOADER_IMAGE   = 0x01,    /* Bootloader Image                  */
  OPEN_BOOTABLE_IMAGE     = 0x02,    /* Bootable Image                    */
  OPEN_CEFS_IMAGE         = 0x03,    /* CEFS Image                        */
  OPEN_MODE_FACTORY_IMAGE = 0x04     /* Factory Image                     */

} open_mode_type;


/* Status Code Enumeration
 * This lists the status result codes passed around in the program.
 * This enum is used to index a table of response packets, so these
 * values map exactly to possible responses. 
 */

typedef enum
{
  ACK = 0x00,                 /* Successful                                  */
  RESERVED_1 = 0x01,          /* Reserved                                    */
  NAK_INVALID_DEST = 0x02,    /* Failure: destination address is invalid.    */
  NAK_INVALID_LEN = 0x03,     /* Failure: operation length is invalid.       */
  NAK_EARLY_END = 0x04,       /* Failure: packet was too short for this cmd. */
  NAK_INVALID_CMD = 0x05,     /* Failure: invalid command                    */
  RESERVED_6 = 0x06,          /* Reserved                                    */
  NAK_FAILED = 0x07,          /* Failure: operation did not succeed.         */
  NAK_WRONG_IID = 0x08,       /* Failure: intelligent ID code was wrong.     */
  NAK_BAD_VPP = 0x09,         /* Failure: programming voltage out of spec    */
  NAK_VERIFY_FAILED = 0x0A,   /* Failure: readback verify did not match      */
  RESERVED_0xB = 0x0B,        /* Reserved */
  NAK_INVALID_SEC_CODE = 0x0C,/* Failure: Incorrect security code            */
  NAK_CANT_POWER_DOWN = 0x0D, /* Failure: Cannot power down phone            */
  NAK_NAND_NOT_SUPP = 0x0E,   /* Failure: Download to NAND not supported     */
  NAK_CMD_OUT_SEQ = 0x0F,     /* Failure: Command out of sequence            */
  NAK_CLOSE_FAILED = 0x10,    /* Failure: Close command failed               */
  NAK_BAD_FEATURE_BITS = 0x11,/* Failure: Incompatible Feature Bits          */
  NAK_NO_SPACE = 0x12,        /* Failure: Out of space                       */
  NAK_INVALID_SEC_MODE = 0x13,/* Failure: Multi-Image invalid security mode  */
  NAK_MIBOOT_NOT_SUPP = 0x14, /* Failure: Multi-Image boot not supported     */
  NAK_PWROFF_NOT_SUPP = 0x15, /* Failure: Power off not supported            */
}
response_code_type;

/*
 *  Structure definition to hold function pointers for dispatching
 */
 typedef struct dispatch {
   boolean (*init)   (void);
   boolean (*active) (void);
   void    (*drain)  (void);
   void    (*check)  (void);
   void    (*xmit)   (byte chr);
 } DISP, *DISPPTR;

//--------------------------------------------------------------------------
// Extern Definitions
//--------------------------------------------------------------------------
/* global pointer to current function dispatch table */
extern DISPPTR disptbl;

#if 0
/* global used to identify target MSM  type */
extern target_id_type target_id;        // target id indicating the MSM number
#endif

#if defined(COMPILE_PRINTF)
#error code not present
#endif



//--------------------------------------------------------------------------
// Function Definitions
//--------------------------------------------------------------------------

/*===========================================================================

FUNCTION packet_loop

DESCRIPTION
  This function is the main loop implementing the DMSS Async Download
  Protocol.  It loops forever, processing packets as they arrive.

DEPENDENCIES
  All necessary initialization for normal CPU operation must have
  been performed, and the UART must have been initialized, before
  entering this function.

RETURN VALUE
  This function does not return.

SIDE EFFECTS
  None.

===========================================================================*/

extern void packet_loop (void);

extern void packet_init (void);


/*===========================================================================

FUNCTION msm_reset

DESCRIPTION
  This function causes the MSM to reset

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void msm_reset (void);


#ifdef USE_BUFFERED_TERMIO
extern void printf_init (void);
extern void term_flush (void);

#endif


#endif /* AP_ARMPRG_H */

