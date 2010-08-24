/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      M A I N   P R O G R A M

GENERAL DESCRIPTION
  This module is the main (C) program for the DMSS Flash Programmer
  Utility.  This utility is a standalone module intended to be
  downloaded into the RAM of a phone with Flash EPROM program memory.
  The utility conforms to the DMSS Async Download Protocol.
  
  Everything in this file should be MSM independent.

EXTERNALIZED FUNCTIONS
  main_c
    This function is called by the assembly language wrapper code.
    It initializes the UART and calls the packet processor to
    execute the download protocol.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   Call main_c to execute the utility.

Copyright (c) 1997-2004, 2008 Qualcomm Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*============================================================================
                          EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/tools/hostdl/HOSTDL.14.00/common/ap_armprg.c#2 $ $DateTime: 2008/01/28 17:30:17 $ $Author: opham $
  Integrated from P402:  //depot/common-sw/armprg/armprg-9.0/common/ap_armprg.c#2 
  
 when       who     what, where, why
 --------   ---     --------------------------------------------------------
 01/22/08   op      Fix source and destination in memcpy()
 12/08/04   drh     Remove deprecated target_id
 07/12/04   drh     Support buffered IO for printf.
 09/18/03   drh     Move printf() into ap_printf.c
 06/16/03   drh     Add USE_NO_TRANSPORT so that initial bringup can happen
                    without any UART or USB transport at all.
 04/07/03   drh     Removed UDIP_FIXUP as it is obsolete.
                    Rearrange transport init so that both UART and USB cables
                    can be hooked up at once.
 11/05/02   drh     Changed name of flash_device_type to more accurately
                    reflect its function
 10/30/02   drh     Added printf() support and some debug prints.
 04/28/02   drh     Moved call to asb_and_msm_init back to startup.s, but
                    it now has no contents.  Call hw_id_and_init from here
                    and its contents are what used to be asb_and_msm_init.
                    Moved target_id and stacks from ap_msm.c to here.
 04/10/02   drh     Moved call to asb_and_msm_init to here from startup.s
 03/20/02   drh     Moved ID Block into MSM specific file ap_msm.c
 03/11/02   drh     Added code to call the USB transport init and active
                    functions and fill in dispatch table according to
                    which transport (UART/USB) is active
                    Moved all release info into a block of pointers to
                    strings so it will appear together in the binary.
 02/11/02   drh     Added dispatch table pointer for transports.
                    Added code to init dispatch with correct transport table.
 01/16/02   drh     Initial version of armprg to support the software 
                    downloads over UART on a single MSM target built in
                    the DMSS directory using DMSS header files.  Based on
                    code cleanup effort by lnr 11/2001.

============================================================================*/

//--------------------------------------------------------------------------
// Include Files
//--------------------------------------------------------------------------

#include "ap_armprg.h"
#include "ap_msm.h"
#include "ap_startup.h"
#include "ap_flash.h"

//--------------------------------------------------------------------------
// Defines
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
// Global Data
//--------------------------------------------------------------------------


/* Pointer to UART/USB function dispatch table
 */
DISPPTR disptbl;


// System mode stacks
//  These are defined as dword and therefore use 1/4 the number
//  of stack elements, hence the divide by 4.  The defines for
//  the size are left the same in the header file.
//  Defining them as dword prevents unaligned access
//
dword svc_stack[SVC_Stack_Size/4];     // Flashprg Stack
dword irq_stack[IRQ_Stack_Size/4];
dword abort_stack[Abort_Stack_Size/4];
dword undef_stack[Undef_Stack_Size/4];

//--------------------------------------------------------------------------
// Extern Definitions
//--------------------------------------------------------------------------

/*
 * Dispatch tables for transports.
 */
extern DISP uart_dispatch_table;
extern DISP usb_dispatch_table;

/* hardware identification and init function */
extern void hw_id_and_init (void);

/* Need reference to it or it will not show up in the binary */
extern const char *id_tbl[];

//--------------------------------------------------------------------------
// Function Prototypes
//--------------------------------------------------------------------------

#ifndef USE_NO_TRANSPORT
void init_interface_to_host (void);
#ifdef DEBUG_LOOPBACK_UART
#error code not present
#endif
#endif
//--------------------------------------------------------------------------
// Function Declarations
//--------------------------------------------------------------------------


/*===========================================================================

FUNCTION main_c

DESCRIPTION
  This function is the main (C) routine for the DMSS Flash Downloader
  Utility.  It initializes the UART and calls the packet processor to
  execute the protocol.

  This function is called main_c rather than main because the name
  main is special to the compiler.  It tries to bring in the C runtime
  library.

DEPENDENCIES
  All necessary initialization for normal CPU operation must have
  been performed before entering this module.

RETURN VALUE
  None.  This function does not return.

SIDE EFFECTS
  None.

===========================================================================*/

void
main_c (void)
{
  /* 
   * Create variable and have it reference the global release info.
   * This way space for the info is created in the memory and we
   * can access the value of it from the HEX file.
   * This local strings should be short, as we only need the reference,
   * it does not matter that we copy the whole string
   */
  volatile char armprg_id_blk[] = "DUMMY";


#if defined(USE_BUFFERED_TERMIO) && defined(COMPILE_PRINTF)
#error code not present
#endif
  
  DPRINTF(("main_c: enter\n"));
  memcpy((void*) armprg_id_blk, (void*) id_tbl[0], sizeof(armprg_id_blk));

  /* Determine target ID type and set up accordingly.
   * This call must be done after call to memory_init which
   * happens before main_c is called.  */
  DPRINTF(("main_c: b4 hw_id_and_init\n"));
  hw_id_and_init();

  /*  Do any one time only init of data used to access flash devices.
   *  This init is on data that is independent of the type of flash
   *  eventually found during the probe sequence.
   */
  DPRINTF(("main_c: b4 flash_init_data\n"));
  flash_device_init_data ();

  KICK_WATCHDOG ();

#ifndef USE_NO_TRANSPORT
  /* initializes the UART interface and identifies the interface used by 
   * the host to perform the software download in to the MSM (UART/USB)
   */
  DPRINTF(("main_c: b4 init_interface_to_host\n"));
  init_interface_to_host ();
#endif

  packet_init ();               // Initialize the packet module.

  /* Find a valid flash device */
  DPRINTF(("main_c: before flash_device_probe\n"));
  flash_device_probe ();         /*lint !e534 */

#ifndef USE_NO_TRANSPORT

#ifdef DEBUG_LOOPBACK_UART
#error code not present
#else
  /*  Stuff a packet and handle commands - never returns */
  DPRINTF(("main_c: b4 packet loop\n"));
  packet_loop ();
#endif /* DEBUG_LOOPBACK_UART */
#else
  DPRINTF(("main_c: looping forever kicking dog\n"));
  while(1)
  {
    KICK_WATCHDOG();
    mdelay(0xFFFF);
   
  }
#endif /* USE_NO_TRANSPORT */
}                               /* main_c() */


#ifndef USE_NO_TRANSPORT
/*===========================================================================

FUNCTION init_interface_to_host

DESCRIPTION
  This function does the required hardware initialization and identifies the
  interface used by the host to download the software into the MSM. It uses
  the functions provided by the UART/USB modules to do all the 
  necessary checks on the UART/USB status registers.

  The dispatch table pointer for the proper transport interface (UART/USB)
  is set up.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void
init_interface_to_host (void)
{
#if !defined(DEBUG_LOOPBACK_UART) && !defined(USE_UART_ONLY)
  boolean usb_running = FALSE;

  /* 
   * Call USB init function which may return TRUE or FALSE.  If USB is
   * running, it is because DLOAD used it to download this binary to the
   * target, just use it, do not even call any UART functions. 
   */
  usb_running = (usb_dispatch_table.init) ();
  if (usb_running == TRUE)
  {
    /* Wait here until USB becomes active with characters */
    while ((usb_dispatch_table.active) () == FALSE);
    disptbl = &usb_dispatch_table;
    return;
  }
#endif

#ifndef USE_USB_ONLY
  /* 
   * If we get here, either there is no USB interface, or it is not in
   * use.  Call UART init function.  No need to check return code as it 
   * is always TRUE.  Transport will always be UART if we get here.
   */
  (void)(uart_dispatch_table.init) ();
  disptbl = &uart_dispatch_table;

#endif /* !USE_USB_ONLY */

  return;
}
#endif /* USE_NO_TRANSPORT */

